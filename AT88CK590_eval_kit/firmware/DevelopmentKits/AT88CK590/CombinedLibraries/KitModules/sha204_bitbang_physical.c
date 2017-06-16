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
 *  \brief Functions of SHA204 Communication Layer That Are Closest to Hardware (physical layer)
 *  \author Atmel Crypto Products
 *  \date June 6, 2011
 */

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "delay_x.h"
#include "sha204_physical.h"
#include "sha204_lib_return_codes.h"
#include "swi_phys.h"


// ******************************************************************************
// ******************* hardware / system related definitions ********************
// ******************************************************************************

#define NO_DEVICE_ID (0xFF)                          //!< no device selected

//#define DEBUG_BITBANG

// debug pin that indicates pulse edge detection. Only enabled if compilation switch _DEBUG is used.
// To debug timing, disable host power (H1 and H2 on CryptoAuth daughter board) and connect logic analyzer
// or storage oscilloscope to the H2 pin that is closer to the H1 header.
#ifdef DEBUG_BITBANG
	#define DEBUG_PORT_DDR  (DDRB)                              //!< direction register for debug pin
	#define DEBUG_PORT_OUT  (PORTB)                             //!< output port register for debug pin
	#define DEBUG_BIT       (6)                                 //!< what pin to use for debugging
	#define DEBUG_LOW       DEBUG_PORT_OUT &= ~_BV(DEBUG_BIT)   //!< set debug pin low
	#define DEBUG_HIGH      DEBUG_PORT_OUT |= _BV(DEBUG_BIT)    //!< set debug pin high
#else
	#define DEBUG_LOW
	#define DEBUG_HIGH
#endif

// time to drive bits at 230.4 kbps
// Using the value below we measured 4340 ns with logic analyzer (10 ns resolution).
//! time it takes to toggle the pin at CPU clock of 16 MHz (ns)
#define PORT_ACCESS_TIME   (290)
//! width of start pulse (ns)
#define START_PULSE_WIDTH  (4340)
//! delay macro for width of one pulse (start pulse or zero pulse, in ns)
#define BIT_DELAY_1        _delay_ns(START_PULSE_WIDTH - PORT_ACCESS_TIME)
//! time to keep pin high for five pulses plus stop bit (used to bit-bang CryptoAuth 'zero' bit, in ns)
#define BIT_DELAY_5        _delay_ns(6 * START_PULSE_WIDTH - PORT_ACCESS_TIME)
//! time to keep pin high for seven bits plus stop bit (used to bit-bang CryptoAuth 'one' bit)
#define BIT_DELAY_7        _delay_ns(7 * START_PULSE_WIDTH - PORT_ACCESS_TIME)
//! turn around time when switching from receive to transmit
#define RX_TX_DELAY        _delay_us(15)

// One loop iteration for edge detection takes about 0.6 us on this hardware.
// Lets set the timeout value for start pulse detection to the maximum bit time of 86 us.
/** \brief This value is decremented while waiting for the falling edge of a start pulse.
 */
#define START_PULSE_TIME_OUT  (255)

// We measured a loop count of 8 for the start pulse. That means it takes about
// 0.6 us per loop iteration. Maximum time between rising edge of start pulse
// and falling edge of zero pulse is 8.6 us. Therefore, a value of 20 (10 to 12 us)
// gives ample time to detect a zero pulse and also leaves enough time to detect
// the following start pulse.
/** \brief This value is decremented while waiting for the falling edge of a zero pulse.
 */
#define ZERO_PULSE_TIME_OUT     (26)

// ******************************************************************************
// ******************* end of hardware / system related definitions *************
// ******************************************************************************

//! declaration of the variable indicating which pin the selected device is connected to
//static uint8_t devicePin = _BV(SIG2_BIT);

// Depending on the interface switch on the Microbase board, the signal wire (SDA)
// of the device is connected either to PB2 or PB7 (PB2: 8-pin; PB7: 3-pin Javan and Javan Jr.)
// when the switch is set to "SPI", or to PD1 when the switch is set to "TWI" (I2C).
/** \todo Combine uint8_t arrays into arrays of struct.
 * */
