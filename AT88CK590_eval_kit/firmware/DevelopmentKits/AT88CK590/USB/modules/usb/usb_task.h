/*This file is prepared for Doxygen automatic documentation generation.*/
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

#ifndef _USB_TASK_H_
#define _USB_TASK_H_

#include "usb_commun.h"

//! @defgroup usb_task USB task entry point
//! @{

//_____ I N C L U D E S ____________________________________________________


//_____ M A C R O S ________________________________________________________

      //! @defgroup usb_software_evts USB software Events Management
      //! Macros to manage USB events detected under interrupt
      //! @{
#define Usb_send_event(x)               (g_usb_event |= (1<<x))
#define Usb_ack_event(x)                (g_usb_event &= ~(1<<x))
#define Usb_clear_all_event()           (g_usb_event = 0)
#define Is_usb_event(x)                 ((g_usb_event & (1<<x)) ? TRUE : FALSE)
#define Is_not_usb_event(x)             ((g_usb_event & (1<<x)) ? FALSE: TRUE)
#define Is_host_emergency_exit()        (Is_usb_id_device()||(device_state==DEVICE_DISCONNECTED))
#define Is_usb_device()                 (g_usb_mode==USB_MODE_DEVICE ? TRUE : FALSE)
#define Is_usb_host()                   (g_usb_mode==USB_MODE_HOST   ? TRUE : FALSE)

#define Otg_send_event(x)               (g_otg_event |= (1<<x))
#define Otg_ack_event(x)                (g_otg_event &= ~(1<<x))
#define Otg_clear_all_event()           (g_otg_event = 0)
#define Is_otg_event(x)                 ((g_otg_event & (1<<x)) ? TRUE : FALSE)
#define Is_not_otg_event(x)             ((g_otg_event & (1<<x)) ? FALSE: TRUE)

#define EVT_USB_POWERED               1         // USB plugged
#define EVT_USB_UNPOWERED             2         // USB un-plugged
#define EVT_USB_DEVICE_FUNCTION       3         // USB in device
#define EVT_USB_HOST_FUNCTION         4         // USB in host
#define EVT_USB_SUSPEND               5         // USB suspend
#define EVT_USB_WAKE_UP               6         // USB wake up
#define EVT_USB_RESUME                7         // USB resume
#define EVT_USB_RESET                 8         // USB reset
#define EVT_HOST_SOF                  9         // Host start of frame sent
#define EVT_HOST_HWUP                 10        // Host wakeup detected
#define EVT_HOST_DISCONNECTION        11        // The target device is disconnected
#define EVT_HOST_REMOTE_WAKEUP        12        // Remote WakeUp has been received (Host)

#define EVT_OTG_HNP_ERROR             1        // The HNP has failed (A has not responded)
#define EVT_OTG_HNP_SUCCESS           2        // The HNP has succeeded
#define EVT_OTG_SRP_RECEIVED          3        // A-Device received a SRP
#define EVT_OTG_DEV_UNSUPPORTED       4        // An error occurred while the device was enumerated
#define EVT_OTG_DEVICE_CONNECTED      5        // B-Device has been configured
      //! @}

#define USB_MODE_UNDEFINED            0x00
#define USB_MODE_HOST                 0x01
#define USB_MODE_DEVICE               0x02


#define   VBUS_PULSE    1
#define   DATA_PULSE    0

#define   BDEV_HNP_NB_RETRY     3       // number of times that a B-Device retries a HNP with a failure as a result
                                        // after "n" failures, the "b_hnp_enable" feature is cancelled


//!
//! @brief Definitions of OTG user requests (user software requests)
//!
#define USER_RQST_SRP           0x01
#define USER_RQST_SUSPEND       0x02
#define USER_RQST_VBUS          0x04    // Vbus TOGGLE
#define USER_RQST_HNP           0x08
#define USER_RQST_RESUME        0x10
#define USER_RQST_RESET         0x20
#define USER_RQST_DISCONNECT    0x40

