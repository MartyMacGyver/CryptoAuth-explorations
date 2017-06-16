/** \file usb_specific_request.c
 *  \brief user call-back functions
 *  Copyright (c) 2004 Atmel.
 *
 *  Use of this program is subject to Atmel's End User License Agreement.
 *  Please read file license.txt for copyright notice.
 *
 *  This file contains the user call-back functions corresponding to the
 *  application:
 *
 *  \version 1.2 at90usb128-demo-hidgen-1_0_2 $Id: usb_specific_request.c,v 1.2
 *  2006/08/02 12:38:28 rletendu Exp $

 *  \todo Merge with Rhino USB stack.
 */

//_____ I N C L U D E S ____________________________________________________
#include "usb_drv.h"
#include "usb_specific_request.h"
#include "usb_task.h"

//_____ M A C R O S ________________________________________________________

//_____ D E F I N I T I O N ________________________________________________
uint8_t jump_bootloader=0;

//_____ P R I V A T E   D E C L A R A T I O N ______________________________
#ifdef __GNUC__
extern PGM_VOID_P pbuffer;
#else
extern uchar   code *pbuffer;
#endif
extern uchar   ucDataToTransfer;
extern code S_usb_hid_report_descriptor usb_hid_report_descriptor;
extern ushort  usInterface;

void Hid_report_get ( void );
void Hid_hid_descriptor_get ( void );
void Hid_report_set ( void );
void Usb_hid_interface_get ( void );

//_____ D E C L A R A T I O N ______________________________________________

/** \brief Usb_user_read_request(ucType, ucRequest);
 *
 *  This function is called by the standard usb read request function when
 *  the Usb request is not supported. This function returns TRUE when the
 *  request is processed. This function returns FALSE if the request is not
 *  supported. In this case, a STALL handshake will be automatically
 *  sent by the standard usb read request function.
 *
 *  \param ucType
 *  \param ucRequest
 */
uchar Usb_user_read_request ( uchar ucType, uchar ucRequest )
{
    static uchar  ucDescriptorType;
    uchar  ucStringType;

    ucStringType     = Usb_byte_read ();
    ucDescriptorType = Usb_byte_read ();
    switch (ucRequest)
    {
    case GET_DESCRIPTOR:
        switch (ucDescriptorType)
        {
        case REPORT:
            Hid_report_get ();
            return TRUE;
            break;

        case HID:
            Hid_hid_descriptor_get ();
            return TRUE;
            break;

        default:
            return FALSE;
            break;
        }
        break;

    case SET_CONFIGURATION:
        switch (ucDescriptorType)
        {
        case SET_REPORT: 	//0x02
            Hid_report_set ();
            return TRUE;
            break;

        case 0x03: 	//0x03
            Hid_report_set ();
            return TRUE;
            break;

        default:
            return FALSE;
            break;
        }
        break;

    case GET_INTERFACE:
        Usb_hid_interface_get ();
        return TRUE;
        break;

    default:
        return FALSE;
        break;
    }
    return FALSE;
}


/** \brief This function configures the endpoints.
 *
 *  \param ucConfNb
 */
void Usb_user_endpoint_initialize ( uchar ucConfNb )
{
/*
	Usb_endpoint_configure( EP_HID_IN,      \
                         TYPE_INTERRUPT,     \
                         DIRECTION_IN,  \
                         SIZE_8,       \
                         ONE_BANK,     \
                         NYET_ENABLED );

    Usb_endpoint_configure( EP_HID_OUT,      \
                         TYPE_INTERRUPT,     \
                         DIRECTION_OUT,  \
                         SIZE_8,       \
                         ONE_BANK,     \
                         NYET_ENABLED );
*/

#if EP_LENGTH == 8
	#define EP_LENGTH_ENCODING	(SIZE_8)
#else
	#define EP_LENGTH_ENCODING	(SIZE_64)
#endif

	Usb_endpoint_configure(EP_HID_IN, TYPE_INTERRUPT, DIRECTION_IN,
							EP_LENGTH_ENCODING, ONE_BANK, NYET_ENABLED);

    Usb_endpoint_configure(EP_HID_OUT, TYPE_INTERRUPT, DIRECTION_OUT,
							EP_LENGTH_ENCODING, ONE_BANK, NYET_ENABLED);
}


/** \brief Usb_user_descriptor_get.
 *
 *  \param ucType
 *  \param ucString
 */
uchar Usb_user_descriptor_get ( uchar ucType, uchar ucString )
{
    switch (ucType)
    {
    case STRING_DESCRIPTOR:
        switch (ucString)
        {
        case LANG_ID:
            ucDataToTransfer = sizeof (usb_user_language_id);
            pbuffer = &(usb_user_language_id.bLength);
            return TRUE;
            break;
        case MAN_INDEX:
            ucDataToTransfer = sizeof (usb_user_manufacturer_string_descriptor);
            pbuffer = &(usb_user_manufacturer_string_descriptor.bLength);
            return TRUE;
            break;
        case PROD_INDEX:
            ucDataToTransfer = sizeof (usb_user_product_string_descriptor);
            pbuffer = &(usb_user_product_string_descriptor.bLength);
            return TRUE;
            break;
        case SN_INDEX:
            ucDataToTransfer = sizeof (usb_user_serial_number);
            pbuffer = &(usb_user_serial_number.bLength);
            return TRUE;
            break;
        default:
            return FALSE;
        }
    default:
    return FALSE;
    }

    return FALSE;
}


/** \brief This function manages hit get repport request.
 */
