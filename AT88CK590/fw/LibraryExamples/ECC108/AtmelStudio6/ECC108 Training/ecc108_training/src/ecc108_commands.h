/*
 * ecc108_commands.h
 *
 * Created: 3/25/2013 10:24:34 AM
 *  Author: james.tomasetta
 */ 

#ifndef ECC108_COMMANDS_H_
#define ECC108_COMMANDS_H_

#include "sha204_command_marshaling.h"
#include "sha204_helper.h"


#define ECC108_RSP_SIZE_MIN SHA204_RSP_SIZE_MIN
#define ECC108_SUCCESS SHA204_SUCCESS


//////////////////////////////////////////////////////////////////////
// command op-code definitions
#define ECC108_GENKEY                   ((uint8_t) 0x40)       //!< GenKey command op-code
#define ECC108_SIGN                     ((uint8_t) 0x41)       //!< Sign command op-code
#define ECC108_VERIFY                   ((uint8_t) 0x45)       //!< Verify command op-code
#define ECC108_PRIVWRITE                ((uint8_t) 0x46)       //!< PrivWrite command op-code




//#define ECC108_CHECKMAC                 ((uint8_t) 0x28)       //!< CheckMac command op-code
//#define ECC108_MAC                      ((uint8_t) 0x08)       //!< MAC command op-code
//#define ECC108_PAUSE                    ((uint8_t) 0x01)       //!< Pause command op-code
//#define ECC108_READ                     ((uint8_t) 0x02)       //!< Read command op-code
//#define ECC108_WRITE                    ((uint8_t) 0x12)       //!< Write command op-code





//////////////////////////////////////////////////////////////////////
// GenKey command definitions
#define GENKEY_ZONE_IDX                 SHA204_PARAM1_IDX      //!< GenKey command index for zone
#define GENKEY_KEYID_IDX                SHA204_PARAM2_IDX      //!< GenKey command index for key id
#define GENKEY_DATA_IDX                 SHA204_DATA_IDX        //!< GenKey command index for optional data
#define GENKEY_COUNT                    SHA204_CMD_SIZE_MIN    //!< GenKey command packet size without "other data"
#define GENKEY_COUNT_DATA               (10)                   //!< GenKey command packet size with "other data"
#define GENKEY_OTHER_DATA_SIZE          (3)                    //!< GenKey size of "other data"
#define GENKEY_MODE_PRIVATE             ((uint8_t) 4)          //!< GenKey zone id data
#define GENKEY_MODE_PUBLIC              ((uint8_t) 0)          //!< GenKey zone id data
//! GenKey typical command delay
#define GENKEY_DELAY                    (70)
//! GenKey maximum execution time
#define GENKEY_EXEC_MAX                 (250)
#define GENKEY_RSP_SIZE                 ((uint8_t) 75)    //!< response size of GenKey command


/** 
 * \brief This structure contains the parameters for the \ref ecc108_gen_key function.
 */
struct ecc108_gen_key_parameters {
   uint8_t *tx_buffer;        //!< pointer to send buffer
   uint8_t *rx_buffer;        //!< pointer to receive buffer
   uint8_t mode;              //!< what to include in the MAC calculation
   uint16_t key_id;           //!< what key to use for the MAC calculation
   uint8_t *other_data;       //!< pointer to 3 bytes of data to be sent to client
};


//////////////////////////////////////////////////////////////////////
// Sign command definitions
#define SIGN_MODE_IDX                 SHA204_PARAM1_IDX      //!< Sign command index for zone
#define SIGN_KEYID_IDX                SHA204_PARAM2_IDX      //!< Sign command index for key id
#define SIGN_COUNT                    SHA204_CMD_SIZE_MIN    //!< Sign command packet size without "other data"
#define SIGN_COUNT_DATA               (7)                   //!< Sign command packet size with "other data"
#define SIGN_MODE_TEMPKEY             ((uint8_t) 0x80)          //!< Sign zone id data
//! Sign typical command delay
#define SIGN_DELAY                    (60)
//! Sign maximum execution time
#define SIGN_EXEC_MAX                 (100)
#define SIGN_RSP_SIZE                 ((uint8_t) 75)    //!< response size of GenKey command

/** 
 * \brief This structure contains the parameters for the \ref ecc108_sign function.
 */
