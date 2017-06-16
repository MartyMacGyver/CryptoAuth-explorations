/** \file usb_standard_request.c
 *  \brief Process USB device enumeration requests.
 *
 *  Copyright (c) 2004 Atmel.
 *
 *  Use of this program is subject to Atmel's End User License Agreement.
 *  Please read file license.txt for copyright notice.
 *
 *  This file contains the USB endpoint 0 management routines corresponding to
 *  the standard enumeration process (refer to chapter 9 of the USB
 *  specification.
 *  This file calls routines of the usb_specific_request.c file for non-standard
 *  request management.
 *  The enumeration parameters (descriptor tables) are contained in the
 *  usb_descriptors.c file.
 *
 *  \version 1.4 at90usb128-demo-hidgen-1_0_2 $Id: usb_standard_request.c,v 1.4
 *  2006/06/14 16:59:02 rletendu Exp $
 */
//_____ I N C L U D E S ____________________________________________________
#include "usb_drv.h"
#include "usb_specific_request.h"
#include "usb_task.h"

//_____ M A C R O S ________________________________________________________

//_____ D E F I N I T I O N ________________________________________________

//_____ P R I V A T E   D E C L A R A T I O N ______________________________

static void Usb_descriptor_get ( void );
static void Usb_address_set ( void );
static void Usb_configuration_set ( void );
static void Usb_feature_clear ( void );
static void Usb_feature_set ( void );
static void Usb_status_get ( void );
static void Usb_configuration_get ( void );
static void Usb_interface_get ( void );
static void Usb_interface_set ( void );


//_____ D E C L A R A T I O N ______________________________________________

static uchar ucZlp;
static uchar  endpoint_status[NB_ENDPOINTS];

#ifdef __GNUC__
        PGM_VOID_P pbuffer;
#else
        uchar   code *pbuffer;
#endif
        uchar   ucDataToTransfer;

        ushort  usInterface;

static  uchar   ucRequestType;

        uchar   ucUsbConfigurationNb;
extern uchar  ucUsbConnected;
extern code S_usb_device_descriptor             usb_user_device_descriptor;
extern code S_usb_user_configuration_descriptor usb_user_configuration_descriptor;
extern code S_usb_device_descriptor usb_dev_desc ;
extern code S_usb_user_configuration_descriptor usb_conf_desc ;


/** Usb_process_request.
 *
 *  \brief This function reads the SETUP request sent to the default control endpoint
 *  and calls the appropriate function. When exiting of the usb_read_request
 *  function, the device is ready to manage the next request.
 *
 *  \note list of supported requests:
 *  GET_DESCRIPTOR
 *  GET_CONFIGURATION
 *  SET_ADDRESS
 *  SET_CONFIGURATION
 *  CLEAR_FEATURE
 *  SET_FEATURE
 *  GET_STATUS
 */
