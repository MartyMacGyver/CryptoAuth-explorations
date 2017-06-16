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
 *  \brief  Application Examples That Use the SHA204 Library
 *  \author Atmel Crypto Products
 *  \date   November 16, 2012
 *
 *   Example functions are given that demonstrate the device.
 *   The examples demonstrate client / host scenarios with a random challenge.
 *   Using a random challenge makes replay attacks impossible.
 *   Examples that need two devices (advanced examples) run only with
 *   I<SUP>2</SUP>C devices or SWI devices using GPIO. When running the advanced examples
 *   with SWI devices, their SDA cannot be shared. Therefore, these examples run 
 *   only in the bit-banged and not in the UART implementation of SWI.
 *   It is possible for SWI devices to share SDA, but then the Pause command
 *   has to be used to idle all devices except one to communicate with.
 *   In such a system, the Selector byte of every device has to be unique
 *   and not 0 which is the default when shipped.
*/
#ifndef SHA204_EXAMPLES_H
#   define SHA204_EXAMPLES_H

#include <stdint.h>                   // data type definitions


/** \defgroup sha204_example_definitions Example Definitions
 *  \ingroup  sha204_examples

SHA204_EXAMPLE_CHECKMAC_FIRMWARE:\n
simple MAC / CheckMac example using firmware as the host
(key storage and SHA-256 calculation)
*/

#define SHA204_EXAMPLE_CHECKMAC_FIRMWARE
/** @} */

#define SHA204_CLIENT_ADDRESS        (0xC8)

#define SHA204_KEY_ID                (0)

#define sha204e_wakeup_sleep()   {sha204p_wakeup(); sha204p_sleep();}

/** \defgroup sha204_example_functions Example Functions
 *  \ingroup  sha204_examples
@{ */
uint8_t sha204e_checkmac_firmware(void);
/** @} */

#endif
