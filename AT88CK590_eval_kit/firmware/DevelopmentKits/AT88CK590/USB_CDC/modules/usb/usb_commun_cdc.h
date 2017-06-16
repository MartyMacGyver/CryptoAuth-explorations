/*This file is prepared for Doxygen automatic documentation generation.*/
//! \file *********************************************************************
//!
//! \brief This file contains the usb CDC definition constant parameters
//!
//! - Compiler:           IAR EWAVR and GNU GCC for AVR
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

#ifndef _USB_COMMUN_CDC_H_
#define _USB_COMMUN_CDC_H_

//! \name Global Class, SubClass & Protocol constants for CDC
//! @{
#define  CDC_GLOB_CLASS          0x02
#define  CDC_COMM_CLASS          0x02
#define  CDC_DATA_CLASS          0x0A
#define  CDC_GLOB_SUBCLASS       0x00
#define  CDC_COMM_SUBCLASS       0x02
#define  CDC_DATA_SUBCLASS       0x00
#define  CDC_GLOB_PROTOCOL       0x00
#define  CDC_COMM_PROTOCOL       0x01
#define  CDC_DATA_PROTOCOL       0x00
//! @}

//! \name Specific setup requests from CDC
//! @{
#define  SETUP_CDC_SEND_ENCAPSULATED_COMMAND                   0x00
#define  SETUP_CDC_GET_ENCAPSULATED_RESPONSE                   0x01
#define  SETUP_CDC_SET_COMM_FEATURE                            0x02
#define  SETUP_CDC_GET_COMM_FEATURE                            0x03
#define  SETUP_CDC_CLEAR_COMM_FEATURE                          0x04
#define  SETUP_CDC_SET_AUX_LINE_STATE                          0x10
#define  SETUP_CDC_SET_HOOK_STATE                              0x11
#define  SETUP_CDC_PULSE_SETUP                                 0x12
#define  SETUP_CDC_SEND_PULSE                                  0x13
#define  SETUP_CDC_SET_PULSE_TIME                              0x14
#define  SETUP_CDC_RING_AUX_JACK                               0x15
#define  SETUP_CDC_SET_LINE_CODING                             0x20
#define  SETUP_CDC_GET_LINE_CODING                             0x21
#define  SETUP_CDC_SET_CONTROL_LINE_STATE                      0x22
#define  SETUP_CDC_SEND_BREAK                                  0x23
#define  SETUP_CDC_SET_RINGER_PARMS                            0x30
#define  SETUP_CDC_GET_RINGER_PARMS                            0x31
#define  SETUP_CDC_SET_OPERATION_PARMS                         0x32
#define  SETUP_CDC_GET_OPERATION_PARMS                         0x33
#define  SETUP_CDC_SET_LINE_PARMS                              0x34
#define  SETUP_CDC_GET_LINE_PARMS                              0x35
#define  SETUP_CDC_DIAL_DIGITS                                 0x36
#define  SETUP_CDC_SET_UNIT_PARAMETER                          0x37
#define  SETUP_CDC_GET_UNIT_PARAMETER                          0x38
#define  SETUP_CDC_CLEAR_UNIT_PARAMETER                        0x39
#define  SETUP_CDC_GET_PROFILE                                 0x3A
#define  SETUP_CDC_SET_ETHERNET_MULTICAST_FILTERS              0x40
#define  SETUP_CDC_SET_ETHERNET_POWER_MANAGEMENT_PATTERNFILTER 0x41
#define  SETUP_CDC_GET_ETHERNET_POWER_MANAGEMENT_PATTERNFILTER 0x42
#define  SETUP_CDC_SET_ETHERNET_PACKET_FILTER                  0x43
#define  SETUP_CDC_GET_ETHERNET_STATISTIC                      0x44
#define  SETUP_CDC_SET_ATM_DATA_FORMAT                         0x50
#define  SETUP_CDC_GET_ATM_DEVICE_STATISTICS                   0x51
#define  SETUP_CDC_SET_ATM_DEFAULT_VC                          0x52
#define  SETUP_CDC_GET_ATM_VC_STATISTICS                       0x53

// Added bNotification codes according cdc spec 1.1 chapter 6.3
#define  SETUP_CDC_BN_RING_DETECT                              0x09
#define  SETUP_CDC_BN_SERIAL_STATE                             0x20
#define  SETUP_CDC_BN_CALL_STATE_CHANGE                        0x28
#define  SETUP_CDC_BN_LINE_STATE_CHANGE                        0x29
//! @}

#endif   // _USB_COMMUN_CDC_H_

