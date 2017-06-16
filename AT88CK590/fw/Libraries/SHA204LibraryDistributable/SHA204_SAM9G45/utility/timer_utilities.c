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
 *  \brief Timer Utility Functions Using the AT91 Library
 *  \author Atmel Crypto Products
 *  \date February 7, 2011
 */

#include <stdint.h>                           //!< data type definitions
#include <board.h>                            //!< evaluation kit definitions
#include <pmc/pmc.h>                          //!< power management controller definitions
#include <timer_utilities.h>                  //!< timer function definitions

/** \brief This function initializes the timer / counter peripheral.
 */
void init_timer(void)
{
    volatile uint32_t dummy;

    // Configure the PMC to enable the Timer Counter clock for TC.
    PMC_EnablePeripheral(AT91C_ID_TC);

    // Disable TC clock.
    *AT91C_TC0_CCR = AT91C_TC_CLKDIS;

    // Disable interrupts.
    *AT91C_TC0_IDR = 0xFFFFFFFF;

    // Clear status register.
    dummy = *AT91C_TC0_SR;
}


/** \brief This function delays for a number of tens of microseconds.
 * \param[in] delay number of 0.01 milliseconds to delay
 */
void delay_10us(uint8_t delay)
{
	uint16_t clock_count = (uint16_t) (BOARD_MCK / 100000 / 8);

   // Set clock frequency to BOARD_MCK / 8.
	*AT91C_TC0_CMR = AT91C_TC_CLKS_TIMER_DIV2_CLOCK;

	// Enable TC clock.
   *AT91C_TC0_CCR = AT91C_TC_CLKEN;

	// Reset clock counter and start TC clock.
   *AT91C_TC0_CCR = AT91C_TC_SWTRG;

	while (*AT91C_TC0_CV < clock_count * (uint16_t) delay);

    // Disable TC clock.
    *AT91C_TC0_CCR = AT91C_TC_CLKDIS;
}


/** \brief This function delays for a number of milliseconds.
 * \param[in] delay number of milliseconds to delay
 */
void delay_ms(uint8_t delay)
{
	/** \todo Chain counter 1 to counter 0 (10 us). */
	// Lets take a simple approach for now and set the
	// counter clock to cover 1 ms and loop.

    // Set clock frequency to BOARD_MCK / 128.
	*AT91C_TC0_CMR = AT91C_TC_CLKS_TIMER_DIV4_CLOCK;

	// Enable TC clock.
    *AT91C_TC0_CCR = AT91C_TC_CLKEN;

	do {
		// Reset clock counter and start TC clock.
		*AT91C_TC0_CCR = AT91C_TC_SWTRG;

		// Wait until the counter has been reset.
		while (*AT91C_TC0_CV >= BOARD_MCK / 1000 / 128);

		// Wait until the counter has reached a value
		// equivalent to 1 ms delay.
		while (*AT91C_TC0_CV < BOARD_MCK / 1000 / 128);
	} while (--delay);

    //  Disable TC clock.
    *AT91C_TC0_CCR = AT91C_TC_CLKDIS;
}
