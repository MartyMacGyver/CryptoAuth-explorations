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
 *  \brief This file contains implementations of utility functions.
 */

#include <stdint.h>
#include <string.h>

#include "kitStatus.h"
#include "utilities.h"


/** \brief This function converts a nibble to Hex-ASCII.
 * \param[in] nibble nibble value to be converted
 * \return ASCII value
**/
uint8_t ConvertNibbleToAscii(uint8_t nibble)
{
    nibble &= 0x0F;
    if (nibble <= 0x09 )
        nibble += '0';
    else
        nibble += ('A' - 10);
    return nibble;
}


/** \brief This function converts an ASCII character to a nibble.
 * \param[in] ascii ASCII value to be converted
 * \return nibble value
**/
uint8_t ConvertAsciiToNibble(uint8_t ascii)
{
    if ((ascii <= '9') && (ascii >= '0'))
        ascii -= '0';
    else if ((ascii <= 'F' ) && (ascii >= 'A'))
        ascii -= ('A' - 10);
    else if ((ascii <= 'f') && (ascii >= 'a'))
        ascii -= ('a' - 10);
    else
        ascii = 0;
    return ascii;
}


/** \brief This function converts ASCII to binary.
 * \param[in] length number of bytes in buffer
 * \param[in, out] buffer pointer to buffer
 * \return number of bytes in buffer
 */
uint16_t ConvertAsciiToBinary(uint16_t length, uint8_t *buffer)
{
	if (length < 2)
		return 0;

	uint16_t i, binIndex;

	for (i = 0, binIndex = 0; i < length; i += 2)
	{
		buffer[binIndex] = ConvertAsciiToNibble(buffer[i]) << 4;
		buffer[binIndex++] |= ConvertAsciiToNibble(buffer[i + 1]);
	}

	return binIndex;
}


/** \brief This function extracts data from a command string and
 * 			converts them to binary.
 *
 * The data load is expected to be in Hex-Ascii and surrounded by parentheses.
 * \param[in] command command string
 * \param[out] dataLength number of bytes extracted
 * \param[out] data pointer to pointer to binary data
 * \return status: invalid parameters or success
 */
uint8_t ExtractDataLoad(char *command, uint16_t *dataLength, uint8_t **data)
{
	uint8_t status = KIT_STATUS_INVALID_PARAMS;
	if (!command || !dataLength || !data)
		return status;

	char *pToken = strchr(command, '(');
	if (!pToken)
		return status;

	char *dataEnd = strchr(pToken, ')');
	if (!dataEnd)
		// Allow a missing closing parenthesis.
		dataEnd = (char *) command + strlen(command);
	else
		dataEnd--;

	uint16_t asciiLength = (uint16_t) (dataEnd - pToken);
	*data = (uint8_t *) pToken + 1;
	*dataLength = ConvertAsciiToBinary(asciiLength, *data);
	return KIT_STATUS_SUCCESS;
}

