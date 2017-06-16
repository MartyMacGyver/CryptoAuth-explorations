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
 *  \brief  Application For Customer Microsoft That Use the SHA204 Library
 *  \author Atmel Crypto Products
 *  \date   August 17, 2012
 *
*/

#include <stdint.h>            // data type definitions
#include <stdbool.h>           // definition for boolean
#include <string.h>            // memcpy()


#include "sha204_comm_marshaling.h"
#include "bitbang_config.h"
#include "sha204_lib_return_codes.h "
#include "hardware.h"


/** \brief This function uses Microbase LED's to indicate
           CheckMac success or failure.
    \param[in] ret_code status byte of CheckMac response
*/
void sha204e_indicate_checkmac_status(uint8_t ret_code)
{
	if (ret_code) {
		uint8_t i;
		// Blink all LED's for one second.
		for (i = 0; i < 5; i++) {
			Led_On();
			_delay_ms(100);
			Led_Off();
			_delay_ms(100);
		}		
	}	
	else {
		// Switch all LED's on for one second.
		Led_On();
		_delay_ms(1000);
		Led_Off();
	}
}


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
		Led_IndicateStatus(ret_code, ret_code, 1);
		return ret_code;
	}
	if (response[SHA204_COUNT_IDX] > SHA204_RSP_SIZE_MIN)
		// Response contains data and not a status.
		return ret_code;
		
	ret_code = response[SHA204_BUFFER_POS_STATUS];
	if (ret_code > 1) {
		// Translate status into bit pattern for LED's.
		// Don't use this function to indicate a failed CheckMac (ret_code = 1).
		uint8_t led_number = (ret_code == SHA204_STATUS_BYTE_COMM ? 0x50 : ret_code << 4);
		sha204p_sleep();
		Led_IndicateStatus(ret_code, led_number, 1);
	}		

	return ret_code;	
}


/** \brief This application authenticates a client device using a CheckMac command
 *         on a host device.
 * \return exit status of application
 */
int main(void)
{
	uint8_t i, check_mac_mode, check_mac_result;
	static uint8_t wakeup_response[SHA204_RSP_SIZE_MIN];
	static uint8_t tx_buffer[SHA204_CMD_SIZE_MAX];
	static uint8_t rx_buffer[SHA204_RSP_SIZE_MAX];
	static uint8_t sn[NONCE_NUMIN_SIZE_PASSTHROUGH];
	static uint8_t mac_challenge[MAC_CHALLENGE_SIZE];
	static uint8_t mac_response[MAC_CHALLENGE_SIZE];
	static uint8_t other_data_gendig[] = {0x1C, 0x04, 0x00, 0x00};
	static uint8_t other_data_mac[CHECKMAC_OTHER_DATA_SIZE];
	static uint8_t key_id[] = {1, 2, 4, 5};
		
	// declared as "static" for easier debugging
	static uint8_t ret_code = SHA204_SUCCESS;

	// Configure GPIO pins that drive LED's.
	Led_Init();
	
	// Show that we entered the main loop.
	Led_On();
	_delay_ms(1000);
	Led_Off();
		
	// Configure the ATSHA204 pins of client and host device.
	sha204p_init();
	
	memset(sn, 0, sizeof(sn));
	memset(mac_challenge, 0, sizeof(mac_challenge));
	memset(other_data_mac, 0, sizeof(other_data_mac));
	
	while (true) {
		// Wakeup client.
		sha204p_set_device_id(CLIENT_ID);
		memset(wakeup_response, 0, sizeof(wakeup_response));
		ret_code = sha204c_wakeup(wakeup_response);
		if (ret_code != SHA204_SUCCESS) {
			Led_IndicateStatus(ret_code, ret_code, 1);
			continue;
		}
			
		// Read serial number.
		ret_code = sha204m_read(tx_buffer, rx_buffer, SHA204_ZONE_CONFIG | READ_ZONE_MODE_32_BYTES, 0);
		ret_code = sha204e_check_response_status(ret_code, rx_buffer);
		if (ret_code != SHA204_SUCCESS)
			continue;
		memcpy(sn, &rx_buffer[SHA204_BUFFER_POS_DATA], 4);
		memcpy(&sn[4], &rx_buffer[SHA204_BUFFER_POS_DATA + 8], 5);

		// Verify diversified keys 2 and 5 using GenDig and keys 1 and 4 using DeriveKey.
		for (i = 0; i < sizeof(key_id); i++) {
			if (i > 0) {
				// Wakeup client.
				sha204p_set_device_id(CLIENT_ID);
				ret_code = sha204c_wakeup(wakeup_response);
				if (ret_code != SHA204_SUCCESS) {
					Led_IndicateStatus(ret_code, ret_code, 1);
					break;
				}
			}			
			check_mac_result = SHA204_SUCCESS;
			
			// Issue MAC command, mode 0, and save data for later use.
			ret_code = sha204m_mac(tx_buffer, rx_buffer, MAC_MODE_CHALLENGE, key_id[i], mac_challenge);
			ret_code = sha204e_check_response_status(ret_code, rx_buffer);
			if (ret_code != SHA204_SUCCESS)
				break;
			memcpy(mac_response, &rx_buffer[SHA204_BUFFER_POS_DATA], sizeof(mac_response));
			memcpy(other_data_mac, &tx_buffer[SHA204_OPCODE_IDX], sizeof(other_data_mac));
		
			// Sleep client.
			sha204p_sleep();
		
			// Wakeup host.
			sha204p_set_device_id(HOST_ID);
			memset(wakeup_response, 0, sizeof(wakeup_response));
			ret_code = sha204c_wakeup(wakeup_response);
			if (ret_code != SHA204_SUCCESS) {
				Led_IndicateStatus(ret_code, ret_code, 1);
				break;
			}
		
			// Issue Nonce command, pass-through mode.
			ret_code = sha204m_nonce(tx_buffer, rx_buffer, NONCE_MODE_PASSTHROUGH, sn);
			ret_code = sha204e_check_response_status(ret_code, rx_buffer);
			if (ret_code != SHA204_SUCCESS)
				break;
			
			if (key_id[i] == 2 || key_id[i] == 5) {
				// Issue GenDig command.
				check_mac_mode = CHECKMAC_MODE_BLOCK1_TEMPKEY | CHECKMAC_MODE_SOURCE_FLAG_MATCH;
				other_data_gendig[2] = key_id[i];
				ret_code = sha204m_gen_dig(tx_buffer, rx_buffer, GENDIG_ZONE_DATA, key_id[i], other_data_gendig);
			}
			else {
				check_mac_mode = CHECKMAC_MODE_CHALLENGE;
				// Issue DeriveKey command.
				ret_code = sha204m_derive_key(tx_buffer, rx_buffer, DERIVE_KEY_RANDOM_FLAG, key_id[i], NULL);				
			}			
			ret_code = sha204e_check_response_status(ret_code, rx_buffer);
			if (ret_code != SHA204_SUCCESS)
				break;
			
			// Issue CheckMac command.
			ret_code = sha204m_check_mac(tx_buffer, rx_buffer, check_mac_mode,
											key_id[i], mac_challenge, mac_response, other_data_mac);
			ret_code = sha204e_check_response_status(ret_code, rx_buffer);
			if (ret_code > 1)
				break;
		
			// Sleep host.
			sha204p_sleep();
			
			// Make a CheckMac error result stick.
			check_mac_result |= ret_code;
		}
		// Indicate CheckMac status.
		sha204e_indicate_checkmac_status(check_mac_result);
	}
		
	return (int) ret_code;
}
