// ----------------------------------------------------------------------------
//         ATMEL Microcontroller Software Support  -  Colorado Springs, CO -
// ----------------------------------------------------------------------------
// DISCLAIMER:  THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
// DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
// OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// ----------------------------------------------------------------------------
/** \file
 *  \brief Functions of CryptoAuthentication Communication Layer when using UART
 *  \date May 10, 2010
 */

#include <stdint.h>
#include <avr/io.h>

#include "Physical.h"
#include "SHA_Status.h"
#include "AVR_compat.h"

// values for setbaud.h -- must be defined before its inclusion
#define BAUD		230400UL		// baud rate
#define BAUD_TOL	4				// tolerance

// must define BAUD, BAUD_TOL before including
#include <util/setbaud.h>


// signal pins and I/O definitions
#define PORT_DDR		(DDRB)                          //!< direction register for device (wakeup pulse)
#define PORT_OUT		(PORTB)                         //!< output port register for device (wakeup pulse)
#define UART_DDR		(DDRD)							//!< direction register of UART port
#define UART_RX_PIN		(PD2)							//!< UART rx pin
#define UART_TX_PIN		(PD3)							//!< UART tx pin

// client socket
#define SIG_BIT			(7)                             //!< bit position of port register that controls signal wire

/** \brief number of polling iterations over UART register before timing out
 *
 * The polling iteration takes about 1 us.
 * For tx, we would need to wait bit time = 39 us.
 * For rx, we need at least wait for
 * tx / rx turn-around time + bit time = 95 us + 39 us = 134 us.
 * Let's make the timeout larger to be safe.
 */
#define BIT_TIMEOUT		(200)

#ifdef DEBUG_UART
	#define DEBUG_PORT_DDR	(DDRB)                      //!< direction register for debug pin
	#define DEBUG_PORT_OUT  (PORTB)                     //!< output port register for debug pin
	#define DEBUG_BIT		(6)							//!< what pin to use for debugging
	#define DEBUG_LOW       DEBUG_PORT_OUT &= ~_BV(DEBUG_BIT)   //!< set debug pin low
	#define DEBUG_HIGH      DEBUG_PORT_OUT |= _BV(DEBUG_BIT)    //!< set debug pin high
#else
	#define DEBUG_LOW
	#define DEBUG_HIGH
#endif

//! declaration of the variable indicating which pin the selected device is connected to
uint8_t devicePin = _BV(SIG_BIT);


/** \brief This function sets the signal pin. Communication functions will use this signal pin.
 *
 *  \param[in] id not used here, but could be used to select different devices on different pins
 *  \return status of the operation
 */
int8_t SHAP_SetDeviceID(uint8_t id) {
	devicePin = _BV(SIG_BIT);

	return SHA_SUCCESS;
}


/** \brief This function initializes the UART.
 *  \return success
 */
int8_t SHAP_Init(void) {
	if (USE_2X)
		UCSRA = _BV(U2X);           // Improve baud rate error by using 2x clock.

	UBRRL = UBRR_VALUE;

	UCSRC = _BV(UCSZ11);			// 7-bit characters

	// Disable tx interrupt.
	UCSRB &= ~_BV(TXCIE1);

#ifdef DEBUG_UART
    DEBUG_PORT_DDR |= _BV(DEBUG_BIT);
    DEBUG_LOW;
#endif

   return SHA_SUCCESS;
}


/** \brief This function sets the signal low or high.
 * \param[in] high if zero set signal low, otherwise high
 */
void SHAP_SetSignalPin(uint8_t high)
{
	// Disable tx and rx.
	UCSRB &= ~(_BV(TXEN) | _BV(RXEN));

	// On Atmel's Microbase board, PD3 is connected to PB7, and PD2 to PB6 through a 700 Ohm resistor.
	// Configure the UART pins on the UART port as input.
	UART_DDR = ~(UART_RX_PIN | UART_TX_PIN);

	// Toggle the signal pin.
	if (high)
		PORT_OUT |= devicePin;
	else
		PORT_OUT &= ~devicePin;

	PORT_DDR |= devicePin;
}


/** \brief This function sends bytes to the device.
 * \param[in] count number of bytes to send
 * \param[in] buffer pointer to tx buffer
 * \return status of the operation
 */
int8_t SHAP_SendBytes(uint8_t count, uint8_t *buffer)
{
	uint8_t i, bitMask, timeout = BIT_TIMEOUT;

	if (count == 0)
		return SHA_BAD_PARAM;

	PORT_DDR &= ~devicePin;	// Configure the signal pin as input.
	UCSRB &= ~_BV(RXEN);	// Turn off receive.
	UCSRB |= _BV(TXEN);		// Turn on transmit.

	for (i = 0; i < count; i++) {
	    DEBUG_HIGH;
		for (bitMask = 1; bitMask > 0; bitMask <<= 1) {
			loop_until_bit_is_set(UCSRA, UDRE);
			// Create a start pulse only ("zero" bit) or a start pulse and a zero pulse ("one" bit).
			// The "zero" pulse is placed at UDR bit 2 (lsb first).
			UDR = (bitMask & buffer[i]) ? 0x7F : 0x7D;
		}
		DEBUG_LOW;
	}
    // Wait until last byte is sent.
	// Should not take longer than bit time (39 us).
	// The polling loop takes around 1 us.
	while ((timeout-- > 0) && ((UCSRA & _BV(TXC)) == 0));

	// We must manually clear transmit complete flag if not using interrupts
	// by writing a logic one to it!
	UCSRA |= _BV(TXC);

	return (timeout == 0 ? SHA_TIMEOUT : SHA_SUCCESS);
}


/** \brief This function receives bytes from device.
 *  \param[in] count number of bytes to receive
 *  \param[in] buffer pointer to rx buffer
 * \return status of the operation
 */
int8_t SHAP_ReceiveBytes(uint8_t count, uint8_t *buffer) {
	uint8_t i, bitMask, bitData, timeout;

	// Turn off transmit. The transmitter will not turn off until transmit is complete.
	UCSRB &= ~_BV(TXEN);

	// Turn on receive.
	UCSRB |= _BV(RXEN);

	DEBUG_HIGH;

	for (i = 0; i < count; i++) {
		buffer[i] = 0;
		for (bitMask = 1; bitMask > 0; bitMask <<= 1) {
			timeout = BIT_TIMEOUT;
			while (bit_is_clear(UCSRA, RXC)) {
				if (timeout-- == 0)
					return (i == 0 ? SHA_TIMEOUT : SHA_RX_FAIL);
			}
			bitData = UDR;
			// If the device sends a "one" bit, UDR bits 1 to 5 are set (0x7C).
			// LSB comes first. Reversing 0x7C results in 0x3E.
			if (((bitData ^ 0x7F) & 0x7C) == 0)
				// Received "one" bit.
				buffer[i] |= bitMask;
		}
	}
    DEBUG_LOW;

    return SHA_SUCCESS;
}
