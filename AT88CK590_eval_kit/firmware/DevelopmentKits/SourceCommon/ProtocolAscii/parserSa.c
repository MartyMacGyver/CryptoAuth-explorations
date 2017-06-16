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
 *        destined for a SA10X device.
 * \author Atmel Crypto Products
 * \date August 30, 2010
 */

#include <string.h>

#include "utilities.h"
#include "kitStatus.h"
#include "SA_Func.h"
#include "SA_Status.h"

#define SA_DEFAULT_OPTIONS   (0)

extern uint8_t SACI_IsHost(void);


/** \brief This function returns the size of the expected response in bytes,
 *         given a properly formatted SA command.
 *
 * \param[in] command pointer to the properly formatted SA command buffer
 * \return the size of the expected response in bytes
 */
uint8_t GetSaCommandResponseSize(uint8_t *command)
{
	switch (((MACcmd_str *) command)->ordinal)
	{
		case 2:   // Read
			return sizeof(SA_Read_out);

		case 4:   // BurnFuse
			return sizeof(SA_BurnFuse_out);

		case 8:   // MAC, Host0
			return SACI_IsHost() ? sizeof(Host0Out) : sizeof(MACout);

		case 16:  // BurnSecure, LoadSram
			return sizeof(SA_BurnSecure_out);

		case 32:  // GenPersonalizationKey
			return sizeof(GPKout);

		case 64:  // Host1
			return sizeof(Host1Out);

		case 128: // Host2
			return sizeof(Host2Out);

		default:
			return sizeof(SA_BurnSecure_out);
	}
	// Return the max size for all commands
	return sizeof(SA_BurnSecure_out);
}


/** \brief This function parses communication commands (ASCII) received from a
 *         PC host and returns a binary response.
 *
 *         protocol syntax:\n\n
 *         functions in SA_Comm.c (Communication layer):\n
 *            w[akeup]                            SAC_Wake\n
 *            s[leep]                             SAC_SendSleepFlag\n
 *            t[alk](command)                     SAC_SendAndReceive\n
 *            c[ommand](data)                     SAC_SendData\n
 *            r[esponse](size)                    SHAP_ReceiveResponse\n
 * \param[in] commandLength number of bytes in command buffer
 * \param[in] command pointer to ASCII command buffer
 * \param[out] responseLength pointer to number of bytes in response buffer
 * \param[out] response pointer to binary response buffer
 * \return the status of the operation
 */
uint8_t ParseSaCommands(uint16_t commandLength, uint8_t *command, uint16_t *responseLength, uint8_t *response)
{
	uint8_t status = KIT_STATUS_UNKNOWN_COMMAND;
	int8_t sa_status;
	uint16_t dataLength;
	uint8_t *data[1];
	uint8_t *dataLoad;
	uint8_t *pByte;
	char *pToken = strchr((char *) command, ':');

	*responseLength = 0;

	if (!pToken)
		return status;

	// Send command.
	if (pToken[1] == 'c') {
		status = ExtractDataLoad(pToken + 2, &dataLength, data);
		if (status != KIT_STATUS_SUCCESS) {
			response[0] = status;
			*responseLength = 1;
			return status;
		}
		dataLoad = data[0];
		status = (uint8_t) SAC_SendData(dataLoad, SA_DEFAULT_OPTIONS);
	}

	// Receive response.
	else if (pToken[1] == 'r') {
		status = ExtractDataLoad(pToken + 2, &dataLength, data);
		if (status != KIT_STATUS_SUCCESS) {
			response[0] = status;
			*responseLength = 1;
			return status;
		}

		pByte = response;
		do {
			sa_status = SAC_ReceiveByte(pByte++);
		} while (dataLength-- && sa_status == SA_SUCCESS);
		*responseLength = response[0];
		status = (uint8_t) sa_status;
	}

	// Talk (SendAndReceive).
	else if (pToken[1] == 't') {
		status = ExtractDataLoad(pToken + 2, &dataLength, data);
		if (status != KIT_STATUS_SUCCESS)
			return status;

		uint8_t response_size = GetSaCommandResponseSize(data[0]);
		sa_status = SAC_SendAndReceive(data[0], response, SA_DELAY_EXEC_MAXIMUM,
					SA_DEFAULT_OPTIONS, &response_size);
		if (sa_status == SA_SUCCESS)
			*responseLength = response[0];
	}

	// Wake up.
	else if (pToken[1] == 'w') {
		// Send the wakeup pulse
		status = (uint8_t) SAC_Wake();
		response[0] = status;
		*responseLength = 1;
	}

	// Sleep.
	else if (pToken[1] == 's') {
		// Put the device to sleep
		SAC_SendSleepFlag();
		status = KIT_STATUS_SUCCESS;
	}

	return status;
}

