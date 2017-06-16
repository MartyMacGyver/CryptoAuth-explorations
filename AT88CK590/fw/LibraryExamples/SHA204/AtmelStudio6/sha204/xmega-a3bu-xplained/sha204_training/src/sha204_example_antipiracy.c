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
 *  \brief  Application Example that Demonstrates Privacy Prevention Using 
 *          a Set of Fixed Challenge / Response Pairs That Help Obfuscating
 *          Code in a Sophisticated Fashion.
 *  \author Atmel Crypto Products
 *  \date   August 22, 2012
*/
#include <string.h>                              // memset(), memcmp(), memcpy()
#include <asf.h>                                 // definitions and declarations for the Atmel Software Framework
#include "sha204_example_led.h"                  // definitions of LED utility functions
#include "sha204_examples.h"                     // definitions and declarations for example functions
#include "sha204_example_antipiracy.h"           // definitions and declarations for this module


/** \brief This function performs a fixed challenge / response authentication.
 *  \param[in] challenge pointer to 32-byte challenge
 *  \param[out] response pointer to 32-byte response
 *  \param[in] key_id what key slot (0..15) to use
 *  \return status of the operation
 */
static uint8_t sha204e_challenge_response(uint8_t *challenge, uint8_t *response, uint16_t key_id, uint8_t compare)
{
	uint8_t ret_code;
	uint8_t i;
	struct sha204_command_parameters command_args;
	static uint8_t wakeup_response[SHA204_RSP_SIZE_MIN];
	static uint8_t tx_buffer[MAC_COUNT_LONG];
	static uint8_t rx_buffer[MAC_RSP_SIZE];
	
	// A strange compiler behavior overrides the pointer "response".
	// Assigning this parameter to a local parameter prevents this override.
	uint8_t *response_local = response;

	// The following command sequence wakes up the device, issues a Mac command
	// using the Command Marshaling layer, and puts the device to sleep.
	// At the end of the function the actual Mac response is compared with the
	// expected one and the result of the comparison is returned.
	
	// Wake up the device and receive the wakeup response.
	memset(wakeup_response, 0, sizeof(wakeup_response));
	ret_code = sha204c_wakeup(wakeup_response);
	if (ret_code != SHA204_SUCCESS) {
		(void) sha204p_sleep();
		return ret_code;
	}			
		
	// Issue a Mac command.
	memset(rx_buffer, 0, sizeof(rx_buffer));
	command_args.op_code = SHA204_MAC;
	command_args.param_1 = MAC_MODE_CHALLENGE;   
	command_args.param_2 = key_id;        // i.e. slot number
	command_args.data_len_1 = MAC_CHALLENGE_SIZE;
	command_args.data_1 = (uint8_t *) challenge;
	command_args.data_len_2 = 0;
	command_args.data_2 = NULL;
	command_args.data_len_3 = 0;
	command_args.data_3 = NULL;
	command_args.tx_size = sizeof(tx_buffer);
	command_args.tx_buffer = tx_buffer;
	command_args.rx_size = sizeof(rx_buffer);
	command_args.rx_buffer = rx_buffer;
		
	ret_code = sha204m_execute(&command_args);

	// Put device to sleep.
	sha204p_sleep();

	if (ret_code != SHA204_SUCCESS)
		return ret_code;
		
	// Compare the Mac response with the expected one.
	// We finish the comparison, even if it fails, to prevent a timing attack.
	if (compare == SHA204_VERIFY_RESPONSE) {
		for (i = SHA204_BUFFER_POS_DATA; i < MAC_CHALLENGE_SIZE + SHA204_BUFFER_POS_DATA; i++)
			ret_code |= (rx_buffer[i] == *(response_local + i) ? SHA204_SUCCESS : SHA204_FUNC_FAIL);
	}
	else
		memcpy(response_local, &rx_buffer[SHA204_BUFFER_POS_DATA], MAC_CHALLENGE_SIZE);
	
	return ret_code;
}


/** 
 * \brief  chaining function #1
 *         This function generates a response to a MAC challenge that will be used
 *         in subsequent MAC challenges (chained challenge / response pairs).
 *         Failure does nothing immediate, but the chain will go invalid.
 *         This function also calculates 5 factorial for obfuscation.
 * \param[in] challenge pointer to challenge data
 * \param[out] response pointer to MAC (message authentication code)
 * \return factorial 5
 */
