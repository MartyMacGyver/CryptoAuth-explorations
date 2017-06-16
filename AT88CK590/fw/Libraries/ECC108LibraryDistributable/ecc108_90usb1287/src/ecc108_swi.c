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
 *  \brief 	Functions for Single Wire, Hardware Independent Physical Layer of ECC108 Library
 *
 *          Possible return codes from send functions in the hardware dependent module
 *          are SWI_FUNCTION_RETCODE_SUCCESS and SWI_FUNCTION_RETCODE_TIMEOUT. These
 *          are the same values in swi_phys.h and sha204_lib_return_codes.h. No return code
 *          translation is needed in these cases (e.g. #ecc108p_idle, #ecc108p_sleep).
 *
 *  \author Atmel Crypto Products
 *  \date 	September 13, 2012
 */

#include "swi_phys.h"                            //!< hardware dependent declarations for SWI
#include "ecc108_physical.h"                     //!< declarations that are common to all interface implementations
#include "ecc108_lib_return_codes.h"             //!< declarations of function return codes
#include "timer_utilities.h"                     //!< definitions for delay functions

/** \defgroup atecc108_swi Module 04: SWI Abstraction Module
 *
 * These functions and definitions abstract the SWI hardware. They implement the functions
 * declared in \ref ecc108_physical.h.
@{ */
#define ECC108_SWI_FLAG_CMD     ((uint8_t) 0x77) //!< flag preceding a command
#define ECC108_SWI_FLAG_TX      ((uint8_t) 0x88) //!< flag requesting a response
#define ECC108_SWI_FLAG_IDLE    ((uint8_t) 0xBB) //!< flag requesting to go into Idle mode
#define ECC108_SWI_FLAG_SLEEP   ((uint8_t) 0xCC) //!< flag requesting to go into Sleep mode


/** \brief This SWI function initializes the hardware.
 */
void ecc108p_init(void)
{
	swi_enable();
}


/** \brief This SWI function selects the GPIO pin used for communication.
 *
 *         It has no effect when using a UART.
 * \param[in] id index into array of pins
 */
void ecc108p_set_device_id(uint8_t id)
{
	swi_set_device_id(id);
}


/** \brief This SWI function sends a command to the device.
	Device versions <= 0x100 need the flag to last longer than 500 us.
	Therefore, we send a dummy flag of 0 before sending the command flag.
 * \param[in] count number of bytes to send
 * \param[in] command pointer to command buffer
 * \return status of the operation
 */
uint8_t ecc108p_send_command(uint8_t count, uint8_t *command)
{
	uint8_t ret_code = swi_send_byte(0);
	if (ret_code != SWI_FUNCTION_RETCODE_SUCCESS)
		return ECC108_COMM_FAIL;
	ret_code = swi_send_byte(ECC108_SWI_FLAG_CMD);
	if (ret_code != SWI_FUNCTION_RETCODE_SUCCESS)
		return ECC108_COMM_FAIL;

	return swi_send_bytes(count, command);
}


/** \brief This SWI function receives a response from the device.
 * \param[in] size number of bytes to receive
 * \param[out] response pointer to response buffer
 * \return status of the operation
 */
uint8_t ecc108p_receive_response(uint8_t size, uint8_t *response)
{
	uint8_t count_byte;
	uint8_t i;
	uint8_t ret_code;

	for (i = 0; i < size; i++)
		response[i] = 0;

	(void) swi_send_byte(ECC108_SWI_FLAG_TX);

	ret_code = swi_receive_bytes(size, response);
	if (ret_code == SWI_FUNCTION_RETCODE_SUCCESS || ret_code == SWI_FUNCTION_RETCODE_RX_FAIL) {
		count_byte = response[ECC108_BUFFER_POS_COUNT];
		if ((count_byte < ECC108_RSP_SIZE_MIN) || (count_byte > size))
			return ECC108_INVALID_SIZE;

		return ECC108_SUCCESS;
	}

	// Translate error so that the Communication layer
	// can distinguish between a real error or the
	// device being busy executing a command.
	if (ret_code == SWI_FUNCTION_RETCODE_TIMEOUT)
		return ECC108_RX_NO_RESPONSE;
	else
		return ECC108_RX_FAIL;
}


/** \brief This SWI function generates a Wake-up pulse and delays.
 * \return success
*/
uint8_t ecc108p_wakeup(void)
{
	swi_set_signal_pin(0);
	delay_10us(ECC108_WAKEUP_PULSE_WIDTH);
	swi_set_signal_pin(1);
	delay_10us(ECC108_WAKEUP_DELAY);
	return ECC108_SUCCESS;
}


/** \brief This SWI function puts the device into idle state.
 * \return status of the operation
 */
uint8_t ecc108p_idle()
{
	return swi_send_byte(ECC108_SWI_FLAG_IDLE);
}


/** \brief This SWI function puts the device into low-power state.
 *  \return status of the operation
 */
uint8_t ecc108p_sleep() {
	return swi_send_byte(ECC108_SWI_FLAG_SLEEP);
}


/** \brief This SWI function is only a dummy since the
 *         functionality does not exist for the SWI
 *         version of the ECC108 device.
 * \return success
 */
uint8_t ecc108p_reset_io(void)
{
	return ECC108_SUCCESS;
}


/** \brief This function re-synchronizes communication.
 *
  Re-synchronizing communication is done in a maximum of five
  steps listed below. This function implements the first three steps.
  Since steps 4 and 5 (sending a Wake-up token and reading the
  response) are the same for TWI and SWI, they are implemented
  in the communication layer (#ecc108c_resync).\n
  If the chip is not busy when the system sends a transmit flag,
  the chip should respond within t_turnaround. If t_exec has not
  already passed, the chip may be busy and the system should poll or
  wait until the maximum tEXEC time has elapsed. If the chip still
  does not respond to a second transmit flag within t_turnaround,
  it may be out of synchronization. At this point the system may
  take the following steps to reestablish communication:
  <ol>
     <li>Wait t_timeout.</li>
     <li>Send the transmit flag.</li>
     <li>
       If the chip responds within t_turnaround,
       then the system may proceed with more commands.
     </li>
     <li>Send a Wake token, wait t_whi, and send the transmit flag.</li>
     <li>
       The chip should respond with a 0x11 return status within
       t_turnaround, after which the system may proceed with more commands.
     </li>
   </ol>

 * \param[in] size size of rx buffer
 * \param[out] response pointer to response buffer
 * \return status of the operation
 */
uint8_t ecc108p_resync(uint8_t size, uint8_t *response)
{
	delay_ms(ECC108_SYNC_TIMEOUT);
	return ecc108p_receive_response(size, response);
}

/** @} */
