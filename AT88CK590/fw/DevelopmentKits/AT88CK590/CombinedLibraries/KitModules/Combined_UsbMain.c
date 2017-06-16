// ----------------------------------------------------------------------------
//         ATMEL Crypto_Devices Software Support  -  Colorado Springs, CO -
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
 * main Microbase module for combined libraries
 * \author Atmel Crypto Products
 * \date July 2, 2013
*/


/** \mainpage AES132 (SPI and I2C) and SHA204 (SWI and I2C) library with
 *            Microbase Firmware Documentation

* \section intro_sec Introduction

Microbase is a development kit for development of your CryptoMemory and CryptoAuth applications.

* \section contact_sec Contact Info
For more info about CryptoMemory visit http://www.atmel.com/products/SecureMem\n
Support mail: cryptomemory@atmel.com\n
*/

#include <stdint.h>              // basic type definitions
#include <string.h>              // string functions
#include <avr/io.h>              //FUSES definitions
//#include <start_boot.h>

#include "usb.h"                 // USB top-of-stack functions
#include "parserAscii.h"         // parser functions
#include "hardware.h"            // Microbase related functions (LED, buzzer, button)
#include "timers.h"              // AVR hardware timers
#include "cpu.h"                 // CPU functions
#include "config.h"              // USB definitions
#include "delay_x.h"             // AVR software delay functions
#include "Combined_Discover.h"   // device discovery functions
#include "sha204_comm.h"         // used to work around the insomnia bug
#include "sha204_twi_physical.h" // used to work around the insomnia bug


// If this macro is defined, the fuses will be programmed when programming the .elf image for this application.
//#define FUSE_SETTINGS
#ifdef FUSE_SETTINGS
FUSES =
{
	.low = 0xFF,
	.high = (FUSE_BOOTSZ1 & FUSE_SPIEN & FUSE_JTAGEN),
	.extended = (FUSE_BODLEVEL2 & FUSE_HWBE)
};
#endif

#define DISCOVERY_INTERVAL   (1000)   //!< interval between calls to device discovery

extern device_info_t device_info[];
extern uint8_t isDiscoveryEnabled;
extern struct twi_SHAP_IdleState sha204p_idle_state;
extern uint8_t device_address;


/** \brief main function
 *
 *  \return 0
 */
int main(void)
{
	
	CLKPR = 0x80;
	CLKPR = 0x00;

	uint16_t tx_length;
	uint8_t *tx_buffer;
	uint8_t *rxBuffer[1];
	static uint8_t heart_beat;

//#if (TARGET_BOARD==AT88RHINO_ECC108)
	wdtdrv_disable();
	start_boot_if_required();
//	clock_prescale_set(0);

//	start_boot_if_required();
//#endif
//
	// Compiler cannot handle "static uint8_t variable = 1;"
	heart_beat = 1;

	Led_Init();

	// Indicate success of boot process if not ECCSIGNER or ECCROOT
	Led_On();

    // Enable interrupts (also needed for timer) and initialize USB.
	Usb_Init();
	Timer_delay_ms(1000);
	
	// Indicate entering infinite loop.
	Led_Off();

	// Start discovery interval timer.
	timer_delay_ms_expired = TRUE;

	rxBuffer[0] = GetRxBuffer();

	// debug
//	uint8_t *rx_buffer_start = rxBuffer[0];

	// Enter main loop that processes USB host packets
	// and runs device discovery at intervals.
	while (1)
	{
		if (Usb_is_device_not_enumerated()) {
			// Enumerate USB if not enumerated yet.
			USB_DEVICE_TASK();
		}
		else
		{
			// Check whether a SHA204 device is in Idle state.
			// todo Replace this insomnia fix (sending and Idle command
			// every half second) with a better one. Translate Idle
			// command into the following sequence:
			// - Read selector byte.
			// - Send Pause command with selector parameter different
			//   from the previously read selector byte.
			if (sha204p_idle_state.idle) {
				// Check whether idle timer has expired.
				if (timer_delay_idle_expired) {
					// Send Idle command.
					uint8_t device_address_saved = device_address;
					device_address = sha204p_idle_state.address;
					sha204p_idle();
					device_address = device_address_saved;
					timer_delay_idle_expired = FALSE;
				}
			}
			// Check for incoming packets if USB is enumerated.
		    if (Usb_CheckDataReceived(rxBuffer[0])) {
			   // EP_LENGTH is defined in the USB stack (EP_LENGTH = 64) as well as in the 
			   // CDC stack (EP_LENGTH = 1).
			   // Although for both configurations, HID and CDC, only their respective
			   // folders are included (-I switch), the compiler might use the wrong config.h.
			   // Make therefore sure that the -I switches for the USB or USB_CDC stack
			   // are first in the list of -I switches. Also, make sure that the output
			   // folders for the binaries are different for USB and CDC.
			   if (CollateUsbPacket(EP_LENGTH, &rxBuffer[0])) {

					// debug
					//*(rxBuffer[0]) = 0; // Don't send new line.
					//Usb_Send(rxBuffer[0] - rx_buffer_start, rx_buffer_start);
					//*(rxBuffer[0]) = KIT_EOP; // Restore new line.

					// We received EOP. Process the packet.
					tx_buffer = ProcessUsbPacket(&tx_length);

					Usb_Send(tx_length, tx_buffer);
					rxBuffer[0] = ResetRxBuffer(0);
			   }
		   }
		}	

		if (!timer_delay_ms_expired)
			continue;

//		Led1(1);  //SHA204
//		Led2(1);  //AES132
//		Led3(1);  //ECC108
		heart_beat = !heart_beat;

		// Restart timer for heart beat. Make the heart beat slower if USB is not enumerated.
		Timer_delay_ms_without_blocking(Usb_is_device_not_enumerated() ? 2000 : DISCOVERY_INTERVAL);


		if (isDiscoveryEnabled)
			DiscoverDevices();

		// Indicate interface and device type found using LED2 and LED3.
		// If LED1 (heart_beat) is off, show interface type, otherwise show device type.
		// interface types: SWI = LED2, TWI = LED3, SPI = LED2 and LED3.
		// device types: SHA204 = LED2, AES132 = LED3.
		// This assumes that if two devices are found they are of the same type.
//		if (heart_beat) {
			// Indicate interface type.
//			if (device_info[0].bus_type == DEVKIT_IF_SWI)
//				Led2(1);
//			else if (device_info[0].bus_type == DEVKIT_IF_I2C)
//				Led3(1);
//			else if (device_info[0].bus_type == DEVKIT_IF_SPI) {
//				Led2(1);
//				Led3(1);
//			}
//		}
//		else {
			// Indicate device type.
			//if (device_info[0].device_type == DEVICE_TYPE_SHA204)
			//{
				//Led2(1);
				//Led3(0);
			//}
			//else if (device_info[0].device_type == DEVICE_TYPE_AES132)
			//{
				//Led2(0);
				//Led3(1);
			//}
			//else if (device_info[0].device_type == DEVICE_TYPE_ECC108) {
				//Led2(1);
				//Led3(1);
			//}
//		}
	}

	return 0;
}
