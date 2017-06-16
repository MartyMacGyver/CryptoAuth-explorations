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

/** \file
 * Microbase module for discovering devices.
 * \author Atmel Crypto Products
 * \date June 24, 2013
*/

#include <avr/io.h>
#include <string.h>

#include "config.h"
#include "Combined_Discover.h"
#include "delay_x.h"
#include "i2c_phys.h"

#include "aes132_lib_return_codes.h"
#include "aes132_commands.h"
#include "aes132_physical.h"

#include "sha204_comm_marshaling.h"
#include "sha204_comm.h"
#include "swi_phys.h"
#include "sha204_lib_return_codes.h"
#include "hardware.h"            // Microbase related functions (LED, buzzer, button)


#if TARGET_BOARD==0
#   error You have to define TARGET_BOARD > 0
#endif


//! This macro updates the device type.
#define UPDATE_DEVICE_TYPE device_info[device_count].device_type = \
		device_info[device_count].dev_rev[2] == 0 ? DEVICE_TYPE_SHA204 : DEVICE_TYPE_ECC108


// declared in sha204_comm.c
extern void sha204c_calculate_crc(uint8_t length, uint8_t *data, uint8_t *crc);

// declared in sha204_bitbang_physical.c
extern uint8_t swi_get_pin_array_size();


uint8_t receivebuf[SHA204_RSP_SIZE_MAX];

uint8_t sha204_devrev_command[SHA204_CMD_SIZE_MIN] = {
			SHA204_CMD_SIZE_MIN, SHA204_DEVREV, 0, 0, 0, 0x03, 0x5D};
uint8_t sa10_genkey_command[23] = {
			23, 32, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0x00, 0x00};
device_info_t device_info[DISCOVER_DEVICE_COUNT_MAX];
uint8_t device_count = 0;


device_info_t *GetDeviceInfo(uint8_t index) {
	if (index >= device_count)
		return NULL;
	return &device_info[index];
}


device_type_t GetDeviceType(uint8_t index) {
	if (index >= device_count)
		return DEVICE_TYPE_UNKNOWN;
	return device_info[index].device_type;
}


/** This function checks the consistency of a SHA204 response.
 * \param[in] response pointer to response buffer
 * \return status of the operation
 */
uint8_t VerifyResponse(uint8_t *response)
{
	uint8_t crc[SHA204_CRC_SIZE];

	// Check count byte.
	uint8_t count = response[SHA204_COUNT_IDX];
	if (count < SHA204_RSP_SIZE_MIN || count > SHA204_RSP_SIZE_MAX)
		return SHA204_INVALID_SIZE;

	// Check CRC.
	count -= SHA204_CRC_SIZE;
	sha204c_calculate_crc(count, response, crc);
	return (*((uint16_t *) crc) == *((uint16_t *) (response + count)))
		? SHA204_SUCCESS : SHA204_BAD_CRC;
}


/** \brief This function tries to detect a SHA204 or ECC108 SWI device.
 *
 *         This function assumes the bit-banging version of the SWI
 *         (sha204_bitbang_physical.c).
 *         Therefore we do not need to call an initialization function.
 *  \return number of devices found
 */
