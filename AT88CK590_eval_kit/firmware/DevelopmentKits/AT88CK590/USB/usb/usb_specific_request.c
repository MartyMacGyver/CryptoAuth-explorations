/*This file has been prepared for Doxygen automatic documentation generation.*/
//! \file *********************************************************************
//!
//! \brief user call-back functions
//!
//!  This file contains the user call-back functions corresponding to the
//!  application:
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


//_____ I N C L U D E S ____________________________________________________

#include "..\conf\config.h"
#include "..\conf\conf_usb.h"
#include "..\lib_mcu\usb\usb_drv.h"
#include "usb_descriptors.h"
#include "..\modules\usb\device_chap9\usb_standard_request.h"
#include "usb_specific_request.h"
#if ((USB_DEVICE_SN_USE==ENABLE) && (USE_DEVICE_SN_UNIQUE==ENABLE))
#include "lib_mcu/flash/flash_drv.h"
#endif

//_____ D E F I N I T I O N ________________________________________________

#ifdef __GNUC__
extern PGM_VOID_P pbuffer;
#else
extern U8   const code *pbuffer;
#endif
extern U8   data_to_transfer;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuninitialized"
extern const code S_usb_hid_report_descriptor usb_hid_report_descriptor;
#pragma GCC diagnostic pop

U8 jump_bootloader=0;

U8 g_u8_report_rate=0;

//_____ D E C L A R A T I O N ______________________________________________

void hid_get_report_descriptor(void);
void usb_hid_set_report_ouput(void);
void usb_hid_set_idle (U8 u8_report_id, U8 u8_duration );
void usb_hid_get_idle (U8 u8_report_id);
void hid_get_hid_descriptor(void);
void usb_hid_set_report_feature(void);


//! @breif This function checks the specific request and if known then processes it
//!
//! @param type      corresponding at bmRequestType (see USB specification)
//! @param request   corresponding at bRequest (see USB specification)
//!
//! @return TRUE,  when the request is processed
//! @return FALSE, if the request is'nt know (STALL handshake is managed by the main standard request function).
//!
Bool usb_user_read_request(U8 type, U8 request)
{
   U8    wValue_msb;
   U8    wValue_lsb;

   // Read wValue
   wValue_lsb = Usb_read_byte();
   wValue_msb = Usb_read_byte();

   //** Specific request from Class HID
   if( USB_SETUP_GET_STAND_INTERFACE == type )
   {
      switch( request )
      {
         case SETUP_GET_DESCRIPTOR:
         switch( wValue_msb ) // Descriptor ID
         {
            case DESCRIPTOR_HID:
            hid_get_hid_descriptor();
            return TRUE;
            break;

            case DESCRIPTOR_REPORT:
            hid_get_report_descriptor();
            return TRUE;

            case DESCRIPTOR_PHYSICAL:
            // TODO
            break;
         }
         break;
      }
   }
   if( USB_SETUP_SET_CLASS_INTER == type )
   {
      switch( request )
      {
         case SETUP_HID_SET_REPORT:
         // The MSB wValue field specifies the Report Type
         // The LSB wValue field specifies the Report ID
         switch (wValue_msb)
         {
            case REPORT_TYPE_INPUT:
            // TODO
            break;

            case REPORT_TYPE_OUTPUT:
            usb_hid_set_report_ouput();
            return TRUE;
            break;

            case REPORT_TYPE_FEATURE:
            usb_hid_set_report_feature();
            return TRUE;
            break;
         }
         break;

         case SETUP_HID_SET_IDLE:
         usb_hid_set_idle(wValue_lsb,wValue_msb);
         return TRUE;

         case SETUP_HID_SET_PROTOCOL:
         // TODO
         break;
      }
   }
   if( USB_SETUP_GET_CLASS_INTER == type )
   {
      switch( request )
      {
         case SETUP_HID_GET_REPORT:
         // TODO
         break;
         case SETUP_HID_GET_IDLE:
         usb_hid_get_idle(wValue_lsb);
         return TRUE;
         case SETUP_HID_GET_PROTOCOL:
         // TODO
         break;
      }
   }
   return FALSE;  // No supported request
}