void Usb_process_request ( void )
{
    uchar  ucRequest;

    ucRequestType = Usb_byte_read ();
    ucRequest     = Usb_byte_read ();

// There were about a dozen requests from a Windows XP machine,
// the last one being SET_CONFIGURATION.

    switch ( ucRequest )
    {
    case GET_DESCRIPTOR:
        if (ucRequestType == 0x80)
        {
            Usb_descriptor_get ();
        }
        else
        {
            Usb_user_read_request ( ucRequestType, ucRequest );
        }
        break;

    case GET_CONFIGURATION:
        if (ucRequestType == 0x80)
        {
            Usb_configuration_get ();
        }
        else
        {
            Usb_user_read_request ( ucRequestType, ucRequest );
        }
        break;

    case SET_ADDRESS:
        if (ucRequestType == 0x00)
        {
            Usb_address_set ();
        }
        else
        {
            Usb_user_read_request ( ucRequestType, ucRequest );
        }
        break;

    case SET_CONFIGURATION:
        if (ucRequestType == 0x00)
        {
            Usb_configuration_set ();
        }
        else
        {
            Usb_user_read_request ( ucRequestType, ucRequest );
        }
        break;

    case CLEAR_FEATURE:
        if (ucRequestType < 0x02)
        {
            Usb_feature_clear ();
        }
        else
        {
            Usb_user_read_request ( ucRequestType, ucRequest );
        }
        break;

    case SET_FEATURE:
        if (ucRequestType < 0x02)
        {
            Usb_feature_set ();
        }
        else
        {
            Usb_user_read_request ( ucRequestType, ucRequest );
        }
        break;

    case GET_STATUS:
        if ((ucRequestType >= 0x7F) & (ucRequestType < 0x82))
        {
            Usb_status_get ();
        }
        else
        {
            Usb_user_read_request ( ucRequestType, ucRequest );
        }
        break;

    case GET_INTERFACE:
        if (ucRequestType == 0x81)
        {
            Usb_interface_get ();
        }
        else
        {
            Usb_user_read_request ( ucRequestType, ucRequest );
        }
        break;

    case SET_INTERFACE:
        if (ucRequestType == 0x01)
        {
            Usb_interface_set ();
        }
        break;

    case SET_DESCRIPTOR:
    case SYNCH_FRAME:
    default:    //!< un-supported request => call to user read request
        if(Usb_user_read_request ( ucRequestType, ucRequest ) == FALSE)
        {
            Usb_stall_handshake_enable ();
            Usb_receive_setup_ack ();
        }
        break;
    }
}


/** Usb_address_set.
 *
 *  This function manages the SET ADDRESS request. When complete, the device
 *  will filter the requests using the new address.
 *
 *  \warning Code:xx bytes (function code length)
 */
void Usb_address_set ( void )
{
    Usb_address_configure ( Usb_byte_read () );

    Usb_receive_setup_ack ();

    Usb_control_in_send ();                 //!< send a ZLP for STATUS phase
    while (!Is_usb_in_ready ());            //!< waits for status phase done
                                            //!< before using the new address
    Usb_address_enable ();
}


/** Usb_configuration_set.
 *
 *  This function manages the SET CONFIGURATION request. If the selected
 *  configuration is valid, this function call the Usb_user_endpoint_initialize()
 *  function that will configure the endpoints following the configuration
 *  number.
 *
 *  \warning Code:xx bytes (function code length)
 */
void Usb_configuration_set ( void )
{
    uchar configuration_number;

    configuration_number = Usb_byte_read ();

    if (configuration_number <= NB_CONFIGURATION)
    {
        Usb_receive_setup_ack ();
        ucUsbConfigurationNb = configuration_number;
    }
    else
    {
        //!< keep that order (set StallRq/clear RxSetup) or a
        //!< OUT request following the SETUP may be acknowledged
        Usb_stall_handshake_enable ();
        Usb_receive_setup_ack ();
        return;
    }

    Usb_control_in_send ();                    //!< send a ZLP for STATUS phase

    Usb_user_endpoint_initialize ( ucUsbConfigurationNb );  //!< endpoint configuration
    Usb_set_configuration_action ();
}


/** Usb_descriptor_get.
 *
 *  This function manages the GET DESCRIPTOR request. The device descriptor,
 *  the configuration descriptor and the device qualifier are supported. All
 *  other descriptors must be supported by the Usb_user_descriptor_get
 *  function.
 *  Only 1 configuration is supported.
 *
 *  \warning Code:xx bytes (function code length)
 */
