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
 *  \brief  Functions for Two-Wire Physical Layer of SHA204 Library
            adapted to the AT91 library
 *  \author Atmel Crypto Products
 *  \date   November 8, 2010
 *  \todo   Remove the need for twi.c or replace at least TWI_ConfigureMaster().
 */

#include <twi/twi.h>                    // AT91 TWI peripheral
#include <pio/pio.h>                    // AT91 PIO peripheral
#include <pmc/pmc.h>                    // AT91 PMC
#include <utility/timer_utilities.h>    // definitions for delay functions

#include "sha204_physical.h"            // SHA204 TWI implementations
#include "sha204_lib_return_codes.h"    // declarations of SHA204 library function return codes

//!brief TWI address used at SHA204 library startup.
#define SHA204_TWI_DEFAULT_ADDRESS      (0xC8)

//! TWI clock frequency is maximum supported on AT91SAM9.
#define TWI_CLOCK                       (400000)

/** \brief This enumeration lists all packet types sent to a SHA204 device.
 *
 * The following byte stream is sent to a SHA204 TWI device:
 *    {TWI start} {TWI address} {word address} [{data}] {TWI stop}.
 * Data are only sent after a word address of value #SHA204_TWI_PACKET_FUNCTION_NORMAL.
 */
enum twi_word_address {
	SHA204_TWI_PACKET_FUNCTION_RESET,   //!< Reset device.
	SHA204_TWI_PACKET_FUNCTION_SLEEP,   //!< Put device into Sleep mode.
	SHA204_TWI_PACKET_FUNCTION_IDLE,    //!< Put device into Idle mode.
	SHA204_TWI_PACKET_FUNCTION_NORMAL   //!< Write / evaluate data that follow this word address byte.
};


/** \brief This enumeration lists flags for TWI read or write addressing. */
enum twi_read_write_flag {
	TWI_WRITE = (uint8_t) 0x00,         //!< write command id
	TWI_READ  = (uint8_t) 0x01          //!< read command id
};

static AT91PS_TWI pTwi;                 //!< pointer to TWI register structure

//! TWI address is set when calling #sha204p_init or #sha204p_set_device_id.
static uint8_t device_address;

/** \brief This TWI function sets the TWI address.
 *         Communication functions will use this address.
 *
 *  \param[in] id TWI address
 */
void sha204p_set_device_id(uint8_t id)
{
	device_address = id >> 1;
}


/** \brief This TWI function initializes the hardware.
 */
void sha204p_init(void)
{
    // Configure the PMC to enable the clock for TWI.
    PMC_EnablePeripheral(AT91C_ID_TWI0);

 	pTwi = AT91C_BASE_TWI0;

	device_address = SHA204_TWI_DEFAULT_ADDRESS >> 1;

	init_timer();
}


/** \brief This TWI function generates a Wake-up pulse and delays.
 * \return status of the operation
 */
uint8_t sha204p_wakeup(void)
{
	Pin pin[1];

	// Reset TWI.
    pTwi->TWI_CR = AT91C_TWI_SWRST;

	// Configure TWI pins as GPIO to be able to generate
	// a Wake-up pulse.
	pin[0].pio = AT91C_BASE_PIOA;
	pin[0].type = PIO_OUTPUT_1;
	pin[0].mask = AT91C_PA20_TWD0;
	pin[0].attribute = PIO_PULLUP;
	PIO_Configure(pin, PIO_LISTSIZE(pin));

	PIO_Clear(&pin[0]);
	delay_10us(SHA204_WAKEUP_PULSE_WIDTH);
	PIO_Set(&pin[0]);

	// Delay before starting any communication.
	delay_ms(SHA204_WAKEUP_DELAY);

	// Reconfigure pins as TWI.
	pin[0].type = PIO_PERIPH_A;
	pin[0].mask = AT91C_PA20_TWD0 | AT91C_PA21_TWCK0;
	PIO_Configure(pin, PIO_LISTSIZE(pin));

	TWI_ConfigureMaster(pTwi, TWI_CLOCK, BOARD_MCK);

	return SHA204_SUCCESS;
}


/** \brief This function sends a TWI packet enclosed by a TWI start and stop to a SHA204 device.
 *
 *         This function combines a TWI packet send sequence that is common to all packet types.
 *         Only if word_address is #TWI_PACKET_FUNCTION_NORMAL, count and buffer parameters are
 *         expected to be non-zero.
 * @param[in] word_address packet function code listed in #twi_word_address
 * @param[in] count number of bytes in data buffer
 * @param[in] buffer pointer to data buffer
 * @return status of the operation
 */
