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
 *  \brief  Application Example That Authenticates a Client Device by
 *          Issuing a CheckMac Command to a Host Device.
 *  \author Atmel Crypto Products
 *  \date   August 22, 2012
 */
#include <stdint.h>                   // data type definitions
#include <string.h>                   // memset(), memcpy()
#include <asf.h>                      // definitions and declarations for the Atmel Software Framework
#include "sha204_example_led.h"       // definitions of LED utility functions
#include "sha204_examples.h"          // definitions and declarations for example functions
#include "sha204_helper.h"            // definitions and declarations for the Helper module (e.g. TempKey calculation)
#include "sha204_example_checkmac.h"  // definitions and declarations for this module

#define SHA204_DEVICE_CLIENT     0
#define SHA204_DEVICE_HOST       1
#define SHA204_MODE_MAC          MAC_MODE_BLOCK2_TEMPKEY | MAC_MODE_SOURCE_FLAG_MATCH
#define SHA204_MODE_CHECKMAC     CHECKMAC_MODE_BLOCK2_TEMPKEY
#define SHA204_IDLE              true
#define SHA204_SLEEP             false

//#define SHA204_CHECKMAC_NO_IDLE


#ifdef SHA204_CHECKMAC_NO_IDLE
/** \brief This function puts both devices to sleep and displays the error.
           Call this function if both devices are awake and one returns an error.
	\param[in] device_id array index into I2C addresses for client or host
	\param[in] ret_code error code
*/
static void sha204e_handle_error(uint8_t device_id, uint8_t ret_code)
{
	// Sleep device that is selected.
	(void) sha204p_sleep();

	// Select other device and put it to sleep.
	sha204p_set_device_id(sha204_i2c_address(device_id == SHA204_DEVICE_CLIENT ? SHA204_DEVICE_HOST : SHA204_DEVICE_CLIENT));
	(void) sha204p_sleep();

	// Display error code.
	display_status_lib(ret_code);
}

#else
/** \brief This function wakes up both devices and puts the one
           that is not selected to sleep or idle mode.
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

	// Select other device and put it to sleep or idle.
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


/** \brief This function puts both devices to sleep and displays the error.
           Call this function if one of the devices is in idle mode and the other one returns an error.
	\param[in] device_id array index into I2C addresses for client or host
	\param[in] ret_code error code
*/
static void sha204e_handle_error(uint8_t device_id, uint8_t ret_code)
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

	display_status_lib(ret_code);
}
#endif


