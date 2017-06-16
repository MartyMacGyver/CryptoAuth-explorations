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
 * \brief This file contains functions for parsing commands at	the
 * 		development kit level (not the daughter board level).
 * \author Atmel Crypto Products
 * \date June 24, 2013
 */

#include <avr/eeprom.h>
#include <string.h>               // string functions

#include "kitStatus.h"            // kit status definitions
#include "config.h"               // various type definitions
#include "sha204_helper.h"        // SHA204 library helper functions
#include "utilities.h"            // function definitions for parser utilities
#include "parserAscii.h"          // definitions for ASCII parser functions
#include "Combined_Discover.h"    // definitions for device discovery functions


/** \brief version of development kit firmware
 *         that contains AES132 and SHA204 library */
const char VersionKit[] PROGMEM = {1, 0, 5};

//! version of SHA204 library
const char VersionSha204[] PROGMEM = {1, 3, 0};

//! version of AES132 library
const char VersionAes132[] PROGMEM = {1, 1, 0};

//! version of ECC108 library
// There is no actual ECC108 library linked, but the SHA204 library is used to
// communicate with an ECC108 device.
const char VersionEcc108[] PROGMEM = {0, 1, 0};

const char StringSha204[] PROGMEM = "SHA204 ";      //!< SHA204 string
const char StringAes132[] PROGMEM = "AES132 ";      //!< AES132 string
const char StringEcc108[] PROGMEM = "ECC108 ";      //!< ECC108 string
//  const char StringKitShort[] PROGMEM = "CK490 ";     //!< short string of Microbase kit
//  const char StringKit[] PROGMEM = "AT88CK490 ";   //!< long string of Microbase kit

// The following compiler statements define the name of the kit based on #defines
// Defines are located in the Combined_Discover.h
#ifdef ECCROOT 
    const char StringKitShort[] PROGMEM = "ECCROOT ";     //!< short string of Microbase kit
    const char StringKit[] PROGMEM = "AT88CKECCROOT ";   //!< long string of Microbase kit
#else	
    #ifdef ECCSIGNER
        const char StringKitShort[] PROGMEM = "ECCSIGN ";     //!< short string of Microbase kit
        const char StringKit[] PROGMEM = "AT88CKECCSIGNER ";   //!< long string of Microbase kit
    #else	
	    #ifdef CK590
            const char StringKitShort[] PROGMEM = "CK590 ";     //!< short string of Microbase kit
            const char StringKit[] PROGMEM = "AT88CK590 ";   //!< long string of Microbase kit
        #else 
           //If neither ECCROOT, ECCSIGNER CK590 then default to CK490
           const char StringKitShort[] PROGMEM = "CK490 ";     //!< short string of Microbase kit
           const char StringKit[] PROGMEM = "AT88CK490 ";   //!< long string of Microbase kit
		#endif   
    #endif	
#endif	    



//! indicates whether discovery should run at intervals
uint8_t isDiscoveryEnabled = TRUE;

// Module number definition and functions
static uint8_t  moduleNum EEMEM;
void setModuleNum(uint8_t modNum);
uint8_t getModuleNum(void);

/** \brief This function parses kit commands (ASCII) received from a
 * 			PC host and returns an ASCII response.
 * \param[in] commandLength number of bytes in command buffer
 * \param[in] command pointer to ASCII command buffer
 * \param[out] responseLength pointer to number of bytes in response buffer
 * \param[out] response pointer to binary response buffer
 * \param[out] responseIsAscii pointer to response type
 * \return the status of the operation
 */