uint8_t DetectSwiDevices(void)
{
	if (device_count >= DISCOVER_DEVICE_COUNT_MAX)
		return 0;

	static uint8_t lib_return;
	uint8_t command[SHA204_CMD_SIZE_MIN];
	uint8_t response[SHA204_RSP_SIZE_VAL];
	uint8_t device_id;
	uint8_t pin_count = swi_get_pin_array_size();

	// ------------------ Detect SWI devices. -----------------

	// Set the interface.
	sha204p_set_interface(DEVKIT_IF_SWI);

	// Wakeup the SHA204 device and obtain its status by
	// sending a TX flag. Do this for every pin in the
	// pin array declared in sha204_bitbang_physical.c.
	// This implementation does not support two devices on the same pin.
	for (device_id = 0; device_id < pin_count; device_id++) {

		// Enable interface.
		sha204p_init();

		sha204p_set_device_id(device_id);

		//  Wake up device.
		lib_return = sha204c_wakeup(response);
		if (lib_return != SHA204_SUCCESS) {
			sha204p_sleep();
			sha204p_disable_interface();
			continue;
		}

		// Delay turn-around time.
		_delay_us(100);

		// We found a device.
		// Send a SHA204 "Device Revision" command (same as ECC108 "Info") and receive its response.
		// We don't use sha204c_send_and_receive() because this function would retry
		// even if we woke up an SA10xS device.
		lib_return = sha204p_send_command(sha204_devrev_command[SHA204_COUNT_IDX], sha204_devrev_command);
		if (lib_return == SHA204_SUCCESS) {
			_delay_ms(DEVREV_EXEC_MAX);
			lib_return = sha204p_receive_response(sizeof(response), response);
			if (lib_return == SHA204_SUCCESS) {
				// We found a SHA204 or ECC108 device.
				if (VerifyResponse(response) == SHA204_SUCCESS) {
					// We got a valid response to a DevRev command. We found a SHA204 device.
					memcpy(device_info[device_count].dev_rev, &response[SHA204_BUFFER_POS_DATA], sizeof(device_info[device_count].dev_rev));

					// Read the Selector byte from the configuration zone.
					lib_return = sha204m_execute(SHA204_READ, 0, 85 / 4, 0, NULL, 0, NULL, 0, NULL,
								sizeof(command), command, sizeof(response), response);
					if (lib_return == SHA204_SUCCESS) {
						device_info[device_count].bus_type = DEVKIT_IF_SWI;
						UPDATE_DEVICE_TYPE;
						device_info[device_count].device_index = device_id;
						device_info[device_count].address = response[2];
					}
					else
						memset(&device_info[device_count], 0, sizeof(device_info_t));
				}
			}
			else {
				// Since a device has woken up with an expected response and it did not reply
				// to a DevRev / Info command, we probably found an SA10x device. Let's confirm this
				// by sending a GenPersonizationKey command that is the same for all three
				// types of SA10x devices, SA100S, SA102S, and SA10HS.
				uint8_t count = sa10_genkey_command[SHA204_COUNT_IDX];
				uint8_t len = count - SHA204_CRC_SIZE;
				sha204c_calculate_crc(len, sa10_genkey_command, sa10_genkey_command + len);
				lib_return = sha204p_send_command(count, sa10_genkey_command);
				if (lib_return == SHA204_SUCCESS) {
					_delay_ms(13);
					lib_return = sha204p_receive_response(SHA204_RSP_SIZE_MIN, response);
					if (lib_return == SHA204_SUCCESS) {
						if (VerifyResponse(response) == SHA204_SUCCESS) {
							// We found a SA10xS device.
							device_info[device_count].bus_type = DEVKIT_IF_SWI;
							device_info[device_count].device_type = DEVICE_TYPE_SA102S;
							device_info[device_count].device_index = device_id;
						}
					}
				}
			}
		}
		sha204p_sleep();
		sha204p_disable_interface();

		if (device_info[device_count].bus_type == DEVKIT_IF_UNKNOWN)
			continue;

		if (++device_count >= DISCOVER_DEVICE_COUNT_MAX)
			break;
	}
 	return device_count;
}


/** \brief This function tries to detect an AES132 I2C device.
 * \param[in] i2c_address I2C address
 */
