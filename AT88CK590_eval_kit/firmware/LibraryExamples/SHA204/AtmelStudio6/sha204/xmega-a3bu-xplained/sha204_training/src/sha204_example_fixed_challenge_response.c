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
 *  \brief  Application Example That Demonstrates a Fixed Challenge / Response 
 *          Authentication Scheme
 *  \author Atmel Crypto Products
 *  \date   October 4, 2012
*/

#include <string.h>                                    // memset()
#include <asf.h>                                       // definitions and declarations for the Atmel Software Framework
#include "sha204_example_led.h"                        // definitions of LED utility functions
#include "sha204_example_gfx.h"                  // definitions of LCD utility functions
#include "sha204_examples.h"                           // definitions and declarations for example functions
#include "sha204_example_fixed_challenge_response.h"   // definitions and declarations for this module


/** \brief This function serves as an example for a fixed challenge / response authentication.
 * \return status of the operation
 */
uint8_t sha204e_fixed_challenge_response(void)
{
	uint8_t ret_code;
	uint8_t i;
	static uint8_t wakeup_response[SHA204_RSP_SIZE_MIN];
	static uint8_t tx_buffer[MAC_COUNT_LONG];
	static uint8_t rx_buffer[MAC_RSP_SIZE];
	static uint8_t *challenge = (uint8_t *) "Here comes the client challenge.";
	struct sha204_nonce_parameters args_nonce = {
		.tx_buffer = tx_buffer, .rx_buffer = rx_buffer, 
		.mode = NONCE_MODE_PASSTHROUGH, .num_in = challenge
	};
	struct sha204_mac_parameters args_mac = {
		.tx_buffer = tx_buffer, .rx_buffer = rx_buffer,
		.mode = MAC_MODE_BLOCK2_TEMPKEY | MAC_MODE_SOURCE_FLAG_MATCH, 
		.key_id = SHA204_KEY_ID_FIXED_CHALLENGE_RESPONSE, .challenge = NULL
	};
	const uint8_t expected_response[] = {
		0x23, // count of Mac response packet = 1(count) + 32(MAC) + 2(CRC)
		0x4e, 0xba, 0x58, 0x80, 0xc2, 0xfd, 0xfc, 0x31, 0x41, 0x90, 0x00, 0x9f, 0xc5, 0x78, 0x3e, 0x90, 
		0x25, 0x4d, 0xbf, 0x69, 0xa3, 0xb2, 0x63, 0xfe, 0x34, 0xd9, 0x2c, 0x3e, 0x6d, 0x98, 0xbd, 0x71, 
		0xe2, 0x15
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
	while (true) {
		gfx_display_exercise_title("Fixed Chall. / Resp.");
		
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
		// Issue a MAC command.
		memset(rx_buffer, 0, sizeof(rx_buffer));
		ret_code = sha204m_mac(&args_mac);

		// Put device to sleep.
		sha204p_sleep();

		if (ret_code != SHA204_SUCCESS) {
			gfx_display_status("MAC Command", ret_code);
			continue;
		}
		
		// Compare the MAC command response with the expected one.
		// To prevent a timing attack we ensure that the comparison loop below
		// takes always the same amount of time:
		// 1. Finish the comparison even if it fails.
		// 2. We assign the comparison result of every byte
		// Before a system is released / deployed it should be tested how secure
		// it is against a timing attack.
		for (i = 0; i < MAC_CHALLENGE_SIZE; i++)
			ret_code |= (rx_buffer[i] == expected_response[i] ? SHA204_SUCCESS : SHA204_FUNC_FAIL);
		//Using memcmp would allow a timing attack.
		//ret_code = (memcmp(rx_buffer, expected_response, sizeof(expected_response)) ? SHA204_FUNC_FAIL : SHA204_SUCCESS);

		gfx_display_status("Verify Response", ret_code);
		gfx_display_buffer(MAC_CHALLENGE_SIZE, &rx_buffer[SHA204_BUFFER_POS_DATA], "MAC response");
		if (ret_code == SHA204_SUCCESS)
			gfx_display_buffer(MAC_CHALLENGE_SIZE, (uint8_t *) &expected_response[SHA204_BUFFER_POS_DATA], "expected response");
	}
	
	return ret_code;
}