struct ecc108_sign_parameters {
   uint8_t *tx_buffer;        //!< pointer to send buffer
   uint8_t *rx_buffer;        //!< pointer to receive buffer
   uint8_t mode;              //!< what to include in the MAC calculation
   uint16_t key_id;           //!< what key to use for the MAC calculation
};

//////////////////////////////////////////////////////////////////////
// Verify command definitions
#define VERIFY_MODE_IDX                 SHA204_PARAM1_IDX      //!< Verify command index for zone
#define VERIFY_KEYID_IDX                SHA204_PARAM2_IDX      //!< Verify command index for key id
#define VERIFY_DATA_IDX                 SHA204_DATA_IDX        //!< GenKey command index for optional data
#define VERIFY_COUNT                    SHA204_CMD_SIZE_MIN    //!< Verify command packet size without "other data"
#define VERIFY_COUNT_DATA               (135)                  //!< Verify command packet size with "other data"
#define VERIFY_OTHER_DATA_SIZE          (170)                  //!< Verify command packet size with "other data"
#define VERIFY_MODE_External            ((uint8_t) 0x02)       //!< Verify zone id data
#define VERIFY_MODE_Stored              ((uint8_t) 0x00)       //!< Verify zone id data
//! Verify typical command delay
#define VERIFY_DELAY                    (80)
//! Verify maximum execution time
#define VERIFY_EXEC_MAX                 (100)
#define VERIFY_RSP_SIZE                 SHA204_RSP_SIZE_MIN    //!< response size of GenKey command

struct ecc108_verify_parameters {
	uint8_t *tx_buffer;        //!< pointer to send buffer
	uint8_t *rx_buffer;        //!< pointer to receive buffer
	uint8_t mode;              //!< what to include in the MAC calculation
	uint16_t key_id;           //!< what key to use for the MAC calculation
	uint8_t *R;                //!< pointer to 32, 36 bytes of data to be sent to client
	uint8_t *S;                //!< pointer to 32, 36 bytes of data to be sent to client
	uint8_t *X;                //!< pointer to 0, 32, 36 bytes of data to be sent to client
	uint8_t *Y;                //!< pointer to 0, 32, 36 bytes of data to be sent to client
	uint8_t *other_data;       //!< pointer to 0,19 bytes of data to be sent to client
};


struct ecc108_PrivWrite_parameters {
   uint8_t *tx_buffer;        //!< pointer to send buffer
   uint8_t *rx_buffer;        //!< pointer to receive buffer
   uint8_t zone;              //!< what to include in the MAC calculation
   uint16_t key_id;           //!< what key to use for the MAC calculation
   uint8_t *Value;            //!< pointer to 32,36 bytes of data to be sent to client
   uint8_t *MAC;              //!< pointer to 32,36 bytes of data to be sent to client
};





/**
 * \
 *
 * @{
 */
//uint8_t sha204m_check_mac(struct sha204_check_mac_parameters *args);
//uint8_t sha204m_dev_rev(struct sha204_dev_rev_parameters *args);
uint8_t ecc108m_gen_key(struct ecc108_gen_key_parameters *args);
//uint8_t sha204m_mac(struct sha204_mac_parameters *args);
uint8_t ecc108m_nonce(struct sha204_nonce_parameters *args);
//uint8_t sha204m_pause(struct sha204_pause_parameters *args);
uint8_t ecc108m_random(struct sha204_random_parameters *args);
//uint8_t sha204m_read(struct sha204_read_parameters *args);
//uint8_t sha204m_write(struct sha204_write_parameters *args);
uint8_t ecc108m_sign(struct ecc108_sign_parameters *args);
uint8_t ecc108m_verify(struct ecc108_verify_parameters *args);

uint8_t ecc108c_wakeup(uint8_t *response);
uint8_t ecc108p_sleep(void);
uint8_t ecc108m_mac(struct sha204_mac_parameters *args);
uint8_t ecc108h_nonce(struct sha204h_nonce_in_out *param);
uint8_t ecc108h_mac(struct sha204h_mac_in_out *param);
uint8_t ecc108m_read(struct sha204_read_parameters *param);
uint8_t ecc108m_write(struct sha204_write_parameters *param);
uint8_t ecc108m_lock(struct sha204_lock_parameters *args);
uint8_t ecc108m_PrivWrite(struct ecc108_PrivWrite_parameters *args);


uint8_t sha204m_execute(struct sha204_command_parameters *args);
//! @}


#endif /* ECC108_COMMANDS_H_ */