/** \file
 *  \brief Header file for parser.c.
 */

#ifndef _PARSER_H_
#define _PARSER_H_

#include "config.h"

////////////////////////////// from Aris+ ////////////////////////////////////
/** \brief enumeration of byte location in string received from host */
enum {
  INDEX_START = 0x00,                         ///< Starting byte is at location 0
  START_BYTE = 3,
  INDEX_CLASSID = (INDEX_START + START_BYTE),   ///< Class ID at byte 3
  CLASSID_BYTE = 1,
  INDEX_CMDID = (INDEX_CLASSID + CLASSID_BYTE), ///< Command id at byte 4
  CMD_BYTE = 1,
  INDEX_LEN = (INDEX_CMDID + CMD_BYTE),         ///< Payload length at byte 5
  LEN_BYTE = 2,
  INDEX_PAYLOAD = (INDEX_LEN + LEN_BYTE)        ///< Payload starts at byte 7
};

// Indexes above are used in packet parsing.
// Since the parser receives a pointer into the usb rx buffer
// with an offset of INDEX_CMDID, below are enumarations for
// parsing the command data.
enum {
  PARSER_INDEX_CMD    = 0,
  // High byte of payload size is assumed to be zero.
  PARSER_INDEX_SIZE   = 2,
  PARSER_INDEX_ADDR   = 3,
  PARSER_INDEX_DATA   = 4
};


// More indexes into command data, specifically authorization data
#define PARSER_INDEX_KID                16
#define PARSER_INDEX_ENCRYPT_ID         17
#define PARSER_INDEX_DEVADDR_ID         18
#define PARSER_INDEX_AUTHZONE_ID        19
#define PARSER_INDEX_AT                 20
#define PARSER_INDEX_MODE_ID            21
#define PARSER_INDEX_USE_PSW            22
#define PARSER_INDEX_PSW_SET            23
#define PARSER_INDEX_PSW_RW             24
#define PARSER_INDEX_ADDRESS_ID         25
#define PARSER_INDEX_NUMBYTES_ID        27
#define PARSER_INDEX_DATA_ID            28

// kit command ids (class id = 0)
#define KIT_VERSION                      1                    ///< Get the version of the Kit
#define KIT_USE_CMC                      2


#define BOWLINE_CMD_START               64

#define LIB_CM_CMD_START               LIB_WRITE_USER_ZONE
#define LIB_CM_CMD_END                 LIB_VERIFY_READ_PWD

//CMC command ids (class id = 2)
// 16 to 37 and 43 to 46 are CMC commands
#define LIB_CMD_START               ( 0)
#define CMC_VERIFY_FLASH            (LIB_CMD_START + 16)      ///< Verify flash             16
#define CMC_STARTUP                 (LIB_CMD_START + 17)      ///< Startup                  17
#define CMC_CHALLENGE               (LIB_CMD_START + 18)      ///< Challenge                18
#define CMC_AUTH_1                  (LIB_CMD_START + 19)      ///< Auth_1                   19
#define CMC_AUTH_2                  (LIB_CMD_START + 20)      ///< Auth_2                   20
#define CMC_ENCRYPT_PW              (LIB_CMD_START + 21)      ///< Encrypt pwd              21
#define CMC_ENCRYPTION_1            (LIB_CMD_START + 22)      ///< Encrypt_1                22
#define CMC_ENCRYPTION_2            (LIB_CMD_START + 23)      ///< Encrypt_2                23
#define CMC_GRIND_BYTES             (LIB_CMD_START + 24)      ///< Grindbytes               24
#define CMC_GET_RANDOM              (LIB_CMD_START + 25)      ///< Get random               25
#define CMC_INCREMENT_COUNTER       (LIB_CMD_START + 26)      ///< Increment counter        26
#define CMC_READ_COUNTER            (LIB_CMD_START + 27)      ///< Read counter             27
#define CMC_WRITE_MEMORY            (LIB_CMD_START + 28)      ///< Write memory             28
#define CMC_WRITE_MEMORY_ENCR       (LIB_CMD_START + 29)      ///< Write memory encr        29
#define CMC_WRITE_MEMORY_AUTH       (LIB_CMD_START + 30)      ///< Write memory auth        30
#define CMC_READ_MEMORY             (LIB_CMD_START + 31)      ///< Read memory              31
#define CMC_READ_MEMORY_DIGEST      (LIB_CMD_START + 32)      ///< Read memory digst        32
#define CMC_READ_MANUFACTURING_ID   (LIB_CMD_START + 33)      ///< Read Manufacturing id    33
#define CMC_LOCK                    (LIB_CMD_START + 34)      ///< Lock                     34
#define CMC_CLEAR                   (LIB_CMD_START + 35)      ///< Clear                    35
#define CMC_STATUS                  (LIB_CMD_START + 36)      ///< Status                   36
#define CMC_RESET_SECURITY          (LIB_CMD_START + 37)      ///< Reset security           37

