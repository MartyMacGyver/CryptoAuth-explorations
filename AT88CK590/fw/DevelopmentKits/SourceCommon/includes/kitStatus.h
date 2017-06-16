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
 *  \brief This file contains status definitions for Atmel's crypto-product development kits.
 *  \author Atmel Crypto Products
 *  \date January 20, 2011
 */

#include <stdint.h>


#ifndef _KIT_STATUS_H_
#   define _KIT_STATUS_H_

// AES132 library occupies codes between 0x00 and 0xB4.
// SHA204 library occupies codes between 0xD0 and 0xF7.
enum {
	KIT_STATUS_SUCCESS             = 0x00,
	KIT_STATUS_UNKNOWN_COMMAND     = 0xC0,
	KIT_STATUS_USB_RX_OVERFLOW     = 0xC1,
	KIT_STATUS_USB_TX_OVERFLOW     = 0xC2,
	KIT_STATUS_INVALID_PARAMS      = 0xC3,
	KIT_STATUS_INVALID_IF_FUNCTION = 0xC4,
	KIT_STATUS_NO_DEVICE           = 0xC5
};

#endif

