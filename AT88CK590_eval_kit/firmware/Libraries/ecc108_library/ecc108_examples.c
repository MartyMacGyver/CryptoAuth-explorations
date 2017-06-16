/** \file
 *  \brief  Application examples that Use the ECC108 Library
 *  \author Atmel Crypto Products
 *  \date   June 20, 2013

* \copyright Copyright (c) 2013 Atmel Corporation. All rights reserved.
*
* \atmel_crypto_device_library_license_start
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
*    Atmel integrated circuit.
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
* \atmel_crypto_device_library_license_stop
 *
 *   Example functions are given that demonstrate the device.

 *   ecc108e_checkmac_device:
 *      Demonstrates communication using a Mac - CheckMac command sequence with 
 *      relatively low security (mode 0: no nonce), but little code space usage.

 *   ecc108e_checkmac_firmware:
 *      Demonstrates high security using a Nonce - GenDig - Mac command sequence
 *      and MAC verification in firmware. This requires more code space because
 *      a sha256 implementation in firmware is needed. Also, the firmware has to
 *      be able to access keys. Having a key stored outside the device poses a
 *      higher security risk.

 *   ecc108e_checkmac_derive_key:
 *      Demonstrates high security in a host / client scenario using a DeriveKey / 
 *      Mac command sequence on one device (client) and a GenDig / CheckMac sequence 
 *      on another device (host). No sha256 implementation in firmware is needed.
 *      All keys are only stored on the devices and never revealed. When using
 *      I2C you have to change the address of one of the devices first.
 *      This example needs modifications introducing the Pause command when using
 *      the SWI UART interface.

 *   ecc108e_change_i2c_address:
 *      This is just a utility that changes the I2C address of a device so that
 *      you can run the \ref ecc108e_checkmac_derive_key example when using I2C.
 *
 *   CAUTION WHEN DEBUGGING: Be aware of the timeout feature of the device. The
 *   device will go to sleep between 0.7 and 1.5 seconds after a Wakeup. When
 *   hitting a break point, this timeout will likely to kick in and the device
 *   has gone to sleep before you continue debugging. Therefore, after you have
 *   examined variables you might have to restart your debug session.
*/

#include <string.h>                   // needed for memset(), memcpy()
#include <stdbool.h>                  // definitions for boolean types

#include "ecc108_lib_return_codes.h"  // declarations of function return codes
#include "ecc108_comm_marshaling.h"   // definitions and declarations for the Command Marshaling module
#include "ecc108_helper.h"            // definitions of functions that calculate SHA256 for every command
#include "ecc108_examples.h"          // definitions and declarations for example functions
#include "timer_utilities.h"          // definitions and declarations for timer functions

#ifdef ECC108_I2C
#   include "i2c_phys.h"
#else
#   include "swi_phys.h"
#endif


/** 
 * \brief  This macro brings a device from Idle mode into Sleep mode by 
 *         waking it up and sending a Sleep flag.
 */
#define ecc108e_wakeup_sleep()   {ecc108p_wakeup(); ecc108p_sleep();}


/** 
 * \brief This function wraps \ref ecc108p_sleep().
 *        It puts both devices to sleep if two devices (client and host) are used.
 *        This function is also called when a Wakeup did not succeed. 
 *        This would not make sense if a device did not wakeup and it is the only
 *        device on SDA, but if there are two devices (client and host) that
 *        share SDA, the device that is not selected might have woken up.
 */
void ecc108e_sleep() 
{
#if defined(ECC108_I2C) && (ECC108_CLIENT_ADDRESS != ECC108_HOST_ADDRESS)
	// Select host device...
	ecc108p_set_device_id(ECC108_HOST_ADDRESS);
	// and put it to sleep.
	(void) ecc108p_sleep();
	// Select client device...
	ecc108p_set_device_id(ECC108_CLIENT_ADDRESS);
	// and put it to sleep.
	(void) ecc108p_sleep();
#else	
	(void) ecc108p_sleep();
#endif
}


/** \brief This function wakes up two I2C devices and puts one back to
           sleep, effectively waking up only one device among two that
		   share SDA.
	\param[in] device_id which device to wake up
	\return status of the operation
*/
uint8_t ecc108e_wakeup_device(uint8_t device_id)
{
	uint8_t ret_code;
	uint8_t wakeup_response[ECC108_RSP_SIZE_MIN];

	ecc108p_set_device_id(device_id);

	// Wake up the devices.
	memset(wakeup_response, 0, sizeof(wakeup_response));
	ret_code = ecc108c_wakeup(wakeup_response);
	if (ret_code != ECC108_SUCCESS) {
		ecc108e_sleep();
		return ret_code;
	}

#if defined(ECC108_I2C) && (ECC108_CLIENT_ADDRESS != ECC108_HOST_ADDRESS)
	// ECC108 I2C devices share SDA. We have to put the other device back to sleep.
	// Select other device...
	ecc108p_set_device_id(device_id == ECC108_CLIENT_ADDRESS ? ECC108_HOST_ADDRESS : ECC108_CLIENT_ADDRESS);
	// and put it to sleep.
	ret_code = ecc108p_sleep();
#endif

	return ret_code;	
}


/** \brief This function checks the response status byte and puts the device
           to sleep if there was an error.
    \param[in] ret_code return code of function
	\param[in] response pointer to response buffer
	\return status of the operation
*/
uint8_t ecc108e_check_response_status(uint8_t ret_code, uint8_t *response)
{
	if (ret_code != ECC108_SUCCESS) {
		ecc108p_sleep();
		return ret_code;
	}
	ret_code = response[ECC108_BUFFER_POS_STATUS];
	if (ret_code != ECC108_SUCCESS)
		ecc108p_sleep();

	return ret_code;	
}


