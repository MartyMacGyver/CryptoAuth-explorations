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
 *  \brief Test Functions for the SHA204 Library
 *  \author Atmel Crypto Products
 *  \date   October 20, 2010
*/
#include <avr/io.h>
#include <stddef.h>
#include <string.h>

#include "timer_utilities.h"
#include "SHA_Tests.h"
#include "sha204_comm_marshaling.h"
#include "sha204_physical.h"


static uint8_t tx_buffer[SHA204_CMD_SIZE_MAX];      //!< command buffer
static uint8_t rx_buffer[SHA204_RSP_SIZE_MAX];   //!< response buffer


/** \brief This function calculates a CRC.
 *
 * \param[in] data pointer to data for which CRC should be calculated
 * \param[in] length number of bytes in buffer
 * \param[out] crc pointer to 16-bit CRC
 */
void sha204t_calculate_crc(uint8_t length, uint8_t *data, uint8_t *crc) {
	uint8_t counter;
	uint8_t crcLow = 0, crcHigh = 0, crcCarry;
	uint8_t polyLow = 0x05, polyHigh = 0x80;
	uint8_t shiftRegister;
	uint8_t dataBit, crcBit;

	for (counter = 0; counter < length; counter++) {
	  for (shiftRegister = 0x01; shiftRegister > 0x00; shiftRegister <<= 1) {
		 dataBit = (data[counter] & shiftRegister) ? 1 : 0;
		 crcBit = crcHigh >> 7;

		 // Shift CRC to the left by 1.
		 crcCarry = crcLow >> 7;
		 crcLow <<= 1;
		 crcHigh <<= 1;
		 crcHigh |= crcCarry;

		 if ((dataBit ^ crcBit) != 0) {
			crcLow ^= polyLow;
			crcHigh ^= polyHigh;
		 }
	  }
	}
	crc[0] = crcLow;
	crc[1] = crcHigh;
}


/** \brief This function checks the consistency of a response.
 *
 * @param response pointer to response
 * @return status of the consistency check
 */
uint8_t sha204t_check_response(uint8_t *response)
{
	if (!response)
		return SHA204_BAD_PARAM;

	uint8_t crc[SHA204_CRC_SIZE];
	uint8_t count = response[SHA204_BUFFER_POS_COUNT];

	if ((count < SHA204_RSP_SIZE_MIN) || (count > SHA204_RSP_SIZE_MAX))
		return SHA204_INVALID_SIZE;

	count -= SHA204_CRC_SIZE;
	sha204t_calculate_crc(count, response, crc);

	return (crc[0] == response[count] && crc[1] == response[count + 1])
		? SHA204_SUCCESS : SHA204_BAD_CRC;
}


#if 0
// This function was used for calibrating the uint32_t delay function SHAP_Delay.
/** \brief This test helps to establish filler code to achieve a 1 us iteration
 *         for a uint8_t decrementing while loop.
 *	\return counter value of last test step
 */
uint16_t SHAT_TestTimeLoop()
{
	volatile uint16_t i;
	uint32_t testDelay[10] = {8, 50, 100, 200, 2500, 30000, 100000, 500000, 1000000, 3840000};
	volatile uint8_t testStep;
	volatile uint8_t startValue = 250;

	// clear OCF1A / clear pending interrupts
	TIFR1 |= _BV(OCF1A);

	// Disable clock source.
	TCCR1B &= ~(_BV(CS10) | _BV(CS11) | _BV(CS12));
	TCNT1 = 0;

	// select timer1 clock source (synchronous - CK / 8)
	TCCR1B |= _BV(CS11);

	do {
		// These many NOPs as filler creates a 1 us loop for an AT90USB1287
		// running at 16 MHz. Use the same filler in SHAP_Delay.
		asm volatile("nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop");
	} while (startValue--);

	// Stop timer.
	TCCR1B &= ~_BV(CS11);
	i = TCNT1; // Should be 500 for CPU clock of 16 MHz.

	// Measure accuracy of SHAP_Delay function with various values <= 30000.
	for (testStep = 0; testStep < 6; testStep++) {
		TCNT1 = 0;
		TCCR1B |= _BV(CS11);
		SHAP_Delay(testDelay[testStep]);
		TCCR1B &= ~_BV(CS11);
		i = TCNT1;
	}

	// Measure accuracy of SHAP_Delay function with various values > 30000.
	// select timer1 clock source (synchronous - CK / 1024)
	for (; testStep < 10; testStep++) {
		TCNT1 = 0;
		TCCR1B |= (_BV(CS10) | _BV(CS12));
		SHAP_Delay(testDelay[testStep]);
		TCCR1B &= ~(_BV(CS10) | _BV(CS12));
		i = TCNT1;
	}

	return i;
}
#endif

/** \brief This function tests communication by generating a Wakeup sequence
 *         and reading the response.
 * \return status of the test
 * */
uint8_t sha204t_test_wakeup()
{
	// Send wakeup token.
	uint8_t status = sha204p_wakeup();
	if (status != SHA204_SUCCESS)
		return status;

	// Receive response.
	uint8_t rxBuffer[SHA204_RSP_SIZE_MIN] = {0, 0, 0, 0};
	status = sha204p_receive_response(SHA204_RSP_SIZE_MIN, rxBuffer);

	// Send sleep flag.
	sha204p_sleep();
	if (status != SHA204_SUCCESS)
		return status;

	// Verify response.
	if (rxBuffer[0] != SHA204_RSP_SIZE_MIN)
		status = SHA204_INVALID_SIZE;
	else if (rxBuffer[1] != SHA204_STATUS_BYTE_WAKEUP)
		status = SHA204_COMM_FAIL;

	return status;
}


