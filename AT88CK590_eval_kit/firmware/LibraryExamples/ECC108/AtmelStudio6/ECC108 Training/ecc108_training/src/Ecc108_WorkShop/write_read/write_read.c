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
*  \brief  Application example demonstrates Plain text Read/Write
*
*  \author Rocendo Bracamontes rocendo.bracamontes@atmel.com
*  \date   January 2013
*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <asf.h>                                 // definitions and declarations for the Atmel Software Framework
#include "sha204_example_led.h"                  // definitions of LED utility functions
#include "ecc108_examples.h"                     // definitions and declarations for example functions
#include "printUtils.h"
#include "vt100.h"


uint8_t sha204_write_plain_text(uint8_t slotId, uint8_t *Buffer) {
	uint8_t ret_code;
	
	// We could re-use rx_buffer for the status response, but we use a separate
	// buffer for easier debugging.
	uint8_t status_response[SHA204_RSP_SIZE_MIN];
	uint8_t tx_buffer[WRITE_COUNT_LONG];
	uint8_t rx_buffer[READ_32_RSP_SIZE];
	struct sha204_write_parameters args_write;
	
	// AVR TWI Driver
	args_write.tx_buffer = tx_buffer;
	args_write.rx_buffer = status_response;
	args_write.zone =  SHA204_ZONE_DATA | READ_ZONE_MODE_32_BYTES;;
	args_write.address = slotId * SHA204_KEY_SIZE;   // calculate offset for slotId
	args_write.new_value = Buffer;
	args_write.mac = NULL;  // Since this is a "plain text" write, a Message Authentication Code (MAC) is not required
	
	// Execute SHA204 Write Command
	ret_code = sha204m_write(&args_write);
    
    return ret_code;
	
}



uint8_t sha204_read_plain_text(uint8_t slotId, uint8_t *RxBuffer) {
	uint8_t ret_code;
	uint8_t i;
	
	// We could re-use rx_buffer for the status response, but we use a separate
	// buffer for easier debugging.
	uint8_t status_response[SHA204_RSP_SIZE_MIN];
	uint8_t tx_buffer[WRITE_COUNT_LONG];
	uint8_t rx_buffer[READ_32_RSP_SIZE];
	struct sha204_write_parameters args_read;
	
	
	// AVR TWI Driver
	args_read.tx_buffer = tx_buffer;
	args_read.rx_buffer = rx_buffer;
	args_read.zone =  SHA204_ZONE_DATA | READ_ZONE_MODE_32_BYTES;;
	args_read.address = slotId * SHA204_KEY_SIZE;   // calculate offset for slotId

    // Execute SHA204 Read Command
    ret_code = sha204m_read(&args_read);	 
 
	for(i = 0; i < 32; i++) {
		RxBuffer[i] = args_read.rx_buffer[SHA204_BUFFER_POS_DATA+i];
	}
   	
	return ret_code;			
}


void write_read(void)
{
	uint8_t ret_code;
	uint8_t slotId;
    uint8_t Message[32];
    uint8_t c,i;
	uint8_t RxBuffer[READ_32_RSP_SIZE];;

	
	while(1) {

// **********************************************************************************
// **  STEP-1   Add your own text to write, 32 Chars
// **********************************************************************************

	  // Data to write to EE Slot inside ATSHA204 (Each Slot is 32 Bytes long)
	  uint8_t *Message = (uint8_t *) "ADD YOUR TEXT HERE 32 CHARS_ _ _";
 
	
// **********************************************************************************
// **  STEP-2   Select the Slot-Id, see comments below
// **********************************************************************************
   
	// The ATSHA204 has been pre-configured to allow
	// plain-text Reads/Writes to Slots 3-10
	// Slots 0-2, 11-14, are readable, but only allow encrypted writes
	// Slot 15 is secret (non readable nor writable)
	
     slotId = 0;
 
 
	  // Execute Wake-up SHA204 command
	  ret_code = sha204c_wakeup(RxBuffer);
   
// **********************************************************************************
// **  STEP-3   Send the write command to SHA204
// **********************************************************************************
 
		ret_code = sha204_write_plain_text(slotId, Message);
		if (ret_code != SHA204_SUCCESS) {
			printf("\nWrite Plain Text to SlotId %X FAILED\n", slotId);
		} 
		else {
			printf("Write Plain Text to SlotId %X SUCCEED\n", slotId);
		}
	

// **********************************************************************************
// **  STEP-4   Read the SlotId
// **********************************************************************************
		ret_code = sha204_read_plain_text(slotId, RxBuffer);
		if (ret_code != SHA204_SUCCESS) {
			printf("Read Plain Text from SlotId %X FAILED\n", slotId);
		} 
		else {
			printf("Read Plain Text from SlotId %X SUCCEED\n", slotId);
		}
	

// **********************************************************************************
// **  STEP-5   Display the Write and Read data
// **********************************************************************************

		printf("Write Data to SlotId %X\n", slotId);
		ShowBuffer(Message, 32); 
   
		printf("Read Data from SlotId %X\n", slotId);
		ShowBuffer(&RxBuffer, 32);
   
		// Compare read data with original buffer
		ret_code = memcmp(Message, &RxBuffer, sizeof(Message));
	
		if (ret_code != 0) {
			printf("Read/Write Example Write to Slot-%d ** FAILED ** \r\n\n", slotId);

		} 
		else {
			printf("Read/Write Example Write to Slot-%d -- SUCCEED -- \r\n\n", slotId);
		}
	
	  
		// After all operations, send the sha204 to sleep  
		sha204p_sleep();
	
	

// **********************************************************************************
// **  TODO-1   Write a routine to Write/Read all Slots
// **           and verify the read/write permissions
// **********************************************************************************


// YOUR CODE HERE 

 
		// pause here to retry
		printf("Press enter to try again:");
  		// Capture terminal text
  		do {
	  		c = getchar();
	  		Message[i++] = c;
  		} while (c != '\n');  	 	


		printf("\n\n=============================================\n\n");
	}
}