/** \brief This function is a simple example for how to use the library. 
           It wakes up the device, sends a DevRev command, receives its 
		   response, and puts the device to sleep. It uses a total of
		   four library functions from all three layers, physical, 
		   communication, and command marshaling layer.
		   
		   Use this example to familiarize yourself with the library
		   and device communication before proceeding to examples that
		   deal with the security features of the device.
	\return status of the operation
*/
uint8_t ecc108e_send_info_command(void)
{
	uint8_t ret_code;
	uint8_t wakeup_response[ECC108_RSP_SIZE_MIN];
	uint8_t command[INFO_COUNT];
	uint8_t response[INFO_RSP_SIZE];

	ecc108p_set_device_id(ECC108_CLIENT_ADDRESS);
	
	while (true) {
		// Wake up the device.
		memset(wakeup_response, 0, sizeof(wakeup_response));
		ret_code = ecc108c_wakeup(wakeup_response);
		if (ret_code != ECC108_SUCCESS)
			continue;
			
		(void) ecc108m_info(command, response, INFO_MODE_REVISION, INFO_NO_STATE);
		
		ecc108p_sleep();
		
		delay_ms(100);
	}
	return ret_code;	
}


/** \brief This function configures client and host device for the ChildKey example.
 *
 *         Creating a child key allows a host device to check a MAC
 *         in a highly secure fashion. No replay attacks are possible
 *         and SHA256 calculation in firmware is not needed.
 *         To run this command sequence successfully the devices have
 *         to be configured first: We use a slot in the client device that is already 
 *         configured for this purpose, but we want to disable SingleUse. Since we
 *         are re-configuring it anyway, we also allow encrypted read.
 *         Only one device could be used for demonstration purpose, but since the
 *         parent keys have to match, the parent key would have to be duplicated
 *         in a different slot with the CheckMac flag set.
 * \param[in] parent_id slot to re-configure
 * \return status of the operation
 */
uint8_t ecc108e_configure(uint8_t parent_id)
{
	// declared as "volatile" for easier debugging
	volatile uint8_t ret_code;
	
	uint8_t config_index;
	
	// configuration zone address for key (e.g. 48, 49)
	uint16_t config_address = 20 + 2 * parent_id;
	
	const uint8_t read_config = 0xC0;
	const uint8_t write_config = 0x70;
	
	// Make the command buffer the minimum size of the Write command.
	uint8_t command[WRITE_COUNT_SHORT];
	
	uint8_t config_data[ECC108_ZONE_ACCESS_4];

	// Make the response buffer the size of a Read response.
	uint8_t response[READ_4_RSP_SIZE];

	// Wake up the client device.
	ret_code = ecc108e_wakeup_device(ECC108_CLIENT_ADDRESS);
	if (ret_code != ECC108_SUCCESS)
		return ret_code;
	
	// Read client device configuration for parent key in order to change only the byte we need to change.
	memset(response, 0, sizeof(response));
	ret_code = ecc108m_read(command, response, ECC108_ZONE_CONFIG, config_address);
	if (ret_code != ECC108_SUCCESS) {
		ecc108p_sleep();
		return ret_code;
	}
	// Modify configuration.
	memcpy(config_data, &response[ECC108_BUFFER_POS_DATA], sizeof(config_data));
	config_index = parent_id / 2 ? 2 : 0;
	if (config_data[config_index] == read_config && config_data[config_index + 1] == write_config) {
		// Slot is already configured.
		ecc108p_sleep();
		return ret_code;
	}
	// ReadConfig = IsSecret | Encrypt. Key id for encrypted read = 0
	config_data[config_index++] = read_config;
	config_data[config_index] = write_config;
	// Write client configuration.
	ret_code = ecc108m_write(command, response, ECC108_ZONE_CONFIG, config_address, config_data, NULL);
	// Put client device to sleep.
	ecc108p_sleep();

	//ecc108e_read_config_zone();
	
	return ret_code;
}


/** \brief This function serves as an example for
 *         the ECC108 Mac and CheckMac commands.
 *
 *         In an infinite loop, it issues the same command
 *         sequence using the Command Marshaling layer of
 *         the ECC108 library.
 * \return status of the operation
 */
