/** \file usb_drv.c
 *
 *  Copyright (c) 2006 Atmel.
 *
 *  Use of this program is subject to Atmel's End User License Agreement.
 *  Please read file license.txt for copyright notice.
 *
 *  \brief This file contains the USB driver routines.
 *
 *  This file contains the USB driver routines.
 *
 *  \version 1.11 at90usb128-demo-hidgen-1_0_2 $Id: usb_drv.c,v 1.11 2006/07/27 07:02:03 
 *   rletendu Exp $
 */

//_____ I N C L U D E S ____________________________________________________
#include "usb_drv.h"

//_____ M A C R O S ________________________________________________________

//_____ D E C L A R A T I O N ______________________________________________

/** \brief This function configures an endpoint with the selected type.
 *
 *  \param config0
 *  \param config1
 *  \return Is_endpoint_configured.
 */
uchar Usb_ep_config ( uchar config0, uchar config1 )
{
    Usb_endpoint_enable ();
    UECFG0X = config0;
    UECFG1X = (UECFG1X & (1 << ALLOC)) | config1;
    Usb_memory_allocate ();
    return (Is_endpoint_configured ());
}


/** \brief This function select the endpoint where an event occurs and returns the
 *  number of this endpoint. If no event occurs on the endpoints, this
 *  function returns 0.
 *
 *  \return endpoint number.
 */
uchar Usb_enpoint_interrupt_select ( void )
{
    uchar interrupt_flags;
    uchar ep_num;

    ep_num = 0;
    interrupt_flags = Usb_interrupt_flags ();

    while (ep_num < MAX_EP_NB)
    {
        if (interrupt_flags & 1)
        {
            return (ep_num);
        }
        else
        {
            ep_num++;
            interrupt_flags = interrupt_flags >> 1;
        }
    }
    return 0;
}


/** \brief This function moves the data pointed by tbuf to the selected endpoint fifo
 *  and sends it through the USB.
 *
 *  \param ep_num       number of the addressed endpoint
 *  \param *tbuf        address of the first data to send
 *  \param data_length  number of bytes to send
 *  \return address of the next uchar to send.
 *
 *  Example:\n
 *  Usb_packet_send(3,&first_data,0x20);    // send packet on the endpoint #3\n
 *  while(!(Usb_tx_complete));              // wait packet ACK'ed by the Host\n
 *  Usb_clear_tx_complete();                     // acknowledge the transmit\n
 *
 *  Note:\n
 *  tbuf is incremented of 'data_length'.\n
 */
uchar Usb_packet_send ( uchar ep_num, uchar* tbuf, uchar data_length )
{
    uchar remaining_length;

    remaining_length = data_length;
    Usb_endpoint_select ( ep_num );
    while (Is_usb_write_enabled () && (0 != remaining_length))
    {
        Usb_byte_write  (*tbuf );
        remaining_length--;
        tbuf++;
    }
    return remaining_length;
}


/** \brief This function moves the data stored in the selected endpoint fifo to
 *  the address specified by *rbuf.
 *
 *  \param ep_num       number of the addressed endpoint
 *  \param *rbuf        aaddress of the first data to write with the USB data
 *  \param data_length  number of bytes to read
 *  \return address of the next uchar to send.
 *
 *  Example:\n
 *  while(!(Usb_rx_complete));                      // wait new packet received\n
 *  Usb_packet_read(4,&first_data,usb_get_nb_byte); // read packet from ep 4\n
 *  Usb_clear_rx();                                 // acknowledge the transmit\n
 *
 *  Note:\n
 *  rbuf is incremented of 'data_length'.
 */
uchar Usb_packet_read ( uchar ep_num, uchar* rbuf, uchar  data_length )
{
    uchar remaining_length;

    remaining_length = data_length;
    Usb_endpoint_select ( ep_num );

    while (Is_usb_read_enabled () && (0 != remaining_length))
    {
        *rbuf = Usb_byte_read ();
        remaining_length--;
        rbuf++;
    }
    return remaining_length;
}


/** \brief This function sends a STALL handshake for the next Host request. A STALL
 *  handshake will be send for each next request untill a SETUP or a Clear Halt
 *  Feature occurs for this endpoint.
 *
 *  \param ep_num number of the addressed endpoint
 */
void Usb_endpoint_halt ( uchar ep_num )
{
    Usb_endpoint_select ( ep_num );
    Usb_stall_handshake_enable ();
}


/** \brief This function initializes the USB device controller and
 *  configures the Default Control Endpoint.
 *
 *  \return status
 */
uchar Usb_device_initialize ( void )
{
    Usb_device_select ();
    if (Is_usb_id_device ())
    {
      Usb_endpoint_select ( EP_CONTROL) ;
      if (!Is_usb_endpoint_enabled())
      {
#if (USB_LOW_SPEED_DEVICE == DISABLE)
         return (Usb_endpoint_configure(EP_CONTROL,    \
                                TYPE_CONTROL,  \
                                DIRECTION_OUT, \
                                SIZE_64,       \
                                ONE_BANK,      \
                                NYET_DISABLED));
#else
         return (Usb_endpoint_configure(EP_CONTROL,    \
                                TYPE_CONTROL,  \
                                DIRECTION_OUT, \
                                SIZE_8,       \
                                ONE_BANK,      \
                                NYET_DISABLED));
#endif
        }
    }
    return (FALSE);
}


