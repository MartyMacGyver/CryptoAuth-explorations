// ----------------------------------------------------------------------------
//         ATMEL Microcontroller Software Support  -  Colorado Springs, CO -
// ----------------------------------------------------------------------------
// DISCLAIMER:  THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
// DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
// OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// ----------------------------------------------------------------------------
/** \file
 *  \brief  Application examples that Use the SHA204 Library
 *  \author Atmel Crypto Products
    \date   December 13, 2012
 *
*/

/** \defgroup sha204_examples Example Functions

sha204e_checkmac_firmware:<br>
Demonstrates high security using a Nonce - GenDig - MAC command sequence
and MAC verification in firmware. This requires more code space because
a sha256 implementation in firmware is needed. Also, the firmware has to
be able to access keys. Having a key stored outside the device poses a
higher security risk.

The example function
SHA204_EXAMPLE_CHECKMAC_FIRMWARE uses the sha204m_execute function that can be used
to send any ATSHA204 command.

CAUTION WHEN DEBUGGING: Be aware of the timeout feature of the device. The
device will go to sleep between 0.7 and 1.7 seconds after a Wakeup. When
hitting a break point, this timeout will kick in and the device
has gone to sleep before you continue debugging. Therefore, after you have
examined variables you might have to restart your debug session.
@{ */

#include <string.h>                   // needed for memset(), memcpy()
#include "sha204_lib_return_codes.h"  // declarations of function return codes
#include "sha204_comm_marshaling.h"   // definitions and declarations for the Command Marshaling module
#include "sha204_helper.h"            // definitions of functions that calculate SHA256 for every command
#include "sha204_examples.h"          // definitions and declarations for example functions
#include "timers.h"                   // definitions and declarations for Idle timer functions


