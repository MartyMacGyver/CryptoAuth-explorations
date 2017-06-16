/** \file usb_device_task.h
 *  \brief This file contains the function declarations for USB device task
 *
 *  Copyright (c) 2004 Atmel.
 *
 *  Please read file license.txt for copyright notice.
 *
 *  \version 1.1 at90usb128-demo-hidgen-1_0_2 $Id: Usb_device_task.h,v 1.1 
 *  2006/03/13 10:09:12 rletendu Exp $
 */

#ifndef _Usb_device_task_H_
#define _Usb_device_task_H_

//! \defgroup Usb_device_task USB device task module
//! \{

//_____ I N C L U D E S ____________________________________________________
#include "config.h"

//_____ M A C R O S ________________________________________________________

//_____ D E C L A R A T I O N S ____________________________________________

void Usb_device_task_initialize ( void );
void Usb_device_start ( void );
void Usb_device_task ( void );

//! \}

#endif /* _Usb_device_task_H_ */