/** \brief This function authenticates an ATSHA204 device that is used as a client by
           issuing a CheckMac command to an ATSHA204 device that is used as a host.
		   
		   The advantage of this way of authentication is that keys are never exposed
		   but remain sealed and safe inside the ATSHA204 client and host device.
		   The authentication sequence starts with the CPU sending a Nonce command to
		   an ATSHA204 device that serves as a host device, and which, after receiving
		   the Nonce command, computes a sha256 hash and stores it in its TempKey. The 
		   CPU then computes the same TempKey using a SHA204 library function and copies 
		   it over to an ATSHA204 device that serves as a client device to be authenticated.
		   The CPU then sends a MAC command to the client with a mode that tells the
		   client device to compute the MAC using its TempKey instead of a challenge.
		   Finally, the CPU sends the MAC response inside a CheckMac command to the
		   host device. If this MAC response matches the MAC calculation the host device
		   performs after receiving a CheckMac command, the host device responds with
		   success (status byte = 0), otherwise with failure (status byte = 1).
		   
		   When implementing such a sequence, one has to keep in mind that the
		   devices stay only awake between 0.7 and 1.7 s. Therefore, the time between
		   the wakeup of the host device and receiving its CheckMac response has to be
		   less than 0.7 s or one has to put the host device into Idle mode while
		   communicating with the client device. The function below implements both
		   types distinguishable with a compilation switch.
		   
		   Below follows a detailed description of such a command flow and the hashes
		   involved for the case that the entire sequence takes less than 0.7 s (no Idle 
		   mode). Message lengths used in the sha256 calculations are given in brackets. 
		   The key in slot 0 is used (key_id = 0).
		   <ol>
             <li>
			   CPU wakes up host device. If client and host device share the SDA line,
			   as assumed here, both devices will wake up.
			 </li>
		     <li>
		       CPU sends Nonce command with 20-byte num_in to host device and receives 32-byte 
			   random number. We chose mode as 1, no seed update, because we run the
			   authentication sequence in a loop which would wear out the EEPROM location
			   for the seed. (The random number is constant if the configuration zone is
			   not locked.)
			 </li>
			 <li>
			   CPU calculates host TempKey = sha256(random[32], num_in[20], Opcode = 0x16[1],
			                                        mode = 1[1], Param2 LSB = 0[1])
			 </li>
			 <li>
			   CPU sends calculated TempKey inside a Nonce command to the client device
			   (mode = 3, pass-through) and receives status (status byte = 0: success).
			 </li>
			 <li>
			   CPU sends MAC command to client device with mode = 5, second 32 bytes in
			   sha256 calculation come from TempKey (bit 0 set) plus TempKey source is
			   fixed, not random (bit 2 set). It receives the following MAC:\n
			   client TempKey(new) = sha256(key[32], TempKey(old, from host)[32], 
			                                Opcode = 0x08[1], mode = 5[1], key_id = 0[1],
											0[11], sn8[1], 0[4], sn0[1], sn1[1], 0[2])
			 </li>
			 <li>
			   CPU sends CheckMac command to host device with the MAC it received from
			   the MAC command it had sent to the client in the step above, and mode = 1,
			   second 32 bytes in sha256 calculation come from TempKey (bit 0 set). The 
			   host device now computes the MAC given below and compares it with the one
			   it received along with the CheckMac command. If they match it responds
			   with a status of success (status byte = 0), otherwise with failure 
			   (status byte = 1).
			   host MAC = sha256(key[32], host TempKey[32], 
			                     MAC command sent to client device = 0x08050000[4],
								 0[11], sn8[1], 0[4], sn0[1], sn1[1], 0[2])
			 </li>
		   </ol>
		   
 * \return result (0: success, otherwise failure)
 */
uint8_t sha204e_checkmac(void)
{
#ifndef SHA204_CHECKMAC_NO_IDLE

	static uint8_t ret_code;               // is zero for successful responses
	static uint8_t tx_buffer[CHECKMAC_COUNT];
	static uint8_t rx_buffer[SHA204_RSP_SIZE_MAX];
	static uint8_t status_response[SHA204_RSP_SIZE_MIN];
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
		.tx_buffer = tx_buffer, .rx_buffer = status_response,
		.mode = NONCE_MODE_PASSTHROUGH, .num_in = temp_key.value
	};
	static struct sha204_mac_parameters args_mac_command_client = {
		.tx_buffer = tx_buffer, .rx_buffer = rx_buffer,	.mode = SHA204_MODE_MAC,
		.key_id = SHA204_KEY_ID_CHECKMAC, .challenge = NULL
	};
	static struct sha204_check_mac_parameters args_checkmac_command_host = {
		.tx_buffer = tx_buffer, .rx_buffer = status_response, .mode = SHA204_MODE_CHECKMAC,
		.key_id = SHA204_KEY_ID_CHECKMAC, .client_challenge = temp_key.value,
		.client_response = &rx_buffer[SHA204_BUFFER_POS_DATA], .other_data = other_data
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
		// a random number returned in args_nonce_command_host.rand_out.
		ret_code = sha204m_nonce(&args_nonce_command_host);
		if (ret_code != SHA204_SUCCESS) {
			sha204e_handle_error(SHA204_DEVICE_HOST, ret_code);
			continue;
		}
		
		// Put host device into idle mode.
		(void) sha204p_idle();

		// Calculate TempKey in host.
		ret_code = sha204h_nonce(&args_nonce_tempkey);
		if (ret_code != SHA204_SUCCESS) {
			sha204e_handle_error(SHA204_DEVICE_HOST, ret_code);
			continue;
		}
		
		/************************** MAC command to client *********************************/
		// Wakeup client device, put host into idle mode.
		ret_code = sha204e_wakeup(SHA204_DEVICE_CLIENT, SHA204_IDLE);
		if (ret_code != SHA204_SUCCESS)
			continue;
		
		// Issue a Nonce command, this time just passing the host TempKey from the host to the client.
		ret_code = sha204m_nonce(&args_nonce_command_client);
		if (ret_code != SHA204_SUCCESS) {
			sha204e_handle_error(SHA204_DEVICE_CLIENT, ret_code);
			continue;
		}
		
		// Issue a MAC command.
		ret_code = sha204m_mac(&args_mac_command_client);
		if (ret_code != SHA204_SUCCESS) {
			sha204e_handle_error(SHA204_DEVICE_CLIENT, ret_code);
			continue;
		}

		// Put client to sleep.
		sha204p_sleep();
		
		// Supply "other_data" for the subsequent CheckMac command.
		memset(other_data, 0, sizeof(other_data));
		memcpy(other_data, &tx_buffer[SHA204_OPCODE_IDX], CHECKMAC_CLIENT_COMMAND_SIZE);

		/************************** CheckMac command to host *********************************/
		// Wake up host device, put client to sleep.
		ret_code = sha204e_wakeup(SHA204_DEVICE_HOST, SHA204_SLEEP);
		if (ret_code != SHA204_SUCCESS)
			continue;
		
		ret_code = sha204m_check_mac(&args_checkmac_command_host);
		(void) sha204p_sleep();
		if (ret_code != SHA204_SUCCESS) {
			display_status_lib(ret_code);
			continue;
		}
		// If CheckMac succeeded the status response byte is 0, otherwise 1.
		if (status_response[SHA204_BUFFER_POS_STATUS] != SHA204_SUCCESS)
			ret_code = SHA204_FUNC_FAIL;

		display_status_lib(ret_code);
	}
	return ret_code;

