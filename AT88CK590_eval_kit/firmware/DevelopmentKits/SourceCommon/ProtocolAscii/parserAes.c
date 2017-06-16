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
 * \brief This file contains functions for parsing AES132 related commands.
 * \author Atmel Crypto Products
 * \date   November 14, 2011
 */

#include <string.h>

#include "utilities.h"
#include "kitStatus.h"
#include "parserAscii.h"

#ifdef AES132_VERSION_2
#   include "aes132.h"
#else
#   include "aes132_comm.h"
#   include "aes132_physical.h"
#   include "aes132_lib_return_codes.h"
#endif

#if defined(SHA204) && defined(AES132)
#   ifndef BOARD
#      include "Combined_Physical.h"
#   endif
#endif

extern uint8_t aes132c_wait_for_device_ready(void);
extern uint8_t aes132c_wait_for_response_ready(void);
extern uint8_t aes132c_reset_io_address(void);

enum aes_layer {
	AES_LAYER_COMM,
	AES_LAYER_PHYS
};


/** \brief This function resets and reads the IO buffer of the device.
 *         It is called when there was an error reading a response but
 *         it is suspected that there are data in the response buffer.
 *         The AES132 library does not read further than the count byte
 *         if the count byte is out of range.
 *  \param[out] rxLength pointer to receive length
 *  \param[out] rxBuffer pointer to receive buffer
 *  \return status of the operation
 */
static uint8_t ReadIoBuffer(uint8_t *rxLength, uint8_t *rxBuffer)
{
	*rxLength = 0;
	uint8_t status = aes132c_reset_io_address();
	if (status != AES132_FUNCTION_RETCODE_SUCCESS)
		return status;

	uint8_t count = 0;
	status = aes132p_read_memory_physical(1, AES132_IO_ADDR, &count);
	if (status != AES132_FUNCTION_RETCODE_SUCCESS)
		return status;

	*rxBuffer++ = count;
	if (count == 0) {
		*rxLength = 1;
		return status;
	}
	if (count > DEVICE_BUFFER_SIZE_MAX_RX)
		count = DEVICE_BUFFER_SIZE_MAX_RX;

	*rxLength = count;

	return aes132p_read_memory_physical(count - 1, AES132_IO_ADDR, rxBuffer);
}


/** \brief This function reads from or writes to the device.
 * \param[in] layer Use communication (retries) or physical layer (no retries.
 * \param[in] token pointer to character that indicates write ('w') or read ('r')
 * \param[out] responseLength pointer to response length
 * \param[out] response pointer to response buffer
 * \return status of the operation
 */
static uint8_t ParseAesReadWriteCommands(uint8_t layer, char *token, uint8_t *responseLength, uint8_t *response)
{
	uint16_t dataLength;
	uint8_t *data[1];
	uint8_t *dataLoad;
	uint16_t word_address;

	uint8_t status = ExtractDataLoad(token + 2, &dataLength, data);
	if (status != KIT_STATUS_SUCCESS) {
		*responseLength = 0;
		return status;
	}
	dataLoad = data[0];

	// write memory
	if (token[0] == 'w') {
		word_address = dataLoad[1] * 256 + dataLoad[2];
		if (layer == AES_LAYER_COMM && word_address < AES132_IO_ADDR) {
			// Use communication layer only when writing to EEPROM.
			status = aes132c_write_memory(dataLoad[0], word_address, dataLoad + 3);
			if (status < AES132_FUNCTION_RETCODE_COMM_FAIL) {
				// Reset and read response buffer.
				status = aes132c_reset_io_address();
				if (status == AES132_FUNCTION_RETCODE_SUCCESS) {
					status = aes132c_read_memory(AES132_RESPONSE_SIZE_MIN, AES132_IO_ADDR, response);
					if (status <= AES132_DEVICE_RETCODE_TEMP_SENSE_ERROR) {
						*responseLength = response[AES132_COMMAND_INDEX_COUNT];
						status = KIT_STATUS_SUCCESS;
					}
					else
						*responseLength = 0;
				}
			}
			else
				*responseLength = 0;
		}
		else {
			// If we use the physical layer, we assume that a host will also retrieve the response
			// by using the physical layer.
			status = aes132p_write_memory_physical(dataLoad[0], word_address, dataLoad + 3);
			*responseLength = 0;
		}
	}

	// read memory
	else {
		word_address = dataLoad[1] * 256 + dataLoad[2];
		status = layer == AES_LAYER_COMM
						? aes132c_read_memory(dataLoad[0], word_address, response)
						: aes132p_read_memory_physical(dataLoad[0], word_address, response);
		if (status == AES132_FUNCTION_RETCODE_SUCCESS)
			*responseLength = dataLoad[0];
	}

	return status;
}


