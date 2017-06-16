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
 *  \brief 	This file contains definitions of hardware independent functions
 *          inside the Physical TWI layer of the SHA204 library.
 *  \date 	December 13, 2012
 */

#ifndef TWI_SHA204_PHYSICAL_H
#define TWI_SHA204_PHYSICAL_H

#include <stdint.h>
#include <stdbool.h>


/** \brief This structure is used to work around the insomnia bug.
*/
struct twi_SHAP_IdleState {
	bool idle;
	uint8_t address;
};


int8_t twi_SHAP_SetDeviceID(uint8_t address);
int8_t twi_SHAP_Init();
void twi_SHAP_SetSignalPin(uint8_t high);
int8_t twi_SHAP_SendBytes(uint8_t count, uint8_t *buffer);
int8_t twi_SHAP_SendByte(uint8_t byteValue);
int8_t twi_SHAP_ReceiveBytes(uint8_t count, uint8_t *buffer);


#endif