#define MB_SWI_PIN_ARRAY_SIZE   4
static uint8_t devicePin[MB_SWI_PIN_ARRAY_SIZE] = {_BV(PB7), _BV(PB6), _BV(PB2), _BV(PD1)};
static volatile uint8_t *devicePortOut[MB_SWI_PIN_ARRAY_SIZE] = {&PORTB, &PORTB, &PORTB, &PORTD};
static volatile uint8_t *deviceDdr[MB_SWI_PIN_ARRAY_SIZE] = {&DDRB, &DDRB, &DDRB, &DDRD};
static volatile uint8_t *devicePortIn[MB_SWI_PIN_ARRAY_SIZE] = {&PINB, &PINB, &PINB, &PIND};
static uint8_t currentPin = _BV(PB7);
static volatile uint8_t *currentPortOut = &PORTB;
static volatile uint8_t *currentPortIn = &PINB;
static volatile uint8_t *currentDdr = &DDRB;
static uint8_t deviceIndex = 0;

uint8_t swi_get_pin_array_size() {return MB_SWI_PIN_ARRAY_SIZE;};


/** \brief This function closes the Single Wire interface.
 *
 *         Since this function is not implemented in the
 *         SHA204 SWI modules, it is implemented here.
 */
uint8_t swi_close_channel(void)
{
	*currentDdr &= ~currentPin;
	*currentPortOut &= ~currentPin; // Disable pull-up resistor.
	return SHA204_SUCCESS;
}


/** \brief This function sets the signal pin.
 * Communication functions will use this signal pin.
 *
 *  \param[in] id index into port arrays
 */
void swi_set_device_id(uint8_t id) {
	if (id >= MB_SWI_PIN_ARRAY_SIZE)
		return;

	deviceIndex = id;

	currentPin = devicePin[deviceIndex];
	currentPortOut = devicePortOut[deviceIndex];
	currentPortIn = devicePortIn[deviceIndex];
	currentDdr = deviceDdr[deviceIndex];
}


/** \brief This function initializes GPIO.
 */
void swi_enable(void) {
#ifdef DEBUG_BITBANG
	// Switch host power off.
	PORTB &= ~_BV(0);
	DDRB |= _BV(0);

	DEBUG_PORT_DDR |= _BV(DEBUG_BIT);
	DEBUG_LOW;
#endif
}


/** \brief This function sets the signal pin low or high.
 * \param[in] high If zero, set signal low, otherwise high.
 */
void swi_set_signal_pin(uint8_t high)
{
	*currentDdr |= currentPin;

	if (high)
		*currentPortOut |= currentPin;
	else
		*currentPortOut &= ~currentPin;

}


/** \brief This function sends bytes to the device.
 * \param[in] count number of bytes to send
 * \param[in] buffer pointer to tx buffer
 * \return status of the operation
 */
uint8_t swi_send_bytes(uint8_t count, uint8_t *buffer) {
	if (count == 0)
		return SHA204_BAD_PARAM;

	uint8_t i, bitMask;

	// Set signal pin as output.
	*currentDdr |= currentPin;

	// Wait turn around time.
	RX_TX_DELAY;

	cli();

	for (i = 0; i < count; i++) {
		for (bitMask = 1; bitMask > 0; bitMask <<= 1) {
			if (bitMask & buffer[i]) {
				*currentPortOut &= ~currentPin;
				BIT_DELAY_1;
				*currentPortOut |= currentPin;
				BIT_DELAY_7;
			}
			else {
				// Send a zero bit.
				*currentPortOut &= ~currentPin;
				BIT_DELAY_1;
				*currentPortOut |= currentPin;
				BIT_DELAY_1;
				*currentPortOut &= ~currentPin;
				BIT_DELAY_1;
				*currentPortOut |= currentPin;
				BIT_DELAY_5;
			}
		}
	}
	sei();

	return SHA204_SUCCESS;
}


/** \brief This GPIO function sends one byte to an SWI device.
 * \param[in] value byte to send
 * \return status of the operation
 */
uint8_t swi_send_byte(uint8_t value)
{
	return swi_send_bytes(1, &value);
}


/** \brief This function receives bytes from device.
 *  \param[in] count number of bytes to receive
 *  \param[in] buffer pointer to rx buffer
 * \return status of the operation
 */
