/** \file
 *  \brief Implementations of Parser functions.
 */
#include "parser.h"
#include "usb.h"
#include "lib_Crypto.h"
#include "cmc_Funcs.h"
#include "ll_port.h"
//#include "log.h"
#include <string.h>

#ifdef UART_LOGGING
 #include "..\..\DevelopmentKits\SourceCommon\UART\utility.h"
 #include "ll_timers.h"
#endif

extern const char* GetVersion(void);


/** \brief Parses kit level commands.
 *
 * \param pucCommand - input array of command.
 * \param pucResponse - output array containing the response.
 * \param pucResponseSize - size of the returned packet.
 * \return Parsing result or Command result.
 */
uchar ParseKitCommands(puchar pucCommand, puchar pucResponse, puchar pucResponseSize)
{
  uchar ucReturn = UNKNOWN_COMMAND;
  uchar ucCommandId = pucCommand[PARSER_INDEX_CMD];
  uchar ucResponseSize = 0;   // default response size
  const char* pVersion;


  switch (ucCommandId)
  {
    case KIT_VERSION:
      pVersion = GetVersion();
      strcpy((char *) pucResponse, pVersion);
      ucResponseSize = strlen(pVersion);
#ifdef UART_LOGGING
      uart_PutString((pschar) "BOARD_NAME: ");
      uart_PutData(pucResponse, ucResponseSize);
      uart_PutCrLf();
#endif
      ucReturn = SUCCESS;
      break;

    case KIT_USE_CMC:
#ifdef UART_LOGGING
      uart_PutString((pschar) "KIT_USE_CMC");
      uart_PutCrLf();
#endif
      cmc_SetDeviceAddress(pucCommand[PARSER_INDEX_ADDR] & 0xFC);
      ucReturn = SUCCESS;
      break;
  }

  *pucResponseSize = ucResponseSize;
  return ucReturn;
}


/** \brief Parses CM/CRF commands.
 *
 * \param pucCommand - input array of command.
 * \param ucPacketSize - size of command.
 * \param pucResult - output array containing the response.
 * \param pucResponseSize - size of the returned packet.
 * \return Parsing result or Command result.
 */
