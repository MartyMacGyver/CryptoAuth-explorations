/** \file usb_task.h
 *  \brief This file contains the function declarations
 *
 *  Copyright (c) 2006 Atmel.
 *
 *  Please read file license.txt for copyright notice.
 *
 *
 *  \version 1.11 at90usb128-demo-hidgen-1_0_2 $Id: Usb_task.h,v 1.11 
 *  2006/06/16 07:05:43 rletendu Exp $
 */

#ifndef _USB_TASK_H_
#define _USB_TASK_H_

//! \defgroup Usb_task USB task entry point module
//! \{

//_____ I N C L U D E S ____________________________________________________
#include "config.h"

//_____ M A C R O S ________________________________________________________
      //! \defgroup usb_software_evts USB software Events Management
      //! Macros to manage USB events detected under interrupt
      //! \{
#define Usb_event_send(x)               (usGUsbEvent |= (1 << x))
#define Usb_event_ack(x)                (usGUsbEvent &= ~(1 << x))
#define Usb_all_event_clear()           (usGUsbEvent = 0)
#define Is_usb_event(x)                 ((usGUsbEvent & (1 << x)) ? TRUE : FALSE)
#define Is_not_usb_event(x)             ((usGUsbEvent & (1 << x)) ? FALSE: TRUE)
#define Is_host_emergency_exit()        (Is_usb_event(EVT_HOST_DISCONNECTION) || \
                                         Is_usb_event(EVT_USB_DEVICE_FUNCTION))
#define Is_usb_device()                 (ucGUsbMode == USB_MODE_DEVICE ? TRUE : FALSE)
#define Is_usb_host()                   (ucGUsbMode == USB_MODE_HOST   ? TRUE : FALSE)

#define EVT_USB_POWERED                1         // USB plugged
#define EVT_USB_UNPOWERED              2         // USB un-plugged
#define EVT_USB_DEVICE_FUNCTION        3         // USB in device
#define EVT_USB_HOST_FUNCTION          4         // USB in host
#define EVT_USB_SUSPEND                5         // USB suspend
#define EVT_USB_WAKE_UP                6         // USB wake up
#define EVT_USB_RESUME                 7         // USB resume
#define EVT_USB_RESET                  8         // USB reset
#define EVT_HOST_SOF                   9         // Host start of frame sent
#define EVT_HOST_HWUP                  10        // Host wakeup detected
#define EVT_HOST_DISCONNECTION         11        // The target device is disconnected
      //! \}

//! \defgroup std_req_values Standard requests defines
//! \{
#define GET_STATUS                     0x00
#define GET_DEVICE                     0x01
#define CLEAR_FEATURE                  0x01           //!< see FEATURES below
#define GET_STRING                     0x03
#define SET_FEATURE                    0x03           //!< see FEATURES below
#define SET_ADDRESS                    0x05
#define GET_DESCRIPTOR                 0x06
#define SET_DESCRIPTOR                 0x07
#define GET_CONFIGURATION              0x08
#define SET_CONFIGURATION              0x09
#define GET_INTERFACE                  0x0A
#define SET_INTERFACE                  0x0B
#define SYNCH_FRAME                    0x0C

#define GET_DEVICE_DESCRIPTOR          1
#define GET_CONFIGURATION_DESCRIPTOR   4

#define REQUEST_DEVICE_STATUS          0x80
#define REQUEST_INTERFACE_STATUS       0x81
#define REQUEST_ENDPOINT_STATUS        0x82
#define ZERO_TYPE                      0x00
#define INTERFACE_TYPE                 0x01
#define ENDPOINT_TYPE                  0x02

                 // Descriptor Types
#define DEVICE_DESCRIPTOR                     0x01
#define CONFIGURATION_DESCRIPTOR              0x02
#define STRING_DESCRIPTOR                     0x03
#define INTERFACE_DESCRIPTOR                  0x04
#define ENDPOINT_DESCRIPTOR                   0x05
#define DEVICE_QUALIFIER_DESCRIPTOR           0x06
#define OTHER_SPEED_CONFIGURATION_DESCRIPTOR  0x07

                // Standard Features
#define FEATURE_DEVICE_REMOTE_WAKEUP   0x01
#define FEATURE_ENDPOINT_HALT          0x00

#define TEST_J                         0x01
#define TEST_K                         0x02
#define TEST_SEO_NAK                   0x03
#define TEST_PACKET                    0x04
#define TEST_FORCE_ENABLE              0x05

                 // Device Status
#define BUS_POWERED                    0
#define SELF_POWERED                   1

//! \}

#define USB_MODE_UNDEFINED             0x00
#define USB_MODE_HOST                  0x01
#define USB_MODE_DEVICE                0x02

//_____ D E C L A R A T I O N S ____________________________________________
extern volatile ushort usGUsbEvent;
extern uchar ucGUsbMode;

void Usb_task_initialize ( void );
void Usb_task ( void );

extern volatile uchar ucPrivateSofCounter;

//! \}

#endif // _USB_TASK_H_

