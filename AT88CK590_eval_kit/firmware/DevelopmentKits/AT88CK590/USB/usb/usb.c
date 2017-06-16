/*This file has been prepared for Doxygen automatic documentation generation.*/
//! \file *********************************************************************
//!
//! \brief This file manages the generic HID IN/OUT task.
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

//_____  USB I N C L U D E S ___________________________________________________





#include "conf\config.h"
#include "conf\conf_usb.h"
#include "usb.h"
#include "lib_mcu\usb\usb_drv.h"
#include "usb_descriptors.h"
#include "modules\usb\device_chap9\usb_standard_request.h"
#include "usb_specific_request.h"
#include "lib_mcu\util\start_boot.h"
#include "delay_x.h"
#include "parser.h"
#include "utility.h"
#include "hardware.h"

//_____  PROJECT I N C L U D E S ________________________________________________

#include <avr/interrupt.h>
#include <setjmp.h>
#include <avr/pgmspace.h>
#include <avr/io.h>

//#include "delay_x.h"

// SA Library Headers
#include "SA_API.h"
//#include "sha256.h"



//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________

#define SOP 0x73
#define EOP 0x70

//_____ U S B  D E C L A R A T I O N S ________________________________________

volatile U8             cpt_sof = 0;
extern   U8             jump_bootloader;
         U8             g_last_joy = 0;
         U8             BUTTON_PRESSED;

         U8 SendResults;
         U8 ucLocalReceiveDataBuffer[16];
         S8 retCode;

         void check_button   (void);

         //_____ L I B R A R Y  D E C L A R A T I O N S ________________________________________

         SA_snBuf       snData;
         SA_statusBuf   statusData;
         SA_mfrID_buf   mfrIDdata;
         SA_revNumBuf   revNumData;
         SA_readBuf     readBuf;

         SA_snBuf       snDataHost;
         SA_statusBuf   statusDataHost;
         SA_mfrID_buf   mfrIDdataHost;
         SA_revNumBuf   revNumDataHost;


//! @brief This function initializes the target board resources.
void usb_init(void)
{
    /*
	 * Initialize LEDs, etc.
	 */

	Led_Init();

    BUTTON_PRESSED = 0;
 

	/*
	 * initialize various buffers
	 */
	memset(snData.bytes, 0, sizeof(snData.bytes));
	memset(statusData.bytes, 0, sizeof(statusData.bytes));
	memset(revNumData.bytes, 0, sizeof(revNumData.bytes));
	memset(mfrIDdata.bytes, 0, sizeof(mfrIDdata.bytes));
}


//! @brief Entry point of the HID generic communication task
//! This function manages IN/OUT reports.
void usb(void)
{
   if (!Is_device_enumerated())          // Check USB HID is enumerated
      return;

   Usb_receive_data_check();

   if (ucParseDataStart == TRUE)
   {
     
	  #ifndef AT88CK427
	   Led1(1); // on while request is being processed
      #endif

      U8 ucReturn = command_parse(ucUsbReceiveDataBuffer + 1, \
                                  ucUsbReceiveDataBuffer[0], \
                                  ucUsbSendDataBuffer, &ucSendDataSize);

      if ((S8) ucReturn == SA_SUCCESS) {

	  #ifndef AT88CK427
         Led2(1); // reset error indicator
      #endif          

         	#ifdef USB_BYTE_DATA
				// FOR BYTE USE
				Usb_byte_data_send(ucSendDataSize, ucUsbSendDataBuffer);
			#else
            	// FOR ASCII USE
            	Usb_data_convert_and_send(ucSendDataSize, ucUsbSendDataBuffer);
			#endif


		}
      else {

	  #ifndef AT88CK427
         Led2(0); // set error indicator
      #endif  
			#ifdef USB_BYTE_DATA
				// FOR BYTE USE
			   Usb_byte_data_send(ucSendDataSize, ucUsbSendDataBuffer);
			#else
	          // FOR ASCII USE
 				Usb_data_convert_and_send(1, &ucReturn);
			#endif
      }
      ucParseDataStart = FALSE;

	  #ifndef AT88CK427
       Led1(0); // done with this request
      #endif
   }

   if (TARGET_BOARD == JAVAN_PLUS)
      check_button();
 
 #ifndef AT88CK427
   show_heart_beat();   // led 0
 #endif

}


/** \brief Check for incoming data
 */
void Usb_receive_data_check ( void )
{
   if (Is_device_enumerated())            //! Check USB HID is enumerated
   {
      Usb_select_endpoint(EP_HID_OUT);    //! Get Data report from Host
      if (Is_usb_receive_out())
		   Usb_receive_data_take();

		// Check if we received DFU mode command from host
		if (jump_bootloader)
		{
		   U8 i;

			for (i = 0; i < 3; i++)
			{
			   Led_On();
		      _delay_ms(200);

			   Led_Off();
			   _delay_ms(200);
   		}

			Usb_detach();                             // Detach actual generic HID application
         /** \todo Replace wait loop with call to _delay_ms(?). */
			U32 volatile tempo;
			for (tempo = 0; tempo < 70000; tempo++);  // Wait some time before
			start_boot();                             // Jumping to boot loader
		}
   }
}