uint8_t DetectI2cAes132(uint8_t i2c_address)
{
	if (device_count >= DISCOVER_DEVICE_COUNT_MAX)
		return 0;

	uint8_t i2c_status, aes_status;
	uint8_t aes_command[3];

	aes_command[0] = i2c_address;
	aes_command[1] = AES132_STATUS_ADDR >> 8;
	aes_command[2] = AES132_STATUS_ADDR & 0xFF;

	// Read device status register.
	i2c_send_start();
	i2c_status = i2c_send_bytes(sizeof(aes_command), aes_command);
	if (i2c_status != I2C_FUNCTION_RETCODE_SUCCESS) {
		i2c_send_stop();
		return i2c_status;
	}
	i2c_address |= I2C_READ_FLAG;
	i2c_send_start();   // repeated start
	i2c_status = i2c_send_bytes(1, &i2c_address);   // I2C read address
	if (i2c_status != I2C_FUNCTION_RETCODE_SUCCESS) {
		i2c_send_stop();
		return i2c_status;
	}
	i2c_status = i2c_receive_bytes(1, &aes_status);
	if (i2c_status == I2C_FUNCTION_RETCODE_SUCCESS) {
		// Found AES132. Update device_info array.
		device_info[device_count].address = i2c_address & ~I2C_READ_FLAG;
		device_info[device_count].bus_type = DEVKIT_IF_I2C;
		device_info[device_count].device_type = DEVICE_TYPE_AES132;
	}
	i2c_send_stop();

	return i2c_status;
}


/** \brief This function sends a DevRev command to a SHA204 or ECC108 I2C device
 *         and receives its response.
 * \param[in] i2c_address I2C address
 * \return I2C status
 */
uint8_t DetectI2cCryptoAuth(uint8_t i2c_address)
{
	if (device_count >= DISCOVER_DEVICE_COUNT_MAX)
		return 0;

	uint8_t sha204_command[2] = {i2c_address, 3};
	uint8_t sha204_response[DEVREV_RSP_SIZE];
	uint8_t response_status;
	uint8_t wakeup_response_expected[] = {0x04, 0x11, 0x33, 0x43};
	uint8_t i2c_status = i2c_send_start();
	if (i2c_status != I2C_FUNCTION_RETCODE_SUCCESS)
		return i2c_status;

	// Receive Wakeup response.
	i2c_address |= I2C_READ_FLAG; // I2C read address
	i2c_status = i2c_send_bytes(1, &i2c_address);
	if (i2c_status != I2C_FUNCTION_RETCODE_SUCCESS) {
		i2c_send_stop();
		return i2c_status;
	}
	i2c_status = i2c_receive_bytes(SHA204_RSP_SIZE_MIN, sha204_response);
	if (i2c_status != I2C_FUNCTION_RETCODE_SUCCESS)
		return i2c_status;
	if (memcmp(sha204_response, wakeup_response_expected, SHA204_RSP_SIZE_MIN))
	   return I2C_FUNCTION_RETCODE_COMM_FAIL;

	// Send DevRev command.
	i2c_status = i2c_send_start();
	if (i2c_status != I2C_FUNCTION_RETCODE_SUCCESS)
		return i2c_status;
	i2c_status = i2c_send_bytes(sizeof(sha204_command), sha204_command);
	if (i2c_status != I2C_FUNCTION_RETCODE_SUCCESS) {
		i2c_send_stop();
		return i2c_status;
	}
	i2c_status = i2c_send_bytes(sizeof(sha204_devrev_command), sha204_devrev_command);
	i2c_send_stop();
	if (i2c_status != I2C_FUNCTION_RETCODE_SUCCESS) {
		/** \todo Should we send a Sleep command here? */
		return i2c_status;
	}

	_delay_ms(DEVREV_EXEC_MAX);

	// Receive response.
	i2c_send_start();
	i2c_status = i2c_send_bytes(1, &i2c_address);
	if (i2c_status != I2C_FUNCTION_RETCODE_SUCCESS) {
		i2c_send_stop();
		return i2c_status;
	}
	// i2c_receive_bytes sends at Stop after the last byte has been received.
	i2c_status = i2c_receive_bytes(sizeof(sha204_response), sha204_response);
	if (i2c_status != I2C_FUNCTION_RETCODE_SUCCESS)
		return i2c_status;

	// Send Sleep command.
	sha204_command[1] = 1;
	i2c_send_start();
	i2c_status = i2c_send_bytes(2, sha204_command);
	i2c_send_stop();

	// Validate response.
	response_status = VerifyResponse(sha204_response);
	if (response_status != SHA204_SUCCESS)
		return I2C_FUNCTION_RETCODE_COMM_FAIL;

	// Found SHA204. Update device_info array.
	device_info[device_count].address = i2c_address & ~I2C_READ_FLAG;
	device_info[device_count].bus_type = DEVKIT_IF_I2C;
	memcpy(device_info[device_count].dev_rev, &sha204_response[SHA204_BUFFER_POS_DATA], sizeof(device_info[device_count].dev_rev));
	UPDATE_DEVICE_TYPE;
	return i2c_status;
}


