/*This file is prepared for Doxygen automatic documentation generation.*/
//! \file *********************************************************************
//!
//! \brief This file manages the USB device controller.
//!
//!  The USB task checks the income of new requests from the USB Host.
//!  When a Setup request occurs, this task will launch the processing
//!  of this setup contained in the usb_standard_request.c file.
//!  Other class specific requests are also processed in this file.
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

//_____  I N C L U D E S ___________________________________________________

#include "conf\config.h"
#include "..\..\..\conf\conf_usb.h"
#include "usb_device_task.h"
#include "..\..\..\modules\usb\usb_task.h"
#if (USB_OTG_FEATURE == ENABLED)
  #include "modules/usb/host_chap9/usb_host_task.h"
#endif
#include "..\..\..\lib_mcu\usb\usb_drv.h"
#include "..\..\..\usb\usb_descriptors.h"
#include "..\..\..\modules\usb\device_chap9\usb_standard_request.h"
#include "..\..\..\lib_mcu\pll\pll_drv.h"

#if ((TARGET_BOARD==SPIDER) && (USB_OTG_FEATURE==ENABLED))
    #include "lib_board/lcd/lcd_drv.h"
#endif

//_____ M A C R O S ________________________________________________________




//_____ D E F I N I T I O N S ______________________________________________

//!
//! Public : (bit) usb_connected
//! usb_connected is set to TRUE when VBUS has been detected
//! usb_connected is set to FALSE otherwise
//!/
bit   usb_connected=FALSE;

//!
//! Public : (bit) usb_suspended
//! usb_suspended is set to TRUE when USB is in suspend mode
//! usb_suspended is set to FALSE otherwise
//!/
bit   usb_suspended=FALSE;




//!
//! Public : (U8) usb_configuration_nb
//! Store the number of the USB configuration used by the USB device
//! when its value is different from zero, it means the device mode is enumerated
//! Used with USB_DEVICE_FEATURE == ENABLED only
//!/
extern U8  usb_configuration_nb;


#if (USB_OTG_FEATURE == ENABLED)
//! Public : (U8) otg_b_device_state;
//! Store the current state of the B-Device
U8  otg_b_device_state;

//! Public : (U8) otg_device_sessions;
//! Store some events and conditions specifics to OTG Devices sessions
U8  otg_device_sessions;

//! Public : (U8) otg_b_device_hnp;
//! Store some events and conditions specifics to OTG B-Device HNP protocol
U8  otg_b_device_hnp;

//! Public : (U16) otg_tb_srp_cpt;
//! Counter used to signal a SRP fail condition (SRP fails if Tb_Srp_Fail elapsed)
U16 otg_tb_srp_cpt;

//! Public : (U8) sof_seen_in_session;
//! Indicates if a SOF has been received during the current session
//!/
U8  sof_seen_in_session;
#endif


//_____ D E C L A R A T I O N S ____________________________________________

//!
//! @brief This function initializes the USB device controller and system interrupt
//!
//! This function enables the USB controller and init the USB interrupts.
//! The aim is to allow the USB connection detection in order to send
//! the appropriate USB event to the operating mode manager.
//!
//! @param none
//!
//! @return none
//!
//!/
void usb_device_task_init(void)
{
   Enable_interrupt();
   Usb_disable();
   Usb_enable();
   Usb_select_device();
#if (USB_LOW_SPEED_DEVICE==ENABLE)
   Usb_low_speed_mode();
#endif
   Enable_interrupt();
#if (USB_OTG_FEATURE == ENABLED)
   Usb_enable_id_interrupt();
   Clear_otg_features_from_host();
   otg_device_sessions = 0;
#endif
}

//!
//! @brief This function initializes the USB device controller
//!
//! This function enables the USB controller and init the USB interrupts.
//! The aim is to allow the USB connection detection in order to send
//! the appropriate USB event to the operating mode manager.
//! Start device function is executed once VBUS connection has been detected
//! either by the VBUS change interrupt either by the VBUS high level
//!
//! @param none
//!
//! @return none
//!
void usb_start_device (void)
{
   Pll_start_auto();
   Wait_pll_ready();

   Disable_interrupt();
   Usb_unfreeze_clock();
   usb_init_device();         // configure the USB controller EP0
   Usb_attach();
   Enable_interrupt();
   
   Usb_enable_suspend_interrupt();
   Usb_enable_reset_interrupt();
#if (USB_OTG_FEATURE == ENABLED)
   Usb_enable_id_interrupt();
#endif
}