/** \brief Take data from USB
 */
void Usb_receive_data_take ( void )
{

   // Take EP_IN_LENGTH bytes of data, store them in local buffer.

	static U8 i;
	static volatile U8 ucUsbReceiveDataIndex;
    static volatile U8 ucUsbDataParseState = 0;
	static U8 ucLocalReceiveDataBuffer[EP_IN_LENGTH];
    U8 packetLength = 0;

	for (i = 0; i < EP_IN_LENGTH; i++)
      ucLocalReceiveDataBuffer[i] = Usb_read_byte (); // Fill report

	Usb_ack_receive_out();

   for (i = 0; i < EP_IN_LENGTH; i++)
   {
      if ((ucLocalReceiveDataBuffer[i] == SOP) && (i == 0))
         ucUsbDataParseState = TAKE_DATA_LENGTH;
     

      else
      {
         switch (ucUsbDataParseState)
         {
            case TAKE_DATA_LENGTH:
               packetLength = ucUsbReceiveDataBuffer[0] = ucLocalReceiveDataBuffer[i];
               packetLength++;
               ucUsbReceiveDataIndex = 0;
               ucUsbDataParseState = TAKE_DATA;
               break;

            case TAKE_DATA:
				#ifdef USB_BYTE_DATA // FOR BYTE USE

	               ucUsbReceiveDataBuffer[++ucUsbReceiveDataIndex] = ucLocalReceiveDataBuffer[i];
	               if (ucUsbReceiveDataIndex == packetLength)
	                  // Received entire packet including EOP.
	                  ucUsbDataParseState = END_OF_VALID_DATA;
				#else
	                if (ucLocalReceiveDataBuffer[i] != EOP)
	                {

	                    ucUsbReceiveDataIndex++;
	                    ucUsbReceiveDataBuffer[ucUsbReceiveDataIndex] = \
	                                            ucLocalReceiveDataBuffer[i];
	                }
	                else
	                {
	                    ucUsbDataParseState = END_OF_VALID_DATA;
	                    break;
	                }
				#endif
               break;

            case END_OF_VALID_DATA:
		
               if (i == (EP_IN_LENGTH - 1))
               {

				      ucUsbDataParseState = END_OF_TAKE_DATA;
                    

				  #ifdef USB_BYTE_DATA // FOR BYTE USE

				      if (ucUsbReceiveDataBuffer[ucUsbReceiveDataIndex] != EOP)
					   {
		                     ucUsbReceiveDataIndex = 0;
		                     ucUsbDataParseState = BLANK;
		                }

					#else
	                  if (ucUsbReceiveDataBuffer[0] != (ucUsbReceiveDataIndex / 2))
	                  {
	                     ucUsbReceiveDataIndex = 0;
	                     ucUsbDataParseState = BLANK;
	                  }
	      		   #endif
               }
               break;

            default:
               break;
         } //End switch()
      } //End if-statement
   }


   if (ucUsbDataParseState == END_OF_TAKE_DATA)
   {
      ucUsbReceiveDataIndex = 0;
      ucUsbDataParseState = BLANK;

		#ifndef USB_BYTE_DATA
         // FOR ASCII USE ONLY
         String_to_hexa_convert ( ucUsbReceiveDataBuffer[0], ucUsbReceiveDataBuffer + 1 );
		#endif
      ucParseDataStart = TRUE;
   }


}


/** \brief Convert data from String to Hexa
 */

void String_to_hexa_convert ( U8 ucDataLength, puchar pucDataBuffer )
{
    U8 i, ucDataBuffer[ucDataLength * 2];

    for (i = 0; i < ucDataLength * 2; i++)
    {
        ucDataBuffer[i] = pucDataBuffer[i];
    }

    for (i = 0; i < ucDataLength; i++)
    {
        ucDataBuffer[2 * i] = Ascii_to_nible ( ucDataBuffer[2 * i] );
        ucDataBuffer[(2 * i) + 1] = Ascii_to_nible ( ucDataBuffer[(2 * i) + 1] );
        pucDataBuffer[i] = ((ucDataBuffer[2 * i]) << 4) + ucDataBuffer[(2 * i) + 1];
    }
}

/** \brief Convert data from Hexa to String
 */