/** \brief This test measures the watchdog timeout of the device.
 * Wakes up the device and reads its status until reading fails.
 * The function then verifies that the variable "watchdogCount"
 * lies between 700 and 1500 (ms).
 * \return status of the test
 * */
uint8_t sha204t_test_watchdog_timeout()
{
	uint8_t status;

	sha204p_set_device_id(0);

	// Send wakeup token.
	status = sha204p_wakeup();
	if (status != SHA204_SUCCESS)
		return status;

	// Receive response.
	uint8_t rxBuffer[SHA204_RSP_SIZE_MIN] = {0, 0, 0, 0};
	uint16_t watchDogCount = 0;
	do {
		status = sha204p_receive_response(SHA204_RSP_SIZE_MIN, rxBuffer);
		delay_10us(100);
		watchDogCount++;
	} while (status == SHA204_SUCCESS);

	// Send sleep flag.
	sha204p_sleep();
	if (watchDogCount < 700 || watchDogCount > 1500)
		status = SHA204_GEN_FAIL;

	return status;
}


/** \brief This function tests communication by sending a command and receiving a response.
 * 	\return status of the operation
 * */
uint8_t sha204t_test_communication()
{
	uint8_t rxSize = SHA204_RSP_SIZE_MIN;
	uint8_t rxBuffer[DEVREV_RSP_SIZE] = {0, 0, 0, 0, 0, 0, 0};

	// Send wakeup token and receive response.
	uint8_t status = sha204c_wakeup(rxBuffer);
	if (status != SHA204_SUCCESS)
		return status;

	// Send DevRev (device revision) command.
	uint8_t cmd[DEVREV_COUNT] = {DEVREV_COUNT, SHA204_DEVREV, 0, 0, 0, 0x03, 0x5D};
	status = sha204p_send_command(sizeof(cmd), cmd);
	if (status != SHA204_SUCCESS)
		return status;

	// command execution delay
	delay_10us(110);

	// Receive response.
	rxSize = DEVREV_RSP_SIZE;
	status = sha204p_receive_response(rxSize, rxBuffer);
	sha204p_sleep();
	if (status != SHA204_SUCCESS)
		return status;

	// Verify response to the Read command.
	if (rxBuffer[SHA204_BUFFER_POS_COUNT] != rxSize)
		return SHA204_INVALID_SIZE;

	return sha204t_check_response(rxBuffer);
}


/** \brief Tests communication by sending a command and receiving a response.
 * 	\return status of the test
 */
uint8_t sha204t_test_send_and_receive()
{
	uint8_t response[RANDOM_RSP_SIZE];
	uint8_t ret_code = sha204c_wakeup(response);
	if (ret_code != SHA204_SUCCESS) {
		ret_code = sha204p_resync(RANDOM_RSP_SIZE, response);
		return ret_code;
	}

	// Random command
	uint8_t cmd[SHA204_CMD_SIZE_MIN] = {SHA204_CMD_SIZE_MIN, SHA204_RANDOM, RANDOM_NO_SEED_UPDATE, 0, 0, 0, 0};

	//uint8_t ret_code = sha204c_send_and_receive(cmd, sizeof(response), response, RANDOM_DELAY, RANDOM_TIMEOUT);
	// In unlocked mode, the execution time for the Random command is only 6.5 ms.
	ret_code = sha204c_send_and_receive(cmd, sizeof(response), response,
				RANDOM_DELAY, RANDOM_EXEC_MAX - RANDOM_DELAY);

	sha204p_sleep();

	return ret_code;
}


/** \brief This function tests the TempSense command.
 * 	\return status of the operation
 * */
uint8_t sha204t_test_command_tempsense()
{
	sha204p_wakeup();

	// Read RevNum from configuration zone.
	// If bit 7 of byte 4 is not set, TempSense is not supported.
	uint8_t cmd[READ_COUNT] = {READ_COUNT, SHA204_READ, 0, 1, 0, 0, 0};
	uint8_t response[READ_4_RSP_SIZE];

	uint8_t status = sha204c_send_and_receive(cmd, sizeof(response), response,
				TEMP_SENSE_DELAY, TEMP_SENSE_EXEC_MAX - TEMP_SENSE_DELAY);
	if (status != SHA204_SUCCESS) {
		sha204p_sleep();
		return status;
	}
	if (!(response[SHA204_BUFFER_POS_DATA + 3] & 0x80)) {
		// TempSense command is not supported.
		sha204p_sleep();
		return status;
	}

	// TempSense command
	cmd[1] = 24;	// op-code
	cmd[3] = 0;		// param2 low byte
	status = sha204c_send_and_receive(cmd, sizeof(response), response, TEMP_SENSE_DELAY,
				TEMP_SENSE_EXEC_MAX - TEMP_SENSE_DELAY);

	sha204p_sleep();

	return status;
}


/** \brief This functions tests compiling and building
 *         the Marshaling layer module.
 * @return status of the test
 */
uint8_t sha204t_test_marshaling_module()
{
	sha204p_wakeup();

	uint8_t status = sha204m_execute(SHA204_DERIVE_KEY, 0, 0, 0, NULL, 0, NULL, 0, NULL,
				SHA204_CMD_SIZE_MIN, tx_buffer, SHA204_RSP_SIZE_MIN, rx_buffer);

	sha204p_sleep();

	if (status != SHA204_SUCCESS)
		return status;

	return sha204t_check_response(rx_buffer);
}

