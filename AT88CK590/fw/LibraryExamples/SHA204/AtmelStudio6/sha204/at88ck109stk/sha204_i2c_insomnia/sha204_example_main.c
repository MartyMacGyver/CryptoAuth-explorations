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
    \brief  Application Examples that Use the SHA204 Library
    \author Atmel Crypto Products
    \date   December 13, 2012
*/

/** \defgroup sha204_main Main Application Module
 *
@{ */

#include <stddef.h>                   // data type definitions
#include "sha204_examples.h"          // definitions and declarations for example functions
#include "sha204_comm_marshaling.h"   // definitions and declarations for the Command Marshaling module


/** \brief This application calls one example function.

The example function sha204e_checkmac_firmware does not return since it runs in an endless loop.
 * \return exit status of application
 */
int main(void)
{
	// declared as "volatile" for easier debugging
	volatile uint8_t ret_code;

	ret_code = sha204e_checkmac_firmware();

	return (int) ret_code;
}
/** @} */