/** \brief This function parses communication commands (ASCII) received from a
 *         PC host and returns a binary response.
 *
 *         protocol syntax:\n\n
 *         functions in aes132_comm.c (Communication layer):\n
 *            s[tatus]                            aes132c_read_device_status_register\n
 *            mw[rite](count, word address, data) aes132c_write_memory\n
 *            mr[ead](count, word address)        aes132c_read_memory\n
 *            c[ommand](command)                  aes132c_send_command\n
 *            cs[leep](mode)                      aes132c_sleep (mode = 0) or aes132c_standby (mode != 0)\n
 *            cr[eset]                            aes132c_send_command(<reset command>\n
 *            r[esponse](buffer size)             aes132c_receive_response\n
 *            wd[evice]                           aes132c_wait_for_device_ready\n
 *            wr[esponse]                         aes132c_wait_for_response_ready\n
 *         functions in aes132_i2c.c / aes132_spi.c (Physical layer, "p[hysical]:"):\n
 *            mw[rite](count, word address, data) aes132p_write_memory_physical\n
 *            mr[ead](count, word address)        aes132p_read_memory_physical\n
 *            s[elect device]:(<address or index) aes132p_select_device\n
 *            i[nterface]:{i[2c] | s[pi]}         aes132p_set_interface\n
 *            e[nable]                            aes132p_enable_interface\n
 *            d[isable]                           aes132p_disable_interface\n
 *            sy[nc]                              aes132p_resync_physical\n
 * \param[in] commandLength number of bytes in command buffer
 * \param[in] command pointer to ASCII command buffer
 * \param[out] responseLength pointer to number of bytes in response buffer
 * \param[out] response pointer to binary response buffer
 * \return the status of the operation
 */
