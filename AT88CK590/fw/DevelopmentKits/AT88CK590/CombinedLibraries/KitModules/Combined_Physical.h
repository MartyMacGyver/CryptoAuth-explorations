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
 *  \brief 	This file contains definitions of hardware independent wrapper
 *   functions of the Physical layer modules of libraries.
 *  \date 	December 14, 2012
 */

#ifndef COMBINED_PHYSICAL
#define COMBINED_PHYSICAL


#include <stdint.h>
#include "parserAscii.h"
#include "aes132_lib_return_codes.h"


typedef enum {
	DEVKIT_LIB_AES132,
	DEVKIT_LIB_SHA204
} lib_id_t;

#define ECC108_RESPONSE_SIZE_MAX       ((uint8_t) (72 + 3))   //!< maximum size of response packet


/** \todo Verify max size. Drop SHA204 from names. */
//#define SHA204_RSP_SIZE_MIN          (4)    //!< minimum number of bytes in response
//#define SHA204_RSP_SIZE_MAX          (35)   //!< maximum size of response packet
//
//#define SHA204_BUFFER_POS_COUNT      (0)             //!< buffer index of count byte in command or response
//#define SHA204_BUFFER_POS_DATA       (1)             //!< buffer index of data in response
//
//! width of Wakeup pulse in 10 us units
//! todo Can we change this to 12 instead of the compilation switch in sha204p_wakeup? */
//#define SHA204_WAKEUP_PULSE_WIDTH    (uint8_t) (6.0 * CPU_CLOCK_DEVIATION_POSITIVE + 0.5)

//! delay between Wakeup pulse and communication in ms
//#define SHA204_WAKEUP_DELAY          (uint8_t) (3.0 * CPU_CLOCK_DEVIATION_POSITIVE + 0.5)


uint8_t aes132p_set_interface(interface_id_t interface);
uint8_t sha204p_set_interface(interface_id_t interface);

// This function is not provided by the SHA204 library but is needed for kit firmware.
void sha204p_disable_interface(void);

uint8_t swi_sha204p_send_command(uint8_t count, uint8_t *command);
uint8_t swi_sha204p_receive_response(uint8_t size, uint8_t *response);
void    swi_sha204p_init(void);
void    swi_sha204p_set_device_id(uint8_t id);
uint8_t swi_sha204p_wakeup(void);
uint8_t swi_sha204p_idle(void);
uint8_t swi_sha204p_sleep(void);
uint8_t swi_sha204p_reset_io(void);
uint8_t swi_sha204p_resync(uint8_t size, uint8_t *response);

uint8_t twi_sha204p_send_command(uint8_t count, uint8_t *command);
uint8_t twi_sha204p_receive_response(uint8_t size, uint8_t *response);
void    twi_sha204p_init(void);
void    twi_sha204p_set_device_id(uint8_t id);
uint8_t twi_sha204p_wakeup(void);
uint8_t twi_sha204p_idle(void);
uint8_t twi_sha204p_sleep(void);
uint8_t twi_sha204p_reset_io(void);
uint8_t twi_sha204p_resync(uint8_t size, uint8_t *response);

#endif
