// ----------------------------------------------------------------------------
//         ATMEL Crypto-Devices Software Support  -  Colorado Springs, CO -
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
 * \brief This file contains functions that parse ASCII protocol commands
 *        destined for an ATSHA204 device.
 * \author Atmel Crypto Products
 * \date July 30, 2013
 */

#include <string.h>

#include "utilities.h"
#include "kitStatus.h"
#include "config.h"

#if TARGET_BOARD == NO_TARGET_BOARD
#   error You have to define one of the target boards listed in config.h. 
#endif

#if TARGET_BOARD == AT88CK454H
#   include "sha204Verify.h"
#endif

#include "sha204_comm.h"
#include "sha204_lib_return_codes.h"
#include "sha204_comm_marshaling.h"
#include "sha204_physical.h"
#include "parserAscii.h"

#ifdef ECC108
#   include "ecc108_physical.h"
#endif

#if defined(SHA204) && defined(AES132) && !defined(BOARD)
#   include "Combined_Physical.h"
#endif

/** \brief This variable tells the library how long to poll for a command 
			response before timing out.
*/
uint8_t command_execution_time;

/** \brief This flag causes this parser to wrap a command inside a Wakeup 
			and Idle.
			
			We need this because of an ATECC108 SWI problem where the device
			goes to sleep before a USB host could send a "talk" message 
			after having received a USB reply to a Wakeup message (about 60 ms).
*/
uint8_t send_wakeup_idle_with_command = 1;


/** \brief This function returns the size of the expected response in bytes,
 *         given a properly formatted SHA204 command.
 *
 * The data load is expected to be in Hex-Ascii and surrounded by parentheses.
 * \param[in] cmdBuf pointer to the properly formatted SHA204 command buffer
 * \return the size of the expected response in bytes
 */
static uint16_t GetSha204ResponseSize(uint8_t *cmdBuf)
{
	// Get the Opcode and Param1
	uint8_t opCode = cmdBuf[SHA204_OPCODE_IDX];
	uint8_t param1 = cmdBuf[SHA204_PARAM1_IDX];
	
	// Return the expected response size
	switch (opCode) {
	case SHA204_CHECKMAC:
		command_execution_time = CHECKMAC_EXEC_MAX;
		return CHECKMAC_RSP_SIZE;

	case SHA204_DERIVE_KEY:
		command_execution_time = DERIVE_KEY_EXEC_MAX;
		return DERIVE_KEY_RSP_SIZE;

	case SHA204_DEVREV:
		command_execution_time = DEVREV_EXEC_MAX;
		return DEVREV_RSP_SIZE;

	case SHA204_GENDIG:
		command_execution_time = GENDIG_EXEC_MAX;
		return GENDIG_RSP_SIZE;

	case SHA204_HMAC:
		command_execution_time = HMAC_EXEC_MAX;
		return HMAC_RSP_SIZE;

	case SHA204_LOCK:
		command_execution_time = LOCK_EXEC_MAX;
		return LOCK_RSP_SIZE;

	case SHA204_MAC:
		command_execution_time = MAC_EXEC_MAX;
		return MAC_RSP_SIZE;

	case SHA204_NONCE:
		command_execution_time = NONCE_EXEC_MAX;
		return ((param1 & NONCE_MODE_MASK) == NONCE_MODE_PASSTHROUGH
					? NONCE_RSP_SIZE_SHORT : NONCE_RSP_SIZE_LONG);

	case SHA204_PAUSE:
		command_execution_time = PAUSE_EXEC_MAX;
		return PAUSE_RSP_SIZE;

	case SHA204_RANDOM:
		command_execution_time = RANDOM_EXEC_MAX;
		return RANDOM_RSP_SIZE;

	case SHA204_READ:
		command_execution_time = READ_EXEC_MAX;
		return (param1 & READ_ZONE_MODE_32_BYTES ? READ_32_RSP_SIZE : READ_4_RSP_SIZE);

	case SHA204_UPDATE_EXTRA:
		command_execution_time = UPDATE_EXEC_MAX;
		return UPDATE_RSP_SIZE;

	case SHA204_WRITE:
		command_execution_time = WRITE_EXEC_MAX;
		return WRITE_RSP_SIZE;
		
	default:
		// Temporary fix to support ECC108.
		command_execution_time = 200;
	}
	// Return the max size for all other commands.
#ifdef ECC108
	return ECC108_RESPONSE_SIZE_MAX;
#else	
	return SHA204_RSP_SIZE_MAX;
#endif	
}


