/** \file
 *  \brief Definition of Parser function.
 */


#include "parser.h"

#include "SA_API.h"

extern	SA_snBuf      snData;
extern	SA_statusBuf  statusData;
extern	SA_mfrID_buf  mfrIDdata;
extern	SA_revNumBuf  revNumData;
extern	SA_readBuf    readBuf;

extern	SA_snBuf      snDataHost;
extern	SA_statusBuf  statusDataHost;
extern	SA_mfrID_buf  mfrIDdataHost;
extern	SA_revNumBuf  revNumDataHost;


#ifdef AT88CK109STK3 // JAVAN + MICROBASE + CA (SOT23-3)
					 // ALSO WITH USED:
					 // JAVAN JR + MICROBASE + CA (SOT23-3)

   version_t libraryVersion = {0x01, 0x00, 0x08};

#elif AT88CK427      // RHINO GREEN USB DONGLE (REV 1)
					 // AT90USB1287 + CA (8LD SOIC) 

   version_t libraryVersion = {0x03, 0x00, 0x08};

#elif AT88CK101STK8   //   SHA204 AND AES-MEMORY

   version_t libraryVersion = {0x03, 0x00, 0x08};



#endif


/** \brief Parsing.
 *
 * \param pucCommand is the input array of command
 * \param ucCommandSize is the size of pucCommand
 * \param pucResult is the output array of result
 * \param pucResponseSize is the expected pucResultSize
 *
 * \return Parsing result or Command result.
 */

