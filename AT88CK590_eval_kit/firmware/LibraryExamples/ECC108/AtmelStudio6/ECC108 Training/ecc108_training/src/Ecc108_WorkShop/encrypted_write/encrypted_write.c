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
*  \brief  Application example demonstrates the Encrypted Writes
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
#include "sha204_helper.h"
#include "printUtils.h"
#include "vt100.h"

#include "encrypted_write.h"
#include "write_read.h"

void WakeUpSha(uint8_t * SHA204Response) {
	uint8_t ret_code;
	
	while ((sha204c_wakeup(&SHA204Response[0])) != SHA204_SUCCESS) {
		sha204p_idle();
	}
}	


uint8_t GetSHASerialNumber(uint8_t * pSN) {
	uint8_t ret_code;
	
	// We could re-use rx_buffer for the status response, but we use a separate
	// buffer for easier debugging.
	uint8_t status_response[SHA204_RSP_SIZE_MIN];
	uint8_t tx_buffer[WRITE_COUNT_LONG];
	uint8_t rx_buffer[READ_32_RSP_SIZE];
	struct sha204_read_parameters args_read;

	// AVR TWI Driver
	args_read.tx_buffer = tx_buffer;
	args_read.rx_buffer = rx_buffer;
	args_read.zone =  SHA204_ZONE_COUNT_FLAG ;
	args_read.address = 0;   // calculate offset for slotId

	// Execute SHA204 Read Command here
	ret_code = sha204m_read(&args_read);
	
	if (ret_code != SHA204_SUCCESS) {
		printf("Read SerialNumber FAILED\n");
	} else {
		printf("Read SerialNumber SUCCEED\n");
	}
		
	// Grab the Bytes from Serial Number // Inverse Endian for itoa functions
	*(pSN+0) = rx_buffer[12]; // SN[]
	*(pSN+1) = rx_buffer[11]; // SN[]
	*(pSN+2) = rx_buffer[10]; // SN[]
	*(pSN+3) = rx_buffer[9];  // SN[]
	*(pSN+4) = rx_buffer[4];  // SN[]
	*(pSN+5) = rx_buffer[3];  // SN[]
	*(pSN+6) = 0;
	*(pSN+7) = 0;

	
	// Special Data for Encrypted-Writes to utility-sha
	*(pSN+10) = rx_buffer[13]; // SN[8]
	*(pSN+9) = rx_buffer[2];  // SN[1]
	*(pSN+8) = rx_buffer[1];  // SN[0]
	
	return ret_code;
}



