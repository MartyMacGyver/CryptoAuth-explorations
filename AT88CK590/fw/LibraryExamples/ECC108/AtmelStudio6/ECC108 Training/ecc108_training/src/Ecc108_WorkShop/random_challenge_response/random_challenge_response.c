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
*  \brief  Application example demonstrates the HASH SHA256 generation
*
*  \author Rocendo Bracamontes rocendo.bracamontes@atmel.com
*  \date   January 2013
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <asf.h>                                          // definitions and declarations for the Atmel Software Framework
#include "sha204_example_led.h"                           // definitions of LED utility functions
#include "ecc108_examples.h"                              // definitions and declarations for example functions
#include "sha204_helper.h"                                // definitions and declarations for the Helper module (e.g. TempKey calculation)
#include "random_challenge_response.h"     // definitions and declarations for this module
#include "printUtils.h"

#define KEY_ID 15


/** \brief This function serves as an example for a random challenge authentication.
 * \return status of the operation
 */
void random_challenge_response(void)
{
	uint8_t ret_code;
	uint8_t wakeup_response[SHA204_RSP_SIZE_MIN];
	uint8_t tx_buffer[MAC_COUNT_LONG];
	uint8_t rand_out[MAC_RSP_SIZE];
	uint8_t mac[MAC_RSP_SIZE];
	uint8_t mac_calculated[SHA204_KEY_SIZE];
	
    uint8_t Message[32];
	uint8_t c,i;
	
	struct sha204h_temp_key temp_key;
    struct sha204_nonce_parameters args_nonce;	
    struct sha204h_nonce_in_out args_nonce_tempkey;	
	struct sha204_mac_parameters args_mac;	
	struct sha204h_mac_in_out args_mac_tempkey;
	
	

// **********************************************************************************
// **  NOTE:  The BEST and more secure solution is to use a ATSHA204 on the  
// **          HOST and CLIENT
// **          For this exercise we will use only 1 ATSHA204 and emulate the 
// **          internal operations on a ATSHA204 to demonstrate the Random Challenge/Response
// ********************************************************************************** 


	while(1) {

		 ret_code = sha204c_wakeup(wakeup_response);
	 
		 if (ret_code != SHA204_SUCCESS) {;
			 printf("WakeUp	FAILED\n\r");
			 return ;
		 } 
		 else {
			printf("WakeUp	Ok\n\r");
		 }


// **********************************************************************************
// **  STEP-1  Start with a Random NONCE command. The output of the NONCE command
// **          is a Random Number used on the calculations. The NONCE number is then
// **          stored inside the SHA204 TempKey
// **********************************************************************************
	
		// This is a value used for the NONCE command NumIn parameter, 20 bytes long
		const uint8_t *NumIn = (uint8_t *) "_HereIsTheChallenge_";

		args_nonce.tx_buffer = tx_buffer;
		args_nonce.rx_buffer = rand_out;
		args_nonce.mode = NONCE_MODE_SEED_UPDATE;
		args_nonce.num_in = (uint8_t *) NumIn;
	   
		// Issue the NONCE command
		ret_code = sha204m_nonce(&args_nonce);
		
		if (ret_code != SHA204_SUCCESS) {
			printf("NONCE	FAILED \n\r");
			return ;
		} else {
			printf("NONCE	Ok \n\r");
			printf("Random Challenge\n\r");		   
			ShowBuffer(&rand_out[1], 32);  // Display data in terminal				
		}
	
				
// **********************************************************************************
// **  STEP-2  In this case, the MAC command combines the value previously 
// **          calculated by the NONCE command and stored inside the SHA204 TempKey,
// **          and one of the Key-id/SlotId, and other data (see datasheet) to 
// **          generate the final SHA256 digest.
// **          The output of the MAC command is the Response used to compare later ..
// **********************************************************************************

		// Populate the MAC command arguments
		args_mac.tx_buffer = tx_buffer;
		args_mac.rx_buffer = mac;         // Final Digest is stored on mac buffer
		args_mac.mode = MAC_MODE_BLOCK2_TEMPKEY;
		args_mac.key_id = KEY_ID;
		args_mac.challenge = NULL;

		ret_code = sha204m_mac(&args_mac);

		if (ret_code != SHA204_SUCCESS) {
			printf("MAC	FAILED \n\r");
			return ;
		} 
		else {
			printf("MAC	Ok \n\r");
		}


// **********************************************************************************
// **  STEP-3  Next, we need to emulate the calculations inside the SHA204 using
// **          the functions from the helper library. 
// **          Using the sha204h_nonce() and the Random number generated before
// **          stored in rand_out, we can replicate the intermediate value of
// **          TempKey
// **********************************************************************************

		args_nonce_tempkey.mode = NONCE_MODE_SEED_UPDATE;
		args_nonce_tempkey.num_in = (uint8_t *) NumIn;
		args_nonce_tempkey.rand_out = &rand_out[SHA204_BUFFER_POS_DATA];
		args_nonce_tempkey.temp_key = &temp_key;
	
		ret_code = sha204h_nonce(&args_nonce_tempkey);

		if (ret_code != SHA204_SUCCESS) {;
      		printf("Emulated NONCE	FAILED \n\r");
			return ;
		}  
		else {
			printf("Emulated NONCE	Ok \n\r");
		}



// **********************************************************************************
// **  STEP-4    Emulate a MAC Command using sha204h_mac()
// **            The output of this command is the "Response" used for the
// **            Random Challenge-Response
// **            Value stored in "mac_calculated"
// **********************************************************************************


		// This is the value already stored on the "Training" SHA204 on Slot/Key 15, -->> change KEY_ID = 15 <<--
		const uint8_t *key = (uint8_t *) "CryptoAuthentication!!!!!!!!!!!!"; // 32 bytes long, Key Slot

		args_mac_tempkey.mode = MAC_MODE_BLOCK2_TEMPKEY;
		args_mac_tempkey.key_id = 15;
		args_mac_tempkey.challenge = NULL;
		args_mac_tempkey.key = (uint8_t *) key;
		args_mac_tempkey.otp = NULL;
		args_mac_tempkey.sn = NULL;
		args_mac_tempkey.response = mac_calculated;
		args_mac_tempkey.temp_key = &temp_key;

		// Emulate MAC.
		ret_code = sha204h_mac(&args_mac_tempkey);

		if (ret_code != SHA204_SUCCESS) {;
			 printf("Emulated MAC	FAILED \n\r");
			 return ;
		} 
		else {
			printf("Emulated MAC	Ok \n\n\r");	
		}
	

// **********************************************************************************
// **  STEP-5  Compare the MACs to check authenticity 
// **********************************************************************************

		// Compare the Mac response with the calculated one.
		ret_code = (memcmp(&mac[SHA204_BUFFER_POS_DATA], mac_calculated, sizeof(mac_calculated)) ? SHA204_FUNC_FAIL : SHA204_SUCCESS);

		// Display results in terminal
		if (ret_code == SHA204_SUCCESS) {
			printf("Random Challenge/Response test --SUCCEEDED-- \n\n\r");
		} 
		else {
			printf("Random Challenge/Response test **FAILED** \n\n\r");
		}

		printf("Received Response:\r\n");
		ShowBuffer(&mac[1], 32);  // Display data in terminal
		printf("Calculated Response:\r\n");
		ShowBuffer(&mac_calculated[0], 32);  // Display data in terminal
			

		// After all operations, send the sha204 to sleep
		sha204p_sleep();
		

// **********************************************************************************
// **  TODO-1  Run several times and check the Generated Challenge and Response pair.
// **          Every time they should be completely different.
// **********************************************************************************

		// pause here to retry
		printf("\nPress enter to try again:");
		// Capture terminal text
		do {
			c = getchar();
			Message[i++] = c;
		} while (c != '\n');


		printf("\n\n=============================================\n\n");
	}		
}


