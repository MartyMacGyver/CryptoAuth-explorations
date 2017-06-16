/** \file usb_descriptors.h
 *  \brief This file contains the usb parameters that uniquely identify the
 *         application through descriptor tables.
 *
 *  Copyright (c) 2004 Atmel.
 *
 *  Use of this program is subject to Atmel's End User License Agreement.
 *  Please read file license.txt for copyright notice.
 *
 *  \brief HID generic Identifers.
 *
 *  \version 1.3 at90usb128-demo-hidgen-1_0_2 $Id: usb_descriptors.h,v 1.3
 *  2006/03/17 13:06:35 rletendu Exp $
 */

#ifndef _USB_DESCRIPTORS_H_
#define _USB_DESCRIPTORS_H_

//_____ I N C L U D E S ____________________________________________________

#include "config.h"
#include "conf_usb.h"

// This is not defined in Aris+, Keen+, and Bamboo projects.
#ifndef EP_LENGTH
	#define EP_LENGTH	(8)
//	#define EP_LENGTH	(64)
#endif

//_____ M A C R O S ________________________________________________________

#define Usb_get_dev_desc_pointer()        (&(usb_dev_desc.bLength))
#define Usb_get_dev_desc_length()         (sizeof (usb_dev_desc))
#define Usb_get_conf_desc_pointer()       (&(usb_conf_desc.cfg.bLength))
#define Usb_get_conf_desc_length()        (sizeof (usb_conf_desc))

//_____ U S B    D E F I N E _______________________________________________

    // USB Device descriptor
#define USB_SPECIFICATION     0x0200
#define DEVICE_CLASS          0      // each configuration has its own class
#define DEVICE_SUB_CLASS      0      // each configuration has its own sub-class
#define DEVICE_PROTOCOL       0      // each configuration has its own protocol
#if (USB_LOW_SPEED_DEVICE == ENABLE)
    #define EP_CONTROL_LENGTH     8
#else
    #define EP_CONTROL_LENGTH     64
#endif
#define VENDOR_ID             0x03EB //! Atmel vendor ID = 03EBh
#define RELEASE_NUMBER        0x0100
#define MAN_INDEX             0x01
#define PROD_INDEX            0x02
#define SN_INDEX              0x03
#define NB_CONFIGURATION      1

    // HID generic CONFIGURATION
#define NB_INTERFACE          1
#define CONF_NB               1
#define CONF_INDEX            0
#define CONF_ATTRIBUTES       USB_CONFIG_BUSPOWERED
#define MAX_POWER             250     // 500 mA

    // USB Interface descriptor
#define INTERFACE_NB          0
#define ALTERNATE             0
#define NB_ENDPOINT           2
#define INTERFACE_CLASS       0x03    //!< HID Class
#define INTERFACE_SUB_CLASS   0x00
#define INTERFACE_PROTOCOL    0x00
#define INTERFACE_INDEX       0

    // USB Endpoint 1 descriptor FS
#define ENDPOINT_NB_1         (EP_HID_IN | 0x80)
#define EP_ATTRIBUTES_1       0x03          //!< BULK = 0x02, INTERUPT = 0x03
#define EP_IN_LENGTH          EP_LENGTH
#define EP_SIZE_1             EP_IN_LENGTH
#define EP_INTERVAL_1         4 //!< Interrupt polling from host

             // USB Endpoint 2 descriptor FS
#define ENDPOINT_NB_2         (EP_HID_OUT)
#define EP_ATTRIBUTES_2       0x03          //!< BULK = 0x02, INTERUPT = 0x03
#define EP_OUT_LENGTH         EP_LENGTH
#define EP_SIZE_2             EP_OUT_LENGTH
#define EP_INTERVAL_2         20 //!< interrupt polling from host

#define SIZE_OF_REPORT        0x35
#define LENGTH_OF_REPORT_IN   EP_LENGTH
#define LENGTH_OF_REPORT_OUT  EP_LENGTH

#define DEVICE_STATUS         0x00 // TBD
#define INTERFACE_STATUS      0x00 // TBD

