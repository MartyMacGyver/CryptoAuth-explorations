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
 *  \brief  Application examples that Use the SHA204 Library
 *  \author Atmel Crypto Products
 *  \date   May 30, 2012
 *
 *   Three example functions are given that demonstrate the device.
*/
#ifndef SHA204_EXAMPLES_H
#   define SHA204_EXAMPLES_H

#include <stdint.h>                   // data type definitions
#include <conf_atsha204.h>

// The examples demonstrate client / host device scenarios for I2C configurations.
// If you have two devices at your disposal you can run this example as a real-world
// host / client demonstration. You have to change the address of one of the devices
// by writing it to configuration zone address 16. Be aware that bit 3 of
// the I2C address is also used to configure the input level reference 
// (see data sheet table 2-1).
#define SHA204_CLIENT_ADDRESS   SHA204_I2C_DEFAULT_ADDRESS
// To make the Mac / CheckMac examples work out-of-the-box, only one device is being 
// used as example default. See above.
//#   define SHA204_HOST_ADDRESS     SHA204_CLIENT_ADDRESS
#define SHA204_HOST_ADDRESS     (0xCA)

#define SHA204_KEY_ID           (0x0000)

#define sha204e_wakeup_sleep()   {sha204p_wakeup(); sha204p_sleep();}

uint8_t sha204e_fixed_challenge_response(void);

#endif
