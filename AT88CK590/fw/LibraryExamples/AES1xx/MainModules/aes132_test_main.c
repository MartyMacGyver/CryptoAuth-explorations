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
/** \file Main module for AES132 library example that runs test vectors.
 *  \date February 2, 2011
 */

#include <stdint.h>
#include <string.h>

#include "aes132_physical.h"
#include "aes132_test_vectors.h"
#include "aes132_lib_return_codes.h"
#include "timer_utilities.h"


/** \brief This function is the entry function for testing the AES132 library.
 * \return result of tests (0: success, otherwise failure)
 */
int main(void)
{
	static uint8_t aes132_lib_return;

	aes132p_enable_interface();

	while (1) {
		aes132_lib_return = aes132t_command_info();
		if (aes132_lib_return != AES132_FUNCTION_RETCODE_SUCCESS)
			delay_ms(10);

//		aes132_lib_return = aes132t_command_temp_sense();
//		if (aes132_lib_return != AES132_FUNCTION_RETCODE_SUCCESS)
//			_delay_ms(10);

//		aes132_lib_return = aes132t_block_read();
//		if (aes132_lib_return != AES132_FUNCTION_RETCODE_SUCCESS)
//			_delay_ms(10);
 	}

	aes132p_disable_interface();

	return AES132_FUNCTION_RETCODE_SUCCESS;
}

