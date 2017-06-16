/*This file has been prepared for Doxygen automatic documentation generation.*/
//! \file *********************************************************************
//!
//! \brief This file contains the function declarations
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

#ifndef _USB_H_
#define _USB_H_

#include "conf\config.h"

//! USB send and receive data buffer size.
#define USB_RECEIVE_MAX_BUFFER 128

#define BLANK              0
#define TAKE_DATA_LENGTH   1
#define TAKE_DATA          2
#define END_OF_VALID_DATA  3
#define END_OF_TAKE_DATA   4

//! USB receive data buffer.
U8 ucUsbReceiveDataBuffer[USB_RECEIVE_MAX_BUFFER];

//! USB receive data buffer.
U8 ucUsbSendDataBuffer[USB_RECEIVE_MAX_BUFFER];
U8 ucSendDataSize;
U8 ucParseDataStart;

void Usb_initialize ( void );
void Usb_variable_initialize ( void );
void Usb_state ( void );
void Usb_receive_data_check ( void );
void Usb_receive_data_take ( void );
void String_to_hexa_convert ( U8 ucDataLength, puchar pucDataBuffer );
void Hexa_to_string_convert ( U8 ucDataLength, puchar pucDataBuffer );
void Usb_data_send ( U8 ucDataLength, puchar pucDataBuffer );
void Usb_data_convert_and_send ( U8 ucDataLength, puchar pucDataBuffer );
void Usb_byte_data_send (U8 ucSendDataSize, puchar ucUsbSendDataBuffer);

void heart_beat_init();
void show_heart_beat();

#endif /* _HID_TASK_H_ */