uint8_t ecc108e_checkmac_device(void)
{
	// declared as "volatile" for easier debugging
	volatile uint8_t ret_code;

	uint8_t i;
	
	// Make the command buffer the size of the CheckMac command.
	static uint8_t command[CHECKMAC_COUNT];

	// Make the response buffer the size of a MAC response.
	static uint8_t response_mac[MAC_RSP_SIZE];
	
	// First four bytes of Mac command are needed for CheckMac command.
	static uint8_t other_data[CHECKMAC_OTHER_DATA_SIZE];
	
	// CheckMac response buffer
	static uint8_t response_checkmac[CHECKMAC_RSP_SIZE];

   // expected MAC response in mode 0
	static const uint8_t mac_mode0_response_expected[MAC_RSP_SIZE] =
	{
		MAC_RSP_SIZE,                                   // count
		0x06, 0x67, 0x00, 0x4F, 0x28, 0x4D, 0x6E, 0x98,
		0x62, 0x04, 0xF4, 0x60, 0xA3, 0xE8, 0x75, 0x8A,
		0x59, 0x85, 0xA6, 0x79, 0x96, 0xC4, 0x8A, 0x88,
		0x46, 0x43, 0x4E, 0xB3, 0xDB, 0x58, 0xA4, 0xFB,
		0xE5, 0x73                                       // CRC
	};

	// data for challenge in MAC mode 0 command
	const uint8_t challenge[MAC_CHALLENGE_SIZE] = {
		0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
		0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
		0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
		0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
	};

	// Initialize the hardware interface.
	// Depending on which interface you have linked the
	// library to, it initializes SWI UART, SWI GPIO, or TWI.
	ecc108p_init();

	while (1) {
		// The following command sequence wakes up the device, issues a MAC command in mode 0
		// using the Command Marshaling layer, puts the device to sleep, and verifies the MAC
		// (fixed challenge / response). Then it wakes up the same or a second device, issues 
		// a CheckMac command supplying data obtained from the previous Mac command, verifies
		// the response status byte, and puts the device to sleep.

		ret_code = ecc108e_wakeup_device(ECC108_CLIENT_ADDRESS);
		if (ret_code != ECC108_SUCCESS)
			return ret_code;

		// Mac command with mode = 0.
		memset(response_mac, 0, sizeof(response_mac));
		ret_code = ecc108m_execute(ECC108_MAC, MAC_MODE_CHALLENGE, ECC108_KEY_ID, sizeof(challenge), (uint8_t *) challenge,
					0, NULL, 0, NULL, sizeof(command), command, sizeof(response_mac), response_mac);
		// Put client device to sleep.
		ecc108p_sleep();
		if (ret_code != ECC108_SUCCESS)
			continue;

		// Compare returned MAC with expected one.
		for (i = 0; i < MAC_RSP_SIZE; i++) {
			if (response_mac[i] != mac_mode0_response_expected[i])
				ret_code = ECC108_GEN_FAIL;
		}

		// Now check the MAC using the CheckMac command.

		ret_code = ecc108e_wakeup_device(ECC108_HOST_ADDRESS);
		if (ret_code != ECC108_SUCCESS)
			return ret_code;

		// CheckMac command with mode = 0.
		// Use the wakeup_response buffer for the CheckMac response.
		memset(response_checkmac, 0, sizeof(response_checkmac));
		// Copy Mac command byte 1 to 5 (op-code, param1, param2) to other_data.
		memcpy(other_data, &command[ECC108_OPCODE_IDX], CHECKMAC_CLIENT_COMMAND_SIZE);
		// Set the remaining nine bytes of other_data to 0.
		memset(&other_data[CHECKMAC_CLIENT_COMMAND_SIZE - 1], 0, sizeof(other_data) - CHECKMAC_CLIENT_COMMAND_SIZE);
		ret_code = ecc108m_execute(ECC108_CHECKMAC, CHECKMAC_MODE_CHALLENGE, ECC108_KEY_ID, 
					sizeof(challenge), (uint8_t *) challenge, 
					CHECKMAC_CLIENT_RESPONSE_SIZE, &response_mac[ECC108_BUFFER_POS_DATA], 
					sizeof(other_data), other_data, 
					sizeof(command), command, sizeof(response_checkmac), response_checkmac);

		// Put host device to sleep.
		ecc108p_sleep();
		
		// Status byte = 0 means success. This line serves only a debug purpose.
		ret_code = response_checkmac[ECC108_BUFFER_POS_STATUS];
	}

	return ret_code;
}


/** \brief This function serves as an example for
 *         the ECC108 Nonce, GenDig, and Mac commands.
 *
 *         In an infinite loop, it issues the same command
 *         sequence using the Command Marshaling layer of
 *         the ECC108 library.
 * \return status of the operation
 */
