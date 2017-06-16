/*
 * \file
 *
 * \brief ATSHA204 example module
 *
 * This file contains the main function and some helper functions.
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
 *   CAUTION WHEN DEBUGGING: Be aware of the timeout feature of the device. The
 *   device will go to sleep between 0.7 and 1.5 seconds after a Wakeup. When
 *   hitting a break point, this timeout will likely to kick in and the device
 *   has gone to sleep before you continue debugging. Therefore, after you have
 *   examined variables you might have to restart your debug session.
 */
#include <stdint.h>                   // data type definitions
#include <string.h>
#include <asf.h>
#include "sha204_example_led.h"
#include "sha204_example_main.h"
#include "sha204_command_marshaling.h"
#include "sha204_helper.h"
#include "sha204_physical.h"
#include "sha204_lib_return_codes.h"

#define SHA204_DEVICE_CLIENT     1
#define SHA204_DEVICE_HOST       0
#define SHA204_KEY_ID_CHECKMAC   0
#define SHA204_MODE_CHECKMAC     MAC_MODE_BLOCK2_TEMPKEY | MAC_MODE_SOURCE_FLAG_MATCH
#define SHA204_IDLE              true
#define SHA204_SLEEP             false


/** \brief This function wakes up both devices and puts the one
           that is not selected to sleep.
	\param[in] device_id array index into I2C addresses for client or host
	\return status of the operation
*/
static uint8_t sha204e_wakeup(uint8_t device_id, bool idle)
{
	static uint8_t ret_code;               // will be zero for successful responses
	uint8_t wakeup_response[SHA204_RSP_SIZE_MIN];

	sha204p_set_device_id(sha204_i2c_address(device_id));

	// Since both devices share SDA both devices will wake up.
	ret_code = sha204c_wakeup(wakeup_response);
	if (ret_code != SHA204_SUCCESS)
		// Send device to sleep.
		(void) sha204p_sleep();

	// Select other device and put it to sleep.
	sha204p_set_device_id(sha204_i2c_address(device_id == SHA204_DEVICE_CLIENT ? SHA204_DEVICE_HOST : SHA204_DEVICE_CLIENT));
	if (idle && ret_code == SHA204_SUCCESS)
		(void) sha204p_idle();
	else
		(void) sha204p_sleep();

	// Re-select desired device.
	sha204p_set_device_id(sha204_i2c_address(device_id));

	if (ret_code != SHA204_SUCCESS)
		display_status_lib(ret_code);
	return ret_code;
}


/** \brief This function puts both devices to sleep.
           Call this function if one of the devices is in idle mode and the other one returns an error.
	\param[in] device_id array index into I2C addresses for client or host
*/
static void sha204e_sleep(uint8_t device_id)
{
	// Sleep device that is awake.
	(void) sha204p_sleep();
	
	// Wake up devices. One of them is in idle mode.
	(void) sha204p_wakeup();
	
	// Sleep currently selected device.
	(void) sha204p_sleep();

	// Select other device and put it to sleep.
	sha204p_set_device_id(sha204_i2c_address(device_id == SHA204_DEVICE_CLIENT ? SHA204_DEVICE_HOST : SHA204_DEVICE_CLIENT));
	(void) sha204p_sleep();
}


/** \brief This function is the entry function for example functions that
           use a modified SHA204 ASF component.
 * \return result (0: success, otherwise failure)
 * \todo Put waking up devices and put one of them back to sleep into a function.
 */