void Hexa_to_string_convert ( U8 ucDataLength, puchar pucDataBuffer )
{
    U8 i, ucDataBuffer[ucDataLength];
    for (i = 0; i < ucDataLength; i++)
    {
        ucDataBuffer[i] = pucDataBuffer[i];
    }

    for (i = 0; i < ucDataLength; i++)
    {
        pucDataBuffer[2 * i] = Nible_to_ascii ( ucDataBuffer[i] >> 4 );
        pucDataBuffer[(2 * i) + 1] = Nible_to_ascii ( ucDataBuffer[i] );
    }
}

/** \brief Send data to USB
 */

void Usb_data_send ( U8 ucDataLength, puchar pucDataBuffer )
{
   static U8 i, j, ucIndex1, ucIndex2;
   static U16 SIZE;

   ucIndex1 = (2 * ucDataLength + 3) / EP_OUT_LENGTH;
   ucIndex2 = (2 * ucDataLength + 3) % EP_OUT_LENGTH;

   if (ucIndex1 == 0)
      SIZE = 1;
	else {
	   if (ucIndex2 == 0)
		   SIZE = ucIndex1;
		else
		   SIZE = ucIndex1 + 1;
	}

	//U8 ucDataBuffer[SIZE*EP_OUT_LENGTH];
    static U8 ucDataBuffer[256];

	memset(ucDataBuffer,0,sizeof(ucDataBuffer));

   ucDataBuffer[0] = SOP;
   ucDataBuffer[1] = ucDataLength;

   for (i = 0; i < 2 * ucDataLength; i++)
      ucDataBuffer[i + 2] = pucDataBuffer[i];

   ucDataBuffer[(2 * ucDataLength) + 2] = EOP;

   Usb_select_endpoint ( EP_HID_IN );

	//! Ready to send information to host application
   if (Is_usb_write_enabled ())
   {
	   for (i = 0 ; i < SIZE; i++)
	   {
         for (j = 0; j < EP_OUT_LENGTH; j++)
         {
            Usb_write_byte ( ucDataBuffer[(i * EP_OUT_LENGTH) + j] );
         }
         Usb_ack_in_ready();              //! Send data over the USB
         _delay_ms ( 20 );
      }
   }
}


/** \brief Send data (BYTES) to USB
 */
void Usb_byte_data_send ( U8 ucDataLength, puchar pucDataBuffer )
{
   U8 i, j, ucIndex1, ucIndex2;
   U16 SIZE;

   // Used to send raw bytes
   ucIndex1 = (ucDataLength + 3) / EP_OUT_LENGTH;
   ucIndex2 = (ucDataLength + 3) % EP_OUT_LENGTH;

   if (ucIndex1 == 0)
      SIZE = 1;
	else {
      if (ucIndex2 == 0)
         SIZE = ucIndex1;
      else
         SIZE = ucIndex1 + 1;;
	}

	U8 ucDataBuffer[SIZE*EP_OUT_LENGTH];

	memset(ucDataBuffer,0,sizeof(ucDataBuffer));

   ucDataBuffer[0] = SOP;
   ucDataBuffer[1] = ucDataLength;

   for (i = 0; i < ucDataLength; i++)
      ucDataBuffer[i + 2] = pucDataBuffer[i];

   ucDataBuffer[(ucDataLength) + 2] = EOP;

   Usb_select_endpoint ( EP_HID_IN );

	//! Ready to send information to host application
   if (Is_usb_write_enabled ())
   {
	   for (i = 0 ; i < SIZE; i++)
	   {
         for (j = 0; j < EP_OUT_LENGTH; j++)
            Usb_write_byte(ucDataBuffer[(i * EP_OUT_LENGTH) + j]);
         Usb_ack_in_ready();              //! Send data over the USB
         _delay_ms(20);
      }
   }
}


/** \brief Function to convert Hexa data to String and send to USB
 */
void Usb_data_convert_and_send ( U8 ucDataLength, puchar pucDataBuffer )
{
    Hexa_to_string_convert ( ucDataLength, pucDataBuffer );
    Usb_data_send ( ucDataLength, pucDataBuffer );
}


//! @brief  This function increments the cpt_sof counter each times
//! the USB Start Of Frame interrupt subroutine is executed (1ms)
//! Useful to manage time delays
//!
void sof_action()
{
   cpt_sof++;
}


// Checking Button
void check_button (void)
{
	if (bit_is_clear(PINB, 4)) {
	   Led1(1);
	   BUTTON_PRESSED = 1;
	}
	else if (BUTTON_PRESSED == 1) {
	   Led1(0);
	   BUTTON_PRESSED = 0;
	}
}


void heart_beat_init()
{
   Led_Init();
}


void show_heart_beat()
{
   static uint8_t heartBeatCount = 0;
   static uint8_t isLedOn = 0;

   //while (1) {
      if (!heartBeatCount++) {
         if (isLedOn)
            Led1(0);
         else
            Led1(1);
         isLedOn = !isLedOn;
      }
      _delay_ms(1);
   //}
}




