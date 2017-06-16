/*
 * radio.h
 *
 * Created: 3/21/2012 1:41:29 PM
 *  Author: rocendob
 */ 


#ifndef RADIO_H_
#define RADIO_H_

#define RADIO_RF231

typedef struct{
      unsigned int   fcf;           // Frame Control Field, see @ref FCF_DATA
      unsigned char  seq;           // Frame sequence number
      unsigned int   panid;         // The PAN ID for the network
      unsigned int   destAddr;      // Short address of receiving node
      unsigned int   srcAddr;       // Short address of sending node (next hop)
      unsigned char  payload[128];  // First byte of an array of application payload
} ftData;


void radio_set_channel(unsigned char channel);
unsigned char radio_get_channel(void);

void radio_set_panid(unsigned int panid);
unsigned int radio_get_panid(void);

void radio_set_address(unsigned int address); 
unsigned int radio_get_address(void);

void radio_set_remote_address(unsigned int remote_address);
unsigned int radio_get_remote_address(void);

void radio_send_data(unsigned char* tx_data, unsigned char len, unsigned int remote_address);

unsigned char radio_set_trx_state(unsigned char new_state);
void radio_ten_clock_delay(void);

void radio_init(void);

void enable_radio_pio_int(void);
void disable_radio_ext_int(void);

unsigned char hal_register_read(unsigned char address);
void hal_register_write(unsigned char address, unsigned char val);
unsigned char spi_read(void);
void spi_write(unsigned char val);
void hal_ss_high(void);
void hal_ss_low(void);
void hal_clr_slptr(void);
void hal_set_slptr(void);
void hal_clr_reset(void);
void hal_set_reset(void);


#endif /* RADIO.H_H_ */