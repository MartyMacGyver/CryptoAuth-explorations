/*
 * \file
 *
 * \brief ATSHA204 file that implements the I2C layer for the device
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
 *  \brief  ATSHA204 Device SWI Interface Implemented Using a USART
 *  \author Atmel Crypto Products
 *  \date   July 26, 2012
 */
#include <asf.h>
#include "conf_usart_serial.h"
#include "conf_atsha204.h"
#include "sha204_physical.h"            // declarations that are common to all interface implementations
#include "sha204_lib_return_codes.h"    // declarations of function return codes
#include "sha204_timer.h"               // definitions for delay functions

#define UART_TX_GPIO                        IOPORT_CREATE_PIN(PORTD, 3)
#define UART_TX_PIN_CONFIG                  UART_TX_GPIO, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH | IOPORT_MODE_WIREDAND

/** 
 * \brief This enumeration lists all packet types sent to a SHA204 device.
 *
 * The following byte stream is sent to a SHA204 TWI device:
 *    {I2C start} {I2C address} {word address} [{data}] {I2C stop}.
 * Data are only sent after a word address of value #SHA204_I2C_PACKET_FUNCTION_NORMAL.
 */
enum sha204_swi_flag {
	SHA204_SWI_FLAG_COMMAND = 0x77,  //!< A command will follow this flag.
	SHA204_SWI_FLAG_TX = 0x88,       //!< Request device to send. 
	SHA204_SWI_FLAG_IDLE = 0xBB,     //!< Put device into Idle mode.
	SHA204_SWI_FLAG_SLEEP = 0xCC     //!< Put device into Sleep mode.
};


/** \brief This function initializes peripherals (timer and communication).
 */
void sha204p_init(void)
{
	// Initialize timer.
	sha204h_timer_init();

	// Initialize interrupt vectors.
	irq_initialize_vectors();

	// Enable interrupts.
	cpu_irq_enable();
	
	// Enable pull-up on USART pin.
	ioport_configure_pin(UART_TX_PIN_CONFIG);

	usart_serial_options_t usart_options = {
		// Start we half speed since we always start with a Wakeup.
		.baudrate = USART_SERIAL_BAUDRATE / 2,
		.charlength = USART_SERIAL_CHAR_LENGTH,
		.paritytype = USART_SERIAL_PARITY,
		.stopbits = USART_SERIAL_STOP_BIT
	};
	if (usart_serial_init(USART_SERIAL, &usart_options) == false) 
		return;
	
	usart_rx_disable(USART_SERIAL);
}


/** 
 * \brief This function sets the device id.
 *         Communication functions will use this id. Not used in UART implementation.
 *
 *  \param[in] id which device (commonly client or host) to communicate with
 */
void sha204p_set_device_id(uint8_t id)
{
}


/** 
 * \brief This function generates a Wake-up pulse and delays.
 * \return status of the operation
 */
uint8_t sha204p_wakeup(void)
{
	// Creates a 60 us low pulse when sending a 0.
	usart_set_baudrate(USART_SERIAL, USART_SERIAL_BAUDRATE / 2, sysclk_get_per_hz());
	
	status_code_t swi_status = usart_serial_putchar(USART_SERIAL, 0);
	if (swi_status != STATUS_OK)
		return (uint8_t) swi_status;
	
	sha204h_delay_ms(SHA204_WAKEUP_DELAY);
	
	// Set UART speed back to communication speed.
	return (usart_set_baudrate(USART_SERIAL, USART_SERIAL_BAUDRATE, sysclk_get_per_hz()) ? SHA204_SUCCESS : SHA204_FUNC_FAIL);
}


/** 
 * \brief This function converts every bit in the buffer to one byte
 *        and sends it to the device.
 * \param[in] buffer pointer to bytes to send
 * \param[in] count number of bytes to send
 * \return status of the operation
 */
static uint8_t sha204p_send_bytes(uint8_t *buffer, uint8_t count)
{
	uint8_t i, bit_mask;
	status_code_t usart_status = STATUS_OK;
	
	for (i = 0; i < count; i++) {
		for (bit_mask = 1; bit_mask > 0; bit_mask <<= 1) {
			// Create a start pulse only ("zero" bit)
			// or a start pulse and a zero pulse ("one" bit).
			// The zero pulse is placed at bit 2 (lsb first).
			usart_status = usart_serial_putchar(USART_SERIAL, (bit_mask & buffer[i]) ? 0x7F : 0x7D);
			if (usart_status != STATUS_OK)
				return SHA204_COMM_FAIL;
		}
	}
	// Wait for USART transmit buffer empty.
	while (usart_tx_is_complete(USART_SERIAL));

	return (usart_status == STATUS_OK ? SHA204_SUCCESS : SHA204_COMM_FAIL);
}


/** 
 * \brief This function receives bytes, converts eight of them into eight bits
 *        and stores them in the receive buffer.
 * \param[out] buffer receive buffer
 * \param[in] count number of bytes to receive
 * \return status of the operation
 */
