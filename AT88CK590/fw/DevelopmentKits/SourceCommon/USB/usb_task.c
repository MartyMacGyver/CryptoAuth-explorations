/** \file usb_task.c
 *  \brief This file manages the USB task either device/host or both.
 *
 *  Copyright (c) 2006 Atmel.
 *
 *  Please read file license.txt for copyright notice.
 *
 *  The USB task selects the correct USB task (usb_device task or usb_host task
 *  to be executed depending on the current mode available.
 *
 *  According to USB_DEVICE_FEATURE and USB_HOST_FEATURE value (located in conf_usb.h file)
 *  The Usb_task can be configured to support USB DEVICE mode or USB Host mode or both
 *  for a dual role device application.
 *
 *  This module also contains the general USB interrupt subroutine. This subroutine is used
 *  to detect asynchronous USB events.
 *
 *  Note:
 *    - The Usb_task belongs to the scheduler, the Usb_device_task and usb_host do not,
 *      they are called from the general Usb_task
 *    - See conf_usb.h file for more details about the configuration of this module
 *
 *
 *  \version 1.28 at90usb128-demo-hidgen-1_0_2 $Id: Usb_task.c,v 1.28
 *  2006/07/03 11:55:11 rletendu Exp $
 */

//_____  I N C L U D E S ___________________________________________________
#include "usb_task.h"
#include "usb_drv.h"
#include "usb_device_task.h"
#include "conf_usb.h"

//_____ M A C R O S ________________________________________________________
#ifndef LOG_STR_CODE
#define LOG_STR_CODE(str)
#else
uchar code log_device_disconnect[] = "Device Disconnected";
uchar code log_id_change[] = "Pin Id Change";
#endif

//_____ D E F I N I T I O N S ______________________________________________

/** Public : ushort usGUsbEvent
 *  ucUsbConnected is used to store USB events detected upon
 *  USB general interrupt subroutine
 *  Its value is managed by the following macros (See Usb_task.h file)
 *  Usb_event_send(x)
 *  Usb_event_ack(x)
 *  Usb_all_event_clear()
 *  Is_usb_event(x)
 *  Is_not_usb_event(x)
 */
volatile ushort usGUsbEvent = 0;


#if (USB_DEVICE_FEATURE == ENABLE)

/** Public : (bit) ucUsbConnected
 *  ucUsbConnected is set to TRUE when VBUS has been detected
 *  ucUsbConnected is set to FALSE otherwise
 *  Used with USB_DEVICE_FEATURE == ENABLE only
 */
extern uchar   ucUsbConnected;

/** Public : (uchar) ucUsbConfigurationNb
 *  Store the number of the USB configuration used by the USB device
 *  when its value is different from zero, it means the device mode is enumerated
 *  Used with USB_DEVICE_FEATURE == ENABLE only
 */
extern uchar    ucUsbConfigurationNb;
#endif


#if (USB_HOST_FEATURE == ENABLE)

/** Private : (uchar) ucPrivateSofCounter
 *  Incremented  by host SOF interrupt subroutime
 *  This counter is used to detect timeout in host requests.
 *  It must not be modified by the user application tasks.
 */
volatile uchar ucPrivateSofCounter = 0;

#if (USB_HOST_PIPE_INTERRUPT_TRANSFER == ENABLE)
extern volatile S_pipe_int   it_pipe_str[MAX_EP_NB];
#endif

#endif

#if ((USB_DEVICE_FEATURE == ENABLE) && (USB_HOST_FEATURE == ENABLE))

/** Public : (uchar) ucGUsbMode
 *  Used in dual role application (both device/host) to store
 *  the current mode the usb controller is operating
 */
    uchar ucGUsbMode = USB_MODE_UNDEFINED;
    uchar ucGOldUsbMode;
#endif

//_____ D E C L A R A T I O N S ____________________________________________

/** \brief This function initializes the USB process.
 *
 *  Depending on the mode supported (HOST/DEVICE/DUAL_ROLE) the function
 *  calls the corresponding usb mode initialization function.
 */