uchar ParseCmCommands(puchar pucCommand, uchar ucPacketSize, puchar pucResult, puchar pucResponseSize)
{
  uchar ucReturn = UNKNOWN_COMMAND;
  uchar ucResponseSize = 0;       // default response size
  uchar ucDeviceAddress = pucCommand[PARSER_INDEX_ADDR];
  uchar ucCommandId = pucCommand[PARSER_INDEX_CMD];
  uchar ucByteCount = 0;
  uchar ucCmcDeviceAddress = cmc_GetDeviceAddress();
  uint uiAddress = 0;
  uint uiTemp = 0;
  puchar pucTemp = NULL;


  switch (ucCommandId)
  {
    case LIB_CMD_CM_INIT:
      if (ucPacketSize != 0)
        return UNKNOWN_COMMAND;
#ifdef UART_LOGGING
      uart_PutString((pschar) "CM_INIT");
      uart_PutCrLf();
#endif
      ucReturn = lib_Init();
      break;


    case LIB_SET_USER_ZONE:
    case LIB_SET_USER_ZONE_AT:
      if (ucPacketSize != 2)
        return UNKNOWN_COMMAND;
#ifdef UART_LOGGING
      uart_PutString((pschar) "CMC_SET_USER_ZONE");
      uart_PutCrLf();
#endif
      ucReturn = cm_SetUserZone(ucDeviceAddress, pucCommand[PARSER_INDEX_DATA], ucCommandId == LIB_SET_USER_ZONE_AT);
      break;


    case LIB_READ_USER_ZONE:
      if (ucPacketSize != 4)
        return UNKNOWN_COMMAND;
#ifdef UART_LOGGING
      uart_PutString((pschar) "CMC_READ_USER_ZONE");
      uart_PutCrLf();
#endif
      ucByteCount = pucCommand[PARSER_INDEX_DATA + 2];
      uiAddress = (pucCommand[PARSER_INDEX_DATA] << 8) | pucCommand[PARSER_INDEX_DATA + 1];
      ucReturn = cm_ReadUserZone(ucDeviceAddress, uiAddress, pucResult, ucByteCount, FALSE);
      if (ucReturn == SUCCESS)
        ucResponseSize = ucByteCount;
      break;


    case LIB_WRITE_USER_ZONE:
      ucByteCount = pucCommand[PARSER_INDEX_DATA + 2];
      if (ucPacketSize < 5 || ucByteCount != (ucPacketSize - 4))
        return UNKNOWN_COMMAND;
#ifdef UART_LOGGING
      uart_PutString((pschar) "CMC_WRITE_USER_ZONE");
      uart_PutCrLf();
#endif
      uiAddress = (pucCommand[PARSER_INDEX_DATA] << 8) | pucCommand[PARSER_INDEX_DATA + 1];
      ucReturn = cm_WriteUserZone(ucDeviceAddress, uiAddress, pucCommand + PARSER_INDEX_DATA + 3, ucByteCount);
      break;


    case LIB_WRITE_SYSTEM:
    case LIB_WRITE_SYSTEM_AT:
      ucByteCount = pucCommand[PARSER_INDEX_DATA + 1];
      if (ucPacketSize < 4 || ucByteCount != (ucPacketSize - 3))
        return UNKNOWN_COMMAND;
#ifdef UART_LOGGING
      uart_PutString((pschar) "LIB_SYSTEM_WRITE");
      uart_PutCrLf();
#endif
      ucReturn = cm_WriteConfigZone(ucDeviceAddress, pucCommand[PARSER_INDEX_DATA], pucCommand + PARSER_INDEX_DATA + 2,
                                    ucByteCount, ucCommandId == LIB_WRITE_SYSTEM_AT);
      break;


    case LIB_WRITE_FUSE:
      if (ucPacketSize != 2)
        return UNKNOWN_COMMAND;
#ifdef UART_LOGGING
      uart_PutString((pschar) "CMC_WRITE_FUSE");
      uart_PutCrLf();
#endif
      ucReturn = cm_WriteFuse(ucDeviceAddress, pucCommand[PARSER_INDEX_DATA]);
      break;


    case LIB_READ_SYSTEM:
      if (ucPacketSize != 3)
        return UNKNOWN_COMMAND;
#ifdef UART_LOGGING
      uart_PutString((pschar) "CMC_SYSTEM_READ");
      uart_PutCrLf();
#endif
      ucByteCount = pucCommand[PARSER_INDEX_DATA + 1];
      ucReturn = cm_ReadConfigZone(ucDeviceAddress, pucCommand[PARSER_INDEX_DATA], pucResult, ucByteCount);
      if (ucReturn == SUCCESS)
        ucResponseSize = ucByteCount;
      break;


    case LIB_READ_FUSE:
      if (ucPacketSize != 1)
        return UNKNOWN_COMMAND;
#ifdef UART_LOGGING
      uart_PutString((pschar) "CMC_READ_FUSE");
      uart_PutCrLf();
#endif
      ucReturn = cm_ReadFuse(ucDeviceAddress, pucResult);
      if (ucReturn == SUCCESS)
        ucResponseSize = 1;
      break;


    case LIB_READ_CHECKSUM:
      if (ucPacketSize != 1)
        return UNKNOWN_COMMAND;
#ifdef UART_LOGGING
      uart_PutString((pschar) "CMC_READ_CHECKSUM");
      uart_PutCrLf();
#endif
      ucReturn = cm_ReadChecksum(ucDeviceAddress, pucResult);
      if (ucReturn == SUCCESS)
        ucResponseSize = 2;
      break;


    case LIB_VERIFY_AUTH:
    case LIB_VERIFY_CRYPTO:
#ifdef UART_LOGGING
      uart_PutString((pschar) "CMC_VERIFY_CRYPTO");
      uart_PutCrLf();
#endif
      if (ucCmcDeviceAddress) {
        if (ucPacketSize != 26)
          return UNKNOWN_COMMAND;
        ucReturn = cm_VerifyCrypto(ucDeviceAddress, pucCommand[PARSER_INDEX_DATA],
                                    pucCommand + PARSER_INDEX_DATA + 9, pucCommand + PARSER_INDEX_DATA + 1,
                                    ucCommandId - LIB_VERIFY_AUTH);
      }
      else {
        if (ucPacketSize != 18 && ucPacketSize != 10)
          return UNKNOWN_COMMAND;
        ucReturn = cm_VerifyCrypto(ucDeviceAddress, pucCommand[PARSER_INDEX_DATA],
                                    pucCommand + PARSER_INDEX_DATA + 1,
                                    (ucPacketSize == 10) ? NULL : pucCommand + PARSER_INDEX_DATA + 9,
                                    ucCommandId - LIB_VERIFY_AUTH);
      }
      break;

/*
    case RESET_CRYPTO:
      if (ucPacketSize != 1)
        return UNKNOWN_COMMAND;
#ifdef UART_LOGGING
      uart_SendStringProgMem("RESET_CRYPTO");
      uart_SendCrLf();
#endif
      ucReturn = cm_ResetCrypto(ucDeviceAddress);
      break;
*/

    case LIB_WRITE_CHECKSUM:
      if (ucPacketSize != 2 && ucPacketSize != 4)
        return UNKNOWN_COMMAND;
#ifdef UART_LOGGING
      uart_PutString((pschar) "CMC_SEND_CHECKSUM");
      uart_PutCrLf();
#endif
      ucReturn = cm_SendChecksum(ucDeviceAddress, ucPacketSize == 2 ? NULL : pucCommand + PARSER_INDEX_DATA + 1);
      break;


    case LIB_VERIFY_WRITE_PWD:
    case LIB_VERIFY_READ_PWD:
      if (ucPacketSize != 5)
        return UNKNOWN_COMMAND;
#ifdef UART_LOGGING
      uart_PutString((pschar) "VERIFY_PASSWORD");
      uart_PutCrLf();
#endif
      ucReturn = cm_VerifyPassword(ucDeviceAddress, pucCommand + PARSER_INDEX_DATA + 1,
                                    pucCommand[PARSER_INDEX_DATA],
                                    ucCommandId - LIB_VERIFY_WRITE_PWD);
      break;


    case LIB_CMD_RESET_PASSWORD:
      if (ucPacketSize != 1)
        return UNKNOWN_COMMAND;
#ifdef UART_LOGGING
      uart_PutString((pschar) "RESET_PASSWORD");
      uart_PutCrLf();
#endif
      ucReturn = cm_ResetPassword(ucDeviceAddress);
      break;


    case CMC_ENCRYPT_PW:
      if (ucPacketSize != 2)
        return UNKNOWN_COMMAND;
#ifdef UART_LOGGING
      uart_PutString((pschar)"CMC_ENCRYPT_PASSWORD");
      uart_PutCrLf();
#endif
      ucReturn = cmc_EncryptPassword(ucDeviceAddress, pucCommand[PARSER_INDEX_DATA], pucResult);
      if (ucReturn == SUCCESS)
        ucResponseSize = 3;
      break;


      /*
    case CMC_SHA1:
      if (pucCommand[1] != (ucPacketSize - 2))
      {
        pucResult[0] = UNKNOWN_COMMAND;
        return UNKNOWN_COMMAND;
      }
      cmc_SHA1(pucCommand + 2, pucCommand[1], pucResult + 1);
      pucResult[0] = SUCCESS;
      ucReturn = SUCCESS;
      ucResponseSize = 21;
      break;
*/

    case LIB_CMD_GET_WRITE_MAC:
      if (ucPacketSize != 1)
        return UNKNOWN_COMMAND;
#ifdef UART_LOGGING
      uart_PutString((pschar) "GET_WRITE_MAC");
      uart_PutCrLf();
#endif
      ucReturn = cm_GetSendChecksum(ucDeviceAddress, pucResult);
      if (ucReturn == SUCCESS) {
        ucResponseSize = 2;
      }
      break;


    case LIB_CMD_GET_ENCR_WRITE_PWD:
    case LIB_CMD_GET_ENCR_READ_PWD:
      if (ucPacketSize != 5)
        return UNKNOWN_COMMAND;
#ifdef UART_LOGGING
      uart_PutString((pschar) "VERIFY_PASSWORD_ENCRYPTED");
      uart_PutCrLf();
#endif
      pucTemp = pucCommand + PARSER_INDEX_DATA + 1;
      ucReturn = cm_VerifyPassword(ucDeviceAddress, pucTemp, pucCommand[PARSER_INDEX_DATA], (ucCommandId == LIB_CMD_GET_ENCR_WRITE_PWD ? 0 : 1));
      if (ucReturn == SUCCESS) {
        ucResponseSize = 3;
        memcpy(pucResult, pucTemp, 3);
      }
      break;


    case LIB_CMD_GET_ENCR_WRITE_DATA:
      ucByteCount = pucCommand[PARSER_INDEX_DATA + 2];
      if (ucPacketSize < 5 || ucByteCount != (ucPacketSize - 4))
        return UNKNOWN_COMMAND;
      uiTemp = (pucCommand[PARSER_INDEX_DATA] << 8) | pucCommand[PARSER_INDEX_DATA + 1];
      pucTemp = pucCommand + PARSER_INDEX_DATA + 3;
      ucReturn = cm_WriteUserZone(ucDeviceAddress, uiTemp, pucTemp, ucByteCount);
      if (ucReturn == SUCCESS) {
        ucResponseSize = ucByteCount + 2;
        memcpy(pucResult, pucTemp, ucByteCount);
        cm_GetSendChecksum(ucDeviceAddress, pucResult + ucByteCount);
      }
      break;


    case LIB_CMD_GET_ENCR_READ_DATA:
      if (ucPacketSize != 4)
        return UNKNOWN_COMMAND;
      uiTemp = (pucCommand[PARSER_INDEX_DATA] << 8) | pucCommand[PARSER_INDEX_DATA + 1];
      ucByteCount = pucCommand[PARSER_INDEX_DATA + 2];
      pucTemp = pucCommand + PARSER_INDEX_DATA + 3 + ucByteCount;  // clear data
      ucReturn = cm_ReadUserZone(ucDeviceAddress, uiTemp, pucTemp, ucByteCount, TRUE);
      if (ucReturn == SUCCESS) {
        ucResponseSize = ucByteCount << 1;
        memcpy(pucResult, pucTemp, ucResponseSize);
      }
      break;
  }

  *pucResponseSize = ucResponseSize;
  return ucReturn;
}