/** \brief factory defaults for key values
*/
const uint8_t sha204_default_key[16][SHA204_KEY_SIZE] = {
	{
		0x00, 0x00, 0x00, 0x0A, 0xA1, 0x1A, 0xAC, 0xC5, 0x57, 0x7F, 0xFF, 0xF4, 0x40, 0x04, 0x4E, 0xE4,
		0x45, 0x5D, 0xD4, 0x40, 0x04, 0x40, 0x01, 0x1B, 0xBD, 0xD0, 0x0E, 0xED, 0xD3, 0x3C, 0xC6, 0x67,
	},
	{
		0x11, 0x11, 0x11, 0x12, 0x23, 0x3B, 0xB6, 0x6C, 0xCC, 0xC5, 0x53, 0x3B, 0xB7, 0x7B, 0xB9, 0x9E,
		0xE9, 0x9B, 0xBB, 0xB5, 0x51, 0x1F, 0xFD, 0xD2, 0x2F, 0xF7, 0x74, 0x4C, 0xCD, 0xD0, 0x0E, 0xE9,
	},
	{
		0x22, 0x22, 0x22, 0x2C, 0xC1, 0x17, 0x7C, 0xC1, 0x1C, 0xC4, 0x4D, 0xD5, 0x56, 0x68, 0x89, 0x9A,
		0xAA, 0xA0, 0x00, 0x04, 0x43, 0x3E, 0xE3, 0x39, 0x9C, 0xCF, 0xFB, 0xB6, 0x6B, 0xB0, 0x0B, 0xB6,
	},
	{
		0x33, 0x33, 0x33, 0x33, 0x33, 0x36, 0x61, 0x14, 0x4A, 0xA1, 0x17, 0x79, 0x9A, 0xA2, 0x23, 0x36,
		0x6C, 0xC7, 0x7F, 0xFE, 0xE4, 0x4B, 0xBE, 0xE2, 0x2F, 0xF1, 0x13, 0x32, 0x20, 0x06, 0x67, 0x79,
	},
	{
		0x44, 0x44, 0x44, 0x49, 0x91, 0x11, 0x18, 0x86, 0x68, 0x83, 0x3D, 0xDB, 0xB8, 0x8D, 0xD3, 0x3F,
		0xF8, 0x85, 0x57, 0x70, 0x0C, 0xC7, 0x74, 0x42, 0x2E, 0xED, 0xDA, 0xAD, 0xDA, 0xA5, 0x52, 0x28,
	},
	{
		0x55, 0x55, 0x55, 0x58, 0x86, 0x6F, 0xF2, 0x2B, 0xB3, 0x32, 0x20, 0x09, 0x98, 0x8A, 0xA6, 0x6E,
		0xE1, 0x1E, 0xE6, 0x63, 0x33, 0x37, 0x7A, 0xA5, 0x52, 0x20, 0x01, 0x10, 0x03, 0x36, 0x6A, 0xA0,
	},
	{
		0x66, 0x66, 0x66, 0x6D, 0xD0, 0x04, 0x45, 0x53, 0x3A, 0xAC, 0xC2, 0x22, 0x25, 0x55, 0x57, 0x7F,
		0xF6, 0x6D, 0xD4, 0x46, 0x6B, 0xB7, 0x7D, 0xDD, 0xDF, 0xF9, 0x96, 0x68, 0x89, 0x9D, 0xDA, 0xA2,
	},
	{
		0x77, 0x77, 0x77, 0x72, 0x2F, 0xF4, 0x4A, 0xA9, 0x9C, 0xCC, 0xC0, 0x05, 0x5E, 0xE4, 0x45, 0x59,
		0x99, 0x9B, 0xBD, 0xD2, 0x26, 0x69, 0x96, 0x6D, 0xDD, 0xD4, 0x49, 0x9F, 0xF8, 0x8A, 0xA5, 0x50,
	},
	{
		0x88, 0x88, 0x88, 0x8C, 0xC6, 0x62, 0x2A, 0xAF, 0xFE, 0xE1, 0x1F, 0xF8, 0x82, 0x2D, 0xD4, 0x4E,
		0xE0, 0x08, 0x85, 0x58, 0x85, 0x53, 0x34, 0x44, 0x4D, 0xD7, 0x77, 0x7B, 0xB8, 0x89, 0x9D, 0xDE,
	},
	{
		0x99, 0x99, 0x99, 0x94, 0x4E, 0xE6, 0x6D, 0xD4, 0x4A, 0xAF, 0xF5, 0x59, 0x92, 0x23, 0x30, 0x06,
		0x6B, 0xBD, 0xD2, 0x2D, 0xD5, 0x52, 0x27, 0x77, 0x7D, 0xD7, 0x77, 0x7B, 0xB3, 0x39, 0x95, 0x5E,
	},
	{
		0xAA, 0xAA, 0xAA, 0xA1, 0x15, 0x5A, 0xA2, 0x25, 0x55, 0x50, 0x0B, 0xBD, 0xD2, 0x2E, 0xEA, 0xA9,
		0x9A, 0xAF, 0xF2, 0x29, 0x96, 0x64, 0x46, 0x61, 0x15, 0x56, 0x69, 0x91, 0x11, 0x11, 0x12, 0x29,
	},
	{
		0xBB, 0xBB, 0xBB, 0xB2, 0x24, 0x4D, 0xDB, 0xB7, 0x78, 0x8A, 0xA8, 0x87, 0x70, 0x06, 0x64, 0x4A,
		0xA1, 0x1F, 0xF0, 0x08, 0x8D, 0xDC, 0xC9, 0x91, 0x17, 0x79, 0x96, 0x66, 0x60, 0x00, 0x0A, 0xAF,
	},
	{
		0xCC, 0xCC, 0xCC, 0xCC, 0xC6, 0x61, 0x17, 0x71, 0x1A, 0xA5, 0x52, 0x24, 0x45, 0x5A, 0xAC, 0xCD,
		0xD2, 0x29, 0x92, 0x24, 0x46, 0x62, 0x28, 0x89, 0x90, 0x06, 0x62, 0x24, 0x4C, 0xCA, 0xA5, 0x56,
	},
	{
		0xDD, 0xDD, 0xDD, 0xDB, 0xBF, 0xFA, 0xAC, 0xC1, 0x11, 0x17, 0x70, 0x05, 0x55, 0x59, 0x9C, 0xCC,
		0xC9, 0x9B, 0xB6, 0x62, 0x28, 0x80, 0x0F, 0xF9, 0x92, 0x29, 0x95, 0x5D, 0xDF, 0xF3, 0x30, 0x00,
	},
	{
		0xEE, 0xEE, 0xEE, 0xE0, 0x08, 0x85, 0x55, 0x57, 0x77, 0x7B, 0xBD, 0xDA, 0xA7, 0x7B, 0xB8, 0x8A,
		0xA7, 0x7A, 0xAF, 0xF5, 0x58, 0x8D, 0xD1, 0x18, 0x8B, 0xB9, 0x92, 0x2F, 0xF0, 0x0D, 0xDF, 0xF7,
	},
	{
		0xFF, 0xFF, 0xFF, 0xF6, 0x68, 0x8B, 0xB7, 0x7B, 0xB8, 0x80, 0x01, 0x1B, 0xBE, 0xE6, 0x66, 0x62,
		0x2C, 0xCE, 0xEC, 0xC7, 0x74, 0x46, 0x68, 0x80, 0x0F, 0xFE, 0xE4, 0x47, 0x7D, 0xDC, 0xC1, 0x1C,
	},
};