void Usb_task_initialize ( void )
{
#if ((USB_HOST_FEATURE == ENABLE) && (USB_DEVICE_FEATURE == ENABLE))
    uchar ucDelay;
#endif

#if (USE_USB_PADS_REGULATOR == ENABLE)  // Otherwise assume USB PADs regulator is not used
    Usb_regulator_enable ();
#endif

// ---- DUAL ROLE DEVICE USB MODE ---------------------------------------------
#if ((USB_DEVICE_FEATURE == ENABLE) && (USB_HOST_FEATURE == ENABLE))
    Usb_uid_pin_enable ();
    ucDelay = PORTA;
    ucGUsbMode = USB_MODE_UNDEFINED;
    if (Is_usb_id_device ())
    {
        ucGUsbMode = USB_MODE_DEVICE;
        Usb_device_task_initialize ();
    }
    else
    {
        ucGUsbMode = USB_MODE_HOST;
        // REQUIRED !!!
        // Startup with ID=0, Ack ID pin transistion (default hwd start up is device mode)
        Usb_id_transition_ack ();
        Usb_id_interrupt_enable ();
        Enable_interrupt ();
        Usb_host_task_initialize ();
    }
    ucGOldUsbMode = ucGUsbMode;   // Store current usb mode, for mode change detection
// -----------------------------------------------------------------------------

// ---- DEVICE ONLY USB MODE ---------------------------------------------------
#elif ((USB_DEVICE_FEATURE == ENABLE) && (USB_HOST_FEATURE == DISABLE))
    Usb_device_mode_force ();
    Usb_device_task_initialize ();
// -----------------------------------------------------------------------------

// ---- REDUCED HOST ONLY USB MODE ---------------------------------------------
#elif ((USB_DEVICE_FEATURE == DISABLE) && (USB_HOST_FEATURE == ENABLE))
    Usb_host_mode_force ();
    Usb_host_task_initialize ();
#elif ((USB_DEVICE_FEATURE == DISABLE) && (USB_HOST_FEATURE == DISABLE))
    #error  at least one of USB_DEVICE_FEATURE or USB_HOST_FEATURE should be enabled
#endif
// -----------------------------------------------------------------------------
}


/** \brief Entry point of the USB mamnagement
 *
 *  Depending on the USB mode supported (HOST/DEVICE/DUAL_ROLE) the function
 *  calls the coresponding usb management function.
 */
void Usb_task ( void )
{
// ---- DUAL ROLE DEVICE USB MODE ---------------------------------------------
#if ((USB_DEVICE_FEATURE == ENABLE) && (USB_HOST_FEATURE == ENABLE))
    if (Is_usb_id_device ())
    {
        ucGUsbMode = USB_MODE_DEVICE;
    }
    else
    {
        ucGUsbMode = USB_MODE_HOST;
    }
    // TODO !!! ID pin hot state change
    // Preliminary management: HARDWARE RESET !!!
#if ( ID_PIN_CHANGE_GENERATE_RESET == ENABLE)
    // Hot ID transition generates wdt reset
    if ((ucGOldUsbMode! = ucGUsbMode))
#ifndef  __GNUC__
    {
        Wdt_change_16ms ();
        while (1);
        LOG_STR_CODE ( log_id_change );
    }
#else
    {
        Wdt_change_enable ();
        while (1);
        LOG_STR_CODE ( log_id_change );
    }
#endif

#endif
    ucGOldUsbMode = ucGUsbMode;   // Store current usb mode, for mode change detection
    // Depending on current usb mode, launch the correct usb task (device or host)
    switch (ucGUsbMode)
    {
        case USB_MODE_DEVICE:
        Usb_device_task ();
        break;
    case USB_MODE_HOST:
        Usb_host_task ();
        break;
    case USB_MODE_UNDEFINED:  // No break !
        default:
        break;
    }
// -----------------------------------------------------------------------------

// ---- DEVICE ONLY USB MODE ---------------------------------------------------
#elif ((USB_DEVICE_FEATURE == ENABLE) && (USB_HOST_FEATURE == DISABLE))
    Usb_device_task ();
// -----------------------------------------------------------------------------

// ---- REDUCED HOST ONLY USB MODE ---------------------------------------------
#elif ((USB_DEVICE_FEATURE == DISABLE) && (USB_HOST_FEATURE == ENABLE))
    Usb_host_task ();
// -----------------------------------------------------------------------------

//! ---- ERROR, NO MODE ENABLED -------------------------------------------------
#elif ((USB_DEVICE_FEATURE == DISABLE) && (USB_HOST_FEATURE == DISABLE))
    #error  at least one of USB_DEVICE_FEATURE or USB_HOST_FEATURE should be enabled
    #error  otherwise the usb task has nothing to do ...
#endif
// -----------------------------------------------------------------------------
}


