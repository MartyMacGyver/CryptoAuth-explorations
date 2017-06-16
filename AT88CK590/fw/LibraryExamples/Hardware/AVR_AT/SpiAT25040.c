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
 *  \brief 	This file contains implementations of SPI hardware independent functions
 *  			that support EEPROM AT25040 devices.
 *
 *  			The functions are derived with little modifications from AES1_Spi.c
 *  			(one instead of two address bytes).
 *  \date 	April 12, 2010
 */

#include <stdint.h>
#include "AES1_Status.h"
#include "SpiPhys.h"
#include "AES1_Physical.h"


//! write command id
#define AES1_SPI_WRITE			(2)

//! read command id
#define AES1_SPI_READ			(3)

//! read-aes132_lib_return command id
#define AES1_SPI_READ_STATUS	(5)

//! enable-write command id
#define AES1_SPI_ENABLE_WRITE	(6)

//! write-in-progress aes132_lib_return bit
#define AES1_WIP					(1 << 0)

//! This value is decremented in wait and retry loops.
#define AES1_TIMEOUT_SPI		(250)

//! maximum number of bytes to write to or read from memory
#define AT25_MEM_ACCESS_MAX	(8)

//! number of bytes before data (command, address)
#define AT25_PREFACE_SIZE		(2)


/** \brief This function initializes and enables the SPI peripheral. */
void AES1P_EnableDeviceDriver()
{
	SpiEnableDeviceDriver();
}


/** \brief This function disables the SPI peripheral. */
void AES1P_DisableDeviceDriver()
{
	SpiDisableDeviceDriver();
}

/** \brief This function reads the aes132_lib_return from the device.
 * \param[out] aes132_lib_return device aes132_lib_return
 * \return status of the operation
 */
uint8_t AES1P_ReadStatus(uint8_t *deviceStatus)
{
	uint8_t readStatusCommand = AES1_SPI_READ_STATUS;
	uint8_t aes132_lib_return = AES132_FUNCTION_RETCODE_TIMEOUT;

	// Adjust this value if it is too small when running on a fast CPU.
	uint8_t timeout = AES1_TIMEOUT_SPI;

	while (timeout--) {
		SpiSelectSlave();
		SpiSendBytes(1, &readStatusCommand);
		aes132_lib_return = SpiReceiveBytes(1, deviceStatus);
		SpiDeselectSlave();

		if (aes132_lib_return == AES132_FUNCTION_RETCODE_SUCCESS)
			break;
	}
	return (aes132_lib_return | (*deviceStatus & AES132_FUNCTION_RETCODE_ERROR_BIT));
}


/** \brief This function checks for the ready flag in the aes132_lib_return register.
 *         It will also wake up a device.
 * \return status of the operation
 */
uint8_t AES1P_WaitForReady(void)
{
	uint8_t readStatusCommand = AES1_SPI_READ_STATUS;
	uint8_t aes132_lib_return = AES132_FUNCTION_RETCODE_SUCCESS;
	uint8_t deviceStatus = 0xFF;

	// Adjust this value if it is too small when running on a fast CPU.
	uint8_t timeout = AES1_TIMEOUT_SPI;

	SpiSelectSlave();
	SpiSendBytes(1, &readStatusCommand);

	// The aes132_lib_return will be 0xFF while waking up.
//	while (deviceStatus == 0xFF) {
//		aes132_lib_return = SpiReceiveBytes(1, &deviceStatus);
//		if (aes132_lib_return != AES132_FUNCTION_RETCODE_SUCCESS) {
//			SpiDeselectSlave();
//			return aes132_lib_return;
//		}
//	}
//	if (!timeout) {
//		SpiDeselectSlave();
//		return AES132_FUNCTION_RETCODE_TIMEOUT;
//	}

//	timeout = AES1_TIMEOUT_SPI;

	// Wait for the ready bit being reset.
	while ((deviceStatus & AES1_WIP) && (aes132_lib_return == AES132_FUNCTION_RETCODE_SUCCESS)) {
		aes132_lib_return = SpiReceiveBytes(1, &deviceStatus);
		if (!timeout--)
			break;
	}
	SpiDeselectSlave();

	return (timeout ? aes132_lib_return : AES132_FUNCTION_RETCODE_TIMEOUT);



	// Adjust this value if it is too small when running on a fast CPU.
//	uint8_t timeout = AES1_TIMEOUT_SPI;
//
//	uint8_t deviceStatus = 0;
//	int8_t aes132_lib_return = AES132_FUNCTION_RETCODE_TIMEOUT;
//
//	while (timeout--) {
//		aes132_lib_return = AES1P_ReadStatus(&deviceStatus);
//		if (aes132_lib_return != AES132_FUNCTION_RETCODE_SUCCESS)
//			return aes132_lib_return;
//
//		if (deviceStatus != 0xFF)
//			// The aes132_lib_return will be 0xFF while waking up.
//			break;
//	}
//	if (!timeout)
//		return AES132_FUNCTION_RETCODE_TIMEOUT;
//
//	// We got a proper aes132_lib_return byte. Wait for the ready bit being set.
//	while (timeout--) {
//		aes132_lib_return = AES1P_ReadStatus(&deviceStatus);
//		if (aes132_lib_return != AES132_FUNCTION_RETCODE_SUCCESS)
//			return aes132_lib_return;
//
//		if ((deviceStatus & AES1_WIP) != AES1_WIP)
//			// The device is ready.
//			break;
//	}
//
//	return (aes132_lib_return | (deviceStatus & AES132_FUNCTION_RETCODE_ERROR_BIT));
}