/** \brief This function checks the response status byte and puts the device
           to sleep if there was an error.
   \param[in] ret_code return code of function
	\param[in] response pointer to response buffer
	\return status of the operation
*/
uint8_t sha204e_check_response_status(uint8_t ret_code, uint8_t *response)
{
	if (ret_code != SHA204_SUCCESS) {
		sha204p_sleep();
		return ret_code;
	}
	ret_code = response[SHA204_BUFFER_POS_STATUS];
	if (ret_code != SHA204_SUCCESS)
		sha204p_sleep();

	return ret_code;	
}


/** \brief This function serves as an example for
 *         the work-around of the insomnia bug.
 *
 *         The bug manifests itself in I2C devices where
 *         the Idle command does not work at certain I2C 
 *         address / speed combinations.
 
 *         In an infinite loop, the function issues the same command
 *         sequence using the Command Marshaling layer of
 *         the SHA204 library.
 * \return status of the operation
 */
uint8_t sha204e_checkmac_firmware(void)
{
	// declared as "volatile" for easier debugging
	volatile uint8_t ret_code;
	uint8_t i;
	uint8_t comparison_result;
	uint8_t mac_mode = MAC_MODE_BLOCK1_TEMPKEY | MAC_MODE_BLOCK2_TEMPKEY;
	struct sha204h_nonce_in_out nonce_param;	//parameter for nonce helper function
	struct sha204h_gen_dig_in_out gendig_param;	//parameter for gendig helper function
	struct sha204h_mac_in_out mac_param;		//parameter for mac helper function
	struct sha204h_temp_key tempkey;			//tempkey parameter for nonce and mac helper function
	static uint8_t wakeup_response[SHA204_RSP_SIZE_MIN];
	static uint8_t tx_buffer[CHECKMAC_COUNT];
	static uint8_t rx_buffer[MAC_RSP_SIZE];
	static uint8_t mac[CHECKMAC_CLIENT_RESPONSE_SIZE];
	uint8_t num_in[NONCE_NUMIN_SIZE] = {
		0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
		0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
		0x30, 0x31, 0x32, 0x33
	};
	uint8_t key_slot_0[SHA204_KEY_SIZE] = {
		0x00, 0x00, 0xA1, 0xAC, 0x57, 0xFF, 0x40, 0x4E,
		0x45, 0xD4,	0x04, 0x01, 0xBD, 0x0E, 0xD3, 0xC6,
		0x73, 0xD3, 0xB7, 0xB8,	0x2D, 0x85, 0xD9, 0xF3,
		0x13, 0xB5, 0x5E, 0xDA, 0x3D, 0x94,	0x00, 0x00
	};

	// Initialize the hardware interface.
	// Depending on which interface you have linked the
	// library to, it initializes SWI UART, SWI GPIO, or TWI.
	sha204p_init();

	while (1) {
		// The following command sequence wakes up the device, issues a Nonce, a GenDig, and 
		// a MAC command using the Command Marshaling layer, and puts the device to sleep. 
		// In parallel it calculates in firmware the TempKey and the MAC using helper 
		// functions and compares the MAC command response with the calculated result.

		// ----------------------- Nonce --------------------------------------------
		// Wake up the device.
		// If you put a break point here you will see the last
		// status after every iteration.
		// You can also inspect the mac and rx_buffer variables which should match.
		memset(wakeup_response, 0, sizeof(wakeup_response));
		ret_code = sha204c_wakeup(wakeup_response);
		if (ret_code != SHA204_SUCCESS) {
			(void) sha204p_sleep();
			continue;
		}			
	
		// Issue a Nonce command. When the configuration zone of the device is not locked the 
		// random number returned is a constant 0xFFFF0000FFFF0000...
		memset(rx_buffer, 0, sizeof(rx_buffer));
		ret_code = sha204m_execute(SHA204_NONCE, NONCE_MODE_NO_SEED_UPDATE, 0, NONCE_NUMIN_SIZE, num_in, 
			0, NULL, 0, NULL, sizeof(tx_buffer), tx_buffer, sizeof(rx_buffer), rx_buffer);
		if (ret_code != SHA204_SUCCESS) {
			(void) sha204p_sleep();
			continue;
		}

		// Put device into Idle mode since the TempKey calculation in firmware might take longer
		// than the device timeout. Putting the device into Idle instead of Sleep mode
		// maintains the TempKey.
		sha204p_idle();
		
		// Calculate TempKey using helper function.
		nonce_param.mode = NONCE_MODE_NO_SEED_UPDATE;
		nonce_param.num_in = num_in;	
		nonce_param.rand_out = &rx_buffer[SHA204_BUFFER_POS_DATA];	
		nonce_param.temp_key = &tempkey;
		ret_code = sha204h_nonce(&nonce_param);
		if (ret_code != SHA204_SUCCESS) {
			sha204e_wakeup_sleep();
			continue;
		}

		// ----------------------- GenDig --------------------------------------------
		// Wake up the device from Idle mode.
		memset(wakeup_response, 0, sizeof(wakeup_response));
		ret_code = sha204c_wakeup(wakeup_response);
		if (ret_code != SHA204_SUCCESS) {
			(void) sha204p_sleep();
			continue;
		}			

		memset(rx_buffer, 0, sizeof(rx_buffer));
		ret_code = sha204m_execute(SHA204_GENDIG, GENDIG_ZONE_DATA, SHA204_KEY_ID, 0,
			NULL, 0, NULL, 0, NULL, sizeof(tx_buffer), tx_buffer, sizeof(rx_buffer), rx_buffer);		 
		if (ret_code != SHA204_SUCCESS) {
			(void) sha204p_sleep();
			continue;
		}
		// Check response status byte for error.
		if (rx_buffer[SHA204_BUFFER_POS_STATUS] != SHA204_SUCCESS) {
			(void) sha204p_sleep();
			continue;
		}
		sha204p_idle();

		// Update TempKey using helper function.
		gendig_param.zone = GENDIG_ZONE_DATA;
		gendig_param.key_id = SHA204_KEY_ID;
		gendig_param.stored_value = key_slot_0;
		gendig_param.temp_key = &tempkey;
		ret_code = sha204h_gen_dig(&gendig_param);
		if (ret_code != SHA204_SUCCESS) {
			sha204e_wakeup_sleep();
			continue;
		}

		// ----------------------- MAC --------------------------------------------
		// Wake up the device from Idle mode.
		memset(wakeup_response, 0, sizeof(wakeup_response));
		ret_code = sha204c_wakeup(wakeup_response);
		if (ret_code != SHA204_SUCCESS) {
			(void) sha204p_sleep();
			continue;
		}
		
		// Issue a MAC command with mode = 3.
		memset(rx_buffer, 0, sizeof(rx_buffer));
		ret_code = sha204m_execute(SHA204_MAC, mac_mode, SHA204_KEY_ID,
			0, NULL, 0, NULL, 0, NULL, sizeof(tx_buffer), tx_buffer, sizeof(rx_buffer), rx_buffer);		 

		// Put device to sleep.
		sha204p_sleep();

		if (ret_code != SHA204_SUCCESS)
			continue;
		
		// Calculate MAC using helper function.
		mac_param.mode = mac_mode;
		mac_param.key_id = SHA204_KEY_ID;
		mac_param.challenge = NULL;
		mac_param.key = NULL;
		mac_param.otp = NULL;
		mac_param.sn = NULL;
		mac_param.response = mac;
		mac_param.temp_key = &tempkey;
		ret_code = sha204h_mac(&mac_param);
		if (ret_code != SHA204_SUCCESS)
			continue;
		
		// Compare the Mac response with the calculated MAC.
		// Make this loop resistant against a timing attack.
		comparison_result = 0;
		for (i = 0; i < sizeof(mac); i++)
			comparison_result |= (rx_buffer[i + SHA204_BUFFER_POS_STATUS] ^ mac[i]);

		ret_code = (comparison_result ? SHA204_GEN_FAIL : SHA204_SUCCESS);
	}

	return ret_code;
}
/** @} */
