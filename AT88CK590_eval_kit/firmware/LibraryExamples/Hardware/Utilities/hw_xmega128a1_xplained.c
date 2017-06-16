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
 *  \brief This file contains implementations of LED functions.
 *  \author Atmel Crypto Products
 *  \date January 30, 2011
 */
#include "hardware.h"
#include "led.h"
#include "xmega_a1_xplained.h"

void Led1(uint8_t on_off)
{
	if (on_off)
		LED_On(LED0_GPIO);
	else
		LED_Off(LED0_GPIO);
}


void Led2(uint8_t on_off)
{
	if (on_off)
		LED_On(LED1_GPIO);
	else
		LED_Off(LED1_GPIO);
}


void Led3(uint8_t on_off)
{
	if (on_off)
		LED_On(LED2_GPIO);
	else
		LED_Off(LED2_GPIO);
}
