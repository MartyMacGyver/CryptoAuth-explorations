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

/** \file SHA_CommMarshalling.c
 *  \brief 	functions for commands
 *  \author Nelson Lunsford
 *  \date 	March 10, 2010
 */
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "SHA_Comm.h"
#include "SHA_Status.h"
#include "SHA_CommMarshalling.h"


// Data definitions
uint8_t				sendbuf[SENDBUF_SIZE];
uint8_t 			receicebuf[RECEIVEBUF_SIZE];
SHA_CommParameters	commparms;


/**
 *
 * \brief Sends a Derive Key command to the device.
 *
 * \param[in]  Random what to include in the MAC calculation
 * \param[in]  KeyID key id
 * \param[in]  Pointer to challenge data
 * \return status of the operation
 */
uint8_t SHAC_DeriveKey(uint8_t Random, uint16_t TargetKey, uint8_t *Data)
{
	sendbuf[COUNT_IDX] = DK_COUNT_SMALL;		// Assume no Data field
	sendbuf[CMD_ORDINAL_IDX] = DERIVE_KEY;
	sendbuf[DK_RANDOM_IDX] = Random;
	memcpy(&sendbuf[DK_TARGETKEY_IDX], &TargetKey, 2);
	if (Data != NULL)
	{
		memcpy(&sendbuf[DK_MAC_IDX], Data, 32);
		sendbuf[COUNT_IDX] = DK_COUNT_LARGE;
	}

	commparms.txBuffer = &sendbuf[0];
	commparms.rxBuffer = &receicebuf[0];
	commparms.rxSize = DERIVE_KEY_RX_SIZE;
	commparms.executionDelay = 100000;
	// Transfer the command to the chip
	//

	return SHAC_SendAndReceive(&commparms);

}

/**
 *
 * \brief Sends a DevRev command to the device.
 *
 * \param[in]  ignored
 * \param[in]  ignored
 * \param[out] 4 bytes indicating the revision number
 * \return status of the operation
 */
uint8_t SHAC_DevRev()
{
	sendbuf[COUNT_IDX] = DR_COUNT;
	sendbuf[CMD_ORDINAL_IDX] = DEVREV;

	commparms.txBuffer = &sendbuf[0];
	commparms.rxBuffer = &receicebuf[0];
	commparms.rxSize = DEVREV_RX_SIZE;
	commparms.executionDelay = 1000;
	// Transfer the command to the chip
	//
	return SHAC_SendAndReceive(&commparms);

}

/**
 *
 * \brief Sends a GenDig command to the device.
 *
 * \param[in]  Zone 1:OTP, 2:Data
 * \param[in]  KeyID key id
 * \param[in]  Pointer to 4 bytes of data for SHA calculation
 * \return status of the operation
 */
uint8_t SHAC_GenDig(uint8_t Zone, uint16_t KeyID, uint8_t *Data)
{
	sendbuf[COUNT_IDX] = GD_COUNT;
	sendbuf[CMD_ORDINAL_IDX] = GENDIG;
	sendbuf[GD_ZONE_IDX] = Zone;
	memcpy(&sendbuf[GD_KEYID_IDX], &KeyID, 2);
	if (Data != NULL)
	{
		memcpy(&sendbuf[GD_DATA_IDX], Data, 4);
		sendbuf[COUNT_IDX] = GD_COUNT_DATA;
	}

	commparms.txBuffer = &sendbuf[0];
	commparms.rxBuffer = &receicebuf[0];
	commparms.rxSize = GENDIG_RX_SIZE;
	commparms.executionDelay = GENDIGDELAY;
	// Transfer the command to the chip
	//
	return SHAC_SendAndReceive(&commparms);

}

/**
 *
 * \brief Sends an HMAC command to the device.
 *
 * \param[in]  Mode
 * \param[in]  KeyID key id
 * \param[out] 32 bytes of HMAC digest
 * \return status of the operation
 */
uint8_t SHAC_HMAC(uint8_t Mode, uint16_t KeyID)
{
	sendbuf[COUNT_IDX] = HM_COUNT;
	sendbuf[CMD_ORDINAL_IDX] = HMAC;
	sendbuf[HM_MODE_IDX] = Mode;
	memcpy(&sendbuf[HM_KEYID_IDX], &KeyID, 2);

	commparms.txBuffer = &sendbuf[0];
	commparms.rxBuffer = &receicebuf[0];
	commparms.rxSize = HMAC_RX_SIZE;
	commparms.executionDelay = HMACDELAY;
	// Transfer the command to the chip
	//
	return SHAC_SendAndReceive(&commparms);

}

