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
 *  \brief 	This file contains implementations of functions that
 *          wrap SWI, I2C and SPI hardware independent functions (aes132p_..., sha204p_...).
 *  \date 	February 9, 2011
 */
#include <avr\io.h>

// kit includes
#include "Combined_Physical.h"
#include "kitStatus.h"

// AES132 library includes
#include "aes132_lib_return_codes.h"
#include "aes132_twi_unified.h"
#include "aes132_spi_unified.h"

// SHA204 library includes
#include "sha204_lib_return_codes.h"
#include "sha204_physical.h"

// hardware includes
#include "i2c_phys.h"


// Let's select I2C at startup.
static interface_id_t devkit_interface = DEVKIT_IF_I2C;


/** \brief This function opens the Single Wire interface.
 *
 *         Since this function is not implemented in the
 *         SHA204 SWI modules, it is implemented here.
 */
void swi_sha204p_open_channel(void)
{
	sha204p_init();
}


/** \brief This function closes the Single Wire interface.
 *
 *         Since this function is not implemented in the
 *         SHA204 SWI modules, it is implemented here.
 */
//void swi_sha204p_close_channel(void)
//{
//	if (devkit_interface == DEVKIT_IF_SWI)
//		// Configure the pin for the single wire as input.
//		DDRB = 0;
//}
extern void swi_close_channel(void);


/** \brief Disables the I2C communications channel.
 */
void i2c_sha204p_close_channel(void)
{
	// Disable the TWI function
	TWCR &= ~_BV(TWEN);
}


// The 'd' in aes132d_ and sha204d_ function pointer names stands for "development kit".
void    (*aes132d_enable_interface)(void) = twi_aes132p_enable_interface;
void    (*aes132d_disable_interface)(void) = twi_aes132p_disable_interface;
uint8_t (*aes132d_select_device)(uint8_t) = twi_aes132p_select_device;
uint8_t (*aes132d_resync_physical)(void) = twi_aes132p_resync_physical;
uint8_t (*aes132d_write_memory_physical)(uint8_t, uint16_t, uint8_t*) = twi_aes132p_write_memory_physical;
uint8_t (*aes132d_read_memory_physical)(uint8_t, uint16_t, uint8_t*) = twi_aes132p_read_memory_physical;

void  (*sha204d_enable_interface)(void) = twi_sha204p_init;
void  (*sha204d_disable_interface)(void) = i2c_sha204p_close_channel;
void  (*sha204d_select_device)(uint8_t) = twi_sha204p_set_device_id;
void  (*sha204d_init)(void) = twi_sha204p_init;
uint8_t  (*sha204d_resync)(uint8_t, uint8_t *) = twi_sha204p_resync;
uint8_t  (*sha204d_wakeup)(void) = twi_sha204p_wakeup;
uint8_t  (*sha204d_send_command)(uint8_t, uint8_t *) = twi_sha204p_send_command;
uint8_t  (*sha204d_receive_response)(uint8_t, uint8_t *) = twi_sha204p_receive_response;
uint8_t  (*sha204d_idle)(void) = twi_sha204p_idle;
uint8_t  (*sha204d_sleep)(void) = twi_sha204p_sleep;


/** \brief This function selects the communication interface for an AES132 device.
 * \param[in] interface type of interface (TWI or SPI)
 * \return status of the operation
 */
uint8_t aes132p_set_interface(interface_id_t interface)
{
	if (devkit_interface == interface)
		return KIT_STATUS_INVALID_PARAMS;

	if (interface == DEVKIT_IF_I2C) {
		spi_aes132p_disable_interface();

		aes132d_enable_interface = twi_aes132p_enable_interface;
		aes132d_disable_interface = twi_aes132p_disable_interface;
		aes132d_select_device = twi_aes132p_select_device;
		aes132d_resync_physical = twi_aes132p_resync_physical;
		aes132d_read_memory_physical = twi_aes132p_read_memory_physical;
		aes132d_write_memory_physical = twi_aes132p_write_memory_physical;
	}
	else if (interface == DEVKIT_IF_SPI) {
		twi_aes132p_disable_interface();

		aes132d_enable_interface = spi_aes132p_enable_interface;
		aes132d_disable_interface = spi_aes132p_disable_interface;
		aes132d_select_device = spi_aes132p_select_device;
		aes132d_resync_physical = spi_aes132p_resync_physical;
		aes132d_read_memory_physical = spi_aes132p_read_memory_physical;
		aes132d_write_memory_physical = spi_aes132p_write_memory_physical;
	}
	else
		return KIT_STATUS_INVALID_PARAMS;

	devkit_interface = interface;
	aes132d_enable_interface();
	return KIT_STATUS_SUCCESS;
}


/** \brief This function selects the communication interface for a SHA204 device.
 * \param[in] interface type of interface (TWI or SWI)
 * \return status of the operation
 */
