/*
 * firmware_protection.c
 *
 * Created: 3/26/2013 11:04:59 AM
 *  Author: james.tomasetta
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <asf.h>                   // definitions and declarations for the Atmel Software Framework
#include "sha204_example_led.h"    // definitions of LED utility functions
#include "ecc108_examples.h"       // definitions and declarations for example functions
#include "firmware_protection.h"   // definitions and declarations for this module
#include "printUtils.h"
#include "sha204_helper.h"         // definitions and declarations for the Helper module (e.g. TempKey calculation)

#include "ecc108_commands.h"

void ObscureKeyComputation2(uint8_t *key)
{
	uint8_t sp[2];
	uint8_t spl, sph;
	uint8_t i ;
	uint8_t values[4];
	
	// Read Stack pointer
	asm volatile ("in %0, __SP_H__  \n\t"
	"in %1, __SP_L__  \n\t"
	:"=r"(sph), "=r"(spl) /* output */
	:        /* input */
	);
	
	// As a variable, we could take the value of Stack pointer, and we remove the 7 least
	// significant bits. This masks the fluctuations due to C-Compiler optimizations.
	// The Calculated Key = SHA256(Stack_Pointer & 0x0080)
	
	// If the SP is taken as variable, the Key value could be affected due to
	// compiler optimizations
	
	//	sp[0] = sph;
	//   sp[1] = spl & 0x80;
	
	// For a more fixed value, we init an array and calculate the sha256
	
	// for more security, these values should be initialized somewhere else
	// in the software
	
	values[0] = 0xFe;
	values[1] = 0x53;
	values[2] = 0x7B;
	values[3] = 0xCA;
	
	sha204h_calculate_sha256(4, values, key);
	
	printf("\nComputed Key :\n");
	ShowBuffer(key, 32);  // Display data in terminal
}

 
void firmware_protection(void)
{
	uint8_t ret_code;
	uint8_t wakeup_response[ECC108_RSP_SIZE_MIN];
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
	
	uint8_t rx_buffer[MAC_RSP_SIZE];
	

	// **********************************************************************************
	// **  NOTE:  The BEST and more secure solution is to use a ATSHA204 on the
	// **          HOST and CLIENT
	// **          For this exercise we will use only 1 ATSHA204 and emulate the
	// **          internal operations on a ATSHA204 to demonstrate the Random Challenge/Response
	// **********************************************************************************


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
		// displayed using the on-board LEDs.
				
		// Wake up the device and receive the wakeup response.
		ret_code = ecc108c_wakeup(wakeup_response);
		if (ret_code != ECC108_SUCCESS) {
			printf("WakeUp	FAILED \n\r");
			return ;
		}else {
			printf("WakeUp	Ok\n\r");
		}		


// **********************************************************************************
// **  STEP-3   The NONCE Command. It is issued to insert the value of NumIn 
//              parameter into the internal "TempKey" of the SHA204
//              Note the "NONCE_MODE_PASSTHROUGH" mode
// **********************************************************************************

		// NONCE commands arguments
		args_nonce.tx_buffer = tx_buffer;
		args_nonce.rx_buffer = rx_buffer;
		args_nonce.mode = NONCE_MODE_PASSTHROUGH;
		args_nonce.num_in = challenge;

		// Issue a Nonce command.
		ret_code = ecc108m_nonce(&args_nonce);

		if (ret_code != ECC108_SUCCESS) {
			ecc108p_sleep();
			printf("NONCE	FAILED \n\r");
			return ;
		} else {
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
		ret_code = ecc108m_mac(&args_mac);

		// Put SHA204 device to sleep.
		ecc108p_sleep();

		if (ret_code != ECC108_SUCCESS) {
			printf("MAC	FAILED \n\r");
			return ;
		} else {
			printf("MAC	Ok \n\r");
		}
	
		printf("\nCompare Challenge and Response:\n\n\r");
  
		//Mac response packet = 1(count) + 32(MAC) + 2(CRC)
		// skip first byte
		for (i = 0; i < 32; i++) { // MAC Response is 32 Bytes
			ret_code |= (rx_buffer[1+i] == expected_response[i] ? ECC108_SUCCESS : SHA204_FUNC_FAIL);
		}
		
		
		// Display results in terminal	
		if (ret_code == ECC108_SUCCESS) {
			 printf("Fixed Challenge/Response test --SUCCEEDED-- \n\r");
		} else {
			 printf("Fixed Challenge/Response test **FAILED** \n\n\r");
			 printf("Received Response:\r\n");
			 ShowBuffer(&rx_buffer[1], 32);  // Display data in terminal
		 
		}		 		 	
	   
 		// After all operations, send the sha204 to sleep
 		ecc108p_sleep();
 	
 
 // **********************************************************************************
 // **  TODO-1  Change the value of the challenge, and check the response
 // **********************************************************************************
 
		 // pause here to retry
		 printf("\nPress enter to try again:");
		 // Capture terminal text
		 do {
			 c = getchar();
			 Message[i++] = c;
		 } while (c != '\n');


		 printf("\n\n=============================================\n\n");


// **********************************************************************************
// **  STEP-6  Start with a Random NONCE command. The output of the NONCE command
// **          is a Random Number used on the calculations. The NONCE number is then
// **          stored inside the SHA204 TempKey
// **********************************************************************************

		ret_code = ecc108c_wakeup(wakeup_response);
		
		if (ret_code != ECC108_SUCCESS) {;
			printf("WakeUp	FAILED\n\r");
			return ;
		} else {
			printf("WakeUp	Ok\n\r");
		}
	
		// This is a value used for the NONCE command NumIn parameter, 20 bytes long
		const uint8_t *NumIn = (uint8_t *) "_HereIsTheChallenge_";

		args_nonce.tx_buffer = tx_buffer;
		args_nonce.rx_buffer = rand_out;
		args_nonce.mode = NONCE_MODE_SEED_UPDATE;
		args_nonce.num_in = (uint8_t *) NumIn;
	
		// Issue the NONCE command
		ret_code = ecc108m_nonce(&args_nonce);
	
		if (ret_code != ECC108_SUCCESS) {
			printf("NONCE	FAILED \n\r");
			return ;
		} else {
			printf("NONCE	Ok \n\r");
			printf("Random Challenge\n\r");
			ShowBuffer(&rand_out[1], 32);  // Display data in terminal
		}
	
	
// **********************************************************************************
// **  STEP-7  In this case, the MAC command combines the value previously
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

		ret_code = ecc108m_mac(&args_mac);

		if (ret_code != ECC108_SUCCESS) {
			printf("MAC	FAILED \n\r");
			return ;
		} else {
			printf("MAC	Ok \n\r");
		}


// **********************************************************************************
// **  STEP-8  Next, we need to emulate the calculations inside the SHA204 using
// **          the functions from the helper library.
// **          Using the sha204h_nonce() and the Random number generated before
// **          stored in rand_out, we can replicate the intermediate value of
// **          TempKey
// **********************************************************************************

		args_nonce_tempkey.mode = NONCE_MODE_SEED_UPDATE;
		args_nonce_tempkey.num_in = (uint8_t *) NumIn;
		args_nonce_tempkey.rand_out = &rand_out[SHA204_BUFFER_POS_DATA];
		args_nonce_tempkey.temp_key = &temp_key;
	
		ret_code = ecc108h_nonce(&args_nonce_tempkey);

		if (ret_code != ECC108_SUCCESS) {;
			printf("Emulated NONCE	FAILED \n\r");
			return ;
		}  else {
			printf("Emulated NONCE	Ok \n\r");
		}



// **********************************************************************************
// **  STEP-9    Emulate a MAC Command using sha204h_mac()
// **            The output of this command is the "Response" used for the
// **            Random Challenge-Response
// **            Value stored in "mac_calculated"
// **********************************************************************************

		// The calculated key, is stored on the "Training" SHA204 on Slot 14,
		uint8_t key[32];
		ObscureKeyComputation2(key);

		args_mac_tempkey.mode = MAC_MODE_BLOCK2_TEMPKEY;
		args_mac_tempkey.key_id = 14;
		args_mac_tempkey.challenge = NULL;
		args_mac_tempkey.key = (uint8_t *) key;
		args_mac_tempkey.otp = NULL;
		args_mac_tempkey.sn = NULL;
		args_mac_tempkey.response = mac_calculated;
		args_mac_tempkey.temp_key = &temp_key;

		// Emulate MAC.
		ret_code = ecc108h_mac(&args_mac_tempkey);

		if (ret_code != ECC108_SUCCESS) {;
			printf("Emulated MAC	FAILED \n\r");
			return ;
		} else {
			printf("Emulated MAC	Ok \n\n\r");	
		}
	

// **********************************************************************************
// **  STEP-10  Compare the MACS's to check authenticity 
// **********************************************************************************

		// Compare the Mac response with the calculated one.
		ret_code = (memcmp(&mac[SHA204_BUFFER_POS_DATA], mac_calculated, sizeof(mac_calculated)) ? SHA204_FUNC_FAIL : SHA204_SUCCESS);

		// Display results in terminal
		if (ret_code == ECC108_SUCCESS) {
				printf("Random Challenge/Response test --SUCCEEDED-- \n\n\r");
		} else {
				printf("Random Challenge/Response test **FAILED** \n\n\r");
		}

		printf("Received Response:\r\n");
		ShowBuffer(&mac[1], 32);  // Display data in terminal
		printf("Calculated Response:\r\n");
		ShowBuffer(&mac_calculated[0], 32);  // Display data in terminal
			
		// After all operations, send the sha204 to sleep
		sha204p_sleep();
		

// **********************************************************************************
// **  TODO-2  Run several times and check the Generated Challenge and Response pair.
// **          Every time the pair should be completely different.
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
