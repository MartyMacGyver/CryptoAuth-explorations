/** \file
 *  \brief Header file for parser.c.
 */

#ifndef _PARSER_H_
#define _PARSER_H_

#include "conf\config.h"
#include "SA_Func.h"

//! Size of ucCommandReceived buffer
#define MAX_SIZE_OF_COMMAND     128
//! Size of ucResponseReceived buffer
#define MAX_SIZE_OF_RESPONSE    128

// Functions OP Code
#define CA_Read_         	0x11
#define CA_DoMAC_			0x12
#define CA_HostCommand_     0x13
#define CA_Burn_			0x14
#define CA_BurnSecure_		0x15
#define CA_LoadSram_		0x16
#define getFWVers_			0x17
#define SAF_Read_    		0x18

//#define SA_PauseLong_		0x19

#define HPWR_ON				0x1B
#define CPWR_ON				0x1C
#define HPWR_OFF			0x1D
#define CPWR_OFF			0x1E

#define SA_TalkCmd			0x31
#define SA_Wake				0x32
#define SA_Sleep			0x33


U8 ucCommandReceived[MAX_SIZE_OF_COMMAND];  //! buffer containing command received in binary
U8 ucResponseReceived[MAX_SIZE_OF_RESPONSE]; //! buffer containing response received in binary
U8 command_parse(puchar pucCommand, U8 ucCommandSize,puchar pucResponse, puchar pucResponseSize);


/*
 * typedefs
 */

// MAC input structs
typedef struct _MACParams
{

	uint8_t	mode;
	uint16_t	keyID;				// fixme: make sure the byte order is correctly maintained!!!
	uint8_t	MACchallenge[32];

} MACParams;


// BurnSecure input structures
typedef struct _BurnSecureParams
{
	uint8_t  Decrypt;
	uint8_t  Map[11];
	uint16_t KeyID;
   uint8_t  Seed[16];

} BurnSecureParams;

// Burn input structures
typedef struct _CA_BurnParams
{
	uint8_t  FuseNumber;
	//uint8_t  LowVoltage;

} CA_BurnParams;



typedef struct _CA_HostCommandParams
{
	uint8_t  				Overwrite;
	uint8_t 				   bypChallenge[32];
	Host1OtherInfo_str 	otherInfo;
	uint8_t 				   caDigest[32];

} CA_HostCommandParams;


// LoadSram input structures
//int8_t SA_LoadSram(uint8_t *Key, uint16_t KeyID, uint8_t *Seed)
typedef struct _CA_LoadSramParams
{
    uint8_t  Key[32];
	uint16_t KeyID;
    uint8_t  Seed[16];


} CA_LoadSramParams;


typedef struct _CA_ReadParams
{
    uint8_t   Mode;
 uint16_t Address;

}CA_ReadParams;

/*
typedef struct _HA_GenPersonalizationKeyParams
{

	uint16_t KeyID;
	uint8_t  Seed[16];

} HA_GenPersonalizationKeyParams;


typedef struct _HA_BurnSecureParams
{
	uint8_t 	byDecrypt;
	uint16_t 	swBurnTime;
	uint8_t 	bypMap[11];

} HA_BurnSecureParams;

*/
#endif // _PARSER_H_



