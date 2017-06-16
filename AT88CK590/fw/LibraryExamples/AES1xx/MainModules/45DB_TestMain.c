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
/** \file Main module for AES1xx library example that tests the SPI driver
 * 		against the dataflash on the STK600.
 * \date April 15, 2010
 */

#include <stdint.h>
#include <string.h>

#include "AES1_Physical.h"
#include "AES1_Status.h"


/** \brief This function is the entry function for testing the AES1xx library.
 * \return result of tests (0: success, otherwise failure)
 */
int main(void)
{
	int8_t status = AES1_STATUS_TIMEOUT;

	// If we make the buffers below too big, the (gnu) debugger reports that
	// source lines are not found (stop at cursor or set breakpoint).
   uint8_t txBuffer[252];
	uint8_t rxBuffer[252];
	uint8_t i;

	for (i = 1; i <= sizeof(txBuffer); i++)
		txBuffer[i] = i;

	AES1P_EnableDeviceDriver();

	status = AES1P_WaitForReady();
   if (status != AES1_STATUS_SUCCESS) {
   	AES1P_DisableDeviceDriver();
      return status;
   }

   while (1) {

      status = AES1P_WriteMemory(sizeof(txBuffer), 0, 0, txBuffer);
      if (status != AES1_STATUS_SUCCESS)
         break;

      status = AES1P_ReadMemory(sizeof(rxBuffer), 0, 0, rxBuffer);
      if (status != AES1_STATUS_SUCCESS)
         break;

      if (memcmp(txBuffer, rxBuffer, sizeof(rxBuffer)))
         break;
   }

	AES1P_DisableDeviceDriver();

	return AES1_STATUS_SUCCESS;
}
