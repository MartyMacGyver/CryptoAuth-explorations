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
 *  \author Rocendo Bracamontes rocendo.bracamontes@atmel.com
  *  \date  Jan 29 2013
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <asf.h>                                       // definitions and declarations for the Atmel Software Framework
#include "sha204_example_led.h"                        // definitions of LED utility functions
#include "ecc108_examples.h"                           // definitions and declarations for example functions
#include "fixed_challenge_response.h"   // definitions and declarations for this module
#include "printUtils.h"

/** \brief This function serves as an example for a fixed challenge / response authentication.
 * \return status of the operation
 */


void fixed_challenge_response(void)
{
	uint8_t ret_code;
	uint8_t Message[32];
	uint8_t c,i;
	uint8_t wakeup_response[SHA204_RSP_SIZE_MIN];
	uint8_t tx_buffer[MAC_COUNT_LONG];
	uint8_t rx_buffer[MAC_RSP_SIZE];
	struct sha204_nonce_parameters args_nonce;
	struct sha204_mac_parameters args_mac;
	

	while(1) {	
	
// **********************************************************************************
// **  STEP-1   The Challenge. This is a --known-- parameter (NumIn) for the 
// **           NONCE command as part of a Fixed/Challenge response. 
// **           It is called fixed, because we pre-calculate the response based on
// **           a fixed-known "NumIn" 
// **********************************************************************************
	
		static uint8_t *challenge = (uint8_t *) "A --KNOWN-- CHALLENGE PARAMETER "; // 32 Bytes long
	
		printf("Fixed Known Challenge:\r\n");
		ShowBuffer(challenge, 32);  // Display data in terminal
	


// **********************************************************************************
// **  STEP-2   The Response. This is a --known-- pre-calculated output 
// **           response from the MAC command, as part of a Fixed/Challenge response
// **           with a --Specific-- SlotId also called Key_Id
// **********************************************************************************
		
		uint8_t expected_response[] = { 
			0x88, 0xbd, 0x8c, 0xaf, 0x3f, 0x6c, 0x97, 0x89,
			0x29, 0x88, 0x4f, 0x2f, 0xf0, 0x58, 0x20, 0x5a,
			0x1a, 0x5c, 0xd1, 0x27, 0xa4, 0xa8, 0xfe, 0xf3,
			0xe4, 0x30, 0x8f, 0xad, 0x17, 0xdc, 0x7e, 0x88
		};

		printf("Fixed Known Response:\r\n");
		ShowBuffer(&expected_response[0], 32);  // Display data in terminal

		// The following command sequence wakes up the device, issues a Nonce
		// and a MAC command using the Command Marshaling layer, and puts the 
		// device to sleep. At the end of the function the MAC response is 
		// compared with the expected one and the result of the comparison is 
		// displayed using the on-board LED's.
				
		// Wake up the device and receive the wakeup response.
		ret_code = sha204c_wakeup(wakeup_response);
		if (ret_code != SHA204_SUCCESS) {
			printf("WakeUp	FAILED \n\r");
			return ;
	    }
		else {
		    printf("WakeUp	Ok\n\r");
	    }		


// **********************************************************************************
// **  STEP-3   The NONCE Command. It is issued to insert the value of NunIn 
//              parameter into the internal "TempKey" of the SHA204
//              Note the "NONCE_MODE_PASSTHROUGH" mode
// **********************************************************************************

		// NONCE commands arguments
		args_nonce.tx_buffer = tx_buffer;
		args_nonce.rx_buffer = rx_buffer;
		args_nonce.mode = NONCE_MODE_PASSTHROUGH;
		args_nonce.num_in = challenge;

		// Issue a Nonce command.
		ret_code = sha204m_nonce(&args_nonce);

		if (ret_code != SHA204_SUCCESS) {
			sha204p_sleep();
			printf("NONCE	FAILED \n\r");
			return ;
		} 
		else {
			printf("NONCE	Ok \n\r");
		}
		

// **********************************************************************************
// **  STEP-4   Select the SlotId also called Key_id to perform the MAC calculation
// **           and capture its response in the rx_buffer
// **********************************************************************************
		
		// MAC Command arguments
		args_mac.key_id = 15;  
		args_mac.tx_buffer = tx_buffer;
		args_mac.rx_buffer = rx_buffer;
		args_mac.mode = MAC_MODE_BLOCK2_TEMPKEY | MAC_MODE_SOURCE_FLAG_MATCH;
		args_mac.challenge = NULL;		

		
		
// *****************************************************************************************
// **  STEP-5  Send the MAC command, capture and compare the response with the expected one
// *****************************************************************************************

		// Issue a MAC command.
		ret_code = sha204m_mac(&args_mac);

		// Put SHA204 device to sleep.
		sha204p_sleep();

		if (ret_code != SHA204_SUCCESS) {
			printf("MAC	FAILED \n\r");
			return ;
		} 
		else {
			printf("MAC	Ok \n\r");
		}
	

		printf("\nCompare Challenge and Response:\n\n\r");
  
		//Mac response packet = 1(count) + 32(MAC) + 2(CRC)
		// skip first byte
		for (i = 0; i < 32; i++) { // MAC Response is 32 Bytes
			ret_code |= (rx_buffer[1+i] == expected_response[i] ? SHA204_SUCCESS : SHA204_FUNC_FAIL);
		}
		
		
		// Display results in terminal	
		if (ret_code == SHA204_SUCCESS) {
			 printf("Fixed Challenge/Response test --SUCCEEDED-- \n\r");
		} 
		else {
			 printf("Fixed Challenge/Response test **FAILED** \n\n\r");
			 printf("Received Response:\r\n");
			 ShowBuffer(&rx_buffer[1], 32);  // Display data in terminal
		 
		}		 		 	
	   
 		// After all operations, send the sha204 to sleep
 		sha204p_sleep();
 	
 
 // **********************************************************************************
 // **  TODO-1  Change the value of the challenge and check the response
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