void Usb_descriptor_get ( void )
{
    ushort  usLength;
    uchar ucDescriptorType;
    uchar ucStringType;
    uchar ucDummy;
    uchar ucNbByte;

    ucZlp             = FALSE;                  /* no zero length packet */
    ucStringType     = Usb_byte_read ();       /* read LSB of ucValue    */
    ucDescriptorType = Usb_byte_read ();       /* read MSB of ucValue    */

    switch (ucDescriptorType)
    {
    case DEVICE_DESCRIPTOR:
        //!< sizeof (usb_user_device_descriptor);
        ucDataToTransfer = Usb_get_dev_desc_length ();
        pbuffer          = Usb_get_dev_desc_pointer ();
        break;
    case CONFIGURATION_DESCRIPTOR:
        //!< sizeof (usb_user_configuration_descriptor);
        ucDataToTransfer = Usb_get_conf_desc_length ();
        pbuffer          = Usb_get_conf_desc_pointer ();
        break;
    default:
        if (Usb_user_descriptor_get ( ucDescriptorType, ucStringType ) == FALSE)
        {
            Usb_stall_handshake_enable ();
            Usb_receive_setup_ack ();
            return;
        }
        break;
    }

    ucDummy = Usb_byte_read ();                     //!< don't care of ucIndex field
    ucDummy = Usb_byte_read ();
    LSB ( usLength ) = Usb_byte_read ();              //!< read usLength
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
        while (ucDataToTransfer != 0)        //!< Send data until necessary
        {
            if (ucNbByte++ == EP_CONTROL_LENGTH) //!< Check endpoint 0 size
            {
                break;
            }
#ifndef __GNUC__
            Usb_byte_write ( *pbuffer++ );
#else       // AVRGCC does not support point to PGM space
            Usb_byte_write ( pgm_read_byte_near( (unsigned int) pbuffer++ ) );
#endif
            ucDataToTransfer--;
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
        while (!Is_usb_read_control_enabled ());
        Usb_control_in_send ();
    }

    while (!Is_usb_receive_out ());
    Usb_receive_out_ack ();
}


/** Usb_configuration_get.
 *
 *  This function manages the GET CONFIGURATION request. The current
 *  configuration number is returned.
 *
 *  \warning Code:xx bytes (function code length)
 */
void Usb_configuration_get ( void )
{
    Usb_receive_setup_ack ();

    Usb_byte_write ( ucUsbConfigurationNb );
    Usb_in_ready_ack ();

    while (!Is_usb_receive_out ());
    Usb_receive_out_ack ();
}

/** Usb_status_get.
 *
 *  This function manages the GET STATUS request. The device, interface or
 *  endpoint status is returned.
 *
 *  \warning Code:xx bytes (function code length)
 */
void Usb_status_get ( void )
{
    uchar ucIndex;
    uchar ucDummy;

    ucDummy  = Usb_byte_read ();                 //!< ucDummy read
    ucDummy  = Usb_byte_read ();                 //!< ucDummy read
    ucIndex = Usb_byte_read ();

    switch (ucRequestType)
    {
        case REQUEST_DEVICE_STATUS:
            Usb_receive_setup_ack ();
            Usb_byte_write ( DEVICE_STATUS );
            break;

        case REQUEST_INTERFACE_STATUS:
            Usb_receive_setup_ack ();
            Usb_byte_write ( INTERFACE_STATUS );
            break;

        case REQUEST_ENDPOINT_STATUS:
            Usb_receive_setup_ack ();
            ucIndex = ucIndex & MSK_EP_DIR;
            Usb_byte_write ( endpoint_status[ucIndex] );
            break;
        default:
            Usb_stall_handshake_enable ();
            Usb_receive_setup_ack ();
            return;
    }

    Usb_byte_write ( 0x00 );
    Usb_control_in_send ();

    while (!Is_usb_receive_out ());
    Usb_receive_out_ack ();
}


/** Usb_feature_set.
 *
 *  This function manages the SET FEATURE request. The USB test modes are
 *  supported by this function.
 *
 *  \warning Code:xx bytes (function code length)
 */
