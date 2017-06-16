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
 *        destined for an ECC108 device.
 * \author Atmel Crypto Products
 * \date May 22, 2013
 */

#include <string.h>

#include "utilities.h"
#include "kitStatus.h"
#include "ecc108_comm.h"
#include "ecc108_lib_return_codes.h"
#include "ecc108_comm_marshaling.h"
#include "ecc108_physical.h"

//#define ECC108_RESPONSE_SIZE_MAX   (75) // Sign and GenKey command response

uint8_t command_execution_time;

/** \brief This function returns the size of the expected response in bytes,
 *         given a properly formatted ECC108 command.
 *
 * The data load is expected to be in Hex-Ascii and surrounded by parentheses.
 * \param[in] cmdBuf pointer to the properly formatted ECC108 command buffer
 * \return the size of the expected response in bytes
 * \todo Add missing ECC commands.
 */
uint16_t GetEcc108ResponseSize(uint8_t *cmdBuf)
{
	// Get the Opcode and Param1
	uint8_t opCode = cmdBuf[ECC108_OPCODE_IDX];
	uint8_t param1 = cmdBuf[ECC108_PARAM1_IDX];

	// Return the expected response size
	switch (opCode) {
	case ECC108_CHECKMAC:
		command_execution_time = CHECKMAC_EXEC_MAX;
		return CHECKMAC_RSP_SIZE;

	case ECC108_DERIVE_KEY:
		command_execution_time = DERIVE_KEY_EXEC_MAX;
		return DERIVE_KEY_RSP_SIZE;

	case ECC108_INFO:
		command_execution_time = INFO_EXEC_MAX;
		return INFO_RSP_SIZE;

	case ECC108_GENDIG:
		command_execution_time = GENDIG_EXEC_MAX;
		return GENDIG_RSP_SIZE;

	case ECC108_HMAC:
		command_execution_time = HMAC_EXEC_MAX;
		return HMAC_RSP_SIZE;

	case ECC108_LOCK:
		command_execution_time = LOCK_EXEC_MAX;
		return LOCK_RSP_SIZE;

	case ECC108_MAC:
		command_execution_time = MAC_EXEC_MAX;
		return MAC_RSP_SIZE;

	case ECC108_NONCE:
		command_execution_time = NONCE_EXEC_MAX;
		return ((param1 & NONCE_MODE_MASK) == NONCE_MODE_PASSTHROUGH
					? NONCE_RSP_SIZE_SHORT : NONCE_RSP_SIZE_LONG);

	case ECC108_PAUSE:
		command_execution_time = PAUSE_EXEC_MAX;
		return PAUSE_RSP_SIZE;

	case ECC108_RANDOM:
		command_execution_time = RANDOM_EXEC_MAX;
		return RANDOM_RSP_SIZE;

	case ECC108_READ:
		command_execution_time = READ_EXEC_MAX;
		return (param1 & READ_ZONE_MODE_32_BYTES ? READ_32_RSP_SIZE : READ_4_RSP_SIZE);

	case ECC108_UPDATE_EXTRA:
		command_execution_time = UPDATE_EXEC_MAX;
		return UPDATE_RSP_SIZE;

	case ECC108_WRITE:
		command_execution_time = WRITE_EXEC_MAX;
		return WRITE_RSP_SIZE;
		
	default:
		// Temporary fix to support missing ECC108 commands.
		command_execution_time = 200;
	}
	// Return the max size for all other commands.
	return ECC108_RESPONSE_SIZE_MAX;
}


