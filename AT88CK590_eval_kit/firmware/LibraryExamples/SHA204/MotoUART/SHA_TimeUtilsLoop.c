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
 *  \brief 	communication layer of SHA204 library
 *  \date 	Feb 23, 2010
 */

#include <stdint.h>
#include "SHA_TimeUtils.h"

// The values below are valid for an AVR 8-bit processor running at 16 MHz.
// Code is compiled with -O1.

//!< Offsets time needed to prepare microseconds loop.
#define SHA_TIME_OFFSET_US		(7)

//!< Fill a while loop with these CPU instructions to achieve 1 us per iteration.
#define SHA_TIME_LOOP_FILLER	asm volatile ("nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop")


/** \brief Delays for a certain amount of time.
 *
 * \param[in] delay Delay for this number of microseconds (us).
 */
void SHAP_Delay(uint32_t delay)
{
	if (delay <= SHA_TIME_OFFSET_US)
		// We spent already this much time entering this function.
		// With other words: A value <= SHA_TIME_OFFSET_US will not produce the desired delay.
		return;

	delay -= SHA_TIME_OFFSET_US;

	// One while iteration takes around 0.6 us when compiled for an AT90USB1287 running at 16 MHz
	// and optimized as -O1. Fill in NOPs so that one loop iteration takes 1 us.
	// If leaving out or adding one NOP leads to unacceptable
	// accuracy, try to add NOPs to go above 1 us and reduce the value in "iterationsPerByte".
	// If an empty while iteration takes more than 1 us, establish an
	// appropriate value for "iterationsPerByte" < 256.
	// It should be possible to achieve a desired accuracy by combining filler NOPs
	// with a matching value for "iterationsPerByte".
	// When using a 32-bit processor, just replace the code in this function with
	// "while (delay--) FILLER_CODE;"

	volatile uint8_t delayByte3 = (uint8_t) ((delay & 0xFF000000) >> 24);
	volatile uint8_t delayByte2 = (uint8_t) ((delay & 0x00FF0000) >> 16);
	volatile uint8_t delayByte1 = (uint8_t) ((delay & 0x0000FF00) >> 8);
	volatile uint8_t delayByte0 = (uint8_t) (delay & 0x000000FF);
	//uint16_t iterationsPerByte = (256 * 75) / 100;	// The result has to be smaller than 256.
	uint16_t iterationsPerByte = 0;	// The result has to be smaller than 256.
	volatile uint8_t nIterations0 = (uint8_t) iterationsPerByte;	// Iterates 256 times when 0.
	volatile uint8_t nIterations1 = (uint8_t) iterationsPerByte;
	volatile uint8_t nIterations2 = (uint8_t) iterationsPerByte;

	if (delayByte3) {
		do {
			while (nIterations2--) {
				while (nIterations1--) {
					while (nIterations0--)
						SHA_TIME_LOOP_FILLER;
				}
			}
		} while (delayByte3--);
		nIterations0 = nIterations1 = iterationsPerByte;
	}
	if (delayByte2) {
		do {
			while (nIterations1--) {
				while (nIterations0--)
					SHA_TIME_LOOP_FILLER;
			}
		} while (delayByte2--);
		nIterations0 = iterationsPerByte;
	}
	if (delayByte1) {
		do {
			while (nIterations0--)
				SHA_TIME_LOOP_FILLER;
		} while (delayByte1--);
	}
	if (delayByte0) {
		do {
			SHA_TIME_LOOP_FILLER;
		} while (delayByte0--);
	}
}
