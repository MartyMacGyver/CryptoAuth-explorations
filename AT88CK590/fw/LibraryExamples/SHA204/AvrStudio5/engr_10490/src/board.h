/**
 * header file for board functions module
 *
 * \date 8/2/2011 11:05:42 AM
 * \author gunter.fuchs
 */ 


#ifndef BOARD_H_
#define BOARD_H_


#include <avr/io.h>

//#define  ATMICROBASE
#ifdef ATMICROBASE
#   define ENGR_10490_LED_PORT    PORTD
#   define ENGR_10490_LED_DDR     DDRD
#   define ENGR_10490_LED_POWER   0
#   define ENGR_10490_BUZZER      0
#   define ENGR_10490_LED1        _BV(PD3)
#   define ENGR_10490_LED2        _BV(PD4)
#else
#   define ENGR_10490_LED_PORT    PORTA
#   define ENGR_10490_LED_DDR     DDRA
#   define ENGR_10490_LED_POWER   _BV(PA1)
#   define ENGR_10490_BUZZER      _BV(PA2)
#   define ENGR_10490_LED1        _BV(PA3)
#   define ENGR_10490_LED2        _BV(PA4)
#endif

#define LED_ERROR              ENGR_10490_LED1
#define LED_TWI                ENGR_10490_LED2


void led_init(void);
void led_on(uint8_t led_pattern);
void led_off(uint8_t led_pattern);


#endif /* BOARD_H_ */