// Ask for the B-PERIPH to generate a SRP
#define Is_user_requested_srp()       (((otg_user_request&USER_RQST_SRP) != 0) ? TRUE : FALSE)
#define Set_user_request_srp()        (otg_user_request |= USER_RQST_SRP)
#define Ack_user_request_srp()        (otg_user_request &= ~USER_RQST_SRP)

// Ask for the A-HOST to enter suspend
#define Is_user_requested_suspend()   (((otg_user_request&USER_RQST_SUSPEND) != 0) ? TRUE : FALSE)
#define Set_user_request_suspend()    (otg_user_request |= USER_RQST_SUSPEND)
#define Ack_user_request_suspend()    (otg_user_request &= ~USER_RQST_SUSPEND)

// Ask for the A-HOST to toggle Vbus
#define Is_user_requested_vbus()      (((otg_user_request&USER_RQST_VBUS) != 0) ? TRUE : FALSE)
#define Set_user_request_vbus()       (otg_user_request |= USER_RQST_VBUS)
#define Ack_user_request_vbus()       (otg_user_request &= ~USER_RQST_VBUS)

// Ask for an HNP initiation on both devices
#define Is_user_requested_hnp()       (((otg_user_request&USER_RQST_HNP) != 0) ? TRUE : FALSE)
#define Set_user_request_hnp()        (otg_user_request |= USER_RQST_HNP)
#define Ack_user_request_hnp()        (otg_user_request &= ~USER_RQST_HNP)

// Ask for the B-PERIPH or the A-PERIPH to disconnect from the bus
#define Is_user_requested_disc()      (((otg_user_request&USER_RQST_DISCONNECT) != 0) ? TRUE : FALSE)
#define Set_user_request_disc()       (otg_user_request |= USER_RQST_DISCONNECT)
#define Ack_user_request_disc()       (otg_user_request &= ~USER_RQST_DISCONNECT)

/* NOT USED */
/*
// Ask for the A-HOST to resume the bus or the B-PERIPH to send an upstream resume
#define Is_user_requested_resume()    (((otg_user_request&USER_RQST_RESUME) != 0) ? TRUE : FALSE)
#define Set_user_request_resume()     (otg_user_request |= USER_RQST_RESUME)
#define Ack_user_request_resume()     (otg_user_request &= ~USER_RQST_RESUME)

// Ask for the A-HOST to reset the bus
#define Is_user_requested_reset()     (((otg_user_request&USER_RQST_RESET) != 0) ? TRUE : FALSE)
#define Set_user_request_reset()      (otg_user_request |= USER_RQST_RESET)
#define Ack_user_request_reset()      (otg_user_request &= ~USER_RQST_RESET)
*/

#define Clear_all_user_request()      (otg_user_request = 0)


/**
 * @brief This macro initializes the timer for OTG specific timings
 *
 *  The corresponding timer is selected in conf_usb.h
 *  An IT is launched every 2 ms when CPU runs at 8 MHz
 *
 *  @param none
 *
 *  @return none
 */
#define Otg_timer_init()              (Timer16_select(OTG_USE_TIMER), Timer16_set_clock(TIMER16_CLKIO_BY_256), \
                                       Timer16_set_mode_output_a(TIMER16_COMP_MODE_NORMAL), \
                                       Timer16_set_waveform_mode(TIMER16_WGM_CTC_OCR), \
                                       Timer16_set_compare_a(62), Timer16_set_counter(0x0000), \
                                       Timer16_clear_compare_a_it(), Timer16_compare_a_it_enable())

//_____ D E C L A R A T I O N S ____________________________________________

extern volatile U16 g_usb_event;
extern U8 g_usb_mode;
extern U8 remote_wakeup_feature;

/**
 * @brief This function initializes the USB process.
 *
 *  This function enables the USB controller and init the USB interrupts.
 *  The aim is to allow the USB connection detection in order to send
 *  the appropriate USB event to the operating mode manager.
 *  Depending on the mode supported (HOST/DEVICE/DUAL_ROLE) the function
 *  calls the corresponding usb mode initialization function
 *
 *  @param none
 *
 *  @return none
 */
void usb_task_init     (void);

