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

/** \file
 * CPU configuration module for AVR 8-bit micro-controllers.
 * \author Atmel Crypto Products
 * \date December 9, 2010
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>


#include "cpu.h"


/** \brief This function writes prescale value to the CPU clock prescale register.
 *
 *         See chapter 6.9 in the AT90USB1287 data sheet.
 *         The resulting clock divisor is 2 ^ value.
 * \param[in] prescale value to be written into the prescale register
 */
void PrescaleCpuClock(uint8_t prescale)
{
	cli();
	CLKPR = _BV(CLKPCE);
	CLKPR = prescale;
	sei();
}