/** \brief Parses CMC commands.
 *
 * \param pucCommand - input array of command.
 * \param ucPacketSize - size of command.
 * \param pucResult - output array containing the response.
 * \param pucResponseSize - size of the returned packet.
 * \return Parsing result or Command result.
 */
uchar ParseCmcCommands(puchar pucCommand, uchar ucPacketSize, puchar pucResult, puchar pucResponseSize)
{
  uchar ucDeviceAddress = pucCommand[PARSER_INDEX_ADDR];
  uchar ucCommandId = pucCommand[PARSER_INDEX_CMD];

  uchar ucReturn = UNKNOWN_COMMAND;
  uchar ucResponseSize = 0;   // default response size
  uint uiTemp = 0;
  uchar ucByteCount = 0;
  static Auth auth;           // Allocate auth data space


  switch (ucCommandId)
  {
    case CMC_CLEAR:
      if (ucPacketSize != 1)
        return UNKNOWN_COMMAND;
      ucReturn = cmc_Clear(ucDeviceAddress);
      break;

    case CMC_VERIFY_FLASH:
      if (ucPacketSize != 41)
        return UNKNOWN_COMMAND;
      ucReturn = cmc_VerifyFlash(ucDeviceAddress, pucCommand + PARSER_INDEX_DATA, pucCommand + PARSER_INDEX_DATA + 20);
      break;

    case CMC_STARTUP:
      if (ucPacketSize != 21)
        return UNKNOWN_COMMAND;
      // *** FNL *** parameters are not changed yet
      ucReturn = cmc_StartUp(ucDeviceAddress, pucCommand + PARSER_INDEX_DATA, pucResult, pucResult + 20);
      if (ucReturn == SUCCESS)
        ucResponseSize = 40;
      break;

    case CMC_CHALLENGE:
      if (ucPacketSize != 21)
        return UNKNOWN_COMMAND;
      // *** FNL *** parameters are not changed yet
      ucReturn = cmc_ChallengeResponse(ucDeviceAddress, pucCommand + PARSER_INDEX_DATA);
      break;

/*
case CMC_START_CHALLENGE:
    if (ucPacketSize != 53)
    {
        pucResult[0] = UNKNOWN_COMMAND;
        return UNKNOWN_COMMAND;
    }
    ucReturn = cmc_StartChallenge(cmc_GetDeviceAddress(), pucCommand+1, pucCommand+21, pucCommand+37);
    pucResult[0] = ucReturn;
    break;
*/

    case CMC_READ_MANUFACTURING_ID:
      if (ucPacketSize != 1)
        return UNKNOWN_COMMAND;
      ucReturn = cmc_ReadManufID(ucDeviceAddress, pucResult);
      if (ucReturn == SUCCESS)
        ucResponseSize = 16;
      break;

    case CMC_READ_MEMORY:
      ucByteCount = pucCommand[PARSER_INDEX_DATA + 2];
      if (ucPacketSize != 4 || ucByteCount > 15)
        return UNKNOWN_COMMAND;
      uiTemp = (pucCommand[PARSER_INDEX_DATA] << 8) | pucCommand[PARSER_INDEX_DATA + 1];
      ucReturn = cmc_ReadMemory(ucDeviceAddress, uiTemp, ucByteCount + 1, pucResult);
      if (ucReturn == SUCCESS)
        ucResponseSize = ucByteCount + 1;
      break;


    case CMC_WRITE_MEMORY:
      ucByteCount = pucCommand[PARSER_INDEX_DATA + 2];
      if (ucPacketSize < 5 || ucByteCount > 15)
        return UNKNOWN_COMMAND;
      uiTemp = (pucCommand[PARSER_INDEX_DATA] << 8) | pucCommand[PARSER_INDEX_DATA + 1];
      ucReturn = cmc_WriteMemory(ucDeviceAddress, uiTemp, ucByteCount + 1, pucCommand + PARSER_INDEX_DATA + 3);
      break;


    case CMC_GET_RANDOM:
      if (ucPacketSize != 1)
        return UNKNOWN_COMMAND;
            ucReturn = cmc_GetRandom(ucDeviceAddress, pucResult);
      if (ucReturn == SUCCESS)
        ucResponseSize = 20;
      break;


    case CMC_READ_COUNTER:
      if (ucPacketSize != 2 || pucCommand[PARSER_INDEX_DATA] > 3)
        return UNKNOWN_COMMAND;
      ucReturn = cmc_ReadCounter(ucDeviceAddress, pucCommand[PARSER_INDEX_DATA], pucResult);
      if (ucReturn == SUCCESS)
        ucResponseSize = 4;
      break;


    case CMC_INCREMENT_COUNTER:
      if (ucPacketSize != 2 || pucCommand[PARSER_INDEX_DATA] > 3)
        return UNKNOWN_COMMAND;
      ucReturn = cmc_IncrementCounter(ucDeviceAddress, pucCommand[PARSER_INDEX_DATA]);
      break;


    case CMC_WRITE_MEMORY_ENCR:
      if (ucPacketSize != 35)
        return UNKNOWN_COMMAND;
      uiTemp = (pucCommand[PARSER_INDEX_DATA] << 8) | pucCommand[PARSER_INDEX_DATA + 1];
      ucReturn = cmc_WriteMemoryEncrypted(ucDeviceAddress, uiTemp, pucCommand + PARSER_INDEX_DATA + 2, pucCommand + PARSER_INDEX_DATA + 18);
      break;


    case CMC_READ_MEMORY_DIGEST:
      if (ucPacketSize != 3)
        return UNKNOWN_COMMAND;
      uiTemp = (pucCommand[PARSER_INDEX_DATA] << 8) | pucCommand[PARSER_INDEX_DATA + 1];
      ucReturn = cmc_ReadMemoryDigest(ucDeviceAddress, uiTemp, pucResult);
      if (ucReturn == SUCCESS)
        ucResponseSize = 20;
      break;


    case CMC_LOCK:
      if (ucPacketSize != 1)
        return UNKNOWN_COMMAND;
      ucReturn = cmc_Lock(ucDeviceAddress);
      break;


    case CMC_CRUNCH:
      if (ucPacketSize != 10)
      	return UNKNOWN_COMMAND;
      ucReturn = cmc_Crunch(ucDeviceAddress, pucCommand[PARSER_INDEX_DATA], pucCommand + PARSER_INDEX_DATA + 1, pucResult);
      if (ucReturn == SUCCESS)
        ucResponseSize = 8;
      break;


    case CMC_STATUS:
      if (ucPacketSize != 1)
        return UNKNOWN_COMMAND;
#ifdef UART_LOGGING
      uart_PutString((pschar) "CMC_STATUS");
      uart_PutCrLf();
#endif
      ucReturn = cmc_Status(ucDeviceAddress, pucResult);
      ucResponseSize = 1;
      break;


    case CMC_GRIND_BYTES:
      ucByteCount = pucCommand[PARSER_INDEX_DATA];
      if (ucPacketSize < 3 || ucByteCount > 20)
        return UNKNOWN_COMMAND;
#ifdef UART_LOGGING
      uart_PutString((pschar)"CMC_GRIND_BYTES");
      uart_PutCrLf();
#endif
      ucReturn = cmc_GrindBytes(ucDeviceAddress, ucByteCount, pucCommand + PARSER_INDEX_DATA + 1, pucResult);
      if (ucReturn == SUCCESS)
        ucResponseSize = ucByteCount + 1;
      break;


    case CMC_RESET_SECURITY:
      if (ucPacketSize != 1)
        return UNKNOWN_COMMAND;
#ifdef UART_LOGGING
      uart_PutString((pschar) "CMC_RESET_SECURITY");
      uart_PutCrLf();
#endif
      if (cmc_GetDeviceAddress())
        cmc_Reset();
      ucReturn = lib_Init();
      break;


    case CMC_RESET:
      if (ucPacketSize != 3)
        return UNKNOWN_COMMAND;
#ifdef UART_LOGGING
      uart_PutString((pschar) "CMC_RESET");
      uart_PutCrLf();
#endif
      uiTemp = (pucCommand[PARSER_INDEX_DATA] << 8) | pucCommand[PARSER_INDEX_DATA + 1];
      cmc_ResetConfigurably(uiTemp);
      ucReturn = SUCCESS;
      break;


    case CMC_AUTH_AND_ACCESS:
      auth.ucCmcDeviceAddress = ucDeviceAddress;
      auth.kid = &pucCommand[PARSER_INDEX_DATA];
      auth.keyIndex = pucCommand[PARSER_INDEX_DATA + PARSER_INDEX_KID];
      auth.encrypt = pucCommand[PARSER_INDEX_DATA + PARSER_INDEX_ENCRYPT_ID];
      auth.ucCmDeviceAddress = pucCommand[PARSER_INDEX_DATA + PARSER_INDEX_DEVADDR_ID];
      auth.zone = pucCommand[PARSER_INDEX_DATA + PARSER_INDEX_AUTHZONE_ID];
      auth.antiTearing = pucCommand[PARSER_INDEX_DATA + PARSER_INDEX_AT];
      auth.mode = pucCommand[PARSER_INDEX_DATA + PARSER_INDEX_MODE_ID];
      auth.usePassword = pucCommand[PARSER_INDEX_DATA + PARSER_INDEX_USE_PSW];
      auth.passwordSet = pucCommand[PARSER_INDEX_DATA + PARSER_INDEX_PSW_SET];
      auth.readWritePassword = pucCommand[PARSER_INDEX_DATA + PARSER_INDEX_PSW_RW];

      if (auth.mode != AUTH_ACCESS_MODE_AUTH_ONLY)
      {
        auth.address = (pucCommand[PARSER_INDEX_DATA + PARSER_INDEX_ADDRESS_ID]) << 8;
        auth.address |= (pucCommand[PARSER_INDEX_DATA + PARSER_INDEX_ADDRESS_ID + 1]);
        auth.nBytes = pucCommand[PARSER_INDEX_DATA + PARSER_INDEX_NUMBYTES_ID];
        ucResponseSize = (auth.nBytes << 1) + CHECKSUM_LENGTH;
        if (auth.usePassword)
          ucResponseSize += PASSWORD_LENGTH;
        if (ucResponseSize > (USB_BUFFER_SIZE - 9))
        {
          //Don't let outbuffer overflow.
          ucReturn = FAILED;
          break;
        }
        if (auth.mode == AUTH_ACCESS_MODE_READ)
        {
        	 auth.encryptedData = pucResult;
			 auth.clearData = auth.encryptedData + auth.nBytes;
			 auth.checksum = auth.clearData + auth.nBytes;
        }
        else
        {
          auth.encryptedData = pucResult;
          auth.clearData = &pucCommand[PARSER_INDEX_DATA + PARSER_INDEX_DATA_ID];
          // Save clear data.
          memcpy(pucResult + auth.nBytes, auth.clearData, auth.nBytes);
          auth.checksum = auth.encryptedData + (auth.nBytes << 1);
        }
        auth.password = auth.checksum + CHECKSUM_LENGTH;
      }
     // else: authenticate only
//           ClearLog();
//           Log(TRUE);
#ifdef UART_LOGGING
      uart_PutString((pschar) "AuthenticateAndAccess:\r\n");
      // TODO: Start a timeout timer here.
      // The function below logs a lot of data. CMC would time out if we
      // don't use the full buffer from the start.
      uart_WaitForBufferEmptyTx();
#endif
      ucReturn = cmc_AuthenticateAndAccess(&auth);

//           Log(FALSE);
      if ((auth.mode == AUTH_ACCESS_MODE_READ) && (ucReturn == 0xC9) && (auth.address & 0x8000))
      	// If Topaz integrated MAC returned a checksum error we still return the response buffer.
      	break;

      if (ucReturn)
        ucResponseSize = 0;

      break;

/*
    case CMC_GET_LOG:

      if (pucCommand[PARSER_INDEX_DATA])      // If the 1st byte of data (Clear flag) is set (TRUE)
      {                                       // Clear the log without returning it
        ClearLog();
        ucResponseSize = 0;
        ucReturn = SUCCESS;
      }
      else                                    // Clear is not set (FALSE), so send the logged data
      {
        logSize = GetLogSize();               // How many bytes to send ?
        if (logSize)                          // If there are bytes to send
        {
          int nBytes = logSize < 128 ? logSize : 128;
          logBuffer = GetLogBuffer(nBytes);
//            SendPacket(CLASS_CM_CMC, logBuffer, nBytes, SUCCESS);
        }
      }
      break;
*/
  }

  *pucResponseSize = ucResponseSize;
  return ucReturn;
}
