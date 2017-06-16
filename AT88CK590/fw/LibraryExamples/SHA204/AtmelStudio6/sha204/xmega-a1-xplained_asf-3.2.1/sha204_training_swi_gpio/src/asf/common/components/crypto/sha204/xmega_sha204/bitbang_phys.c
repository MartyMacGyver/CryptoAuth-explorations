/*
 * \file
 *
 * \brief XMEGA-A1 Xplained timer utility functions.
 *
 *
 * Copyright (c) 2011-2012 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#include <asf.h>             // all ASF definitions
#include "swi_phys.h"        // hardware dependent declarations for SWI
#include "bitbang_config.h"  // non-portable macro definitions
#include "conf_atsha204.h"

#define SHA204_PIN_CONFIG    IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH | IOPORT_MODE_WIREDAND

//! declaration of the variable indicating which pin the selected device is connected to
static uint8_t device_pin = PIN_SWI_CLIENT;

void delay_pulses(uint32_t pulse_count);
void delay_rx_tx(void);


/** \brief This GPIO function sets the signal pin.
 *         Communication functions will use this signal pin.
 *
 *  \param[in] id client if zero, otherwise host
 *  \return status of the operation
 ****************************************************************
 */
void swi_set_device_id(uint8_t id) {
	device_pin = PIN_SWI_CLIENT;
}


/** \brief 	This GPIO function sets the bit position of the
 *          signal pin to its default.
 */
void swi_enable(void)
{
	device_pin = PIN_SWI_CLIENT;
 	ioport_configure_pin(device_pin, SHA204_PIN_CONFIG);
	
#ifdef DEBUG_BITBANG
  	ioport_configure_pin(DEBUG_PIN_SWI, SHA204_PIN_CONFIG);
	DEBUG_LOW;
#endif	
}


/** \brief 	This GPIO function resets the bit position of the
 *          signal pin to input.
 */
void swi_disable(void)
{
	device_pin = PIN_SWI_CLIENT;
 	ioport_configure_pin(device_pin, IOPORT_DIR_INPUT);
	
#ifdef DEBUG_BITBANG
  	ioport_configure_pin(DEBUG_PIN_SWI, IOPORT_DIR_INPUT);
	DEBUG_LOW;
#endif	
}


/** \brief This GPIO function sets the signal pin low or high.
 * \param[in] is_high 0: set signal low, otherwise high.
 */
void swi_set_signal_pin(uint8_t is_high)
{
	ioport_configure_pin(device_pin, IOPORT_DIR_OUTPUT | IOPORT_MODE_WIREDAND | (is_high ? IOPORT_INIT_HIGH : IOPORT_INIT_LOW));
}


/** \brief This GPIO function sends bytes to an SWI device.
 * \param[in] count number of bytes to send
 * \param[in] buffer pointer to tx buffer
 * \return status of the operation
 */
uint8_t swi_send_bytes(uint8_t count, uint8_t *buffer)
{
   volatile uint8_t delay;
   uint8_t i, bit_mask;

	// Disable interrupts while sending.
	swi_disable_interrupts();

	// Set signal pin as output.
	ioport_configure_pin(device_pin, SHA204_PIN_CONFIG);

	// Wait turn around time.
	RX_TX_DELAY;

	for (i = 0; i < count; i++) {
		for (bit_mask = 1; bit_mask > 0; bit_mask <<= 1) {
			if (bit_mask & buffer[i]) {
				gpio_toggle_pin(device_pin); // high to low
				BIT_DELAY_1;
				gpio_toggle_pin(device_pin); // low to high
				BIT_DELAY_8;
			}
			else {
				// Send a zero bit.
				gpio_toggle_pin(device_pin); // high to low
				BIT_DELAY_1;
				gpio_toggle_pin(device_pin); // low to high
				BIT_DELAY_1;
				gpio_toggle_pin(device_pin); // high to low
				BIT_DELAY_1;
				gpio_toggle_pin(device_pin); // low to high
				BIT_DELAY_6;
			}
		}
	}
	swi_enable_interrupts();
	return SWI_FUNCTION_RETCODE_SUCCESS;
}


/** \brief This GPIO function sends one byte to an SWI device.
 * \param[in] value byte to send
 * \return status of the operation
 */
uint8_t swi_send_byte(uint8_t value)
{
	return swi_send_bytes(1, &value);
}


/** \brief This GPIO function receives bytes from an SWI device.
 *  \param[in] count number of bytes to receive
 *  \param[out] buffer pointer to rx buffer
 * \return status of the operation
 */