uint8_t ecc108e_checkmac_firmware(void)
{
	// declared as "volatile" for easier debugging
	volatile uint8_t ret_code;
	uint8_t i;
	uint8_t mac_mode = MAC_MODE_BLOCK1_TEMPKEY | MAC_MODE_BLOCK2_TEMPKEY;
	struct ecc108h_nonce_in_out nonce_param;	//parameter for nonce helper function
	struct ecc108h_gen_dig_in_out gendig_param;	//parameter for gendig helper function
	struct ecc108h_mac_in_out mac_param;		//parameter for mac helper function
	struct ecc108h_temp_key tempkey;			//tempkey parameter for nonce and mac helper function
	static uint8_t wakeup_response[ECC108_RSP_SIZE_MIN];
	static uint8_t tx_buffer[CHECKMAC_COUNT];
	static uint8_t rx_buffer[MAC_RSP_SIZE];
	static uint8_t mac[CHECKMAC_CLIENT_RESPONSE_SIZE];
	uint8_t num_in[NONCE_NUMIN_SIZE] = {
		0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
		0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
		0x30, 0x31, 0x32, 0x33
	};
	uint8_t key_slot_0[ECC108_KEY_SIZE] = {
		0x00, 0x00, 0xA1, 0xAC, 0x57, 0xFF, 0x40, 0x4E,
		0x45, 0xD4,	0x04, 0x01, 0xBD, 0x0E, 0xD3, 0xC6,
		0x73, 0xD3, 0xB7, 0xB8,	0x2D, 0x85, 0xD9, 0xF3,
		0x13, 0xB5, 0x5E, 0xDA, 0x3D, 0x94,	0x00, 0x00
	};

	// Initialize the hardware interface.
	// Depending on which interface you have linked the
	// library to, it initializes SWI UART, SWI GPIO, or TWI.
	ecc108p_init();

	while (1) {
		// The following command sequence wakes up the device, issues a Nonce, a GenDig, and 
		// a Mac command using the Command Marshaling layer, and puts the device to sleep. 
		// In parallel it calculates in firmware the TempKey and the MAC using helper 
		// functions and compares the Mac command response with the calculated result.

		// ----------------------- Nonce --------------------------------------------
		// Wake up the device.
		memset(wakeup_response, 0, sizeof(wakeup_response));
		ret_code = ecc108c_wakeup(wakeup_response);
		if (ret_code != ECC108_SUCCESS) {
			(void) ecc108p_sleep();
			continue;
		}			
	
		// Issue a Nonce command. When the configuration zone of the device is not locked the 
		// random number returned is a constant 0xFFFF0000FFFF0000...
		memset(rx_buffer, 0, sizeof(rx_buffer));
		ret_code = ecc108m_execute(ECC108_NONCE, NONCE_MODE_NO_SEED_UPDATE, 0, NONCE_NUMIN_SIZE, num_in, 
			0, NULL, 0, NULL, sizeof(tx_buffer), tx_buffer, sizeof(rx_buffer), rx_buffer);
		if (ret_code != ECC108_SUCCESS) {
			(void) ecc108p_sleep();
			continue;
		}

		// Put device into Idle mode since the TempKey calculation in firmware might take longer
		// than the device timeout. Putting the device into Idle instead of Sleep mode
		// maintains the TempKey.
		ecc108p_idle();
		
		// Calculate TempKey using helper function.
		nonce_param.mode = NONCE_MODE_NO_SEED_UPDATE;
		nonce_param.num_in = num_in;	
		nonce_param.rand_out = &rx_buffer[ECC108_BUFFER_POS_DATA];	
		nonce_param.temp_key = &tempkey;
		ret_code = ecc108h_nonce(&nonce_param);
		if (ret_code != ECC108_SUCCESS) {
			ecc108e_wakeup_sleep();
			continue;
		}

		// ----------------------- GenDig --------------------------------------------
		// Wake up the device from Idle mode.
		memset(wakeup_response, 0, sizeof(wakeup_response));
		ret_code = ecc108c_wakeup(wakeup_response);
		if (ret_code != ECC108_SUCCESS) {
			(void) ecc108p_sleep();
			continue;
		}			

		memset(rx_buffer, 0, sizeof(rx_buffer));
		ret_code = ecc108m_execute(ECC108_GENDIG, GENDIG_ZONE_DATA, ECC108_KEY_ID, 0,
			NULL, 0, NULL, 0, NULL, sizeof(tx_buffer), tx_buffer, sizeof(rx_buffer), rx_buffer);		 
		if (ret_code != ECC108_SUCCESS) {
			(void) ecc108p_sleep();
			continue;
		}
		// Check response status byte for error.
		if (rx_buffer[ECC108_BUFFER_POS_STATUS] != ECC108_SUCCESS) {
			(void) ecc108p_sleep();
			continue;
		}
		ecc108p_idle();

		// Update TempKey using helper function.
		gendig_param.zone = GENDIG_ZONE_DATA;
		gendig_param.key_id = ECC108_KEY_ID;
		gendig_param.stored_value = key_slot_0;
		gendig_param.temp_key = &tempkey;
		ret_code = ecc108h_gen_dig(&gendig_param);
		if (ret_code != ECC108_SUCCESS) {
			ecc108e_wakeup_sleep();
			continue;
		}

		// ----------------------- Mac --------------------------------------------
		// Wake up the device from Idle mode.
		memset(wakeup_response, 0, sizeof(wakeup_response));
		ret_code = ecc108c_wakeup(wakeup_response);
		if (ret_code != ECC108_SUCCESS) {
			(void) ecc108p_sleep();
			continue;
		}
		
		// Issue a Mac command with mode = 3.
		memset(rx_buffer, 0, sizeof(rx_buffer));
		ret_code = ecc108m_execute(ECC108_MAC, mac_mode, ECC108_KEY_ID,
			0, NULL, 0, NULL, 0, NULL, sizeof(tx_buffer), tx_buffer, sizeof(rx_buffer), rx_buffer);		 

		// Put device to sleep.
		ecc108p_sleep();

		if (ret_code != ECC108_SUCCESS)
			continue;
		
		// Calculate MAC using helper function.
		mac_param.mode = mac_mode;
		mac_param.key_id = ECC108_KEY_ID;
		mac_param.challenge = NULL;
		mac_param.key = NULL;
		mac_param.otp = NULL;
		mac_param.sn = NULL;
		mac_param.response = mac;
		mac_param.temp_key = &tempkey;
		ret_code = ecc108h_mac(&mac_param);
		if (ret_code != ECC108_SUCCESS)
			continue;
		
		// Compare the Mac response with the calculated MAC.
		for (i = 0; i < sizeof(mac); i++) {
			if (rx_buffer[i + ECC108_BUFFER_POS_STATUS] != mac[i])
				ret_code = ECC108_GEN_FAIL;
		}
	}

	return ret_code;
}