/** \brief This function parses communication commands (ASCII) received from a
 *         PC host and returns a binary response.
 *
 *         protocol syntax:\n\n
 *         functions for command sequences:\n
 *            v[erify]                            several Communication and Command Marshaling layer functions
 *            a[tomic]                            Wraps "talk" into a Wakeup / Idle.
 *         functions in sha204_comm.c (Communication layer):\n
 *            w[akeup]                            sha204c_wakeup\n
 *            t[alk](command)                     sha204c_send_and_receive\n
 *         functions in sha204_i2c.c / sha204_swi.c (Physical layer):\n
 *            [physical:]s[leep]                  sha204p_sleep\n
 *            [physical:]i[dle]                   sha204p_idle\n
 *            p[hysical]:r[esync]                 sha204p_resync\n
 *            p[hysical]:e[nable]                 sha204p_init\n
 *            p[hysical]:d[isable]                sha204p_disable_interface\n
 *            c[ommand](data)                     sha204p_send_command\n
 *            r[esponse](size)                    sha204p_receive_response\n
 * \param[in] commandLength number of bytes in command buffer
 * \param[in] command pointer to ASCII command buffer
 * \param[out] responseLength pointer to number of bytes in response buffer
 * \param[out] response pointer to binary response buffer
 * \return the status of the operation
 */
