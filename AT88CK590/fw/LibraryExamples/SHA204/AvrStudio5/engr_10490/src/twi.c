/**
 * This module contains TWI parser and slave functions.
 * adopted from Atmel application note AVR311
 *
 * \date 8/2/2011 10:26:15 AM
 * \author gunter.fuchs
 *
 * STOP header pin on ENGR-10490 is SDA, START is SCL.
 */


#include <stdint.h>
#include <string.h>
#include <avr/io.h>
#include "twi_slave.h"
#include "twi.h"
#include "board.h"
#include "sha204_lib_return_codes.h"


/** \brief This function initializes the TWI peripheral as slave.
 * 
 */
void twi_init(void)
{
	// Enable pull-ups.
	//DDRD &= ~(_BV(PD0) | _BV(PD1));
	//PORTD |= (_BV(PD0) | _BV(PD1));
   
	TWI_Slave_Initialise(0xC8);
	
	sei();
	
	// Sets lastTransOK to FALSE!
	TWI_Start_Transceiver(TRUE);
}


/** \brief This function starts the TWI transceiver.
 *         It is called when the TWI host does not expect a response.
 *  \return busy status if busy
 */
uint8_t twi_start_transceiver(void)
{
   if (TWI_Transceiver_Busy()) 
      return ENGR_10490_STATUS_TWI_BUSY;
   
   TWI_Start_Transceiver(TRUE);
   
   return ENGR_10490_STATUS_SUCCESS; 
}


/** \brief This function waits for and receives a TWI packet from a host.
 *         Packet format: <target><data length><expected response length><data>
 *         The <data> field is of variable length. All other fields are one byte long.
 *  \param[in]  buffer_size size of receive buffer
 *  \param[out] command pointer to command structure
 *  \return status of the operation
 */
uint8_t twi_receive(uint8_t buffer_size, command_t *command)
{
	uint8_t status = ENGR_10490_STATUS_SUCCESS;
	uint8_t twi_status = FALSE;
	
	command->command_header.tx_count = 0;
	
	// Check if the TWI Transceiver has completed an operation.
	if (TWI_Transceiver_Busy()) 
		return ENGR_10490_STATUS_TWI_BUSY;

	if (!TWI_statusReg.RxDataInBuf)
		return ENGR_10490_STATUS_TWI_BUSY;

	led_on(LED_TWI);

	// Get command header.
	twi_status = TWI_Get_Data_From_Transceiver((uint8_t *) command, sizeof(command_header_t));
	if (twi_status == FALSE) {
		// Disable ack until we are ready to send the error response.
		TWI_Start_Transceiver(FALSE);
		status = ENGR_10490_STATUS_TWI_RX_ERROR;
	}		  
	else if (command->command_header.tx_count + sizeof(command_header_t) > buffer_size) {
	    status = ENGR_10490_STATUS_TWI_RX_OVERFLOW;
		// Read and discard.
		while (TWI_Get_Data_From_Transceiver(command->data, 1) && TWI_statusReg.RxDataInBuf && !TWI_statusReg.timeOut);
        // Disable ack until we are ready to send the error response.
        TWI_Start_Transceiver(FALSE);
	}		  
	else if (command->command_header.tx_count > 0)  {
		// Reading command header succeeded. Read all.
		if (TWI_Wait_For_Transceiver_Idle() == FALSE)
			// Timed out.
   			status = ENGR_10490_STATUS_TWI_RX_ERROR;
		else {
			// Disable ack until we copied the TWI buffer and processed a command with data.
			TWI_Start_Transceiver(FALSE);
   			twi_status = TWI_Get_Data_From_Transceiver((uint8_t *) command, command->command_header.tx_count + sizeof(command_header_t));
			if (twi_status == FALSE)
   				status = ENGR_10490_STATUS_TWI_RX_ERROR;
		}			   
	}
	else
      // Disable ack until we processed a command without data.
      TWI_Start_Transceiver(FALSE);

	if (status == ENGR_10490_STATUS_SUCCESS)
		led_off(LED_ERROR);
	else
		led_on(LED_ERROR);

	return status;
}


/** \brief This function sends a response to the TWI host.
 *  \param[in] response pointer to response structure
 *  \return status of the operation
 */
uint8_t twi_send(response_t *response)
{
	uint8_t status = ENGR_10490_STATUS_SUCCESS;
	
    // Check if the TWI Transceiver has completed an operation.
    if (TWI_Transceiver_Busy()) {
	    response->response_header.count = 0;
	    response->response_header.status = ENGR_10490_STATUS_TWI_BUSY;
    }	  

    // Send response.
	uint8_t tx_length = response->response_header.count + sizeof(response_header_t);
	// Debug This does not prevent resetting.
	if (tx_length > sizeof(response_t)) {
		response->response_header.count = 0;
		response->response_header.status = SHA204_INVALID_SIZE;
		tx_length = sizeof(response_header_t);
	}
	TWI_Start_Transceiver_With_Data((uint8_t *) response, tx_length);
	
    if (status == ENGR_10490_STATUS_SUCCESS)
        led_off(LED_ERROR);
    else
        led_on(LED_ERROR);

    led_off(LED_TWI);

	return status;
}
