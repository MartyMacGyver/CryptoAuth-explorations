/*
 * sha204_example_write_read.h
 *
 * Created: 1/27/2013 4:56:57 PM
 *  Author: rocendob
 */ 


#ifndef SHA204_EXAMPLE_WRITE_READ_H_
#define SHA204_EXAMPLE_WRITE_READ_H_

uint8_t sha204_write_plain_text(uint8_t slotId, uint8_t *Buffer);
uint8_t sha204_read_plain_text(uint8_t slotId, uint8_t *RxBuffer);



#endif /* SHA204_EXAMPLE_WRITE_READ_H_ */