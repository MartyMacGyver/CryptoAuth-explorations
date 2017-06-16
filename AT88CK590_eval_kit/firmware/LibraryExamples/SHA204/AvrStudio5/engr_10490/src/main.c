/**
 * This module contains the main function for the I2C to SWI bridge
 * that is used to communicate with a SHA204 device.
 *
 * \date: 8/2/2011 9:53:16 AM
 * \author gunter.fuchs
 */ 

#include <avr/io.h>
#include <string.h>
#include "twi.h"
#include "swi.h"
#include "sha204_physical.h"
#include "sha204_lib_return_codes.h"
#include "board.h"
#include "timers.h"


/** \brief This function is the entry point for the application.
 *  \return 0
 */
int main(void)
{
	static command_t command;
	static response_t response;
	uint8_t response_version[] = {1, 1, 0};
	uint8_t i;
   
	led_init();
	twi_init(); // Also enables interrupts.
	sha204p_init();
   
	for (i = 0; i < 2; i++) {
		led_on(ENGR_10490_LED1 | ENGR_10490_LED2);
		Timer_delay_ms(250);
		led_off(ENGR_10490_LED1 | ENGR_10490_LED2);
		Timer_delay_ms(250);
	}   
    
	while (1)
	{
		// Receive TWI host command.
		response.response_header.status = twi_receive(COMMAND_BUFFER_SIZE, &command);
		if (response.response_header.status != ENGR_10490_STATUS_SUCCESS) {
			// Evaluate error.
			if ((response.response_header.status != ENGR_10490_STATUS_TWI_BUSY) &&
				(response.response_header.status != ENGR_10490_STATUS_TWI_GOT_POLLED)) {
				response.response_header.count = 0;
       			twi_send(&response);
			}				
			continue;
		}      
	   
		// This also sets the header to default values (count = 0, status = 0: ENGR_10490_STATUS_SUCCESS).
		memset(&response, 0, sizeof(response_t));

		// Process TWI host command.
		switch (command.command_header.target) {
		case ENGR_10490_TARGET_DEVICE:
			if (command.command_header.rx_count > RESPONSE_BUFFER_SIZE) {
				response.response_header.status = SHA204_BAD_PARAM;
				break;
			}
			response.response_header.status = sha204_communicate(command.data, response.data, command.command_header.rx_count);
			response.response_header.count = (response.response_header.status == SHA204_SUCCESS) 
				? response.data[SHA204_BUFFER_POS_COUNT] : 0;
		    break;
			  
		case ENGR_10490_TARGET_WAKEUP:
			set_wakeup_delay(command.data[0]);
			break;

		case ENGR_10490_TARGET_PULLUP:
			set_pullup(command.data[0]);
			break;

		case ENGR_10490_TARGET_TIMEOUT:
			set_timeout(command.data[0]);
			break;

		case ENGR_10490_TARGET_VERSION:
			response.response_header.status = ENGR_10490_STATUS_SUCCESS;
			response.response_header.count = sizeof(response_version);
			memcpy(response.data, response_version, sizeof(response_version));
			break;
			   
		default:
			response.response_header.status = ENGR_10490_STATUS_UNKNOWN_COMMAND;
			response.response_header.count = 0;
			break;
		}
  		twi_send(&response);
   }
   return 0;
}