/** \brief This function parses communication commands (ASCII) received from a
 *         PC host and returns a binary response.
 *
 *         protocol syntax:\n\n
 *         functions for command sequences:\n
 *            v[erify]                            several Communication and Command Marshaling layer functions
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
uint8_t ParseEccCommands(uint16_t commandLength, uint8_t *command, uint16_t *responseLength, uint8_t *response)
{
	uint8_t status = KIT_STATUS_SUCCESS;
	uint16_t dataLength;
	uint8_t *data[1];
	uint8_t *dataLoad;
	uint16_t response_size;
	char *pToken = strchr((char *) command, ':');

	*responseLength = 0;

	if (!pToken)
		return KIT_STATUS_UNKNOWN_COMMAND;

	// Talk (send command and receive response)
	switch (pToken[1]) {
	case 't':
		status = ExtractDataLoad(pToken + 2, &dataLength, data);
		if (status != KIT_STATUS_SUCCESS)
			return status;

		// Reset count byte.
		// For RhinoWhite, this overwrites the first byte of the command buffer since the USB rx and tx
		// buffers are shared. That's okay as long as the data load is not overwritten.
		response[ECC108_BUFFER_POS_COUNT] = 0;

		// Send command and receive response.
		response_size = GetEcc108ResponseSize(data[0]); // Also updates ecc108_command_execution_time.
		// todo This will not work with SWI because of an ECC108 bug. See fix in the Microbase project.
		status = ecc108c_send_and_receive(data[0], response_size, &response[0], command_execution_time, 0);
		if (status >= ECC108_CHECKMAC_FAILED && status <= ECC108_STATUS_UNKNOWN)
			// Reset status if the function returned error because the response status byte indicates error.
			status = ECC108_SUCCESS;
		*responseLength = response[ECC108_BUFFER_POS_COUNT];
		break;

	// Wake.
	case 'w':
		// Send the wake-up pulse and receive response.
		status = ecc108c_wakeup(response);
		*responseLength = (status == ECC108_SUCCESS ? response[ECC108_BUFFER_POS_COUNT] : 0);
		break;

	// The commands below are translated into calls to
	// Physical layer functions. But for downward compatibility
	// the "physical:" in the command string is optional.
	// send command
	case 'c':
		status = ExtractDataLoad(pToken + 2, &dataLength, data);
		if (status != KIT_STATUS_SUCCESS)
			return status;
		dataLoad = data[0];
		status = ecc108p_send_command((uint8_t) dataLength, dataLoad);
		break;

	// receive response
	case 'r':
		status = ExtractDataLoad(pToken + 2, &dataLength, data);
		if (status != KIT_STATUS_SUCCESS)
			return status;
		// Reset count byte.
		response[ECC108_BUFFER_POS_COUNT] = 0;
		status = ecc108p_receive_response(*data[0], response);
		*responseLength = response[ECC108_BUFFER_POS_COUNT];
		break;

	// Sleep
	case 's':
		status = ecc108p_sleep();
		break;

	// Idle
	case 'i':
		status = ecc108p_idle();
		break;


	// --------- functions in ecc108_i2c.c and ecc108_swi.c  --------------------
	case 'p':
		// ----------------------- "e[cc108]:p[hysical]:" ---------------------------
		pToken = strchr(&pToken[1], ':');
		if (!pToken)
			return KIT_STATUS_UNKNOWN_COMMAND;

		switch (pToken[1]) {
		// Wake-up without receive.
		case 'w':
			status = ecc108p_wakeup();
			break;

		case 'c':
			// Send command.
			status = ExtractDataLoad(pToken + 2, &dataLength, data);
			if (status != KIT_STATUS_SUCCESS)
				return status;
			dataLoad = data[0];
			status = ecc108p_send_command((uint8_t) dataLength, dataLoad);
			break;

		// Receive response.
		case 'r':
			status = ExtractDataLoad(pToken + 2, &dataLength, data);
			if (status != KIT_STATUS_SUCCESS)
				return status;
			// Reset count byte.
			response[ECC108_BUFFER_POS_COUNT] = 0;
			status = ecc108p_receive_response(*data[0], response);
			*responseLength = response[ECC108_BUFFER_POS_COUNT];
			break;

		case 's':
			if (pToken[2] == 'y') {
				// "sy[nc]"
				status = ecc108p_resync(ECC108_RESPONSE_SIZE_MIN, response);
				*responseLength = (status == ECC108_SUCCESS ? response[ECC108_BUFFER_POS_COUNT] : 0);
			}
			else {
				// -- "s[elect](device index | TWI address)" or "s[leep]" ----------------
				status = ExtractDataLoad(pToken + 2, &dataLength, data);
				if (status == KIT_STATUS_SUCCESS) {
					// Select device (I2C: address; SWI: index into GPIO array).
					dataLoad = data[0];
					ecc108p_set_device_id(dataLoad[0]);
				}
				else
					// Sleep command
					status = ecc108p_sleep();
			}
			break;

		default:
			status = KIT_STATUS_UNKNOWN_COMMAND;
			break;
			
		} // end switch physical
		
		break; // end case p

		
	default:
		status = KIT_STATUS_UNKNOWN_COMMAND;
		break;
	}
	
	return status;
}