static void sha204p_receive_bytes(uint8_t *buffer, uint8_t count)
{
	uint8_t i, bit_mask, bit_data;

	// Somehow the USART has two bytes in its receive buffer after disabling tx
	// and enabling rx. We suspect that the USART is setting the tx-is-empty flag
	// too early so that the receiver sees the last two bytes of the TX flag.
	usart_serial_getchar(USART_SERIAL, &bit_data);
	usart_serial_getchar(USART_SERIAL, &bit_data);
	
	for (i = 0; i < count; i++) {
		buffer[i] = 0;
		for (bit_mask = 1; bit_mask > 0; bit_mask <<= 1) {
			usart_serial_getchar(USART_SERIAL, &bit_data);
			// If the device sends a "one" bit, the bits 1 to 5 in the received byte are set (0x7C).
			// LSB comes first. Reversing 0x7C results in 0x3E.
			if (((bit_data ^ 0x7F) & 0x7C) == 0)
				// Received "one" bit.
				buffer[i] |= bit_mask;
		}
	}
}


/** 
 * \brief This function sends a command to the device.
 * \param[in] count number of bytes to send
 * \param[in] command pointer to command buffer
 * \return status of the operation
 */
uint8_t sha204p_send_command(uint8_t count, uint8_t *command)
{
	uint8_t sha204_flag = SHA204_SWI_FLAG_COMMAND;
	sha204p_send_bytes(&sha204_flag, 1);
	return sha204p_send_bytes(command, count);
}


/** 
 * \brief This function puts the SHA204 device into idle state.
 * \return status of the operation
 */
uint8_t sha204p_idle(void)
{
	uint8_t sha204_flag = SHA204_SWI_FLAG_IDLE;
	return sha204p_send_bytes(&sha204_flag, 1);
}


/** 
 * \brief This function puts the SHA204 device into low-power state.
 * \return status of the operation
 */
uint8_t sha204p_sleep(void)
{
	uint8_t sha204_flag = SHA204_SWI_FLAG_SLEEP;
	return sha204p_send_bytes(&sha204_flag, 1);
}


/** \brief This SWI function is only a dummy since the
 *         functionality does not exist for the SWI
 *         version of the SHA204 device.
 * \return success
 */
uint8_t sha204p_reset_io(void)
{
	return SHA204_SUCCESS;
}

// Add function, possibly inline, that converts received bytes into bits.

/** 
 * \brief This function receives a response from the SHA204 device.
 *
 * \param[in] size size of receive buffer
 * \param[out] response pointer to receive buffer
 * \return status of the operation
 * \todo It seems that two bytes are read in the beginning.
 */
uint8_t sha204p_receive_response(uint8_t size, uint8_t *response)
{
	uint8_t count;
	
	// Send tx flag.
	uint8_t swi_flag = SHA204_SWI_FLAG_TX;
	uint8_t swi_status = sha204p_send_bytes(&swi_flag, 1);
	if (swi_status != STATUS_OK)
		return (swi_status == ERR_TIMEOUT ? SHA204_TIMEOUT : SHA204_RX_NO_RESPONSE);
	
	usart_tx_disable(USART_SERIAL);
	usart_rx_enable(USART_SERIAL);

	sha204p_receive_bytes(response, size);

	usart_rx_disable(USART_SERIAL);
	
	// Disabling usart_tx changes the pin configuration.
	// So we have to re-configure it every time we re-enable tx.
	ioport_configure_pin(UART_TX_PIN_CONFIG);
	usart_tx_enable(USART_SERIAL);

	return SHA204_SUCCESS;
}


/** \brief This function re-synchronizes communication.
 *
  Re-synchronizing communication is done in a maximum of five
  steps listed below. This function implements the first three steps.
  Since steps 4 and 5 (sending a Wake-up token and reading the
  response) are the same for TWI and SWI, they are implemented
  in the communication layer (#sha204c_resync).\n
  If the chip is not busy when the system sends a transmit flag,
  the chip should respond within t_turnaround. If t_exec has not
  already passed, the chip may be busy and the system should poll or
  wait until the maximum tEXEC time has elapsed. If the chip still
  does not respond to a second transmit flag within t_turnaround,
  it may be out of synchronization. At this point the system may
  take the following steps to reestablish communication:
  <ol>
     <li>Wait t_timeout.</li>
     <li>Send the transmit flag.</li>
     <li>
       If the chip responds within t_turnaround,
       then the system may proceed with more commands.
     </li>
     <li>Send a Wake token, wait t_whi, and send the transmit flag.</li>
     <li>
       The chip should respond with a 0x11 return status within
       t_turnaround, after which the system may proceed with more commands.
     </li>
   </ol>

 * \param[in] size size of rx buffer
 * \param[out] response pointer to response buffer
 * \return status of the operation
 */
uint8_t sha204p_resync(uint8_t size, uint8_t *response)
{
	sha204h_delay_ms(85);
	return sha204p_receive_response(size, response);
}
