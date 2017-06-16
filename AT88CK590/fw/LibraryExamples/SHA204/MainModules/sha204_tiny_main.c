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
 *  \brief  Host example of an Application That Uses the SHA204 Library
 *  \author Atmel Crypto Products
 *  \date   November 22, 2011
*/

#include <stdint.h>
#include <avr/io.h>
#include <string.h>
#include "i2c_phys.h"
#include "soft_uart.h"
#include "delay_x.h"

#define SECURITY_XPLAINED
#ifdef SECURITY_XPLAINED
// Security Xplained SHA204 TWI address of first device (out of four)
#   define SHA204_TWI_ADDRESS_WRITE  (0xCA)
#   define SHA204_TWI_ADDRESS_READ   (0xCB)
#else
// SHA204 TWI default address
#   define SHA204_TWI_ADDRESS_WRITE  (0xC8)
#   define SHA204_TWI_ADDRESS_READ   (0xC9)
#endif

#define SHA204_RSP_SIZE_MAX       (35)
#define SHA204_RSP_SIZE_MIN       ( 4)
#define SHA204_CMD_SIZE_MAX       (84)
#define SHA204_CRC_SIZE           ( 2)

// definitions for packet indexes
#define SHA204_COMM_BUFFER_INDEX_TWI_ADDRESS   0
// transmit
#define SHA204_COMM_BUFFER_INDEX_FUNCTION      1
#define SHA204_COMM_BUFFER_INDEX_COUNT_TX      2
#define SHA204_COMM_BUFFER_INDEX_OPCODE        3
#define SHA204_COMM_BUFFER_INDEX_PARAM1        4
#define SHA204_COMM_BUFFER_INDEX_PARAM2_LSB    5
#define SHA204_COMM_BUFFER_INDEX_PARAM2_MSB    6
#define SHA204_COMM_BUFFER_INDEX_DATA_TX       7
// receive
#define SHA204_COMM_BUFFER_INDEX_COUNT_RX      1
#define SHA204_COMM_BUFFER_INDEX_STATUS        2
#define SHA204_COMM_BUFFER_INDEX_DATA_RX       2

// definitions for CheckMac command
#define CHECKMAC_COUNT              (84)       //!< CheckMAC command packet size
#define SHA204_CHECKMAC           (0x28)       //!< CheckMac command op-code

// definitions for Mac commmand (needed as part of CheckMac data)
#define SHA204_MAC                (0x08)       //!< MAC command op-code


enum i2c_word_address {
	SHA204_I2C_PACKET_FUNCTION_RESET,  //!< Reset device.
	SHA204_I2C_PACKET_FUNCTION_SLEEP,  //!< Put device into Sleep mode.
	SHA204_I2C_PACKET_FUNCTION_IDLE,   //!< Put device into Idle mode.
	SHA204_I2C_PACKET_FUNCTION_NORMAL  //!< Write / evaluate data that follow this word address byte.
};


// Make the command buffer the size of the longest
// command we shall be using, including I2C address
// and packet function bytes, the CheckMac command.
uint8_t sha204_comm_buffer[SHA204_CMD_SIZE_MAX + 2];


/** \brief This function calculates CRC.
 *
 * \param[in] length number of bytes in buffer
 * \param[in] data pointer to data for which CRC should be calculated
 * \param[out] crc pointer to 16-bit CRC
 */
void sha204c_calculate_crc(uint8_t length, uint8_t *data, uint8_t *crc) {
	uint8_t counter;
	uint16_t crc_register = 0;
	uint16_t polynom = 0x8005;
	uint8_t shift_register;
	uint8_t data_bit, crc_bit;

	for (counter = 0; counter < length; counter++) {
	  for (shift_register = 0x01; shift_register > 0x00; shift_register <<= 1) {
		 data_bit = (data[counter] & shift_register) ? 1 : 0;
		 crc_bit = crc_register >> 15;
		 crc_register <<= 1;
		 if (data_bit != crc_bit)
			crc_register ^= polynom;
	  }
	}
	crc[0] = (uint8_t) (crc_register & 0x00FF);
	crc[1] = (uint8_t) (crc_register >> 8);
}


/** \brief This function wakes up the SHA204 device.
 */
void sha204_wakeup(void)
{
	USI_TWI_Disable();
	DDR_USI  |= (1 << PIN_USI_SDA);   // Enable SDA as output.
	PORT_USI &= ~(1 << PIN_USI_SDA);  // Pull SDA low.
   _delay_us(60);
   PORT_USI |= (1 << PIN_USI_SDA);   // Pull SDA high.
   _delay_ms(3);
	USI_TWI_Master_Initialise();
}


/** \brief This function puts the SHA204 device to sleep.
 */
uint8_t sha204_sleep(void)
{
	uint8_t sha204_sleep[] = {SHA204_TWI_ADDRESS_WRITE, SHA204_I2C_PACKET_FUNCTION_SLEEP};
	return USI_TWI_Start_Transceiver_With_Data(sha204_sleep, sizeof(sha204_sleep));
}


/** \brief This functions sends a CheckMac command to the device and receives its response.
 *  \return communication success (TRUE) or failure (FALSE)
 */
