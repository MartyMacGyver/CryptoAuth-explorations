/** \file
 *  \brief Hardware Interface Functions - I2C bit-banged
 *
 *  Use these functions to control the low level hardware of your
 *  microcontroller. The pin and port definitions are in the file
 *  i2c_phys_bitbang.h
 */
#include "i2c_phys_bitbang.h"


/** \brief This function sends an ACK or NACK to the device (after a read).
 *
 *  \param ack - if non-zero, an ACK will be sent, otherwise NACK
 */
void i2c_send_ack(uint8_t ack) {
	if (ack) {
		I2C_SET_OUTPUT_LOW();  //Low data line indicates an ACK.
		while (I2C_DATA_IN() == true);
	}
	else {
		I2C_SET_OUTPUT_HIGH(); // High data line indicates a NACK.
		while (I2C_DATA_IN() == false);
	}

	// Clock out acknowledgment.
	I2C_CLOCK_HIGH();
	I2C_CLOCK_DELAY_SEND_ACK();
	I2C_CLOCK_LOW();
}


/** \brief This function configures the pins for I2C clock and data as output.
 * */
void i2c_enable(void) {
	I2C_ENABLE();
}


/** \brief This function configures the pins for I2C clock and data as input.
 * */
void i2c_disable(void) {
	I2C_DISABLE();
}


/** \brief This function sends a start sequence. */
uint8_t i2c_send_start(void) {
	// Set clock high in case we re-start.
	I2C_CLOCK_HIGH();
	I2C_SET_OUTPUT_HIGH();
	I2C_DATA_LOW();
	I2C_HOLD_DELAY();
	I2C_CLOCK_LOW();
	return I2C_FUNCTION_RETCODE_SUCCESS;
}


/** \brief This function sends a stop sequence. */
uint8_t i2c_send_stop(void) {
	I2C_SET_OUTPUT_LOW();
	I2C_CLOCK_DELAY_WRITE_LOW();
	I2C_CLOCK_HIGH();
	I2C_HOLD_DELAY();
	I2C_DATA_HIGH();
	return I2C_FUNCTION_RETCODE_SUCCESS;
}


/** \brief This function sends one byte.
*
 *  \param data - byte to write
 *  \return whether the byte got acknowledged or not
*/
uint8_t i2c_send_byte(uint8_t i2c_byte) {
	uint8_t i, status = I2C_FUNCTION_RETCODE_NACK;

	DISABLE_INTERRUPT();

	// This avoids spikes but adds an if condition.
	// We could parametrize the call to I2C_SET_OUTPUT
	// and translate the msb to OUTSET or OUTCLR,
	// but then the code would become target specific.
	if (i2c_byte & 0x80) {
		I2C_SET_OUTPUT_HIGH();
	}
	else {
		I2C_SET_OUTPUT_LOW();
	}

	// Send 8 bits of data.
	for (i = 0; i < 8; i++)	{
		I2C_CLOCK_LOW();
		if (i2c_byte & 0x80)
			I2C_DATA_HIGH();
		else
			I2C_DATA_LOW();

		I2C_CLOCK_DELAY_WRITE_LOW();
		// Clock in the data bit.
		I2C_CLOCK_HIGH();

		// Shifting while clock is high compensates for the time it
		// takes to evaluate the bit while clock is low.
		// That way, the low and high time of the clock pin is
		// almost equal.
		i2c_byte <<= 1;
		I2C_CLOCK_DELAY_WRITE_HIGH();
	}
	// Clock in last data bit.	
	I2C_CLOCK_LOW();
	
	// Set data line to be an input.
	I2C_SET_INPUT();

	I2C_CLOCK_DELAY_READ_LOW();
	// Wait for the ack.
	I2C_CLOCK_HIGH();
	for (i = 0; i < I2C_ACK_TIMEOUT; i++) {
		if (!I2C_DATA_IN()) {
			status = I2C_FUNCTION_RETCODE_SUCCESS;
			I2C_CLOCK_DELAY_READ_HIGH();
			break;
		}
	}
	I2C_CLOCK_LOW();

	ENABLE_INTERRUPT();

	return status;
}


/** \brief This function sends a number of bytes.
 *
 * \param[in] buffer - pointer to a buffer containing ucLen bytes to send
 * \param count[in] - number of bytes to send
 *
 * \return ack or nack
 */
uint8_t i2c_send_bytes(uint8_t count, uint8_t *buffer) {
	uint8_t status, i;
	
	for (i = 0; i < count; i++) {
		status = i2c_send_byte(buffer[i]);
		if (status != I2C_FUNCTION_RETCODE_SUCCESS)
			return status;
	}
	return I2C_FUNCTION_RETCODE_SUCCESS;
}


/** \brief This function reads a byte from device, MSB first.
 *  \param[in]  acknowledge or don't
 *  \return byte received
 */
uint8_t i2c_receive_one_byte(uint8_t ack) {
	uint8_t i, i2c_byte;

	DISABLE_INTERRUPT();

	I2C_SET_INPUT();
	for (i = 0x80, i2c_byte = 0; i; i >>= 1) {
		I2C_CLOCK_HIGH();
		I2C_CLOCK_DELAY_READ_HIGH();
		if (I2C_DATA_IN())
			i2c_byte |= i;
		I2C_CLOCK_LOW();
		if (i > 1)
			// We don't need to delay after the last bit because
			// it takes time to switch the pin to output for acknowledging.
			I2C_CLOCK_DELAY_READ_LOW();
	}
    i2c_send_ack(ack);

	ENABLE_INTERRUPT();

	return i2c_byte;
}


/** \brief This function receives one byte and acknowledges it.
    \param[out] data pointer to received byte
	\return success
*/
uint8_t i2c_receive_byte(uint8_t *i2c_byte) {
	*i2c_byte = i2c_receive_one_byte(1);
	return I2C_FUNCTION_RETCODE_SUCCESS;
}


/** \brief This function receives bytes.
 *
 * \param[out] buffer - pointer to receive buffer
 * \param[in] count - number of bytes to receive
 * \return success
 */
uint8_t i2c_receive_bytes(uint8_t count, uint8_t *buffer) {
    while (--count) {
      *buffer++ = i2c_receive_one_byte(1);
    }
    *buffer = i2c_receive_one_byte(0);

	return i2c_send_stop();
}
