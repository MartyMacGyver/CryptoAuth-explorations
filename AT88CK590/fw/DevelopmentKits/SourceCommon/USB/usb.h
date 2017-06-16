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
 *  \brief This file contains definitions for USB functions.
 *  \author Atmel Crypto Products
 *  \date April 12, 2011
 */

#ifndef _USB_H_
#define _USB_H_

#include <stdint.h>

/** \todo Rebuild Aris+ and Keen+. */
//#include "config.h"

//! USB send and receive data buffer size.
#ifdef __AVR_AT90USB162__
#   define USB_BUFFER_SIZE 128
#else
#   define USB_BUFFER_SIZE 255
#endif


//! USB packet state.
//enum usb_packet_state
//{
//  PACKET_STATE_IDLE,
//  PACKET_STATE_TAKE_DATA,
//  PACKET_STATE_END_OF_VALID_DATA,
//  PACKET_STATE_OVERFLOW
//};

/** \todo For Rhino, use the same buffer for rx and tx. */
//! USB receive data buffer.
//uint8_t pucUsbRxBuffer[USB_BUFFER_SIZE];
//! USB send data buffer.
//uint8_t pucUsbTxBuffer[USB_BUFFER_SIZE];

void Usb_WaitForEnumeration();
uint8_t Usb_CheckDataReceived(uint8_t *rxBuffer);
void Usb_Send(uint8_t ucDataLength, uint8_t* pucDataBuffer);

/** \todo Introduce return values. */
//void Usb_Initialize(void);
//void Usb_CheckPacketState(void);
//void Usb_CheckDataReceived(void);
//void Usb_ReceivePacket(void);
//void Usb_Send(uint8_t dataLength, uint8_t *dataBuffer);
//void ToggleLed1(void);

#endif //_USB_H_
