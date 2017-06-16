/*This file is prepared for Doxygen automatic documentation generation.*/
//! \file *********************************************************************
//!
//! \brief This file contains the usb HID definition constant parameters
//! from Firmware Specification Version 1.11
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

#ifndef _USB_COMMUN_HID_H_
#define _USB_COMMUN_HID_H_


//! \name Global Class, SubClass & Protocol constants for HID
//! @{
#define  HID_CLASS                            0x03
#define  HID_SUB_CLASS_BOOT                   0x01    //!< Is used to signal the BIOS BOOT support (0=no no sub class,1=boot interface SubClass)
#define  HID_PROTOCOL_KEYBOARD                0x01    //!< Protocol keyboard standard
#define  HID_PROTOCOL_MOUSE                   0x02    //!< Protocol mouse standard
//! @}
        

//! \name Specific setup requests from HID driver (USB_SETUP_SET_CLASS_INTER or USB_SETUP_GET_CLASS_INTER)
//! @{
#define  SETUP_HID_GET_REPORT                 0x01
#define  SETUP_HID_GET_IDLE                   0x02
#define  SETUP_HID_GET_PROTOCOL               0x03
#define  SETUP_HID_SET_REPORT                 0x09
#define  SETUP_HID_SET_IDLE                   0x0A
#define  SETUP_HID_SET_PROTOCOL               0x0B
        
   //! \name HID Descriptor types
   //! @{
#define  DESCRIPTOR_HID                       0x21
#define  DESCRIPTOR_REPORT                    0x22
#define  DESCRIPTOR_PHYSICAL                  0x23
   //! @}
        
   //! \name HID Report type (used by SETUP_HID_GET_REPORT & SETUP_HID_SET_REPORT)
   //! @{
#define  REPORT_TYPE_INPUT                    0x01
#define  REPORT_TYPE_OUTPUT                   0x02
#define  REPORT_TYPE_FEATURE                  0x03
   //! @}

   //! \name Constants of field DESCRIPTOR_HID
   //! @{
#define  HID_BDC                              0x0111  //!< Numeric expression identifying the HID Class Specification release (here V1.11)
#define  HID_CLASS_DESC_NB_DEFAULT            0x01    //!< Numeric expression specifying the number of class descriptors (always at least one i.e. Report descriptor.)

      //! \name Country code
      //! @{
#define  HID_NO_COUNTRY_CODE                  0       // Not Supported       
#define  HID_COUNTRY_ARABIC                   1       // Arabic              
#define  HID_COUNTRY_BELGIAN                  2       // Belgian             
#define  HID_COUNTRY_CANADIAN_BILINGUAL       3       // Canadian-Bilingual  
#define  HID_COUNTRY_CANADIAN_FRENCH          4       // Canadian-French     
#define  HID_COUNTRY_CZECH_REPUBLIC           5       // Czech Republic      
#define  HID_COUNTRY_DANISH                   6       // Danish              
#define  HID_COUNTRY_FINNISH                  7       // Finnish             
#define  HID_COUNTRY_FRENCH                   8       // French              
#define  HID_COUNTRY_GERMAN                   9       // German              
#define  HID_COUNTRY_GREEK                    10      // Greek               
#define  HID_COUNTRY_HEBREW                   11      // Hebrew              
#define  HID_COUNTRY_HUNGARY                  12      // Hungary             
#define  HID_COUNTRY_INTERNATIONAL_ISO        13      // International (ISO) 
#define  HID_COUNTRY_ITALIAN                  14      // Italian             
#define  HID_COUNTRY_JAPAN_KATAKANA           15      // Japan (Katakana)    
#define  HID_COUNTRY_KOREAN                   16      // Korean              
#define  HID_COUNTRY_LATIN_AMERICAN           17      // Latin American      
#define  HID_COUNTRY_NETHERLANDS_DUTCH        18      // Netherlands/Dutch
#define  HID_COUNTRY_NORWEGIAN                19      // Norwegian
#define  HID_COUNTRY_PERSIAN_FARSI            20      // Persian (Farsi)
#define  HID_COUNTRY_POLAND                   21      // Poland
#define  HID_COUNTRY_PORTUGUESE               22      // Portuguese
#define  HID_COUNTRY_RUSSIA                   23      // Russia
#define  HID_COUNTRY_SLOVAKIA                 24      // Slovakia
#define  HID_COUNTRY_SPANISH                  25      // Spanish
#define  HID_COUNTRY_SWEDISH                  26      // Swedish
#define  HID_COUNTRY_SWISS_FRENCH             27      // Swiss/French
#define  HID_COUNTRY_SWISS_GERMAN             28      // Swiss/German
#define  HID_COUNTRY_SWITZERLAND              29      // Switzerland
#define  HID_COUNTRY_TAIWAN                   30      // Taiwan
#define  HID_COUNTRY_TURKISH_Q                31      // Turkish-Q
#define  HID_COUNTRY_UK                       32      // UK
#define  HID_COUNTRY_US                       33      // US
#define  HID_COUNTRY_YUGOSLAVIA               34      // Yugoslavia
#define  HID_COUNTRY_TURKISH_F                35      // Turkish-F
      //! @}
   //! @}
