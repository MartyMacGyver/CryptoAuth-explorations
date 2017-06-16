/**
 * header file for the TWI parser module
 *
 * \date 8/2/2011 10:57:29 AM
 * \author gunter.fuchs
 * \todo Add Doxygen comments.
 */ 


#ifndef TWI_H_
#define TWI_H_

#include "twi_slave.h"


// derived from Microbase KIT_STATUS...
enum {
	ENGR_10490_STATUS_SUCCESS         = 0x00,
	ENGR_10490_STATUS_UNKNOWN_COMMAND = 0xC0,
	ENGR_10490_STATUS_TWI_RX_OVERFLOW = 0xC1,
	ENGR_10490_STATUS_TWI_BUSY        = 0xC2,
	ENGR_10490_STATUS_TWI_RX_ERROR    = 0xC3,
	ENGR_10490_STATUS_TWI_GOT_POLLED  = 0xC4,
};

// targets
enum {
	ENGR_10490_TARGET_DEVICE          = 0x00,
	ENGR_10490_TARGET_WAKEUP          = 0x01,
	ENGR_10490_TARGET_PULLUP          = 0x02,
	ENGR_10490_TARGET_TIMEOUT         = 0x03,
	ENGR_10490_TARGET_VERSION         = 0x04,
};


void twi_init(void);
uint8_t twi_receive(uint8_t size, command_t *command);
uint8_t twi_send(response_t *response);

#endif /* TWI_H_ */