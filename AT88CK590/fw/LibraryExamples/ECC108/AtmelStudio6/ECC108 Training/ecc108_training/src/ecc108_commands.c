/*
 * ecc108_commands.c
 *
 * Created: 3/25/2013 10:24:12 AM
 *  Author: james.tomasetta
 */ 
#include "ecc108_commands.h"
#include <asf.h>                       // definitions and declarations for the Atmel Software Framework
#include <string.h>                    // needed for memcpy()



uint8_t ecc108m_random(struct sha204_random_parameters *args)
{
	return sha204m_random(args);
}

uint8_t ecc108m_nonce(struct sha204_nonce_parameters *args)
{
	return sha204m_nonce(args);
}

uint8_t ecc108c_wakeup(uint8_t *response)
{
	return sha204c_wakeup(response);

}

uint8_t ecc108p_sleep(void)
{
	return sha204p_sleep();

}
uint8_t ecc108m_mac(struct sha204_mac_parameters *args)
{
	return sha204m_mac(args);

}
uint8_t ecc108h_nonce(struct sha204h_nonce_in_out *param)
{
	return sha204h_nonce(param);

}
uint8_t ecc108h_mac(struct sha204h_mac_in_out *param)
{
	return sha204h_mac(param);

}

uint8_t ecc108m_write(struct sha204_write_parameters *param)
{
	return sha204m_write(param);
}

uint8_t ecc108m_read(struct sha204_read_parameters *param)
{
	return sha204m_read(param);
}

uint8_t ecc108m_lock(struct sha204_lock_parameters *args)
{
	return sha204m_lock(args);

}






uint8_t ecc108m_gen_key(struct ecc108_gen_key_parameters *args)
{
	if (!args->tx_buffer || !args->rx_buffer)
	return SHA204_BAD_PARAM;

	if (args->key_id > SHA204_KEY_ID_MAX)
	return SHA204_BAD_PARAM;

	args->tx_buffer[SHA204_OPCODE_IDX] = ECC108_GENKEY;
	args->tx_buffer[GENKEY_ZONE_IDX] = args->mode;
	args->tx_buffer[GENKEY_KEYID_IDX] = args->key_id;
	args->tx_buffer[GENKEY_KEYID_IDX + 1] = 0;
		memcpy(&args->tx_buffer[GENKEY_DATA_IDX], args->other_data, GENKEY_OTHER_DATA_SIZE);
		args->tx_buffer[SHA204_COUNT_IDX] = GENKEY_COUNT_DATA;

	struct sha204_send_and_receive_parameters comm_parameters = {
		.tx_buffer = args->tx_buffer,
		.rx_buffer = args->rx_buffer,
		.rx_size = GENKEY_RSP_SIZE,
		.poll_delay = GENKEY_DELAY,
		.poll_timeout = GENKEY_EXEC_MAX - GENKEY_DELAY
	};
	return sha204c_send_and_receive(&comm_parameters);
}


uint8_t ecc108m_sign(struct ecc108_sign_parameters *args)
{
	if (!args->tx_buffer || !args->rx_buffer)
	return SHA204_BAD_PARAM;

	if (args->key_id > SIGN_KEYID_IDX)
	return SHA204_BAD_PARAM;

	args->tx_buffer[SHA204_OPCODE_IDX] = ECC108_SIGN;
	args->tx_buffer[SIGN_MODE_IDX] = args->mode;
	args->tx_buffer[SIGN_KEYID_IDX] = args->key_id;
	args->tx_buffer[SIGN_KEYID_IDX + 1] = 0;
	args->tx_buffer[SHA204_COUNT_IDX] = SIGN_COUNT;

	struct sha204_send_and_receive_parameters comm_parameters = {
		.tx_buffer = args->tx_buffer,
		.rx_buffer = args->rx_buffer,
		.rx_size = SIGN_RSP_SIZE,
		.poll_delay = SIGN_DELAY,
		.poll_timeout = SIGN_EXEC_MAX - SIGN_DELAY
	};
	return sha204c_send_and_receive(&comm_parameters);
}


uint8_t ecc108m_verify(struct ecc108_verify_parameters *args)
{
	if (!args->tx_buffer || !args->rx_buffer)
	return SHA204_BAD_PARAM;


	args->tx_buffer[SHA204_OPCODE_IDX] = ECC108_VERIFY;
	args->tx_buffer[VERIFY_MODE_IDX] = args->mode;
	args->tx_buffer[VERIFY_KEYID_IDX] = args->key_id;
	args->tx_buffer[VERIFY_KEYID_IDX + 1] = 0;
	if (args->other_data != NULL)
	{
		memcpy(&args->tx_buffer[VERIFY_DATA_IDX], args->other_data, VERIFY_OTHER_DATA_SIZE);
		args->tx_buffer[SHA204_COUNT_IDX] = VERIFY_COUNT_DATA;
	}
	else
	args->tx_buffer[SHA204_COUNT_IDX] = VERIFY_COUNT;

	struct sha204_send_and_receive_parameters comm_parameters = {
		.tx_buffer = args->tx_buffer,
		.rx_buffer = args->rx_buffer,
		.rx_size = VERIFY_RSP_SIZE,
		.poll_delay = VERIFY_DELAY,
		.poll_timeout = VERIFY_EXEC_MAX - VERIFY_DELAY
	};
	return sha204c_send_and_receive(&comm_parameters);
}


uint8_t ecc108m_PrivWrite(struct ecc108_PrivWrite_parameters *args)
{
	uint8_t *p_command;
	uint8_t count;
	uint16_t address;

	if (!args->tx_buffer || !args->rx_buffer || !args->Value || ((args->zone & ~WRITE_ZONE_MASK) != 0))
		return SHA204_BAD_PARAM;

	p_command = &args->tx_buffer[SHA204_OPCODE_IDX];
	*p_command++ = ECC108_PRIVWRITE;
	*p_command++ = args->zone;
	*p_command++ = args->key_id;
	*p_command++ = args->key_id >> 8;

	memcpy(p_command, args->Value, 36);
	p_command += 36;

	memcpy(p_command, args->MAC, 32);
	p_command += 32;

	// Supply count.
	args->tx_buffer[SHA204_COUNT_IDX] = (uint8_t) (p_command - &args->tx_buffer[0] + SHA204_CRC_SIZE);

	struct sha204_send_and_receive_parameters comm_parameters = {
		.tx_buffer = args->tx_buffer,
		.rx_buffer = args->rx_buffer,
		.rx_size = WRITE_RSP_SIZE,
		.poll_delay = WRITE_DELAY,
		.poll_timeout = WRITE_EXEC_MAX - WRITE_DELAY
	};
	return sha204c_send_and_receive(&comm_parameters);
}