int sha204e_generate_mac_response_1(uint8_t *challenge, uint8_t *response)
{
	uint16_t accumulator = 0;
	int i;
	sha204e_challenge_response(challenge, response, 0x00, SHA204_COPY_RESPONSE);	
	for (i = 0; i < 6; i++)
		accumulator += i;
	//Resp[4]++;  // this breaks the chain if not commented out.
	return accumulator;
}


/** 
 * \brief chaining function #2
 * This function generates a MAC response.
 * It is supposed to take the response from sha204e_generate_mac_response_1 as a challenge.
 * It also constructs a string for obfuscation.
 * \param[in] challenge pointer to challenge data
 * \param[out] response pointer to MAC
 * \param[in] address_buffer_size size of address buffer
 * \param[out] address pointer to a person's address string
 * \return status of second MAC verification
 */
int sha204e_generate_mac_response_2(uint8_t *challenge, uint8_t *response, uint8_t address_buffer_size, char *address)
{
	int status = sha204e_challenge_response(challenge, response, 0x00, SHA204_COPY_RESPONSE);
	const char *name = "Fred Williams\n";
	const char *street = "1325 Fillmore Place\n";
	const char *city = "Colorado Springs";
	if (strlen(name) + strlen(street) + strlen(city) >= address_buffer_size)
		return status;
		
	strcpy((char *) address, name);
	strcat((char *) address, street);
	strcat((char *) address, city);

	return status;
}

/** 
 * \brief chaining function #3
 * This function generates a MAC response.
 * It is supposed to take the response from sha204e_generate_mac_response_2 as a challenge
 * if the response was correct. Failure does something nasty to be named later.
 * \param[in] challenge pointer to challenge data
 * \param[out] response pointer to MAC
 * \return status of third MAC verification
 */
int sha204e_generate_mac_response_3(uint8_t *challenge, uint8_t *response)
{
	uint16_t i;
	static uint8_t expected_response[] = {
		0xb0, 0x2d, 0x9c, 0xa7, 0xe8, 0xb8, 0x4d, 0x4b, 0x51, 0xbc, 0x0c, 0x04, 0x0c, 0x57, 0x43, 0xbb,
		0x85, 0x57, 0x15, 0x58, 0x1a, 0xa8, 0xa8, 0x7d, 0x2e, 0xac, 0xa0, 0x8f, 0x94, 0xd0, 0x19, 0xa2};				
	int status = sha204e_challenge_response(challenge, response, 0x00, SHA204_COPY_RESPONSE);
	
	// This loop is designed to take the same time, whether response and expected response match or not.
	for (i = 0; i < sizeof(expected_response); i++) {
		if (response[i] != expected_response[i])
			status |= 0xF0;
		else
			status |= 0x00;
	}			
	return status;
}


/** 
 * \brief This function checks for a correct MAC response.
 *        Challenge and response are on the stack and such exist only during
 *        the execution of this function. The short time the challenge / response
 *        pair exists in RAM makes it very difficult for an adversary to poke it.
 * \return status of the check
 */
