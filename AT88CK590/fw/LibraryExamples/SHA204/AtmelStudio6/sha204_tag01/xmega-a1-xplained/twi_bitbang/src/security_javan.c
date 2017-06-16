/**
 * \file
 *
 * \brief SECURITY_XPLAINED extension board adaptation.
 *
 *
 * Copyright (c) 2011-2012 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
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
 *    Atmel microcontroller product.
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
 * \asf_license_stop
 *
 */


#include "security_javan.h"
#include "sha204_physical.h"
#include "conf_atsha204.h"


/** \brief This function initializes peripherals needed to communicate with
 *         the I2C security device ATSHA204.
 */
void security_board_init(void)
{
	sha204p_init();
}


/** \brief This function returns the I2C address of a chosen SHA204 device.
 *  \param[in] index the selected device on the Security Xplained board
 *  \return I2C address of chosen device
 */
uint8_t sha204_i2c_address(uint8_t index)
{
	//static uint8_t i2c_addresses[SHA204_DEVICE_COUNT] = {SHA204_I2C_DEFAULT_ADDRESS, 0xCA};
	static uint8_t i2c_addresses[SHA204_DEVICE_COUNT] = {SHA204_I2C_DEFAULT_ADDRESS};
	return i2c_addresses[index % SHA204_DEVICE_COUNT];
}