//! @brief This function configures the end points
//!
//! @param conf_nb configuration number chosen by USB host
//!
void usb_user_endpoint_init(U8 conf_nb)
{
   usb_configure_endpoint( EP_HID_IN,     \
                           TYPE_INTERRUPT,\
                           DIRECTION_IN,  \
                           SIZE_64,       \
                           ONE_BANK,      \
                           NYET_ENABLED);

   usb_configure_endpoint( EP_HID_OUT,    \
                           TYPE_INTERRUPT,\
                           DIRECTION_OUT, \
                           SIZE_64,       \
                           ONE_BANK,      \
                           NYET_ENABLED);
}
/*
The SIZE_n can have the following values:
SIZE_8: 8 bytes
SIZE_16: 16 bytes
SIZE_32: 32
SIZE_64: 64 bytes
SIZE_128: 128 bytes
SIZE_256: 256 bytes
SIZE_512: 512 bytes
SIZE_1024: 1024 bytes
*/

//! @brief This function returns the interface alternate setting
//!
//! @param wInterface         Interface selected
//!
//! @return alternate setting configurated
//!
U8   usb_user_interface_get( U16 wInterface )
{
   return 0;  // Only one alternate setting possible for all interface
}


//! @brief This function selects (and resets) the interface alternate setting
//!
//! @param wInterface         Interface selected
//! @param alternate_setting  alternate setting selected
//!
void usb_user_interface_reset(U16 wInterface, U8 alternate_setting)
{
   // default setting selected = reset data toggle
   if( INTERFACE_NB == wInterface )
   {
      // Interface HID
      Usb_select_endpoint(EP_HID_IN);
      Usb_disable_stall_handshake();
      Usb_reset_endpoint(EP_HID_IN);
      Usb_reset_data_toggle();
      Usb_select_endpoint(EP_HID_OUT);
      Usb_disable_stall_handshake();
      Usb_reset_endpoint(EP_HID_OUT);
      Usb_reset_data_toggle();
   }
}


//! This function fills the global descriptor
//!
//! @param type      corresponding at MSB of wValue (see USB specification)
//! @param string    corresponding at LSB of wValue (see USB specification)
//!
//! @return FALSE, if the global descriptor no filled
//!
Bool usb_user_get_descriptor(U8 type, U8 string)
{
   switch(type)
   {
      case DESCRIPTOR_STRING:
      switch (string)
      {
         case LANG_ID:
         data_to_transfer = sizeof (usb_user_language_id);
         pbuffer = &(usb_user_language_id.bLength);
         return TRUE;
         break;

         case MAN_INDEX:
         data_to_transfer = sizeof (usb_user_manufacturer_string_descriptor);
         pbuffer = &(usb_user_manufacturer_string_descriptor.bLength);
         return TRUE;
         break;

         case PROD_INDEX:
         data_to_transfer = sizeof (usb_user_product_string_descriptor);
         pbuffer = &(usb_user_product_string_descriptor.bLength);
         return TRUE;
         break;

#if (USB_DEVICE_SN_USE==ENABLE)
         case SN_INDEX:
         data_to_transfer = sizeof (usb_user_serial_number);
         pbuffer = &(usb_user_serial_number.bLength);
#if (USE_DEVICE_SN_UNIQUE==ENABLE)
         f_get_serial_string=TRUE;
         data_to_transfer += (SN_LENGTH*4);
#endif
         return TRUE;
         break;
#endif
      }
      break;
   }
   return FALSE;
}


//! @brief This function manages hit get repport request.
//!
void hid_get_report_descriptor(void)
{
   U16 wLength;
   U8  nb_byte;
   bit zlp = FALSE;
   U16 wInterface;

   LSB(wInterface)=Usb_read_byte();
   MSB(wInterface)=Usb_read_byte();

   data_to_transfer = sizeof(usb_hid_report_descriptor);
   pbuffer = &(usb_hid_report_descriptor.report[0]);

   LSB(wLength) = Usb_read_byte();
   MSB(wLength) = Usb_read_byte();
   Usb_ack_receive_setup();

   if (wLength > data_to_transfer)
   {
      if ((data_to_transfer % EP_CONTROL_LENGTH) == 0) { zlp = TRUE; }
      else { zlp = FALSE; }
   }
   else
   {
      data_to_transfer = (U8)wLength;           // send only requested number of data
   }

   while((data_to_transfer != 0) && (!Is_usb_receive_out()))
   {
      while(!Is_usb_read_control_enabled());

      nb_byte=0;
      while(data_to_transfer != 0)              // Send data until necessary
      {
         if(nb_byte++==EP_CONTROL_LENGTH)       // Check endpoint 0 size
         {
            break;
         }
#ifndef __GNUC__
         Usb_write_byte(*pbuffer++);
#else    // AVRGCC does not support point to PGM space
//warning with AVRGCC assumes devices descriptors are stored in the lower 64Kbytes of on-chip flash memory
         Usb_write_byte(pgm_read_byte_near((unsigned int)pbuffer++));
#endif
         data_to_transfer --;
      }
      Usb_send_control_in();
   }

   if(Is_usb_receive_out())
   {
      // abort from Host
      Usb_ack_receive_out();
      return;
   }
   if(zlp == TRUE)
   {
      while(!Is_usb_read_control_enabled());
      Usb_send_control_in();
   }

   while(!Is_usb_receive_out());
   Usb_ack_receive_out();
}


