/*
 * \file
 *
 * \brief ATSHA204 example module
 *
 * This file contains the main function.
 *
 *
 * Copyright (c) 2011-2013 Atmel Corporation. All rights reserved.
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
 *   CAUTION WHEN DEBUGGING: Be aware of the timeout feature of the device. The
 *   device will go to sleep between 0.7 and 1.7 seconds after a Wakeup. When
 *   hitting a break point this timeout will kick in, and the device
 *   has gone to sleep by the time you continue debugging. Therefore, after you have
 *   examined variables you might have to restart your debug session to make 
 *   an example succeed.
 
 Some of the ATECC108 examples are derived from ATSHA204 examples. Therefore, you
 will find references to SHA204 library definitions.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <asf.h>                                 // definitions and declarations for the Atmel Software Framework
#include "ecc108_examples.h"                     // definitions and declarations for example functions
#include "sha204_example_led.h"                  // definitions of LED utility functions
#include "usart.h"
#include "conf_usart_cdc_xplained.h"
#include "vt100.h"
#include "COMPortTest.h"    

extern int stdio_putchar(char c, FILE *stream);
extern int stdio_getchar(FILE *stream);

// Open stdio stream/file
FILE stdio_str = FDEV_SETUP_STREAM(stdio_putchar, stdio_getchar, _FDEV_SETUP_RW);

void InitAll(void) {
      // Init stdio
      stdout = stdin = &stdio_str;

      // USART options.
      usart_rs232_options_t USART_SERIAL_OPTIONS = {
	      .baudrate = USART_CDC_XPLAINED_BAUDRATE,
	      .charlength = USART_CDC_XPLAINED_CHAR_LENGTH,
	      .paritytype = USART_CDC_XPLAINED_PARITY,
	      .stopbits = USART_CDC_XPLAINED_STOP_BIT
      };
      
      // Initialize system clock.
      sysclk_init();
      // Initialize the board.
      board_init();

      // Init USARTC0 <-> CDC AVR32-UC3 on board
      usart_init_rs232(USART_CDC_XPLAINED, &USART_SERIAL_OPTIONS);
 
      // Init USART for SHA204 Exercises
      usart_init_rs232(USART_SHA_EXERSICES, &USART_SERIAL_OPTIONS);
	       
      // Indicate end of hardware initialization.
//      led_display_number(0xFF);
      sha204h_delay_ms(300);
//      led_display_number(0x00);
      
      // Initial Debug message
      vt100Init();
      vt100ClearScreen();
      printf("\n\r");
      printf("-- Crypto ATECC108 Examples --\n\r");
      printf("-- Compiled: %s %s --\n\n\r", __DATE__, __TIME__);	
	
}


int main(void)
{

// Init All hardware 
	InitAll();
	
// Examples

	lock();

//  *************************************************************
//  **  Workshop:1 
//  **         This example verifies that the COM Port is working.
//  *************************************************************

//	comport_test();


//  *************************************************************
//  **  Workshop:2 
//  **         This example uses the ECC108 to generate a key pair.
//  **         Sign the key with a root key and then store the public
//  **         key and signature back to the device.
//  *************************************************************
   
	personalize();

//  *************************************************************
//  **  Workshop:3
//  **         This example uses the ECC108 to verify the signature
//  **         was generated by the root key.
//  *************************************************************

    Auth();


//  *************************************************************
//  **  Workshop:4
//  **         This example demonstrates the Firmware Protection.
//  *************************************************************
    
// 	 firmware_protection();

   while(1) {
	  asm volatile ("NOP");
   } 	
 
}
