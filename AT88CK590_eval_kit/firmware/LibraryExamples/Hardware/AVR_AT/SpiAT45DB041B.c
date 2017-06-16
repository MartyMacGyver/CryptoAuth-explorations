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
            that support a AT45DB041B EEPROM device that is used on an STK600.

            It was written to test the SPI driver before it is used to support
            an AT88AES1xx device.

 *  \date 	April 15, 2010
 */

#include <stdint.h>
#include "AES1_Status.h"
#include "SpiPhys.h"
#include "AES1_Physical.h"


//! read-continuous-array command id
#define AT45_SPI_READ_CONTINUOUS (0xE8)

//! read-buffer1 command id
#define AT45_SPI_READ_BUFFER1    (0xD4)

//! write-through-buffer command id
#define AT45_SPI_WRITE_THROUGH   (0x82)

//! write-buffer1   command id
#define AT45_SPI_WRITE_BUFFER1   (0x84)

//! read-status command id
#define AT45_SPI_READ_STATUS     (0xD7)

//! write-in-progress status bit
#define AT45_SPI_READY			   (1 << 7)

//!
#define AT45_DEFAULT_STATUS      (0x9C)

//! This value is decremented in wait and retry loops.
#define AT45_SPI_TIMEOUT		   (250)


/** \brief This function initializes and enables the SPI driver. */
void AES1P_EnableDeviceDriver()
{
	SpiEnableDeviceDriver();
}


/** \brief This function disables the SPI driver. */
void AES1P_DisableDeviceDriver()
{
	SpiDisableDeviceDriver();
}


/** \brief This function reads the status from the device.

      At startup, we should see a value of AT45_DEFAULT_STATUS
      in "deviceStatus".
 * \param[out] status device status
 * \return status of the operation
 */
int8_t AES1P_ReadStatus(uint8_t *deviceStatus)
{
	uint8_t readStatusCommand = AT45_SPI_READ_STATUS;
	int8_t status = AES1_STATUS_TIMEOUT;

	// Adjust this value if it is too small when running on a fast CPU.
	uint8_t timeout = AT45_SPI_TIMEOUT;

	while (timeout--) {
		SpiSelectSlave();
		SpiSendBytes(1, &readStatusCommand);
      // It takes about 8 us from the command bit until the first
      // response bit on an ATmega2560 clocking at 16 MHz.
		status = SpiReceiveBytes(1, deviceStatus);
		SpiDeselectSlave();

		if (status == AES1_STATUS_SUCCESS)
			break;
	}
	return status;
}


/** \brief This function checks for the ready flag in the status register.
 *         It will also wake up a device.
 * \return status of the operation
 */
int8_t AES1P_WaitForReady(void)
{
	// Adjust this value if it is too small when running on a fast CPU.
	uint8_t timeout = AT45_SPI_TIMEOUT;

	uint8_t deviceStatus = 0;
	int8_t status = AES1_STATUS_TIMEOUT;

	while (timeout-- && !(deviceStatus & AT45_SPI_READY)) {
		status = AES1P_ReadStatus(&deviceStatus);
		if (status != AES1_STATUS_SUCCESS)
			return status;
	}
	if (!timeout)
		return AES1_STATUS_TIMEOUT;

	return status;
}


/** \brief This function writes bytes to buffer 1.
 * \param[in] count number of bytes to write
 * \param[in] addressHigh high byte of address
 * \param[in] addressLow low byte of address
 * \param[in] data pointer to tx buffer
 * \return status of the operation
 */
int8_t AES1P_WriteMemory(uint8_t count, uint8_t addressHigh, uint8_t addressLow, uint8_t *data)
{
	int8_t status = AES1_STATUS_TIMEOUT;
	uint8_t preface[4];

	if (!count || !data)
		return AES1_STATUS_BAD_PARAM;

	status = AES1P_WaitForReady();
	if (status != AES1_STATUS_SUCCESS)
		return status;

   // Just for testing SPI. The AT45DB041B preface is:
   // <op-code, 8 bits> <not used, 15 bits> <address, 9 bits>
	preface[0] = AT45_SPI_WRITE_BUFFER1;
   preface[1] = 0;
	preface[2] = addressHigh & 1;
	preface[3] = addressLow;

	SpiSelectSlave();
	SpiSendBytes(sizeof(preface), preface);
	SpiSendBytes(count, data);
	SpiDeselectSlave();

	return status;
}


/** \brief This function reads bytes from buffer 1.
 * \param[in] count number of bytes to write
 * \param[in] addressHigh high byte of address
 * \param[in] addressLow low byte of address
 * \param[out] data pointer to rx buffer
 * \return status of the operation
 */
int8_t AES1P_ReadMemory(uint8_t count, uint8_t addressHigh, uint8_t addressLow, uint8_t *data)
{
	uint8_t preface[5] = {AT45_SPI_READ_BUFFER1, 0, 0, 0, 0};
   int8_t status = AES1_STATUS_TIMEOUT;

	if (!count || !data)
		return AES1_STATUS_BAD_PARAM;

	status = AES1P_WaitForReady();
	if (status != AES1_STATUS_SUCCESS)
		return status;

   // Just for testing SPI. The AT45DB041B preface is:
   // <op-code, 8 bits> <not used, 15 bits> <address, 9 bits> <not used, 8 bits>
	preface[2] = addressHigh & 1; // Only LSB is valid. Nine address bits in total for buffers.
	preface[3] = addressLow;

	SpiSelectSlave();
	SpiSendBytes(sizeof(preface), preface);
	SpiReceiveBytes(count, data);
	SpiDeselectSlave();

	return status;
}