uint8_t encrypted_write_procedure(uint8_t *Data, uint8_t DataSlotNumber, uint8_t *KeyValue, uint8_t KeySlotNumber) {
    uint8_t i;
	uint8_t ret_code, param2;
    uint8_t UtilityBuff[128] = {0};
    uint8_t RandOut[MAC_RSP_SIZE];
    uint8_t TempKeyNonce[32];
    uint8_t TempKeyGendig[32];
    uint8_t NumIn[32];
    uint8_t SerialNunmber[16];
    uint8_t DataEncrypted[32];
    uint8_t InputMac[32];


	uint8_t status_response[SHA204_RSP_SIZE_MIN];
	uint8_t tx_buffer[WRITE_COUNT_LONG];
	uint8_t rx_buffer[READ_32_RSP_SIZE];
	struct sha204_nonce_parameters args_nonce;
	struct sha204_gen_dig_parameters args_gendig;	
	struct sha204_write_parameters args_write;
	
    GetSHASerialNumber(&SerialNunmber[0]);

   // Calculate SHA256-Key for NumIn
    sha204h_calculate_sha256(32, &KeyValue[0], &NumIn[0]);
 
// **********************************************************************************
// **   Create TempKey in SHA204, execute RND_NONCE
// **           
// **********************************************************************************
 
  args_nonce.tx_buffer = tx_buffer;
  args_nonce.rx_buffer = RandOut;
  args_nonce.mode = NONCE_MODE_SEED_UPDATE;
  args_nonce.num_in = (uint8_t *) NumIn;
  
  // Issue the NONCE command
    ret_code = sha204m_nonce(&args_nonce);
	
	if (ret_code != SHA204_SUCCESS) {
		printf("NONCE FAILED\n");
	} else {
		printf("NONCE SUCCEED\n");
	}  

// **********************************************************************************
// **   Gendig KeySlotNumber in SHA204 
// **
// **********************************************************************************

	args_gendig.tx_buffer = tx_buffer;   
	args_gendig.rx_buffer = rx_buffer;       
	args_gendig.zone = SHA204_ZONE_DATA;              
	args_gendig.key_id = KeySlotNumber; 
	args_gendig.other_data = NULL;  

   ret_code = sha204m_gen_dig(&args_gendig);

	if (ret_code != SHA204_SUCCESS) {
		printf("GENDIG FAILED\n");
	} else {
		printf("GENDIG SUCCEED\n");
	}   
   
// **********************************************************************************
// **   Reproduce nonce in software
// **
// **********************************************************************************
// 
// 32 RandOut
// 20 NumIn
// 1 0x16
// 1 mode
// 0x00

// Copy RandOut to utility buffer
   memcpy((char *) &UtilityBuff[0], (char *) &RandOut[SHA204_BUFFER_POS_DATA], 32);

// Copy NumIn to utility buffer
   memcpy((char *) &UtilityBuff[32], (char *) &NumIn[0], 20);

// Opcode, always 0x16
   UtilityBuff[52] = 0x16;
// Mode
   UtilityBuff[53] = 0x00;
// always 0x00
   UtilityBuff[54] = 0x00;

//Calculate Tempkey value after NONCE
   sha204h_calculate_sha256(55, &UtilityBuff[0], &TempKeyNonce[0]);


// **********************************************************************************
// **   Reproduce gendig in software
// **
// **********************************************************************************
// 32 Key
// 1 opcode
// 1 param1
// 2 param2
// 1 SN[8]
// 2 SN[0:1]
// 25 zeroes
// 32 TempKeyGendig

// Copy Key to utility buffer
   memcpy((char *) &UtilityBuff[0], (char *) &KeyValue[0], 32);

// Opcode
   UtilityBuff[32] = 0x15;
// param1
   UtilityBuff[33] = 0x02;
// param2[0]
   UtilityBuff[34] = 0x0F;
// param2[1]
   UtilityBuff[35] = 0x00;
// SN[8]
   UtilityBuff[36] = SerialNunmber[10];
// SN[0]
   UtilityBuff[37] = SerialNunmber[8];
// SN[1]
   UtilityBuff[38] = SerialNunmber[9];

// 25 bytes 0's
   memset (&UtilityBuff[39], 0, 25);

// add TempKeyNonce
   memcpy((char *) &UtilityBuff[64], (char *) &TempKeyNonce[0], 32);

   sha204h_calculate_sha256(96, &UtilityBuff[0], &TempKeyGendig[0]);


//*************************************************************************************************
//** 
//**  Whenever the input data is encrypted an authorizing input MAC is always required when
//**  writing the data zone. This MAC is computed as:
//**  SHA-256(TempKey, Opcode, Param1, Param2, SN[8], SN[0:1], <25 bytes of 0’s>, PlainTextData)
//** 
//*************************************************************************************************

// Copy TempKeyGendig to utility buffer
   memcpy((char *) &UtilityBuff[0], (char *) &TempKeyGendig[0], 32);

// Opcode
   UtilityBuff[32] = 0x12;
// param1
   UtilityBuff[33] = OP_32_BYTES | DATA_ZONE;
   param2 = (DataSlotNumber << 3) | OFFSET_BYTE_0;
// param2[0]
   UtilityBuff[34] = param2;
// param2[1]
   UtilityBuff[35] = param2 >> 8;
// SN[8]
   UtilityBuff[36] = SerialNunmber[10];
// SN[0]
   UtilityBuff[37] = SerialNunmber[8];
// SN[1]
   UtilityBuff[38] = SerialNunmber[9];

// 25 bytes 0's
   memset (&UtilityBuff[39], 0, 25);

// plaintext Data
   memcpy((char *) &UtilityBuff[64], (char *) &Data[0], 32);

   sha204h_calculate_sha256(96, &UtilityBuff[0], &InputMac[0]);



//*************************************************************************************************
//**  Encrypt Data/Count
//**
//*************************************************************************************************

   for(i=0; i<32; i++) {
	    DataEncrypted[i] = *(Data+i) ^ TempKeyGendig[i];
   }


//*************************************************************************************************
//**  Encrypted Write with MAC authentication
//**
//*************************************************************************************************

	args_write.tx_buffer = tx_buffer;
	args_write.rx_buffer = rx_buffer;
	args_write.zone =  SHA204_ZONE_DATA | READ_ZONE_MODE_32_BYTES;;
	args_write.address = DataSlotNumber * SHA204_KEY_SIZE;   // calculate offset for slotId;   
	args_write.new_value = DataEncrypted;
	args_write.mac = &InputMac[0]; 	
	
  // Execute SHA204 Write Command here
	 ret_code = sha204m_write(&args_write);
	
	if (ret_code != SHA204_SUCCESS) {
		printf("ENC-WRITE FAILED\n");
	} else {
		printf("ENC-WRITE SUCCEED\n");
	}
	
//Debug stuff
/*	
	 printf("Data Clear:\n");
	 ShowBuffer(Data, 32);

	 printf("Key Clear:\n");
	 ShowBuffer(KeyValue, 32);
*/			
	 printf("Data Encrypted: \n");
	 ShowBuffer(DataEncrypted, 32);
	  	
    return ret_code;

}


