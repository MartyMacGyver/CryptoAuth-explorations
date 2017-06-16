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
 *  \brief  Example of an Application That Uses the SHA204 Library
 *  \author Atmel Crypto Products
 *  \date   September 29, 2010
 *  \todo Leave only one Nonce / MAC pair and move everything else
 *        to the test module.
*/

#include <stddef.h>

#include "sha204_lib_return_codes.h"  //!< declarations of function return codes
#include "sha204_comm_marshaling.h"   //!< definitions and declarations for the Command module


/** \brief This function evaluates a function return code
 *         and puts the device to sleep if the return code
 *         indicates that the device is awake.
 * \param[in] ret_code return code of the last call to a SHA204 library function
 */
void evaluate_ret_code(uint8_t ret_code)
{
	if ((ret_code == SHA204_PARSE_ERROR)
				|| (ret_code == SHA204_CMD_FAIL)
				|| (ret_code == SHA204_RX_FAIL))
		// We got some kind of response. Return codes of
		// SHA204_PARSE_ERROR and SHA204_CMD_FAIL indicate
		// a consistent response whereas SHA204_RX_FAIL
		// just indicates that we received some bytes,
		// possibly garbled. In all these cases we put
		// the device to sleep.
		(void) sha204p_sleep();
}



/** \brief This function serves as an example that shows
 *         how to use the SHA204 Nonce and MAC commands.
 *
 *         In an infinite loop, it issues the same command
 *         sequence, first using the Communication layer
 *         and then the Command Marshaling layer of the
 *         SHA204 library.
 * @return exit status of application
 */
