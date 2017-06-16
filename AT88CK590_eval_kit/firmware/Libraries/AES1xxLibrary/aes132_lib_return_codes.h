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
 *  \brief definitions of function return codes
 *
 *         Error values below 0xA0 are reserved for return codes of the
 *         device response (byte at index 1 in device response buffer).
 *  \date  March 2, 2011
 *  \author Atmel Crypto Products
 */
#ifndef AES132_LIB_RETURN_CODES_H
#   define AES132_LIB_RETURN_CODES_H

#include <stdint.h>


// ------------------------- library return codes ----------------------------
#define AES132_FUNCTION_RETCODE_ADDRESS_WRITE_NACK   ((uint8_t) 0xA0) //!< TWI nack when sending a TWI address for writing
#define AES132_FUNCTION_RETCODE_ADDRESS_READ_NACK    ((uint8_t) 0xA1) //!< TWI nack when sending a TWI address for reading
#define AES132_FUNCTION_RETCODE_SIZE_TOO_SMALL       ((uint8_t) 0xA2) //!< Count value in response was smaller than expected.

// The codes below are the same as in the SHA204 library.
#define AES132_FUNCTION_RETCODE_SUCCESS              ((uint8_t) 0x00) //!< Function succeeded.
#define AES132_FUNCTION_RETCODE_BAD_CRC_TX           ((uint8_t) 0xD4) //!< Device status register bit 4 (CRC) is set.
#define AES132_FUNCTION_RETCODE_NOT_IMPLEMENTED      ((uint8_t) 0xE0) //!< interface function not implemented
#define AES132_FUNCTION_RETCODE_DEVICE_SELECT_FAIL   ((uint8_t) 0xE3) //!< device index out of bounds
#define AES132_FUNCTION_RETCODE_COUNT_INVALID        ((uint8_t) 0xE4) //!< count byte in response is out of range
#define AES132_FUNCTION_RETCODE_BAD_CRC_RX           ((uint8_t) 0xE5) //!< incorrect CRC received
#define AES132_FUNCTION_RETCODE_TIMEOUT              ((uint8_t) 0xE7) //!< Function timed out while waiting for response.
#define AES132_FUNCTION_RETCODE_COMM_FAIL            ((uint8_t) 0xF0) //!< Communication with device failed.


#endif
