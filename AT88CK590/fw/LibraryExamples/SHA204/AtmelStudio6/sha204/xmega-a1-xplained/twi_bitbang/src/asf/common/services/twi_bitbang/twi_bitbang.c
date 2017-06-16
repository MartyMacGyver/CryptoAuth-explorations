// ----------------------------------------------------------------------------
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
 *  \brief  TWI bit-banged driver. This driver is not part of original ASF.
 *  \author Atmel Crypto Products
 *  \date   June 6, 2012
 *  \todo   Investigate why this module does not work when optimization is on
 *          and the ASF delay service is used. When optimization is on and
 *          the TC driver is used it works, although in that case, the polling
 *          does not work because the polling timer uses the same TC peripheral
 *          as the us delay timer.
*/

#include "twi_bitbang.h"
#include "sha204_timer.h"

/** 
 * \brief This function configures two GPIO's (SDA and SCL) for output.
 * 
 */
void i2c_enable()
{
	ioport_configure_group(I2C_PORT, I2C_PIN_GROUP, I2C_PIN_OUTPUT_CONFIG);
}


/** 
 * \brief This function pulses SDA low.
 * \param[in] duration Sets SDA low for this many microseconds.
 */
void i2c_pulse_sda_low(uint16_t duration)
{
	// Make sure SDA is high.
	ioport_set_pin_high(I2C_SDA_GPIO);
	// Keep it high for a little.
	I2C_CLOCK_DELAY;
	ioport_toggle_pin(I2C_SDA_GPIO);
	sha204h_delay_us(duration);
	ioport_toggle_pin(I2C_SDA_GPIO);
}


/** 
 * \brief This function generates an I2C Start condition.
 * 
 */
void i2c_send_start()
{
	ioport_set_group_high(I2C_PORT, I2C_PIN_GROUP);
	I2C_CLOCK_DELAY;
	ioport_set_pin_low(I2C_SDA_GPIO);	
	I2C_CLOCK_DELAY;
	ioport_set_pin_low(I2C_SCL_GPIO);
	I2C_CLOCK_DELAY;
 }
 
 
/** 
 * \brief This function generates an I2C Stop condition.
 * 
 */
void i2c_send_stop()
{
	ioport_set_pin_low(I2C_SDA_GPIO);
	I2C_CLOCK_DELAY;
	ioport_set_pin_high(I2C_SCL_GPIO);
	I2C_CLOCK_DELAY;
	ioport_toggle_pin(I2C_SDA_GPIO);
	I2C_CLOCK_DELAY;
}


/** 
 * \brief This function receives one byte.
 * \param[in] ack whether we should acknowledge or not
 * \return received byte
 */
uint8_t i2c_receive_byte(bool ack)
{
	uint8_t bit_count, data;
	
	// Configure SDA as input.
	ioport_configure_pin(I2C_SDA_GPIO, I2C_PIN_INPUT_CONFIG);
	
	// Make sure SCL starts low.
	ioport_set_pin_low(I2C_SCL_GPIO);
	
	// Receive one byte.
	for (bit_count = 0, data = 0; bit_count < 8; bit_count++) {
		data <<= 1;
		ioport_toggle_pin(I2C_SCL_GPIO);       // high
		I2C_CLOCK_DELAY;
		data |= ioport_get_value(I2C_SDA_GPIO);
		ioport_toggle_pin(I2C_SCL_GPIO);       // low
		I2C_CLOCK_DELAY;
	}

	// Configure SDA as output and set it to "ack".
	ioport_configure_pin(I2C_SDA_GPIO, IOPORT_DIR_OUTPUT | (ack ? IOPORT_INIT_LOW : IOPORT_INIT_HIGH) | IOPORT_WIRED_AND_PULL_UP);
	
	// Clock the ack.
	ioport_toggle_pin(I2C_SCL_GPIO);       // high
	I2C_CLOCK_DELAY;
	ioport_toggle_pin(I2C_SCL_GPIO);       // high

	// There should be no need to delay or clear SDA.
	// We should be able to send a Repeat Start, a Stop, or continue receiving without delay and clearing SDA. 	
	return data;	
}


/** 
 * \brief This function sends one byte.
 *        This function does not work when optimization is on (not -O0) and running
 *        on an Xmega. In particular, the nack variable is not returned correctly.
 * \param[in] data byte to send
 * \return received ack (true) or nack (false)
 */
static bool i2c_send_byte(uint8_t data)
{
	uint8_t bit_mask;
	bool nack;
	
	// Send byte.
	for (bit_mask = 0x80; bit_mask > 0; bit_mask >>= 1) {
		ioport_set_value(I2C_SDA_GPIO, (data & bit_mask) ? true : false);
		ioport_toggle_pin(I2C_SCL_GPIO);       // Sets SCL high because it is low after Start.
		I2C_CLOCK_DELAY;
		ioport_toggle_pin(I2C_SCL_GPIO);       // Sets SCL low.
		I2C_CLOCK_DELAY;
	}
	// Receive ack.
	// Configure SDA as input.
	ioport_configure_pin(I2C_SDA_GPIO, I2C_PIN_INPUT_CONFIG);
	// Read SDA.
	ioport_toggle_pin(I2C_SCL_GPIO);       // Sets SCL high.
	I2C_CLOCK_DELAY;
	nack = ioport_get_value(I2C_SDA_GPIO);
	ioport_toggle_pin(I2C_SCL_GPIO);       // Sets SCL low.
	I2C_CLOCK_DELAY;

	// Configure SDA as output.
	ioport_configure_pin(I2C_SDA_GPIO, I2C_PIN_OUTPUT_CONFIG);
	return !nack;
}


/** 
 * \brief This function sends a number of bytes.
 * \param[in] count number of bytes to send
 * \param[in] data pointer to data buffer
 * \return status (success or nack)
 */
uint8_t i2c_send_bytes(uint8_t count, uint8_t *data)
{
	uint8_t i;
	
	for (i = 0; i < count; i++) {
		if (i2c_send_byte(data[i]) == false)
			return ERR_IO_ERROR;
	}
	return STATUS_OK;
}


/** 
 * \brief This function receives a number of bytes.
 * \param[in] count number of bytes to receive
 * \param[in] data pointer to data buffer
 */
void i2c_receive_bytes(uint8_t count, uint8_t *data)
{
	uint8_t i;

	for (i = 0; i < count - 1; i++)
		data[i] = i2c_receive_byte(true);
	data[i] = i2c_receive_byte(false);
}