void Usb_feature_set ( void )
{
    uchar ucValue;
    uchar ucIndex;
    uchar ucDummy;

    if (ucRequestType == INTERFACE_TYPE)
    {
        //!< keep that order (set StallRq/clear RxSetup) or a
        //!< OUT request following the SETUP may be acknowledged
        Usb_stall_handshake_enable ();
        Usb_receive_setup_ack ();
        return;
    }
    else if (ucRequestType == ENDPOINT_TYPE)
    {
        ucValue = Usb_byte_read ();
        ucDummy  = Usb_byte_read ();                //!< ucDummy read

        if (ucValue == FEATURE_ENDPOINT_HALT)
        {
            ucIndex = (Usb_byte_read () & MSK_EP_DIR);

            if (ucIndex == EP_CONTROL)
            {
                Usb_stall_handshake_enable ();
                Usb_receive_setup_ack ();
                return;
            }

            Usb_endpoint_select ( ucIndex );
            if (Is_usb_endpoint_enabled ())
            {
                Usb_stall_handshake_enable ();
                Usb_endpoint_select ( EP_CONTROL );
                endpoint_status[ucIndex] = 0x01;
                Usb_receive_setup_ack ();
                Usb_control_in_send ();
            }
            else
            {
                Usb_endpoint_select ( EP_CONTROL );
                Usb_stall_handshake_enable ();
                Usb_receive_setup_ack ();
                return;
            }
        }
        else
        {
            Usb_stall_handshake_enable ();
            Usb_receive_setup_ack ();
            return;
        }
    }
}


/** Usb_feature_clear.
 *
 *  This function manages the SET FEATURE request.
 *
 *  \warning Code:xx bytes (function code length)
 */
void Usb_feature_clear ( void )
{
    uchar ucValue;
    uchar ucIndex;
    uchar ucDummy;

    if (ucRequestType == ZERO_TYPE)
    {
        //!< keep that order (set StallRq/clear RxSetup) or a
        //!< OUT request following the SETUP may be acknowledged
        Usb_stall_handshake_enable ();
        Usb_receive_setup_ack ();
        return;
    }
    else if (ucRequestType == INTERFACE_TYPE)
    {
        //!< keep that order (set StallRq/clear RxSetup) or a
        //!< OUT request following the SETUP may be acknowledged
        Usb_stall_handshake_enable ();
        Usb_receive_setup_ack ();
        return;
    }
    else if (ucRequestType == ENDPOINT_TYPE)
    {
        ucValue = Usb_byte_read ();
        ucDummy  = Usb_byte_read ();                //!< ucDummy read

        if (ucValue == FEATURE_ENDPOINT_HALT)
        {
            ucIndex = (Usb_byte_read () & MSK_EP_DIR);

            Usb_endpoint_select ( ucIndex );
            if (Is_usb_endpoint_enabled ())
            {
                if (ucIndex != EP_CONTROL)
                {
                    Usb_stall_handshake_disable ();
                    Usb_endpoint_reset ( ucIndex );
                    Usb_data_toggle_reset ();
                }
                Usb_endpoint_select ( EP_CONTROL );
                endpoint_status[ucIndex] = 0x00;
                Usb_receive_setup_ack ();
                Usb_control_in_send ();
            }
            else
            {
                Usb_endpoint_select ( EP_CONTROL );
                Usb_stall_handshake_enable ();
                Usb_receive_setup_ack ();
                return;
            }
        }
        else
        {
            Usb_stall_handshake_enable ();
            Usb_receive_setup_ack ();
            return;
        }
    }
}


/** Usb_interface_get.
 *
 *  This function manages the GET_INTERFACE request.
 *
 *  \warning Code:xx bytes (function code length)
 */
void Usb_interface_get ( void )
{
    Usb_stall_handshake_enable ();
    Usb_receive_setup_ack ();
}


/** Usb_interface_set.
 *
 *  This function manages the SET_INTERFACE request.
 *
 *  \warning Code:xx bytes (function code length)
 */
void Usb_interface_set ( void )
{
    Usb_receive_setup_ack ();
    Usb_control_in_send ();                    //!< send a ZLP for STATUS phase
    while (!Is_usb_in_ready ());
}
