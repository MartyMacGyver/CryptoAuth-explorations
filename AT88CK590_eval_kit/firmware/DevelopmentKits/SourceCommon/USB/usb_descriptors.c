/** \file usb_descriptors.c
 *  \brief USB parameters.
 *
 *  Copyright (c) 2004 Atmel.
 *
 *  Use of this program is subject to Atmel's End User License Agreement.
 *  Please read file license.txt for copyright notice.
 *
 *  This file contains the usb parameters that uniquely identify the
 *  application through descriptor tables.
 *
 *  \version 1.4 at90usb128-demo-hidgen-1_0_2 $Id: usb_descriptors.c,v 1.4
 *  2006/08/02 12:38:28 rletendu Exp $
 */


//_____ I N C L U D E S ____________________________________________________
#include "usb_drv.h"
#include "usb_specific_request.h"
#include "usb_standard_request.h"
#include "usb_task.h"
#include "usb_product_id.h"


//_____ M A C R O S ________________________________________________________

//_____ D E F I N I T I O N ________________________________________________

//! usb_user_device_descriptor
code S_usb_device_descriptor usb_dev_desc =
{
  sizeof(usb_dev_desc)
, DEVICE_DESCRIPTOR
, Usb_word_enum_struc_write(USB_SPECIFICATION)
, DEVICE_CLASS
, DEVICE_SUB_CLASS
, DEVICE_PROTOCOL
, EP_CONTROL_LENGTH
, Usb_word_enum_struc_write(VENDOR_ID)
, Usb_word_enum_struc_write(USB_PRODUCT_ID)
, Usb_word_enum_struc_write(RELEASE_NUMBER)
, MAN_INDEX
, PROD_INDEX
, 0
, NB_CONFIGURATION
};

//! usb_user_configuration_descriptor FS
code S_usb_user_configuration_descriptor usb_conf_desc =
{
 {
   sizeof(S_usb_configuration_descriptor)
 , CONFIGURATION_DESCRIPTOR
 , Usb_word_enum_struc_write(sizeof(S_usb_configuration_descriptor)\
   +sizeof(S_usb_interface_descriptor)+sizeof(S_usb_hid_descriptor)\
   +sizeof(S_usb_endpoint_descriptor)+sizeof(S_usb_endpoint_descriptor))
 , NB_INTERFACE
 , CONF_NB
 , CONF_INDEX
 , CONF_ATTRIBUTES
 , MAX_POWER
 }
 ,
 {
   sizeof(S_usb_interface_descriptor)
 , INTERFACE_DESCRIPTOR
 , INTERFACE_NB
 , ALTERNATE
 , NB_ENDPOINT
 , INTERFACE_CLASS
 , INTERFACE_SUB_CLASS
 , INTERFACE_PROTOCOL
 , INTERFACE_INDEX
 }
 ,
 {
   9
 , HID
 , 0x0111
 , 0
 , 1
 , REPORT
 , SIZE_OF_REPORT
 }
 ,
 {
   sizeof(S_usb_endpoint_descriptor)
 , ENDPOINT_DESCRIPTOR
 , ENDPOINT_NB_1
 , EP_ATTRIBUTES_1
 , Usb_word_enum_struc_write(EP_SIZE_1)
 , EP_INTERVAL_1
 }
 ,
 {
   sizeof(S_usb_endpoint_descriptor)
 , ENDPOINT_DESCRIPTOR
 , ENDPOINT_NB_2
 , EP_ATTRIBUTES_2
 , Usb_word_enum_struc_write(EP_SIZE_2)
 , EP_INTERVAL_2
 }
};


//! usb_user_manufacturer_string_descriptor
code S_usb_manufacturer_string_descriptor usb_user_manufacturer_string_descriptor =
{
  sizeof(usb_user_manufacturer_string_descriptor)
, STRING_DESCRIPTOR
, USB_MANUFACTURER_NAME
};

//! usb_user_product_string_descriptor
code S_usb_product_string_descriptor usb_user_product_string_descriptor =
{
  sizeof(usb_user_product_string_descriptor)
, STRING_DESCRIPTOR
, USB_PRODUCT_NAME
};

//! usb_user_serial_number
code S_usb_serial_number usb_user_serial_number =
{
  sizeof(usb_user_serial_number)
, STRING_DESCRIPTOR
, USB_SERIAL_NUMBER
};

//! usb_user_language_id
code S_usb_language_id usb_user_language_id =
{
  sizeof(usb_user_language_id)
, STRING_DESCRIPTOR
, Usb_word_enum_struc_write(LANGUAGE_ID)
};


//! usb_hid_report_descriptor
code S_usb_hid_report_descriptor usb_hid_report_descriptor =
{{
      0x06, 0xFF, 0xFF,         //!< 04|2   , Usage Page (vendordefined)
      0x09, 0x01,               //!< 08|1   , Usage      (vendordefined)
      0xA1, 0x01,               //!< A0|1   , Collection (Application)
      // IN report
      0x09, 0x02,               //!< 08|1   , Usage      (vendordefined)
      0x09, 0x03,               //!< 08|1   , Usage      (vendordefined)
      0x15, 0x00,               //!< 14|1   , Logical Minimum(0 for signed byte)
      0x26 ,0xFF,0x00,          //!< 24|1   , Logical Maximum(255 for signed byte)
      0x75, 0x08,               //!< 74|1   , Report Size(8) = field size in bits = 1 byte
      0x95, LENGTH_OF_REPORT_IN,   //!< 94|1   , ReportCount(size) = repeat count of previous item
      0x81, 0x02,               //!< 80|1   , IN report (Data,Variable, Absolute)
      // OUT report
      0x09, 0x04,               //!< 08|1   , Usage      (vendordefined)
      0x09, 0x05,               //!< 08|1   , Usage      (vendordefined)
      0x15, 0x00,               //!< 14|1   , Logical Minimum(0 for signed byte)
      0x26, 0xFF,0x00,          //!< 24|1   , Logical Maximum(255 for signed byte)
      0x75, 0x08,               //!< 74|1   , Report Size(8) = field size in bits = 1 byte
      0x95, LENGTH_OF_REPORT_OUT,   //!< 94|1   , ReportCount(size) = repeat count of previous item
      0x91, 0x02,               //!< 90|1   , OUT report (Data,Variable, Absolute)
      // Feature report
      0x09, 0x06,               //!< 08|1   , Usage      (vendordefined)
      0x09, 0x07,               //!< 08|1   , Usage      (vendordefined)
      0x15, 0x00,               //!< 14|1   , LogicalMinimum(0 for signed byte)
      0x26, 0xFF,0x00,          //!< 24|1   , Logical Maximum(255 for signed byte)
      0x75, 0x08,               //!< 74|1   , Report Size(8) =field size in bits = 1 byte
      0x95, 0x04,               //!< 94|1   , ReportCount
      0xB1, 0x02,               //!< B0|1   , Feature report
      0xC0                      //!< C0|0   , End Collection
}};



