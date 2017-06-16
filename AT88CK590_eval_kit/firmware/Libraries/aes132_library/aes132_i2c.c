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
 *  \brief 	This file contains implementations of I2C functions.
 *  \author Atmel Crypto Products
 *  \date 	June 16, 2011
 */

#include <stdint.h>                    //!< C type definitions
#include <avr\io.h>                    //!< GPIO definitions
#include <util\twi.h>                  //!< I2C AVR definitions
#include "aes132_i2c.h"                //!< I2C library definitions

/** \brief I2C address used at AES132 library startup. */
#define AES132_I2C_DEFAULT_ADDRESS   ((uint8_t) 0xA0)


/** \brief These enumerations are flags for I2C read or write addressing. */
enum aes132_i2c_read_write_flag {
	I2C_WRITE = (uint8_t) 0x00,	//!< write command id
	I2C_READ  = (uint8_t) 0x01   //! read command id
};

//! I2C address currently in use.
static uint8_t i2c_address_current = AES132_I2C_DEFAULT_ADDRESS;


/** \brief This function initializes and enables the I2C peripheral.
 * */
void i2c_enable(void)
{
	PRR0 &= ~_BV(PRTWI);            // Disable power saving.

#ifdef I2C_PULLUP
	DDRD &= ~(_BV(PD0) | _BV(PD1)); // Configure I2C as input to allow setting the pull-up resistors.
	PORTD |= (_BV(PD0) | _BV(PD1)); // Connect pull-up resistors on I2C clock and data pins.
#endif

	TWBR = ((uint8_t) (((double) F_CPU / I2C_CLOCK - 16.0) / 2.0 + 0.5)); // Set the baud rate
}


/** \brief This function disables the I2C peripheral. */
void i2c_disable(void)
{
	TWCR = 0;                       // Disable TWI.
	PRR0 |= _BV(PRTWI);             // Enable power saving.
}


/** \brief This function creates a Start condition (SDA low, then SCL low).
 * \return status of the operation
 * */
uint8_t i2c_send_start(void)
{
	uint8_t timeout_counter = I2C_START_TIMEOUT;
	uint8_t i2c_status;

	TWCR = (_BV(TWEN) | _BV(TWSTA) | _BV(TWINT));
	do {
		if (timeout_counter-- == 0)
			return I2C_FUNCTION_RETCODE_TIMEOUT;
	} while ((TWCR & (_BV(TWINT))) == 0);

	i2c_status = TW_STATUS;
	if ((i2c_status != TW_START) && (i2c_status != TW_REP_START))
		return I2C_FUNCTION_RETCODE_COMM_FAIL;

	return I2C_FUNCTION_RETCODE_SUCCESS;
}


/** \brief This function creates a Stop condition (SCL high, then SDA high).
 * \return status of the operation
 * */
uint8_t i2c_send_stop(void)
{
	uint8_t timeout_counter = I2C_STOP_TIMEOUT;

	TWCR = (_BV(TWEN) | _BV(TWSTO) | _BV(TWINT));
	do {
		if (timeout_counter-- == 0)
			return I2C_FUNCTION_RETCODE_TIMEOUT;
	} while ((TWCR & _BV(TWSTO)) > 0);

	if (TW_STATUS == TW_BUS_ERROR)
		return I2C_FUNCTION_RETCODE_COMM_FAIL;

	return I2C_FUNCTION_RETCODE_SUCCESS;
}


/** \brief This function sends bytes to an I2C device.
 * \param[in] count number of bytes to send
 * \param[in] data pointer to tx buffer
 * \return status of the operation
 */
uint8_t i2c_send_bytes(uint8_t count, uint8_t *data)
{
	uint8_t timeout_counter;
	uint8_t twi_status;
	uint8_t i;

	for (i = 0; i < count; i++) {
		TWDR = *data++;
		TWCR = _BV(TWEN) | _BV(TWINT);

		timeout_counter = I2C_BYTE_TIMEOUT;
		do {
			if (timeout_counter-- == 0)
				return I2C_FUNCTION_RETCODE_TIMEOUT;
		} while ((TWCR & (_BV(TWINT))) == 0);

		twi_status = TW_STATUS;
		if ((twi_status != TW_MT_SLA_ACK)
					&& (twi_status != TW_MT_DATA_ACK)
					&& (twi_status != TW_MR_SLA_ACK))
			// Return error if byte got nacked.
			return I2C_FUNCTION_RETCODE_NACK;
	}

	return I2C_FUNCTION_RETCODE_SUCCESS;
}


