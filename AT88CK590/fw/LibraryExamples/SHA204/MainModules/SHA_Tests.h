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
 *  \brief 	Test Functions for the SHA204 Library
 *  \author Atmel Crypto Products
 *  \date   August 23, 2010
*/

#ifndef SHA_TESTS
#define SHA_TESTS

#include "sha204_lib_return_codes.h"
//#include "sha204_comm_interface.h"
//#include "sha204_physical.h"
#include "sha204_comm.h"

void sha204t_calculate_crc(uint8_t length, uint8_t *data, uint8_t *crc);
uint16_t SHAT_TestTimeLoop();
uint8_t sha204t_test_wakeup();
uint8_t sha204t_test_watchdog_timeout();
uint8_t sha204t_test_communication();
uint8_t sha204t_test_send_and_receive();
uint8_t sha204t_test_command_tempsense();
uint8_t sha204t_test_marshaling_module();

#endif