#define LANG_ID               0x00

/* HID specific */
#define HID                   0x21
#define REPORT                0x22
#define SET_REPORT            0x02

#define HID_DESCRIPTOR        0x21
//#define HID_BDC               0x1001
#define HID_COUNTRY_CODE      0x00
#define HID_CLASS_DESC_NB     0x01
#define HID_DESCRIPTOR_TYPE   0x22


#define USB_MN_LENGTH         5
#define USB_MANUFACTURER_NAME \
{ Usb_unicode('A') \
, Usb_unicode('T') \
, Usb_unicode('M') \
, Usb_unicode('E') \
, Usb_unicode('L') \
}

#define USB_PN_LENGTH         18
#define USB_PRODUCT_NAME \
{ Usb_unicode('C') \
 ,Usb_unicode('r') \
 ,Usb_unicode('y') \
 ,Usb_unicode('p') \
 ,Usb_unicode('t') \
 ,Usb_unicode('o') \
 ,Usb_unicode(' ') \
 ,Usb_unicode('M') \
 ,Usb_unicode('e') \
 ,Usb_unicode('m') \
 ,Usb_unicode('o') \
 ,Usb_unicode('r') \
 ,Usb_unicode('y') \
 ,Usb_unicode(' ') \
 ,Usb_unicode('D') \
 ,Usb_unicode('e') \
 ,Usb_unicode('m') \
 ,Usb_unicode('o') \
}

#define USB_SN_LENGTH         0x00
#define USB_SERIAL_NUMBER \
{}

#define LANGUAGE_ID           0x0409

    //! Usb Request
typedef struct
{
    uchar      ucRequestType;        //!< Characteristics of the request
    uchar      bRequest;             //!< Specific request
    ushort     wValue;               //!< field that varies according to request
    ushort     wIndex;               //!< field that varies according to request
    ushort     wLength;              //!< Number of bytes to transfer if Data
}  S_UsbRequest;

    //! Usb Device Descriptor
typedef struct
{
    uchar      bLength;              //!< Size of this descriptor in bytes
    uchar      bDescriptorType;      //!< DEVICE descriptor type
    ushort     bscUSB;               //!< Binay Coded Decimal Spec. release
    uchar      bDeviceClass;         //!< Class code assigned by the USB
    uchar      bDeviceSubClass;      //!< Sub-class code assigned by the USB
    uchar      bDeviceProtocol;      //!< Protocol code assigned by the USB
    uchar      bMaxPacketSize0;      //!< Max packet size for EP0
    ushort     idVendor;             //!< Vendor ID. ATMEL = 0x03EB
    ushort     idProduct;            //!< Product ID assigned by the manufacturer
    ushort     bcdDevice;            //!< Device release number
    uchar      iManufacturer;        //!< Index of manu. string descriptor
    uchar      iProduct;             //!< Index of prod. string descriptor
    uchar      iSerialNumber;        //!< Index of S.N.  string descriptor
    uchar      bNumConfigurations;   //!< Number of possible configurations
}  S_usb_device_descriptor;


    //! Usb Configuration Descriptor
typedef struct
{
    uchar      bLength;              //!< size of this descriptor in bytes
    uchar      bDescriptorType;      //!< CONFIGURATION descriptor type
    ushort     wTotalLength;         //!< total length of data returned
    uchar      bNumInterfaces;       //!< number of interfaces for this conf.
    uchar      bConfigurationValue;  //!< value for SetConfiguration resquest
    uchar      iConfiguration;       //!< index of string descriptor
    uchar      bmAttibutes;          //!< Configuration characteristics
    uchar      MaxPower;             //!< maximum power consumption
}  S_usb_configuration_descriptor;


    //! Usb Interface Descriptor