uint8_t ParseAesCommands(uint8_t commandLength, uint8_t *command, uint16_t *responseLength16, uint8_t *response)
{
	uint8_t status = KIT_STATUS_UNKNOWN_COMMAND;
	uint16_t dataLength;
	uint8_t *data[1];
	uint8_t *dataLoad;
	uint8_t responseLength = 0;
//	uint8_t maxDelay = 145; // milliseconds for TempSense command to execute
//	uint8_t maxDelay = 1; // milliseconds for TempSense command to execute

	char *pToken = strchr((char *) command, ':');
	if (!pToken)
		return status;

	// --------- functions in aes132_comm.c --------------------
	// ------------------ "a[es]:" -------------------------------
	if (pToken[1] == 's') {
		// ------------------ "a[es]:s[tatus]" -------------------------------
		// Read device status register.
		uint8_t devStatus;
		status = aes132c_read_device_status_register(&devStatus);
		if (status == KIT_STATUS_SUCCESS) {
			*response = devStatus;
			responseLength = 1;
		}
	}

	else if (pToken[1] == 'm') {
		// ---- "a[es]:m{w[rite] | r[ead]}(<size, 1 byte><address, 2 bytes>[<data>])" ----------
		// Access memory using Communication layer.
		status = ParseAesReadWriteCommands(AES_LAYER_COMM, pToken + 2, &responseLength, response);
	}

	else if (pToken[1] == 't') {
		// ------------------ "a[es]:t[alk](command)" -------------------------------
		// Send command & receive response
		status = ExtractDataLoad(pToken + 2, &dataLength, data);
		if (status != KIT_STATUS_SUCCESS)
			return status;

		dataLoad = data[0];
			
#ifdef AES132_VERSION_2
		status = aes132c_send_and_receive(dataLoad, DEVICE_BUFFER_SIZE_MAX_RX, response, AES132_OPTION_DEFAULT);
#else
		status = aes132c_send_and_receive(dataLoad, 1, DEVICE_BUFFER_SIZE_MAX_RX, response, AES132_OPTION_DEFAULT);
#endif
		if (status == AES132_FUNCTION_RETCODE_SUCCESS) {
			responseLength = response[AES132_RESPONSE_INDEX_COUNT];
			status = KIT_STATUS_SUCCESS;
		}
		else if (status <= AES132_DEVICE_RETCODE_TEMP_SENSE_ERROR) {
			responseLength = response[AES132_RESPONSE_INDEX_COUNT];
			status = KIT_STATUS_SUCCESS;
		}
		else if ((status < AES132_FUNCTION_RETCODE_TIMEOUT && responseLength == 0) || status == AES132_FUNCTION_RETCODE_COMM_FAIL) {
			// Received something but it was garbage. Lets try to read the IO buffer.
			status = ReadIoBuffer(&responseLength, response);
		}
 	}

	else if (pToken[1] == 'c') {
		if (pToken[2] == 'r') {
			// ------------------ "a[es]:cr[eset]" -------------------------------
			// Re-use command buffer.
			memset(command, 0, AES132_COMMAND_SIZE_MIN);
			command[AES132_COMMAND_INDEX_COUNT] = AES132_COMMAND_SIZE_MIN;
			command[AES132_COMMAND_INDEX_OPCODE] = AES132_OPCODE_RESET;
			status = aes132c_send_command(command, AES132_OPTION_NO_STATUS_READ);
		}
		else {
			status = ExtractDataLoad(pToken + 2, &dataLength, data);
			if (status != KIT_STATUS_SUCCESS) {
				return status;
			}
			dataLoad = data[0];

			if (pToken[2] == 's')
				// ------------------ "a[es]:cs[leep](mode)" -------------------------------
				status = dataLoad[0] ? aes132c_standby() : aes132c_sleep();
			else
				// ------------------ "a[es]:c[ommand](command)" -------------------------------
				// Send command.
				status = aes132c_send_command(dataLoad, AES132_OPTION_DEFAULT);
		}			
	}

	else if (pToken[1] == 'r') {
		// ------------------ "a[es]:r[esponse](size)" -------------------------------
		// Receive response.
		status = ExtractDataLoad(pToken + 2, &dataLength, data);
		if (status != KIT_STATUS_SUCCESS)
			return status;

		dataLoad = data[0];
		status = aes132c_receive_response(dataLoad[0], response);
		if (status < AES132_FUNCTION_RETCODE_COMM_FAIL) {
			responseLength = response[AES132_RESPONSE_INDEX_COUNT];
			status = KIT_STATUS_SUCCESS;
		}
	}

	else if (pToken[1] == 'w')
		// ------------------ "a[es]:w{d[evice] | r[esponse]}" -------------------------------
		// Wait for device or response ready.
		status = pToken[2] == 'd'
						? aes132c_wait_for_device_ready()
						: aes132c_wait_for_response_ready();


	// --------- functions in aes132_i2c.c and aes132_spi.c  --------------------
	// --------- that are wrapped in Combined_Physical.c  -----------------------
	else if (pToken[1] == 'p') {
		// ----------------------- "a[es]:p[hysical]:" ---------------------------
		pToken = strchr(&pToken[1], ':');
		if (!pToken)
			return status;

		if (pToken[1] == 'i') {
			// ---------------------- "i[nt]:{i[2c] | s[pi]} ------------------------
			// Set and enable interface (I2C or SPI).
			pToken = strchr(&pToken[1], ':');
			if (!pToken)
				return status;

#if defined(SHA204) && defined(AES132) && !defined(PARSER_ONE_INTERFACE)
			if (pToken[1] == 'i')
				status = aes132p_set_interface(DEVKIT_IF_I2C);
			else if (pToken[1] == 's')
				status = aes132p_set_interface(DEVKIT_IF_SPI);
#endif
		}

		else if (pToken[1] == 'm')
			// ---------------------- "m{w[rite] | r[ead]}(data | size)" ---------------------
			// Access memory using Physical layer.
			status = ParseAesReadWriteCommands(AES_LAYER_PHYS, pToken + 2, &responseLength, response);

		else if (pToken[1] == 's') {
			if (pToken[2] == 'y')
				// ---------------------- "sy[nc]' ---------------------
				// Resynchronizes communication by resetting the device
				// I/O  buffer and, for I2C, sending a Start, 0xFF, and Stop.
				status = aes132p_resync_physical();

			else {
				// ---------------------- "s[elect](device index | I2C address) ---------------------
				// Select device (I2C: address; SPI: index into GPIO array).
				status = ExtractDataLoad(pToken + 2, &dataLength, data);
				if (status != KIT_STATUS_SUCCESS)
					return status;

				dataLoad = data[0];
				status = aes132p_select_device(dataLoad[0]);
			}
		}

		else if (pToken[1] == 'e' || pToken[1] == 'd') {
			// ---------------------- "{e[nable] | d[isable]} ---------------------
			// Enables or disables the current interface.
			// These are only wrapper functions for the hardware dependent
			// enable and disable functions.
			pToken[1] == 'e'
							? aes132p_enable_interface()
							: aes132p_disable_interface();
			status = KIT_STATUS_SUCCESS;
		}
	}

	if (status >= AES132_FUNCTION_RETCODE_COMM_FAIL)
		responseLength = 0;

	*responseLength16 = (uint16_t) responseLength;
	
	return status;
}
