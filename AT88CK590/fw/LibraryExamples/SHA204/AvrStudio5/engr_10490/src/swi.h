/**
 * This module is the header file for swi.c.
 *
 * \date 8/5/2011 5:05:01 PM
 * \author gunter.fuchs
 */ 


#ifndef SWI_H_
#define SWI_H_

void set_wakeup_delay(uint8_t delay);
uint8_t get_wakeup_delay(void);
void set_pullup(uint8_t enable);
void set_timeout(uint8_t timeout);
uint8_t sha204_communicate(uint8_t *tx_buffer, uint8_t *rx_buffer, uint8_t rx_count);


#endif /* SWI_H_ */