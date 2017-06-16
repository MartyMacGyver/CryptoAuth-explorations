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
 *  \brief  Definitions for Configurable Values of the SHA204 Library
 *
 *          This file contains several library configuration sections
 *          for the three interfaces the library supports
 *          (SWI using GPIO or UART, and I2C) and one that is common
 *          to all interfaces.
 *  \author Atmel Crypto Products
 *  \date 	February 2, 2011
 */
#ifndef SHA204_CONFIG_H
#   define SHA204_CONFIG_H

#include <stddef.h>                    // data type definitions

//////////////////////////////////////////////////////////////////////////
////////// definitions common to all interfaces //////////////////////////
//////////////////////////////////////////////////////////////////////////

/** \brief maximum CPU clock deviation to higher frequency (crystal etc.)
 * This value is used to establish time related worst case numbers, for
 * example to calculate execution delays and timeouts.
 */
#define CPU_CLOCK_DEVIATION_POSITIVE   (1.01)

/** \brief maximum CPU clock deviation to lower frequency (crystal etc.)
 * This value is used to establish time related worst case numbers, for
 * example to calculate execution delays and timeouts.
 */
#define CPU_CLOCK_DEVIATION_NEGATIVE   (0.99)

/** \brief number of command / response retries
 *
 * If communication is lost, re-synchronization includes waiting for the
 * longest possible execution time of a command.
 * This adds a #SHA204_COMMAND_EXEC_MAX delay to every retry.
 * Every increment of the number of retries increases the time
 * the library is spending in the retry loop by #SHA204_COMMAND_EXEC_MAX.
 */
#define SHA204_RETRY_COUNT           (1)


//////////////////////////////////////////////////////////////////////////
///////////// definitions specific to interface //////////////////////////
//////////////////////////////////////////////////////////////////////////

//! Define this if you are using GPIO for SWI communication.
//#define SHA204_SWI_BITBANG

//! Define this if you are using a UART for SWI communication.
//#define SHA204_SWI_UART

//! Define this if you are using TWI communication.
//#define SHA204_I2C

////////////////////////////// GPIO configurations //////////////////////////////
#ifdef SHA204_SWI_BITBANG

//! #START_PULSE_TIME_OUT in us instead of loop counts
#   define SWI_RECEIVE_TIME_OUT      ((uint16_t) 163)

//! It takes 312.5 us to send a byte (9 single-wire bits / 230400 Baud * 8 flag bits).
#   define SWI_US_PER_BYTE           ((uint16_t) 313)
#endif


////////////////////////////// UART configurations //////////////////////////////
#ifdef SHA204_SWI_UART

//! receive timeout in us instead of loop counts
#   define SWI_RECEIVE_TIME_OUT      ((uint16_t) 153)

//! It takes 312.5 us to send a byte (9 single-wire bits / 230400 Baud * 8 flag bits).
#   define SWI_US_PER_BYTE           ((uint16_t) 313)

//! SWI response timeout is the sum of receive timeout and the time it takes to send the TX flag.
#   ifndef SHA204_RESPONSE_TIMEOUT
#      define SHA204_RESPONSE_TIMEOUT   ((uint16_t) SWI_RECEIVE_TIME_OUT + SWI_US_PER_BYTE)
#   endif

#endif


///////// SWI configurations common for GPIO and UART implementation ////////////////////////
#if defined(SHA204_SWI_BITBANG) || defined(SHA204_SWI_UART)

//! delay before sending a transmit flag in the synchronization routine
#   define SHA204_SYNC_TIMEOUT       ((uint8_t) 85)

#   ifndef SHA204_RESPONSE_TIMEOUT
//! SWI response timeout is the sum of receive timeout and the time it takes to send the TX flag.
#      define SHA204_RESPONSE_TIMEOUT   ((uint16_t) SWI_RECEIVE_TIME_OUT + SWI_US_PER_BYTE)
#   endif

#endif


////////////////////////////// I2C configurations //////////////////////////////
#ifdef SHA204_I2C

/** \brief For I2C, the response polling time is the time
 *         it takes to send the I2C address.
 *
 *         This value is used to timeout when waiting for a response.
 */
#   ifndef SHA204_RESPONSE_TIMEOUT
#      define SHA204_RESPONSE_TIMEOUT     ((uint16_t) 37)
#   endif

#endif


#endif
