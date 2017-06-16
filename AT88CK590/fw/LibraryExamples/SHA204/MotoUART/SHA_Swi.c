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
 *  \brief 	functions for single wire communication layer of SHA204 library
 *  \date 	May 10, 2010
 */

#include <ctype.h>
#include "Physical.h"
#include "SHA_Swi.h"
#include "SHA_Status.h"


/** \brief Sends a command to the device.
 * \param[in] count number of bytes to send
 * \param[in] buffer pointer to command buffer
 * \return status of the operation
 */
int8_t SHAP_SendCommand(uint8_t count, uint8_t *buffer)
{
	uint8_t cmdFlag = FLAG_CMD;
	SHAP_SendBytes(1, &cmdFlag);
	return SHAP_SendBytes(count, buffer);
}


/** \brief Receives a response from the device.
 * \param[in] count number of bytes to receive
 * \param[in] buffer pointer to response buffer
 * \return status of the operation
 */
int8_t SHAP_ReceiveResponse(uint8_t count, uint8_t *buffer)
{
	uint8_t txFlag = FLAG_TX;
	SHAP_SendBytes(1, &txFlag);
	return SHAP_ReceiveBytes(count, buffer);
}


/** \brief Puts the device into idle state.
 * \return status of the operation
 */
int8_t SHAP_Idle()
{
	uint8_t idleFlag = FLAG_IDLE;
	return SHAP_SendBytes(1, &idleFlag);
}


/** \brief Puts device into low-power state.
 *  \return status of the operation
 */
int8_t SHAP_Sleep() {
	uint8_t sleepFlag = FLAG_SLEEP;
	return SHAP_SendBytes(1, &sleepFlag);
}
