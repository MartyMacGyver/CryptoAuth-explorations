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
 *  \brief Header file for aes132_comm.c.
 *  \author Atmel Crypto Products
 *  \date  June 3, 2011
 */

#ifndef AES132_COMM_H_
#   define AES132_COMM_H_

#include <stdint.h>


// ----------------------------------------------------------------------------------
// ------------------------ definitions for timeout counters ------------------------
// ----------- Adjust the values for the timeout counters to your hardware. ---------
// ----------------------------------------------------------------------------------

/** \brief time for polling a bit in the device status register in us (measured)
 *
 * With an oscilloscope or logic analyzer, measure the time it takes for one
 * loop iteration in aes132c_wait_for_status_bit(), and enter it here.
 */
//#define AES132_STATUS_REG_POLL_TIME   (10)
#define AES132_STATUS_REG_POLL_TIME     (41)

/** \brief time in us per byte (number of clocks per byte * number of bytes / clock frequency)
 *
 * If you cannot measure the polling time for a status bit in the device status register,
 * you can derive it by establishing three separate values,
 * #AES132_STATUS_REG_POLL_TIME_COMM, #AES132_STATUS_REG_POLL_TIME_OVERHEAD,
 * and #AES132_STATUS_REG_POLL_TIME_LOOP.
 *
 * Time to poll the device status register:\n
 *    t_poll = t_comm + t_comm_overhead + t_loop,\n
 * where\n
 *    t_comm(spi) = (4 bytes * 8 clocks / spi_clock)\n
 *    t_comm_overhead(spi) = t_data_register_write * 3 + t_data_register_read,\n
 *    t_comm(i2c, when I2C address gets nacked -> Device is busy.)
 *       = (1 byte * 9 clocks / i2c_clock + t_start + t_stop)\n
 *    t_comm_overhead(i2c) = t_execute_start_function + t_data_register_write + t_execute_stop_function\n
 *    t_loop = t_loop(aes132c_read_device_status_register())\n
 *
 * Example for AVR eight-bit CPU running at 8 MHz:\n
 *    SPI (clocked at 4 MHz):\n
 *    \todo Review SPI numbers. Compare ATmega2560 and AT90USB1287.
 *       t_comm(spi) = 32 bits / 4000000 Hz + 12.0 us =  20.0 us\n
 *       t_comm_overhead(spi) = 12.0 us\n
 *       t_loop(spi) = 28.0 us\n
 *    I2C (clocked at 200 kHz):\n
 *       t_comm(i2c, when nacked) = (1 byte * 9 clocks /  200000 Hz + 2.6 us + 3.6 us) =  51.2 us\n
 *       t_comm_overhead(i2c) = 18.6 us\n
 *    	t_loop(i2c) = 13.0 us\n
 *       t_poll(i2c) = 51.2 us + 18.6 us + 13 us = 83 us
 *
 * The number is for I2C clocking at 200 kHz.
 * Since the sum of this number and #AES132_STATUS_REG_POLL_TIME_OVERHEAD is greater than the sum for SPI,
 * the values for I2C can be used for SPI as well.\n
 */
#define AES132_STATUS_REG_POLL_TIME_COMM         ( 51)

/** \brief time in us it takes to access the interface peripheral
 *
 * For I2C, add times for calling twi_send_start and twi_send_stop.
 */
#define AES132_STATUS_REG_POLL_TIME_OVERHEAD     ( 19)

//! time in us the polling loop takes to decrement the counter and branch
#define AES132_STATUS_REG_POLL_TIME_LOOP         ( 13)

#ifndef AES132_STATUS_REG_POLL_TIME
/** \brief time for polling a bit in the device status register in us (calculated)

 * Use either the definition where the value was measured
 * or this one where individual values are calculated and / or estimated.
 */
#   define   AES132_STATUS_REG_POLL_TIME (AES132_STATUS_REG_POLL_TIME_COMM + AES132_STATUS_REG_POLL_TIME_OVERHEAD + AES132_STATUS_REG_POLL_TIME_LOOP))
#endif

// ----------------------------------------------------------------------------------
// --------------- end of definitions for timeout counter values --------------------
// ----------------------------------------------------------------------------------


// ------------------------ timing definitions -----------------------------------

//! Poll this many ms for the device being ready for access.
#define AES132_DEVICE_READY_TIMEOUT      (100)

