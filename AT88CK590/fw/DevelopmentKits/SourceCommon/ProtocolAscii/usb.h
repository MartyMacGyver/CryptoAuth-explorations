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
 *  \date September 6, 2011
 */

#ifndef _USB_H_
#   define _USB_H_

#include "compiler.h"
#include "usb_device_task.h"

#define USB_SELECT_ENDPOINT(end_point)   Usb_select_endpoint(end_point)
#define USB_WRITE_BYTE(byte)             Usb_write_byte(byte)
#define USB_READ_BYTE()                  Usb_read_byte()
#define USB_ACK_RECEIVE_OUT()            Usb_ack_receive_out()
#define USB_ACK_IN_READY()               Usb_ack_in_ready()
#define USB_DEVICE_TASK()                usb_device_task()
#define USB_TASK_INITIALIZE()            usb_task_init()

void Usb_Init();
uint8_t Usb_is_device_not_enumerated();
void Usb_WaitForEnumeration();
uint8_t Usb_CheckDataReceived(uint8_t *rxBuffer);
void Usb_Send(uint16_t ucDataLength, uint8_t *pucDataBuffer);

#endif //_USB_H_
