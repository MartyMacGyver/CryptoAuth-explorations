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
 * \brief This file contains top-level functions for the Microbase protocol parser.
 * \author Atmel Crypto Products
 * \date July 5, 2013
 */

#include <string.h>
#include <ctype.h>

#include "config.h"
#include "kitStatus.h"
#include "parserAscii.h"
#include "utilities.h"
#include "hardware.h"

#if TARGET_BOARD == NO_TARGET_BOARD
#   error You have to define TARGET_BOARD > 0
#endif


#if defined(SHA204)
#   include "sha204_comm.h"
#   include "sha204_lib_return_codes.h"
#endif

#if defined( AES132)
#   ifdef AES132_VERSION_2
#      include "aes132.h"
#   else
#      include "aes132_comm.h"
#      include "aes132_lib_return_codes.h"
#   endif
#endif

#if defined(ECC108)
#   include "ecc108_comm.h"
#   include "ecc108_lib_return_codes.h"
#endif

#if !defined(SHA204) && !defined(ECC108) && !defined(AES132) && !defined(SA10X) && !defined(RHINO_RED)
#   error no device or board defined (SHA204 or ECC108 or AES132 or SA10X or RHINO_RED)
#endif


//! USB packet state.
enum usb_packet_state
{
  PACKET_STATE_IDLE,
  PACKET_STATE_TAKE_DATA,
  PACKET_STATE_END_OF_VALID_DATA,
  PACKET_STATE_OVERFLOW
};

//! USB receive data buffer
uint8_t pucUsbRxBuffer[USB_BUFFER_SIZE_RX];
//! USB send data buffer
#if TARGET_BOARD == AT88CK460
   // The SA102 library and the ECC108 device use too much RAM. Use one buffer for both
   // operations, USB receive and transmit.
   // Be aware that a CRC error in the status byte causes the SHA204 / ECC108 library to resend
   // the command. When sharing rx and tx buffer, the library will resend the status response.
   // !!! There is currently no host application that supports this Rhino Red ASCII protocol !!!
   uint8_t *pucUsbTxBuffer = pucUsbRxBuffer;
#else
   uint8_t pucUsbTxBuffer[USB_BUFFER_SIZE_TX];
#endif

static uint8_t packetCollatingState = PACKET_STATE_IDLE;	//!< state of packet collator
static uint8_t rxPacketStatus = KIT_STATUS_SUCCESS;
static uint8_t packetReceived = FALSE;
static uint16_t rxBufferIndex = 0;
uint8_t is_talking = FALSE;


/** \brief This function returns the rx buffer.
 *  \return pointer to the current rx buffer index
 */
uint8_t *GetRxBuffer(void)
{
	return &pucUsbRxBuffer[rxBufferIndex];
}


/** \brief This function resets the rx buffer.
 * \param[in] reset_value value to be written into buffer
 * \return pointer to start of rx buffer
 */
uint8_t *ResetRxBuffer(uint8_t reset_value)
{
	memset(pucUsbRxBuffer, reset_value, sizeof(pucUsbRxBuffer));
	rxBufferIndex = 0;
	return pucUsbRxBuffer;
}


/** \brief This function converts binary response data to hex-ascii and packs it into a protocol response.
           <status byte> <'('> <hex-ascii data> <')'> <'\n'>
    \param[in] length number of bytes in data load plus one status byte
    \param[in] buffer pointer to data
    \return length of ASCII data
*/
uint16_t CreateUsbPacket(uint16_t length, uint8_t *buffer)
{
	uint16_t binBufferIndex = length - 1;
	// Size of data load is length minus status byte.
	uint16_t asciiLength = 2 * (length - 1) + 5; // + 5: 2 status byte characters + '(' + ")\n"
	uint16_t asciiBufferIndex = asciiLength - 1;
	uint8_t byteValue;

	// Terminate ASCII packet.
	buffer[asciiBufferIndex--] = KIT_EOP;

	// Append ')'.
	buffer[asciiBufferIndex--] = ')';

	// Convert binary data to hex-ascii starting with the last byte of data.
	while (binBufferIndex)
	{
		byteValue = buffer[binBufferIndex--];
		buffer[asciiBufferIndex--] = ConvertNibbleToAscii(byteValue);
		buffer[asciiBufferIndex--] = ConvertNibbleToAscii(byteValue >> 4);
	}

	// Start data load with open parenthesis.
	buffer[asciiBufferIndex--] = '(';

	// Convert first byte (function return value) to hex-ascii.
	byteValue = buffer[0];
	buffer[asciiBufferIndex--] = ConvertNibbleToAscii(byteValue);
	buffer[asciiBufferIndex] = ConvertNibbleToAscii(byteValue >> 4);

	return asciiLength;
}


