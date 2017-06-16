/** \file usb_specific_request.h
 *  \brief This file contains the user call-back functions corresponding to the
 *  application: MASS STORAGE DEVICE
 *
 *  Copyright (c) 2004 Atmel.
 * 
 *  RELEASE:      at90usb128-demo-hidgen-1_0_2
 *  REVISION:     1.1
 */

#ifndef _USB_USER_ENUM_H_
#define _USB_USER_ENUM_H_

/*_____ I N C L U D E S ____________________________________________________*/

#include "config.h"
#include "usb_descriptors.h"

/*_____ M A C R O S ________________________________________________________*/

extern  code S_usb_device_descriptor usb_dev_desc;
extern  code S_usb_user_configuration_descriptor usb_conf_desc;
extern  code S_usb_manufacturer_string_descriptor usb_user_manufacturer_string_descriptor;
extern  code S_usb_product_string_descriptor usb_user_product_string_descriptor;
extern  code S_usb_serial_number usb_user_serial_number;
extern  code S_usb_language_id usb_user_language_id;

/*_____ D E F I N I T I O N ________________________________________________*/
uchar  Usb_user_read_request ( uchar, uchar );
uchar  Usb_user_descriptor_get ( uchar , uchar );
void  Usb_user_endpoint_initialize ( uchar );
void Hid_report_get ( void );
void Hid_report_set ( void );
void Usb_hid_interface_get ( void );
void Usb_hid_idle_set ( void );
void Hid_hid_descriptor_get ( void );

//! Usb HID Report Descriptor
typedef struct 
{
    uchar ucReport[SIZE_OF_REPORT];
} S_usb_hid_report_descriptor;


#endif // _USB_USER_ENUM_H_

