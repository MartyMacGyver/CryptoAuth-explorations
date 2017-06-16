/** \file
 *  \brief Data converter functions
 */
// TODO: Clean up uart logging. Create for instance functions like
// log_DataAndStatus(uchar rxTx, uchar status, puchar buffer);
// Make logging on-off switchable at run-time.
// Add CMC logging.

#include <stdlib.h>
#include <avr/pgmspace.h>

#include "uart.h"
#include "utility.h"

#ifdef UART_LOGGING /* DEBUG MODE */

/** \brief Converts Nibble to hex-ASCII byte
 *
 *  \param ucData - nibble data to be converted
 *  \return ASCII value
 */
uchar util_ConvertNibbleToAscii(uchar ucData)
{
  ucData &= 0x0F;
  if (ucData <= 0x09)
    ucData += '0';
  else
    ucData = ucData - 10 + 'A';
  return ucData;
}


/** \brief Converts hex-ASCII byte to nibble
 *
 *  \param ucData - ASCII value to be converted
 *  \return nibble value
 */
uchar util_ConvertAsciiToNibble(uchar ucData)
{
  if ((ucData <= '9' ) && (ucData >= '0')) 
    ucData -= '0';
  else if ((ucData <= 'F' ) && (ucData >= 'A')) 
    ucData = ucData -'A' + 10;
  else if ((ucData <= 'f' ) && (ucData >= 'a')) 
    ucData = ucData -'a' + 10;
  else 
    ucData = 0;
  return ucData;
}


/** \brief Put byte converted into two hex-ASCII characters to ringbuffer for transmitting via UART.
 *
 *  \param ucData - a data byte to be transmitted as two hex-ASCII bytes.
 *  \return error status
 */
RETURN_CODE uart_PutCharHex(uchar ucData)
{
  uchar ucTemp = ucData >> 4, i, ucReturn = FAILED;

  for (i = 0; i < 2; i++) {
    ucTemp = util_ConvertNibbleToAscii(ucTemp);
    ucReturn = uart_PutChar(ucTemp);
    if (ucReturn)
      return ucReturn;

    ucTemp = ucData;
  }
  return ucReturn;
}


/** \brief Convert data bytes in buffer to hex-ASCII and put them to ringbuffer
 * for transmitting via UART
 *
 *  \param pucData - the pointer to the data buffer
 *  \param ucLength - the byte count
 *  \return error status
 */
RETURN_CODE uart_PutDataHex(puchar pucData, uchar ucLength)
{
  RETURN_CODE ucReturn = FAILED;

  if (!pucData || !ucLength)
    return ucReturn;

  do {
    ucReturn = uart_PutChar(' ');
    if (ucReturn)
      return ucReturn;

    ucReturn = uart_PutCharHex(*pucData++);
    if (ucReturn)
      return ucReturn;

  } while (--ucLength);

  return uart_PutCrLf();
}


/** \brief Logs Bowline transmit, receive data and status.
 *
 *  \param pucTx - pointer to transmit data
 *  \param ucTxLen - transmit data size
 *  \param pucRx - pointer to receive data
 *  \param ucRxLen - receive data size
 *  \param ucStatus - status
 *  \param ucExpected - expected receive data
 *  \return status
 */
RETURN_CODE uart_LogBowline(puchar pucTx, uchar ucTxLen, puchar pucRx, uchar ucRxLen, uchar ucStatus, uchar ucExpected)
{
  if (!pucTx || !ucTxLen)
    return FAILED;

  uart_PutString((pschar) "Bowline Tx:");
  uart_PutDataHex(pucTx, ucTxLen);
  uart_PutString((pschar) "Status: " );
  if (ucStatus == SUCCESS && pucRx)
  {
    uart_PutCharHex(pucRx[0]);
    uart_PutCrLf();
    if ((pucRx[0] == ucExpected) && (ucRxLen > 1)) {
      uart_PutString((pschar) "Bowline Rx:");
      uart_PutDataHex(pucRx + 1, ucRxLen - 1);
    }
  }
  else {
    uart_PutCharHex(ucStatus);
    uart_PutCrLf();
  }
  return SUCCESS;
}

#endif


