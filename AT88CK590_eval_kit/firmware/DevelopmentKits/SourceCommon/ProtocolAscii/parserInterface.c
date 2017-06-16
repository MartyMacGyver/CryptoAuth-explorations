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
 * \brief This file contains functions for interface commands.
 * \author Atmel Crypto Products
 * \date January 2, 2013
 */

#include <string.h>

#include "kitStatus.h"
#include "utilities.h"


#if defined(SHA204_SWI_BITBANG) | defined(SHA204_SWI_UART)
#   include "swi_phys.h"

/** \brief This function parses SWI commands (ASCII) received from a
 * 			PC host and returns a binary response.
 *
 *         protocol syntax:\n\n
 *         i[nterface]:swi:<bus command>\n
 *             d[river](1, 0)        : enable, disable\n
 *             s[elect device](1, 0) : host, client\n
 *             t[ransmit](data)      : send bytes\n
 *             r[eceive](count)      : receive bytes\n
 *             w[akeup]              : wakeup and receive response\n
 * \param[in] commandLength number of bytes in command buffer
 * \param[in] command pointer to ASCII command buffer after "interface:swi"
 * \param[out] responseLength pointer to number of bytes in response buffer
 * \param[out] response pointer to binary response buffer
 * \return the status of the operation
 */
uint8_t ParseSwiCommands(uint16_t commandLength, uint8_t *command, uint16_t *responseLength, uint8_t *response)
{
	uint8_t status = KIT_STATUS_UNKNOWN_COMMAND;
	uint16_t dataLength;
	uint8_t *data[1];
	char *pToken = strchr((char *) command, ':');

	*responseLength = 0;

	if (!pToken)
		return status;

	// Transmit data.
	if (pToken[1] == 't') {
		status = ExtractDataLoad(pToken + 2, &dataLength, data);
		if (status == KIT_STATUS_SUCCESS) {
			status = swi_send_bytes(dataLength, data[0]);
			*response = status;
			*responseLength = 1;
		}
	}

	// Receive data.
	else if (pToken[1] == 'r') {
		status = ExtractDataLoad(pToken + 2, &dataLength, data);
		if (status == KIT_STATUS_SUCCESS) {
			status = swi_receive_bytes(dataLength, response);
			*response = status;
			*responseLength = dataLength;
		}
	}

	/** \todo Add Wakeup that sends a wakeup pulse and reads its response. */


	if (status != KIT_STATUS_SUCCESS)
		*responseLength = 0;

	return status;
}
#endif


#ifdef I2C
#   include "i2c_phys.h"

/** \brief This function parses hardware dependent I2C commands (ASCII) received from a
 * 		  PC host and returns a binary response.
 *
 *         protocol syntax:\n\n
 *         i[nterface]:i2[c]:<bus command>\n
 *            t[ransmit](data) : send bytes\n
 *            r[eceive](count) : receive bytes\n
 *	           sta[rt]          : create Start condition\n
 *	           sto[p]           : create Stop condition\n
 *	           e[nable]         : enable peripheral\n
 *	           d[isable]        : disable peripheral\n
 * \param[in] commandLength number of bytes in command buffer
 * \param[in] command pointer to ASCII command buffer after "interface:i2c"
 * \param[out] responseLength pointer to number of bytes in response buffer
 * \param[out] response pointer to binary response buffer
 * \return the status of the operation
 */
uint8_t ParseTwiCommands(uint16_t commandLength, uint8_t *command, uint16_t *responseLength, uint8_t *response)
{
	uint8_t status = KIT_STATUS_UNKNOWN_COMMAND;
	uint16_t dataLength;
	uint8_t *data[1];
	char *pToken = strchr((char *) command, ':');

	*responseLength = 0;
	if (!pToken)
		return status;

	// Transmit data.
	if (pToken[1] == 't') {
		status = ExtractDataLoad(pToken + 2, &dataLength, data);
		if (status == KIT_STATUS_SUCCESS) {
			*response = i2c_send_bytes(dataLength, data[0]);
			*responseLength = 1;
		}
	}

	// Receive data.
	else if (pToken[1] == 'r') {
		status = ExtractDataLoad(pToken + 2, &dataLength, data);
		if (status == KIT_STATUS_SUCCESS) {
			status = i2c_receive_bytes(*(data[0]), response);
			if (status == KIT_STATUS_SUCCESS)
				*responseLength = *(data[0]);
		}
	}

	// Create Start or Stop condition.
	else if (pToken[1] == 's') {
		if (pToken[3] == 'a')
			status = i2c_send_start();
		else if (pToken[3] == 'o')
			status = i2c_send_stop();

		*response = status;
		*responseLength = 1;
	}

	if (status != KIT_STATUS_SUCCESS)
		*responseLength = 0;

	return status;
}
#endif


