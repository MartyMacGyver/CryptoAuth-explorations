/**
 * \file
 *
 * Copyright (c) 2010-2013 Atmel Corporation. All rights reserved.
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
*  \brief  Application example demonstrates the Sign / Verify
*
*  \author James Tomasetta james.tomasetta@atmel.com
*  \date   March 2013
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <asf.h>                                          // definitions and declarations for the Atmel Software Framework
#include "sha204_example_led.h"                           // definitions of LED utility functions
#include "ecc108_examples.h"                              // definitions and declarations for example functions
#include "sha204_helper.h"                                // definitions and declarations for the Helper module (e.g. TempKey calculation)
#include "sign_verify.h"     // definitions and declarations for this module
#include "printUtils.h"

#include "ecc108_commands.h"




void lock(void)
{
	uint8_t wakeup_response[ECC108_RSP_SIZE_MIN];
	uint8_t ret_code;
	uint8_t tx_buffer[WRITE_COUNT_SHORT];
	uint8_t rx_buffer[READ_32_RSP_SIZE];
	uint8_t Data[4];
	
    struct sha204_read_parameters args_read;
    struct sha204_write_parameters args_write;
    struct sha204_lock_parameters args_lock;

	// **********************************************************************************
	// **  Wakeup the ECC108
	// **********************************************************************************
	ret_code = ecc108c_wakeup(wakeup_response);
	
	if (ret_code != ECC108_SUCCESS)
	{
		printf("WakeUp	FAILED\n\r");
		return ;
	} else {
		printf("WakeUp	Ok\n\r");
	}
	
	args_write.tx_buffer = tx_buffer;
	args_write.rx_buffer = rx_buffer;
	args_write.zone = SHA204_ZONE_CONFIG;
	args_write.mac = NULL;

	// Config slot 0 + 1 - ECC Private without GenKey
	args_write.address = 0x0014;
	Data[0] = 0xc3;
	Data[1] = 0x0f;
	Data[2] = 0xc3;
	Data[3] = 0x2f;

	args_write.new_value = Data;
	ret_code = ecc108m_write(&args_write);
	if (ret_code != ECC108_SUCCESS) {
		printf("WRITE	FAILED \n\r");
//		return ;
	} else {
		printf("WRITE	Ok \n\r");
	}

	args_write.address = 0x0060;
	Data[0] = 0x33;
	Data[1] = 0x0f;
	Data[2] = 0x33;
	Data[3] = 0x0f;

	args_write.new_value = Data;
	ret_code = ecc108m_write(&args_write);
	if (ret_code != ECC108_SUCCESS) {
		printf("WRITE	FAILED \n\r");
//		return ;
	} else {
		printf("WRITE	Ok \n\r");
	}

	// Config slot 8 + 9 - ECC Private with GenKey
	args_write.address = 0x0024;
	Data[0] = 0x0f;
	Data[1] = 0x0f;
	Data[2] = 0x0f;
	Data[3] = 0x0f;

	args_write.new_value = Data;
	ret_code = ecc108m_write(&args_write);
	if (ret_code != ECC108_SUCCESS) {
		printf("WRITE	FAILED \n\r");
//		return ;
	} else {
		printf("WRITE	Ok \n\r");
	}

	args_write.address = 0x0070;
	Data[0] = 0x32;
	Data[1] = 0x0f;
	Data[2] = 0x32;
	Data[3] = 0x0f;

	args_write.new_value = Data;
	ret_code = ecc108m_write(&args_write);
	if (ret_code != ECC108_SUCCESS) {
		printf("WRITE	FAILED \n\r");
//		return ;
	} else {
		printf("WRITE	Ok \n\r");
	}

	// Config slot A + B - ECC Public Root
	args_write.address = 0x0028;
	Data[0] = 0x0f;
	Data[1] = 0x0f;
	Data[2] = 0x0f;
	Data[3] = 0x0f;

	args_write.new_value = Data;
	ret_code = ecc108m_write(&args_write);
	if (ret_code != ECC108_SUCCESS) {
		printf("WRITE	FAILED \n\r");
//		return ;
	} else {
		printf("WRITE	Ok \n\r");
	}

	args_write.address = 0x0074;
	Data[0] = 0x32;
	Data[1] = 0x0f;
	Data[2] = 0x32;
	Data[3] = 0x0f;

	args_write.new_value = Data;
	ret_code = ecc108m_write(&args_write);
	if (ret_code != ECC108_SUCCESS) {
		printf("WRITE	FAILED \n\r");
//		return ;
	} else {
		printf("WRITE	Ok \n\r");
	}

	// Config slot C + D - ECC Public Key
	args_write.address = 0x002c;
	Data[0] = 0x0f;
	Data[1] = 0x0f;
	Data[2] = 0x0f;
	Data[3] = 0x0f;

	args_write.new_value = Data;
	ret_code = ecc108m_write(&args_write);
	if (ret_code != ECC108_SUCCESS) {
		printf("WRITE	FAILED \n\r");
//		return ;
	} else {
		printf("WRITE	Ok \n\r");
	}

	args_write.address = 0x0078;
	Data[0] = 0x32;
	Data[1] = 0x0f;
	Data[2] = 0x32;
	Data[3] = 0x0f;

	args_write.new_value = Data;
	ret_code = ecc108m_write(&args_write);
	if (ret_code != ECC108_SUCCESS) {
		printf("WRITE	FAILED \n\r");
//		return ;
	} else {
		printf("WRITE	Ok \n\r");
	}

	// Config slot E + F - ECC Sign of Public Key from Root
	args_write.address = 0x0030;
	Data[0] = 0x0f;
	Data[1] = 0x4f;
	Data[2] = 0x0f;
	Data[3] = 0x8f;

	args_write.new_value = Data;
	ret_code = ecc108m_write(&args_write);
	if (ret_code != ECC108_SUCCESS) {
		printf("WRITE	FAILED \n\r");
//		return ;
	} else {
		printf("WRITE	Ok \n\r");
	}

	args_write.address = 0x007c;
	Data[0] = 0x3e;
	Data[1] = 0x0f;
	Data[2] = 0x3e;
	Data[3] = 0x0f;

	args_write.new_value = Data;
	ret_code = ecc108m_write(&args_write);
	if (ret_code != ECC108_SUCCESS) {
		printf("WRITE	FAILED \n\r");
//		return ;
	} else {
		printf("WRITE	Ok \n\r");
	}

	// Lock Config
	args_lock.zone = SHA204_ZONE_CONFIG | 0x80;
	args_lock.tx_buffer = tx_buffer;
	args_lock.rx_buffer = rx_buffer;
	
	ret_code = ecc108m_lock(&args_lock);
	if (ret_code != ECC108_SUCCESS) {
		printf("CONFIG LOCK	FAILED \n\r");
//		return ;
	} else {
		printf("CONFIG LOCK	Ok \n\r");
	}
	
	// Private Write Root
	struct ecc108_PrivWrite_parameters args_PrivWrite;

	uint8_t RootKey[] = {0x00, 0x00, 0x00, 0x00,
						 0xec, 0x77, 0x3d, 0xc3, 0xf1, 0x13, 0x5e, 0xc0,
						 0x87, 0x99, 0x17, 0xee, 0x15, 0x30, 0xa5, 0x12,
						 0xac, 0xee, 0xeb, 0x87, 0x3e, 0x7c, 0xb3, 0x73,
						 0x80, 0x74, 0x0a, 0xce, 0x1b, 0x35, 0x68, 0x77};

	
	uint8_t tx_privwritebuffer[100];
	args_PrivWrite.tx_buffer = tx_privwritebuffer;
	args_PrivWrite.rx_buffer = rx_buffer;
	args_PrivWrite.zone = 0x00;
	args_PrivWrite.key_id = PRIVATE_ROOT_KEY_ID;
	args_PrivWrite.Value = RootKey;
	
	uint8_t *Digest[32];
	args_PrivWrite.MAC = Digest;
	ret_code = ecc108m_PrivWrite(&args_PrivWrite);
	if (ret_code != ECC108_SUCCESS) {
		printf("WRITE Root PrivKey	FAILED \n\r");
//		return ;
	} else {
		printf("WRITE Root PrivKey	Ok \n\r");
	}

	
	// Gen Root Pub
    struct ecc108_gen_key_parameters args_genkey;
	uint8_t public_key[GENKEY_RSP_SIZE];
	const uint8_t *public_key_data = 0x000000;

	// Generate the Private Key with GENKEY command arguments
	args_genkey.tx_buffer = tx_buffer;
	args_genkey.rx_buffer = public_key;         // Final Digest is stored on mac buffer
	args_genkey.mode = GENKEY_MODE_PUBLIC;
	args_genkey.key_id = PRIVATE_ROOT_KEY_ID;
	args_genkey.other_data = (uint8_t *) public_key_data;

	ret_code = ecc108m_gen_key(&args_genkey);

	if (ret_code != ECC108_SUCCESS) {
		printf("GENKEY	Root Pub FAILED \n\r");
		//		return ;
	} else {
		printf("GENKEY Root Pub	Ok \n\r");
		printf("Root Public Key\n\r");
		ShowBuffer(&public_key[1], 64);  // Display data in terminal
	}

	// Write Root Public

	uint8_t rootpubkey[72];
	memset(rootpubkey, 0, 72);
	memcpy(&rootpubkey[8], &public_key[1], 64);
	
	args_write.tx_buffer = tx_buffer;
	args_write.rx_buffer = rx_buffer;
	args_write.zone = SHA204_ZONE_DATA | 0x80;
	args_write.mac = NULL;

	// first block of 32
	args_write.address = PUBLIC_ROOT_KEY_ID * SHA204_KEY_SIZE;
	args_write.new_value = &rootpubkey[0];
	ret_code = ecc108m_write(&args_write);
	if (ret_code != ECC108_SUCCESS) {
		printf("WRITE Root PubKey	FAILED \n\r");
//		return ;
	} else {
		printf("WRITE Root PubKey	Ok \n\r");
	}
	// second block of 32
	args_write.address = (PUBLIC_ROOT_KEY_ID * SHA204_KEY_SIZE) | 0x400;
	args_write.new_value = &rootpubkey[32];
	ret_code = ecc108m_write(&args_write);
	if (ret_code != ECC108_SUCCESS) {
		printf("WRITE Root PubKey	FAILED \n\r");
//		return ;
	} else {
		printf("WRITE Root PubKey	Ok \n\r");
	}
	// third block of 32
	args_write.address = (PUBLIC_ROOT_KEY_ID * SHA204_KEY_SIZE) | 0x800;
	args_write.new_value = &rootpubkey[64];
	ret_code = ecc108m_write(&args_write);
	if (ret_code != ECC108_SUCCESS) {
		printf("WRITE Root PubKey	FAILED \n\r");
//		return ;
	} else {
		printf("WRITE Root PubKey	Ok \n\r");
	}

	
	// Lock Data
	args_lock.zone = 0x01 | 0x80;
	args_lock.tx_buffer = tx_buffer;
	args_lock.rx_buffer = rx_buffer;
	
	ret_code = ecc108m_lock(&args_lock);
	if (ret_code != ECC108_SUCCESS) {
		printf("DATA LOCK	FAILED \n\r");
//		return ;
	} else {
		printf("DATA LOCK	Ok \n\r");
	}

//	args_read.tx_buffer = tx_buffer;
//	args_read.rx_buffer = rx_buffer;
//	args_read.zone = SHA204_ZONE_CONFIG;


//	for(int x = 0; x < 0x80; x=x+4)
//	{
//		args_read.address = 0x0000 + x;

		// Issue the read command
//		ret_code = ecc108m_read(&args_read);

//		if (ret_code != ECC108_SUCCESS) {
//			printf("READ	FAILED \n\r");
//			return ;
//		} else {
//			printf("READ	Ok \n\r");
//			ShowBuffer(&rx_buffer[1], 4);  // Display data in terminal
//		}
//	}
	ecc108p_sleep();
	printf("Sleep	Ok\n\r");


}

void personalize(void)
{
	uint8_t wakeup_response[ECC108_RSP_SIZE_MIN];
	uint8_t tx_buffer[MAC_COUNT_LONG];
	uint8_t ret_code;
	uint8_t signature[SIGN_RSP_SIZE];


    struct sha204_write_parameters args_write;
    struct sha204_random_parameters args_random;
    struct ecc108_gen_key_parameters args_genkey;
	struct ecc108_sign_parameters args_sign;
	struct sha204_nonce_parameters args_nonce;

	uint8_t rx_buffer[READ_32_RSP_SIZE];
	uint8_t rand_out[MAC_RSP_SIZE];
	uint8_t public_key[GENKEY_RSP_SIZE];


	// **********************************************************************************
	// **  Wakeup the ECC108
	// **********************************************************************************
	ret_code = ecc108c_wakeup(wakeup_response);
	
	if (ret_code != ECC108_SUCCESS)
	{
		printf("WakeUp	FAILED\n\r");
		return ;
	} else {
		printf("WakeUp	Ok\n\r");
	}
	
	// GenKey
	
	// **********************************************************************************
	// **  STEP-1  Start with a Random command. The output of the Random command
	// **          is a Random Number.
	// **********************************************************************************

	args_random.tx_buffer = tx_buffer;
	args_random.rx_buffer = rand_out;
	args_random.mode = RANDOM_SEED_UPDATE;

	// Issue the RANDOM command
	ret_code = ecc108m_random(&args_random);

	if (ret_code != ECC108_SUCCESS) {
		printf("RANDOM	FAILED \n\r");
//		return ;
	} else {
		printf("RANDOM	Ok \n\r");
		printf("Random Number\n\r");
		ShowBuffer(&rand_out[1], 32);  // Display data in terminal
	}

	// **********************************************************************************
	// **  STEP-2  GenKey command creates a Private/Public Key Pair.
	// **********************************************************************************

	const uint8_t *public_key_data = 0x000000;

	// Generate the Private Key with GenKey command arguments
	args_genkey.tx_buffer = tx_buffer;
	args_genkey.rx_buffer = public_key;         // Final Digest is stored on mac buffer
	args_genkey.mode = GENKEY_MODE_PRIVATE;
	args_genkey.key_id = PRIVATE_KEY_ID;
	args_genkey.other_data = (uint8_t *) public_key_data;

	ret_code = ecc108m_gen_key(&args_genkey);

	if (ret_code != ECC108_SUCCESS) {
		printf("GENKEY	FAILED \n\r");
//		return ;
	} else {
		printf("GENKEY	Ok \n\r");
		printf("Public Key\n\r");
		ShowBuffer(&public_key[1], 64);  // Display data in terminal
	}
	
	// Write Public

	// **********************************************************************************
	// **  STEP-3  Write Client Public Key.
	// **********************************************************************************
	uint8_t pubkey[72];
	memset(pubkey, 0, 72);
	memcpy(&pubkey[8], &public_key[1], 64);
	
	args_write.tx_buffer = tx_buffer;
	args_write.rx_buffer = rx_buffer;
	args_write.zone = SHA204_ZONE_DATA | 0x80;
	args_write.mac = NULL;

	// first block of 32
	args_write.address = PUBLIC_KEY_ID * SHA204_KEY_SIZE;
	args_write.new_value = &pubkey[0];
	ret_code = ecc108m_write(&args_write);
	if (ret_code != ECC108_SUCCESS) {
		printf("WRITE PubKey	FAILED \n\r");
//		return ;
	} else {
		printf("WRITE PubKey	Ok \n\r");
	}
	// second block of 32
	args_write.address = (PUBLIC_KEY_ID * SHA204_KEY_SIZE) | 0x400;
	args_write.new_value = &pubkey[32];
	ret_code = ecc108m_write(&args_write);
	if (ret_code != ECC108_SUCCESS) {
		printf("WRITE PubKey	FAILED \n\r");
//		return ;
	} else {
		printf("WRITE PubKey	Ok \n\r");
	}
	// third block of 32
	args_write.address = (PUBLIC_KEY_ID * SHA204_KEY_SIZE) | 0x800;
	args_write.new_value = &pubkey[64];
	ret_code = ecc108m_write(&args_write);
	if (ret_code != ECC108_SUCCESS) {
		printf("WRITE PubKey	FAILED \n\r");
//		return ;
	} else {
		printf("WRITE PubKey	Ok \n\r");
	}

	// Sign Pub with Root

	//SHA256
	
	// **********************************************************************************
	// **  Hash the PubKey
	// **********************************************************************************

	uint8_t *Digest[32];
	
	
	sha204h_calculate_sha256(64,  &pubkey[8],  Digest);
	printf("HASH PubKey	Ok \n\r");



	
	// **********************************************************************************
	// **  STEP-4  With a Nonce command load the digest of the certificate.  The digest 
	// **          number is then stored inside TempKey.
	// **********************************************************************************
	args_nonce.tx_buffer = tx_buffer;
	args_nonce.rx_buffer = rand_out;
	args_nonce.mode = NONCE_MODE_PASSTHROUGH;
	args_nonce.num_in = (uint8_t *) Digest;

	// Issue the Nonce command
	ret_code = ecc108m_nonce(&args_nonce);

	if (ret_code != ECC108_SUCCESS) {
		printf("NONCE Digest	FAILED \n\r");
//		return ;
	} else {
		printf("NONCE Digest	Ok \n\r");
		ShowBuffer(&Digest[0], 32);  // Display data in terminal
	}
	
	// **********************************************************************************
	// **  STEP-5  SIGN the Digest with the root private key.
	// **********************************************************************************

	// Generate the Signature with SIGN command arguments
	args_sign.tx_buffer = tx_buffer;
	args_sign.rx_buffer = signature;         // Final Digest is stored on mac buffer
	args_sign.mode = SIGN_MODE_TEMPKEY;
	args_sign.key_id = PRIVATE_ROOT_KEY_ID;

	ret_code = ecc108m_sign(&args_sign);

	if (ret_code != ECC108_SUCCESS) {
		printf("SIGN Digest	FAILED \n\r");
//		return ;
	} else {
		printf("SIGN Digest	Ok \n\r");
		printf("Signature\n\r");
		ShowBuffer(&signature[1], 64);  // Display data in terminal
	}
	
	// Write Signature

	// **********************************************************************************
	// **  STEP-6  Write Client Signature.
	// **********************************************************************************

	uint8_t sig[72];
	memset(sig, 0, 72);
	memcpy(&sig[8], &signature[1], 64);
	
	args_write.tx_buffer = tx_buffer;
	args_write.rx_buffer = rx_buffer;
	args_write.zone = SHA204_ZONE_DATA | 0x80;
	args_write.mac = NULL;

	// first block of 32
	args_write.address = SIGN_ID * SHA204_KEY_SIZE;
	args_write.new_value = &sig[0];
	ret_code = ecc108m_write(&args_write);
	if (ret_code != ECC108_SUCCESS) {
		printf("WRITE Signature	FAILED \n\r");
//		return ;
	} else {
		printf("WRITE Signature	Ok \n\r");
	}
	// second block of 32
	args_write.address = (SIGN_ID * SHA204_KEY_SIZE) | 0x400;
	args_write.new_value = &sig[32];
	ret_code = ecc108m_write(&args_write);
	if (ret_code != ECC108_SUCCESS) {
		printf("WRITE Signature	FAILED \n\r");
//		return ;
	} else {
		printf("WRITE Signature	Ok \n\r");
	}
	// second block of 32
	args_write.address = (SIGN_ID * SHA204_KEY_SIZE) | 0x800;
	args_write.new_value = &sig[64];
	ret_code = ecc108m_write(&args_write);
	if (ret_code != ECC108_SUCCESS) {
		printf("WRITE Signature	FAILED \n\r");
//		return ;
	} else {
		printf("WRITE Signature	Ok \n\r");
	}
	
	ecc108p_sleep();
	printf("Sleep	Ok\n\r");
}


void Auth(void)
{
	uint8_t signature[SIGN_RSP_SIZE];
	uint8_t verify_response[ECC108_RSP_SIZE_MIN];
	uint8_t rand_out[MAC_RSP_SIZE];
	uint8_t wakeup_response[ECC108_RSP_SIZE_MIN];
	uint8_t ret_code;
	uint8_t tx_buffer[MAC_COUNT_LONG];
	uint8_t verify_data[128];
	memset(verify_data, 0, 128);

	uint8_t challenge[32];
	memset(challenge, 0, 32);

    struct sha204_random_parameters args_random;
	struct sha204_nonce_parameters args_nonce;
	struct ecc108_verify_parameters args_verify;
	struct ecc108_sign_parameters args_sign;
	struct sha204_read_parameters args_read;

	// **********************************************************************************
	// **  Wakeup the ECC108.
	// **********************************************************************************
	ret_code = ecc108c_wakeup(wakeup_response);
	
	if (ret_code != ECC108_SUCCESS)
	{
		printf("WakeUp	FAILED\n\r");
		return ;
	} else {
		printf("WakeUp	Ok\n\r");
	}

	uint8_t rx_buffer[READ_32_RSP_SIZE];

	args_read.tx_buffer = tx_buffer;
	args_read.rx_buffer = rx_buffer;
	args_read.zone = SHA204_ZONE_DATA | 0x80;

	// **********************************************************************************
	// **  STEP-1  Read Root Pub. 
	// **********************************************************************************
	uint8_t rootpubkey[64];
	memset(rootpubkey, 0, 64);

	args_read.address = PUBLIC_ROOT_KEY_ID * SHA204_KEY_SIZE;
	
	ret_code = ecc108m_read(&args_read);
	if (ret_code != ECC108_SUCCESS) {
		printf("READ Root PubKey	FAILED \n\r");
		return ;
	} else {
		printf("READ Root PubKey	Ok \n\r");
		memcpy(&rootpubkey[0], &rx_buffer[9], 24);
	}

	args_read.address = (PUBLIC_ROOT_KEY_ID * SHA204_KEY_SIZE) | 0x400;
	
	ret_code = ecc108m_read(&args_read);
	if (ret_code != ECC108_SUCCESS) {
		printf("READ Root PubKey	FAILED \n\r");
		return ;
	} else {
		printf("READ Root PubKey	Ok \n\r");
		memcpy(&rootpubkey[24], &rx_buffer[1], 32);
	}

	args_read.address = (PUBLIC_ROOT_KEY_ID * SHA204_KEY_SIZE) | 0x800;
	
	ret_code = ecc108m_read(&args_read);
	if (ret_code != ECC108_SUCCESS) {
		printf("READ Root PubKey	FAILED \n\r");
		return ;
	} else {
		printf("READ Root PubKey	Ok \n\r");
		memcpy(&rootpubkey[56], &rx_buffer[1], 8);
		ShowBuffer(&rootpubkey[0], 64);  // Display data in terminal
	}

	// **********************************************************************************
	// **  STEP-2  Read Client Public Key. 
	// **********************************************************************************
	uint8_t pubkey[64];
	memset(pubkey, 0, 64);
	args_read.address = PUBLIC_KEY_ID * SHA204_KEY_SIZE;
	
	ret_code = ecc108m_read(&args_read);
	if (ret_code != ECC108_SUCCESS) {
		printf("READ PubKey	FAILED \n\r");
		return ;
	} else {
		printf("READ PubKey	Ok \n\r");
		memcpy(&pubkey[0], &rx_buffer[9], 24);
	}

	args_read.address = (PUBLIC_KEY_ID * SHA204_KEY_SIZE) | 0x400;
	
	ret_code = ecc108m_read(&args_read);
	if (ret_code != ECC108_SUCCESS) {
		printf("READ PubKey	FAILED \n\r");
		return ;
	} else {
		printf("READ PubKey	Ok \n\r");
		memcpy(&pubkey[24], &rx_buffer[1], 32);
	}

	args_read.address = (PUBLIC_KEY_ID * SHA204_KEY_SIZE) | 0x800;
	
	ret_code = ecc108m_read(&args_read);
	if (ret_code != ECC108_SUCCESS) {
		printf("READ PubKey	FAILED \n\r");
		return ;
	} else {
		printf("READ PubKey	Ok \n\r");
		memcpy(&pubkey[56], &rx_buffer[1], 8);
		ShowBuffer(&pubkey[0], 64);  // Display data in terminal
	}


	// **********************************************************************************
	// **  STEP-3  Read Signature. 
	// **********************************************************************************
	uint8_t sign[64];
	memset(sign, 0, 64);
	args_read.address = SIGN_ID * SHA204_KEY_SIZE;
	
	ret_code = ecc108m_read(&args_read);
	if (ret_code != ECC108_SUCCESS) {
		printf("READ Sign	FAILED \n\r");
		return ;
	} else {
		printf("READ Sign	Ok \n\r");
		memcpy(&sign[0], &rx_buffer[9], 24);
	}

	args_read.address = (SIGN_ID * SHA204_KEY_SIZE) | 0x400;

	ret_code = ecc108m_read(&args_read);
	if (ret_code != ECC108_SUCCESS) {
		printf("READ Sign	FAILED \n\r");
		return ;
	} else {
		printf("READ Sign	Ok \n\r");
		memcpy(&sign[24], &rx_buffer[1], 32);
	}

	args_read.address = (SIGN_ID * SHA204_KEY_SIZE) | 0x800;

	ret_code = ecc108m_read(&args_read);
	if (ret_code != ECC108_SUCCESS) {
		printf("READ Sign	FAILED \n\r");
		return ;
	} else {
		printf("READ Sign	Ok \n\r");
		memcpy(&sign[56], &rx_buffer[1], 8);
		ShowBuffer(&sign[0], 64);  // Display data in terminal
	}

	// **********************************************************************************
	// **  STEP-4 Hash Client Public Key (Client Pub)
	// **********************************************************************************
	uint8_t *Digest[32];
	
	
	sha204h_calculate_sha256(64, pubkey,  Digest);
	printf("HASH PubKey	Ok \n\r");
	
	// **********************************************************************************
	// **  STEP-5 Nonce Digest (Hash of Client Pub)
	// **********************************************************************************
	args_nonce.tx_buffer = tx_buffer;
	args_nonce.rx_buffer = rand_out;
	args_nonce.mode = NONCE_MODE_PASSTHROUGH;
	args_nonce.num_in = (uint8_t *) Digest;

	// Issue the NONCE command
	ret_code = ecc108m_nonce(&args_nonce);

	if (ret_code != ECC108_SUCCESS) {
		printf("NONCE Digest	FAILED \n\r");
//		return ;
	} else {
		printf("NONCE Digest	Ok \n\r");
		ShowBuffer(&Digest[0], 32);  // Display data in terminal
	}
	
	// **********************************************************************************
	// **  STEP-6 Verify Client Public Key (Root Pub, Sign).
	// **********************************************************************************
	memcpy(&verify_data[0], &sign[0], 64);
	memcpy(&verify_data[64], &rootpubkey[0], 64);
	
	
	
	// Verify the Signature with the VERIFY command arguments
	args_verify.tx_buffer = tx_buffer;
	args_verify.rx_buffer = verify_response;         // Final Digest is stored on mac buffer
	args_verify.mode = VERIFY_MODE_External;
	args_verify.key_id = 0x04;
	args_verify.other_data = verify_data;

	ret_code = ecc108m_verify(&args_verify);

	if (ret_code != ECC108_SUCCESS) {
		printf("ROOT VERIFY	FAILED \n\r");
//		return ;
	} else {
		printf("ROOT VERIFY	Ok \n\r");
	}
	
	
	// Random Challenge

	// **********************************************************************************
	// **  STEP-7  Start with a Random command. The output of the Random command
	// **          is a Random Number.
	// **********************************************************************************

	args_random.tx_buffer = tx_buffer;
	args_random.rx_buffer = rand_out;
	args_random.mode = RANDOM_SEED_UPDATE;

	// Issue the RANDOM command
	ret_code = ecc108m_random(&args_random);

	if (ret_code != ECC108_SUCCESS) {
		printf("RANDOM CHALLENGE FAILED \n\r");
		return ;
	} else {
		printf("RANDOM CHALLENGE Ok \n\r");
		printf("Random Number\n\r");
		ShowBuffer(&rand_out[1], 32);  // Display data in terminal
		memcpy(&challenge[0], &rand_out[1], 32);
	}
	
	// Sign RN

	// **********************************************************************************
	// **  STEP-8  With a Nonce command load the digest of the certificate.  The digest 
	// **          number is then stored inside TempKey.
	// **********************************************************************************
	args_nonce.tx_buffer = tx_buffer;
	args_nonce.rx_buffer = rand_out;
	args_nonce.mode = NONCE_MODE_PASSTHROUGH;
	args_nonce.num_in = (uint8_t *) challenge;

	// Issue the Nonce command
	ret_code = ecc108m_nonce(&args_nonce);

	if (ret_code != ECC108_SUCCESS) {
		printf("CHALLENGE NONCE	FAILED \n\r");
		return ;
	} else {
		printf("CHALLENGE NONCE	Ok \n\r");
		ShowBuffer(&challenge[0], 32);  // Display data in terminal
	}
	
	// **********************************************************************************
	// **  STEP-9  SIGN the Digest with the client private key.
	// **********************************************************************************

	// Generate the Signature with Sign command arguments
	args_sign.tx_buffer = tx_buffer;
	args_sign.rx_buffer = signature;         // Final Digest is stored on mac buffer
	args_sign.mode = SIGN_MODE_TEMPKEY;
	args_sign.key_id = PRIVATE_KEY_ID;

	ret_code = ecc108m_sign(&args_sign);

	if (ret_code != ECC108_SUCCESS) {
		printf("CHALLENGE SIGN FAILED \n\r");
		return ;
	} else {
		printf("CHALLENGE Signature	Ok \n\r");
		printf("CHALLENGE Signature\n\r");
		ShowBuffer(&signature[1], 64);  // Display data in terminal
	}

	// Verify RN

	// **********************************************************************************
	// **  STEP-10  With a Nonce command load the digest of the certificate.  The digest 
	// **          number is then stored inside TempKey.
	// **********************************************************************************

	args_nonce.tx_buffer = tx_buffer;
	args_nonce.rx_buffer = rand_out;
	args_nonce.mode = NONCE_MODE_PASSTHROUGH;
	args_nonce.num_in = (uint8_t *) challenge;

	// Issue the NONCE command
	ret_code = ecc108m_nonce(&args_nonce);

	if (ret_code != ECC108_SUCCESS) {
		printf("VERIFY NONCE	FAILED \n\r");
		return ;
	} else {
		printf("VERIFY NONCE	Ok \n\r");
		ShowBuffer(&challenge[0], 32);  // Display data in terminal
	}
	
	// **********************************************************************************
	// **  STEP-11  VERIFY the signature with the public key.
	// **********************************************************************************
	memcpy(&verify_data[0], &signature[1], 64);
	memcpy(&verify_data[64], &pubkey[0], 64);

	// Verify the Signature with the Verify command arguments
	args_verify.tx_buffer = tx_buffer;
	args_verify.rx_buffer = verify_response;         // Final Digest is stored on mac buffer
	args_verify.mode = VERIFY_MODE_External;
	args_verify.key_id = 0x04;
	args_verify.other_data = verify_data;

	ret_code = ecc108m_verify(&args_verify);

	if (ret_code != ECC108_SUCCESS) {
		printf("CHALLENGE VERIFY	FAILED \n\r");
		return ;
	} else {
		printf("CHALLENGE VERIFY	Ok \n\r");
	}
	
	ecc108p_sleep();
	printf("Sleep	Ok\n\r");
}