//! @brief This function manages hit set report request.
//!
void usb_hid_set_report_ouput (void)
{
   Usb_ack_receive_setup();
   Usb_send_control_in();

   while(!Is_usb_receive_out());
   Usb_ack_receive_out();
   Usb_send_control_in();
}


//! @brief This function manages hid set idle request.
//!
//! @param Duration     When the upper byte of wValue is 0 (zero), the duration is indefinite else from 0.004 to 1.020 seconds
//! @param Report ID    0 the idle rate applies to all input reports, else only applies to the Report ID
//!
void usb_hid_set_idle (U8 u8_report_id, U8 u8_duration )
{
   U16 wInterface;

   // Get interface number to put in idle mode
   LSB(wInterface)=Usb_read_byte();
   MSB(wInterface)=Usb_read_byte();
   Usb_ack_receive_setup();

   g_u8_report_rate = u8_duration;

   Usb_send_control_in();
   while(!Is_usb_in_ready());
}


//! @brief This function manages hid get idle request.
//!
//! @param Report ID    0 the idle rate applies to all input reports, else only applies to the Report ID
//!
void usb_hid_get_idle (U8 u8_report_id)
{
   U16 wLength;
   U16 wInterface;

   // Get interface number to put in idle mode
   LSB(wInterface)= Usb_read_byte();
   MSB(wInterface)= Usb_read_byte();
   LSB(wLength)   = Usb_read_byte();
   MSB(wLength)   = Usb_read_byte();
   Usb_ack_receive_setup();

   if( wLength != 0 )
   {
      Usb_write_byte(g_u8_report_rate);
      Usb_send_control_in();
   }

   while(!Is_usb_receive_out());
   Usb_ack_receive_out();
}

void usb_hid_set_report_feature(void)
{

   Usb_ack_receive_setup();
   Usb_send_control_in();

   while(!Is_usb_receive_out());

   if(Usb_read_byte()==0x55)
      if(Usb_read_byte()==0xAA)
         if(Usb_read_byte()==0x55)
            if(Usb_read_byte()==0xAA)
            {
               jump_bootloader=1;
            }
   Usb_ack_receive_out();
   Usb_send_control_in();
   while(!Is_usb_in_ready());
}

//! @brief This function manages hid get hid descriptor request.
//!
void hid_get_hid_descriptor(void)
{
   U16 wLength;
   U8  nb_byte;
   bit zlp=FALSE;
   U16 wInterface;

   LSB(wInterface)=Usb_read_byte();
   MSB(wInterface)=Usb_read_byte();

   data_to_transfer = sizeof(usb_conf_desc.hid);
   pbuffer = &(usb_conf_desc.hid.bLength);

   LSB(wLength) = Usb_read_byte();
   MSB(wLength) = Usb_read_byte();
   Usb_ack_receive_setup();

   if (wLength > data_to_transfer)
   {
      if ((data_to_transfer % EP_CONTROL_LENGTH) == 0) { zlp = TRUE; }
      else { zlp = FALSE; }                     // no need of zero length packet
   }
   else
   {
      data_to_transfer = (U8)wLength;           // send only requested number of data
   }

   while((data_to_transfer != 0) && (!Is_usb_receive_out()))
   {
      while(!Is_usb_read_control_enabled());

      nb_byte=0;
      while(data_to_transfer != 0)              // Send data until necessary
      {
         if(nb_byte++==EP_CONTROL_LENGTH)       // Check endpoint 0 size
         {
            break;
         }
#ifndef __GNUC__
         Usb_write_byte(*pbuffer++);
#else    // AVRGCC does not support point to PGM space
//warning with AVRGCC assumes devices descriptors are stored in the lower 64Kbytes of on-chip flash memory
         Usb_write_byte(pgm_read_byte_near((unsigned int)pbuffer++));
#endif
         data_to_transfer --;
      }
      Usb_send_control_in();
   }

   if(Is_usb_receive_out())
   {
      // abort from Host
      Usb_ack_receive_out();
      return;
   }
   if(zlp == TRUE)
   {
      while(!Is_usb_read_control_enabled());
      Usb_send_control_in();
   }

   while(!Is_usb_receive_out());
   Usb_ack_receive_out();
}