/** \brief Poll this many ms for the response buffer being ready for reading.
 *
 * When adjusting this number, consider the command execution delays used.
 * As these delays lie closer to or farther from the minimum command execution
 * delays, this number has to be made bigger or smaller accordingly. With other
 * words: The earlier we start polling after sending a command,
 * the longer we have to make the wait-for-response-ready time-out.
 */
#define AES132_RESPONSE_READY_TIMEOUT     (145) // Biggest response timeout is the one for the TempSense command (in ms).

//! timeout loop iterations per ms derived from the times it takes to communicate and loop
#define AES132_ITERATIONS_PER_MS          (1000 / AES132_STATUS_REG_POLL_TIME)

//! Poll this many times for the device being ready for access.
#define AES132_RETRY_COUNT_DEVICE_READY   ((uint16_t) (AES132_DEVICE_READY_TIMEOUT * AES132_ITERATIONS_PER_MS))

//! Poll this many times for the response buffer being ready for reading.
#define AES132_RETRY_COUNT_RESPONSE_READY ((uint16_t) (AES132_RESPONSE_READY_TIMEOUT * AES132_ITERATIONS_PER_MS))

//! It takes this many milliseconds to write a page of 32 bytes to memory.
#define AES132_MS_PER_PAGE_WRITE          ((uint8_t) 3)


// ----------------------- definitions for retry counts -----------------------------

//! number of retries for sending a command, receiving a response, and accessing memory
#define AES132_RETRY_COUNT_ERROR           ((uint8_t) 2)

//! number of re-synchronization retries
#define AES132_RETRY_COUNT_RESYNC          ((uint8_t) 2)


// ------------- definitions for packet sizes --------------------

//! size of CRC
#define AES132_CRC_SIZE                    ((uint8_t)  2)

//! minimum command size
#define AES132_COMMAND_SIZE_MIN            ((uint8_t)  9)

//! maximum command size (DecRead, KeyImport, and WriteCompute command)
#define AES132_COMMAND_SIZE_MAX            ((uint8_t) 63)

//! minimum response size (EncRead, Encrypt, and WriteCompute command)
#define AES132_RESPONSE_SIZE_MIN           ((uint8_t)  4)

//! maximum response size
#define AES132_RESPONSE_SIZE_MAX           ((uint8_t) 52)

//! maximum number of bytes to write to or read from memory
#define AES132_MEM_ACCESS_MAX              ((uint8_t) 32)


// ------------- definitions for device word addresses --------------------

//! word address of command / response buffer
#define AES132_IO_ADDR                     ((uint16_t) 0xFE00)

//! Write to this word address to reset the index of the command / response buffer.
#define AES132_RESET_ADDR                  ((uint16_t) 0xFFE0)

//! word address of device status register
#define AES132_STATUS_ADDR                 ((uint16_t) 0xFFF0)


// ------------- definitions for device status register bits --------------------

//! bit position of the Write-In-Progress bit (WIP) in the device status register
#define AES132_WIP_BIT                          ((uint8_t) 0x01)

//! bit position of the Write-Enabled bit in the device status register (SPI only)
#define AES132_WEN_BIT                          ((uint8_t) 0x02)

//! bit position of the power state bit in the device status register
#define AES132_WAKE_BIT                         ((uint8_t) 0x04)

//! bit position of reserved bit 3 in the device status register
#define AES132_RESERVED3_BIT                    ((uint8_t) 0x08)

//! bit position of the CRC error bit in the device status register
#define AES132_CRC_ERROR_BIT                    ((uint8_t) 0x10)

//! bit position of reserved bit 5 in the device status register
#define AES132_RESERVED5_BIT                    ((uint8_t) 0x20)

//! bit position of the CRC error bit in the device status register
#define AES132_RESPONSE_READY_BIT               ((uint8_t) 0x40)

//! bit position of bit in the device status register that indicates error
#define AES132_DEVICE_ERROR_BIT                 ((uint8_t) 0x80)


// --- definitions for device return codes (byte at index 1 of device response buffer ---

//! no error in executing a command and receiving a response, or writing data to memory
#define AES132_DEVICE_RETCODE_SUCCESS           ((uint8_t) 0x00)

//! error when crossing a page or key boundary for a Write, BlockRead or EncRead
#define AES132_DEVICE_RETCODE_BOUNDARY_ERROR    ((uint8_t) 0x02)