/**
 *
 * \brief Sends an HostHMAC command to the device.
 *
 * \param[in]  Mode
 * \param[in]  KeyID key id
 * \param[in]  Cliet Response
 * \param[in]  Other Data
 * \param[out] 1 byte of result
 * \return status of the operation
 */
uint8_t SHAC_HostHMAC(uint8_t Mode, uint16_t KeyID, uint8_t *ClietResponse, uint8_t *OtherData)
{
	sendbuf[COUNT_IDX] = HHMAC_COUNT;
	sendbuf[CMD_ORDINAL_IDX] = HOSTHMAC;
	sendbuf[HHMAC_MODE_IDX] = Mode;
	memcpy(&sendbuf[HHMAC_KEYID_IDX], &KeyID, 2);
	memcpy(&sendbuf[HHMAC_CLIENTRSP], ClietResponse, 32);
	memcpy(&sendbuf[HHMAC_DATA], OtherData, 13);

	commparms.txBuffer = &sendbuf[0];
	commparms.rxBuffer = &receicebuf[0];
	commparms.rxSize = HOSTHMAC_RX_SIZE;
	commparms.executionDelay = HMACDELAY;
	// Transfer the command to the chip
	//
	return SHAC_SendAndReceive(&commparms);

}

/**
 *
 * \brief Sends an HostMAC command to the device.
 *
 * \param[in]  Mode
 * \param[in]  KeyID key id
 * \param[in]  Cliet Response
 * \param[in]  Other Data
 * \param[out] 1 byte of result
 * \return status of the operation
 */
uint8_t SHAC_HostMAC(uint8_t Mode, uint16_t KeyID, uint8_t *ClietChallenge, uint8_t *ClietResponse, uint8_t *OtherData)
{
	sendbuf[COUNT_IDX] = HOSTMAC_COUNT;
	sendbuf[CMD_ORDINAL_IDX] = HOSTMAC;
	sendbuf[HOSTMAC_MODE_IDX] = Mode;
	memcpy(&sendbuf[HOSTMAC_KEYID_IDX], &KeyID, 2);
	memcpy(&sendbuf[HOSTMAC_CLIENCHALL], ClietChallenge, 32);
	memcpy(&sendbuf[HOSTMAC_CLIENTRSP], ClietResponse, 32);
	memcpy(&sendbuf[HOSTMAC_DATA], &KeyID, 2);

	commparms.txBuffer = &sendbuf[0];
	commparms.rxBuffer = &receicebuf[0];
	commparms.rxSize = HOSTMAC_RX_SIZE;
	commparms.executionDelay = HMACDELAY;
	// Transfer the command to the chip
	//
	return SHAC_SendAndReceive(&commparms);

}


/**
 *
 * \brief Sends an Lock command to the device.
 *
 * \param[in]  Zone
 * \param[in]  Summary
 * \param[out] 1 byte of response
 * \return status of the operation
 */
uint8_t SHAC_Lock(uint8_t Zone, uint16_t Summary)
{
	sendbuf[COUNT_IDX] = LK_COUNT;
	sendbuf[CMD_ORDINAL_IDX] = LOCK;
	sendbuf[LK_ZONE_IDX] = Zone;
	memcpy(&sendbuf[LK_SUMMARY_IDX], &Summary, 2);

	commparms.txBuffer = &sendbuf[0];
	commparms.rxBuffer = &receicebuf[0];
	commparms.rxSize = LOCK_RX_SIZE;
	commparms.executionDelay = LOCKDELAY;
	// Transfer the command to the chip
	//
	return SHAC_SendAndReceive(&commparms);

}

/**
 *
 * \brief Sends an MAC command to the device.
 *
 * \param[in]  Mode
 * \param[in]  KeyID key id
 * \param[in]  Challenge
 * \param[out] 32 bytes of response
 * \return status of the operation
 */
uint8_t SHAC_Mac(uint8_t Mode, uint16_t KeyID, uint8_t *Challenge)
{
	sendbuf[COUNT_IDX] = MAC_COUNT_SHORT;
	sendbuf[CMD_ORDINAL_IDX] = MAC;
	sendbuf[MAC_MODE_IDX] = Mode;
	memcpy(&sendbuf[MAC_KEYID_IDX], &KeyID, 2);
	if ((Challenge != NULL) && ((Mode & 0x01) == 0))
	{
		memcpy(&sendbuf[MAC_CHALL_IDX], Challenge, 32);
		sendbuf[COUNT_IDX] = MAC_COUNT_LARGE;
	}

	commparms.txBuffer = &sendbuf[0];
	commparms.rxBuffer = &receicebuf[0];
	commparms.rxSize = MAC_RX_SIZE;
	commparms.executionDelay = MACDELAY;
	// Transfer the command to the chip
	//
	return SHAC_SendAndReceive(&commparms);

}

