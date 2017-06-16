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
/** \file
 *  \brief This file contains the main function for AES132 library example.
 *  \author Atmel Crypto Products
 *  \date April 29, 2011
 */

#include <stdint.h>
#include <string.h>

#include "aes132_lib_return_codes.h"
#include "aes132_comm.h"
#include "aes132_physical.h"


/** \brief This function is the entry function for an example application that
           uses the AES132 library.
 * \return result (0: success, otherwise failure)
 */
int main(void)
{
	uint16_t word_address = 0x0000;
	uint8_t rx_byte_count = 4;
	uint8_t tx_buffer_write[] = {0x55, 0xAA, 0xBC, 0xDE};
	uint8_t tx_buffer_command[AES132_COMMAND_SIZE_MIN];
	uint8_t rx_buffer[AES132_RESPONSE_SIZE_MIN + 4]; // 4: number of bytes to read using the BlockRead command
	uint8_t aes132_lib_return;

	aes132p_enable_interface();

	// -------------------- Write memory. -----------------------------------
   // Don't put this in an infinite loop. Otherwise the non-volatile memory will wear out.
	aes132_lib_return = aes132c_write_memory(sizeof(tx_buffer_write), word_address, tx_buffer_write);
	if (aes132_lib_return != AES132_FUNCTION_RETCODE_SUCCESS) {
	   aes132p_disable_interface();
		return aes132_lib_return;
	}

	// -------------------- Read memory. -----------------------------------
	aes132_lib_return = aes132c_read_memory(rx_byte_count, word_address, rx_buffer);
	if (aes132_lib_return != AES132_FUNCTION_RETCODE_SUCCESS) {
	   aes132p_disable_interface();
		return aes132_lib_return;
	}

	// -------------------- Compare written with read data. -----------------------------------
	aes132_lib_return =  memcmp(tx_buffer_write, rx_buffer, sizeof(tx_buffer_write));
	if (aes132_lib_return != AES132_FUNCTION_RETCODE_SUCCESS) {
	   aes132p_disable_interface();
		return aes132_lib_return;
	}

	// ------- Send a BlockRead command and receive its response. -----------------------------
	aes132_lib_return = aes132m_execute(AES132_OPCODE_BLOCK_READ, 0, word_address, rx_byte_count,
	                                    0, NULL, 0, NULL, 0, NULL, 0, NULL, tx_buffer_command, rx_buffer);
	if (aes132_lib_return != AES132_FUNCTION_RETCODE_SUCCESS) {
	   aes132p_disable_interface();
		return aes132_lib_return;
	}

	// -------------------- Compare written with read data. -----------------------------------
	aes132_lib_return =  memcmp(tx_buffer_write, &rx_buffer[AES132_RESPONSE_INDEX_DATA], sizeof(tx_buffer_write));

	aes132p_disable_interface();

	return aes132_lib_return;
}

