/**
 * \file
 *
 * \brief SECURITY_XPLAINED_BOARD board header file.
 *
 * This file contains definitions and services related to the features of the
 * SECURITY_XPLAINED Xplained board.
 *
 * To use the board, define EXT_BOARD=SECURITY_XPLAINED.
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
#ifndef SECURITY_JAVAN_H_
#   define SECURITY_JAVAN_H_

#include <compiler.h>

#if EXT_BOARD == SECURITY_XPLAINED
//! number of ATSHA204 I2C devices on Security Xplained extension board
#   define SHA204_DEVICE_COUNT         (4)
#elif EXT_BOARD == SECURITY_JAVAN
//! number of ATSHA204 I2C devices on Security Javan extension board
#   define SHA204_DEVICE_COUNT         (2)
#elif EXT_BOARD == SECURITY_CK101
//! number of ATSHA204 I2C devices on Security Javan Junior extension board
#   define SHA204_DEVICE_COUNT         (1)
#endif

void security_board_init(void);
uint8_t sha204_i2c_address(uint8_t index);

#endif /* SECURITY_JAVAN_H_ */