/** \brief This function serves as an example for the ECC108 Nonce, 
 *         DeriveKey, and Mac commands for a client, and the Nonce, GenDig, and 
 *         CheckMac commands for a host device.
 *
 *         Creating a child key on the client allows a host device to check a MAC
 *         in a highly secure fashion. No replay attacks are possible,
 *         SHA256 calculation in firmware is not needed, and keys are only stored
 *         on the secure device. 
 *         A brief explanation for this command sequence:
 *         The client generates a child key (DeriveKey command) derived from a 
 *         parent key that it shares with the host device, and stores it in one
 *         of its key slots using a random nonce (commands Random and Nonce). 
 *         The host generates the same key and stores it in its TempKey using 
 *         the same nonce. Now, when the client receives a Mac command with the 
 *         child key id, a CheckMac command on the host using the TempKey will 
 *         succeed.
 *         To run this command sequence successfully the devices have
 *         to be configured first: The child key has to point to the parent,
 *         and the parent key in the host device has to be flagged as CheckOnly.
 *         This sequence could be run with one device, but since the
 *         parent keys have to match the parent key would have to be duplicated
 *         in a different slot.
 *         Because every time this command sequence is executed the slot for the
 *         child key is being written, this sequence does not run in loop to
 *         prevent wearing out the flash.

Use the following sequence for secure authentication using the default configuration
and setting ReadConfig of slot 3 (parent key) from the default A3 (single use) to C0
(read encrypted using slot 0). Use slot 8 as the child key for debugging since it is readable.
1. MCU to client device: fixed nonce -> TempKey
2. MCU to client device: DeriveKey -> child key in chosen slot (parent key in slot 3)
3. MCU to client device: fixed nonce -> TempKey
4. MCU to client device: Mac -> response = sha256(chosen slot  / child key, fixed nonce / TempKey, command, 3 bytes of SN)
5. MCU to host device:   GenDig -> TempKey = child key
6. MCU to host device:   CheckMac -> sha256(child key / TempKey, challenge / fixed nonce, MAC command, 3 bytes of SN)
 * \return status of the operation
 */