//! Access to the specified User Zone is not permitted due to the current configuration or internal state.
#define AES132_DEVICE_RETCODE_RW_CONFIG         ((uint8_t) 0x04)

//! Address is not implemented, or address is illegal for this command, or attempted to write locked memory.
#define AES132_DEVICE_RETCODE_BAD_ADDR          ((uint8_t) 0x08)

//! Counter limit reached, or count usage error, or restricted key error.
#define AES132_DEVICE_RETCODE_COUNT_ERROR       ((uint8_t) 0x10)

//! no nonce available, or nonce invalid, or nonce does not include a random source, or MacCount limit has been reached
#define AES132_DEVICE_RETCODE_NONCE_ERROR       ((uint8_t) 0x20)

//! Authorization MAC input is missing, or MAC compare failed.
#define AES132_DEVICE_RETCODE_MAC_ERROR         ((uint8_t) 0x40)

//! bad opcode, bad mode, bad parameter, invalid length, or other encoding failure
#define AES132_DEVICE_RETCODE_PARSE_ERROR       ((uint8_t) 0x50)

//! EEPROM post-write automatic data verification failed due to data mismatch.
#define AES132_DEVICE_RETCODE_DATA_MISMATCH     ((uint8_t) 0x60)

//! Lock command contained bad checksum or bad MAC.
#define AES132_DEVICE_RETCODE_LOCK_ERROR        ((uint8_t) 0x70)

/** \brief Key is not permitted to be used for this operation,
 *         or wrong key was used for operation,
 *         or prior authentication has not been performed,
 *         or other authentication error,
 *         or other key error has occurred.
 */
#define AES132_DEVICE_RETCODE_KEY_ERROR         ((uint8_t) 0x80)

//! temperature sensor timeout error
#define AES132_DEVICE_RETCODE_TEMP_SENSE_ERROR  ((uint8_t) 0x90)


// ------------- definitions for option flags used when sending a command --------

//! default flags for option parameter
#define AES132_OPTION_DEFAULT                   ((uint8_t) 0x00)

/** \brief flag for option parameter that indicates whether or not to
 *         calculate and append a CRC.
 */
#define AES132_OPTION_NO_APPEND_CRC             ((uint8_t) 0x01)

/** \brief flag for option parameter that indicates whether or not to
 *         read the device status register after sending a command.
 */
#define AES132_OPTION_NO_STATUS_READ            ((uint8_t) 0x02)


// ----- definitions for byte indexes of command buffer --------

//! count at index 0 (1 byte)
#define AES132_COMMAND_INDEX_COUNT              (0)

//! op-code at index 1 (1 byte)
#define AES132_COMMAND_INDEX_OPCODE             (1)

//! mode at index 2 (1 byte)
#define AES132_COMMAND_INDEX_MODE               (2)

//! msb of param1 (2 bytes) at index 3
#define AES132_COMMAND_INDEX_PARAM1_MSB         (3)

//! lsb of param1 (2 bytes) at index 4
#define AES132_COMMAND_INDEX_PARAM1_LSB         (4)

//! msb of param2 (2 bytes) at index 5
#define AES132_COMMAND_INDEX_PARAM2_MSB         (5)

//! msb of param2 (2 bytes) at index 5
#define AES132_COMMAND_INDEX_PARAM2_LSB         (6)


// ----- definitions for Standby and Sleep modes --------

//! value of mode byte for the Sleep command to put device into Sleep mode
#define AES132_COMMAND_MODE_SLEEP               ((uint8_t) 0x00)

//! value of mode byte for the Sleep command to put device into Standby mode
#define AES132_COMMAND_MODE_STANDBY             ((uint8_t) 0x40)


// ----- definitions for byte indexes of response buffer --------

//! count at index 0 (1 byte)
#define AES132_RESPONSE_INDEX_COUNT             ((uint8_t)    0)

//! response return code at index 1 (1 byte)
#define AES132_RESPONSE_INDEX_RETURN_CODE       ((uint8_t)    1)

//! Response data start at index 2 (1 or more bytes).
#define AES132_RESPONSE_INDEX_DATA              ((uint8_t)    2)


// ----- definitions for command codes --------

//! op-code for the Auth command
#define AES132_OPCODE_AUTH                      ((uint8_t) 0x03)

//! op-code for the AuthCheck command
#define AES132_OPCODE_AUTH_CHECK                ((uint8_t) 0x15)

//! op-code for the AuthCompute command
#define AES132_OPCODE_AUTH_COMPUTE              ((uint8_t) 0x14)

