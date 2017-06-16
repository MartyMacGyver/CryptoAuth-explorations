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
 *  \brief time utility functions
 *  \date June 22, 2010
 */

#include <stdint.h>
#include "time_utils.h"


// The values below are valid for an AVR 8-bit processor running at 8 MHz.
// Code is compiled with -O1.

#if F_CPU == 16000000UL
//! Offsets time needed to prepare microseconds loop. Only used in delay_us_uint32 function.
#define   TIME_UTILS_LOOP_OFFSET_US                  (7UL)

//! Fill a while loop with these CPU instructions to achieve 1 us per iteration.
#define   TIME_UTILS_LOOP_FILLER           asm volatile ("\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop")

/** \brief If decrement iteration takes more than 1 us, do not start from 0 (256)
 * but take this value. A value != 0 (256) reduces the timer resolution to a multiple of 1 us.
 */
/** \brief If decrement iteration takes more than 1 us, divide the delay parameter by this value.
 * A value != 1 reduces the timer resolution to a multiple of 1 us.
 * This macro is only used in delay_us function.
 */
#define   TIME_UTILS_DELAY_DIVISOR         ((uint8_t)   2)

/** \brief If decrement iteration takes more than 1 us, do not start from 0 (256)
 * but take this value. A value != 0 (256) reduces the timer resolution to a multiple of 1 us.
 * This macro is only used in delay_us_uint32 function.
 */
#define   TIME_UTILS_ITERATIONS_PER_BYTE   ((uint8_t)   0)

#elif F_CPU == 8000000UL
//! Offsets time needed to prepare microseconds loop. Only used in delay_us_uint32 function.
#define   TIME_UTILS_LOOP_OFFSET_US                  (7UL)

//! Fill a while loop with these CPU instructions to achieve a multiple of 1 us per iteration.
#define   TIME_UTILS_LOOP_FILLER           __asm__ volatile ("\n\tnop\n\tnop\n\tnop\n\tnop") //\n\tnop")

/** \brief If decrement iteration takes more than 1 us, divide the delay parameter by this value.
 * A value > 1 reduces the timer resolution to a multiple of 1 us.
 * This macro is only used in delay_us function.
 */
#define   TIME_UTILS_DELAY_DIVISOR         ((uint8_t)   1)

/** \brief The delay_ms function calls delay_us four times per ms with this parameter.
 *
 *         Usually this value is less than 250, since it takes some time to execute the loop.
 */
#define   TIME_UTILS_MS_CALIBRATION        ((uint8_t) 250)

/** \brief If decrement iteration takes more than 1 us, do not start from 0 (256)
 * but take this value. A value != 0 (256) reduces the timer resolution to a multiple of 1 us.
 * This macro is only used in delay_us_uint32 function.
 */
#define   TIME_UTILS_ITERATIONS_PER_BYTE   ((uint8_t) 141)

#else
#error   Time macros are not defined.
#endif


/** \brief This function delays for a number of microseconds.
 *
 * This function will not time correctly, if one loop iteration
 * plus the time it takes to enter this function takes more than 255 us.
 * Also, if one loop iteration takes more than 1 us, the resolution
 * of the function deteriorates.
 * \param[in] delay number of microseconds to delay
 */
void delay_us(uint8_t delay)
{
// The more time we take for one loop iteration the
// smaller we have to make the loop count (parameter "delay").
	delay /= TIME_UTILS_DELAY_DIVISOR;

	do {
		TIME_UTILS_LOOP_FILLER;
	} while (--delay > 0);
}


/** \brief This function delays for a number of milliseconds.
 *
 * \param[in] delay number of milliseconds to delay
 */
void delay_ms(uint8_t delay)
{
	uint8_t i;

	do {
		i = 4;
		do {
			delay_us(TIME_UTILS_MS_CALIBRATION);
		} while (--i > 0);
	} while (--delay > 0);
}


/** \brief This function delays for a certain amount of time.
 * \param[in] delay Delay for this number of microseconds (us).
 */
void delay_us_uint32(uint32_t delay)
{
	// One while iteration takes around 0.6 us when compiled for an AT90USB1287 running at 16 MHz
	// and optimized as -O1. Fill in NOPs so that one inner loop iteration takes 256 us.
	// If leaving out or adding one NOP leads to unacceptable
	// accuracy, try to add NOPs to go above 1 us and reduce the value in "iterations_per_byte".
	// If an empty while iteration takes more than 1 us, establish an
	// appropriate value for "iterations_per_byte" < 256.
	// It should be possible to achieve a desired accuracy by combining filler NOPs
	// with a matching value for "iterations_per_byte".
	// When using a 32-bit processor, just replace the code in this function with
	// "while (delay--) FILLER_CODE;"
	// This implementation is blocking. In a real-time OS environment, you might want to
	// replace the code below with a call to an OS timer utility function, or a call-back
	// function that lets another OS task run in the meantime.

	volatile uint8_t delay_byte_3;
	volatile uint8_t delay_byte_2;
	volatile uint8_t delay_byte_1;
	volatile uint8_t delay_byte_0;
	volatile uint8_t n_iterations_0;   // Iterates 256 times when 0.
	volatile uint8_t n_iterations_1;
	volatile uint8_t n_iterations_2;
	uint8_t iterations_per_byte = TIME_UTILS_ITERATIONS_PER_BYTE;

	if (delay <= TIME_UTILS_LOOP_OFFSET_US)
		// We spent already this much time entering this function.
		// With other words: A value < TIME_UTILS_LOOP_OFFSET_US will not produce the desired delay.
		return;

	delay -= TIME_UTILS_LOOP_OFFSET_US;

	delay_byte_3 = (uint8_t) ((delay & 0xFF000000) >> 24);
	delay_byte_2 = (uint8_t) ((delay & 0x00FF0000) >> 16);
	delay_byte_1 = (uint8_t) ((delay & 0x0000FF00) >> 8);
	delay_byte_0 = (uint8_t) (delay & 0x000000FF);

	if (iterations_per_byte != 0)
		// The loop resolution for delay_byte_0 is now only 2 us.
		delay_byte_0 /= 2;

	if (delay_byte_3 > 0) {
		do {
			n_iterations_2 = iterations_per_byte;
			while (n_iterations_2-- > 0) {
				n_iterations_1 = iterations_per_byte;
				while (n_iterations_1-- > 0) {
					n_iterations_0 = iterations_per_byte;
					while (n_iterations_0-- > 0)
						TIME_UTILS_LOOP_FILLER;
				}
			}
		} while (delay_byte_3-- > 0);
	}
	if (delay_byte_2 > 0) {
		do {
			n_iterations_1 = iterations_per_byte;
			while (n_iterations_1-- > 0) {
				n_iterations_0 = iterations_per_byte;
				while (n_iterations_0-- > 0)
					TIME_UTILS_LOOP_FILLER;
			}
		} while (delay_byte_2-- > 0);
	}
	if (delay_byte_1 > 0) {
		do {
			n_iterations_0 = iterations_per_byte;
			while (n_iterations_0-- > 0)
				TIME_UTILS_LOOP_FILLER;
		} while (delay_byte_1-- > 0);
	}
	if (delay_byte_0 > 0) {
		do {
			TIME_UTILS_LOOP_FILLER;
		} while (delay_byte_0-- > 0);
	}
}
