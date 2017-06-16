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
 *  \brief  Main Application to Test the SHA204 Library
 *  \author Atmel Crypto Products
 *  \date   September 30, 2010
*/

#include "sha204_lib_return_codes.h"
#include "SHA_Tests.h"


int main()
{
	static uint8_t ret_code;

	sha204p_init();

	//uint16_t tickCount = SHAT_TestTimeLoop();
	//status = SHAT_TestWakeupSequence();
	//status = SHAT_TestWatchdogTimeout();
	while (1) {
		//status = sha204t_test_communication();
		ret_code = sha204t_test_send_and_receive();
		//status = sha204t_test_command_tempsense();
	}

	// The following calls to the Marshaling layer are implemented
	// to test building without errors. A functional test is not
	// intended except to test communication with the device.
	//status = SHAT_TestMarshalingModule();

	return (int) ret_code;
}

