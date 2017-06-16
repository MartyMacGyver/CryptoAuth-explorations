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
 *  \brief 	This file contains definitions for device discovery.
 *  \author Atmel Crypto Products
 *  \date 	February 9, 2011
 */

#ifndef COMBINED_DISCOVER
#   define COMBINED_DISCOVER


#include "Combined_Physical.h"
#include "sha204_comm_marshaling.h"
#include "parserAscii.h"

//! lsb of TWI address is set when reading
#define I2C_READ_FLAG                    (1)

//! Wait the maximum execution time after sending a command during device discovery.
#define SHA204_EXEC_MAX        HMAC_EXEC_MAX


interface_id_t DiscoverDevices();
device_info_t *GetDeviceInfo(uint8_t index);
device_type_t GetDeviceType(uint8_t index);

//Select one or none of these.  Defaults to AT88CK490 if none are defined
//#define ECCROOT
//#define ECCSIGNER
//#define CK590

#endif