int main(void)
{
	// declared as "static" for easier debugging
	static uint8_t ret_code;

	// Make the command buffer the size of the longest
	// command we shall be using, the Nonce command.
	static uint8_t command[NONCE_COUNT_LONG];

	// Make the response buffer the size of the longest
	// response we expect, the MAC response.
	static uint8_t response[SHA204_RSP_SIZE_MAX];

	// expected MAC response in pass-through mode
	const uint8_t mac_response_expected[SHA204_RSP_SIZE_MAX] = {
		// count
		SHA204_RSP_SIZE_MAX,
		// MAC
		0xEF, 0x85, 0x7D, 0xA0, 0x9A, 0xE6, 0x7A, 0xA0,
		0x42, 0x69, 0x1D, 0xF3, 0xE9, 0xEA, 0xD1, 0x57,
		0xD9, 0x95, 0x44, 0xB1, 0x55, 0x17, 0xBB, 0x70,
		0x76, 0x08, 0xD1, 0x63, 0x62, 0xB6, 0x1D, 0x91,
		// CRC
		0x11, 0x2C
	};

// factory default keys
//	  0000A1AC57FF404E45D40401BD0ED3C673D3B7B82D85D9F313B55EDA3D940000
//   111123B6CC53B7B9E9BB51FD2F74CD0E91D97FEB847B9809F4CD936AB6481111
//   2222C17C1C4D5689AA0043E39CFB6B0B6849E32C24A31B0634491E906B622222
//   333333614A179A236C7FE4BE2F132067903DB51C72E0C931296DF45A3E443333
//   44449118683DB8D3F8570C742EDADA52888730A509185456C9A17238CF3C4444
//   555586F2B32098A6E1E6337A5201036A0DB50402021C55B257DF0C735F055555
//   6666D0453AC22557F6D46B7DDF9689DA2CBCD9C35AD59A42DE3032CD25FC6666
//   77772F4A9CC05E4599BD2696DD49F8A506C8B639CD3DA84CC6D13C32CA0F7777
//   8888C62AFE1F82D4E08585344D77B89DEC36F20627E4F0CF030E27B8EEE38888
//   99994E6D4AF592306BD2D5277D77B395E3C3508CDAE0981F9D2817988DF49999
//   AAAA15A2550BD2EA9AF29646156911129612F6F729FD507C9AA26792A144AAAA
//   BBBB24DB78A87064A1F08DC91796600AFF62D4C44C3E10202AAA8FECB68ABBBB
//   CCCCC6171A5245ACD292462890624CA5662B22BBD195DA2A9E49B808850DCCCC
//   DDDDBFAC1170559CC9B6280F9295DF300DEA22A0654E21C9CE74105A65D2DDDD
//   EEEE085577BDA7B8A7AF58D18B92F0DF79AD055E4282E9421ED13D7BBD2EEEEE
//   FFFF68B7B801BE662CEC74680FE47DC1C672543AE5BEDA2E919AE50D32A1FFFF

   // expected MAC responses in mode 0
	static const uint8_t mac_mode0_response_expected[SHA204_KEY_ID_MAX + 1][SHA204_RSP_SIZE_MAX] =
	{
		{  // slot 0
			SHA204_RSP_SIZE_MAX,                             // count
			0x06, 0x67, 0x00, 0x4F, 0x28, 0x4D, 0x6E, 0x98,
			0x62, 0x04, 0xF4, 0x60, 0xA3, 0xE8, 0x75, 0x8A,
			0x59, 0x85, 0xA6, 0x79, 0x96, 0xC4, 0x8A, 0x88,
			0x46, 0x43, 0x4E, 0xB3, 0xDB, 0x58, 0xA4, 0xFB,
			0xE5, 0x73                                       // CRC
		},
		{  // slot 1
			SHA204_RSP_SIZE_MAX,                             // count
			0x72, 0xC2, 0x7F, 0x94, 0x19, 0xC6, 0x5A, 0xDC,
			0x6E, 0xEA, 0x23, 0x36, 0x7E, 0x38, 0xC3, 0x5F,
			0x85, 0x13, 0x50, 0xB2, 0xD3, 0x28, 0xDD, 0x0C,
			0x30, 0x59, 0x1D, 0x5D, 0x5D, 0x87, 0xEA, 0xF9,
			0x0B, 0x61                                       // CRC
		},
		{  // slot 2
			SHA204_RSP_SIZE_MAX,                             // count
			0xF9, 0xDB, 0x11, 0xEC, 0x16, 0x1A, 0xE8, 0x24,
			0x44, 0xBB, 0xE4, 0x0C, 0xBE, 0x05, 0x63, 0x2B,
			0x23, 0x4E, 0x94, 0x39, 0xF8, 0x56, 0xAB, 0xE7,
			0x50, 0x8D, 0x65, 0x62, 0xC5, 0xA5, 0xE9, 0x29,
			0x39, 0x0E                                       // CRC
		},
		{  // slot 3, not checked because factory configuration is Single Use
			SHA204_RSP_SIZE_MAX,                             // count
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00                                       // CRC
		},
		{  // slot 4, not checked because factory configuration is CheckMac
			SHA204_RSP_SIZE_MAX,                             // count
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00                                       // CRC
		},
		{  // slot 5, not checked because factory configuration is DeriveKey
			SHA204_RSP_SIZE_MAX,                             // count
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00                                       // CRC
		},
		{  // slot 6
			SHA204_RSP_SIZE_MAX,                             // count
			0x5E, 0xCD, 0xB7, 0x0A, 0xB8, 0x4F, 0x69, 0x70,
			0x06, 0x95, 0xA7, 0x82, 0x53, 0xA8, 0xFE, 0xD2,
			0x68, 0x26, 0x31, 0x52, 0x69, 0xA9, 0xBF, 0xE3,
			0x29, 0x9B, 0x1B, 0xE0, 0x78, 0x71, 0xEC, 0xB9,
			0xE0, 0xCA                                       // CRC
		},
		{  // slot 7
			SHA204_RSP_SIZE_MAX,                             // count
			0xE2, 0x63, 0x5B, 0x3B, 0x4C, 0x35, 0x0F, 0x26,
			0x55, 0x90, 0xF7, 0x0F, 0x21, 0x7C, 0x3B, 0xBB,
			0xDE, 0xB0, 0xE9, 0xD4, 0x4A, 0x27, 0xF1, 0x9D,
			0xA9, 0xDF, 0xFF, 0x19, 0x04, 0x64, 0x17, 0x47,
			0x19, 0x39                                       // CRC
		},
		{  // slot 8
			SHA204_RSP_SIZE_MAX,                             // count
			0xDF, 0xF0, 0xFA, 0xA3, 0x8B, 0xCE, 0x5D, 0x88,
			0xB8, 0x8D, 0x57, 0x7D, 0x6F, 0x2B, 0x5C, 0xDA,
			0x77, 0xE4, 0x0F, 0x7F, 0xE0, 0x35, 0xCA, 0x29,
			0x54, 0xFC, 0xFF, 0x15, 0xD3, 0x1A, 0x56, 0x67,
			0x1C, 0x32                                       // CRC
		},
		{  // slot 9
			SHA204_RSP_SIZE_MAX,                             // count
			0xE3, 0x9C, 0x55, 0x91, 0xD4, 0x65, 0x37, 0xEB,
			0x1B, 0x26, 0xAE, 0x08, 0x45, 0xD1, 0x92, 0xF5,
			0x09, 0xF9, 0x61, 0x3D, 0x0B, 0x1D, 0x3A, 0x05,
			0xA3, 0x44, 0xBF, 0x97, 0x7B, 0xC0, 0xCE, 0x9D,
			0x36, 0x90                                       // CRC
		},
		{  // slot 10
			SHA204_RSP_SIZE_MAX,                             // count
			0xD5, 0x72, 0x3F, 0xA8, 0xBF, 0x72, 0x8F, 0x93,
			0x9C, 0x56, 0x5D, 0x95, 0xD4, 0x1B, 0xB4, 0x60,
			0x20, 0x57, 0xD3, 0xE9, 0xB7, 0x7E, 0x62, 0xBF,
			0x09, 0xF7, 0xFC, 0x3E, 0xA6, 0xE2, 0xDC, 0xBE,
			0xF6, 0x8A                                       // CRC
		},
		{  // slot 11
			SHA204_RSP_SIZE_MAX,                             // count
			0xDA, 0xD1, 0x76, 0xC9, 0x3A, 0xFB, 0x72, 0xE9,
			0xB5, 0x06, 0xFB, 0x41, 0xBD, 0x53, 0xF2, 0x7D,
			0xB9, 0xBF, 0xB4, 0xF3, 0xCC, 0xA5, 0x97, 0x1F,
			0xA6, 0xD4, 0xE5, 0x0A, 0x2C, 0x24, 0x89, 0x7A,
			0xBD, 0xD7                                       // CRC
		},
		{  // slot 12
			SHA204_RSP_SIZE_MAX,                             // count
			0xC6, 0x9B, 0x92, 0x30, 0xEB, 0x52, 0x9E, 0x3D,
			0xC0, 0xC3, 0x54, 0x8A, 0x86, 0xBB, 0x12, 0xD9,
			0xE7, 0x67, 0xD4, 0x10, 0xCA, 0xCD, 0xEA, 0x41,
			0xC4, 0x1F, 0x59, 0x06, 0xCC, 0x26, 0x7A, 0x2E,
			0xFB, 0x52                                       // CRC
		},
		{  // slot 13, not checked because factory configuration is DeriveKey / Parent Key
			SHA204_RSP_SIZE_MAX,                             // count
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00                                       // CRC
		},
		{  // slot 14
			SHA204_RSP_SIZE_MAX,                             // count
			0x5B, 0xDB, 0xF0, 0x74, 0x6D, 0x1C, 0x1F, 0xCC,
			0x9F, 0xD4, 0xCD, 0x90, 0xB2, 0x46, 0x47, 0xB2,
			0xF7, 0x75, 0xA2, 0x02, 0x95, 0x1A, 0x90, 0xFD,
			0x2B, 0x58, 0x3E, 0x9A, 0x60, 0x56, 0x5D, 0x61,
			0xD9, 0xCF                                       // CRC
		},
		{  // slot 15
			SHA204_RSP_SIZE_MAX,                             // count
			0x96, 0x8C, 0x35, 0x14, 0x70, 0xBE, 0x44, 0x82,
			0x28, 0x45, 0x48, 0xC9, 0xE0, 0x25, 0x8F, 0x71,
			0xD4, 0xB9, 0x7F, 0xCA, 0x1C, 0xBD, 0xE6, 0xC5,
			0x23, 0x8B, 0x15, 0x7A, 0xE5, 0x92, 0xB0, 0x37,
			0xA1, 0xA8                                       // CRC
//			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//			0x00, 0x00                                       // CRC
		}
	};

	// data for numin in Nonce command and challenge in MAC mode 0 command
	const uint8_t numin[NONCE_NUMIN_SIZE_PASSTHROUGH] = {
		0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
		0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
		0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
		0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
	};

	uint8_t i;
	static uint16_t key_id; // "static" for debugging
	const uint8_t *challenge = numin;


	// Initialize the hardware interface.
	// Depending on which interface you have linked the
	// library to, it initializes SWI UART, SWI GPIO,
	// or TWI.
	sha204p_init();


	while (1) {
		// The following code sequence issues a Nonce command in Pass-Through
		// mode and a subsequent MAC command using the Communication layer.
		// Remember that when putting break points the device has probably
		// gone to sleep before you hit "continue". The time for the device
		// going to sleep lies between 700 and 1500 ms.


		// *********** MAC command using Communication layer ****************

		// Wake up the device.
		ret_code = sha204c_wakeup(&response[0]);
		if (ret_code != SHA204_SUCCESS) {
			evaluate_ret_code(ret_code);
			continue;
		}

		// Supply parameters for Nonce command in Pass-Trough mode.
		command[SHA204_COUNT_IDX] = NONCE_COUNT_LONG;
		command[SHA204_OPCODE_IDX] = SHA204_NONCE;
		command[SHA204_PARAM1_IDX] = NONCE_MODE_PASSTHROUGH;
		command[SHA204_PARAM2_IDX] = command[SHA204_PARAM2_IDX + 1] = 0;

		// Supply pass-through key for Nonce command.
		for (i = SHA204_DATA_IDX; i < (NONCE_COUNT_LONG - SHA204_CRC_SIZE); i++)
			command[i] = numin[i - SHA204_DATA_IDX];

		// Send Nonce command and receive response.
		// The called function supplies the CRC.
//		for (i = 0; i < NONCE_RSP_SIZE_SHORT; i++)
//			response[i] = 0;

		ret_code = sha204c_send_and_receive(&command[0], NONCE_RSP_SIZE_SHORT,
					&response[0], NONCE_DELAY, NONCE_EXEC_MAX - NONCE_DELAY);
		if (ret_code != SHA204_SUCCESS) {
			evaluate_ret_code(ret_code);
			continue;
		}

		// Supply parameters for MAC command in Pass-Trough mode.
		command[SHA204_COUNT_IDX] = MAC_COUNT_SHORT;
		command[SHA204_OPCODE_IDX] = SHA204_MAC;
		command[SHA204_PARAM1_IDX] = MAC_MODE_PASSTHROUGH;
		command[SHA204_PARAM2_IDX] = command[SHA204_PARAM2_IDX + 1] = 0;

		// Send MAC command and receive response.
		ret_code = sha204c_send_and_receive(&command[0], MAC_RSP_SIZE,
					&response[0], MAC_DELAY, MAC_EXEC_MAX - MAC_DELAY);
		if (ret_code != SHA204_SUCCESS) {
			evaluate_ret_code(ret_code);
			continue;
		}

		// Put device to sleep.
		ret_code = sha204p_sleep();

		// Compare returned MAC with expected one.
		ret_code = SHA204_SUCCESS;
		for (i = 0; i < SHA204_RSP_SIZE_MAX; i++) {
			if (response[i] != mac_response_expected[i]) {
				ret_code = SHA204_GEN_FAIL;
				break;
			}
		}

		// Put device to sleep.
		ret_code = sha204p_sleep();

		// Compare returned MAC with expected one.
		ret_code = SHA204_SUCCESS;
		for (i = 0; i < SHA204_RSP_SIZE_MAX; i++) {
			if (response[i] != mac_response_expected[i]) {
				ret_code = SHA204_GEN_FAIL;
				break;
			}
		}


		// *********** MAC command mode 0 *************

		// The following code sequence issues a MAC command, mode 0,
		// using the Command Marshaling layer.

		// Iterate through all key identifiers for keys whose factory default
		// configuration is for general MAC use. Identifiers 3, 4, 5, 13, and 15 (???)
		// would return error.
		for (key_id = 0; key_id <= SHA204_KEY_ID_MAX; key_id++) {

			if (((key_id >= 3) && (key_id <= 5)) || (key_id == 13))
				continue;

			// Wake up the device.
			ret_code = sha204c_wakeup(&response[0]);
			if (ret_code != SHA204_SUCCESS) {
				evaluate_ret_code(ret_code);
				continue;
			}

			// Mac command with mode = 0.
			ret_code = sha204m_execute(SHA204_MAC, 0, key_id,
						MAC_CHALLENGE_SIZE, (uint8_t *) challenge, 0, NULL, 0, NULL,
						MAC_COUNT_LONG, &command[0], MAC_RSP_SIZE, &response[0]);
			if (ret_code != SHA204_SUCCESS) {
				evaluate_ret_code(ret_code);
				continue;
			}

			// Compare returned MAC with expected one.
			ret_code = SHA204_SUCCESS;
			for (i = 0; i < SHA204_RSP_SIZE_MAX; i++) {
				if (response[i] != mac_mode0_response_expected[key_id][i]) {
					ret_code = SHA204_GEN_FAIL;
					break;
				}
			}

			// Put device to sleep.
			ret_code = sha204p_sleep();
			if (ret_code != SHA204_SUCCESS)
				break;
		}
	}

	return (int) ret_code;
}