/** \brief This function loops through all possible I2C addresses
 *         and tries to a device when an address was acknowledged.
 *  \return number of devices found
 */
uint8_t DetectI2cDevices()
{
	if (device_count >= DISCOVER_DEVICE_COUNT_MAX)
		return 0;

	uint8_t i2c_address;
	uint8_t i2c_status;

	i2c_enable();

	twi_sha204p_wakeup();

	// I2C factory default address of a SHA204 is 0xC8,
	// for ECC108 it is 0xC0.
//	for (i2c_address = 255; i2c_address > 2; i2c_address -= 2) {
//		i2c_status = DetectI2cCryptoAuth(i2c_address & ~I2C_READ_FLAG);
//		if (i2c_status == I2C_FUNCTION_RETCODE_SUCCESS) {
//			if (++device_count >= DISCOVER_DEVICE_COUNT_MAX)
//				break;
//		}
		// We got an address ack from an unknown device.
//	}
		i2c_status = DetectI2cCryptoAuth(0xC0 & ~I2C_READ_FLAG);
		if (i2c_status == I2C_FUNCTION_RETCODE_SUCCESS) {
			Led3(1);  //ECC108
			if (++device_count >= DISCOVER_DEVICE_COUNT_MAX)
				return device_count;
		}
		i2c_status = DetectI2cCryptoAuth(0xC8 & ~I2C_READ_FLAG);
		if (i2c_status == I2C_FUNCTION_RETCODE_SUCCESS) {
			Led1(1);  //SHA204
			if (++device_count >= DISCOVER_DEVICE_COUNT_MAX)
				return device_count;
		}
//	if (device_count)
		// We don't support a mix of SHA204 and AES132.
//		return device_count;

//	for (i2c_address = 255; i2c_address > 2; i2c_address -= 2) {
		i2c_status = DetectI2cAes132(0xA0 & ~I2C_READ_FLAG);
		if (i2c_status == AES132_FUNCTION_RETCODE_SUCCESS) {
			Led2(1);  //AES132
			if (++device_count >= DISCOVER_DEVICE_COUNT_MAX)				// Found AES132 devices are awake.
				return device_count;
//			continue;
		}
//	}

	//i2c_disable();

	return device_count;
}


/** \brief This function tries to find an AES132 SPI device.
 *  \return number of devices found (maximum 1)
 */
uint8_t DetectSpiDevice()
{
	uint8_t status;
	uint8_t rx_buffer[AES132_RESPONSE_SIZE_INFO];
	memset(rx_buffer, 0, sizeof(rx_buffer));

	aes132p_set_interface(DEVKIT_IF_SPI);

	status = aes132m_info(AES132_INFO_DEV_NUM, rx_buffer);
	if (status > AES132_DEVICE_RETCODE_TEMP_SENSE_ERROR) {
		aes132p_disable_interface();
		return 0;
	}

	device_info[device_count].bus_type = DEVKIT_IF_SPI;
	device_info[device_count].device_type = DEVICE_TYPE_AES132;
	device_info[device_count].device_index = 0;

	return ++device_count;
}


/** \brief This function tries to find SHA204 and / or AES132 devices.
 *
 *         It calls functions for all three interfaces,
 *         SWI, I2C, and SPI. They in turn enter found
 *         devices into the #device_info array.
 * \return interface found
 */