uint8_t ecc108e_derive_key(void)
{
	// declared as "volatile" for easier debugging
	volatile uint8_t ret_code;

	const uint8_t parent_id = 0;
	const uint8_t child_id = 3; // WriteConfig = 0x60. 6: Encrypt without MAC. 0: parent key
	static uint8_t response_random[RANDOM_RSP_SIZE];
	uint8_t *random = &response_random[ECC108_BUFFER_POS_DATA];
	
	// Make the command buffer the minimum size of the Write command.
	static uint8_t command[ECC108_CMD_SIZE_MAX];

	// Make the response buffer the maximum size.
	static uint8_t response_status[ECC108_RSP_SIZE_MIN];

	// Mac response buffer
	static uint8_t response_mac[ECC108_RSP_SIZE_MAX];
	
	// We need this buffer for the DeriveKey, GenDig, and CheckMac command.
	static uint8_t other_data[CHECKMAC_OTHER_DATA_SIZE];
	
	static uint8_t command_derive_key[GENDIG_OTHER_DATA_SIZE];
	
	static uint8_t command_mac[CHECKMAC_CLIENT_COMMAND_SIZE];
		
	// Initialize the hardware interface.
	// Depending on which interface you have linked the
	// library to, it initializes SWI GPIO, or TWI.
	// This example does not run when SWI UART is used.
	ecc108p_init();

	ret_code = ecc108e_configure(parent_id);
	if (ret_code != ECC108_SUCCESS)
		return ret_code;

	ret_code = ecc108e_wakeup_device(ECC108_HOST_ADDRESS);
	if (ret_code != ECC108_SUCCESS)
		return ret_code;
	
	/*
	Obtain a random number from host device. We can generate a random number to
	be used by a pass-through nonce (TempKey.SourceFlag = Input = 1) in whatever 
	way we want but we use the host	device because it has a high-quality random 
	number generator. We are using the host and not the client device because we 
	like to show a typical accessory authentication example where the MCU this 
	code is running on and the host device are inaccessible to an adversary, 
	whereas the client device is built into an easily accessible accessory. We 
	prevent an adversary to	mount replay attacks by supplying the pass-through
	nonce. For the same reason, we do not want to use the same pass-through 
	number every time we authenticate. The same nonce would produce the same Mac 
	response. Be aware that the Random command returns a fixed number
	(0xFFFF0000FFFF0000...) when the device is not locked.
	*/
	// No need to update the seed because it gets updated with every wake / sleep
	// cycle anyway.
	ret_code = ecc108m_random(command, response_random, RANDOM_NO_SEED_UPDATE);
	ecc108p_sleep();
	if (ret_code != ECC108_SUCCESS)
		return ret_code;
	
	// ---------------------------------------------------------------------------
	// client: Create child key using a random pass-through nonce. 
	// Then send a Mac command using the same nonce.
	// ---------------------------------------------------------------------------
	ret_code = ecc108e_wakeup_device(ECC108_CLIENT_ADDRESS);
	if (ret_code != ECC108_SUCCESS)
		return ret_code;

	// Send Nonce command in pass-through mode using the random number in preparation
	// for DeriveKey command. TempKey holds the random number after this command succeeded.
	ret_code = ecc108m_nonce(command, response_status, NONCE_MODE_PASSTHROUGH, random);
	ret_code = ecc108e_check_response_status(ret_code, response_status);
	if (ret_code != ECC108_SUCCESS)
		return ret_code;

	// Send DeriveKey command.
	// child key = sha256(parent key[32], DeriveKey command[4], sn[3], 0[25], TempKey[32])
	ret_code = ecc108m_derive_key(command, response_status, DERIVE_KEY_RANDOM_FLAG, child_id, NULL);
	ret_code = ecc108e_check_response_status(ret_code, response_status);
	if (ret_code != ECC108_SUCCESS)
		return ret_code;
	
	// Copy op-code and parameters to other_data to be used in subsequent GenDig and CheckMac
	// host commands.
	// Save op-code and parameters to be used in the GenDig command for the host.
	memcpy(command_derive_key, &command[ECC108_OPCODE_IDX], sizeof(command_derive_key));
	// Send Nonce command in preparation for Mac command.
	ret_code = ecc108m_nonce(command, response_status, NONCE_MODE_PASSTHROUGH, random);
	ret_code = ecc108e_check_response_status(ret_code, response_status);
	if (ret_code != ECC108_SUCCESS)
		return ret_code;

	// Send Mac command.
	// MAC = sha256(child key[32], TempKey[32], Mac command[4], 0[11], sn8[1], 0[4], sn0_1[2], 0[2])
	// where TempKey = random
	// mode: first 32 bytes data slot (= child key), second 32 bytes TempKey (= nonce), TempKey.SourceFlag = Input
	ret_code = ecc108m_mac(command, response_mac, MAC_MODE_BLOCK2_TEMPKEY | MAC_MODE_SOURCE_FLAG_MATCH, child_id, NULL);
	ret_code = ecc108e_check_response_status(ret_code, response_status);
	if (ret_code != ECC108_SUCCESS)
		return ret_code;

	// Save op-code and parameters to be used in the CheckMac command for the host.
	memcpy(command_mac, &command[ECC108_OPCODE_IDX], sizeof(command_mac));
		
	// Put client device to sleep.
	ecc108p_sleep();
	
	// ---------------------------------------------------------------------------
	// host: Generate digest (GenDig) using a random pass-through nonce.
	// Then send a CheckMac command with the Mac response.
	// ---------------------------------------------------------------------------

	ret_code = ecc108e_wakeup_device(ECC108_HOST_ADDRESS);
	if (ret_code != ECC108_SUCCESS)
		return ret_code;

	// Send Nonce command in pass-through mode using the random number in preparation
	// for GenDig command. TempKey holds the random number after this command succeeded.
	ret_code = ecc108m_nonce(command, response_status, NONCE_MODE_PASSTHROUGH, random);
	ret_code = ecc108e_check_response_status(ret_code, response_status);
	if (ret_code != ECC108_SUCCESS)
		return ret_code;

	// Send GenDig command. Tempkey holds the child key of the client after this command succeeded.
	// TempKey host = sha256(parent key[32], DeriveKey command[4], sn[3], 0[25], TempKey[32]) = child key client
	// Copy DeriveKey command bytes (op-code, param1, param2) to other_data.
	memcpy(other_data, command_derive_key, sizeof(command_derive_key));
	// Set the remaining nine bytes of other_data to 0.
	memset(&other_data[sizeof(command_derive_key) - 1], 0, sizeof(other_data) - sizeof(command_derive_key));
	ret_code = ecc108m_gen_dig(command, response_status, GENDIG_ZONE_DATA, parent_id, other_data);
	ret_code = ecc108e_check_response_status(ret_code, response_status);
	if (ret_code != ECC108_SUCCESS)
		return ret_code;
		
	// Send CheckMac command.
	// CheckMac = sha256(TempKey[32], random[32], Mac command[4], 0[11], sn8[1], 0[4], sn0_1[2], 0[2])
	// mode: first 32 bytes TempKey (= child key), second 32 bytes TempKey (= nonce), TempKey.SourceFlag = Input
	// TempKey = child key -> CheckMac = Mac
	// Copy Mac command bytes (op-code, param1, param2) to other_data.
	memcpy(other_data, command_mac, sizeof(command_mac));
	// Set the remaining nine bytes of other_data to 0.
	memset(&other_data[sizeof(command_mac) - 1], 0, sizeof(other_data) - sizeof(command_mac));
	ret_code = ecc108m_check_mac(command, response_status, CHECKMAC_MODE_BLOCK1_TEMPKEY | CHECKMAC_MODE_SOURCE_FLAG_MATCH, 0, random, &response_mac[ECC108_BUFFER_POS_DATA], other_data);
	ret_code = ecc108e_check_response_status(ret_code, response_status);

	return ret_code;
}


/** \brief This function changes the I2C address of a device.
           Running it will access the device with I2C address \ref ECC108_CLIENT_ADDRESS
		   and change it to \ref ECC108_HOST_ADDRESS as long as the configuration zone is 
		   not locked (byte under address 87 = 0x55). Be aware that bit 3 of the I2C address
		   is also used as a TTL enable bit. So make sure you give it a value that
		   agrees with your system (see data sheet).
 * \return status of the operation
 */
uint8_t ecc108e_change_i2c_address(void)
{
	// declared as "volatile" for easier debugging
	volatile uint8_t ret_code;
	
	uint16_t config_address;
	
	// Make the command buffer the minimum size of the Write command.
	uint8_t command[WRITE_COUNT_SHORT];
	
	uint8_t config_data[ECC108_ZONE_ACCESS_4];

	// Make the response buffer the size of a Read response.
	uint8_t response[READ_4_RSP_SIZE];

	ecc108p_init();

	ecc108p_set_device_id(ECC108_CLIENT_ADDRESS);
	
	ret_code = ecc108c_wakeup(response);
	if (ret_code != ECC108_SUCCESS)
		return ret_code;
		
	// Make sure that configuration zone is not locked.
	memset(response, 0, sizeof(response));
	config_address = 84;
	ret_code = ecc108m_read(command, response, ECC108_ZONE_CONFIG, config_address);
	if (ret_code != ECC108_SUCCESS) {
		ecc108p_sleep();
		return ret_code;
	}
	if (response[4] != 0x55) {
		// Configuration zone is locked. We cannot change the I2C address.
		ecc108p_sleep();
		return ECC108_FUNC_FAIL;
	}
	
	// Read device configuration at address 16 that contains the I2C address.
	memset(response, 0, sizeof(response));
	config_address = 16;
	ret_code = ecc108m_read(command, response, ECC108_ZONE_CONFIG, config_address);
	if (ret_code != ECC108_SUCCESS) {
		ecc108p_sleep();
		return ret_code;
	}
	config_data[0] = ECC108_HOST_ADDRESS;
	memcpy(&config_data[1], &response[ECC108_BUFFER_POS_DATA + 1], sizeof(config_data - 1));

	ret_code = ecc108m_write(command, response, ECC108_ZONE_CONFIG, config_address, config_data, NULL);

	ecc108p_sleep();
	if (ret_code != ECC108_SUCCESS)
		return ret_code;

	// Check whether we had success.
	ecc108p_set_device_id(ECC108_HOST_ADDRESS);
	ret_code = ecc108c_wakeup(response);
	ecc108p_sleep();

	return ret_code;
}