/** \brief This function converts binary data to Hex-ASCII.
 * \param[in] length number of bytes to send
 * \param[in] buffer pointer to tx buffer
 * \return new length of data
 */
static uint16_t ConvertData(uint16_t length, uint8_t *buffer)
{
	if (length > DEVICE_BUFFER_SIZE_MAX_RX) {
		buffer[0] = KIT_STATUS_USB_TX_OVERFLOW;
		length = DEVICE_BUFFER_SIZE_MAX_RX;
	}
	return CreateUsbPacket(length, buffer);
}


/** \brief This function processes a USB rx packet.
 *
 *         The first byte in #pucUsbTxBuffer is reserved for the function return value.
 *
 *  \param[out] txLength size of response
 *  \return pointer to response buffer
 * */
uint8_t *ProcessUsbPacket(uint16_t *txLength)
{
	uint8_t status = KIT_STATUS_SUCCESS;
	uint8_t responseIsAscii = FALSE;
	uint16_t i;
	uint16_t rxLength = rxBufferIndex - 1;
	uint8_t *txBuffer = pucUsbTxBuffer;

	char *pRxBuffer = (char *) pucUsbRxBuffer;

//	Led2(TRUE); // on while request is being processed
		Led1(FALSE);  //SHA204
		Led2(FALSE);  //AES132
		Led3(FALSE);  //ECC108

		delay_ms(1);

	if (rxPacketStatus != KIT_STATUS_SUCCESS) {
		pucUsbTxBuffer[0] = rxPacketStatus;
		*txLength = 1;
		*txLength = ConvertData(*txLength, pucUsbTxBuffer);
	}

	else {

#if TARGET_BOARD != AT88CK460
		// Don't reset tx buffer for Rhino White because the rx buffer
		// is being reused as tx buffer.
  		memset(pucUsbTxBuffer, 0, USB_BUFFER_SIZE_TX);
#endif

		// Process packet.
		for (i = 0; i < rxLength; i++)
			pRxBuffer[i] = tolower(pRxBuffer[i]);

		if (pRxBuffer[0] == 'l') {	// lib
			// "lib" as the first field is optional. Move rx pointer to the next field.
			pRxBuffer = memchr(pRxBuffer, ':', rxBufferIndex);
			if (!pRxBuffer)
				status = KIT_STATUS_UNKNOWN_COMMAND;
			else
				pRxBuffer++;
		}

		switch (pRxBuffer[0]) {
//#if defined(AES132)
			case 'a':	// "aes:"; parse AES132 library commands.
				status = ParseAesCommands((uint8_t) rxLength, (uint8_t *) pRxBuffer, txLength, pucUsbTxBuffer + 1);
				Led2(TRUE);  //AES132
				break;
//#endif

//#if defined(SHA204)
			case 's':	// "sha204:"; parse SHA204 library commands
				status = ParseShaCommands(rxLength, (uint8_t *) pRxBuffer, txLength, pucUsbTxBuffer + 1);
				Led1(TRUE);  //SHA204
				break;
//#elif defined(ECC108)
			// For Rhino White: We use 's' for now instead of 'e'.
			// Once ACES has switch ECC commands from sha204 to ecc108, we can use this function.
//			case 's':	// "sha204:"; parse raw ECC108 commands
			case 'e':	// "ecc108:"; parse raw ECC108 commands
				status = ParseShaCommands(rxLength, (uint8_t *) pRxBuffer, txLength, pucUsbTxBuffer + 1);
//				status = ParseEccCommands(rxLength, (uint8_t *) pRxBuffer, txLength, pucUsbTxBuffer + 1);
				Led3(TRUE);  //ECC108
				break;
//#elif defined(RHINO_RED) || defined(SA10X)
			/** \todo Fix type of txLength. */
//			case 's':  // "sa1:"; parse SA10X library commands
//			status = ParseSaCommands((uint8_t) rxLength, (uint8_t *) pRxBuffer, txLength, pucUsbTxBuffer + 1);
//			break;
//#endif
			case 'b':
				// board level commands ("b[oard]")
				status = ParseBoardCommands((uint8_t) rxLength, (uint8_t *) pRxBuffer,
													txLength, txBuffer, &responseIsAscii);
				break;

#if (TARGET_BOARD == AT88UBASE) || (TARGET_BOARD == XMEGA_A1_XPLAINED)
			case 'i':
				// bus interface commands (hardware dependent part of Physical layer)
				// SWI, I2C, SPI: short: '1', '2', '3'; long: "swi", "i2c", "spi"
				status = ParseBusCommands(rxLength, (uint8_t *) pRxBuffer, txLength, pucUsbTxBuffer + 1);
				break;
#endif
			default:
				status = KIT_STATUS_UNKNOWN_COMMAND;
				*txLength = 1;
				break;
		}

		if (!responseIsAscii) {
			// Copy leading function return byte.
			pucUsbTxBuffer[0] = status;
			// Tell ConvertData the correct txLength.
			if (*txLength < DEVICE_BUFFER_SIZE_MAX_RX)
				(*txLength)++;
			*txLength = ConvertData(*txLength, pucUsbTxBuffer);
		}
	}

	packetReceived = FALSE;
	is_talking = FALSE;

	// Done with command execution. Switch LED2 off.
//	Led2(FALSE);
//		Led1(FALSE);  //SHA204
//		Led2(FALSE);  //AES132
//		Led3(FALSE);  //ECC108

	// Indicate error condition.
//	Led3(status);

	return txBuffer;
}


