/*This file has been prepared for Doxygen automatic documentation generation.*/
//! \file *********************************************************************
//!
//! \brief This file contains the low level macros and definition for stk525 board
//!
//! - Compiler:           IAR EWAVR and GNU GCC for AVR
//! - Supported devices:  AT90USB1287, AT90USB1286, AT90USB647, AT90USB646
//!
//! \author               Atmel Corporation: http://www.atmel.com \n
//!                       Support and FAQ: http://support.atmel.no/
//!
//! ***************************************************************************

/* Copyright (c) 2007, Atmel Corporation All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of ATMEL may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef JAVAN_PLUS_H
#define JAVAN_PLUS_H

//_____ I N C L U D E S ____________________________________________________
#include "..\..\conf\config.h"

#if (TARGET_BOARD==JAVAN_PLUS)

//_____ M A C R O S ________________________________________________________

//! @defgroup JAVAN_PLUS_module JAVAN_PLUS
//! This module contains low level hardware abstraction layer for JAVAN_PLUS board
//! @image html stk525.gif
//! @{


      //! @defgroup JAVAN_PLUS_leds Leds Management
      //! Macros to manage Leds on JAVAN+
      //! @{
#define  LED_PORT             PORTE
#define  LED_DDR              DDRE
#define  LED_PIN              PINE
#define  LED0_BIT             PINE7    // PE7
#define  LED1_BIT             PINE6	   // PE6
#define  LED2_BIT             PINE5	   // PE5

                      
#define  Leds_init()          (LED_DDR  |=  (1<<LED0_BIT) | (1<<LED1_BIT) | (1<<LED2_BIT))
#define  Leds_on()            (LED_PORT |=  (1<<LED0_BIT) | (1<<LED1_BIT) | (1<<LED2_BIT))
#define  Leds_off()           (LED_PORT &= ~((1<<LED0_BIT) | (1<<LED1_BIT) | (1<<LED2_BIT)))

#define  Leds_set_val(c)      (Leds_off(),LED_PORT |= (c<<4)&((1<<LED0_BIT) | (1<<LED1_BIT) | (1<<LED2_BIT)))
#define  Leds_get_val()       (LED_PORT>>4)

#define  Led0_on()            (LED_PORT |=  (1<<LED0_BIT))
#define  Led1_on()            (LED_PORT |=  (1<<LED1_BIT))
#define  Led2_on()            (LED_PORT |=  (1<<LED2_BIT))

#define  Led0_off()           (LED_PORT &= ~(1<<LED0_BIT))
#define  Led1_off()           (LED_PORT &= ~(1<<LED1_BIT))
#define  Led2_off()           (LED_PORT &= ~(1<<LED2_BIT))

#define  Led0_toggle()        (LED_PIN  |=  (1<<LED0_BIT))
#define  Led1_toggle()        (LED_PIN  |=  (1<<LED1_BIT))
#define  Led2_toggle()        (LED_PIN  |=  (1<<LED2_BIT))

#define  Is_led0_on()         (LED_PIN  &   (1<<LED0_BIT) ? TRUE : FALSE)
#define  Is_led1_on()         (LED_PIN  &   (1<<LED1_BIT) ? TRUE : FALSE)
#define  Is_led2_on()         (LED_PIN  &   (1<<LED2_BIT) ? TRUE : FALSE)



      //! @defgroup USB_key_HWB HWB button management
      //! HWB button is connected to PE2 and can also
      //! be used as generic push button
      //! @{
#define  Hwb_button_init()    (DDRE  &= ~(1<<PINE2), PORTE |= (1<<PINE2))
#define  Is_hwb()             ((PINE &   (1<<PINE2)) ?  FALSE : TRUE)
#define  Is_not_hwb()         ((PINE &   (1<<PINE2)) ?  TRUE : FALSE)
      //! @}


//HOST and CLIENT Power on Javan
#define  PWR_PORT             PORTD
#define  PWR_DDR              DDRD
#define  PWR_PIN              PIND

#define  HPWR_BIT             PIND5	   // PE5
#define  CPWR_BIT             PIND6	   // PE6

                      
#define  Pwr_init()          (PWR_DDR  |=  (1<<HPWR_BIT) | (1<<CPWR_BIT))
#define  Pwr_on()            (PWR_PORT |=  (1<<HPWR_BIT) | (1<<CPWR_BIT))
#define  Pwr_off()           (PWR_PORT &= ~((1<<HPWR_BIT) | (1<<CPWR_BIT)))


#define  Hpwr_on()            (PWR_PORT |=  (1<<HPWR_BIT ))
#define  Cpwr_on()            (PWR_PORT |=  (1<<CPWR_BIT))


#define  Hpwr_off()           (PWR_PORT &= ~(1<<HPWR_BIT))
#define  Cpwr_off()           (PWR_PORT &= ~(1<<CPWR_BIT))


#endif   // TARGET_BOARD==JAVAN_PLUS

#endif   // JAVAN_PLUS_H