static uint8_t sha204p_twi_send(uint8_t word_address, uint8_t count, uint8_t *buffer)
{
	if (count == 0) {
		// for packets that send only a word address (Sleep, Idle, ResetIo)
		TWI_Stop(pTwi);
		TWI_StartWrite(pTwi, device_address, AT91C_TWI_IADRSZ_NO, 0, word_address);
		while (!TWI_ByteSent(pTwi));
		return SHA204_SUCCESS;
	}

	// more than one byte to send
	TWI_StartWrite(pTwi, device_address, AT91C_TWI_IADRSZ_NO, 0, word_address);
	while (!TWI_ByteSent(pTwi));

	while (--count > 0) {
		TWI_WriteByte(pTwi, *buffer++);
		while (!TWI_ByteSent(pTwi));
	}
	// Send last byte and a stop.
	TWI_Stop(pTwi);
	TWI_WriteByte(pTwi, *buffer);
	while (!TWI_TransferComplete(pTwi));

	return SHA204_SUCCESS;
}


/** \brief This TWI function sends a command to the device.
 * \param[in] count number of bytes to send
 * \param[in] command pointer to command buffer
 * \return status of the operation
 */
uint8_t sha204p_send_command(uint8_t count, uint8_t *command)
{
	return sha204p_twi_send(SHA204_TWI_PACKET_FUNCTION_NORMAL, count, command);
}


/** \brief This TWI function puts the SHA204 device into idle state.
 * \return status of the operation
 */
uint8_t sha204p_idle(void)
{
	return sha204p_twi_send(SHA204_TWI_PACKET_FUNCTION_IDLE, 0, NULL);
}


/** \brief This TWI function puts the SHA204 device into low-power state.
 *  \return status of the operation
 */
uint8_t sha204p_sleep(void)
{
	return sha204p_twi_send(SHA204_TWI_PACKET_FUNCTION_SLEEP, 0, NULL);
}


/** \brief This TWI function resets the I/O buffer of the SHA204 device.
 * \return status of the operation
 */
uint8_t sha204p_reset_io(void)
{
	return sha204p_twi_send(SHA204_TWI_PACKET_FUNCTION_RESET, 0, NULL);
}

/** \brief This function reads one byte from the device.
 *  \param[out] byte pointer to received byte
 *  \return status of the operation
*/
uint8_t sha204p_read_byte(uint8_t *byte)
{
	volatile uint32_t twi_status;

    // Send Start condition, send device address, send read bit,
	// receive byte, and send Stop condition.
    pTwi->TWI_CR = AT91C_TWI_START | AT91C_TWI_STOP;

	do {
		twi_status = pTwi->TWI_SR;
	} while (!(twi_status & AT91C_TWI_TXCOMP) && !(twi_status & AT91C_TWI_NACK_SLAVE));

	if (twi_status & AT91C_TWI_NACK_SLAVE)
		// SHA204 is busy executing a command.
		return SHA204_RX_NO_RESPONSE;

	while (!TWI_ByteReceived(pTwi));

	*byte = TWI_ReadByte(pTwi);

	return SHA204_SUCCESS;
}


/** \brief This TWI function receives a response from the SHA204 device.
 *
 * \param[in] size size of rx buffer
 * \param[out] response pointer to rx buffer
 * \return status of the operation
 */
uint8_t sha204p_receive_response(uint8_t size, uint8_t *response)
{
	uint8_t count;
	uint8_t ret_code;

    // Set slave address and number of internal address bytes.
    pTwi->TWI_MMR = 0;
    pTwi->TWI_MMR = AT91C_TWI_MREAD | (device_address << 16);

    // Set internal address bytes.
    pTwi->TWI_IADR = 0;

	ret_code = sha204p_read_byte(&count);
	if (ret_code != SHA204_SUCCESS)
		return ret_code;

	*response++ = count;

	if ((count < SHA204_RSP_SIZE_MIN) || (count > size))
		return SHA204_INVALID_SIZE;

	while (--count > 0) {
		ret_code = sha204p_read_byte(response++);
		if (ret_code != SHA204_SUCCESS)
			return ret_code;
	}

	return SHA204_SUCCESS;
}


/** \brief This TWI function resynchronizes communication.
 *
 * Parameters are not used for TWI.\n
 * Re-synchronizing communication is done in a maximum of three steps
 * listed below. This function implements the first step. Since
 * steps 2 and 3 (sending a Wake-up token and reading the response)
 * are the same for TWI and SWI, they are
 * implemented in the communication layer (#sha204c_resync).
  <ol>
     <li>
       To ensure an IO channel reset, the system should send
       the standard TWI software reset sequence, as follows:
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
	TWI_Stop(pTwi);
	TWI_StartWrite(pTwi, 0, AT91C_TWI_IADRSZ_NO, 0, 0);
	while (!TWI_ByteSent(pTwi));

	// Try to send a Reset IO command if re-sync succeeded.
	return sha204p_reset_io();
}
