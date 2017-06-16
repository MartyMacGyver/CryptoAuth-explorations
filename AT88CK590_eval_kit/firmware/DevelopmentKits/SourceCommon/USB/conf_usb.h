/** \file conf_usb.h
 *  \brief This file contains the possible external configuration of the USB
 *
 *  Copyright (c) 2006 Atmel.
 *
 *  \version 1.1 at90usb128-demo-hidgen-1_0_2 $Id: conf_usb.h,v 1.1
 *  2006/03/17 13:06:36 rletendu Exp $
 */

#ifndef _CONF_USB_H_
#define _CONF_USB_H_

#include "config.h"

/** \defgroup usb_general_conf USB application configuration
 *
 *  \{
 */
    // _________________ USB MODE CONFIGURATION ____________________________

    /** \defgroup USB_op_mode USB operating modes configuration
     *  defines to enable device or host usb operating modes
     *  supported by the application
     *  \{
     */
    /** \brief ENABLE to activate the host software library support
     *
     *  Possible values ENABLE or DISABLE
     */
    #define USB_HOST_FEATURE            DISABLE

    /** \brief ENABLE to activate the device software library support
     *
     *  Possible values ENABLE or DISABLE
     */
    #define USB_DEVICE_FEATURE          ENABLE

    //! \}

    // _________________ USB REGULATOR CONFIGURATION _______________________

    /** \defgroup USB_reg_mode USB regulator configuration
     *  \{
     */

    /** \brief Enable the internal regulator for USB pads
     *
     *  When the application voltage is lower than 3.5V, to optimize power consumption
     *  the internal USB pads regulator can be disabled.\n
     *  Possible values ENABLE or DISABLE.
     */
#ifndef USE_USB_PADS_REGULATOR
#   define USE_USB_PADS_REGULATOR   ENABLE
#endif
    //! \}

    // _________________ HOST MODE CONFIGURATION ____________________________

    /** \defgroup USB_host_mode_cfg USB host operating mode configuration
     *  \{
     */

    #define HID_CLASS          0x03
    #define MS_CLASS           0x08

    /**   \brief VID/PID supported table list
     *
     *    This table contains the VID/PID that are supported by the reduced host application.\n
     *
     *    VID_PID_TABLE format definition:\n
     *    VID_PID_TABLE {VID1, number_of_pid_for_this_VID1, PID11_value, . . . ,\n
     *                   PID1X_Value, . . . , VIDz, number_of_pid_for_this_VIDz,\n
     *                   PIDz1_value, . . . , PIDzX_Value}
     */
    #define VID_PID_TABLE     {0x03EB, 2, 0x201C, 0x2014, 0x0123, 3, 0x2000, 0x2100, 0x1258}

    /**   \brief CLASS/SUBCLASS_PROTOCOL supported table list
     *
     *    This table contains the CLASS/SUBCLASS/PROTOCOL that is supported by the reduced host
     *    application.\n
     *    This table definition allows to extended the reduced application device support to an
     *    entire Class/subclass/protocol instead of a simple VID/PID table list.\n
     *
     *    CLASS_SUBCLASS_PROTOCOL format definition: \n
     *    CLASS_SUBCLASS_PROTOCOL {CLASS1, SUB_CLASS1,PROTOCOL1, \n
     *                              . . . , CLASSz, SUB_CLASSz, PROTOCOLz}
     */
    #define CLASS_SUBCLASS_PROTOCOL   {0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00}

    //! The size of RAM buffer reserved of descriptors manipulation
    #define SIZEOF_DATA_STAGE        250

    //! The address that will be assigned to the connected device
    #define DEVICE_ADDRESS           0x05

    /** The host controller will be limited to the strict VID/PID list.
     *  When enabled, if the device PID/VID does not belongs  to the supported list,
     *  the host controller library will not go to deeper configuration, but to error state.
     */
    #define HOST_STRICT_VID_PID_TABLE      DISABLE

    //! Try to configure the host pipe according to the device descriptors received
    #define HOST_AUTO_CFG_ENDPOINT         ENABLE

    //! Host start of frame interrupt always enable
    #define HOST_CONTINUOUS_SOF_INTERRUPT  DISABLE

    //! When Host error state detected, goto unattached state
    #define HOST_ERROR_RESTART             ENABLE

    //! Force WDT reset upon ID pin change
    #define ID_PIN_CHANGE_GENERATE_RESET   DISABLE

    //! NAK handshake in 1/4sec (250ms) before timeout
    #define NAK_TIMEOUT_DELAY              1

    #if (HOST_AUTO_CFG_ENDPOINT == FALSE)
      //! If no auto configuration of EP, map here user function
      #define        User_configure_endpoint()
    #endif

    /** \defgroup host_cst_actions USB host custom actions
     *
     *  \{
     */
    // write here the action to associate to each USB host event
    // be careful not to waste time in order not disturbing the functions
    #define Usb_id_transition_action()
    #define Host_device_disconnection_action()
    #define Host_device_connection_action()
    #define Host_sof_action()
    #define Host_suspend_action()
    #define Host_hwup_action()
    #define Host_device_not_supported_action()
    #define Host_device_supported_action()
    #define Host_device_error_action()
    //! \}

    //! \}

    // _________________ DEVICE MODE CONFIGURATION __________________________

    /** \defgroup USB_device_mode_cfg USB device operating mode configuration
     *
     *  \{
     */
    #define Usb_unicode(a)          ((ushort) (a))

    //! Number of endpoints in the application
    #define NB_ENDPOINTS          2
    #define EP_HID_IN             1
    #define EP_HID_OUT            2

    //! The USB device speed mode (default DISABLE to get full speed connection)
    #define USB_LOW_SPEED_DEVICE  DISABLE

    /** \defgroup device_cst_actions USB device custom actions
     *
     *  \{
     */
    // write here the action to associate to each USB event
    // be careful not to waste time in order not disturbing the functions
    #define Usb_sof_action()
    #define Usb_wake_up_action()
    #define Usb_resume_action()
    #define Usb_suspend_action()
    #define Usb_reset_action()
    #define Usb_vbus_on_action()
    #define Usb_vbus_off_action()
    #define Usb_set_configuration_action()
    //! \}

    extern void sof_action ( void );
    //! \}

//! \}

#endif // _CONF_USB_H_