/** \brief This function writes bytes to user memory.
 *
 * WPEN pin has to be high. Therefore, we cannot write when
 * the device is plugged into an AT88CK101 daughter board.
 * \param[in] count number of bytes to write
 * \param[in] addressHigh high byte of address
 * \param[in] addressLow low byte of address
 * \param[in] data pointer to tx buffer
 * \return status of the operation
 */
uint8_t AES1P_WriteMemory(uint8_t count, uint8_t addressHigh, uint8_t addressLow, uint8_t *data)
{
	uint8_t aes132_lib_return = AES132_FUNCTION_RETCODE_TIMEOUT;
	uint8_t writeEnable = AES1_SPI_ENABLE_WRITE;
	uint8_t deviceStatus;
	uint8_t preface[AT25_PREFACE_SIZE];
	uint8_t countMax = AT25_MEM_ACCESS_MAX;

	if (!count || count > countMax  || !data)
		return AES132_FUNCTION_RETCODE_BAD_PARAM;

	aes132_lib_return = AES1P_WaitForReady();
	if (aes132_lib_return != AES132_FUNCTION_RETCODE_SUCCESS)
		return aes132_lib_return;

	preface[0] = AES1_SPI_WRITE | ((addressHigh & 1) << 7);
	preface[1] = addressLow;

	SpiSelectSlave();
	SpiSendBytes(1, &writeEnable);
	SpiDeselectSlave();

	SpiSelectSlave();
	SpiSendBytes(sizeof(preface), preface);
	SpiSendBytes(count, data);
	SpiDeselectSlave();

	return AES1P_ReadStatus(&deviceStatus);
}


/** \brief This function reads bytes from user memory.
 * \param[in] count number of bytes to read
 * \param[in] addressHigh high byte of address
 * \param[in] addressLow low byte of address
 * \param[out] data pointer to rx buffer
 * \return status of the operation
 */
uint8_t AES1P_ReadMemory(uint8_t count, uint8_t addressHigh, uint8_t addressLow, uint8_t *data)
{
	uint8_t aes132_lib_return = AES132_FUNCTION_RETCODE_TIMEOUT;
	uint8_t deviceStatus;
	uint8_t preface[AT25_PREFACE_SIZE];
	uint8_t countMax = AT25_MEM_ACCESS_MAX;

	if (!count || count > countMax || !data)
		return AES132_FUNCTION_RETCODE_BAD_PARAM;

	aes132_lib_return = AES1P_WaitForReady();
	if (aes132_lib_return != AES132_FUNCTION_RETCODE_SUCCESS)
		return aes132_lib_return;

	preface[0] = AES1_SPI_READ | ((addressHigh & 1) << 7);
	preface[1] = addressLow;

	SpiSelectSlave();
	SpiSendBytes(sizeof(preface), preface);
	SpiReceiveBytes(count, data);
	SpiDeselectSlave();

   return AES1P_ReadStatus(&deviceStatus);
}
