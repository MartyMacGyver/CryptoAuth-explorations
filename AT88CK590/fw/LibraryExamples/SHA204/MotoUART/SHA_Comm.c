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
 *  \brief 	communication layer of SHA204 library
 *  \author G&uuml;nter Fuchs
 *  \date 	Feb 21, 2010
 */

#include <stdint.h>
#include "SHA_Comm.h"
#include "SHA_CommInterface.h"
#include "SHA_TimeUtils.h"
#include "SHA_Status.h"


/** \brief Calculates CRC
 *
 * \param[in] length number of bytes in buffer
 * \param[in] data pointer to data for which CRC should be calculated
 * \param[out] crc pointer to 16-bit CRC
 */
void SHAC_CalculateCrc(uint8_t length, uint8_t *data, uint8_t *crc) {
	uint8_t counter;
	uint8_t crcLow = 0, crcHigh = 0, crcCarry;
	uint8_t polyLow = 0x05, polyHigh = 0x80;
	uint8_t shiftRegister;
	uint8_t dataBit, crcBit;

	for (counter = 0; counter < length; counter++) {
	  for (shiftRegister = 0x01; shiftRegister > 0x00; shiftRegister <<= 1) {
		 dataBit = (data[counter] & shiftRegister) ? 1 : 0;
		 crcBit = crcHigh >> 7;

		 // Shift CRC to the left by 1.
		 crcCarry = crcLow >> 7;
		 crcLow <<= 1;
		 crcHigh <<= 1;
		 crcHigh |= crcCarry;

		 if ((dataBit ^ crcBit) != 0) {
			crcLow ^= polyLow;
			crcHigh ^= polyHigh;
		 }
	  }
	}
	crc[SHA_CRC_BYTE_INDEX_LOW] = crcLow;
	crc[SHA_CRC_BYTE_INDEX_HIGH] = crcHigh;
}


/** \brief Sends a wakeup pulse and delays. */
void SHAC_Wakeup()
{
	SHAP_SetSignalPin(0);
	SHAP_Delay(60);
	SHAP_SetSignalPin(1);
	SHAP_Delay(3000);
}


/** \brief Runs a communication sequence: Append CRC to tx buffer, send command, delay, receive and verify response.
 *
 * The first byte in tx buffer must be its byte count.
 * If CRC or count of the response is incorrect, or a command byte got "nacked" (TWI),
 * this function requests to re-send the response.
 * If the response contains an error status, this function resends the command.
 *
 * \param[in] params pointer to parameter structure
 * \return status of the operation
 */
int8_t SHAC_SendAndReceive(SHA_CommParameters *params)
{
	uint8_t rxSize = params->rxSize;
	uint8_t *rxBuffer = params->rxBuffer;
	uint8_t *txBuffer = params->txBuffer;
	uint8_t count;
	uint8_t countMinusCrc;
	int8_t status;
	uint8_t nRetries;
	uint8_t i;
	uint8_t statusByte;
	uint8_t crc[SHA_CRC_SIZE];

	if (!params)
		return SHA_BAD_PARAM;
	if (!params->txBuffer)
		return SHA_BAD_PARAM;

	count = txBuffer[SHA_BUFFER_POS_COUNT];
	countMinusCrc = count - SHA_CRC_SIZE;
	if (count < SHA_COMMAND_SIZE_MIN || rxSize < SHA_RESPONSE_SIZE_MIN || !rxBuffer)
		return SHA_BAD_PARAM;

	// Append CRC and send command.
	SHAC_CalculateCrc(countMinusCrc, txBuffer, txBuffer + countMinusCrc);
	status = SHAP_SendCommand(count, txBuffer);

	if (status != SHA_SUCCESS) {
		// Re-send command.
		status = SHAP_SendCommand(count, txBuffer);
		if (status != SHA_SUCCESS) {
			// We lost communication. Wait until device goes to sleep.
			SHAP_Delay(WATCHDOG_TIMEOUT * 1000000);
			return status;
		}
	}

	// Wait for device to finish command execution.
	SHAP_Delay(params->executionDelay);

	// Receive response.
	nRetries = SHA_RETRY_COUNT;
	do {
		// Reset response buffer.
		for (i = 0; i < rxSize; i++)
			rxBuffer[i] = 0;

		status = SHAP_ReceiveResponse(rxSize, rxBuffer);
		if (status != SHA_SUCCESS && !rxBuffer[SHA_BUFFER_POS_COUNT]) {
			// We lost communication. Wait until device goes to sleep.
			//SHAP_Delay(WATCHDOG_TIMEOUT * 1000000);
			return status;
		}

		// Check whether we received a status packet instead of a full response.
		if (rxSize != SHA_RESPONSE_SIZE_MIN && rxBuffer[SHA_BUFFER_POS_COUNT] == SHA_RESPONSE_SIZE_MIN) {
			statusByte = rxBuffer[SHA_BUFFER_POS_STATUS];
			if (statusByte == SHA_STATUS_BYTE_PARSE)
				return SHA_PARSE_ERROR;
			if (statusByte == SHA_STATUS_BYTE_EXEC)
				return SHA_CMD_FAIL;
			if (statusByte != SHA_STATUS_BYTE_COMM)
				return SHA_STATUS_UNKNOWN;

			// Communication error. Request device to re-transmit response.
			continue;
		}

		// Received response. Verify count and CRC.
		if (rxBuffer[SHA_BUFFER_POS_COUNT] != rxSize) {
			status = SHA_BAD_SIZE;
			continue;
		}
		countMinusCrc = rxSize - SHA_CRC_SIZE;
		SHAC_CalculateCrc(countMinusCrc, rxBuffer, crc);
		status = ((rxBuffer[countMinusCrc] == crc[SHA_CRC_BYTE_INDEX_LOW])
				&& (rxBuffer[countMinusCrc + 1] == crc[SHA_CRC_BYTE_INDEX_HIGH]))
			? SHA_SUCCESS : SHA_BAD_CRC;
	} while (--nRetries && status != SHA_SUCCESS);

	return status;
}