uint8_t ParseShaCommands(uint16_t commandLength, uint8_t *command, uint16_t *responseLength, uint8_t *response)
{
	uint8_t status = KIT_STATUS_SUCCESS;
	uint16_t dataLength;
	uint8_t *data_load[1];
	uint8_t *dataLoad;
	uint16_t response_size;
	char *pToken = strchr((char *) command, ':');

	*responseLength = 0;

	if (!pToken)
		return status;

	// Talk (send command and receive response)
	switch (pToken[1]) {
	case 't':
		status = ExtractDataLoad(pToken + 2, &dataLength, data_load);
		if (status != KIT_STATUS_SUCCESS)
			return status;

		// Reset count byte.
		response[SHA204_BUFFER_POS_COUNT] = 0;

		if (send_wakeup_idle_with_command) {
			status = sha204c_wakeup(response);
			if (status != KIT_STATUS_SUCCESS)
				break;
		}

		// Send command and receive response.
		response_size = GetSha204ResponseSize(data_load[0]); // Also updates sha204_command_execution_time.
		
		// Because of the command flag errata for the ECC108 SWI device version 0x10, we have to poll.
		// Because of the SHA204 library, the command execution time cannot be set higher than 0xFFFF minus 
		// SHA204_RESPONSE_TIMEOUT. HMAC has the greatest execution time of 69 ms, so starting to poll
		// after 5 ms keeps the last parameter in the function below inside the uint16_t range.
		status = sha204c_send_and_receive(data_load[0], response_size, &response[0], 5, command_execution_time - 5);
		if (status >= SHA204_CHECKMAC_FAILED && status <= SHA204_STATUS_UNKNOWN)
			// Reset status if the function returned error because the response status byte indicates error.
			status = KIT_STATUS_SUCCESS;
		*responseLength = response[SHA204_BUFFER_POS_COUNT];

		if (send_wakeup_idle_with_command)
			status = sha204p_idle();
		break;

#if TARGET_BOARD == AT88CK454H
	// Verify diversified key.
	case 'v':
		status = ExtractDataLoad(pToken + 2, &dataLength, data_load);
		if (status != KIT_STATUS_SUCCESS)
			return status;
		status = sha204VerifyDiversifiedKey(dataLength, data_load[0], &response[0], pToken[2] == 'd');
		if (status >= SHA204_CHECKMAC_FAILED && status <= SHA204_STATUS_UNKNOWN)
			// Reset status if the function returned error because the response status byte indicates error.
			status = SHA204_SUCCESS;
		*responseLength = response[SHA204_BUFFER_POS_COUNT];
		break;
#endif

	// Wake.
	case 'w':
		if (send_wakeup_idle_with_command) {
			// Don't send Wakeup now but only once a "talk" message has been received.
//			status = KIT_STATUS_SUCCESS;
			response[0] = 0x04; response[1] = 0x11; response[2] = 0x33; response[3] = 0x43;
			*responseLength = SHA204_RSP_SIZE_MIN;
			break;
		}
		// Send the wake-up pulse and receive response.
		status = sha204c_wakeup(response);
		*responseLength = response[SHA204_BUFFER_POS_COUNT];
		break;

	// The commands below are translated into calls to
	// Physical layer functions. But for downward compatibility
	// the "physical:" in the command string is optional.
	// send command
	case 'c':
		status = ExtractDataLoad(pToken + 2, &dataLength, data_load);
		if (status != KIT_STATUS_SUCCESS)
			return status;
		dataLoad = data_load[0];
		status = sha204p_send_command((uint8_t) dataLength, dataLoad);
		break;

	// receive response
	case 'r':
		status = ExtractDataLoad(pToken + 2, &dataLength, data_load);
		if (status != KIT_STATUS_SUCCESS)
			return status;
		// Reset count byte.
		response[SHA204_BUFFER_POS_COUNT] = 0;
		status = sha204p_receive_response(*data_load[0], response);
		*responseLength = response[SHA204_BUFFER_POS_COUNT];
		break;

	// Sleep
	case 's':
		if (send_wakeup_idle_with_command) {
			status = sha204c_wakeup(response);
			if (status != KIT_STATUS_SUCCESS)
				break;
		}
		status = sha204p_sleep();
		break;

	// Idle
	case 'i':
		if (send_wakeup_idle_with_command) {
			// Don't send Idle now but only after the response to command has been received.
//			status = KIT_STATUS_SUCCESS;
			break;
		}
		status = sha204p_idle();
		break;
		
	// Switch whether to wrap a Wakeup / Idle around a "talk" message.
	case 'a':
		status = ExtractDataLoad(pToken + 2, &dataLength, data_load);
		if (status != KIT_STATUS_SUCCESS)
			return status;
		send_wakeup_idle_with_command = *data_load[0];
		break;


	// --------- calls functions in sha204_i2c.c and sha204_swi.c  ------------------
	case 'p':
		// ----------------------- "s[ha204]:p[hysical]:" ---------------------------
		pToken = strchr(&pToken[1], ':');
		if (!pToken)
			return status;

		switch (pToken[1]) {
		// Wake-up without receive.
		case 'w':
			status = sha204p_wakeup();
			break;

		case 'c':
			// Send command.
			status = ExtractDataLoad(pToken + 2, &dataLength, data_load);
			if (status != KIT_STATUS_SUCCESS)
				return status;
			dataLoad = data_load[0];
			status = sha204p_send_command((uint8_t) dataLength, dataLoad);
			break;

		// Receive response.
		case 'r':
			status = ExtractDataLoad(pToken + 2, &dataLength, data_load);
			if (status != KIT_STATUS_SUCCESS)
				return status;
			// Reset count byte.
			response[SHA204_BUFFER_POS_COUNT] = 0;
			status = sha204p_receive_response(*data_load[0], response);
			*responseLength = response[SHA204_BUFFER_POS_COUNT];
			break;

		case 's':
			if (pToken[2] == 'y') {
				// "sy[nc]"
				status = sha204p_resync(SHA204_RSP_SIZE_MIN, response);
				*responseLength = response[SHA204_BUFFER_POS_COUNT];
			}
			else {
				// -- "s[elect](device index | TWI address)" or "s[leep]" ----------------
				status = ExtractDataLoad(pToken + 2, &dataLength, data_load);
				if (status == KIT_STATUS_SUCCESS) {
					// Select device (I2C: address; SWI: index into GPIO array).
					dataLoad = data_load[0];
					sha204p_set_device_id(dataLoad[0]);
				}
				else
					// Sleep command
					status = sha204p_sleep();
			}
			break;


#if defined(SHA204) && defined(AES132) && !defined(PARSER_ONE_INTERFACE)
// indicates combined library support
		// ---------------------- "{e[nable] | d[isable]} ---------------------
		case 'e':
			// Enables the current interface.
			sha204p_init();
			status = KIT_STATUS_SUCCESS;
			break;

		case 'd':
			// Disables the current interface.
			sha204p_disable_interface();
			status = KIT_STATUS_SUCCESS;
			break;

		case 'i':
			// ---------------------- "i[nt]:{i[2c] | s[wi]} ------------------------
			pToken = strchr(&pToken[1], ':');
			if (pToken) {
				// Set and enable interface (I2C or SWI).
				if (pToken[1] == 'i')
					status = sha204p_set_interface(DEVKIT_IF_I2C);
				else if (pToken[1] == 's')
					status = sha204p_set_interface(DEVKIT_IF_SWI);
			}
			else {
				// Idle command
				status = sha204p_idle();
			}
			break;
#endif

		default:
			status = KIT_STATUS_UNKNOWN_COMMAND;
			break;
			
		} // end physical
		break;

	default:
		status = KIT_STATUS_UNKNOWN_COMMAND;
		break;
	}
	return status;
}

