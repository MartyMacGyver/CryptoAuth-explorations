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
 *  \date 	Feb 21, 2010
 */

#ifndef SHA_COMM_H
#define SHA_COMM_H

#include <stdint.h>

#define SHA_WATCHDOG_TIMEOUT	(21)	//!< maximum watchdog timeout of device in s

#define SHA_COMMAND_SIZE_MIN	(6)		//!< minimum number of bytes in command (from count byte to second CRC byte)
#define SHA_RESPONSE_SIZE_MIN	(4)		//!< minimum number of bytes in response

#define SHA_CRC_SIZE			(2)		//!< number of CRC bytes

#ifdef BIG_ENDIAN
	#define SHA_CRC_BYTE_INDEX_LOW	(1)		//!< array index for low CRC byte (big-endian)
	#define SHA_CRC_BYTE_INDEX_HIGH	(0)		//!< array index for high CRC byte (big-endian)
#else
	#define SHA_CRC_BYTE_INDEX_LOW	(0)		//!< array index for low CRC byte (little-endian)
	#define SHA_CRC_BYTE_INDEX_HIGH	(1)		//!< array index for high CRC byte (little-endian)
#endif

#define SHA_BUFFER_POS_COUNT	(0)		//!< buffer index of count byte in command or response
#define SHA_BUFFER_POS_STATUS	(1)		//!< buffer index of status byte in response

#define SHA_STATUS_BYTE_WAKEUP	(0x11)	//!< status byte after wakeup
#define SHA_STATUS_BYTE_PARSE	(0x03)	//!< command parse error
#define SHA_STATUS_BYTE_EXEC	(0x0F)	//!< command execution error
#define SHA_STATUS_BYTE_COMM	(0xFF)	//!< communication error

#define SHA_RETRY_COUNT			(1)		//!< number of rx retries

#define WATCHDOG_TIMEOUT		(21)	//!< maximum watchdog timeout of device in s


/** \brief used as parameter group for communication functions */
typedef struct {
	uint8_t *txBuffer;
	uint8_t *rxBuffer;
	uint8_t rxSize;
	uint32_t executionDelay;
} SHA_CommParameters;


void SHAC_Wakeup();
int8_t SHAC_SendAndReceive(SHA_CommParameters *params);
#endif