#else /********* command sequence without idling a device *********************/

	static uint8_t ret_code;               // will be zero for successful responses
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
	static struct sha204_mac_parameters args_mac_command_client = {
		.tx_buffer = tx_buffer, .rx_buffer = rx_buffer,	.mode = SHA204_MODE_MAC,
		.key_id = SHA204_KEY_ID_CHECKMAC, .challenge = NULL
	};
	static struct sha204_check_mac_parameters args_checkmac_command_host = {
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
		// Wake up devices.
		sha204p_wakeup();
		
		// Select host.
		sha204p_set_device_id(sha204_i2c_address(SHA204_DEVICE_HOST));

		// Issue a Nonce command.
		// Set num_in to any value you like. The device will randomize it by hashing it with
		// a random number returned in "challenge".
		memset(num_in, 0, sizeof(num_in));
		memset(rx_buffer, 0, sizeof(rx_buffer));
		ret_code = sha204m_nonce(&args_nonce_command_host);
		if (ret_code != SHA204_SUCCESS) {
			sha204e_handle_error(SHA204_DEVICE_HOST, ret_code);
			continue;
		}

		// Calculate TempKey in host.
		ret_code = sha204h_nonce(&args_nonce_tempkey);
		if (ret_code != SHA204_SUCCESS) {
			sha204e_handle_error(SHA204_DEVICE_HOST, ret_code);
			continue;
		}

		/************************** MAC command to client *********************************/
		// Select client.
		sha204p_set_device_id(sha204_i2c_address(SHA204_DEVICE_CLIENT));
		
		// Issue a Nonce command, this time just passing the host TempKey from the host to the client.
		ret_code = sha204m_nonce(&args_nonce_command_client);
		if (ret_code != SHA204_SUCCESS) {
			sha204e_handle_error(SHA204_DEVICE_CLIENT, ret_code);
			continue;
		}

		// Issue a MAC command.
		ret_code = sha204m_mac(&args_mac_command_client);
		
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
		// Select host.
		sha204p_set_device_id(sha204_i2c_address(SHA204_DEVICE_HOST));
		
		ret_code = sha204m_check_mac(&args_checkmac_command_host);
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

#endif
}