// Due to historical reasons the CM commands below are grouped with the CMC command ids.
#define LIB_CMD_GET_ENCR_WRITE_DATA (LIB_CMD_START + 38)      ///< Get Encr Write data      38
#define LIB_CMD_GET_ENCR_READ_DATA  (LIB_CMD_START + 39)      ///< Get Encr Read data       39
#define LIB_CMD_GET_WRITE_MAC       (LIB_CMD_START + 40)      ///<                          40
#define LIB_CMD_GET_ENCR_WRITE_PWD  (LIB_CMD_START + 41)      ///<                          41
#define LIB_CMD_GET_ENCR_READ_PWD   (LIB_CMD_START + 42)      ///<                          42

#define CMC_RESET                   (LIB_CMD_START + 43)      ///<                          43
#define CMC_CRUNCH                  (LIB_CMD_START + 44)      ///< Crunch                   44
#define CMC_AUTH_AND_ACCESS         (LIB_CMD_START + 45)      ///< Enters authentication / encryption mode and
#define CMC_GET_LOG                 (LIB_CMD_START + 46)      ///< Get the logged data for security

#define LIB_CMD_RESET_PASSWORD      (LIB_CMD_START + 47)      ///< resets a password        47

#define LIB_CMD_CM_INIT             (LIB_CMD_START + 49)      ///< initializes the library  49


#define CMC_CMD_START               CMC_VERIFY_FLASH
#define CMC_CMD_END                 CMC_GET_LOG

// Bowline commands
#define BOWLINE_INIT                BOWLINE_CMD_START
#define TAG_N_LIB_INIT              (BOWLINE_CMD_START +  1)
#define ABORT                       (BOWLINE_CMD_START +  2)
#define CLEAR                       (BOWLINE_CMD_START +  3)
#define POLL_CONTINUOUS             (BOWLINE_CMD_START +  4)
#define POLL_SINGLE                 (BOWLINE_CMD_START +  5)
#define READ_BUFFER                 (BOWLINE_CMD_START +  6)
#define READ_REGISTER               (BOWLINE_CMD_START +  7)
#define RF_ON_OFF                   (BOWLINE_CMD_START +  8)
#define SLEEP                       (BOWLINE_CMD_START +  9)
#define TX_DATA                     (BOWLINE_CMD_START + 10)
#define WRITE_BUFFER                (BOWLINE_CMD_START + 11)
#define WRITE_REGISTER              (BOWLINE_CMD_START + 12)

// RF tag commands
#define REQB                        (BOWLINE_CMD_START + 20)
#define WUPB                        (BOWLINE_CMD_START + 21)
#define SLOT_MARKER                 (BOWLINE_CMD_START + 22)
#define ATTRIB                      (BOWLINE_CMD_START + 23)
#define HLTB                        (BOWLINE_CMD_START + 24)
#define DESELECT                    (BOWLINE_CMD_START + 25)
#define IDLE                        (BOWLINE_CMD_START + 26)
#define CMC_BOWLINE_START            BOWLINE_CMD_START
#define CMC_BOWLINE_END             (BOWLINE_CMD_START + 26)


//#define CM_INIT             49
//#define RESET_CRYPTO        59


//Encryption functions
//#define READ_USER_ZONE_ENCRYPTION      39 //0x42
//#define WRITE_USER_ZONE_ENCRYPTION     38 //0x43

// Needs to be split for read and write
//#define VERIFY_PASSWORD_ENCRYPTION     41 //0x44


uchar ParseKitCommands(puchar pucCommand, puchar pucResponse, puchar pucResponseSize);
uchar ParseCmCommands(puchar pucCommand, uchar ucCommandSize, puchar pucResponse, puchar pucResponseSize);
uchar ParseCmcCommands(puchar pucCommand, uchar ucCommandSize, puchar pucResponse, puchar pucResponseSize);
uchar ParseBowlineCommands(puchar pucCommand, uchar ucCommandSize, puchar pucResponse, puchar pucResponseSize);

#endif // _PARSER_H_