void encrypted_write(void)
{
	uint8_t ret_code;
	uint8_t slotId;
    uint8_t Message[32];
    uint8_t c,i;
	uint8_t RxBuffer[READ_32_RSP_SIZE];;

    uint8_t Key15Value[32] = {'C','r','y','p','t','o','A','u','t','h','e','n','t','i','c','a',
                              't','i','o','n','!','!','!','!','!','!','!','!','!','!','!','!'}; 

	while(1) {

// **********************************************************************************
// **  STEP-1   Add your own text to write, 32 Chars
// **********************************************************************************

	  // Data to write to EE Slot inside ATSHA204 (Each Slot is 32 Bytes long)
	  uint8_t *Message = (uint8_t *) "ADD YOUR TEXT HERE 32 CHARS_ _ _";
  
	
// **********************************************************************************
// **  STEP-2   Select the Slot-Id, see comments below
// **********************************************************************************
   
		// Slots 0-2, 11-14, are readable, but only allow encrypted writes
		// Slot 15 is secret (non readable nor writable)
	
		// Select Slotd-Id 0,1,2, 11,12,13
	
		slotId = 1;
 
		// Execute Wake-up SHA204 command
		ret_code = sha204c_wakeup(RxBuffer);
   
// **********************************************************************************
// **  STEP-3   Read first the contents of slotId
// **********************************************************************************
 
		ret_code = sha204_read_plain_text(slotId, RxBuffer);
	
		if (ret_code != SHA204_SUCCESS) {
			printf("Read Plain Text from SlotId %X FAILED\n", slotId);
		} else {
			printf("Read Plain Text from SlotId %X SUCCEED\n", slotId);
		}
	
	   printf("Read Data from SlotId %X\n", slotId);
	   ShowBuffer(&RxBuffer, 32);
   
   
// **********************************************************************************
// **  STEP-4   Encrypted Write to slotId
// **********************************************************************************
		ret_code = encrypted_write_procedure(Message, slotId, Key15Value, 15); 
	
		if (ret_code != SHA204_SUCCESS) {
			printf("Encrypted Write Text to SlotId %X FAILED\n", slotId);
		} else {
			printf("Encrypted Write Text to  SlotId %X SUCCEED\n", slotId);
		}
	

// **********************************************************************************
// **  STEP-5   Compare the Data read after Encrypted Write
// **********************************************************************************
	   ret_code = sha204_read_plain_text(slotId, RxBuffer);

	   printf("\nWrite Data to SlotId %X\n", slotId);
	   ShowBuffer(Message, 32); 
   
	   printf("Read Data from SlotId %X\n", slotId);
	   ShowBuffer(&RxBuffer, 32);
   
		// Compare read data with original buffer
		ret_code = memcmp(Message, &RxBuffer, sizeof(Message));
	
		if (ret_code != 0) {
			printf("Read/Write Example Write to Slot-%d ** FAILED ** \r\n\n", slotId);

		} else {
			printf("Read/Write Example Write to Slot-%d -- SUCCEED -- \r\n\n", slotId);
		}
	
	  
		// After all operations, send the sha204 to sleep  
		sha204p_sleep();
	
	

// **********************************************************************************
// **  TODO-1  Test different slots confirgure for encrypted writes  
// **********************************************************************************

 
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