//! op-code for the BlockRead command
#define AES132_OPCODE_BLOCK_READ                ((uint8_t) 0x10)

//! op-code for the Counter command
#define AES132_OPCODE_COUNTER                   ((uint8_t) 0x0A)

//! op-code for the Crunch command
#define AES132_OPCODE_CRUNCH                    ((uint8_t) 0x0B)

//! op-code for the DecRead command
#define AES132_OPCODE_DEC_READ                  ((uint8_t) 0x17)

//! op-code for the Decrypt command
#define AES132_OPCODE_DECRYPT                   ((uint8_t) 0x07)

//! op-code for the EncRead command
#define AES132_OPCODE_ENC_READ                  ((uint8_t) 0x04)

//! op-code for the Encrypt command
#define AES132_OPCODE_ENCRYPT                   ((uint8_t) 0x06)

//! op-code for the EncWrite command
#define AES132_OPCODE_ENC_WRITE                 ((uint8_t) 0x05)

//! op-code for the Info command
#define AES132_OPCODE_INFO                      ((uint8_t) 0x0C)

//! op-code for the KeyCreate command
#define AES132_OPCODE_KEY_CREATE                ((uint8_t) 0x08)

//! op-code for the KeyExport command
#define AES132_OPCODE_KEY_EXPORT                ((uint8_t) 0x18)

//! op-code for the KeyImport command
#define AES132_OPCODE_KEY_IMPORT                ((uint8_t) 0x19)

//! op-code for the KeyLoad command
#define AES132_OPCODE_KEY_LOAD                  ((uint8_t) 0x09)

//! op-code for the KeyTransfer command
#define AES132_OPCODE_KEY_TRANSFER              ((uint8_t) 0x1A)

//! op-code for the Legacy command
#define AES132_OPCODE_LEGACY                    ((uint8_t) 0x0F)

//! op-code for the Lock command
#define AES132_OPCODE_LOCK                      ((uint8_t) 0x0D)

//! op-code for the Nonce command
#define AES132_OPCODE_NONCE                     ((uint8_t) 0x01)

//! op-code for the NonceCompute command
#define AES132_OPCODE_NONCE_COMPUTE             ((uint8_t) 0x13)

//! op-code for the Random command
#define AES132_OPCODE_RANDOM                    ((uint8_t) 0x02)

//! op-code for the Reset command
#define AES132_OPCODE_RESET                     ((uint8_t) 0x00)

//! op-code for the Sleep command
#define AES132_OPCODE_SLEEP                     ((uint8_t) 0x11)

//! op-code for the TempSense command
#define AES132_OPCODE_TEMP_SENSE                ((uint8_t) 0x0E)

//! op-code for the WriteCompute command
#define AES132_OPCODE_WRITE_COMPUTE             ((uint8_t) 0x16)


// ------------- declarations for functions in Command layer --------

uint8_t aes132m_execute(uint8_t op_code, uint8_t mode, uint16_t param1, uint16_t param2,
			uint8_t datalen1, uint8_t *data1, uint8_t datalen2, uint8_t *data2,
			uint8_t datalen3, uint8_t *data3, uint8_t datalen4, uint8_t *data4,
			uint8_t *tx_buffer, uint8_t *rx_buffer);


// ------------- declarations for functions in Communication layer --------

uint8_t aes132c_read_memory(uint8_t count, uint16_t word_address, uint8_t *data);
uint8_t aes132c_write_memory(uint8_t count, uint16_t word_address, uint8_t *data);
uint8_t aes132c_read_device_status_register(uint8_t *deviceStatus);
uint8_t aes132c_send_command(uint8_t *command, uint8_t options);
uint8_t aes132c_receive_response(uint8_t count, uint8_t *response);
uint8_t aes132c_send_and_receive(uint8_t *command, uint8_t delay, uint8_t size, uint8_t *response, uint8_t options);
uint8_t aes132c_wakeup(void);
uint8_t aes132c_sleep(void);
uint8_t aes132c_standby(void);
uint8_t aes132c_resync(void);


// helper functions

void    aes132c_calculate_crc(uint8_t count, uint8_t *data, uint8_t *crc);
uint8_t aes132c_wait_for_status_register_bit(uint8_t mask, uint8_t is_set, uint16_t n_retries);
uint8_t aes132c_send_sleep_command(uint8_t standby);


#endif
