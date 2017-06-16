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
 *  \date June 16, 2011
 */

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdint.h>

#include "usb.h"
#include "lib_mcu/usb/usb_drv.h"
#include "lib_mcu/power/power_drv.h"
#include "lib_mcu/wdt/wdt_drv.h"
#include "lib_mcu/util/start_boot.h"
//#include "modules/usb/device_chap9/usb_device_task.h"
#include "modules/usb/device_chap9/usb_standard_request.h"
#include "modules/usb/usb_task.h"
#include "conf_usb.h"
#include "uart_usb_lib.h"
#include "config.h"
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


/** \brief Initializes USB and waits until it is enumerated. */
void Usb_WaitForEnumeration()
{
	USB_TASK_INITIALIZE();
	Enable_interrupt();
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
	usb_device_task();

	if (!uart_usb_test_hit())
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

		Usb_detach();

		//uint32_t volatile tempo;
		//for (tempo = 0; tempo < 70000; tempo++);  // Wait some time before
      _delay_ms(70);

		jump_bootloader = 0;

		start_boot(); // Jumping to boot loader
	}
#endif

	*rxBuffer = (uint8_t) uart_usb_getchar();
	return TRUE;
}


/** \brief This function sends data over USB.
 * 			It does not care whether the data are in binary or ASCII.
 * \param[in] length number of bytes to send
 * \param[in] buffer pointer to tx buffer
 */
void Usb_Send(uint16_t length, uint8_t *buffer)
{
	uart_usb_send_buffer(buffer, length);
}
