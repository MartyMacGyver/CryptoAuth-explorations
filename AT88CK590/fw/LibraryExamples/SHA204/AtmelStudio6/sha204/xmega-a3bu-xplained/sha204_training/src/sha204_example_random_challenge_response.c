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
 *  \brief  Application Example that Demonstrates a Random Challenge
 *          Authentication Scheme.
 *  \author Atmel Crypto Products
 *  \date   October 5, 2012
 */

#include <string.h>                                       // memset(), memcmp()
#include <asf.h>                                          // definitions and declarations for the Atmel Software Framework
#include "sha204_example_led.h"                           // definitions of LED utility functions
#include "sha204_example_gfx.h"                  // definitions of LCD utility functions
#include "sha204_examples.h"                              // definitions and declarations for example functions
#include "sha204_helper.h"                                // definitions and declarations for the Helper module (e.g. TempKey calculation)
#include "sha204_example_random_challenge_response.h"     // definitions and declarations for this module


// When the device is in Idle mode we have to wake it up first before we can put it to sleep.
#define HANDLE_ERROR 			{sha204p_wakeup(); sha204p_sleep(); gfx_display_status("TempKey", ret_code);}


/** \brief This function serves as an example for a random challenge authentication.
 * \return status of the operation
 */
uint8_t sha204e_random_challenge_response(void)
{
	uint8_t ret_code;
	static uint8_t wakeup_response[SHA204_RSP_SIZE_MIN];
	static uint8_t tx_buffer[MAC_COUNT_LONG];
	static uint8_t rx_buffer[MAC_RSP_SIZE];
	const uint8_t *challenge = (uint8_t *) "Here comes the client challenge.";
	const uint8_t key0[] = {
		0x00, 0x00, 0xA1, 0xAC, 0x57, 0xFF, 0x40, 0x4E, 0x45, 0xD4, 0x04, 0x01, 0xBD, 0x0E, 0xD3, 0xC6, 
		0x73, 0xD3, 0xB7, 0xB8, 0x2D, 0x85, 0xD9, 0xF3, 0x13, 0xB5, 0x5E, 0xDA, 0x3D, 0x94, 0x00, 0x00
	};
	static uint8_t mac_calculated[SHA204_KEY_SIZE];
	static struct sha204h_temp_key temp_key;
	struct sha204_nonce_parameters args_nonce = {
		.tx_buffer = tx_buffer, .rx_buffer = rx_buffer, 
		.mode = NONCE_MODE_SEED_UPDATE, .num_in = (uint8_t *) challenge
	};
	struct sha204h_nonce_in_out args_nonce_tempkey = {
		.mode = NONCE_MODE_SEED_UPDATE,	.num_in = (uint8_t *) challenge,
		.rand_out = &rx_buffer[SHA204_BUFFER_POS_DATA], .temp_key = &temp_key
	};
	struct sha204_mac_parameters args_mac = {
		.tx_buffer = tx_buffer, .rx_buffer = rx_buffer,	.mode = MAC_MODE_BLOCK2_TEMPKEY, 
		.key_id = SHA204_KEY_ID_RANDOM_CHALLENGE_RESPONSE, .challenge = NULL
	};
	struct sha204h_mac_in_out args_mac_tempkey = {
		.mode = MAC_MODE_BLOCK2_TEMPKEY, .key_id = SHA204_KEY_ID_RANDOM_CHALLENGE_RESPONSE, .challenge = NULL,
		.key = (uint8_t *) key0, .otp = NULL, .sn = NULL, .response = mac_calculated, .temp_key = &temp_key
	};

	// Initialize system clock.
	sysclk_init();

	// Initialize the board.
	board_init();

	// Initialize graphics library.
	gfx_init();

	// Indicate end of hardware initialization.
	led_display_number(0xFF);
	sha204h_delay_ms(1000);
	led_display_number(0x00);


	// The following command sequence wakes up the device, issues a Nonce
	// and a MAC command using the Command Marshaling layer, and puts the 
	// device to sleep. At the end of the function the MAC response is 
	// compared with the expected one and the result of the comparison is 
	// displayed using the on-board LED's.
	while(true) {
		gfx_display_exercise_title("Random Chall. / Resp.");
		
		// Wake up the device and receive the wakeup response.
		memset(wakeup_response, 0, sizeof(wakeup_response));
		ret_code = sha204c_wakeup(wakeup_response);
		if (ret_code != SHA204_SUCCESS) {
			gfx_display_status("Wakeup", ret_code);
			continue;
		}			
		// Issue a Nonce command.
		memset(rx_buffer, 0, sizeof(rx_buffer));
		ret_code = sha204m_nonce(&args_nonce);
		if (ret_code != SHA204_SUCCESS) {
			sha204p_sleep();
			gfx_display_status("Nonce", ret_code);
			continue;
		}
		// Send Idle command because calculating the TempKey might take
		// longer than the wake timeout of the device. If we would not
		// idle the device before its wake timeout triggers, it would
		// lose its TempKey value.
		sha204p_idle();
		
		// Calculate TempKey.
		ret_code = sha204h_nonce(&args_nonce_tempkey);
		if (ret_code != SHA204_SUCCESS) {
			HANDLE_ERROR;
			continue;
		}
		
		// Calculate MAC.
		ret_code = sha204h_mac(&args_mac_tempkey);
		if (ret_code != SHA204_SUCCESS) {
			HANDLE_ERROR;
			continue;
		}
		
		// Wake up the device to issue a MAC command.
		// Because we rely now on the fact that a good communication
		// is established, we don't bother reading the Wakeup response.
		sha204p_wakeup();
		
		// Issue a Mac command.
		memset(rx_buffer, 0, sizeof(rx_buffer));
		ret_code = sha204m_mac(&args_mac);

		// Put device to sleep.
		sha204p_sleep();

		if (ret_code != SHA204_SUCCESS) {
			gfx_display_status("MAC", ret_code);
			continue;
		}
		
		// Compare the Mac response with the calculated one.
		// Here we can use memcmp since a timing attack is not possible due to the injection of a random nonce.
		ret_code = (memcmp(&rx_buffer[SHA204_BUFFER_POS_DATA], mac_calculated, sizeof(mac_calculated)) ? SHA204_FUNC_FAIL : SHA204_SUCCESS);

		gfx_display_status("Verify Response", ret_code);
		gfx_display_buffer(MAC_CHALLENGE_SIZE, &rx_buffer[SHA204_BUFFER_POS_DATA], "MAC response");
		if (ret_code == SHA204_SUCCESS)
			gfx_display_buffer(MAC_CHALLENGE_SIZE, mac_calculated, "calculated response");
	}
	
	return ret_code;
}
