/*
 * \file
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
 *  \brief  Application Example That Retrieves the Revision From an ATSHA204 Device
 *  \author Atmel Crypto Products
 *  \date   July 16, 2012
*/
#include <string.h>                              // memset()
#include <asf.h>                                 // definitions and declarations for the Atmel Software Framework
#include "sha204_example_led.h"                  // definitions of LED utility functions
#include "sha204_examples.h"                     // definitions and declarations for example functions
#include "sha204_example_device_revision.h"      // definitions and declarations for this module


/** \brief This function retrieves the revision from an ATSHA204 device.
 * \return result (0: success, otherwise failure)
 */
uint8_t sha204e_get_device_revision(void)
{
	static uint8_t tx_buffer[SHA204_CMD_SIZE_MIN];
	static uint8_t rx_buffer[DEVREV_RSP_SIZE];
	static uint8_t wakeup_response[SHA204_RSP_SIZE_MIN];
	struct sha204_dev_rev_parameters dev_rev = {
		.rx_buffer = rx_buffer, .tx_buffer = tx_buffer
	};
	uint8_t sha204_lib_return;
	uint8_t i;
	uint8_t device_present_mask;
	uint8_t sha204_revision;

	// Initialize system clock.
    sysclk_init();

	// Initialize the board.
	board_init();

	// Indicate entering main loop.
	led_display_number(0xFF);
	sha204h_delay_ms(1000);
	led_display_number(0x00);

	// The main loop wakes up a device, retrieves its revision, and puts it
	// back to sleep.
	while (true) {
		device_present_mask = sha204_revision = 0;
		// Generate Wakeup pulse and read Wakeup response.
		sha204_lib_return = sha204c_wakeup(wakeup_response);
	    if (sha204_lib_return != SHA204_SUCCESS) {
			// Indicate Wakeup failure.
			for (i = 0; i < 8; i++) {
				led_display_number(0xFF);
				sha204h_delay_ms(50);
				led_display_number(0x00);
				sha204h_delay_ms(50);
			}
			continue;		 
	    }

        
		// --------------- Small Communication Exercise ---------------------------
		// Read the revision from the device and put it to sleep.
		// Then display the revision (LED's 0 to 3) and the number of devices found
		// (LED's 4 to 7). For this UART exercise we use only one device.
	   
		// Send DevRev command and receive its response.
		memset(rx_buffer, 0, sizeof(rx_buffer));
		sha204_lib_return = sha204m_dev_rev(&dev_rev);

		// Send Sleep flag.
		sha204p_sleep();

		if (sha204_lib_return == SHA204_SUCCESS) {
			// Store result and display revision.
			device_present_mask = 1;
			sha204_revision = rx_buffer[SHA204_BUFFER_POS_DATA + 3];
			display_status(device_present_mask, sha204_revision);
		}
		else
			// Display error.
			display_status_lib(sha204_lib_return);
	}	

	return sha204_lib_return;
}