uint8_t swi_receive_bytes(uint8_t count, uint8_t *buffer) {
	uint8_t status = SWI_FUNCTION_RETCODE_SUCCESS;
	uint8_t i;
	uint8_t bit_mask;
	uint8_t pulse_count;
	uint8_t timeout_count;

	// Disable interrupts while receiving.
	swi_disable_interrupts();

	// Configure signal pin as input.
	ioport_configure_pin(device_pin, IOPORT_DIR_INPUT | IOPORT_MODE_WIREDAND);

#ifndef DEBUG_BITBANG

	// Receive bits and store in buffer.
	for (i = 0; i < count; i++) {
		for (bit_mask = 1; bit_mask > 0; bit_mask <<= 1) {
			pulse_count = 0;

			// Make sure that the variable below is big enough.
			// Change it to uint16_t if 255 is too small, but be aware that
			// the loop resolution decreases on an 8-bit controller in that case.
			timeout_count = START_PULSE_TIME_OUT;

			// Detect start bit.
			while (--timeout_count > 0) {
				// Wait for falling edge.
				if (ioport_pin_is_high(device_pin) == false)
					break;
			}

			if (timeout_count == 0) {
				status = SWI_FUNCTION_RETCODE_TIMEOUT;
				break;
			}

			do {
				// Wait for rising edge.
				if (ioport_pin_is_high(device_pin) == true) {
					// For an Atmel microcontroller this might be faster than "pulse_count++".
					pulse_count = 1;
					break;
				}
			} while (--timeout_count > 0);

			if (pulse_count == 0) {
				status = SWI_FUNCTION_RETCODE_TIMEOUT;
				break;
			}

			// Trying to measure the time of start bit and calculating the timeout
			// for zero bit detection is not accurate enough for an 8 MHz 8-bit CPU.
			// So let's just wait the maximum time for the falling edge of a zero bit
			// to arrive after we have detected the rising edge of the start bit.
			timeout_count = ZERO_PULSE_TIME_OUT;

			// Detect possible edge indicating zero bit.
			do {
	            if (ioport_pin_is_high(device_pin) == false) {
					// For an Atmel microcontroller this might be faster than "pulse_count++".
					pulse_count = 2;
					break;
				}
			} while (--timeout_count > 0);

			// Wait for rising edge of zero pulse before returning. Otherwise we might interpret
			// its rising edge as the next start pulse.
			if (pulse_count == 2) {
				timeout_count = ZERO_PULSE_TIME_OUT;
				do {
				   if (ioport_pin_is_high(device_pin) == true)
						break;
				} while (timeout_count-- > 0);

				// This check is taken out, because it makes the bit
				// sampling loop too slow on an AT90USB1287 running at 16 MHz.
//				if (time_out_count == 0) {
//					status = SWI_FUNCTION_RETCODE_TIMEOUT;
//					break;
//				}
			}

			// Update byte at current buffer index.
			else
				// received "one" bit
				buffer[i] |= bit_mask;
		}

		if (status != SWI_FUNCTION_RETCODE_SUCCESS)
			break;
	}
	swi_enable_interrupts();

	if (status == SWI_FUNCTION_RETCODE_TIMEOUT) {
		if (i > 0)
			// Indicate that we timed out after having received at least one byte.
			status = SWI_FUNCTION_RETCODE_RX_FAIL;
	}
	return status;

#else

	DEBUG_LOW;

#ifdef   DEBUG_BITBANG_MEASURE
	// Use this variable to measure the number of loop counts per pulse
	// to establish a value for the zero bit detection timeout.
	// For the AT88CK109STK3 (AT90USB1287 at 16 MHz) it is 8 for the start pulse.
	volatile uint8_t start_pulse_width = 0;
#endif

	for (i = 0; i < count; i++) {
		for (bit_mask = 1; bit_mask > 0; bit_mask <<= 1) {
			pulse_count = 0;

			// Make sure that the variable below is big enough.
			// Change it to uint16_t if 255 is too small, but be aware that
			// the loop resolution decreases on an 8-bit controller in that case.
			timeout_count = START_PULSE_TIME_OUT;

#ifdef   DEBUG_BITBANG_MEASURE
			// Use this variable to measure the number of loop counts per pulse
			// to establish a value for the zero bit detection timeout.
			// For the AT88CK109STK3 (AT90USB1287 at 16 MHz) it was 8 for the start pulse.
			start_pulse_width = 0;
#endif

			// Detect start bit.
			DEBUG_HIGH;
			while (--timeout_count > 0) {
				// Wait for falling edge.
				if (ioport_pin_is_high(device_pin) == false)
					break;
			}
			DEBUG_LOW;

			if (timeout_count == 0) {
				// Allows to put a break point.
				//asm volatile("nop"::);
				status = SWI_FUNCTION_RETCODE_TIMEOUT;
				break;
			}
			DEBUG_HIGH;

			do {
				// Wait for rising edge.
				if (ioport_pin_is_high(device_pin) == true) {
					// For an Atmel microcontroller this might be faster than "edgeCount++".
					pulse_count = 1;
					break;
				}
#ifdef   DEBUG_BITBANG_MEASURE
				start_pulse_width++;
#endif
			} while (--timeout_count > 0);
			DEBUG_LOW;

#ifdef   DEBUG_BITBANG_MEASURE
			if (pulse_count == 0 || start_pulse_width == 0)
#else
			if (pulse_count == 0)
#endif
			{
				// Allows to put a break point.
				//asm volatile("nop"::);
				status = SWI_FUNCTION_RETCODE_TIMEOUT;
				break;
			}

			// Trying to measure the time of start bit and calculating the timeout
			// for zero bit detection is not accurate enough for an 8 MHz 8-bit CPU.
			// So let's just wait the maximum time for the falling edge of a zero bit
			// to arrive after we have detected the rising edge of the start bit.
			timeout_count = ZERO_PULSE_TIME_OUT;

			// Detect possible edge indicating zero bit.
			DEBUG_HIGH;
			do {
				if (ioport_pin_is_high(device_pin) == false) {
					// For an Atmel microcontroller this might be faster than "edgeCount++".
					pulse_count = 2;
					break;
				}
			} while (--timeout_count > 0);
			DEBUG_LOW;

			// Wait for rising edge of zero pulse before returning. Otherwise we might interpret
			// its rising edge as the next start pulse.
			if (pulse_count == 2) {
				DEBUG_HIGH;
				do {
					if (ioport_pin_is_high(device_pin) == true)
						break;
				} while (timeout_count-- > 0);
			}

			// Update byte at current buffer index.
			else
				// received "one" bit
				buffer[i] |= bit_mask;

			DEBUG_LOW;
		}
	}
	swi_enable_interrupts();

	return status;

#endif	// DEBUG_BITBANG
}