interface_id_t DiscoverDevices()
{
	// A simpler sequence would be to try to discover AES132
	// first since this device does not require a Wake-up pulse.
	// But we have encountered SWI devices that wake up with
	// spurious signals generated by an attempt to talk I2C or SPI.
	interface_id_t bus_type;
	uint8_t lib_return;
	uint8_t i;

	// Disable current interface.
	if (device_count > 0) {
		if (device_info[0].device_type == DEVICE_TYPE_AES132)
			aes132p_disable_interface();
		else
			sha204p_disable_interface();
	}

	device_count = 0;
	memset(device_info, 0, sizeof(device_info));

//#if (TARGET_BOARD!=AT88RHINO_ECC108)
	// Disable SWI discovery when running on a Rhino board with AT90USB1287 and ECC108.
//	DetectSwiDevices();
//#endif

	if (device_count == 0)
		// No SWI devices discovered. Try to find TWI devices.
		DetectI2cDevices();

//#if (TARGET_BOARD!=AT88RHINO_ECC108)
	// Disable SPI discovery when running on a Rhino board with AT90USB1287 and ECC108.
//	if (device_count == 0)
//		DetectSpiDevice();
//#endif

	if (device_count == 0)
		return DEVKIT_IF_UNKNOWN;

	bus_type = device_info[0].bus_type;

		for (i = 0; i < device_count; i++) {


	// Switch to interface and test it with a high-level function.
	if (device_info[i].device_type == DEVICE_TYPE_SHA204 || device_info[i].device_type == DEVICE_TYPE_ECC108) {
		sha204p_set_interface(bus_type);
		// Enable interface.
		sha204p_init();

	//	for (i = 0; i < device_count; i++) {
			sha204p_set_device_id((bus_type == DEVKIT_IF_I2C) ? device_info[i].address : device_info[i].device_index);
			lib_return = sha204c_wakeup(receivebuf);
			if (lib_return != SHA204_SUCCESS) {
				sha204p_sleep();
				sha204p_disable_interface();

				// Reset device info.
				device_count = 0;
				memset(device_info, 0, sizeof(device_info));
				return DEVKIT_IF_UNKNOWN;
			}
			uint8_t command[SHA204_CMD_SIZE_MIN];
			// This delay is needed for an ECC108 device.
			_delay_us(100);
			lib_return = sha204m_execute(SHA204_DEVREV, 0, 0, 0, NULL, 0, NULL, 0, NULL, sizeof(command), command, DEVREV_RSP_SIZE, receivebuf);
			sha204p_sleep();
			if (lib_return != SHA204_SUCCESS) {
				sha204p_disable_interface();
				// Reset device info.
				device_count = 0;
				device_info[i].bus_type = DEVKIT_IF_UNKNOWN;
				return DEVKIT_IF_UNKNOWN;
			}
//		}
	//	sha204p_disable_interface();
	}
	else {
		// AES132
		aes132p_set_interface(bus_type);
		aes132p_enable_interface();

	//	for (i = 0; i < device_count; i++) {
			aes132p_select_device((bus_type == DEVKIT_IF_I2C) ? device_info[i].address : device_info[i].device_index);
			lib_return = aes132m_info(AES132_INFO_DEV_NUM, receivebuf);
			if (lib_return != AES132_FUNCTION_RETCODE_SUCCESS) {
				aes132p_disable_interface();
				// Reset device info.
				device_count = 0;
				device_info[i].bus_type = DEVKIT_IF_UNKNOWN;
				device_info[i].device_type = DEVICE_TYPE_UNKNOWN;
				return DEVKIT_IF_UNKNOWN;
			}
			device_info[i].dev_rev[2] = receivebuf[AES132_RESPONSE_INDEX_DATA];
			device_info[i].dev_rev[3] = receivebuf[AES132_RESPONSE_INDEX_DATA + 1];
	//	}
	//		aes132p_disable_interface();
	}
		}
	return bus_type;
}
