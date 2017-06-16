/*This file has been prepared for Doxygen automatic documentation generation.*/
//! \file *********************************************************************
//!
//! \brief Library for the board STK526
//!
//! - Compiler:           IAR EWAVR and GNU GCC for AVR
//! - Supported devices:  AT90USB162, AT90USB82
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

#ifndef AT88CK_440_H
#define AT88CK_440_H

//! @defgroup at88ck_440_module at88ck440
//! This module contains low level hardware abstraction layer for at88ck440 board
//! @image html at88ck_440.gif
//! @{


      //! @defgroup at88ck_440_leds Leds Management
      //! Macros to manage Leds on at88ck440
      //! @{
#define  LED_PORT  PORTD
#define  LED_DDR   DDRD
#define  LED_PIN   PIND

#define  LED_BIT    PIND4

#define  Leds_init()             (LED_DDR  |=  (1<<LED_BIT), Leds_off())
#define  Leds_on()               (LED_PORT |=  (1<<LED_BIT))
#define  Leds_off()              (LED_PORT &= ~(1<<LED_BIT))
#define  Leds_set_val(c)         (Leds_off(),LED_PORT |= (c<<4)&(1<<LED_BIT))
#define  Leds_get_val()          (LED_PORT>>4)
                                

#define  Led_on()                (LED_PORT |=  (1<<LED_BIT))
#define  Led_off()               (LED_PORT &= ~(1<<LED_BIT))
#define  Led2_toggle()           (LED_PIN  |=  (1<<LED_BIT))

#define  Is_led_on()            (LED_PIN  &   (1<<LED_BIT) ? TRUE : FALSE)



      //! @}

      //! @defgroup STK526_HWB HWB button management
      //! HWB button is connected to PD7 and can also
      //! be used as generic push button
      //! @{
#define  Hwb_button_init()       (DDRD &= ~(1<<PIND7))
#define  Is_hwb()                ((PIND & (1<<PIND7)) ? FALSE : TRUE)
#define  Is_not_hwb()            ((PIND & (1<<PIND7)) ? TRUE : FALSE)
      //! @}


//! @}

#endif  // AT88CK_440