int sha204e_authenticate()
{
	// The correct response to the generated challenge is
	/*
	0x23,																						    //Size
	0x76, 0x7e, 0x5a, 0x90, 0x49, 0x45, 0x3b, 0xf3, 0x19, 0x6c, 0xc1, 0x86, 0x59, 0xe9, 0xec, 0xed, // MAC
	0x49, 0xa3, 0xc4, 0x86, 0x22, 0x0c, 0xdf, 0x7b, 0xd1, 0x94, 0x0f, 0x48, 0xbe, 0xc4, 0xd1, 0x47,
	0xe5, 0x85                                                                                      // CRC
	*/	
	static uint8_t expected_response[35];  // expected response buffer
	static uint8_t challenge[32];          // generated challenge buffer
	static uint8_t mask1[] = {
		0xdb, 0x66, 0x77, 0xbc, 0x12, 0xb6, 0x84, 0x93, 0x35, 0xf2, 0x32, 0x81, 0x4f, 0x21, 0x96, 0x2a, 
		0x61, 0x94, 0x32, 0x8b, 0xa2, 0x81, 0x49, 0x3e, 0x59, 0xb7, 0x61, 0x40, 0xc3, 0xea, 0x05, 0x90, 
		0xb8, 0x04, 0xd2, 0xc2, 0x99, 0x9d, 0x39, 0x04, 0x15, 0xe8, 0xad, 0xf3, 0xc4, 0x66, 0x7b, 0x03,

		// The two lines below (32 bytes) are the data from which an expected response is calculated.
		0xd3, 0x08, 0x9f, 0xfa, 0x3c, 0x5f, 0xa5, 0xe3, 0xf1, 0xf9, 0x2d, 0x9f, 0x04, 0xb4, 0x7f, 0xa4,
		0x80, 0x95, 0x2c, 0x20, 0x14, 0x65, 0x30, 0xe4, 0xe0, 0x0d, 0x2b, 0x96, 0x31, 0x6a, 0xb5, 0xf1,

		0x76, 0xeb, 0xe0, 0x32, 0x91, 0xd3, 0x08, 0x9f, 0xfa, 0x3c, 0x5f, 0xa5, 0xe3, 0xf1, 0xf9, 0x2d, 
		0x9f, 0x04, 0x78, 0xb4, 0x7f, 0xa4, 0x80, 0x95, 0x2c, 0x20, 0x14, 0x65, 0x30, 0xe4, 0xe0, 0x0d, 
		0x2b, 0x96, 0x31, 0x6a, 0xb5, 0xf1, 0xa5, 0x76, 0xc5, 0x6a, 0x75, 0x1a, 0x9e, 0x10, 0xe8, 0x95
	};
	static uint8_t mask2[32] = {
		0xa5, 0x76, 0xc5, 0x6a, 0x75, 0x1a, 0x9e, 0x10, 0xe8, 0x95, 0xec, 0x19, 0x5d, 0x5d, 0x93, 0x49, 
		0xc9, 0x36, 0xe8, 0xa6, 0x36, 0x69, 0xef, 0x9f, 0x31, 0x99, 0x24, 0xde, 0x8f, 0xae, 0x64, 0xb6};
	uint8_t authentication_status;
	int i, j, k;
		
	// After this, the de-compiler must track pointers to pointers, not just function calls.
	// This is another place that ten or a hundred pointers can come and go.
	sha204e_misdirected_function_pointer = sha204e_challenge_response;
	
	// This goes on the stack. It disappears on exiting this function.
	// It regenerates each time this function is executed.
    for (i = 128, j = 0, k = 128; j < 32; i++, j++, k--)
		challenge[j] = (j ^ k) - i;
	
	/* this builds the expected response to the generated challenge  */
	expected_response[0] = 0x23;
	for (i = 0; i < 32; i++)
	{
		expected_response[i + 1] = mask1[i + 48] ^ mask2[i];
	}
	// expected_response[30] and [31] don't matter.
	
	// Run a fixed challenge / response with a function pointer.
	authentication_status = sha204e_misdirected_function_pointer(challenge, expected_response, 0x00, SHA204_COPY_RESPONSE);
				
	// Clean up the stack.
	memset(expected_response, 0xFF, sizeof(expected_response));
	memset(challenge, 0xCE, sizeof(challenge));
	sha204e_misdirected_function_pointer = NULL;
	return authentication_status;
}


/** \brief This function shows how to use the ATSHA204 device to prevent
 *         - in order of complexity - firmware / software piracy.
 *         This function first runs an authentication that picks one challenge / response pair that is randomly chosen from
 *         eleven pairs. Success makes the LED's blink slowly, failure fast. It then runs a more complex 
 *         authentication where the response from the first challenge is used for the next challenge. 
 *         In this example, this chaining is done three times. At the end of this function, the overall status is 
 *         shown by slowly or fast blinking LED's.
 * \return result (0: success, otherwise failure)
 */
