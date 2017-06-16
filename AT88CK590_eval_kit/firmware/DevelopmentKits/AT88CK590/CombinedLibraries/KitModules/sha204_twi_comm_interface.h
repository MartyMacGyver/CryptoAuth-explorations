// ----------------------------------------------------------------------------
//         ATMEL Microcontroller Software Support  -  Colorado Springs, CO -
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
 *  \brief 	declarations, prototypes for communication layer of SHA204 library
 *  \author Atmel Crypto Products
 *  \date 	December 4, 2012
 */
#ifndef SHA_TWI_COMMINTERFACE_H
#define SHA_TWI_COMMINTERFACE_H

#include <stdint.h>     	// C99 standard typedefs
#include <setjmp.h>
#include <ctype.h>
#include <stdio.h>

int8_t twi_SHAP_SendCommand(uint8_t count, uint8_t *buffer);
int8_t twi_SHAP_ReceiveResponse(uint8_t count, uint8_t *buffer);
int8_t twi_SHAP_Idle(void);
int8_t twi_SHAP_Sleep(void);

#endif
