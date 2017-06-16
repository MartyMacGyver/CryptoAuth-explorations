/**
* \file
*
* Copyright (c) 2010-2012 Atmel Corporation. All rights reserved.
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
 *  \brief  Application Example that Demonstrates Writing and Reading in the Clear
 *  \author Atmel Crypto Products
 *  \date   October 2, 2012
*/

#include <string.h>                              // memset()
#include <asf.h>                                 // definitions and declarations for the Atmel Software Framework
#include "sha204_example_led.h"                  // definitions of LED utility functions
#include "sha204_example_gfx.h"                  // definitions of LCD utility functions
#include "sha204_examples.h"                     // definitions and declarations for example functions


/** \brief This function serves as an example for writing and reading in the clear.
 * \return status of the operation
 */
uint8_t sha204e_write_read_clear(void)
{
	uint8_t ret_code;
	// We could re-use rx_buffer for the status response, but we use a separate
	// buffer for easier debugging.
	static uint8_t status_response[SHA204_RSP_SIZE_MIN];
	static uint8_t tx_buffer[WRITE_COUNT_LONG];
	static uint8_t rx_buffer[READ_32_RSP_SIZE];
	static uint8_t *write_data = (uint8_t *) "Slot eight config = open access.";
	static uint8_t saved_data[SHA204_ZONE_ACCESS_32];
	struct sha204_write_parameters args_write;
	struct sha204_read_parameters args_read;

	// Initialize the board.
	board_init();

	// Initialize graphics library.
	gfx_init();

	// Indicate end of hardware initialization.
	led_display_number(0xFF);
	sha204h_delay_ms(1000);
	led_display_number(0x00);
	
	while (true) {
		gfx_display_exercise_title("Clear Read / Write");
		
		// Wake up the device.
		memset(status_response, 0, sizeof(status_response));
		ret_code = sha204c_wakeup(status_response);
		if (ret_code != SHA204_SUCCESS) {
			gfx_display_status("Wakeup", ret_code);
			continue;
		}

#define SHA204_EXAMPLE_READ_SLOT_CONFIG
#ifdef SHA204_EXAMPLE_READ_SLOT_CONFIG
		// Read slot configuration.
		args_read.tx_buffer = tx_buffer;
		args_read.rx_buffer = rx_buffer;
		args_read.zone = SHA204_ZONE_CONFIG | READ_ZONE_MODE_32_BYTES;
		args_read.address = 32; // slot 6 onwards
		ret_code = sha204m_read(&args_read);
		sha204p_sleep();
		if (ret_code != SHA204_SUCCESS) {
			gfx_display_status("Read Config", ret_code);
			continue;
		}
		gfx_display_buffer(SHA204_ZONE_ACCESS_32, 
			&rx_buffer[SHA204_BUFFER_POS_DATA],
			"slot config 6 - 15");
		ret_code = sha204c_wakeup(status_response);
		if (ret_code != SHA204_SUCCESS) {
			gfx_display_status("Wakeup", ret_code);
			continue;
		}
#endif

#define SHA204_EXMPLE_READ_LOCK_CONFIG
#ifdef SHA204_EXMPLE_READ_LOCK_CONFIG
		// Read lock configuration.
		args_read.tx_buffer = tx_buffer;
		args_read.rx_buffer = rx_buffer;
		args_read.zone = SHA204_ZONE_CONFIG;
		args_read.address = 84; // Last two bytes contain the lock values.
		ret_code = sha204m_read(&args_read);
		sha204p_sleep();
		if (ret_code != SHA204_SUCCESS) {
			gfx_display_status("Read Config", ret_code);
			continue;
		}
		uint8_t lock_status_config = rx_buffer[SHA204_BUFFER_POS_DATA + 3];
		gfx_display_lock_status(rx_buffer[SHA204_BUFFER_POS_DATA + 2],
								lock_status_config);
		if (lock_status_config) {
			// Write will only succeed if the configuration zone is locked.
			gfx_display_string(
				" ATSHA204 Clear R/W\n"
				"Locking Config Zone\n"
				"Press SW0 to proceed\n"
				"or SW1 to cancel");
			uint8_t button_status_sw0, button_status_sw1;
			do {
				button_status_sw0 = gpio_pin_is_high(GPIO_PUSH_BUTTON_0);
				button_status_sw1 = gpio_pin_is_high(GPIO_PUSH_BUTTON_1);
			} while (button_status_sw0 && button_status_sw1);
			if (button_status_sw0 == false) {
				// Lock config zone.
				struct sha204_lock_parameters args_lock = 
					{.tx_buffer = tx_buffer, .rx_buffer = rx_buffer, .summary = LOCK_ZONE_NO_CRC};
				
				ret_code = sha204c_wakeup(status_response);
				if (ret_code != SHA204_SUCCESS) {
					gfx_display_status("Wakeup Lock", ret_code);
					continue;
				}
				ret_code = sha204m_lock(&args_lock);
				sha204p_sleep();
				if (ret_code != SHA204_SUCCESS) {
					gfx_display_status("Lock Config", ret_code);
					continue;
				}
			}				
		}
		ret_code = sha204c_wakeup(status_response);
		if (ret_code != SHA204_SUCCESS) {
			gfx_display_status("Wakeup", ret_code);
			continue;
		}
#endif

		// Read data from slot 8.
		memset(rx_buffer, 0, sizeof(rx_buffer));
		args_read.tx_buffer = tx_buffer;
		args_read.rx_buffer = rx_buffer;
		args_read.zone = SHA204_ZONE_DATA | READ_ZONE_MODE_32_BYTES;
		args_read.address = 8 * SHA204_KEY_SIZE; // slot 8
		ret_code = sha204m_read(&args_read);
		if (ret_code != SHA204_SUCCESS) {
			sha204p_sleep();
			gfx_display_status("Read original data", ret_code);
			continue;
		}
		// Save received data. We shall restore them at the end of the command sequence.
		memcpy(saved_data, &args_read.rx_buffer[SHA204_BUFFER_POS_DATA], sizeof(saved_data));
	
		// Write new data.
		args_write.tx_buffer = tx_buffer;
		args_write.rx_buffer = status_response;
		args_write.zone = args_read.zone;
		args_write.address = args_read.address;
		args_write.new_value = write_data;
		args_write.mac = NULL;
		ret_code = sha204m_write(&args_write);
		// Write will fail if configuration zone is not locked.
		if (ret_code != SHA204_SUCCESS) {
			sha204p_sleep();
			gfx_display_status("Write new data", ret_code);
			continue;
		}

		// Read back data from slot and confirm that all bytes were written successfully.
		memset(rx_buffer, 0, sizeof(rx_buffer));
		ret_code = sha204m_read(&args_read);
		if (ret_code != SHA204_SUCCESS) {
			sha204p_sleep();
			gfx_display_status("Read new data", ret_code);
			continue;
		}
		ret_code = (memcmp(write_data, &args_read.rx_buffer[SHA204_BUFFER_POS_DATA], sizeof(write_data))
				? SHA204_FUNC_FAIL : SHA204_SUCCESS);
		if (ret_code != SHA204_SUCCESS) {
			sha204p_sleep();
			gfx_display_status("Verify written data", ret_code);
			continue;
		}
			
		// Restore saved data.
		memcpy(args_write.new_value, saved_data, sizeof(saved_data));
		ret_code = sha204m_write(&args_write);
		if (ret_code != SHA204_SUCCESS) {
			sha204p_sleep();
			gfx_display_status("Write original data", ret_code);
			continue;
		}
	
		sha204p_sleep();

		gfx_display_string(
			" ATSHA204 Clear R/W\n"
			"       Success\n"
			"Press SW0 to see\n"
			"content of slot 8");
		gfx_wait_for_continue();
		gfx_display_buffer(SHA204_KEY_SIZE, saved_data, "content of slot 8");
	}		
	return ret_code;
}
