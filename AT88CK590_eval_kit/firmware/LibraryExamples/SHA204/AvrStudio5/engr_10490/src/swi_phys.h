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
 *  \brief  Definitions and Prototypes for SWI Hardware Dependent Physical Layer of SHA204 Library
 *  \author Atmel Crypto Products
 *  \date 	September 29, 2010
 *  \todo Move interface header files from "Hardware\AVR_AT" to "Hardware\Includes"
 *        so that they can be shared by other hardware implementations.
 */
#ifndef SWI_PHYS_H
#   define SWI_PHYS_H

#include <stdint.h>                                       // data type definitions

// error codes for physical hardware dependent module
// Codes in the range 0x00 to 0xF7 are shared between physical interfaces (SWI, TWI).
// Codes in the range 0xF8 to 0xFF are special for the particular interface.
#define SWI_FUNCTION_RETCODE_SUCCESS     ((uint8_t) 0x00) //!< Communication with device succeeded.
#define SWI_FUNCTION_RETCODE_TIMEOUT     ((uint8_t) 0xF1) //!< Communication timed out.
#define SWI_FUNCTION_RETCODE_RX_FAIL     ((uint8_t) 0xF9) //!< Communication failed after at least one byte was received.


// Function Prototypes
void    swi_enable(void);
void    swi_set_device_id(uint8_t id);
void    swi_set_signal_pin(uint8_t end);
uint8_t swi_send_bytes(uint8_t count, uint8_t *buffer);
uint8_t swi_send_byte(uint8_t value);
uint8_t swi_receive_bytes(uint8_t count, uint8_t *buffer);


#endif
