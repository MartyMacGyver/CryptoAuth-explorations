// ----------------------------------------------------------------------------
//         ATMEL Crypto-Devices Software Support  -  Colorado Springs, CO -
// ----------------------------------------------------------------------------
// DISCLAIMER:  THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
// DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
// OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// ----------------------------------------------------------------------------

/** \file
 *  \brief This file contains implementations of USB functions.
 *  \author Atmel Crypto Products
 *  \date May 6, 2013
 */

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdint.h>

#include "config.h"
#include "usb.h"
#include "usb_drv.h"
#include "usb_device_task.h"
#include "usb_standard_request.h"
#include "conf_usb.h"

#include "start_boot.h"
#include "hardware.h"
#include "delay_x.h"


extern uint8_t jump_bootloader;


/** \brief This function initializes USB. */
void Usb_Init()
{
	USB_TASK_INITIALIZE();
	Enable_interrupt();
}


/** \brief This function returns the enumeration status.
  * \return true if not enumerated, otherwise false
*/
uint8_t Usb_is_device_not_enumerated()
{
	return Is_device_not_enumerated();
}



/** \brief This function initializes USB and waits until it is enumerated. */
void Usb_WaitForEnumeration()
{
	Usb_Init();
	while (Is_device_not_enumerated())
		USB_DEVICE_TASK();
}


/** \brief This function reads new data into rx buffer.
 * \param[in] rxBuffer pointer to receive buffer
 * \return TRUE if packet received, otherwise FALSE
 * */
uint8_t Usb_CheckDataReceived(uint8_t *rxBuffer)
{
#ifdef AT88MICROBASE
	static uint8_t buttonIsPressed = FALSE;

	//	Check Hardware Boot button.
	if (bit_is_clear(PINA, 2)) {
	   buttonIsPressed = TRUE;
//	   Led1(buttonIsPressed);
	}
	else if (buttonIsPressed == TRUE) {
	   buttonIsPressed = FALSE;
//	   Led1(buttonIsPressed);
	}
#endif

	USB_DEVICE_TASK();

	// Return if USB HID is not enumerated.
	if (Is_device_not_enumerated())
		return FALSE;

	uint8_t i;

#ifndef NO_BOOTLOADER
	// Check if we received DFU mode command from host.
	if (jump_bootloader)
	{
		for (i = 0; i < 3; i++)
		{
			Led_On();
			_delay_ms(200);

			Led_Off();
			_delay_ms(200);
		}

		// Detach actual generic HID application
		Usb_detach();

		// Wait some time before jumping to the boot loader.
		_delay_ms(70);
		jump_bootloader = FALSE;

		// Jumping to boot loader
		start_boot();
	}
#endif

	USB_SELECT_ENDPOINT(EP_HID_OUT);
	if (!Is_usb_receive_out())
		return FALSE;

	// USB packet received
	for (i = 0; i < EP_LENGTH; i++)
		*rxBuffer++ = USB_READ_BYTE();

	USB_ACK_RECEIVE_OUT();
	return TRUE;
}


/** \brief This function sends data over USB.
 * 			It does not care whether the data are in binary or ASCII.
 * \param[in] length number of bytes to send
 * \param[in] buffer pointer to tx buffer
 */
void Usb_Send(uint16_t length, uint8_t *buffer)
{
	uint16_t i, j, offset = 0;
	uint8_t nUsbPackets = length / EP_LENGTH;

	if (length % EP_LENGTH)
		nUsbPackets++;

	USB_SELECT_ENDPOINT(EP_HID_IN);

	// ready to send information to host application
	if (Is_usb_write_enabled())
	{
		for (i = 0; i < nUsbPackets; i++)
		{
			for (j = 0; j < EP_LENGTH; j++, offset++)
				USB_WRITE_BYTE(buffer[offset]);

			USB_ACK_IN_READY();              // Send data over the USB

			while (!Is_usb_in_ready()) {}
      }
   }
}
