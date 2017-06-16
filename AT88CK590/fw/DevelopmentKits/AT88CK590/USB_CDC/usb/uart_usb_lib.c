/*This file is prepared for Doxygen automatic documentation generation.*/
//! \file *********************************************************************
//!
//! \brief This file controls the UART USB functions.
//!
//! - Compiler:           IAR EWAVR and GNU GCC for AVR
//! - Supported devices:  ATmega32U4
//!
//! \author               Atmel Corporation: http://www.atmel.com \n
//!                       Support and FAQ: http://support.atmel.no/
//!
//! ***************************************************************************

/* Copyright (c) 2009 Atmel Corporation. All rights reserved.
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
 * 3. The name of Atmel may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an Atmel
 * AVR product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*_____ I N C L U D E S ____________________________________________________*/

#include "config.h"
#include "lib_mcu/usb/usb_drv.h"
#include "lib_mcu/compiler.h"
#include "usb_descriptors.h"

#include "uart_usb_lib.h"

/*_____ M A C R O S ________________________________________________________*/

/*_____ D E F I N I T I O N ________________________________________________*/


Uchar rx_counter;

/*_____ D E C L A R A T I O N ______________________________________________*/

/**
  * @brief Initializes the uart_usb library
  */
void uart_usb_init(void)
{
  rx_counter = 0;
}

/**
  * @brief This function checks if a character has been received on the USB bus.
  *
  * @return bit (true if a byte is ready to be read)
  */
bit uart_usb_test_hit(void)
{
  if(!Is_device_enumerated())
     return FALSE;

  if (!rx_counter)
  {
    Usb_select_endpoint(RX_EP);
    if (Is_usb_receive_out())
    {
      rx_counter = Usb_byte_counter();
      if (!rx_counter)
      {
        Usb_ack_receive_out();
      }
    }
  }
  return (rx_counter != 0);
}

/**
  * @brief This function reads one byte from the USB bus
  *
  * If one byte is present in the USB fifo, this byte is returned. If no data
  * is present in the USB fifo, this function waits for USB data.
  *
  * @return U8 byte received
  */
char uart_usb_getchar(void)
{
  register Uchar data_rx;

  Usb_select_endpoint(RX_EP);
  if (!rx_counter) while (!uart_usb_test_hit());
  data_rx = Usb_read_byte();
  rx_counter--;
  if (!rx_counter) Usb_ack_receive_out();
  return data_rx;
}


/**
  * @brief This function checks if the USB emission buffer is ready to accept at
  * at least 1 byte
  *
  * @return Boolean. TRUE if the firmware can write a new byte to transmit.
  */
bit uart_usb_tx_ready(void)
{
  if(!Is_device_enumerated())
     return FALSE;

  if (!Is_usb_write_enabled())
  {
    return FALSE;
  }
  return TRUE;
}

/**
  * @brief This function fills the USB transmit buffer with the new data. This buffer
  * is sent if complete. To flush this buffer before waiting full, launch
  * the uart_usb_flush() function.
  *
  * @param data_to_send
  *
  * @return
  */
int uart_usb_putchar(int data_to_send)
{
   uart_usb_send_buffer((U8*)&data_to_send, 1);
   return data_to_send;
}



/**
  * @brief This function transmits a ram buffer content to the USB.
  * This function is mode efficient in term of USB bandwith transfer.
  *
  * @param U8 *buffer : the pointer to the RAM buffer to be sent
  * @param data_to_send : the number of data to be sent
  */
void uart_usb_send_buffer(U8 *buffer, U8 nb_data)
{
   U8 zlp;

  if(!Is_device_enumerated())
     return;

   // Compute if zlp required
   if(nb_data%TX_EP_SIZE)
   { zlp=FALSE;}
   else { zlp=TRUE; }

   Usb_select_endpoint(TX_EP);
   while (nb_data)
   {
      while(Is_usb_write_enabled()==FALSE); // Wait Endpoint ready
      while(Is_usb_write_enabled() && nb_data)
      {
         Usb_write_byte(*buffer);
         buffer++;
         nb_data--;
   }
      Usb_ack_in_ready();
   }
   if(zlp)
   {
      while(Is_usb_write_enabled()==FALSE); // Wait Endpoint ready
      Usb_ack_in_ready();
}
}