//! @}


//! \name HID KEYS values
//! @{
#define  HID_A                4
#define  HID_B                5
#define  HID_C                6
#define  HID_D                7
#define  HID_E                8
#define  HID_F                9
#define  HID_G                10
#define  HID_H                11
#define  HID_I                12
#define  HID_J                13
#define  HID_K                14
#define  HID_L                15
#define  HID_M                16
#define  HID_N                17
#define  HID_O                18
#define  HID_P                19
#define  HID_Q                20
#define  HID_R                21
#define  HID_S                22
#define  HID_T                23
#define  HID_U                24
#define  HID_V                25
#define  HID_W                26
#define  HID_X                27
#define  HID_Y                28
#define  HID_Z                29
#define  HID_1                30  
#define  HID_2                31  
#define  HID_3                32  
#define  HID_4                33  
#define  HID_5                34  
#define  HID_6                35  
#define  HID_7                36  
#define  HID_8                37  
#define  HID_9                38  
#define  HID_0                39  
#define  HID_ENTER            40 
#define  HID_ESCAPE           41  
#define  HID_BACKSPACE        42  
#define  HID_TAB              43  
#define  HID_SPACEBAR         44  
#define  HID_UNDERSCORE       45  
#define  HID_PLUS             46  
/*                           
#define  HID_[ {              47  
#define  HID_] }              48  
*/                           
#define  HID_BACKSLASH        49  
/*                           
#define  HID_# ~              50  
#define  HID_; :              51  
#define  HID_‘ "              52  
*/                           
#define  HID_TILDE            53  
#define  HID_COMMA            54  
#define  HID_DOT              55
#define  HID_SLASH            56
#define  HID_CAPS LOCK        57  
#define  HID_F1               58  
#define  HID_F2               59  
#define  HID_F3               60  
#define  HID_F4               61  
#define  HID_F5               62  
#define  HID_F6               63  
#define  HID_F7               64  
#define  HID_F8               65  
#define  HID_F9               66  
#define  HID_F10              67  
#define  HID_F11              68  
#define  HID_F12              69  
#define  HID_PRINTSCREEN      70  
#define  HID_SCROLL LOCK      71  
#define  HID_PAUSE            72  
#define  HID_INSERT           73  
#define  HID_HOME             74  
#define  HID_PAGEUP           75  
#define  HID_DELETE           76  
#define  HID_END              77  
#define  HID_PAGEDOWN         78  
#define  HID_RIGHT            79  
#define  HID_LEFT             80  
#define  HID_DOWN             81  
#define  HID_UP               82  
#define  HID_KEYPAD_NUM_LOCK  83  
#define  HID_KEYPAD_DIVIDE    84  
#define  HID_KEYPAD_AT        85  
#define  HID_KEYPAD_MULTIPLY  85
#define  HID_KEYPAD_MINUS     86
#define  HID_KEYPAD_PLUS      87
#define  HID_KEYPAD_ENTER     88  
#define  HID_KEYPAD_1         89  
#define  HID_KEYPAD_2         90  
#define  HID_KEYPAD_3         91  
#define  HID_KEYPAD_4         92  
#define  HID_KEYPAD_5         93  
#define  HID_KEYPAD_6         94  
#define  HID_KEYPAD_7         95  
#define  HID_KEYPAD_8         96  
#define  HID_KEYPAD_9         97  
#define  HID_KEYPAD_0         98

   //! \name HID modifier values
   //! @{
#define  HID_MODIFIER_NONE          0x00
#define  HID_MODIFIER_LEFT_CTRL     0x01
#define  HID_MODIFIER_LEFT_SHIFT    0x02
#define  HID_MODIFIER_LEFT_ALT      0x04
#define  HID_MODIFIER_LEFT_GUI      0x08
#define  HID_MODIFIER_RIGHT_CTRL    0x10
#define  HID_MODIFIER_RIGHT_SHIFT   0x20
#define  HID_MODIFIER_RIGHT_ALT     0x40
#define  HID_MODIFIER_RIGHT_GUI     0x80
   //! @}
//! @}


#endif   // _USB_COMMUN_HID_H_