uint8_t sha204p_set_interface(interface_id_t interface)
{
	if (devkit_interface == interface)
		return KIT_STATUS_INVALID_PARAMS;

	if (interface == DEVKIT_IF_I2C) {
		// Calling the function pointer does not work. It looks
		// like the compiler is inlining the function and does not let
		// the lines below execute as a consequence.
//		sha204d_disable_interface();
		swi_close_channel();

		sha204d_enable_interface = twi_sha204p_init;
		sha204d_disable_interface = i2c_sha204p_close_channel;
		sha204d_select_device = twi_sha204p_set_device_id;
		sha204d_init = twi_sha204p_init;
		sha204d_wakeup = twi_sha204p_wakeup;
		sha204d_send_command = twi_sha204p_send_command;
		sha204d_receive_response = twi_sha204p_receive_response;
		sha204d_idle = twi_sha204p_idle;
		sha204d_sleep = twi_sha204p_sleep;
		sha204d_resync = twi_sha204p_resync;
	}
	else if (interface == DEVKIT_IF_SWI) {
		i2c_sha204p_close_channel();

		sha204d_enable_interface = swi_sha204p_init; //swi_sha204p_open_channel;
		sha204d_disable_interface = swi_close_channel;
		sha204d_select_device = swi_sha204p_set_device_id;
		sha204d_init = swi_sha204p_init;
		sha204d_wakeup = swi_sha204p_wakeup;
		sha204d_send_command = swi_sha204p_send_command;
		sha204d_receive_response = swi_sha204p_receive_response;
		sha204d_idle = swi_sha204p_idle;
		sha204d_sleep = swi_sha204p_sleep;
		sha204d_resync = swi_sha204p_resync;
	}
	else
		return KIT_STATUS_INVALID_PARAMS;

	devkit_interface = interface;
	sha204d_enable_interface();
	return KIT_STATUS_SUCCESS;
}


/** \brief This function enables the AES132 interface (SPI or TWI).
 *
 */
void aes132p_enable_interface(void)
{
	if (aes132d_enable_interface)
		aes132d_enable_interface();
}


/** \brief This function disables the AES132 interface (SPI or I2C).
 *
 */
void aes132p_disable_interface(void)
{
	if (aes132d_disable_interface)
		aes132d_disable_interface();
}


/** \brief This function selects a device by supplying an I2C addresses
 *         for AES132 devices on the same bus or from an array of SPI
 *         chip select pins on the same SPI bus.
 *
 * \param[in] device_id I2C address or SPI index into CS pin array
 * \return error if function pointer is null
 */
uint8_t aes132p_select_device(uint8_t device_id)
{
	return aes132d_select_device ? aes132d_select_device(device_id) : KIT_STATUS_INVALID_IF_FUNCTION;
}


/** \brief This function reads bytes from the device.
 * \param[in] size number of bytes to read
 * \param[in] word_address word address to read from
 * \param[out] data pointer to rx buffer
 * \return status of the operation
 */
uint8_t aes132p_read_memory_physical(uint8_t count, uint16_t word_address, uint8_t *data)
{
	return aes132d_read_memory_physical ? aes132d_read_memory_physical(count, word_address, data) : KIT_STATUS_INVALID_IF_FUNCTION;
}


/** \brief This function writes bytes to the device.
 * \param[in] count number of bytes to write
 * \param[in] word_address word address to write to
 * \param[in] data pointer to tx buffer
 * \return status of the operation
 */
uint8_t aes132p_write_memory_physical(uint8_t count, uint16_t word_address, uint8_t *data)
{
	return aes132d_write_memory_physical ? aes132d_write_memory_physical(count, word_address, data) : KIT_STATUS_INVALID_IF_FUNCTION;
}


/** \brief This function re-synchronizes
 *         communication, but is not implemented when using SPI.
 * \return TWI: status of the operation; SPI: success
 */
uint8_t aes132p_resync_physical(void)
{
	return aes132d_resync_physical ? aes132d_resync_physical() : KIT_STATUS_INVALID_IF_FUNCTION;
}


/** \brief This function sets the signal pin (SWI) or the TWI address (TWI).
 *  \param[in] address index into pin array (SWI) or TWI address (TWI).
 */
void sha204p_set_device_id(uint8_t address)
{
	if (sha204d_select_device)
		sha204d_select_device(address);
}


/** \This function initializes the interface (SWI or TWI).
 *
 */
void sha204p_init(void)
{
	if (sha204d_init)
		sha204d_init();
}


/** \This function initializes the interface (SWI or TWI).
 *
 */
void sha204p_disable_interface(void)
{
	if (sha204d_disable_interface)
		sha204d_disable_interface();
}


/** \brief This function re-synchronizes communication.
 * \param[in] size size of the response buffer
 * \param[out] response pointer to response buffer
 * \return status of the operation
 */
uint8_t sha204p_resync(uint8_t size, uint8_t *response)
{
	return sha204d_resync ? sha204d_resync(size, response) : KIT_STATUS_INVALID_IF_FUNCTION;
}

/** \brief This function wakes up a SHA204 device.
 * \return status of the operation
 */
uint8_t sha204p_wakeup()
{
	return sha204d_wakeup ? sha204d_wakeup() : KIT_STATUS_INVALID_IF_FUNCTION;
}


/** \brief This function sends a command to a SHA204 device.
 * \param[in] count number of bytes in command buffer
 * \param[in] buffer pointer to command buffer
 * \return status of the operation
 */
uint8_t sha204p_send_command(uint8_t count, uint8_t *buffer)
{
	return sha204d_send_command ? sha204d_send_command(count, buffer) : KIT_STATUS_INVALID_IF_FUNCTION;
}


/** \brief This function receives a response from a SHA204 device.
 * \param[in] count size of response buffer
 * \param[out] buffer pointer to response buffer
 * \return status of the operation
 */
uint8_t sha204p_receive_response(uint8_t count, uint8_t *buffer)
{
	return sha204d_receive_response ? sha204d_receive_response(count, buffer) : KIT_STATUS_INVALID_IF_FUNCTION;
}


/** \brief This function puts a SHA204 device into Idle mode.
 * \return status of the operation
 */
uint8_t sha204p_idle(void)
{
	return sha204d_idle ? sha204d_idle() : KIT_STATUS_INVALID_IF_FUNCTION;
}


/** \brief This function puts a SHA204 device into Sleep mode.
 * \return status of the operation
 */
uint8_t sha204p_sleep(void)
{
	return sha204d_sleep ? sha204d_sleep() : KIT_STATUS_INVALID_IF_FUNCTION;
}
