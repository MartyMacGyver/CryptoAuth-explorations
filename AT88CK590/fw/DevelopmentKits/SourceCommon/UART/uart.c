/** \file
 *  \brief Low-level Uart definitions
 */

#ifdef UART_LOGGING /* DEBUG MODE */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include "uart.h"
//#include "config"
#include "ll_timers.h"
#include "led.h"

///< UBRR0_VAL baud rate setting for 57600kbps @ Fcpu=8.0000MHz, double speed
///< The error is too big (-3.5%) for 115200 baud to work with HyperTerminal.
#define SPEED_MULTIPLIER        (2UL)
#define BAUD_RATE               (57600UL)
#define UBRR0_VAL               ((F_CPU / ((16UL / SPEED_MULTIPLIER) * BAUD_RATE)) - 1UL)

///< Buffer size for uart receive.
#define UART_BUFFER_SIZE     1536 //1024

#define PACKET_INDEX_LEN_HIGH   5
#define PACKET_INDEX_LEN_LOW    6
#define PACKET_INDEX_DATA       7

#define ENABLE_TX_INTERRUPT     UCSR1B |= _BV(TXCIE1)
#define DISABLE_TX_INTERRUPT    UCSR1B &= ~_BV(TXCIE1)

//static unsigned rxBufferIndex = 0;
static volatile uint uiTxBufferIndex = 0;
static volatile uint uiTxStartIndex = 0;
static volatile uint uiTxByteCount = 0;
static volatile uint uiTxBytesFreeCount = 0;
static volatile uchar ucBufferIsEmpty = TRUE;
static char uart_buffer_tx[UART_BUFFER_SIZE];        ///< Uart Circular Tx Buffer


/** \brief Put 1 character.
 *
 *  \param scChar - character to put
 *  \return status
 */
RETURN_CODE uart_PutChar(schar scChar)
{
	uchar ucChar = (uchar) scChar;
  return uart_PutData(&ucChar, 1);
}


/** \brief Puts ucLength data.
 *
 *  \param pucData - pointer to data
 *  \param ucLength - data size
 *  \return status
*/

RETURN_CODE uart_PutData(puchar pucData, uchar ucLength) {
  if (!ucLength || !pucData)
    return FAILED;

	// Get the ball rolling if buffer is empty.
  if (ucBufferIsEmpty) {
    DISABLE_TX_INTERRUPT;
    uiTxStartIndex =  0;
  	uiTxBufferIndex = uiTxByteCount = (uint) ucLength;
    uiTxBytesFreeCount = UART_BUFFER_SIZE - uiTxByteCount;

    ucBufferIsEmpty = FALSE;

    memcpy(uart_buffer_tx, pucData, ucLength);
    UDR1 = *uart_buffer_tx;

    ENABLE_TX_INTERRUPT;

    return SUCCESS;
	}

  // Make sure we enter the loop that checks for free space
  // in circular buffer.
  DISABLE_TX_INTERRUPT;
  volatile uint uiBytesFreeCount = uiTxBytesFreeCount;
  ENABLE_TX_INTERRUPT;

  while (ucLength--) {
    // Wait until circular buffer has at least one byte free.
    while (!uiBytesFreeCount) {
      DISABLE_TX_INTERRUPT;
      uiBytesFreeCount = uiTxBytesFreeCount;
      ENABLE_TX_INTERRUPT;
      //LED3_ON
    }
    // Append byte to buffer.
    DISABLE_TX_INTERRUPT;
    uart_buffer_tx[uiTxBufferIndex++] = *pucData++;
    if (uiTxBufferIndex >= UART_BUFFER_SIZE)
      uiTxBufferIndex = 0;
    uiTxBytesFreeCount--;
    uiBytesFreeCount--;
    uiTxByteCount++;
    ENABLE_TX_INTERRUPT;
  }
  //LED3_OFF

  return SUCCESS;
}


/** \brief Puts string.
 *
 *  \param pscString - pointer to string
 *  \return status
 */
RETURN_CODE uart_PutString(const pschar pscString)
{
  if (!pscString)
    return FAILED;
  
  int len = strlen((const char *) pscString);
  if (!len || len > UART_BUFFER_SIZE)
    return FAILED;
  
  return uart_PutData((puchar) pscString, len);
}


/** \brief Puts constant string.
 *
 *  \param pscData - pointer to string
 *  \return status
 */
RETURN_CODE uart_PutStringProgMem(const char* pscData)
{
  char c;
  RETURN_CODE ucReturn = FAILED;

  while ((c = ROM_READ_BYTE(pscData++))) {
    ucReturn = uart_PutChar(c);
    if (ucReturn)
      break;
  }
  return ucReturn;
}


/** \brief Returns number of bytes sent.
 */
unsigned uart_GetByteCountTx(void) {
  // Save current interrupt enable bit.
  uchar interruptEnabled = UCSR1B & _BV(TXCIE1);
  uint uiByteCount;

  // Disable interrupt.
  DISABLE_TX_INTERRUPT;

  uiByteCount = uiTxByteCount;
  if (interruptEnabled)
    ENABLE_TX_INTERRUPT;

  return uiByteCount;
}


/** \brief Waits until transmit buffer is empty.
 */
void uart_WaitForBufferEmptyTx() {
  // The byte-size variable ucBufferIsEmpty can be accessed atomically.
  // Hence we don't need to disable the tx-ready interrupt before accessing it.
  // With optimization equalling -O1 this variable has to be declared as volatile.
  // Otherwise the AVR compiler produces a branching line PC -= 0 instead of PC -= 2.
  while (!ucBufferIsEmpty);
}


/** \brief Transmit Interrupt Routine for USART1.
*/
ISR(USART1_TX_vect)
{
	if (!(--uiTxByteCount)) {
    // Tx buffer is empty.
    uiTxStartIndex = uiTxBufferIndex = 0;
    uiTxBytesFreeCount = UART_BUFFER_SIZE;
    ucBufferIsEmpty = TRUE;
		return;
  }
  if (++uiTxStartIndex >= UART_BUFFER_SIZE)
    uiTxStartIndex = 0;
  uiTxBytesFreeCount++;

  UDR1 = uart_buffer_tx[uiTxStartIndex];
}



/** \brief Initialize UART parameters, enable receive interrupt, clear
 *  uart receive buffer 
 */
void uart_Init(void)
{
  UBRR1L = (unsigned char) UBRR0_VAL;
  UBRR1H = (UBRR0_VAL >> 8);
  UCSR1A = _BV(U2X1);  // double speed
  UCSR1C = _BV(UCSZ10) | _BV(UCSZ11); // asynchronous, no parity, 8 bits
  uart_Enable(TRUE);
  DISABLE_TX_INTERRUPT;

  ucBufferIsEmpty = TRUE;
  uiTxStartIndex = uiTxBufferIndex = uiTxByteCount = 0;
  uiTxBytesFreeCount = UART_BUFFER_SIZE;
}


/** \brief Disable / enable uart and its tx-ready interrupt 
 *
 *  \param onOff - if 0, turn off uart.
 */
void uart_Enable(uchar onOff)
{
  if (onOff)
    UCSR1B = (_BV(TXCIE1) | _BV(TXEN1));
  else
    UCSR1B &= ~(_BV(TXCIE1) | _BV(TXEN1));
}

#endif
