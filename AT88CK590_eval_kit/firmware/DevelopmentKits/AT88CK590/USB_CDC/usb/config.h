/*This file has been prepared for Doxygen automatic documentation generation.*/
//! \file *********************************************************************
//!
//! \brief This file contains the system configuration definition.
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

#ifndef _CONFIG_H_
#define _CONFIG_H_

// Compiler switch (do not change these settings)
#include "lib_mcu\compiler.h"             // Compiler definitions
#include "usb_descriptors.h"

#ifdef __GNUC__
   #include <avr/io.h>                    // Use AVR-GCC library
#elif __ICCAVR__
   #define ENABLE_BIT_DEFINITIONS
   #include <ioavr.h>                     // Use IAR-AVR library
#else
   #error Current COMPILER not supported
#endif


//! @defgroup global_config Application configuration
//! @{


// Board defines (do not change these settings)
#define  STK525           1
#define  USBKEY           2
#define  JAVAN_PLUS       3
#define  AT88UBASE        4
#define  STK600           5
#define  AT88RHINO_ECC108 6

//! Enable or not the ADC usage
#undef USE_ADC

//! To include proper target hardware definitions, select
//! target board
#define TARGET_BOARD AT88UBASE

#if (TARGET_BOARD==USBKEY)
   //! @warning for #define USBKEY_HAS_321_DF, only first prototypes versions have AT45DB321C memories
   //! should be undefined for std series
   #define USBKEY_HAS_321_DF
   #include "..\lib_board\usb_key\usb_key.h"
   #define FOSC 8000

#elif (TARGET_BOARD==STK525)
   #include "..\lib_board\stk_525\stk_525.h"
   #define FOSC 8000

#elif (AT88CK427 || AT88CK109STK3 || AT88CK109STK8 || AT88CK101STK3 || AT88CK101STK8 || AT88MICROBASE)
   #define FOSC F_CPU / 1000

#else
   #error TARGET_BOARD must be defined somewhere
#endif

#define FOSC F_CPU / 1000

// -------- END Generic Configuration -------------------------------------


/** \brief packet size definition */
#define EP_LENGTH   1


// UART Sample configuration, if we have one ... __________________________
#define BAUDRATE        57600
#define USE_UART2

#define uart_putchar putchar
#define r_uart_ptchar int
#define p_uart_ptchar int
typedef unsigned char   *puchar;

// ADC Sample configuration, if we have one ... ___________________________

//! ADC Prescaler value
#define ADC_PRESCALER 64
//! Right adjust
#define ADC_RIGHT_ADJUST_RESULT 1
//! AVCC As reference voltage (See adc_drv.h)
#define ADC_INTERNAL_VREF  2

//! @}


/** \brief Unknown command error code definition.
 */
#ifndef UNKNOWN_COMMAND
#define UNKNOWN_COMMAND 0xA2
#endif

#endif // _CONFIG_H_

