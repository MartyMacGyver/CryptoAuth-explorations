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
 *  \brief  Application Examples that Use the SHA204 Library
 *  \author Atmel Crypto Products
 *  \date   May 31, 2012
 *
 *   Example functions are given that demonstrate the device.
 *
 *   sha204e_fixed_challenge_response:
 *      Demonstrates a fixed challenge / response authentication scheme.
 *
 *   CAUTION WHEN DEBUGGING: Be aware of the timeout feature of the device. The
 *   device will go to sleep between 0.7 and 1.5 seconds after a Wakeup. When
 *   hitting a break point, this timeout will likely to kick in and the device
 *   has gone to sleep before you continue debugging. Therefore, after you have
 *   examined variables you might have to restart your debug session.
*/

#include <string.h>                      // needed for memset()

#include "sha204_lib_return_codes.h"     // declarations of function return codes
#include "sha204_command_marshaling.h"   // definitions and declarations for the Command Marshaling module
#include "sha204_examples.h"             // definitions and declarations for example functions


/** 
 * \brief  This macro brings a device from Idle mode into Sleep mode by 
 *         waking it up and sending a Sleep flag.
 */
#define sha204e_wakeup_sleep()   {sha204p_wakeup(); sha204p_sleep();}

uint8_t sha204e_wakeup_device(uint8_t device_id);
uint8_t sha204e_check_response_status(uint8_t ret_code, uint8_t *response);
void sha204e_sleep(void);
uint8_t sha204e_configure(uint8_t parent_id);


/** 
 * \brief This function wraps \ref sha204p_sleep().
 *        It puts both devices to sleep if two devices (client and host) are used.
 *        This function is also called when a Wakeup did not succeed. 
 *        This would not make sense if a device did not wakeup and it is the only
 *        device on SDA, but if there are two devices (client and host) that
 *        share SDA, the device that is not selected might have woken up.
 */
void sha204e_sleep(void) 
{
#if defined(SHA204_I2C) && (SHA204_CLIENT_ADDRESS != SHA204_HOST_ADDRESS)
	// Select host device...
	sha204p_set_device_id(SHA204_HOST_ADDRESS);
	// and put it to sleep.
	(void) sha204p_sleep();
	// Select client device...
	sha204p_set_device_id(SHA204_CLIENT_ADDRESS);
	// and put it to sleep.
	(void) sha204p_sleep();
#else	
	(void) sha204p_sleep();
#endif
}


/** \brief This function wakes up two I2C devices and puts one back to
           sleep, effectively waking up only one device among two that
		   share SDA.
	\param[in] device_id which device to wake up
	\return status of the operation
*/
uint8_t sha204e_wakeup_device(uint8_t device_id)
{
	uint8_t ret_code;
	uint8_t wakeup_response[SHA204_RSP_SIZE_MIN];

	sha204p_set_device_id(device_id);

	// Wake up the devices.
	memset(wakeup_response, 0, sizeof(wakeup_response));
	ret_code = sha204c_wakeup(wakeup_response);
	if (ret_code != SHA204_SUCCESS) {
		sha204e_sleep();
		return ret_code;
	}

	// SHA204 I2C devices share SDA. We have to put the other device back to sleep.
	// Select other device...
	sha204p_set_device_id(device_id == SHA204_CLIENT_ADDRESS ? SHA204_HOST_ADDRESS : SHA204_CLIENT_ADDRESS);
	// and put it to sleep.
	ret_code = sha204p_sleep();

	return ret_code;	
}


/** \brief This function checks the response status byte and puts the device
           to sleep if there was an error.
    \param[in] ret_code return code of function
	\param[in] response pointer to response buffer
	\return status of the operation
*/
uint8_t sha204e_check_response_status(uint8_t ret_code, uint8_t *response)
{
	if (ret_code != SHA204_SUCCESS) {
		sha204p_sleep();
		return ret_code;
	}
	ret_code = response[SHA204_BUFFER_POS_STATUS];
	if (ret_code != SHA204_SUCCESS)
		sha204p_sleep();

	return ret_code;	
}


/** \brief This function serves as an example for a fixed
 *         challenge / response authentication.
 *
 * \return status of the operation
 */
uint8_t sha204e_fixed_challenge_response(void)
{
	uint8_t ret_code;
	uint8_t i;
	struct sha204_command_parameters command_args;
	static uint8_t wakeup_response[SHA204_RSP_SIZE_MIN];
	static uint8_t tx_buffer[MAC_COUNT_LONG];
	static uint8_t rx_buffer[MAC_RSP_SIZE];
	char *challenge = "Here comes the client challenge.";
	const uint8_t expected_response[] = {
		0x23, // count of Mac response packet: count(1) + MAC(32) + CRC(2)
		0x13, 0xE0, 0xCE, 0xA1, 0xA0, 0x12, 0xC6, 0x18, 0x73, 0x30, 0x5E, 0x6A, 0x68, 0xCA, 0x92, 0xD6, 
		0x4E, 0x40, 0x81, 0x46, 0x29, 0x2B, 0x8B, 0x82, 0xFB, 0xC4, 0x1E, 0x2D, 0x0D, 0x54, 0x60, 0x36,
		0x0C, 0x89};

	// The following command sequence wakes up the device, issues a Mac command 
	// using the Command Marshaling layer, and puts the device to sleep.
	// At the end of the function the actual Mac response is compared with the
	// expected one and the result of the comparison is returned.

	// Wake up the device and receive the wakeup response.
	memset(wakeup_response, 0, sizeof(wakeup_response));
	ret_code = sha204c_wakeup(wakeup_response);
	if (ret_code != SHA204_SUCCESS) {
		(void) sha204p_sleep();
		return ret_code;
	}			
		
	// Issue a Mac command.
	memset(rx_buffer, 0, sizeof(rx_buffer));
	command_args.op_code = SHA204_MAC;
	command_args.param_1 = MAC_MODE_CHALLENGE;
	command_args.param_2 = SHA204_KEY_ID;
	command_args.data_len_1 = MAC_CHALLENGE_SIZE;
	command_args.data_1 = (uint8_t *) challenge;
	command_args.data_len_2 = 0;
	command_args.data_2 = NULL;
	command_args.data_len_3 = 0;
	command_args.data_3 = NULL;
	command_args.tx_size = sizeof(tx_buffer);
	command_args.tx_buffer = tx_buffer;
	command_args.rx_size = sizeof(rx_buffer);
	command_args.rx_buffer = rx_buffer;
	ret_code = sha204m_execute(&command_args);

	// Put device to sleep.
	sha204p_sleep();

	if (ret_code != SHA204_SUCCESS)
		return ret_code;
		
	// Compare the Mac response with the expected one.
	// We finish the comparison even if it fails to prevent a timing attack.
	for (i = 1; i < MAC_CHALLENGE_SIZE + 1; i++) {
		if (rx_buffer[i] != expected_response[i])
			ret_code = SHA204_FUNC_FAIL;
	}
	//Using memcmp would allow a timing attack.
	//return (memcmp(rx_buffer, expected_response, sizeof(expected_response)) ? SHA204_FUNC_FAIL : SHA204_SUCCESS);
	return ret_code;
}