/** \brief This function reads all 88 bytes from the configuration zone.
           Obtain the data by putting a breakpoint after every read and inspecting "response".

factory defaults of configuration zone
01 23 76 ab 00 04 05 00 0c 8f b7 bd ee 55 01 00 c8 00 55 00 8f 80 80 a1 82 e0 a3 60 94 40 a0 85
86 40 87 07 0f 00 89 f2 8a 7a 0b 8b 0c 4c dd 4d c2 42 af 8f ff 00 ff 00 ff 00 1f 00 ff 00 1f 00
ff 00 ff 00 1f ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 00 00 55 55

Byte #		Name			Value		Description
0 - 3		SN[0-3]			012376ab	part of the serial number
4 - 7		RevNum			00040500	device revision (= 4)
8 - 12		SN[4-8]			0c8fb7bdee	part of the serial number
13			Reserved		55			set by Atmel (55: First 16 bytes are unlocked / special case.)
14			I2C_Enable		01			SWI / I2C (1: I2C)
15			Reserved		00			set by Atmel
16			I2C_Address		c8			default I2C address
17			RFU				00			reserved for future use; must be 0
18			OTPmode			55			55: consumption mode, not supported at this time
19			SelectorMode	00			00: Selector can always be written with UpdateExtra command.
20			slot  0, read	8f			8: Secret. f: Does not matter.
21			slot  0, write	80			8: Never write. 0: Does not matter.
22			slot  1, read	80			8: Secret. 0: CheckMac copy
23			slot  1, write	a1			a: MAC required (roll). 1: key id
24			slot  2, read	82			8: Secret. 2: Does not matter.
25			slot  2, write	e0			e: MAC required (roll) and write encrypted. 0: key id
26			slot  3, read	a3			a: Single use. 3: Does not matter.
27			slot  3, write	60			6: Encrypt, MAC not required (roll). 0: Does not matter.
28			slot  4, read	94			9: CheckOnly. 4: Does not matter.
29			slot  4, write	40			4: Encrypt. 0: key id
30			slot  5, read	a0			a: Single use. 0: key id
31			slot  5, write	85			8: Never write. 5: Does not matter.
32			slot  6, read	86			8: Secret. 6: Does not matter.
33			slot  6, write	40			4: Encrypt. 0: key id
34			slot  7, read	87			8: Secret. 7: Does not matter.
35			slot  7, write	07			0: Write. 7: Does not matter.
36			slot  8, read	0f			0: Read. f: Does not matter.
37			slot  8, write	00			0: Write. 0: Does not matter.
38			slot  9, read	89			8: Secret. 9: Does not matter.
39			slot  9, write	f2			f: Encrypt, MAC required (create). 2: key id
40			slot 10, read	8a			8: Secret. a: Does not matter.
41			slot 10, write	7a			7: Encrypt, MAC not required (create). a: key id
42			slot 11, read	0b			0: Read. b: Does not matter.
43			slot 11, write	8b			8: Never Write. b: Does not matter.
44			slot 12, read	0c			0: Read. c: Does not matter.
45			slot 12, write	4c			4: Encrypt, not allowed as target. c: key id
46			slot 13, read	dd			d: CheckOnly. d: key id
47			slot 13, write	4d			4: Encrypt, not allowed as target. d: key id
48			slot 14, read	c2			c: CheckOnly. 2: key id
49			slot 14, write	42			4: Encrypt. 2: key id
50			slot 15, read	af			a: Single use. f: Does not matter.
51			slot 15, write	8f			8: Never write. f: Does not matter.
52			UseFlag 0		ff			8 uses
53			UpdateCount 0	00			count = 0
54			UseFlag 1		ff			8 uses
55			UpdateCount 1	00			count = 0
56			UseFlag 2		ff			8 uses
57			UpdateCount 2	00			count = 0
58			UseFlag 3		1f			5 uses
59			UpdateCount 3	00			count = 0
60			UseFlag 4		ff			8 uses
61			UpdateCount 4	00			count = 0
62			UseFlag 5		1f			5 uses
63			UpdateCount 5	00			count = 0
64			UseFlag 6		ff			8 uses
65			UpdateCount 6	00			count = 0
66			UseFlag 7		ff			8 uses
67			UpdateCount 7	00			count = 0
68 - 83		LastKeyUse		1fffffffffffffffffffffffffffffff
84			UserExtra
85			Selector		00			Pause command with chip id 0 leaves this device active.
86			LockValue		55			OTP and Data zones are not locked.
87			LockConfig		55			Configuration zone is unlocked.


slot summary:
Slot 1 is parent key and slot 1 is child key (DeriveKey-Roll).
Slot 2 is parent key and slot 0 is child key (DeriveKey-Roll).
Slot 3 is parent key and child key has to be given in Param2 (DeriveKey-Roll).
Slots 4, 13, and 14 are CheckOnly.
Slots 5 and 15 are single use.
Slot 8 is plain text.
Slot 10 is parent key and slot 10 is child key (DeriveKey-Create).
Slot 12 is not allowed as target.
 * \return status of the operation
 */
