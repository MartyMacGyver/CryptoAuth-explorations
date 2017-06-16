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
 *  \brief  This file contains implementations of command functions
 *          for the AES132 device.
 *  \author Atmel Crypto Products
 *  \date   June 13, 2011
 */

#include <stdint.h>
#include "aes132_commands.h"
#ifdef AES132_I2C
#   include "aes132_i2c.h"
#elif AES132_SPI
#   include "aes132_spi.h"
#else
#   error You have to define AES132_I2C or AES132_SPI.
#endif


/** \brief This function sends an Info command and reads its response.
 *
 * @param[in] selector
 *        0x00: Return the current MacCount value. This value may be useful
 *              if the tag and system get out of sync.
 *              0x00 is always returned as the most significant byte of the
 *              Result field.
 *        0x05: Returns the KeyID if a previous Auth command succeeded,
 *              otherwise returns 0xFFFF. The KeyID is reported as 0x00KK,
 *              where KK is the KeyID number.
 *        0x06: The first byte provides the Atmel device code which can be
 *              mapped to an Atmel catalog number. The second byte provides
 *              the device revision number.
 *        0x0C: Returns a code indicating the device state:
                   0x0000 indicates the ChipState = Active.
                   0xFFFF indicates the ChipState =  Power Up.
                   0x5555 indicates the ChipState = "Wakeup from Sleep".
 * @param[out] result pointer to response
 * @return status of the operation
 */
uint8_t aes132m_info(uint8_t selector, uint8_t *result)
{
	uint8_t command[AES132_COMMAND_SIZE_MIN] = {AES132_COMMAND_SIZE_MIN, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	command[AES132_COMMAND_INDEX_PARAM1_LSB] = selector;

	return aes132c_send_and_receive(command, AES132_RESPONSE_SIZE_INFO, result, AES132_OPTION_DEFAULT);
}


/** \brief This function sends a TempSense command, reads the response, and returns the
 *         difference between the high and low temperature values.
 * @param temp_diff difference between the high and low temperature values inside response
 * @return status of the operation
 */
uint8_t aes132m_temp_sense(uint16_t *temp_diff)
{
	const uint8_t command[] = {AES132_COMMAND_SIZE_MIN, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0xD9, 0x9C};
	uint8_t response[AES132_RESPONSE_SIZE_TEMP_SENSE];

	uint8_t aes132_lib_return = aes132c_send_and_receive((uint8_t *) command,
				AES132_RESPONSE_SIZE_TEMP_SENSE, response, AES132_OPTION_NO_APPEND_CRC);
	if (aes132_lib_return != AES132_FUNCTION_RETCODE_SUCCESS)
		return aes132_lib_return;

	// Calculate temperature difference.
	*temp_diff = (uint16_t) ((response[AES132_RESPONSE_INDEX_TEMP_CODE_HIGH_MSB] * 256 + response[AES132_RESPONSE_INDEX_TEMP_CODE_HIGH_LSB])
	                       - (response[AES132_RESPONSE_INDEX_TEMP_CODE_LOW_MSB] * 256 + response[AES132_RESPONSE_INDEX_TEMP_CODE_LOW_LSB]));
	return aes132_lib_return;
}


/** This function sends a BlockRead command and receives the read data.
 *
 * @param[in] word_address start address to read from
 * @param[in] n_bytes number of bytes to read
 * @param[out] result pointer to read buffer
 * @return status of the operation
 */
uint8_t aes132m_block_read(uint16_t word_address, uint8_t n_bytes, uint8_t *result)
{
	uint8_t command[AES132_COMMAND_SIZE_MIN] = {AES132_COMMAND_SIZE_MIN, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	command[AES132_COMMAND_INDEX_PARAM1_MSB] = word_address >> 8;
	command[AES132_COMMAND_INDEX_PARAM1_LSB] = word_address & 0xFF;
	command[AES132_COMMAND_INDEX_PARAM2_LSB] = n_bytes;

	return aes132c_send_and_receive(command, AES132_RESPONSE_SIZE_BLOCK_READ + n_bytes, result, AES132_OPTION_DEFAULT);
}