/** \brief This function receives one byte from an I2C device.
 *
 * \param[out] data pointer to received byte
 * \return status of the operation
 */
uint8_t i2c_receive_byte(uint8_t *data)
{
	uint8_t timeout_counter = I2C_BYTE_TIMEOUT;

	// Enable acknowledging data.
	TWCR = (_BV(TWEN) | _BV(TWINT) | _BV(TWEA));
	do {
		if (timeout_counter-- == 0)
			return I2C_FUNCTION_RETCODE_TIMEOUT;
	} while ((TWCR & (_BV(TWINT))) == 0);

	if (TW_STATUS != TW_MR_DATA_ACK) {
		// Do not override original error.
		(void) i2c_send_stop();
		return I2C_FUNCTION_RETCODE_COMM_FAIL;
	}
	*data = TWDR;

	return I2C_FUNCTION_RETCODE_SUCCESS;
}


/** \brief This function receives bytes from an I2C device
 *         and sends a Stop.
 *
 * \param[in] count number of bytes to receive
 * \param[out] data pointer to rx buffer
 * \return status of the operation
 */
uint8_t i2c_receive_bytes(uint8_t count, uint8_t *data)
{
	uint8_t i;
	uint8_t timeout_counter;

	// Acknowledge all bytes except the last one.
	for (i = 0; i < count - 1; i++) {
		// Enable acknowledging data.
		TWCR = (_BV(TWEN) | _BV(TWINT) | _BV(TWEA));
		timeout_counter = I2C_BYTE_TIMEOUT;
		do {
			if (timeout_counter-- == 0)
				return I2C_FUNCTION_RETCODE_TIMEOUT;
		} while ((TWCR & (_BV(TWINT))) == 0);

		if (TW_STATUS != TW_MR_DATA_ACK) {
			// Do not override original error.
			(void) i2c_send_stop();
			return I2C_FUNCTION_RETCODE_COMM_FAIL;
		}
		*data++ = TWDR;
	}

	// Disable acknowledging data for the last byte.
	TWCR = (_BV(TWEN) | _BV(TWINT));
	timeout_counter = I2C_BYTE_TIMEOUT;
	do {
		if (timeout_counter-- == 0)
			return I2C_FUNCTION_RETCODE_TIMEOUT;
	} while ((TWCR & (_BV(TWINT))) == 0);

	if (TW_STATUS != TW_MR_DATA_NACK) {
		// Do not override original error.
		(void) i2c_send_stop();
		return I2C_FUNCTION_RETCODE_COMM_FAIL;
	}
	*data = TWDR;

	return i2c_send_stop();
}


/** \brief This function creates a Start condition and sends the I2C address.
 * \param[in] read I2C_READ for reading, I2C_WRITE for writing
 * \return status of the operation
 */
uint8_t aes132p_send_slave_address(uint8_t read)
{
	uint8_t sla = i2c_address_current | read;
	uint8_t aes132_lib_return = i2c_send_start();
	if (aes132_lib_return != AES132_FUNCTION_RETCODE_SUCCESS)
		return aes132_lib_return;

	aes132_lib_return = i2c_send_bytes(1, &sla);
	if (aes132_lib_return != AES132_FUNCTION_RETCODE_SUCCESS) {
		i2c_send_stop();

		// Translate generic nack error to specific one.
		if (aes132_lib_return == I2C_FUNCTION_RETCODE_NACK)
			return (read == I2C_READ
						? AES132_FUNCTION_RETCODE_ADDRESS_READ_NACK
						: AES132_FUNCTION_RETCODE_ADDRESS_WRITE_NACK);
	}

	return aes132_lib_return;
}


/** \brief This function initializes and enables the I2C hardware peripheral. */
void aes132p_enable_interface(void)
{
	i2c_enable();
}


/** \brief This function disables the I2C hardware peripheral. */
void aes132p_disable_interface(void)
{
	i2c_disable();
}


/** \brief This function selects a I2C AES132 device.
 *
 * @param[in] device_id I2C address
 * @return always success
 */