uint8_t swi_receive_bytes(uint8_t count, uint8_t *buffer) {
	uint8_t status = SHA204_SUCCESS;
	uint8_t i;
	uint8_t bitMask;
	uint8_t pulseCount;
	uint8_t timeOutCount;

	// Configure signal pin as input.
	*currentDdr &= ~currentPin;

	cli();

#ifndef DEBUG_BITBANG

	// Receive bits and store in buffer.
	for (i = 0; i < count; i++) {
		buffer[i] = 0;
		for (bitMask = 1; bitMask > 0; bitMask <<= 1) {
			pulseCount = 0;

			// Make sure that the variable below is big enough.
			// Change it to uint16_t if 255 is too small, but be aware that
			// the loop resolution decreases on an 8-bit controller in that case.
			timeOutCount = START_PULSE_TIME_OUT;

			// Detect start bit.
			while (--timeOutCount) {
				// Wait for falling edge.
				if (!(*currentPortIn & currentPin))
					break;
			}

			if (!timeOutCount) {
				status = SHA204_TIMEOUT;
				break;
			}

			do {
				// Wait for rising edge.
				if (*currentPortIn & currentPin) {
					pulseCount = 1;
					break;
				}
			} while (--timeOutCount);

			if (!pulseCount) {
				status = SHA204_TIMEOUT;
				break;
			}

			// Trying to measure the time of start bit and calculating the timeout
			// for zero bit detection is not accurate enough for an 8 MHz 8-bit CPU.
			// So let's just wait the maximum time for the falling edge of a zero bit
			// to arrive after we have detected the rising edge of the start bit.
			timeOutCount = ZERO_PULSE_TIME_OUT;

			// Detect possible edge indicating zero bit.
			do {
				if (!(*currentPortIn & currentPin)) {
					pulseCount = 2;
					break;
				}
			} while (--timeOutCount);

			// Wait for rising edge of zero pulse before returning. Otherwise we might interpret
			// its rising edge as the next start pulse.
			if (pulseCount == 2) {
				do {
					if (*currentPortIn & currentPin)
						break;
				} while (timeOutCount--);
			}

			// Update byte at current buffer index.
			else
				// received "one" bit
				buffer[i] |= bitMask;
		}
		if (status != SHA204_SUCCESS)
			break;
	}

	if (status == SHA204_TIMEOUT) {
		if (i > 0)
			// Indicate that we timed out after having received at least one byte.
			status = SHA204_RX_FAIL;
	}

	sei();

	return status;


#else
	DEBUG_PORT_DDR |= _BV(DEBUG_BIT);
	DEBUG_LOW;

	cli();

	// Receive bits and store in buffer.
	for (i = 0; i < count; i++) {
		buffer[i] = 0;
		for (bitMask = 1; bitMask > 0; bitMask <<= 1) {
			pulseCount = 0;

			// Make sure that the variable below is big enough.
			// Change it to uint16_t if 255 is too small, but be aware that
			// the loop resolution decreases on an 8-bit controller in that case.
			timeOutCount = START_PULSE_TIME_OUT;

			// Detect start bit.
			DEBUG_HIGH;
			while (--timeOutCount > 0) {
				// Wait for falling edge.
				if (!(*currentPortIn & currentPin))
					break;
			}
			DEBUG_LOW;

			if (!timeOutCount) {
				status = SHA204_TIMEOUT;
				break;
			}

			DEBUG_HIGH;
			do {
				// Wait for rising edge.
				if (*currentPortIn & currentPin) {
					pulseCount = 1;
					break;
				}
			} while (--timeOutCount);
			DEBUG_LOW;

			if (!pulseCount) {
				status = SHA204_TIMEOUT;
				break;
			}

			// Trying to measure the time of start bit and calculating the timeout
			// for zero bit detection is not accurate enough for an 8 MHz 8-bit CPU.
			// So let's just wait the maximum time for the falling edge of a zero bit
			// to arrive after we have detected the rising edge of the start bit.
			timeOutCount = ZERO_PULSE_TIME_OUT;

			// Detect possible edge indicating zero bit.
			DEBUG_HIGH;
			do {
				if (!(*currentPortIn & currentPin)) {
					pulseCount = 2;
					break;
				}
			} while (--timeOutCount);
			DEBUG_LOW;

			// Wait for rising edge of zero pulse before returning. Otherwise we might interpret
			// its rising edge as the next start pulse.
			if (pulseCount == 2) {
				DEBUG_HIGH;
				do {
					if (*currentPortIn & currentPin)
						break;
				} while (timeOutCount--);
				DEBUG_LOW;
			}

			// Update byte at current buffer index.
			else
				// received "one" bit
				buffer[i] |= bitMask;
		}
		if (status != SHA204_SUCCESS)
			break;
	}

	if (status == SHA204_TIMEOUT) {
		if (i)
			// Indicate that we timed out after having received at least one byte.
			status = SHA204_RX_FAIL;
	}
	DEBUG_PORT_DDR &= ~_BV(DEBUG_BIT);

	sei();

	return status;

#endif
}
