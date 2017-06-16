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
 *  \brief Header file for aes132_commands.c.
 */

#ifndef AES132_COMMANDS_H_
#define AES132_COMMANDS_H_

#include <stdint.h>
#include "aes132.h"


// --------------------- definitions for the Info command ---------------

//! response size after successfully executed Info command
#define AES132_RESPONSE_SIZE_INFO                  (6)

//! execution time in ms for Info command
#define AES132_INFO_EXECUTION_TIME                 ((uint8_t)   0)

//! Info command selector for MacCount
#define AES132_INFO_MAC_COUNT                      ((uint8_t) 0x00)

//! Info command selector for AuthOK
#define AES132_INFO_DEVICE_AUTH_OK                 ((uint8_t) 0x05)

//! Info command selector for DevNum
#define AES132_INFO_DEV_NUM                        ((uint8_t) 0x06)

//! Info command selector for ChipState
#define AES132_INFO_CHIP_STATE                     ((uint8_t) 0x0C)


// --------------------- definitions for the TempSense command ---------------

//! response size after successfully executed TempSense command
#define AES132_RESPONSE_SIZE_TEMP_SENSE            (8)

//! execution time in ms for TempSense command
#define AES132_TEMP_SENSE_EXECUTION_TIME           ((uint8_t)   4)

//! size of TempOffset configuration register
#define AES132_TEMP_OFFSET_REGISTER_SIZE           (2)

//! word address of TempOffset configuration register
#define AES132_TEMP_OFFSET_REGISTER_ADDRESS        ((uint16_t) 0xF042)

//! response buffer index of msb of Temperature Code High
#define AES132_RESPONSE_INDEX_TEMP_CODE_HIGH_MSB   (AES132_RESPONSE_INDEX_DATA)

//! response buffer index of lsb of Temperature Code High
#define AES132_RESPONSE_INDEX_TEMP_CODE_HIGH_LSB   (AES132_RESPONSE_INDEX_DATA + 1)

//! response buffer index of msb of Temperature Code Low
#define AES132_RESPONSE_INDEX_TEMP_CODE_LOW_MSB    (AES132_RESPONSE_INDEX_DATA + 2)

//! response buffer index of lsb of Temperature Code Low
#define AES132_RESPONSE_INDEX_TEMP_CODE_LOW_LSB    (AES132_RESPONSE_INDEX_DATA + 3)


// --------------------- definitions for the BlockRead command ---------------

//! minimum response size after successfully executed BlockRead command
#define AES132_RESPONSE_SIZE_BLOCK_READ            (4)

//! execution time in ms for BlockRead command
/** \todo Verify value. */
#define AES132_BLOCK_READ_EXECUTION_TIME           ((uint8_t)   4)



// --------------------- function declarations ---------------

uint8_t aes132m_block_read(uint16_t word_address, uint8_t n_bytes, uint8_t *result);
uint8_t aes132m_info(uint8_t selector, uint8_t *result);
uint8_t aes132m_temp_sense(uint16_t *temp_diff);

#endif
