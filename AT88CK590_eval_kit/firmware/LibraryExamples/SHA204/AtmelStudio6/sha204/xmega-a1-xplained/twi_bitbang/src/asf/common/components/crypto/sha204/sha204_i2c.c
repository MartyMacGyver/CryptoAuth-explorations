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
 *  \brief  Functions for I2C Physical Hardware Independent Layer of SHA204 Library
 *          Adapted to bit-banged version.
 *  \author Atmel Crypto Products
 *  \date   May 30, 2012
 */

#include "twi_bitbang.h"
#include "conf_atsha204.h"
#include "sha204_physical.h"         // declarations that are common to all interface implementations
#include "sha204_lib_return_codes.h" // declarations of function return codes
#include "sha204_timer.h"            // definitions for delay functions


/** \brief This enumeration lists all packet types sent to a SHA204 device.
 *
 * The following byte stream is sent to a SHA204 I2C device:
 *    {I2C start} {I2C address} {word address} [{data}] {I2C stop}.
 * Data are only sent after a word address of value #SHA204_I2C_PACKET_FUNCTION_NORMAL.
 */
enum i2c_word_address {
	SHA204_I2C_PACKET_FUNCTION_RESET,  //!< Reset device.
	SHA204_I2C_PACKET_FUNCTION_SLEEP,  //!< Put device into Sleep mode.
	SHA204_I2C_PACKET_FUNCTION_IDLE,   //!< Put device into Idle mode.
	SHA204_I2C_PACKET_FUNCTION_NORMAL  //!< Write / evaluate data that follow this word address byte.
};


/** \brief This enumeration lists flags for I2C read or write addressing. */
enum i2c_read_write_flag {
	I2C_WRITE = (uint8_t) 0x00,  //!< write command flag
	I2C_READ  = (uint8_t) 0x01   //!< read command flag
};


//! I2C address is set when calling #sha204p_init or #sha204p_set_device_id.
static uint8_t device_address;


/** \brief This I2C function sets the I2C address.
 *         Communication functions will use this address.
 *
 *  \param[in] id I2C address
 */
void sha204p_set_device_id(uint8_t id)
{
	device_address = id;
}


/** \brief This I2C function initializes the hardware.
 */
void sha204p_init(void)
{
	// Initialize timer.
	sha204h_timer_init();

	// Initialize interrupt vectors.
	irq_initialize_vectors();

	// Enable interrupts.
	cpu_irq_enable();

	// Enable I2C.
	i2c_enable();

	device_address = SHA204_I2C_DEFAULT_ADDRESS;
}


/** \brief This I2C function generates a Wake-up pulse and delays.
 * \return success
 */
uint8_t sha204p_wakeup(void)
{
	i2c_pulse_sda_low(SHA204_WAKEUP_PULSE_WIDTH);
	sha204h_delay_ms(SHA204_WAKEUP_DELAY);
	return SHA204_SUCCESS;
}


/** \brief This function creates a Start condition and sends the I2C address.
 * \param[in] read #I2C_READ for reading, #I2C_WRITE for writing
 * \return status of the I2C operation
 * \todo Investigate why a nack does not result in an error.
 */
static uint8_t sha204p_send_slave_address(uint8_t read)
{
	uint8_t ack_status;
	uint8_t sla = device_address | read;

	i2c_send_start();

	ack_status = i2c_send_bytes(1, &sla);
	if (ack_status != STATUS_OK)
		i2c_send_stop();

	return ack_status;
}


/** \brief This function sends a I2C packet enclosed by a I2C start and stop to a SHA204 device.
 *
 *         This function combines a I2C packet send sequence that is common to all packet types.
 *         Only if word_address is #I2C_PACKET_FUNCTION_NORMAL, count and buffer parameters are
 *         expected to be non-zero.
 * @param[in] word_address packet function code listed in #i2c_word_address
 * @param[in] count number of bytes in data buffer
 * @param[in] buffer pointer to data buffer
 * @return status of the operation
 */
static uint8_t sha204p_i2c_send(uint8_t word_address, uint8_t count, uint8_t *buffer)
{
	uint8_t i2c_status = sha204p_send_slave_address(I2C_WRITE);
	if (i2c_status != STATUS_OK)
		return SHA204_COMM_FAIL;

	i2c_status = i2c_send_bytes(1, &word_address);
	if (i2c_status != STATUS_OK)
		return SHA204_COMM_FAIL;

	if (count == 0) {
		// We are done for packets that are not commands (Sleep, Idle, Reset).
		i2c_send_stop();
		return SHA204_SUCCESS;
	}

	i2c_status = i2c_send_bytes(count, buffer);

	i2c_send_stop();

	return (i2c_status != STATUS_OK ? SHA204_COMM_FAIL : SHA204_SUCCESS);
}


/** \brief This I2C function sends a command to the device.
 * \param[in] count number of bytes to send
 * \param[in] command pointer to command buffer
 * \return status of the operation
 */
