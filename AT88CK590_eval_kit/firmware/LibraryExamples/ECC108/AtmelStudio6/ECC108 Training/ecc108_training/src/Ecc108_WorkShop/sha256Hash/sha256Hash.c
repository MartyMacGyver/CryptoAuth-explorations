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

#include <asf.h>                                       // definitions and declarations for the Atmel Software Framework
#include "sha204_example_led.h"                        // definitions of LED utility functions
#include "ecc108_examples.h"                           // definitions and declarations for example functions
#include "sha204_helper.h" 
#include "printUtils.h"
#include "vt100.h"


void ProcessMsgHash(unsigned char n, unsigned char *Message) {
   uint8_t digest_hash[MAC_RSP_SIZE];
   char i;
   
   i = sizeof(Message);
	  
   printf("\nMessage %d Bytes:\r\n", n);
   ShowBuffer(Message, n);

   // Calculate HASH using a software SHA256 
   sha204h_calculate_sha256(n, Message, digest_hash);
   
   printf("Digest-Hash 32 Bytes:\n\r");
   ShowBuffer(digest_hash, 32);  // Display data in terminal
  	
   printf("===========================================================\n\n\r"); 

}   	
  


void sha256Hash(void)
{
	uint8_t Message[128];
	uint8_t c,i;

// **********************************************************************************
// **  STEP-1   Enter Phrase and use a software SHA256 to generate 
// **           the HASH of a given message
// **********************************************************************************

	while (1) {
	
		i = 0;	
		printf("Enter Text: ");
    
		// Capture terminal text
		 do {
			 c = getchar();
			 Message[i++] = c;
		   } while (c != '\n');
	   
		 // Replace \n to NULL
		 Message[i-1] = 0;  
	    
		 // Print the Hash
		 ProcessMsgHash(i-1, Message);
	 
// **********************************************************************************
// **  TODO-1   Change the previous Message by just 1 character, calculate
// **           and display the HASH values
// **********************************************************************************
	 
  
   }  

}

