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
*  \brief  Application example demonstrates the Session Key Generation
*
*  \author Rocendo Bracamontes rocendo.bracamontes@atmel.com
*  \date   January 2013
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <asf.h>                                       // definitions and declarations for the Atmel Software Framework
#include "sha204_example_led.h"                        // definitions of LED utility functions
#include "ecc108_examples.h"                           // definitions and declarations for example functions
#include "printUtils.h"

#define KEY_ID 15

void session_key_gen(void)
{
	uint8_t Message[32];
    uint8_t c,i;

	uint8_t ret_code;
	static uint8_t wakeup_response[SHA204_RSP_SIZE_MIN];
	static uint8_t tx_buffer[MAC_COUNT_LONG];
	static uint8_t session_key[MAC_RSP_SIZE];
	static uint8_t rand_out[MAC_RSP_SIZE];
	struct sha204_random_parameters args_random;
	struct sha204_mac_parameters args_mac;
	
	while(1) {
	
		// Wake up the device and receive the wakeup response.
		ret_code = sha204c_wakeup(wakeup_response);
		if (ret_code != SHA204_SUCCESS) {
			printf("WakeUp	FAILED \n\r");
			return;
		} 
		else {		 
			printf("WakeUp	Ok\n\r");
		}	
	
	
// **********************************************************************************
// **  STEP-1   To create a Session-Key, first we want to get a Random Number
// **           to be used as input to a MAC command        
// **********************************************************************************
	
		args_random.tx_buffer = tx_buffer;
		args_random.rx_buffer = rand_out;
		args_random.mode = RANDOM_SEED_UPDATE;
	
		ret_code = sha204m_random(&args_random);		
	
		if (ret_code != SHA204_SUCCESS) {
			printf("Random	FAILED \n\r");
			return ;
		}
		else {
			printf("Random	Ok\n\n\r");
			printf("Random Number: \n\r");
			ShowBuffer(&rand_out[SHA204_BUFFER_POS_DATA], 32);  // Display data in terminal
		}
		

// **********************************************************************************
// **  STEP-2   Use the Random Number as a challenge for a MAC command
// **           in combination with a Key/Slot.
// **           The output of the MAC command is the Session-Key.
// **********************************************************************************
		
		args_mac.tx_buffer = tx_buffer;   
		args_mac.rx_buffer = session_key;     
		args_mac.mode = MAC_MODE_CHALLENGE;            
		args_mac.key_id = KEY_ID;           
		args_mac.challenge = &rand_out[SHA204_BUFFER_POS_DATA];        

		ret_code = sha204m_mac(&args_mac);
	
	
		if (ret_code != SHA204_SUCCESS) {
			printf("MAC	FAILED \n\r");
			return ;
		}
		else {
			printf("MAC	Ok\n\n\r");
			printf("Session Key Val: \n\r");
			ShowBuffer(&session_key[SHA204_BUFFER_POS_DATA], 32);  // Display data in terminal
		}
	
		sha204p_sleep();
		
		// pause here to retry
		printf("\nPress enter to try again:");
		do {
			c = getchar();
			Message[i++] = c;
		} while (c != '\n');
		printf("\n\n=============================================\n\n");
	}
} 
 
 
