/**
 * \file
 *
 * \brief ATSHA204 CryptoAuth component configuration file
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
#ifndef CONF_ATSHA204_H_INCLUDED
#define CONF_ATSHA204_H_INCLUDED

#include <board.h>

// Interface configuration (J4) for XMEGA-A1 and XMEGA-A3BU Xplained
#if BOARD == XMEGA_A1_XPLAINED
#  define ATSHA204_TWI_PORT            (&TWIC)
#elif BOARD == XMEGA_A3BU_XPLAINED
#  define ATSHA204_TWI_PORT            (&TWIE)
#else
// Interface configuration for other boards
#  error ATSHA204 driver must be configured. Please see conf_atsha204.h.
#endif // BOARD

// Board independent configuration
#define ATSHA204_TWI_SPEED             (400000)

//! TWI address used at SHA204 library startup
#if EXT_BOARD == SECURITY_XPLAINED
#   define SHA204_I2C_DEFAULT_ADDRESS  (0xCA)
#else
#   define SHA204_I2C_DEFAULT_ADDRESS  (0xC8)
#endif

#endif /* CONF_ATSHA204_H_INCLUDED */