/** \brief USB interrupt subroutine
 *
 *  This function is called each time a USB interrupt occurs.
 *  The following USB DEVICE events are taken in charge:
 *  - VBus On / Off
 *  - Start Of Frame
 *  - Suspend
 *  - Wake-Up
 *  - Resume
 *  - Reset
 *  - Start of frame
 *
 *  The following USB HOST events are taken in charge:
 *  - Device connection
 *  - Device Disconnection
 *  - Start Of Frame
 *  - ID pin change
 *  - SOF (or Keep alive in low speed) sent
 *  - Wake up on USB line detected
 *
 *  For each event, the user can launch an action by completing
 *  the associate define (See conf_usb.h file to add action upon events)
 *
 *  Note: Only interrupts events that are enabled are processed
 */
#ifdef __GNUC__
ISR ( USB_GEN_vect )
#else
#pragma vector = USB_GENERAL_vect
__interrupt void Usb_general_interrupt ()
#endif
{
#if (USB_HOST_PIPE_INTERRUPT_TRANSFER == ENABLE)
    uchar i;
    uchar ucSavePipeNb;
#endif
// ---------- DEVICE events management -----------------------------------
#if (USB_DEVICE_FEATURE == ENABLE)
    //- VBUS state detection
    if (Is_usb_vbus_transition () && Is_usb_vbus_interrupt_enabled ())
    {
        Usb_vbus_transition_ack ();
        if (Is_usb_vbus_high ())
        {
            ucUsbConnected = TRUE;
            Usb_vbus_on_action ();
            Usb_event_send ( EVT_USB_POWERED );
            Usb_reset_interrupt_enable ();
            Usb_device_start ();
            Usb_attach ();
        }
        else
        {
             Usb_vbus_off_action ();
             ucUsbConnected = FALSE;
             ucUsbConfigurationNb = 0;
             Usb_event_send ( EVT_USB_UNPOWERED );
        }
    }
    // - Device start of frame received
    if (Is_usb_sof () && Is_sof_interrupt_enabled ())
    {
        Usb_sof_ack ();
        Usb_sof_action ();
    }
    // - Device Suspend event (no more USB activity detected)
    if (Is_usb_suspend () && Is_suspend_interrupt_enabled ())
    {
        Usb_suspend_ack ();
        Usb_wake_up_interrupt_enable ();
        Usb_wake_up_ack ();                 // clear wake up to detect next event
        Usb_clock_freeze ();
        Usb_event_send ( EVT_USB_SUSPEND );
        Usb_suspend_action ();
    }
    // - Wake up event (USB activity detected): Used to resume
    if (Is_usb_wake_up () && Is_swake_up_interrupt_enabled ())
    {
        Usb_clock_unfreeze ();
        Usb_wake_up_ack ();
        Usb_wake_up_interrupt_disable ();
        Usb_wake_up_action ();
        Usb_event_send ( EVT_USB_WAKE_UP );
    }
    // - Resume state bus detection
    if (Is_usb_resume() && Is_resume_interrupt_enabled ())
    {
        Usb_wake_up_interrupt_disable ();
        Usb_resume_ack ();
        Usb_resume_interrupt_disable ();
        Usb_resume_action ();
        Usb_event_send ( EVT_USB_RESUME );
    }
    // - USB bus reset detection
    if (Is_usb_reset () && Is_reset_interrupt_enabled ())
    {
        Usb_reset_ack ();
        Usb_device_initialize ();
        Usb_reset_action ();
        Usb_event_send ( EVT_USB_RESET );
    }
#endif// End DEVICE FEATURE MODE

// ---------- HOST events management -----------------------------------
#if ((USB_HOST_FEATURE == ENABLE) && (USB_DEVICE_FEATURE == ENABLE))
    // - ID pin change detection
    if (Is_usb_id_transition () && Is_usb_id_interrupt_enabled ())
    {
        if (Is_usb_id_device ())
        {
            ucGUsbMode = USB_MODE_DEVICE;
        }
        else
        {
            ucGUsbMode = USB_MODE_HOST;
        }
        Usb_id_transition_ack ();
        if (ucGUsbMode != ucGOldUsbMode) // Basic Debounce
        {
            if (Is_usb_id_device ()) // Going to device mode
            {
                Usb_event_send ( EVT_USB_DEVICE_FUNCTION );
            }
            else                   // Going to host mode
            {
                Usb_event_send ( EVT_USB_HOST_FUNCTION );
            }
            Usb_id_transition_action ();
            LOG_STR_CODE ( log_id_change );
#if ( ID_PIN_CHANGE_GENERATE_RESET == ENABLE)
            // Hot ID transition generates wdt reset
#ifndef  __GNUC__
            Wdt_change_16ms ();
            while (1);
#else
            Wdt_change_enable ();
            while (1);
#endif
#endif
        }
    }
#endif
#if (USB_HOST_FEATURE == ENABLE)
    // - The device has been disconnected
    if (Is_device_disconnection () && \
        Is_host_device_disconnection_interrupt_enabled ())
    {
        Host_all_pipe_disable ();
        Host_device_disconnection_ack ();
        device_state = DEVICE_DISCONNECTED;
        Usb_event_send ( EVT_HOST_DISCONNECTION );
        LOG_STR_CODE ( log_device_disconnect );
        Host_device_disconnection_action ();
    }
    // - Device connection
    if (Is_device_connection () && Is_host_device_connection_interrupt_enabled ())
    {
        Host_device_connection_ack ();
        Host_all_pipe_disable ();
        Host_device_connection_action ();
    }
    // - Host Start of frame has been sent
    if (Is_host_sof () && Is_host_sof_interrupt_enabled ())
    {
        Host_sof_ack ();
        Usb_event_send ( EVT_HOST_SOF );
        ucPrivateSofCounter++;

        // delay timeout management for interrupt tranfer mode in host mode
#if ((USB_HOST_PIPE_INTERRUPT_TRANSFER == ENABLE) && (TIMEOUT_DELAY_ENABLE == ENABLE))
        if (ucPrivateSofCounter >= 250)   // Count 1/4 sec
        {
            ucPrivateSofCounter = 0;
            for (i = 0; i < MAX_EP_NB; i++)
            {
                if (it_pipe_str[i].enable == ENABLE)
                {
                    ucSavePipeNb = Host_selected_pipe_get ();
                    Host_pipe_select ( i );
                    it_pipe_str[i].timeout++;
                    if ((it_pipe_str[i].timeout > TIMEOUT_DELAY) && \
                        (Host_pipe_type_get () != TYPE_INTERRUPT))
                    {
                        it_pipe_str[i].enable = DISABLE;
                        it_pipe_str[i].status = PIPE_DELAY_TIMEOUT;
                        Host_stop_pipe_interrupt ( i );
                        if (is_any_interrupt_pipe_active () == FALSE)
                        // If no more transfer is armed
                        {
                            if (g_sav_int_sof_enable == FALSE)
                            {
                                Host_sof_interrupt_disable ();
                            }
                        }
                        it_pipe_str[i].handle ( PIPE_DELAY_TIMEOUT, \
                                                it_pipe_str[i].nb_byte_processed );
                    }
                    Host_pipe_select ( ucSavePipeNb );
                }
            }
        }
#endif  // (USB_HOST_PIPE_INTERRUPT_TRANSFER==ENABLE) && (TIMEOUT_DELAY_ENABLE==ENABLE))
        Host_sof_action ();
    }
    // - Host Wake-up has been received
    if (Is_host_hwup () && Is_host_hwup_interrupt_enabled ())
    {
        // Wake up interrupt should be disable host is now wake up !
        Host_hwup_interrupt_disable ();
        // CAUTION HWUP can be cleared only when USB clock is active (not frozen)!
        Pll_start_auto ();               // First Restart the PLL for USB operation
        Wait_pll_ready ();               // Get sure pll is lock
        Usb_clock_unfreeze ();           // Enable clock on USB interface
        Host_hwup_ack ();                // Clear HWUP interrupt flag
        Usb_event_send ( EVT_HOST_HWUP );  // Send software event
        Host_hwup_action ();             // Map custom action
    }
#endif // End HOST FEATURE MODE
}


