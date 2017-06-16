/** \file
 *  \brief  Application examples that Use the ECC108 Library
 *  \author Atmel Crypto Products
 *  \date   June 20, 2013
 *

* \copyright Copyright (c) 2013 Atmel Corporation. All rights reserved.
*
* \atmel_crypto_device_library_license_start
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
* 3. The name of Atmel may not be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
* 4. This software may only be redistributed and used in connection with an
*    Atmel integrated circuit.
*
* THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
* EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
* ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
* \atmel_crypto_device_library_license_stop

 *   Three example functions are given that demonstrate the device.
*/
#ifndef ECC108_EXAMPLES_H
#   define ECC108_EXAMPLES_H

#include <stdint.h>                   // data type definitions

// The examples demonstrate client / host device scenarios for I2C and SWI bitbang
// configurations.
#ifdef ECC108_I2C
// If you have two devices at your disposal you can run this example as a real-world
// host / client demonstration. You have to change the address of one of the devices
// by writing it to configuration zone address 16. Be aware that bit 3 of
// the I2C address is also used to configure the input level reference 
// (see data sheet table 2-1).
#   define ECC108_CLIENT_ADDRESS   (0xC0)
// To make the Mac / CheckMac examples work out-of-the-box, only one device is being 
// used as example default. See above.
#   define ECC108_HOST_ADDRESS     (0xC2)
#else
// These settings have an effect only when using bit-banging where the SDA of every 
// device is connected to its own GPIO pin. When using one UART the SDA of both 
// devices is connected to the same GPIO pin. In that case you have to use a Pause
// flag. (Refer to data sheet.)
#   define ECC108_CLIENT_ADDRESS   (0x00)
#   define ECC108_HOST_ADDRESS     (0x01)
#endif

#define ECC108_KEY_ID            (0x0000)

#define ecc108e_wakeup_sleep()   {ecc108p_wakeup(); ecc108p_sleep();}

uint8_t ecc108e_send_info_command(void);
uint8_t ecc108e_checkmac_device(void);
uint8_t ecc108e_checkmac_firmware(void);
uint8_t ecc108e_derive_key(void);
uint8_t ecc108e_change_i2c_address(void);
uint8_t ecc108e_read_config_zone(void);

#endif