//! @brief Entry point of the USB device management
//!
//! This function is the entry point of the USB management. Each USB
//! event is checked here in order to launch the appropriate action.
//! If a Setup request occurs on the Default Control Endpoint,
//! the usb_process_request() function is call in the usb_standard_request.c file
//!
//! @param none
//!
//! @return none
void usb_device_task(void)
{
  
#if (USB_OTG_FEATURE == ENABLED)
   // Check if a reset has been received
   if(Is_usb_event(EVT_USB_RESET))
   {
      Usb_ack_event(EVT_USB_RESET);
      Usb_reset_endpoint(0);
      usb_configuration_nb=0;
      otg_b_device_state = B_IDLE;
      Clear_otg_features_from_host();
   }

   // When OTG mode enabled, B-Device is managed thanks to its state machine
   switch (otg_b_device_state)
   {
   //------------------------------------------------------
   //   B_IDLE state
   //
   //   - waits for Vbus to rise
   //   - initiate SRP if asked by user
   //
   case B_IDLE:
     if (Is_usb_vbus_high())
     {
       // Vbus rise
       usb_connected = TRUE;
       remote_wakeup_feature = DISABLED;
       usb_start_device();
       Usb_vbus_on_action();
       Usb_attach();
       otg_b_device_state = B_PERIPHERAL;
       Ack_user_request_srp();
       Clear_otg_features_from_host();
       remote_wakeup_feature = DISABLED;
       End_session_with_srp();
       if (Is_srp_sent_and_waiting_answer() && (sof_seen_in_session == TRUE))
       {
         Ack_srp_sent_and_answer();
         Otg_print_new_failure_message(OTGMSG_A_RESPONDED,OTG_TEMPO_2SEC);
       }
       Usb_enable_sof_interrupt();
       
     }
     else
     {
       if (Is_user_requested_srp() && Is_usb_id_device())
       {
         // User has requested a SRP
         Ack_user_request_srp();
         if (!Is_srp_sent_and_waiting_answer())
         {
           Pll_start_auto();  // reinit device mode
           Wait_pll_ready();
           Usb_disable();
           Usb_enable_uid_pin();
           Usb_enable();
           Usb_unfreeze_clock();
           Usb_select_device();
           Usb_attach();
           otg_b_device_state = B_SRP_INIT;
           Usb_device_initiate_srp();       // hardware waits for initial condition (SE0, Session End level)
           sof_seen_in_session = FALSE;
         }
       }
       if ((Is_srp_sent_and_waiting_answer()) && (Is_tb_srp_counter_overflow()))
       {
         // SRP failed because A-Device did not respond
         End_session_with_srp();
         Ack_srp_sent_and_answer();
         Otg_print_new_failure_message(OTGMSG_SRP_A_NO_RESP,OTG_TEMPO_3SEC);
       }
     }
     break;

     
   //------------------------------------------------------
   //   B_SRP_INIT
   //
   //   - a SRP has been initiated
   //   - B-Device waits it is finished to initialize variables
   //
   case B_SRP_INIT:
     if (!Is_usb_device_initiating_srp())
     {
       otg_b_device_state = B_IDLE;   // SRP initiated, return to Idle state (wait for Vbus to rise)
       Srp_sent_and_waiting_answer();
       Init_tb_srp_counter();
       Start_session_with_srp();
       Otg_print_new_event_message(OTGMSG_SRP_STARTED,TB_SRP_FAIL_MIN);
     }
     break;

     
   //------------------------------------------------------
     //   B_PERIPHERAL : the main state of OTG Peripheral
   //
   //   - all events are interrupt-handled
   //   - but they are saved and this function can execute alternate actions
   //   - also handle user requests (disconnect)
   //
   // ======================================================================================
   case B_PERIPHERAL:
     if (Is_otg_event(EVT_OTG_DEVICE_CONNECTED))
     {
       Otg_ack_event(EVT_OTG_DEVICE_CONNECTED); // set on a SetConfiguration descriptor reception
       Otg_print_new_event_message(OTGMSG_CONNECTED_TO_A,OTG_TEMPO_4SEC);
     }
     if (Is_usb_event(EVT_USB_SUSPEND)) // SUSPEND state
     {
        // Suspend and HNP operations are handled in the interrupt functions
     }
     if (Is_srp_sent_and_waiting_answer() && (sof_seen_in_session == TRUE))
     {
       Ack_srp_sent_and_answer();
       Otg_print_new_failure_message(OTGMSG_A_RESPONDED,OTG_TEMPO_2SEC);
     }
     if ((Is_srp_sent_and_waiting_answer()) && (Is_tb_srp_counter_overflow())) 
     {
       // SRP failed because A-Device did not respond
       End_session_with_srp();
       Ack_srp_sent_and_answer();
       Otg_print_new_failure_message(OTGMSG_SRP_A_NO_RESP,OTG_TEMPO_3SEC);
     }
     
     if (Is_usb_event(EVT_USB_RESUME) && !Is_usb_pending_remote_wake_up())  // RESUME signal detected
     {
       Usb_ack_event(EVT_USB_RESUME);
       Usb_ack_event(EVT_USB_SUSPEND);
       Usb_ack_remote_wake_up_start();
     }
     if (Is_usb_event(EVT_USB_UNPOWERED))
     {
       Usb_ack_event(EVT_USB_UNPOWERED);
       Clear_all_user_request();
       otg_b_device_state = B_IDLE;
     }
     if(Is_usb_event(EVT_USB_RESET))
     {
       Usb_ack_event(EVT_USB_RESET);
       Usb_reset_endpoint(0);
       usb_configuration_nb=0;
       Clear_otg_features_from_host();
     }
     if (Is_otg_event(EVT_OTG_HNP_ERROR))
     {
       Otg_ack_event(EVT_OTG_HNP_ERROR);
       Otg_print_new_failure_message(OTGMSG_DEVICE_NO_RESP,OTG_TEMPO_4SEC);
       PORTC &= ~0x10;
     }
     if (Is_user_requested_disc())
     {
       Ack_user_request_disc();
       if (Is_usb_id_device())
       {
         Usb_detach();
         Usb_freeze_clock();
         while (Is_usb_vbus_high());  // wait for Vbus to be under Va_vbus_valid
         otg_b_device_state = B_IDLE;
         usb_configuration_nb = 0;
         usb_connected = FALSE;
         Clear_all_user_request();
       }
     }
     break;

   //------------------------------------------------------
   //   B_HOST
   //
   //   - state entered after an HNP success
   //   - handle user requests (disconnection, suspend, hnp)
   //   - call the "host_task()" for Host level handlers
   //
   // ======================================================================================
   case B_HOST:
     if (Is_otg_event(EVT_OTG_DEV_UNSUPPORTED))
     {
       Otg_ack_event(EVT_OTG_DEV_UNSUPPORTED);
       Clear_all_user_request();
       otg_b_device_state = B_IDLE;
       device_state = DEVICE_UNATTACHED;
     }
     if (Is_user_requested_disc() || Is_user_requested_suspend() || Is_user_requested_hnp())
     {
       Ack_user_request_disc();   // suspend and hnp requests cleared in B_END_HNP_SUSPEND stage
       Host_disable_sof();        // go into suspend mode
       Usb_host_reject_hnp();
       otg_b_device_state = B_END_HNP_SUSPEND;
       Usb_ack_suspend();
       Usb_enable_suspend_interrupt();
     }
     if (Is_usb_event(EVT_USB_UNPOWERED))
     {
       Usb_ack_event(EVT_USB_UNPOWERED);
       Usb_freeze_clock();
       otg_b_device_state = B_IDLE;
       device_state = DEVICE_UNATTACHED;
     }
     usb_host_task();   // call the host task
     break;

   //------------------------------------------------------
   //   B_END_HNP_SUSPEND
   //
   //   - device enters this state after being B_HOST, on a user request to stop bus activity (suspend, disconnect or hnp request)
   //   - macro is reset to peripheral mode
   //
   // ======================================================================================
   case B_END_HNP_SUSPEND:
     if (Is_usb_event(EVT_USB_SUSPEND))
     {
       Usb_ack_event(EVT_USB_SUSPEND);
       Usb_device_stop_hnp();
       Usb_select_device();
       device_state = DEVICE_UNATTACHED;
       if (Is_user_requested_hnp() || Is_user_requested_suspend())
       {
         otg_b_device_state = B_PERIPHERAL;
         Ack_user_request_suspend();
         Ack_user_request_hnp();
       }
       else
       {
         otg_b_device_state = B_IDLE;
         Usb_detach();
         Usb_freeze_clock();
       }
     }
     break;


   default:
     otg_b_device_state = B_IDLE;
     Clear_all_user_request();
     device_state = DEVICE_UNATTACHED;
     break;
   }


#else
   
   // Non-OTG exclusives Device operations

   // VBUS state detection
   if (Is_usb_vbus_high()&& (usb_connected==FALSE))
   {
      usb_connected = TRUE;
      remote_wakeup_feature = DISABLED;
      Usb_vbus_on_action();
      Usb_send_event(EVT_USB_POWERED);
      usb_start_device();
   }
   if (Is_usb_vbus_low()&& (usb_connected==TRUE))
   {
      usb_connected = FALSE;
      usb_configuration_nb = 0;
      Usb_send_event(EVT_USB_UNPOWERED);
      Usb_detach();
      Usb_freeze_clock();
      Usb_vbus_off_action();
   }

   if(Is_usb_event(EVT_USB_RESET))
   {
      Usb_ack_event(EVT_USB_RESET);
      Usb_reset_endpoint(0);
      usb_configuration_nb=0;
   }

#endif

   
   
   // =======================================
   // Common Standard Device Control Requests
   // =======================================
   //   - device enumeration process
   //   - device control commands and features
   Usb_select_endpoint(EP_CONTROL);
   if (Is_usb_receive_setup())
   {
      usb_process_request();
   }
}
