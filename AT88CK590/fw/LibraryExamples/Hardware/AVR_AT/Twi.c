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
 *  \brief Functions of CryptoAuthentication Communication Layer That Are Closest to Hardware
 *  \author Nelson Lunsford
 *  \date Feb 23, 2010
 */

#include <ctype.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <avr/io.h>
#include <util/twi.h>

#include "delay_x.h"
#include "Physical.h"
#include "SHA_Status.h"


// ******************************************************************************
// ******************* hardware / system related definitions ********************
// ******************************************************************************

#define SIG1_PORT_DDR      (DDRD)                           //!< direction register for device id 0
#define SIG1_PORT_OUT      (PORTD)                          //!< output port register for device id 0
#define SIG1_PORT_IN       (PIND)                           //!< input port register for device id 0
#define SIG1_BIT           (PD1)                            //!< bit position of port register for device id 0
#define SIG1_LOW           SIG1_PORT_OUT &= ~_BV(SIG1_BIT)  //!< set pin for device id 0 low
#define SIG1_HIGH          SIG1_PORT_OUT |= _BV(SIG1_BIT)   //!< set pin for device id 0 high

#define NO_DEVICE_ID		(0xFF)							//!< no device selected

/****************************************************************************************************/
// MACROS
/****************************************************************************************************/
#define TWSR_status					(TWSR & 0xF8)
#define TWSR_status_is_not(cond) 	(TWSR_status != cond)		//< TWI TWSR status query
#define TWSR_status_is(cond) 		(TWSR_status == cond)		//< TWI TWSR status query
#define TWBR_VAL					0x0A
//#define TWBR_VAL					0x0C	///< Value to program in the TWI baudrate register
//#define TWBR_VAL					0x18	///< Value to program in the TWI baudrate register

// ******************************************************************************
// ******************* end of hardware / system related definitions *************
// ******************************************************************************

#define READ_MASK					0x01		//!< Bit0 = 1 tells TWI slave HW that the master is reading

//! Variable indicating which TWI device that is being communicated to
uint8_t deviceAddress = 0xC9;


/** \brief Sets the TWI address. Communication functions will use this address.
 *
 *  \param[in] address TWI address
 *  \return status of the operation
 ****************************************************************
 */
int8_t SHAP_SetDeviceID(uint8_t address)
{
	deviceAddress = address;

	return SHA_SUCCESS;
}


/** \brief 	Initializes TWI peripheral.
 *  \return success
 */
int8_t SHAP_Init()
{

	// Initialize TWI
	TWBR	= TWBR_VAL;		// Define the baud rate
	TWCR	= _BV(TWEN) | _BV(TWINT) | _BV(TWSTA);	// Enable TWI transceiver
	while (!(TWCR & (1<<TWINT)));

	return SHA_SUCCESS;
}


/** \brief Sets the signal low or high.
 *  \param[in] high if zero set signal low, otherwise high
 */
void SHAP_SetSignalPin(uint8_t high)
{
	// Disable the TWI function
	SHAP_CloseChannel();

	// Make sure that the bit is an output
	SIG1_PORT_DDR |= _BV(SIG1_BIT);

	if (high)
		SIG1_HIGH;
	else
		SIG1_LOW;

}

/** \brief Disable the TWI communications channel
 *
 */
int8_t SHAP_CloseChannel(void)
{
	// Disable the TWI function
	TWCR &= ~_BV(TWEN);

	return SHA_SUCCESS;
}

/** \brief Enable the TWI communications channel
 *
 */
int8_t SHAP_OpenChannel(void)
{
	// Enable the TWI function
	TWCR |= _BV(TWEN);

	return SHA_SUCCESS;
}

/** \brief Send bytes to the device.
 *  \param[in] count number of bytes to send
 *  \param[in] buffer pointer to tx buffer
 * \return status of the operation
 */
int8_t SHAP_SendBytes(uint8_t count, uint8_t *buffer)
{

	uint16_t				i;

	if (!count || !buffer)
		return SHA_BAD_PARAM;
/*
	TWDR = *buffer++;
	count--;
	// Enable the transceiver in Master Transmit mode and initiate a START condition
	TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWSTA);
	loop_until_bit_is_set(TWCR, TWINT);
	if( TWSR_status_is_not(TW_START) && TWSR_status_is_not(TW_REP_START))
	{
		// Transmit all of payload or terminate and return on error.
		for(i=0; i<count; i++)
		{
			TWDR = *buffer++;
			TWCR = _BV(TWEN) | _BV(TWINT);

			loop_until_bit_is_set(TWCR, TWINT);
			if( TWSR_status_is_not(TW_MT_SLA_ACK) && TWSR_status_is_not(TW_MT_DATA_ACK)) // Verify success on each byte
			{
				TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWSTO);
				return SHA_COMM_FAIL;
			}
		}
		// Issue a TWI stop command to terminate transaction if desired
		TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWSTO);

	}
	else
	{
		TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWSTO);
		return SHA_COMM_FAIL;
	}
*/

//   MODIFIED WORKING CODE
//   I changed function TWSR_status_is_not(condition) to TWSR_status_is(condition)

// Enable the transceiver in Master Transmit mode and initiate a START condition
	TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWSTA);
	loop_until_bit_is_set(TWCR, TWINT);

	if( TWSR_status_is(TW_START) || TWSR_status_is(TW_REP_START))
	{
		// Transmit all of payload or terminate and return on error.
		for(i=0; i<count; i++)
		{
			TWDR = *buffer++;
			TWCR = _BV(TWEN) | _BV(TWINT);

			loop_until_bit_is_set(TWCR, TWINT);
			if( TWSR_status_is_not(TW_MT_SLA_ACK) && TWSR_status_is_not(TW_MT_DATA_ACK)) // Verify success on each byte
			{
				TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWSTO);
				return SHA_COMM_FAIL;
			}
		}
		// Issue a TWI stop command to terminate transaction if desired
		TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWSTO);

	}
	else
	{
		TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWSTO);
		return SHA_COMM_FAIL;
	}

	return SHA_SUCCESS;
}

/** \brief Receives bytes from device.
 *  \param[in] count number of bytes to receive
 *  \param[in] buffer pointer to rx buffer
 * \return status of the operation
 */
int8_t SHAP_ReceiveBytes(uint8_t count, uint8_t *buffer)
{

	uint16_t	i;
	uint8_t 	twiAddress;

	if (!count || !buffer)
		return SHA_BAD_PARAM;

	twiAddress = deviceAddress | READ_MASK;

	// Request a read
	// TWDR Gets transmitted when this next line is executed
	TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWSTA);
	for (i=0; i<10; i++);

	loop_until_bit_is_set(TWCR, TWINT);
	if ((TWSR & 0xF8) == 0x08)
	{
		TWDR = twiAddress;
		TWCR = (1<<TWINT) | (1<<TWEN);
		while (!(TWCR & (1<<TWINT)));

		for(i=0; i<count; i++)
		{
			TWCR = _BV(TWEN) | _BV(TWINT)| _BV(TWEA);
			loop_until_bit_is_set(TWCR, TWINT);
			if(TWSR_status_is_not(TW_MR_DATA_ACK)) return SHA_COMM_FAIL;
			*buffer++ = TWDR;
		}
		// Issue a TWI stop command to terminate transaction
		TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWSTO);
	}
	else
	{
		TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWSTO);
		return SHA_COMM_FAIL;
	}

	return SHA_SUCCESS;
}