int main(void)
{
	static uint8_t ret_code;               // will be zero for successful responses
	static uint8_t wakeup_response[SHA204_RSP_SIZE_MIN];
	static uint8_t tx_buffer[CHECKMAC_COUNT];
	static uint8_t rx_buffer[SHA204_RSP_SIZE_MAX];
	static uint8_t num_in[NONCE_NUMIN_SIZE];
	static uint8_t other_data[CHECKMAC_OTHER_DATA_SIZE];
	static struct sha204_nonce_parameters args_nonce_command_host = {
		.tx_buffer = tx_buffer, .rx_buffer = rx_buffer,
		.mode = NONCE_MODE_NO_SEED_UPDATE, .num_in = num_in
	};
	static struct sha204h_temp_key temp_key;
	static struct sha204h_nonce_in_out args_nonce_tempkey = {
		.mode = NONCE_MODE_NO_SEED_UPDATE, .num_in = num_in, 
		.rand_out = &rx_buffer[SHA204_BUFFER_POS_DATA], .temp_key = &temp_key
	};
	static struct sha204_nonce_parameters args_nonce_command_client = {
		.tx_buffer = tx_buffer, .rx_buffer = rx_buffer,
		.mode = NONCE_MODE_PASSTHROUGH, .num_in = temp_key.value
	};
	static struct sha204_mac_parameters args_mac_command = {
		.tx_buffer = tx_buffer, .rx_buffer = rx_buffer,	.mode = SHA204_MODE_CHECKMAC, 
		.key_id = SHA204_KEY_ID_CHECKMAC, .challenge = NULL
	};
	static struct sha204_check_mac_parameters args_checkmac_command = {
		.tx_buffer = tx_buffer, .rx_buffer = rx_buffer,	.mode = SHA204_MODE_CHECKMAC,
		.key_id = SHA204_KEY_ID_CHECKMAC, .client_challenge = temp_key.value,
		.client_response = &rx_buffer[SHA204_BUFFER_POS_DATA],
		.other_data = other_data
	};

	// Initialize system clock.
	sysclk_init();

	// Initialize the board.
	board_init();


	while (true) {
		/************************** Nonce command to host *********************************/
		// Wake up host device, put client to sleep.
		ret_code = sha204e_wakeup(SHA204_DEVICE_HOST, SHA204_SLEEP);
		if (ret_code != SHA204_SUCCESS)
			continue;
	
		// Issue a Nonce command.
		// Set num_in to any value you like. The device will randomize it by hashing it with
		// a random number returned in "challenge".
		memset(num_in, 0, sizeof(num_in));
		memset(rx_buffer, 0, sizeof(rx_buffer));
		ret_code = sha204m_nonce(&args_nonce_command_host);
		
		// Put host device to sleep.
		(void) sha204p_sleep();

		if (ret_code != SHA204_SUCCESS) {
			display_status_lib(ret_code);
			continue;
		}

		// Calculate TempKey in host.
		temp_key.valid = 0;
		ret_code = sha204h_nonce(&args_nonce_tempkey);
		if (ret_code != SHA204_SUCCESS)
			continue;
	
		/************************** MAC command to client *********************************/
		// Wakeup client device, put host into idle mode.
		ret_code = sha204e_wakeup(SHA204_DEVICE_CLIENT, SHA204_IDLE);
		if (ret_code != SHA204_SUCCESS)
			continue;
	
		// Issue a Nonce command, this time just passing the host TempKey from the host to the client.
		ret_code = sha204m_nonce(&args_nonce_command_client);
		if (ret_code != SHA204_SUCCESS) {
			sha204e_sleep(SHA204_DEVICE_CLIENT);
			display_status_lib(ret_code);
			continue;
		}
	
		// Issue a MAC command.
		ret_code = sha204m_mac(&args_mac_command);

		// Put client to sleep.
		sha204p_sleep();
		if (ret_code != SHA204_SUCCESS) {
			display_status_lib(ret_code);
			continue;
		}
		
		// Supply "other_data" for the subsequent CheckMac command.
		memset(other_data, 0, sizeof(other_data));
		memcpy(other_data, &tx_buffer[SHA204_OPCODE_IDX], CHECKMAC_CLIENT_COMMAND_SIZE);

		/************************** CheckMAC command to host *********************************/
		// Wake up host device, put client to sleep.
		ret_code = sha204e_wakeup(SHA204_DEVICE_HOST, SHA204_SLEEP);
		if (ret_code != SHA204_SUCCESS)
			continue;
			
		ret_code = sha204m_check_mac(&args_checkmac_command);
		(void) sha204p_sleep();
		if (ret_code != SHA204_SUCCESS) {
			display_status_lib(ret_code);
			continue;
		}
		// If CheckMac succeeded the status response byte is 0, otherwise 1.
		if (rx_buffer[SHA204_BUFFER_POS_STATUS] != SHA204_SUCCESS)
			ret_code = SHA204_FUNC_FAIL;

		display_status_lib(ret_code);
	}		
	return ret_code;
}