/**
 *  @brief Entry point of the USB management
 *
 *  Depending on the mode supported (HOST/DEVICE/DUAL_ROLE) the function
 *  calls the corresponding usb management function
 *
 *  @param none
 *
 *  @return none
*/
void usb_task          (void);

extern volatile U8 private_sof_counter;


/**
 *  @brief External public declarations for OTG features
 *
 *  @param none
 *
 *  @return none
*/
extern volatile U8 otg_features_supported;
extern U8 otg_user_request;

extern volatile U16 g_otg_event;

#if (OTG_VBUS_AUTO_AFTER_A_PLUG_INSERTION == ENABLED)
  extern U8 id_changed_to_host_event;
#endif

extern void otg_not_supported_device(void);

#if (USB_OTG_FEATURE == ENABLED)
  //! @brief Enable some additional feature to pass compliance plan
  //!
  //! This feature must be  ENABLED to pass the OTG compliance program (FS-A-UUT tests TD4.5-2.9ms and TD4.6)
  //! Possible values are : ENABLE to add a special feature to OTG firmware : the problem comes from the disconnection delay
  //!                       of A-PERIPH once it has detected a Suspend condition. This delay is 3ms, but compliance test
  //!                       is not enough precise. This feature waits 500µs freezing clock when it notices that SOF are missing
  //!                       DISABLE to disable this feature (that may lead to malfunction in original cases)
  #ifndef   OTG_COMPLIANCE_TRICKS
    #define OTG_COMPLIANCE_TRICKS                   DISABLED
  #endif

  //! @brief Selects a Vbus delivery option
  //!
  //! This feature must be ENABLED to pass the OTG compliance program (Checklist OTG Protocol P23/P24)
  //! Possible values are : ENABLE to make the application initiate a session (like an answer to SRP) once A-plug inserted
  //!                       DISABLE to disable this feature
  //! This feature is compatible with OTG_VBUS_AUTO_WHEN_A_PLUG feature disabled
  #ifndef   OTG_VBUS_AUTO_AFTER_A_PLUG_INSERTION
    #define OTG_VBUS_AUTO_AFTER_A_PLUG_INSERTION    DISABLED
  #endif

  //! @brief ENABLE to make the B-Device run a HNP automatically if a SetFeature(b_hnp_enable) is received and Suspend detected
  //!
  //! This feature must be ENABLED to pass the OTG compliance program
  //! Possible values ENABLE or DISABLE
  #ifndef   OTG_B_DEVICE_AUTORUN_HNP_IF_REQUIRED
    #define OTG_B_DEVICE_AUTORUN_HNP_IF_REQUIRED    ENABLED
  #endif

  //! @brief Selects the Reset Length (x11ms)
  //!
  //! This value is the number of consecutive resets sent by the Host
  #ifndef   OTG_RESET_LENGTH
    #define OTG_RESET_LENGTH                        1
  #endif
#endif


//! @brief  OTG Messaging definitions
//!         "No Silent Failure" rule makes any OTG compliant device handle messaging functions
//!         Different means are supported : LCD display, LEDs, etc.

#define   MSG_DISPLAY_NODELAY     0xFFFF
#define   OTG_TEMPO_1SEC          0x01F4  // 500 (x2ms)
#define   OTG_TEMPO_2SEC          0x03E8  // 1000
#define   OTG_TEMPO_3SEC          0x05DC  // 1500
#define   OTG_TEMPO_4SEC          0x07D0  // 2000
#define   OTR_TEMPO_5SEC          0x09C4  // 2500

// These messages ID numbers must be used with the OTG messaging functions
#define   OTGMSG_SRP_STARTED      1      // Event
#define   OTGMSG_SRP_A_NO_RESP    2      // FAILURE msg
#define   OTGMSG_A_RESPONDED      3      // Event
#define   OTGMSG_CONNECTED_TO_A   4      // Event
#define   OTGMSG_UNSUPPORTED      5      // FAILURE msg
#define   OTGMSG_UNSUPPORTED_HUB  6      // FAILURE msg
#define   OTGMSG_SRP_RECEIVED     7      // Event
#define   OTGMSG_DEVICE_NO_RESP   8      // FAILURE msg
#define   OTGMSG_VBUS_SURCHARGE   9      // Event