uint8_t sha204_check_mac(void)
{
	// Create CheckMac command packet. The challenge / response data have already been copied
	// to the command buffer.
	uint8_t polling_count;
	uint8_t *buffer = sha204_comm_buffer;

	*buffer++ = SHA204_TWI_ADDRESS_WRITE; // TWI write address
	*buffer++ = SHA204_I2C_PACKET_FUNCTION_NORMAL; // Command packet follows.
	*buffer++ = CHECKMAC_COUNT;           // CheckMac command packet size including this byte and two bytes CRC
	*buffer++ = SHA204_CHECKMAC;          // CheckMac command op-code
	*buffer++ = 0;                        // CheckMac command param1 (mode)
	*buffer++ = 0;                        // CheckMac command param2 LSB (key id)
	*buffer++ = 0;                        // CheckMac command param2 MSB (always 0)
	buffer += 64;                         // Jump over challenge / response
	*buffer++ = SHA204_MAC;
	memset(buffer, 0x00, CHECKMAC_COUNT - 70);

	sha204c_calculate_crc(CHECKMAC_COUNT - SHA204_CRC_SIZE,
				&sha204_comm_buffer[SHA204_COMM_BUFFER_INDEX_COUNT_TX],
				&sha204_comm_buffer[SHA204_COMM_BUFFER_INDEX_COUNT_TX + CHECKMAC_COUNT - SHA204_CRC_SIZE]);

	// Send CheckMac command.
	uint8_t ret_code = USI_TWI_Start_Transceiver_With_Data(sha204_comm_buffer, sizeof(sha204_comm_buffer));
	if (ret_code == FALSE)
		return ret_code;

	// Poll for TWI address ack and read response.
	sha204_comm_buffer[SHA204_COMM_BUFFER_INDEX_TWI_ADDRESS] = SHA204_TWI_ADDRESS_READ;
	polling_count = 38; // ms
   do {
   	ret_code = USI_TWI_Start_Transceiver_With_Data(sha204_comm_buffer, 5);
   	_delay_ms(1);
   } while ((USI_TWI_Get_State_Info() == USI_TWI_NO_ACK_ON_ADDRESS) && polling_count--);
   if ((ret_code == FALSE) || !polling_count)
   	return ret_code;
//   do {
//   	ret_code = USI_TWI_Start_Transceiver_With_Data(sha204_comm_buffer, 5);
//   } while (USI_TWI_Get_State_Info() == USI_TWI_NO_ACK_ON_ADDRESS);
//   if (ret_code == FALSE)
//   	return ret_code;

   // Evaluate response status.
   // todo Compare all four response bytes (0x04, 0x00, 0x03, 0x40) if code space allows.
   return (sha204_comm_buffer[SHA204_COMM_BUFFER_INDEX_STATUS] ? FALSE : TRUE);
}


/** \brief This function serves as an example that shows
 *         how to use the SHA204 as a host device.
 *
 * This host application communicates with a client device
 * (ATmega) through a UART to authenticate the client.
 * <ul>
 *   <li>Send a Mac command to client.
 *   <li>Send a CheckMac command to host.
 * </ul>
 *
 * Protocol: <count><data><two-byte crc>
 *           For now, we only send zeroes for CRC.
 *
 * \return exit status of application
 */
int main(void)
{
	uint8_t ret_code = 0;
	uint8_t uart_byte;
	uint8_t *challenge_response = &sha204_comm_buffer[SHA204_COMM_BUFFER_INDEX_DATA_TX];

	suart_init();

	while (TRUE) {
		// Wait for command from transmitter. For now, we expect only the MAC
		// information from the client SHA204.
		// challenge_response = &sha204_comm_buffer[SHA204_COMM_BUFFER_INDEX_DATA_TX];
		suart_receive_bytes(1, &uart_byte);
		// todo We might have to introduce a state machine because count alone
		//      might be ambiguous.
		switch (uart_byte) {
		case 4:
			// version
			suart_receive_bytes(1, sha204_comm_buffer);
			if (sha204_comm_buffer[0] != 'v')
				continue;
			suart_send_bytes(6, (uint8_t *) "\x06\x00\x00\x01\x00\x00");
			break;

		case 67:
         // challenge / response data from client Mac command
			suart_receive_bytes(66, challenge_response);

			sha204_wakeup();

   #ifdef SHA204_READ_WAKEUP_RESPONSE
			uint8_t sha204_wakeup_response[5];
			sha204_wakeup_response[SHA204_COMM_BUFFER_INDEX_TWI_ADDRESS] = SHA204_TWI_ADDRESS_READ;
	      // msgSize parameter includes TWI address.
			ret_code = USI_TWI_Start_Transceiver_With_Data(sha204_wakeup_response, sizeof(sha204_wakeup_response));
			if (ret_code == FALSE)
				continue;
			if (sha204_wakeup_response[SHA204_COMM_BUFFER_INDEX_COUNT_RX] != (sizeof(sha204_wakeup_response) - 1))
				continue;
	#endif

			ret_code = sha204_check_mac();
			sha204_sleep();

         if (ret_code == FALSE) {
         	// Create fake error response using an invalid status byte.
         	sha204_comm_buffer[SHA204_COMM_BUFFER_INDEX_COUNT_RX] = SHA204_RSP_SIZE_MIN;
         	sha204_comm_buffer[SHA204_COMM_BUFFER_INDEX_STATUS] = USI_TWI_Get_State_Info();
         	sha204c_calculate_crc(2, &sha204_comm_buffer[SHA204_COMM_BUFFER_INDEX_COUNT_RX],
         				&sha204_comm_buffer[SHA204_COMM_BUFFER_INDEX_STATUS + 1]);
         }
      	suart_send_bytes(4, &sha204_comm_buffer[SHA204_COMM_BUFFER_INDEX_COUNT_RX]);

		default:
         // todo Send error response.
			continue;
		}
	}
	return (int) ret_code;
}