uint8_t ParseBoardCommands(uint16_t commandLength, uint8_t *command, uint16_t *responseLength, uint8_t *response, uint8_t *responseIsAscii)
{
	uint8_t status = KIT_STATUS_UNKNOWN_COMMAND;
	uint16_t responseIndex = 0;
	uint16_t deviceIndex;
	uint16_t dataLength = 1;
	uint8_t *rxData[1];
	interface_id_t device_interface = DEVKIT_IF_UNKNOWN;
	device_info_t *device_info;
	//TODO:	When we change this string from "TWI" to "I2C", the older versions of ACES cannot discover the kit (2/14/2013)
	//		wait a few months while older versions of ACES are upgraded to ECC108 versions before we make this this change
	//char *StringInterface[] = {"no_device ", "SPI ", "I2C ", "SWI "};
	char *StringInterface[] = {"no_device ", "SPI ", "TWI ", "SWI "};

	char *pToken = strchr((char *) command, ':');
	if (!pToken)
		return status;

	*responseIsAscii = 1;

	switch(pToken[1]) {
	case 'v':
		// Gets abbreviated board name and, if found, first device type and interface type.
		// response (no device): <kit version>, "no_devices"<status>()
		// response (device found): <kit version>, <device type>, <interface><status>(<address>)

		status = KIT_STATUS_SUCCESS;

		device_interface = DiscoverDevices();
		isDiscoveryEnabled = (device_interface == DEVKIT_IF_UNKNOWN);

		// Copy version string into response buffer.
		strcpy_P((char *) response, StringKitShort);
		if (device_interface != DEVKIT_IF_UNKNOWN) {
			// Append device type to response.
			device_info = GetDeviceInfo(0);
			if (device_info->device_type == DEVICE_TYPE_SHA204)
				strcat_P((char *) response, StringSha204);
			else if (device_info->device_type == DEVICE_TYPE_AES132)
				strcat_P((char *) response, StringAes132);
			else if (device_info->device_type == DEVICE_TYPE_ECC108)
				strcat_P((char *) response, StringEcc108);
		}
		// Append interface type to response.
		strcat((char *) response, StringInterface[device_interface]);
		responseIndex = strlen((char *) response);

		// If device was found, append its address / selector byte.
		// Skip one byte for the status.
		if (device_interface != DEVKIT_IF_UNKNOWN) {
			dataLength++;
			response[responseIndex + 1] = device_info->bus_type == DEVKIT_IF_I2C
			   ? device_info->address : device_info->device_index;
		}
		break;


	case 'f':
		// Gets firmware name and version (index 0: kit, 1: SHA204 library, 2: AES132 library).
		status = ExtractDataLoad(pToken, &dataLength, rxData);
		if (status != KIT_STATUS_SUCCESS)
			break;

		dataLength = 4; // size of versions + status byte

		switch (*rxData[0]) {
		case 0: // kit version
			strcpy_P((char *) response, StringKit);
			responseIndex = strlen((char *) response);
			memcpy_P((char *) (response + responseIndex + 1), VersionKit, dataLength);
			break;

		case 1: // SHA204 library version
			strcpy_P((char *) response, StringSha204);
			responseIndex = strlen((char *) response);
			memcpy((char *) (response + responseIndex + 1), sha204h_get_library_version(), dataLength);
			break;

		case 2: // AES132 library version
			strcpy_P((char *) response, StringAes132);
			responseIndex = strlen((char *) response);
			memcpy_P((char *) (response + responseIndex + 1), VersionAes132, dataLength);
			break;

		case 3: // ECC108 library version
			strcpy_P((char *) response, StringEcc108);
			responseIndex = strlen((char *) response);
			memcpy_P((char *) (response + responseIndex + 1), VersionEcc108, dataLength);
			break;

		default:
			status = KIT_STATUS_INVALID_PARAMS;
			break;
		}
		break;


	case 'd':
		if (pToken[2] == '?') {
			// Return isDisoveryEnabled.
			dataLength++;
			response[responseIndex + 1] = isDiscoveryEnabled;
			status = KIT_STATUS_SUCCESS;
			break;				
		}

		status = ExtractDataLoad(pToken, &dataLength, rxData);
		if (status != KIT_STATUS_SUCCESS)
			break;
			
		// The first letter can stand for device ('d[evice]') or discovery ('di[scovery]').
		if (pToken[2] == 'i') {
			// Discovery can be switched on (*rxData[0] != 0) and off (*rxData[0] == 0).
			isDiscoveryEnabled = *rxData[0];
			break;
		}
		// Gets information about any one of the found devices.
		// Which device is specified by an index parameter in the USB command.
		device_interface = DiscoverDevices();
		isDiscoveryEnabled = (device_interface == DEVKIT_IF_UNKNOWN);

		deviceIndex = *rxData[0];
		device_info = GetDeviceInfo(deviceIndex);
		if (!device_info) {
			status = KIT_STATUS_NO_DEVICE;
			break;
		}

		// Copy device type into response.
		switch (device_info->device_type) {
		case DEVICE_TYPE_SHA204:
			strcpy_P((char *) response, StringSha204);
			break;

		case DEVICE_TYPE_AES132:
			strcpy_P((char *) response, StringAes132);
			break;

		case DEVICE_TYPE_ECC108:
			strcpy_P((char *) response, StringEcc108);
			break;

		case DEVICE_TYPE_UNKNOWN:
			strcpy((char *) response, StringInterface[0]);
			status = KIT_STATUS_NO_DEVICE;
			break;

		default:
			// We got a response using a particular interface,
			// but we don't know from what kind of device.
			// We should not come here.
			strcpy((char *) response, "unknown_device");
			break;
		}


		if (device_info->bus_type == DEVKIT_IF_UNKNOWN) {
			responseIndex = strlen((char *) response);
			break;
		}
		
		// Append interface type to response.
		strcat((char *) response, StringInterface[device_interface]);
		responseIndex = strlen((char *) response);

		// Append the address (TWI) / index (SWI) of the device.
		// Skip one byte for status.
		dataLength++;
		response[responseIndex + 1] = device_info->bus_type == DEVKIT_IF_I2C
			? device_info->address : device_info->device_index;
		break;

	// Get/Set the module number  --------------------
	case 'm':
		// ---- "b[oard]:m{g[et] | s[et]}(<number, 1 byte>)" ----------
		status = ExtractDataLoad(pToken, &dataLength, rxData);
		switch (pToken[2])
		{
			// Get the TPM module number
			case 'g':
				*responseLength = 1;
				response[0] = getModuleNum();
				break;
					
			// Set the TPM module number
			case 's':
				setModuleNum(rxData[0][0]);
				break;
		}
		break;

	default:
		status = KIT_STATUS_UNKNOWN_COMMAND;
		break;
	}
	// Append <status>(<data>).
	response[responseIndex] = status;
	*responseLength = CreateUsbPacket(dataLength, &response[responseIndex]) + responseIndex;
	return status;
}

/** \brief This function sets the module number into EEPROM.
 * \param[in] modNum The number to set this module to
 * \return the status of the operation
 */
void setModuleNum(uint8_t modNum)
{
	eeprom_write_byte(&moduleNum, modNum);
}

/** \brief This function gets the module number from EEPROM.
 * \return The module number from EEPROM
 */
uint8_t getModuleNum(void)
{
	// Read the Module number
	uint8_t modNum = eeprom_read_byte(&moduleNum);
	return modNum;
}