void Hid_report_get ( void )
{

    ushort usLength;
    uchar  ucNbByte;
    uchar ucZlp = 0;

    LSB ( usInterface ) = Usb_byte_read ();
    MSB ( usInterface ) = Usb_byte_read ();

    ucDataToTransfer = sizeof (usb_hid_report_descriptor);
    pbuffer = &(usb_hid_report_descriptor.ucReport[0]);

    LSB ( usLength ) = Usb_byte_read ();      //!< read usLength
    MSB ( usLength ) = Usb_byte_read ();
    Usb_receive_setup_ack () ;                  //!< clear the receive setup flag

    if (usLength > ucDataToTransfer)
    {
        if ((ucDataToTransfer % EP_CONTROL_LENGTH) == 0)
        {
            ucZlp = TRUE;
        }
        else
        {
            ucZlp = FALSE;
        }                   //!< no need of zero length packet
    }
    else
    {
        ucDataToTransfer = (uchar) usLength;         //!< send only requested number of data
    }

    while ((ucDataToTransfer != 0) && (!Is_usb_receive_out ()))
    {
        while (!Is_usb_read_control_enabled ());

        ucNbByte=0;
        while (ucDataToTransfer != 0)           //!< Send data until necessary
        {
            if (ucNbByte++ == EP_CONTROL_LENGTH) //!< Check endpoint 0 size
            {
                break;
            }
#ifndef __GNUC__
            Usb_byte_write ( *pbuffer++ );
#else    // AVRGCC does not support point to PGM space
            Usb_byte_write ( pgm_read_byte_near ( (unsigned int) pbuffer++ ) );
#endif
            ucDataToTransfer --;
        }
        Usb_control_in_send ();
    }

    Usb_control_in_send ();

    if (Is_usb_receive_out ())
    {
        Usb_receive_out_ack ();
        return;
    } //!< abort from Host
    if (ucZlp == TRUE)
    {
        Usb_control_in_send ();
    }

    while (!Is_usb_receive_out ());
    Usb_receive_out_ack ();
}


/** \brief This function manages hit set report request.
 */
void Hid_report_set ( void )
{

    Usb_receive_setup_ack ();
    Usb_control_in_send ();

   while(!Is_usb_receive_out());

   if(Usb_byte_read()==0x55)
      if(Usb_byte_read()==0xAA)
         if(Usb_byte_read()==0x55)
            if(Usb_byte_read()==0xAA)
            {
               jump_bootloader=1;
            }
   Usb_receive_out_ack ();
   Usb_control_in_send ();
   while(!Is_usb_in_ready());


}


/** \brief This function manages hid set idle request.
 */
void Usb_hid_idle_set ( void )
{
    uchar ucDummy;
    ucDummy = Usb_byte_read ();
    ucDummy = Usb_byte_read ();
    LSB ( usInterface ) = Usb_byte_read ();
    MSB ( usInterface ) = Usb_byte_read ();

    Usb_receive_setup_ack ();

    Usb_control_in_send ();                       /* send a ZLP for STATUS phase */
    while (!Is_usb_in_ready ());
}


/** \brief This function manages hid get interface request.
 */
void Usb_hid_interface_get ( void )
{
    uchar ucDummy;
    ucDummy = Usb_byte_read ();
    ucDummy = Usb_byte_read ();
    LSB ( usInterface ) = Usb_byte_read ();
    MSB ( usInterface ) = Usb_byte_read ();

    Usb_receive_setup_ack ();

    Usb_control_in_send ();                       /* send a ZLP for STATUS phase */
    while (!Is_usb_in_ready ());
}


/** \brief This function manages hid get hid descriptor request.
 */
void Hid_hid_descriptor_get ( void )
{
    ushort usLength;
    uchar  ucNbByte;
    uchar ucZlp = 0;

    LSB ( usInterface ) = Usb_byte_read ();
    MSB ( usInterface ) = Usb_byte_read ();

    ucDataToTransfer = sizeof (usb_conf_desc.hid);
    pbuffer = &(usb_conf_desc.hid.bLength);

    LSB ( usLength ) = Usb_byte_read ();      //!< read usLength
    MSB ( usLength ) = Usb_byte_read ();
    Usb_receive_setup_ack () ;                  //!< clear the receive setup flag

    if (usLength > ucDataToTransfer)
    {
        if ((ucDataToTransfer % EP_CONTROL_LENGTH) == 0)
        {
            ucZlp = TRUE;
        }
        else
        {
            ucZlp = FALSE;
        }                   //!< no need of zero length packet
    }
    else
    {
        ucDataToTransfer = (uchar) usLength;         //!< send only requested number of data
    }

    while ((ucDataToTransfer != 0) && (!Is_usb_receive_out ()))
    {
        while (!Is_usb_read_control_enabled ());

        ucNbByte = 0;
        while (ucDataToTransfer != 0)           //!< Send data until necessary
        {
            if (ucNbByte++ == EP_CONTROL_LENGTH) //!< Check endpoint 0 size
            {
                break;
            }
#ifndef __GNUC__
           Usb_byte_write ( *pbuffer++ );
#else    
			// AVRGCC does not support pointing to PGM space.
           Usb_byte_write ( pgm_read_byte_near((unsigned int) pbuffer++) );
#endif
           ucDataToTransfer --;
        }
        Usb_control_in_send ();
    }

    Usb_control_in_send ();

    if (Is_usb_receive_out ())
    {
        Usb_receive_out_ack ();
        return;
    } //!< abort from Host
    if (ucZlp == TRUE)
    {
        Usb_control_in_send ();
    }

    while (!Is_usb_receive_out ());
    Usb_receive_out_ack ();
}
