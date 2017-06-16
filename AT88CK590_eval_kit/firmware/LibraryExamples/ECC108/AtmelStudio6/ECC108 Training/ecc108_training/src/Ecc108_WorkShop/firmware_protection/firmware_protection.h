/*
 * firmware_protection.h
 *
 * Created: 3/26/2013 11:05:14 AM
 *  Author: james.tomasetta
 */ 


#ifndef FIRMWARE_PROTECTION_H_
#define FIRMWARE_PROTECTION_H_

#include <stdint.h>                   // data type definitions
#include <conf_atsha204.h>            // firmware configuration for the device

void firmware_protection(void);

#define SHA204_KEY_ID_FIXED_CHALLENGE_RESPONSE            (0x0000)
#define KEY_ID 14



#endif /* FIRMWARE_PROTECTION_H_ */