uint8_t sha204p_send_command(uint8_t count, uint8_t *command)
{
	return sha204p_i2c_send(SHA204_I2C_PACKET_FUNCTION_NORMAL, count, command);
}


/** \brief This I2C function puts the SHA204 device into idle state.
 * \return status of the operation
 */
uint8_t sha204p_idle(void)
{
	return sha204p_i2c_send(SHA204_I2C_PACKET_FUNCTION_IDLE, 0, NULL);
}


/** \brief This I2C function puts the SHA204 device into low-power state.
 *  \return status of the operation
 */
uint8_t sha204p_sleep(void)
{
	return sha204p_i2c_send(SHA204_I2C_PACKET_FUNCTION_SLEEP, 0, NULL);
}


/** \brief This I2C function resets the I/O buffer of the SHA204 device.
 * \return status of the operation
 */
uint8_t sha204p_reset_io(void)
{
	return sha204p_i2c_send(SHA204_I2C_PACKET_FUNCTION_RESET, 0, NULL);
}


/** \brief This I2C function receives a response from the SHA204 device.
 *
 * @param[in] size size of rx buffer
 * @param[out] response pointer to rx buffer
 * @return status of the operation
 */
uint8_t sha204p_receive_response(uint8_t size, uint8_t *response)
{
	uint8_t count, i2c_status;

	// Address the device and indicate that bytes are to be read.
	i2c_status = sha204p_send_slave_address(I2C_READ);
	if (i2c_status != STATUS_OK)
		// I2C address got nacked. Device might be busy or not awake.
		return SHA204_RX_NO_RESPONSE;

	// Receive count byte.
	count = i2c_receive_byte(true);
	response[SHA204_BUFFER_POS_COUNT] = count;
	if ((count < SHA204_RSP_SIZE_MIN) || (count > size)) {
		i2c_send_stop();
		return SHA204_INVALID_SIZE;
	}		

	i2c_receive_bytes(count - 1, &response[SHA204_BUFFER_POS_DATA]);
	
	i2c_send_stop();

	return SHA204_SUCCESS;
}


/** \brief This I2C function resynchronizes communication.
 *
 * Parameters are not used for I2C.\n
 * Re-synchronizing communication is done in a maximum of three steps
 * listed below. This function implements the first step. Since
 * steps 2 and 3 (sending a Wake-up token and reading the response)
 * are the same for I2C and SWI, they are
 * implemented in the communication layer (#sha204c_resync).
  <ol>
     <li>
       To ensure an IO channel reset, the system should send
       the standard I2C software reset sequence, as follows:
       <ul>
         <li>a Start condition</li>
         <li>nine cycles of SCL, with SDA held high</li>
         <li>another Start condition</li>
         <li>a Stop condition</li>
       </ul>
       It should then be possible to send a read sequence and
       if synchronization has completed properly the ATSHA204 will
       acknowledge the device address. The chip may return data or
       may leave the bus floating (which the system will interpret
       as a data value of 0xFF) during the data periods.\n
       If the chip does acknowledge the device address, the system
       should reset the internal address counter to force the
       ATSHA204 to ignore any partial input command that may have
       been sent. This can be accomplished by sending a write
       sequence to word address 0x00 (Reset), followed by a
       Stop condition.
     </li>
     <li>
       If the chip does NOT respond to the device address with an ACK,
       then it may be asleep. In this case, the system should send a
       complete Wake token and wait t_whi after the rising edge. The
       system may then send another read sequence and if synchronization
       has completed the chip will acknowledge the device address.
     </li>
     <li>
       If the chip still does not respond to the device address with
       an acknowledge, then it may be busy executing a command. The
       system should wait the longest TEXEC and then send the
       read sequence, which will be acknowledged by the chip.
     </li>
  </ol>
 * \param[in] size size of rx buffer
 * \param[out] response pointer to response buffer
 * \return status of the operation
 * \todo Run MAC test in a loop until a communication error occurs and this routine is executed.
 */
uint8_t sha204p_resync(uint8_t size, uint8_t *response)
{
	uint8_t nine_clocks = 0xFF;
	uint8_t ret_code;

	i2c_send_start();

	// Do not evaluate the return code that most likely indicates error,
	// since nine_clocks is unlikely to be acknowledged.
	(void) i2c_send_bytes(1, &nine_clocks);

	// Send another Start. The function sends also one byte,
	// the I2C address of the device, because I2C specification
	// does not allow sending a Stop right after a Start condition.
	ret_code = sha204p_send_slave_address(I2C_READ);

	// Send only a Stop if the above call succeeded.
	// Otherwise the above function has sent it already.
	if (ret_code != STATUS_OK)
		return ret_code;
		
	// Try to send a Reset IO command if re-sync succeeded.
	i2c_send_stop();
	return sha204p_reset_io();
}