typedef struct
{
    uchar      bLength;               //!< size of this descriptor in bytes
    uchar      bDescriptorType;       //!< INTERFACE descriptor type
    uchar      bInterfaceNumber;      //!< Number of interface
    uchar      bAlternateSetting;     //!< value to select alternate setting
    uchar      bNumEndpoints;         //!< Number of EP except EP 0
    uchar      bInterfaceClass;       //!< Class code assigned by the USB
    uchar      bInterfaceSubClass;    //!< Sub-class code assigned by the USB
    uchar      bInterfaceProtocol;    //!< Protocol code assigned by the USB
    uchar      iInterface;            //!< Index of string descriptor
}  S_usb_interface_descriptor;


               //! Usb Endpoint Descriptor
typedef struct
{
    uchar      bLength;               //!< Size of this descriptor in bytes
    uchar      bDescriptorType;       //!< ENDPOINT descriptor type
    uchar      bEndpointAddress;      //!< Address of the endpoint
    uchar      bmAttributes;          //!< Endpoint's attributes
    ushort     wMaxPacketSize;        //!< Maximum packet size for this EP
    uchar      bInterval;             //!< Interval for polling EP in ms
} S_usb_endpoint_descriptor;


    //! Usb Device Qualifier Descriptor
typedef struct
{
    uchar      bLength;               //!< Size of this descriptor in bytes
    uchar      bDescriptorType;       //!< Device Qualifier descriptor type
    ushort     bscUSB;                //!< Binay Coded Decimal Spec. release
    uchar      bDeviceClass;          //!< Class code assigned by the USB
    uchar      bDeviceSubClass;       //!< Sub-class code assigned by the USB
    uchar      bDeviceProtocol;       //!< Protocol code assigned by the USB
    uchar      bMaxPacketSize0;       //!< Max packet size for EP0
    uchar      bNumConfigurations;    //!< Number of possible configurations
    uchar      bReserved;             //!< Reserved for future use, must be zero
}  S_usb_device_qualifier_descriptor;


    //! Usb Language Descriptor
typedef struct
{
    uchar      bLength;               //!< size of this descriptor in bytes
    uchar      bDescriptorType;       //!< STRING descriptor type
    ushort     wlangid;               //!< language id
}  S_usb_language_id;


//_____ U S B   M A N U F A C T U R E R   D E S C R I P T O R _______________


//! struct usb_st_manufacturer
typedef struct
{
    uchar  bLength;               //!< size of this descriptor in bytes
    uchar  bDescriptorType;       //!< STRING descriptor type
    ushort wstring[USB_MN_LENGTH];//!< unicode characters
} S_usb_manufacturer_string_descriptor;


//_____ U S B   P R O D U C T   D E S C R I P T O R _________________________


//!struct usb_st_product
typedef struct
{
    uchar  bLength;               //!< size of this descriptor in bytes
    uchar  bDescriptorType;       //!< STRING descriptor type
    ushort wstring[USB_PN_LENGTH];//!< unicode characters
} S_usb_product_string_descriptor;


//_____ U S B   S E R I A L   N U M B E R   D E S C R I P T O R _____________


//!< struct usb_st_serial_number
typedef struct
{
    uchar  bLength;               //!< size of this descriptor in bytes
    uchar  bDescriptorType;       //!< STRING descriptor type
    ushort wstring[USB_SN_LENGTH];//!< unicode characters
} S_usb_serial_number;


/*_____ U S B   H I D   D E S C R I P T O R __________________________________*/

typedef struct
{
    uchar  bLength;               //!< Size of this descriptor in bytes.
    uchar  bDescriptorType;       //!< HID descriptor type.
    ushort bscHID;                //!< Binary Coded Decimal Spec. release.
    uchar  bCountryCode;          //!< Hardware target country.
    uchar  bNumDescriptors;       //!< Number of HID class descriptors to follow.
    uchar  bRDescriptorType;      //!< Report descriptor type.
    ushort wDescriptorLength;     //!< Total length of Report descriptor.
} S_usb_hid_descriptor;

typedef struct
{
    S_usb_configuration_descriptor cfg;
    S_usb_interface_descriptor     ifc;
    S_usb_hid_descriptor           hid;
    S_usb_endpoint_descriptor      ep1;
    S_usb_endpoint_descriptor      ep2;
} S_usb_user_configuration_descriptor;


#endif

