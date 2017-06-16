/*
 * \file
 *
 * \brief XMEGA-A1 Xplained timer utility functions.
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
#ifndef BITBANG_CONFIG_H
#   define BITBANG_CONFIG_H


#include "gpio.h"                  // GPIO definitions
#include "conf_atsha204.h"

#define swi_enable_interrupts    Enable_global_interrupt  //!< enable interrupts
#define swi_disable_interrupts   Disable_global_interrupt //!< disable interrupts

#define PIN_SWI_CLIENT           IOPORT_CREATE_PIN(PORTD, 3)
#define CLIENT_ID                (0)        //!< identifier for client
//#define HOST_ID                  (1)        //!< identifier for host

// debug pin that indicates pulse edge detection. Only enabled if compilation switch _DEBUG is used.
// To debug timing, disable host power (H1 and H2 on CryptoAuth daughter board) and connect logic analyzer
// or storage oscilloscope to the H2 pin that is closer to the H1 header.
// todo Define debug pin for Xmega-A1-Xplained.
#define DEBUG_BITBANG
#ifdef DEBUG_BITBANG
#   define DEBUG_PIN_SWI  IOPORT_CREATE_PIN(PORTD, 5)        //!< debug pin (J3, pin 6)
#   define DEBUG_LOW      gpio_set_pin_low(DEBUG_PIN_SWI)    //!< set debug pin low
#   define DEBUG_HIGH     gpio_set_pin_high(DEBUG_PIN_SWI)   //!< set debug pin high
#else
#   define DEBUG_LOW
#   define DEBUG_HIGH
#endif

// delay loops to send bits at 230.4 kbps
//! time it takes to toggle the pin at CPU clock of 32 MHz (ns) with optimization -O1
#define BIT_DELAY_1              delay = 12; while (delay--); asm volatile ("\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n"::) // should be 4.34 us, is 4.33 us
//! time to keep pin high for five pulses plus stop bit (used to bit-bang CryptoAuth 'zero' bit)
#define BIT_DELAY_6              delay = 82; while (delay--) // should be 26.0 us, is 25.9 us
//! time to keep pin high for seven bits plus stop bit (used to bit-bang CryptoAuth 'one' bit)
#define BIT_DELAY_8              delay = 111; while (delay--) // should be 34.7 us, is 34.8 us
//! turn around time when switching from receive to transmit
#define RX_TX_DELAY              delay = 16; while (delay--)

// Lets set the timeout value for start pulse detection to the uint8_t maximum.
/** \brief This value is decremented while waiting for the falling edge of a start pulse. */
#define START_PULSE_TIME_OUT     (255)

// Maximum time between rising edge of start pulse and falling edge of a zero pulse is 8.6 us. 
// A value of 50 (around 11 us) gives ample time to detect a zero pulse and also leaves enough 
// time to detect the following start pulse.
/** \brief This value is decremented while waiting for the falling edge of a zero pulse. */
#define ZERO_PULSE_TIME_OUT       (50)

#endif
