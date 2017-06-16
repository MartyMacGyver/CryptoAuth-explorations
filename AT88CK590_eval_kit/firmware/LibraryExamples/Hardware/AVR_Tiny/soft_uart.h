#ifndef _suart_h_
#   define _suart_h_


#include <avr/io.h>
#include <avr/interrupt.h>
#include "type_definitions.h"


#define XTAL			F_CPU
//#define DEBUG_SUART

#ifdef __AVR_ATtiny85__
// rx
#   define RXD				SBIT(PORTB, 3)
#   define RXD_DDR		SBIT(DDRB, 3)
#   define RXD_PIN		SBIT(PINB, 3)
#   define RXD_INT		PCINT3

// tx
#   define STX				SBIT(PORTB, 1)
#   define STX_DDR		SBIT(DDRB, 1)	// must be OC1A !

// debug
#   define DEBUG_OUT		SBIT(PORTB, 4)
#   define DEBUG_DDR		SBIT(DDRB, 4)

#elif __AVR_ATtiny44__
// rx
#   define RXD				SBIT(PORTA, 7)
#   define RXD_DDR		SBIT(DDRA, 7)
#   define RXD_PIN		SBIT(PINA, 7)
#   define RXD_INT		PCINT7

// tx
#   define STX				SBIT(PORTA, 5)
#   define STX_DDR		SBIT(DDRA, 5)	// must be OC1A/B !

// debug
#   define DEBUG_OUT		SBIT(PORTA, 0)
#   define DEBUG_DDR		SBIT(DDRA, 0)

#else
#   error This driver does not support the selected target.
#endif

// todo Reuse SHA204 communication buffers.
// rx buffer size = count (1 byte) + challenge (32 bytes) + digest (32 bytes) + crc (2 bytes)
#define SRX_SIZE		67
// return size of get version command
#define TRX_SIZE		6


//#define	INVERT_SIGNAL			// RS-232 level converter
#ifdef INVERT_SIGNAL
#   define get_rxd() (RXD_PIN == 0)
#   define	TX_HIGH	(0)
#else
#   define get_rxd() (RXD_PIN == 1)
#   define	TX_HIGH	(1)
#endif
#define	TX_LOW		!TX_HIGH


void suart_init(void);
void suart_send_bytes(uint8_t len, uint8_t *tx_buffer);
uint8_t suart_receive_bytes(uint8_t len, uint8_t *rx_buffer);


#endif
