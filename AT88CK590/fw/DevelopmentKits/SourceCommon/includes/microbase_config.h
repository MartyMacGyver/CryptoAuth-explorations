// ----------------------------------------------------------------------------
//         ATMEL Crypto_Devices Software Support  -  Colorado Springs, CO -
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

/** \file This file contains definitions needed for the Microbase development kit.
 * \date April 20, 2010
 */

#ifndef _CONFIG_H
#define _CONFIG_H

#include <stdint.h>

// The "includes" below are needed for usb_descriptors.c.
#include <avr/interrupt.h>
#include <avr/pgmspace.h>


#define TRUE		(1)	//!< definition for boolean type true
#define FALSE		(0)	//!< definition for boolean type false
#define ENABLE		(1)	//!< definition for enable
#define DISABLE	(0)	//!< definition for disable

/** \brief uint8_t type definition used by USB driver modules */
#define uchar		uint8_t

/** \brief uint16_t type definition used by USB driver modules */
#define ushort		uint16_t

/** \brief uint8_t* type definition used by USB driver modules */
#define puchar		(uint8_t *)

/** \brief USB packet size definition */
#define EP_LENGTH	(64)

/** \brief Endian type
 */
#define LITTLE_ENDIAN

/******************************************************************************/
/* GCC COMPILER                                                               */
/******************************************************************************/
#ifdef __GNUC__
	#define _ConstType_ __flash
	#define _MemType_
	#define _GenericType_ __generic
	#define code PROGMEM
	#define xdata
	#define idata
	#define data
	#define At(x) @ x
	#define pdata
	#define bdata
	#define bit uchar
	#define Bool uchar
	#define Enable_interrupt() sei()
	#define Disable_interrupt() cli()
	#define Get_interrupt_state() (SREG&0x80)
#endif

// U16/U32 endian handlers
#ifdef LITTLE_ENDIAN
// LITTLE_ENDIAN => 16bit: (LSB,MSB), 32bit: (LSW,MSW)
// or (LSB0,LSB1,LSB2,LSB3) or (MSB3,MSB2,MSB1,MSB0)
	#define MSB(u16)        (((uint8_t *) &u16)[1])
	#define LSB(u16)        (((uint8_t *) &u16)[0])
	#define MSW(u32)        (((uint16_t *) &u32)[1])
	#define LSW(u32)        (((uint16_t *) &u32)[0])
	#define MSB0(u32)       (((uint8_t *) &u32)[3])
	#define MSB1(u32)       (((uint8_t *) &u32)[2])
	#define MSB2(u32)       (((uint8_t *) &u32)[1])
	#define MSB3(u32)       (((uint8_t *) &u32)[0])
	#define LSB0(u32)       MSB3(u32)
	#define LSB1(u32)       MSB2(u32)
	#define LSB2(u32)       MSB1(u32)
	#define LSB3(u32)       MSB0(u32)
#else
// BIG_ENDIAN => 16bit: (MSB,LSB), 32bit: (MSW,LSW)
// or (LSB3,LSB2,LSB1,LSB0) or (MSB0,MSB1,MSB2,MSB3)
	#define MSB(u16)        (((uint8_t *) &u16)[0])
	#define LSB(u16)        (((uint8_t *) &u16)[1])
	#define MSW(u32)        (((uint16_t *) &u32)[0])
	#define LSW(u32)        (((uint16_t *) &u32)[1])
	#define MSB0(u32)       (((uint8_t *) &u32)[0])
	#define MSB1(u32)       (((uint8_t *) &u32)[1])
	#define MSB2(u32)       (((uint8_t *) &u32)[2])
	#define MSB3(u32)       (((uint8_t *) &u32)[3])
	#define LSB0(u32)       MSB3(u32)
	#define LSB1(u32)       MSB2(u32)
	#define LSB2(u32)       MSB1(u32)
	#define LSB3(u32)       MSB0(u32)
#endif


#endif