U8 command_parse(puchar pucCommand, U8 ucCommandSize, puchar pucResult, puchar pucResponseSize)
{
	uint8_t i;
	uint8_t ucResponseSize = 1;      // default response size
	static uint8_t	retCode = SA_SUCCESS;
	uint8_t wakeSleepCrc = 0;
	uint8_t readBuf[10];
	uint8_t MACdigest[32];
	uint8_t talkResponse[MAX_SIZE_OF_RESPONSE];
	SA_Delay cmdDelay = SA_DELAY_EXEC_MAC;
	MACParams* pMac;
	BurnSecureParams* pBS;
	CA_BurnParams* pBurn;
	CA_HostCommandParams* pHAHC;
	CA_LoadSramParams* pLS;
	CA_ReadParams* pCR;
	SA_readBuf outBuf;
	static uint8_t testVector[5];
	version_t* pVer;


	// Set DeviceID
	/** \todo PC applications should first obtain available device identifiers and associated types.
	*        This needs additions to the protocol. For now we translate the identifiers coming from
	*        the PC to the correct ones.
	*/
	if (pucCommand[0] != getFWVers_) {

		retCode = SAP_SetDeviceID(pucCommand[ucCommandSize - 1]);

		if (retCode != SA_SUCCESS) {
			 *pucResponseSize = ucResponseSize;
			 return retCode;
		}
	}

	switch (pucCommand[0])
	{
		case CA_Read_:
		/*
			if (pucCommand[1] == getFWRev) { // getFWRev stands alone
				pVer = SA_GetLibraryVersion();
				testVector[0] = pucResult[0] = pVer->major;
				testVector[1] = pucResult[1] = pVer->minor;
				testVector[2] = pucResult[2] = pVer->release;
				ucResponseSize = 0x03;
				break;
			}
		*/

			retCode = SA_Read(pucCommand[1], readBuf);
			if (retCode != SA_SUCCESS) {
				pucResult[0] = retCode; // Failed
			break;
			}

			//Passed
			switch(pucCommand[1])
			{
				// Assign response size.
				case getSN:
					ucResponseSize = sizeof(snData.bytes);
					break;

				case getStatus:
					ucResponseSize = sizeof(statusData.bytes);
					break;

				case getMfrID:
					ucResponseSize = sizeof(mfrIDdata.bytes);
					break;

				case getRevNum:
					ucResponseSize = sizeof(revNumData.bytes);
					break;

				case getMemValid:
					ucResponseSize = 1;
					break;
			}
			memcpy(pucResult, readBuf, ucResponseSize);
			// Using memcpy instead of the for loop below is due to optimization.
			// To prevent that optimizing takes out the for loop "ucResponseSize"
			// would have to be defined as volatile. But then we would get a
			// warning if we use it in an uint8_t argument (as in SAC_SendAndReceive).
			//for (i = 0; i < ucResponseSize; i++)
			//   pucResult[i] = readBuf[i];

			break;

		case CA_DoMAC_:
			pMac = (MACParams *) (pucCommand + 1);

			if ((retCode =  SA_DoMAC(pMac->mode, pMac->keyID, pMac->MACchallenge, MACdigest)))
				pucResult[0] =  retCode;

			else { //Passed
				ucResponseSize = 0x20;
				memcpy(pucResult,MACdigest,ucResponseSize);
			}
			break;

		case CA_Burn_:
			pBurn = (CA_BurnParams *) (pucCommand + 1);
			retCode = pucResult[0] = SA_Burn(pBurn->FuseNumber);
			break;

		case CA_BurnSecure_:
			pBS = (BurnSecureParams*) (pucCommand + 1);
			retCode = pucResult[0] = SA_BurnSecure(pBS->Decrypt, pBS->Map, pBS->KeyID, pBS->Seed);
			break;

		case CA_HostCommand_:
			pHAHC = (CA_HostCommandParams *) (pucCommand + 1);
			retCode = pucResult[0] = SA_HostCommand(pHAHC->Overwrite, pHAHC->bypChallenge, &(pHAHC->otherInfo), pHAHC->caDigest);
			break;

		case CA_LoadSram_:
			pLS = (CA_LoadSramParams *) (pucCommand + 1);
			retCode  = pucResult[0] =  SA_LoadSram(pLS->Key, pLS->KeyID, pLS->Seed);
			break;

		case SA_TalkCmd:
			// Set to the maximum response size.  This is the size of the return buffer.
			ucResponseSize = MAX_SIZE_OF_RESPONSE - 1;

			// Set the command delay to the maximum instead of look-up per command
			//cmdDelay = SAC_GetDelay(pucCommand[2]);
			cmdDelay = SA_DELAY_EXEC_MAXIMUM;

			// Wake/Sleep/NoCrc flags: Never wake or sleep or send CRC.  Possible Flags: WAKE_FLAG | SLEEP_FLAG | NO_CRC_CALC_FLAG
			//wakeSleepCrc = NO_CRC_CALC_FLAG;
			wakeSleepCrc = 0;
			retCode = SAC_SendAndReceive(	(pucCommand + 1),	// data to send
											talkResponse,		// where to put output data
											cmdDelay,			// use the max execution delay
											wakeSleepCrc,		// wake/sleep/noCrc flags: Never wake or sleep or send CRC
											&ucResponseSize);	// in: output buffer size / out: bytes returned
			if (retCode)
			{
				ucResponseSize = 1;
				pucResult[0] = retCode;
			}
			else
			{ //Passed
				for (i = 0; i < ucResponseSize; i++)
					pucResult[i] = talkResponse[i];
			}
			break;

		case SA_Wake:
			retCode = SAC_Wake();
			ucResponseSize = 0;
			break;

		case SA_Sleep:
			SAC_SendSleepFlag();
			ucResponseSize = 0;
			break;

		case getFWVers_:
		   // To distinguish which board is being used, we don't
		   // use the actual library version (SA_GetLibraryVersion)
		   // but a number that indicates the board.
        //pVer = SA_GetLibraryVersion();
        pVer = &libraryVersion;

			testVector[0] = pucResult[0] = pVer->major;
			testVector[1] = pucResult[1] = pVer->minor;
			testVector[2] = pucResult[2] = pVer->release;
			ucResponseSize = 0x03;
			break;

		case SAF_Read_:
			pCR = (CA_ReadParams *) (pucCommand + 1);
			if ((retCode = SAF_Read(pCR->Mode, pCR->Address, &outBuf)))
			pucResult[0] =  retCode;
			else { //Passed
			ucResponseSize = 0x04;
			memcpy(pucResult,&outBuf,sizeof(outBuf));
			}
			break;

/*
		case CPWR_ON: //Soft PWR ON Control for CLIENT SOCKET (JAVAN)
			Cpwr_on();
			pucResult[0] = SA_SUCCESS;
			retCode =  SA_SUCCESS;
			break;

		case HPWR_ON:	//Soft PWR ON Control for HOST SOCKET (JAVAN)

			Hpwr_on();
			pucResult[0] = SA_SUCCESS;
			retCode =  SA_SUCCESS;
			break;

		case CPWR_OFF: //Soft PWR OFF Control for CLIENT SOCKET (JAVAN)
			Cpwr_off();
			pucResult[0] = SA_SUCCESS;
			 retCode =  SA_SUCCESS;
			break;

		case HPWR_OFF: //Soft PWR OFF Control for HOST SOCKET (JAVAN)

			Hpwr_off();
			pucResult[0] = SA_SUCCESS;
			retCode =  SA_SUCCESS;
			break;
	*/
		default:
		return UNKNOWN_COMMAND;
		break;
	}

	*pucResponseSize = ucResponseSize;
	return retCode;
}