uint8_t aes132p_select_device(uint8_t device_id)
{
	i2c_address_current = device_id & ~1;
	return AES132_FUNCTION_RETCODE_SUCCESS;
}


/** \brief This function writes bytes to the device.
 * \param[in] count number of bytes to write
 * \param[in] word_address word address to write to
 * \param[in] data pointer to tx buffer
 * \return status of the operation
 */
uint8_t aes132p_write_memory_physical(uint8_t count, uint16_t word_address, uint8_t *data)
{
	// In both, big-endian and little-endian systems, we send MSB first.
	const uint8_t word_address_buffer[] = {(uint8_t) (word_address >> 8), (uint8_t) (word_address & 0xFF)};
	uint8_t aes132_lib_return = aes132p_send_slave_address(I2C_WRITE);
	if (aes132_lib_return != AES132_FUNCTION_RETCODE_SUCCESS)
		// There is no need to create a Stop condition, since function
		// aes132p_send_slave_address does that already in case of error.
		return aes132_lib_return;

	aes132_lib_return = i2c_send_bytes(sizeof(word_address), (uint8_t *) word_address_buffer);
	if (aes132_lib_return != AES132_FUNCTION_RETCODE_SUCCESS) {
		// Don't override the return code from i2c_send_bytes in case of error.
		(void) i2c_send_stop();
		return aes132_lib_return;
	}

	if (count > 0)
		// A count of zero covers the case when resetting the I/O buffer address.
		// This case does only require a write access to the device,
		// but data don't have to be actually written.
		aes132_lib_return = i2c_send_bytes(count, data);

	if (aes132_lib_return != AES132_FUNCTION_RETCODE_SUCCESS) {
		// Don't override the return code from i2c_send_bytes in case of error.
		(void) i2c_send_stop();
		return aes132_lib_return;
	}

	// success
	return i2c_send_stop();
}


/** \brief This function reads bytes from the device.
 * \param[in] size number of bytes to write
 * \param[in] word_address word address to read from
 * \param[out] data pointer to rx buffer
 * \return status of the operation
 */
uint8_t aes132p_read_memory_physical(uint8_t size, uint16_t word_address, uint8_t *data)
{
	// Random read:
	// Start, I2C address with write bit, word address,
	// Start, I2C address with read bit

	// In both, big-endian and little-endian systems, we send MSB first.
	const uint8_t word_address_buffer[] = {(uint8_t) (word_address >> 8), (uint8_t) (word_address & 0xFF)};

	uint8_t aes132_lib_return = aes132p_send_slave_address(I2C_WRITE);
	if (aes132_lib_return != AES132_FUNCTION_RETCODE_SUCCESS)
		// There is no need to create a Stop condition, since function
		// aes132p_send_slave_address does that already in case of error.
		return aes132_lib_return;

	aes132_lib_return = i2c_send_bytes(sizeof(word_address), (uint8_t *) word_address_buffer);
	if (aes132_lib_return != AES132_FUNCTION_RETCODE_SUCCESS) {
		// Don't override the return code from i2c_send_bytes in case of error.
		(void) i2c_send_stop();
		return aes132_lib_return;
	}

	aes132_lib_return = aes132p_send_slave_address(I2C_READ);
	if (aes132_lib_return != AES132_FUNCTION_RETCODE_SUCCESS)
		return aes132_lib_return;

	return i2c_receive_bytes(size, data);
}


/** \brief This function resynchronizes communication.
 * \return status of the operation
 */
uint8_t aes132p_resync_physical(void)
{
	uint8_t nine_clocks = 0xFF;
	uint8_t n_retries = 2;
	uint8_t aes132_lib_return;

	do {
		aes132_lib_return = i2c_send_start();
		if (aes132_lib_return != AES132_FUNCTION_RETCODE_SUCCESS) {
			// If a device is holding SDA or SCL, disabling and
			// re-enabling the I2C peripheral might help.
			i2c_disable();
			i2c_enable();
		}
		if (--n_retries == 0)
			return aes132_lib_return;

		// Retry creating a Start condition if it failed.
	} while(aes132_lib_return != AES132_FUNCTION_RETCODE_SUCCESS);

	// Do not evaluate the return code which most likely indicates error,
	// since nine_clocks is unlikely to be acknowledged.
	(void) i2c_send_bytes(1, &nine_clocks);

	return i2c_send_stop();
}