/**
 *
 * \brief Sends an Nonce command to the device.
 *
 * \param[in]  Mode
 * \param[in]  Numin
 * \param[out] 1 or 32 bytes of response
 * \return status of the operation
 */
uint8_t SHAC_Nonce(uint8_t Mode, uint8_t *Numin)
{
	sendbuf[COUNT_IDX] = N_COUNT_SHORT;
	sendbuf[CMD_ORDINAL_IDX] = NONCE;
	sendbuf[N_MODE_IDX] = Mode;
	memset(&sendbuf[N_ZERO_IDX], 0x00, 2);
	if ((Mode & 0x03) != 0x03)
	{
		memcpy(&sendbuf[N_NUMIN_IDX], Numin, 20);
		commparms.rxSize = NONCE_RX_SIZE_LONG;
	}
	else
	{
		memcpy(&sendbuf[N_NUMIN_IDX], Numin, 32);
		sendbuf[COUNT_IDX] = N_COUNT_LONG;
		commparms.rxSize = NONCE_RX_SIZE_SHORT;
	}

	commparms.txBuffer = &sendbuf[0];
	commparms.rxBuffer = &receicebuf[0];

	commparms.executionDelay = SHORTRANDDELAY;
	// Transfer the command to the chip
	//
	return SHAC_SendAndReceive(&commparms);

}

/**
 *
 * \brief Sends an Pause command to the device.
 *
 * \param[in]  Selector
 * \param[in]  Zero
 * \param[out] 1 bytes of response
 * \return status of the operation
 */
uint8_t SHAC_Pause(uint8_t Selector)
{
	sendbuf[COUNT_IDX] = P_COUNT;
	sendbuf[CMD_ORDINAL_IDX] = PAUSE;
	sendbuf[P_SELECT_IDX] = Selector;
	memset(&sendbuf[P_ZERO_IDX], 0x00, 2);

	commparms.txBuffer = &sendbuf[0];
	commparms.rxBuffer = &receicebuf[0];
	commparms.rxSize = PAUSE_RX_SIZE;
	commparms.executionDelay = GENERALCMDDELAY;
	// Transfer the command to the chip
	//
	return SHAC_SendAndReceive(&commparms);

}

/**
 *
 * \brief Sends an Random command to the device.
 *
 * \param[in]  Mode
 * \param[in]  Zero
 * \param[out] 32 bytes of response
 * \return status of the operation
 */
uint8_t SHAC_Random(uint8_t Mode)
{
	sendbuf[COUNT_IDX] = RAN_COUNT;
	sendbuf[CMD_ORDINAL_IDX] = RANDOM;
	sendbuf[RAN_MODE_IDX] = Mode;
	memset(&sendbuf[RAN_ZERO_IDX], 0x00, 2);

	commparms.txBuffer = &sendbuf[0];
	commparms.rxBuffer = &receicebuf[0];
	commparms.rxSize = RANDOM_RX_SIZE;
	commparms.executionDelay = SHORTRANDDELAY;
	// Transfer the command to the chip
	//
	return SHAC_SendAndReceive(&commparms);

}

/**
 *
 * \brief Sends an Read command to the device.
 *
 * \param[in]  Zone
 * \param[in]  Address
 * \param[out] 4 or 32 bytes of response
 * \return status of the operation
 */
uint8_t SHAC_Read(uint8_t Zone, uint16_t Address)
{
	sendbuf[COUNT_IDX] = READ_COUNT;
	sendbuf[CMD_ORDINAL_IDX] = READ;
	sendbuf[READ_ZONE_IDX] = Zone;
	memcpy(&sendbuf[READ_ADDR_IDX], &Address, 2);

	commparms.txBuffer = &sendbuf[0];
	commparms.rxBuffer = &receicebuf[0];
	if (Zone & 0x80)			// if bit 7 = 1, 32 bytes
		commparms.rxSize = READ_32_RX_SIZE;
	else
		commparms.rxSize = READ_4_RX_SIZE;

	// The execution delay will have to increased for clear text & enc data
	commparms.executionDelay = GENERALCMDDELAY;
	// Transfer the command to the chip
	//
	return SHAC_SendAndReceive(&commparms);

}