uint8_t sha204e_antipiracy(void)
{
	int i = 0xFF;                       // index variable for general use
	int status;                         // will be zero for successful responses
	uint16_t status_total = 0x0000;     // tracks whether earlier responses failed
	int challenge_array_index = 0x01;   // variable that gets pseudo-random inputs for pick-one challenge / response pair
	static uint8_t response_buffer[32]; // buffer to hold the responses from chained challenges
	uint8_t eeprom_buffer;              // buffer for eeprom byte reads
	uint16_t random_seed;               // random seed. Will be incremented and written back.	
	struct nvm_device_serial serial;    // storage for serial number
	static char person[64];             // dummy result for sha204e_generate_mac_response_2
	
	/* simple challenge response
		This can be scattered throughout code, using different challenges and responses. It has the advantages of being simple,
		easy to implement, and it will force multiple efforts to find all the challenge / response pairs.
	*/	
	// ASCII for "Here comes the client challenge.";
	static uint8_t challenge[] = {
		0x48, 0x65, 0x72, 0x65,  0x20, 0x63, 0x6f, 0x6d,  0x65, 0x73, 0x20, 0x74,  0x68, 0x65,
		0x20, 0x63,  0x6c, 0x69, 0x65, 0x6e,  0x74, 0x20, 0x63, 0x68,  0x61, 0x6c, 0x6c, 0x65, 
		0x6e, 0x67, 0x65, 0x2e};
	static uint8_t expected_response[] = {
		0x23, // count of Mac response packet: count(1) + MAC(32) + CRC(2)
		0x13, 0xE0, 0xCE, 0xA1, 0xA0, 0x12, 0xC6, 0x18, 0x73, 0x30, 0x5E, 0x6A, 0x68, 0xCA, 0x92, 0xD6,
		0x4E, 0x40, 0x81, 0x46, 0x29, 0x2B, 0x8B, 0x82, 0xFB, 0xC4, 0x1E, 0x2D, 0x0D, 0x54, 0x60, 0x36,
	    0x0C, 0x89};
	
	// Initialize system clock.
	sysclk_init();

	// Initialize the board.
	board_init();

	// eeprom random seed storage handling
    // Read the Random function seed from eeprom if present or regenerate if not present.
	eeprom_buffer = nvm_eeprom_read_byte(16);
	random_seed = eeprom_buffer << 8;           // upper 8
	eeprom_buffer = nvm_eeprom_read_byte(17);
	random_seed |= eeprom_buffer;               // lower 8
	if (random_seed == 0xffff)  // eeprom has been erased.
	{
		nvm_read_device_serial(&serial);   // Use the x and y coordinates from the serial number as a seed.
		random_seed = serial.coordx0 << 8;
		random_seed = random_seed | serial.coordy0;	
	}
	// Increment and write back for the next run.
	random_seed++;
	eeprom_buffer = (uint8_t)(random_seed & 0x00FF);
	nvm_eeprom_write_byte(17, eeprom_buffer);
	eeprom_buffer = (uint8_t)(random_seed >> 8);
	nvm_eeprom_write_byte(16, eeprom_buffer);
	srand(random_seed);
	random_seed = rand();
	random_seed = rand();  // Now, the random quality of the random seed is good enough for this example.

	status = sha204e_challenge_response(challenge, expected_response, 0x00, SHA204_VERIFY_RESPONSE);
	status_total = status;        
	
	// Slow blink for success (0x00), fast blink otherwise. Three times for simple challenge / response.
	display_status_lib(status);
	display_status_lib(status);    
	display_status_lib(status);    
	// end simple challenge / response
	
	// pick-one challenge response
	/* The challenge_array_index picks one of 11 challenge response pairs, and optionally the slot to use.
	* In the real thing, the array would be one to two orders of magnitude larger, and these would be scattered
	* throughout the code, in places where a slight decrease in performance would be harmless.
	* Someone monitoring the bus will see a large and random set of challenge / response pairs, and if the seeding 
	* is correct, an unpredictable sequence.
	*/	
	challenge_array_index = random() % 11;
	
	status = sha204e_challenge_response(challenge_set_array[challenge_array_index].challenge, 
	                                           challenge_set_array[challenge_array_index].response, 
											   challenge_set_array[challenge_array_index].key_id,
											   SHA204_VERIFY_RESPONSE);
	if (status_total == 0x00)
		status_total = status;  // If simple challenge failed, propagate error down.
		
	/*
		While no binary is impossible to reverse engineer, it can be made non-trivial.
		This example SHOULD NOT actually be a function as it is here. It should have
		the code scattered in among the other functional code in the application.
	*/	
	// This sets to the function to call only inside the function.
	sha204e_misdirected_function_pointer = NULL;
	// Should be set back up above in a normal application.
	i = sha204e_authenticate();
	if (status_total == 0x00)  // Propagate earlier errors, if any
		status_total = i;      // which may well be 0x00
		
	// chaining
	sha204e_generate_mac_response_1(challenge, response_buffer);	
	sha204e_generate_mac_response_2(response_buffer, response_buffer, sizeof(person), person);
	status = sha204e_generate_mac_response_3(response_buffer, response_buffer);	
	if (status_total == 0x00)
		// If an earlier routine failed, display that error.
		status_total = status;
		
	// Display success of whole routine.
	while (true) {
		display_status_lib(status_total);    // Slow blink for success (0x00), fast blink otherwise.		
	}	
	  
	return 0x00;
}