uint8_t ecc108e_read_config_zone(void)
{
#if 0
	// declared as "volatile" for easier debugging
	volatile uint8_t ret_code;
	
	uint16_t config_address;
	
	// Make the command buffer the size of the Read command.
	uint8_t command[READ_COUNT];

	// Make the response buffer the size of the maximum Read response.
	uint8_t response[READ_32_RSP_SIZE];
	
	// Use this buffer to read the last 24 bytes in 4-byte junks.
	uint8_t response_read_4[READ_4_RSP_SIZE];
	
	uint8_t *p_response;

	ecc108p_init();

	ecc108p_set_device_id(ECC108_CLIENT_ADDRESS);
	
	// Read first 32 bytes. Put a breakpoint after the read and inspect "response" to obtain the data.
	ret_code = ecc108c_wakeup(response);
	if (ret_code != ECC108_SUCCESS)
		return ret_code;
		
	memset(response, 0, sizeof(response));
	config_address = 0;
	ret_code = ecc108m_read(command, response, ECC108_ZONE_CONFIG | READ_ZONE_MODE_32_BYTES, config_address);
	ecc108p_sleep();
	if (ret_code != ECC108_SUCCESS)
		return ret_code;
		
	// Read second 32 bytes. Put a breakpoint after the read and inspect "response" to obtain the data.
	memset(response, 0, sizeof(response));
	ret_code = ecc108c_wakeup(response);
	if (ret_code != ECC108_SUCCESS)
		return ret_code;

	config_address += ECC108_ZONE_ACCESS_32;
	memset(response, 0, sizeof(response));
	ret_code = ecc108m_read(command, response, ECC108_ZONE_CONFIG | READ_ZONE_MODE_32_BYTES, config_address);
	ecc108p_sleep();
	if (ret_code != ECC108_SUCCESS)
		return ret_code;
		
	// Read last 24 bytes in six four-byte junks.
	memset(response, 0, sizeof(response));
	ret_code = ecc108c_wakeup(response);
	if (ret_code != ECC108_SUCCESS)
		return ret_code;
	
	config_address += ECC108_ZONE_ACCESS_32;
	response[ECC108_BUFFER_POS_COUNT] = 0;
	p_response = &response[ECC108_BUFFER_POS_DATA];
	memset(response, 0, sizeof(response));
	while (config_address < 88) {
		memset(response_read_4, 0, sizeof(response_read_4));
		ret_code = ecc108m_read(command, response_read_4, ECC108_ZONE_CONFIG, config_address);
		if (ret_code != ECC108_SUCCESS) {
			ecc108p_sleep();
			return ret_code;
		}
		memcpy(p_response, &response_read_4[ECC108_BUFFER_POS_DATA], ECC108_ZONE_ACCESS_4);
		p_response += ECC108_ZONE_ACCESS_4;
		response[ECC108_BUFFER_POS_COUNT] += ECC108_ZONE_ACCESS_4; // Update count byte in virtual response packet.
		config_address += ECC108_ZONE_ACCESS_4;
	}	
	// Put a breakpoint here and inspect "response" to obtain the data.
	ecc108p_sleep();

	return ret_code;
#else
	
#   ifdef ECC108_I2C
	uint8_t device_id_write = 0xC0;
	uint8_t device_id_read = device_id_write + 1;
	uint8_t sleep_flag = 1;
#   endif

	uint8_t response_wakeup[ECC108_RSP_SIZE_MIN];
	uint8_t ret_status = ECC108_SUCCESS;
	
	ecc108p_init();
	
#   ifdef ECC108_I2C

	ecc108p_set_device_id(device_id_write);

	while (1) {
		
		// Generate wakeup pulse.
		ecc108p_wakeup();
		
		// Read wakeup response.
		i2c_send_start();
		ret_status = i2c_send_bytes(1, &device_id_read);
		if (ret_status == I2C_FUNCTION_RETCODE_SUCCESS)
			i2c_receive_bytes(sizeof(response_wakeup), response_wakeup);
		i2c_send_stop();

		if (ret_status == I2C_FUNCTION_RETCODE_SUCCESS) {
			// Send Sleep flag.
			i2c_send_start();
			ret_status = i2c_send_bytes(1, &device_id_write);
			if (ret_status == I2C_FUNCTION_RETCODE_SUCCESS)
				(void) i2c_send_bytes(1, &sleep_flag);
			i2c_send_stop();
		}
		delay_ms(10);
	}

#   else
	
	ecc108p_set_device_id(0);

	while (1) {
		// Generate wakeup pulse.
		ecc108p_wakeup();

		// Read wakeup response.
		ret_status = ecc108p_receive_response(sizeof(response_wakeup), response_wakeup);
		if (ret_status != ECC108_SUCCESS)
			continue;
			// Send Sleep flag.

		// Send DevRev command.
		ecc108p_sleep();

		delay_ms(100);
	}		
#   endif
	
	return 0;	
#endif
}