/**
 *
 * \brief Sends an TempSense command to the device.
 *
 * \param[in]  Mode
 * \param[in]  Zero1
 * \param[in]  Zero2
 * \param[out] 4 bytes of response
 * \return status of the operation
 */
uint8_t SHAC_TempSense(uint8_t *Temp)
{
	sendbuf[COUNT_IDX] = TS_COUNT;
	sendbuf[CMD_ORDINAL_IDX] = TEMPSENSE;
	memset(&sendbuf[TS_ZERO1_IDX], 0x00, 3);		// ZERO 1 & 2 = 0x00

	commparms.txBuffer = &sendbuf[0];
	commparms.rxBuffer = &receicebuf[0];
	commparms.rxSize = TEMPSENSE_RX_SIZE;
	commparms.executionDelay = TEMPDELAY;
	// Transfer the command to the chip
	//
	return SHAC_SendAndReceive(&commparms);

}

/**
 *
 * \brief Sends an Write command to the device.
 *
 * \param[in]  Zone
 * \param[in]  Address
 * \param[in]  Value
 * \param[in]  MACData
 * \param[out] 1 bytes of response
 * \return status of the operation
 */
uint8_t SHAC_Write(uint8_t Zone, uint16_t Address, uint8_t *Value, uint8_t *MACData)
{
	uint8_t		index;

	index = 0;
	sendbuf[COUNT_IDX] = W_COUNT_SHORT;			// Assume shortest command
	sendbuf[CMD_ORDINAL_IDX] = WRITE;
	sendbuf[W_ZONE_IDX] = Zone;
	memcpy(&sendbuf[W_ADDR_IDX], &Address, 2);
	if (Zone & 0x80)							// if bit 7 set, then 32 bytes
	{
		memcpy(&sendbuf[W_VALUE_IDX], Value, 32);
		sendbuf[COUNT_IDX] = W_COUNT_LONG;
		index = W_COUNT_LONG;
	}
	else
		memcpy(&sendbuf[W_VALUE_IDX], &Value, 4);

	if (MACData != NULL)
	{
		if (Zone & 0x80)
		{
			memcpy(&sendbuf[W_MAC_VL_IDX], &MACData, 32);
			sendbuf[COUNT_IDX] = W_COUNT_LONG_MAC;
		}
		else
		{
			memcpy(&sendbuf[W_MAC_VS_IDX], &MACData, 32);
			sendbuf[COUNT_IDX] = W_COUNT_SHORT_MAC;
		}
	}

	commparms.txBuffer = &sendbuf[0];
	commparms.rxBuffer = &receicebuf[0];
	commparms.rxSize = WRITE_RX_SIZE;
	commparms.executionDelay = WRITEDELAY;
	// Transfer the command to the chip
	//
	return SHAC_SendAndReceive(&commparms);

}

/** \brief This function returns the size of the expected response in bytes, given a properly formatted SHA command
 *
 * The data load is expected to be in Hex-Ascii and surrounded by parentheses.
 * \param[in] the properly formatted SHA command buffer
 * \return the size of the expected response in bytes
 */
uint8_t SHAC_CmdResponseSize(uint8_t *cmdBuf)
{
	// Get the Opcode and Param1
	uint8_t opCode = cmdBuf[CMD_ORDINAL_IDX];
	uint8_t param1 = cmdBuf[PARAM1_IDX];

	// Return the expected response size
	switch (opCode)
	{
		case DERIVE_KEY:
			return DERIVE_KEY_RX_SIZE;
		case DEVREV:
			return DEVREV_RX_SIZE;
		case GENDIG:
			return GENDIG_RX_SIZE;
		case HMAC:
			return HMAC_RX_SIZE;
		case HOSTHMAC:
			return HOSTHMAC_RX_SIZE;
		case HOSTMAC:
			return HOSTMAC_RX_SIZE;
		case LOCK:
			return LOCK_RX_SIZE;
		case MAC:
			return MAC_RX_SIZE;
		case NONCE:
			if ((param1 & 0x03) != 0x03)
				return NONCE_RX_SIZE_LONG;
			else
				return NONCE_RX_SIZE_SHORT;
		case PAUSE:
			return PAUSE_RX_SIZE;
		case RANDOM:
			return RANDOM_RX_SIZE;
		case READ:
			if (param1 & 0x80)
				return READ_32_RX_SIZE;
			else
				return READ_4_RX_SIZE;
		case TEMPSENSE:
			return TEMPSENSE_RX_SIZE;
		case WRITE:
			return WRITE_RX_SIZE;
	}
	// Return the max size for all commands
	return MAX_CMD_RX_SIZE;
}
