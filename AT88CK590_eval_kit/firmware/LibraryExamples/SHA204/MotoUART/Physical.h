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
 *  \author Nelson Lunsford
 *  \date 	Feb 25, 2010
 */
#ifndef PHYSICAL_H
#define PHYSICAL_H

#include <stdint.h>     	// C99 standard typedefs
#include <setjmp.h>
#include <ctype.h>
#include <stdio.h>

// Function Prototypes

int8_t SHAP_SetDeviceID(uint8_t address);
int8_t SHAP_Init(void);
void SHAP_SetSignalPin(uint8_t high);
int8_t SHAP_SendBytes(uint8_t count, uint8_t *buffer);
int8_t SHAP_ReceiveBytes(uint8_t count, uint8_t *buffer);
int8_t SHAP_OpenChannel(void);
int8_t SHAP_CloseChannel(void);



#endif