/** \brief This function assembles a complete protocol packet.
 *
 * It assumes that the last part of a packet is padded to EP_LENGTH.
 * \param[in] count number of bytes in buffer
 * \param[in, out] buffer pointer to pointer that returns the current rx pointer
 * \return non-zero if EOP has been received, zero otherwise
 * */
uint8_t CollateUsbPacket(uint8_t count, uint8_t **usb_buffer)
{
	uint8_t i;

	for (i = 0; i < count; i++)
	{
		switch (packetCollatingState)
		{
			case PACKET_STATE_IDLE:
				rxBufferIndex = 0;
				rxPacketStatus = KIT_STATUS_SUCCESS;
				packetCollatingState = PACKET_STATE_TAKE_DATA;
				packetReceived = FALSE;
				is_talking = TRUE;
				// intentionally no break here

			case PACKET_STATE_TAKE_DATA:
				if (pucUsbRxBuffer[rxBufferIndex] == KIT_EOP) {
					packetCollatingState = PACKET_STATE_IDLE;
					packetReceived = TRUE;
					*usb_buffer = &pucUsbRxBuffer[rxBufferIndex++];
					return packetReceived;
				}
				if (rxBufferIndex >= sizeof(pucUsbRxBuffer)) {
					packetCollatingState = PACKET_STATE_OVERFLOW;
				}
				else
					rxBufferIndex++;
				break;

			case PACKET_STATE_OVERFLOW:
				if (pucUsbRxBuffer[rxBufferIndex] == KIT_EOP) {
					*usb_buffer = &pucUsbRxBuffer[rxBufferIndex];
					packetCollatingState = PACKET_STATE_IDLE;
					rxPacketStatus = KIT_STATUS_USB_RX_OVERFLOW;
					packetReceived = TRUE;
					return packetReceived;
				}
				break;
		}
	}
	*usb_buffer = &pucUsbRxBuffer[rxBufferIndex];
	return packetReceived;
}