// Messages definitions for OTG library, should not be used directly by user
#define   OTGSTR_SRP_STARTED      "SRP Initiated     "  // Event
#define   OTGSTR_SRP_A_NO_RESP    "A-Dev No Response "  // FAILURE
#define   OTGSTR_A_RESPONDED      "A-Device Responded"  // Event
#define   OTGSTR_CONNECTED_TO_A   "Connected to A-Dev"  // Event
#define   OTGSTR_UNSUPPORTED      "Unsupported Device"  // FAILURE
#define   OTGSTR_UNSUPPORTED_HUB  "Hub Unsuppported  "  // FAILURE
#define   OTGSTR_SRP_RECEIVED     "SRP Received      "  // Event
#define   OTGSTR_DEVICE_NO_RESP   "Device No Response"  // FAILURE
#define   OTGSTR_VBUS_SURCHARGE   "VBUS OverCurrent !"  // Event

extern U16 otg_msg_event_delay;
extern U16 otg_msg_failure_delay;

// Output messaging method choice (mode and functions defined in "conf_usb.h")
#define     OTGMSG_NONE     0   // no messages displayed
#define     OTGMSG_FAIL     1   // only failure messages displayed
#define     OTGMSG_ALL      2   // all messages displayed

#if (OTG_MESSAGING_OUTPUT == OTGMSG_ALL)            // ALL MESSAGES
  extern void Otg_messaging_init(void);
  extern void Otg_output_event_msg(U8);
  extern void Otg_output_failure_msg(U8);
  extern void Otg_output_event_clear(void);
  extern void Otg_output_failure_clear(void);
  //! Otg_print_new_event_message(str,tm) displays the "str" message on the EVENT line during the "tm" delay (x2ms)  
  #define   Otg_print_new_event_message(str,tm)     (Otg_output_event_msg(str), otg_msg_event_delay = tm)
  #define   Otg_clear_event_message()               Otg_output_event_clear()
  #define   Get_event_msg_delay()                   (otg_msg_event_delay)
  #define   Decrement_event_msg_delay()             (otg_msg_event_delay--)
  //! Otg_print_new_failure_message(str,tm) displays the "str" message on the FAILURE line during the "tm" delay (x2ms)
  #define   Otg_print_new_failure_message(str,tm)   (Otg_output_failure_msg(str), otg_msg_failure_delay = tm)
  #define   Otg_clear_failure_message()             Otg_output_failure_clear()
  #define   Get_failure_msg_delay()                 (otg_msg_failure_delay)
  #define   Decrement_failure_msg_delay()           (otg_msg_failure_delay--)
#elif (OTG_MESSAGING_OUTPUT == OTGMSG_FAIL)         // ONLY FAILURE MESSAGES (NEEDED FOR COMPLIANCE)
  extern void Otg_messaging_init(void);
  extern void Otg_output_failure_msg(U8);
  extern void Otg_output_failure_clear(void);
  #define   Otg_print_new_event_message(str,tm)     
  #define   Otg_clear_event_message()               
  #define   Get_event_msg_delay()                   
  #define   Decrement_event_msg_delay()             
  #define   Otg_print_new_failure_message(str,tm)   (Otg_output_failure_msg(str), otg_msg_failure_delay = tm)
  #define   Otg_clear_failure_message()             Otg_output_failure_clear()
  #define   Get_failure_msg_delay()                 (otg_msg_failure_delay)
  #define   Decrement_failure_msg_delay()           (otg_msg_failure_delay--)
#else                                               // NOT OTG COMPLIANT
  #define   Otg_messaging_init()
  #define   Otg_print_new_event_message(str,tm)
  #define   Otg_clear_event_message()
  #define   Get_event_msg_delay()
  #define   Decrement_event_msg_delay()
  #define   Otg_print_new_failure_message(str,tm)
  #define   Otg_clear_failure_message()
  #define   Get_failure_msg_delay()
  #define   Decrement_failure_msg_delay()
#endif
//! @}

#endif /* _USB_TASK_H_ */