#ifdef SPI
#   include "spi_phys.h"

/** \brief This function parses SPI commands (ASCII) received from a
 * 			PC host and returns a binary response.
 *
 *         protocol syntax:\n\n
 *         i[nterface]:spi:<bus command>\n
 *             d[river](1, 0)        : enable, disable\n
 *             s[elect device](1, 0) : CS low, CS high\n
 *             t[ransmit](data)      : send bytes\n
 *             r[eceive](count)      : receive bytes\n
 * \param[in] commandLength number of bytes in command buffer
 * \param[in] command pointer to ASCII command buffer after "interface:spi"
 * \param[out] responseLength pointer to number of bytes in response buffer
 * \param[out] response pointer to binary response buffer
 * \return the status of the operation
 */
uint8_t ParseSpiCommands(uint16_t commandLength, uint8_t *command, uint16_t *responseLength, uint8_t *response)
{
	uint8_t status = KIT_STATUS_UNKNOWN_COMMAND;
	uint8_t onOff = 0;
	uint16_t dataLength;
	uint8_t *data[1];
	char *pToken = strchr((char *) command, ':');

	*responseLength = 0;
	if (!pToken)
		return status;

	// Enable or disable device driver.
	if (pToken[1] == 'd') {
		status = ExtractDataLoad(pToken + 2, &dataLength, data);
		// If no data found, assume disabling driver.
		if (status == KIT_STATUS_SUCCESS)
			onOff = *(data[0]);

		if (onOff)
			spi_enable();
		else
			spi_disable();
		*response = KIT_STATUS_SUCCESS;
		*responseLength = 1;
	}

	// Set chip select low or high.
	else if (pToken[1] == 's') {
		status = ExtractDataLoad(pToken + 2, &dataLength, data);
		// If no data found, assume CS high (off).
		if (status == KIT_STATUS_SUCCESS)
			onOff = *(data[0]);

		if (onOff)
			spi_select_slave();
		else
			spi_deselect_slave();
		*response = KIT_STATUS_SUCCESS;
		*responseLength = 1;
	}

	// Transmit data.
	else if (pToken[1] == 't') {
		status = ExtractDataLoad(pToken + 2, &dataLength, data);
		if (status == KIT_STATUS_SUCCESS) {
			status = spi_send_bytes(dataLength, data[0]);
			*response = status;
			*responseLength = 1;
		}
	}

	// Receive data.
	else if (pToken[1] == 'r') {
		status = ExtractDataLoad(pToken + 2, &dataLength, data);
		if (status == KIT_STATUS_SUCCESS) {
			status = spi_receive_bytes(dataLength, response);
			*response = status;
			*responseLength = dataLength;
		}
	}

	if (status != KIT_STATUS_SUCCESS)
		*responseLength = 0;

	return status;
}
#endif


/** \brief This function parses interface commands (ASCII) received from a
 * 			PC host and returns a binary response.
 *
 * \param[in] commandLength number of bytes in command buffer
 * \param[in] command pointer to ASCII command buffer
 * \param[out] responseLength pointer to number of bytes in response buffer
 * \param[out] response pointer to binary response buffer
 * \return the status of the operation
 */
uint8_t ParseBusCommands(uint16_t commandLength, uint8_t *command, uint16_t *responseLength, uint8_t *response)
{
	uint8_t status = KIT_STATUS_UNKNOWN_COMMAND;
	char *pToken = strchr((char *) command, ':');
	if (!pToken)
		return status;

	pToken++;

#if defined(SHA204_SWI_BITBANG) | defined(SHA204_SWI_UART)
	if (strstr((const char *) pToken, (const char *) "sw") || pToken[0] == '1')
		return ParseSwiCommands(commandLength, (uint8_t *) pToken, responseLength, response);
	else
#endif

#ifdef I2C
	if (strstr((const char *) pToken, (const char *) "i2") || pToken[0] == '2')
		return ParseTwiCommands(commandLength, (uint8_t *) pToken, responseLength, response);
	else
#endif

#ifdef SPI
	if (strstr((const char *) pToken, (const char *) "sp") || pToken[0] == '3')
		return ParseSpiCommands(commandLength, (uint8_t *) pToken, responseLength, response);
#endif
	;
	
	return status;
}



