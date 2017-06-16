// ----------------------------------------------------------------------------
//         ATMEL Crypto_Devices Software Support  -  Colorado Springs, CO -
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
/** \file module for AES132 library test vectors
 *  \date February 2, 2011
 */

#include <stdint.h>
#include <string.h>

#include "aes132_physical.h"
#include "aes132_comm.h"
#include "aes132_commands.h"
#include "aes132_lib_return_codes.h"
#include "timer_utilities.h"


/** \brief This function tests the TempSense command.
 *  \return test result
 */
uint8_t aes132t_command_temp_sense(void)
{
	uint8_t aes132_lib_return;
	uint16_t temp_diff = 0;

	aes132_lib_return = aes132m_temp_sense(&temp_diff);
	if (aes132_lib_return != AES132_FUNCTION_RETCODE_SUCCESS)
		return aes132_lib_return;

	// Calculate actual temperature in Centigrade.

	// Read temperature offset register from configuration map.
	// Reads 0xffff from the offset register of the device under test.
//	aes132_lib_return = aes132c_read_memory(AES132_TEMP_OFFSET_REGISTER_SIZE, AES132_TEMP_OFFSET_REGISTER_ADDRESS, rx_buffer);
//	if (aes132_lib_return != AES132_FUNCTION_RETCODE_SUCCESS) {
//		_delay_ms(6);
//		continue;
//	}
//	uint16_t temp_offset = 256 * rx_buffer[0] + rx_buffer[1];
//	volatile float temp_die = 1.65 * (float) temp_diff + 67.2 - (float) temp_offset;

	return AES132_FUNCTION_RETCODE_SUCCESS;
}


/** \brief This function tests the Info command.
 *
 *  \return test result
 */
uint8_t aes132t_command_info(void)
{
	uint8_t rx_buffer[AES132_RESPONSE_SIZE_INFO];
	memset(rx_buffer, 0, sizeof(rx_buffer));
	return aes132m_info(AES132_INFO_DEV_NUM, rx_buffer);
}


/** \brief This function tests the Sleep command.
 *
 * @return test result
 */
uint8_t aes132t_command_sleep(void)
{
	aes132c_wakeup();
	aes132c_sleep();
	delay_ms(1);

	aes132c_wakeup();
	/** \todo Add reading the ChipState (Info command). */
	aes132c_standby();
	delay_ms(1);
	/** \todo Add reading the ChipState (Info command). */
	return AES132_FUNCTION_RETCODE_SUCCESS;
}


/** \brief This function writes data to memory and reads them back.
 *
 *         Avoid to run this test often, e.g. in a loop, to prevent exhausting the EEPROM.
 *  \return test result
 */
uint8_t aes132t_memory(void)
{
	uint16_t word_address = 0x0000;
	uint8_t tx_buffer[] = {0x55, 0xAA, 0xBC, 0xDE};
	uint8_t rx_buffer[4];

// -------------------- Write memory. -----------------------------------
	uint8_t aes132_lib_return = aes132c_write_memory(sizeof(tx_buffer), word_address, tx_buffer);
	if (aes132_lib_return != AES132_FUNCTION_RETCODE_SUCCESS)
		return aes132_lib_return;

// -------------------- Read memory. -----------------------------------
	aes132_lib_return = aes132c_read_memory(sizeof(rx_buffer), word_address, rx_buffer);
	if (aes132_lib_return != AES132_FUNCTION_RETCODE_SUCCESS)
		return aes132_lib_return;

	return (memcmp(tx_buffer, rx_buffer, sizeof(tx_buffer)));
}


/** \brief This function reads several configuration registers.
 *
 *  \return test result
 */
uint8_t aes132t_block_read(void)
{
	uint8_t rx_buffer[AES132_RESPONSE_SIZE_BLOCK_READ + 3];
	memset(rx_buffer, 0, sizeof(rx_buffer));

	// Read ManufacturingID and PermConfig register.
	uint8_t aes132_lib_return = aes132m_block_read(0xF012, 3, rx_buffer);
	if (aes132_lib_return != AES132_FUNCTION_RETCODE_SUCCESS)
		return aes132_lib_return;

	// Read TwiAddr and ChipConfig register.
	return aes132m_block_read(0xF040, 2, rx_buffer);
  }


