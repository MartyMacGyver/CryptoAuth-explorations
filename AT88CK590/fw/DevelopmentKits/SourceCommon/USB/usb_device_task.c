/** \file usb_device_task.c
 *  \brief This file manages the USB device controller.
 *
 *  Copyright (c) 2004 Atmel.
 *
 *  Please read file license.txt for copyright notice.
 *
 *  The USB task checks the income of new requests from the USB Host.
 *  When a Setup request occurs, this task will launch the processing
 *  of this setup contained in the usb_standard_request.c file.
 *  Other class specific requests are also processed in this file.
 *
 *  \version 1.5 at90usb128-demo-hidgen-1_0_2 $Id: Usb_device_task.c,v 1.5
 *  2006/07/27 07:04:41 rletendu Exp $
 */

//_____  I N C L U D E S ___________________________________________________
#include "usb_drv.h"
#include "usb_standard_request.h"
#include "pll_drv.h"
#include "usb_task.h"
#include "conf_usb.h"


//_____ M A C R O S ________________________________________________________

//_____ D E F I N I T I O N S ______________________________________________

/**
 *  Public : (bit) ucUsbConnected
 *  ucUsbConnected is set to TRUE when VBUS has been detected
 *  ucUsbConnected is set to FALSE otherwise
 */
uchar  ucUsbConnected;

/**
 *  Public : (uchar) ucUsbConfigurationNb
 *  Store the number of the USB configuration used by the USB device
 *  when its value is different from zero, it means the device mode is enumerated
 *  Used with USB_DEVICE_FEATURE == ENABLE only
 */
extern uchar  ucUsbConfigurationNb;

//_____ D E C L A R A T I O N S ____________________________________________

/**
 *  \brief This function initializes the USB device controller and system interrupt
 *
 *  This function enables the USB controller and init the USB interrupts.
 *  The aim is to allow the USB connection detection in order to send
 *  the appropriate USB event to the operating mode manager.
 */
void Usb_device_task_initialize(void)
{
  Enable_interrupt();
  Usb_disable();
  Usb_enable();
  Usb_device_select();
#if (USB_LOW_SPEED_DEVICE == ENABLE)
  Usb_low_speed_mode ();
#endif
  Usb_vbus_interrupt_enable();
  Enable_interrupt();
}


/**
 *  \brief This function initializes the USB device controller
 *
 *  This function enables the USB controller and init the USB interrupts.
 *  The aim is to allow the USB connection detection in order to send
 *  the appropriate USB event to the operating mode manager.
 *  Start device function is executed once VBUS connection has been detected
 *  either by the VBUS change interrupt or by the VBUS high level.
 */
void Usb_device_start ( void )
{
    Pll_start_auto ();
    Wait_pll_ready ();
    Usb_clock_unfreeze ();
    Usb_reset_interrupt_enable ();
    Usb_device_initialize ();         // configure the USB controller EP0
    Usb_attach ();
}


/** \brief Entry point of the USB device management
 *
 *  This function is the entry point of the USB management. Each USB
 *  event is checked here in order to launch the appropriate action.
 *  If a Setup request occurs on the Default Control Endpoint,
 *  the Usb_process_request() function is called in the usb_standard_request.c file
 */
void Usb_device_task( void )
{
    if (Is_usb_vbus_high () && ucUsbConnected == FALSE)
    {
        ucUsbConnected = TRUE;
        Usb_device_start ();
        Usb_vbus_on_action ();
    }

    if(Is_usb_event ( EVT_USB_RESET ))
    {
        Usb_event_ack ( EVT_USB_RESET );
        Usb_endpoint_reset ( 0 );
        ucUsbConfigurationNb = 0;
    }

    // Here connection to the device enumeration process
    Usb_endpoint_select ( EP_CONTROL );
    if (Is_usb_receive_setup ())
    {
        Usb_process_request ();
    }
}
