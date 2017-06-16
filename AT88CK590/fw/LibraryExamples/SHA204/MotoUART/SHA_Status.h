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
 *  \brief 	status / return code declarations
 *  \date 	Feb 12, 2010
 *  \modified March 19, 2010		by Nelson Lunsford
 */
#ifndef SHA_STATUS_H
#define SHA_STATUS_H

#include <stdint.h>     	// C99 standard typedefs

#define SHA_SUCCESS      	(int8_t)  0x00 //!< Function succeeded. Device status, if available, was okay.
#define SHA_GEN_FAIL		(int8_t)  0xD0 //!< unspecified error
#define SHA_BAD_PARAM		(int8_t)  0xD1 //!< bad argument (out of range, null pointer, etc.)
#define SHA_PARSE_ERROR   	(int8_t)  0xD2 //!< response status byte indicates parsing error
#define SHA_CMD_FAIL		(int8_t)  0xD3 //!< response status byte indicates command execution error
#define SHA_STATUS_UNKNOWN	(int8_t)  0xD4 //!< response status byte is unknown
#define SHA_FUNC_FAIL     	(int8_t)  0xE0 //!< Function could not execute due to incorrect condition / state.
#define SHA_COMM_FAIL     	(int8_t)  0xF0 //!< Communication with device failed.
#define SHA_TIMEOUT			(int8_t)  0xF1 //!< Timed out while waiting for response. Number of bytes received is 0.
#define SHA_BAD_SIZE		(int8_t)  0xF2 //!< Size value in response was different than expected.
#define SHA_HW_FAIL       	(int8_t)  0xF3 //!< Hardware failure, for instance setting up a timer or port.
#define SHA_INVALID_ID    	(int8_t)  0xF4 //!< invalid device id, id not set
#define SHA_INVALID_SIZE	(int8_t)  0xF5 //!< Could not copy response because receive buffer was too small.
#define SHA_BAD_CRC     	(int8_t)  0xF6 //!< incorrect CRC received
#define SHA_RX_FAIL			(int8_t)  0xF7 //!< Timed out while waiting for response. Number of bytes received is > 0.

#endif
