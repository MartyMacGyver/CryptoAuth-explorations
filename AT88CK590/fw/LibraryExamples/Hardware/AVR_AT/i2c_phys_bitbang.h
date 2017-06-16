/** \file
 *  \brief definitions for bit-banged I2C
 *  \date July 17, 2013
 *  \author Atmel Crypto Group
 */

#ifndef _I2C_PHYS_BITBANG_H_
#   define _I2C_PHYS_BITBANG_H_

#include <stdint.h>
#include <avr/io.h>
#include "config.h"

#if TARGET_BOARD == NO_TARGET_BOARD
#   error You have to define a target board in config.h.
#endif

// pin and access definitions for I2C clock and data
// Make sure you include config.h which contains definitions for TARGET_BOARD.
// Otherwise the first entry below will be active because TARGET_BOARD will be 0
// and anything on the right side of the equality operator will also be 0.
// Such, the result of the if condition will be true.
#if TARGET_BOARD==AT88UBASE
#   define I2C_PIN_CLOCK   (0)        //!< bit position of port register for second device
#   define I2C_PIN_DATA    (1)        //!< bit position of port register for second device
#   define I2C_PORT_DDR    (DDRD)     //!< direction register for bit-banged I2C port
#   define I2C_PORT_OUT    (PORTD)    //!< output port register for bit-banged I2C port
#   define I2C_PORT_IN     (PIND)     //!< input port register for bit-banged I2C port
#elif TARGET_BOARD==AT88CK460 // Rhino White
#   define I2C_PIN_CLOCK   (1)        //!< bit position of port register for second device
#   define I2C_PIN_DATA    (2)        //!< bit position of port register for second device
#   define I2C_PORT_DDR    (DDRB)     //!< direction register for bit-banged I2C port
#   define I2C_PORT_OUT    (PORTB)    //!< output port register for bit-banged I2C port
#   define I2C_PORT_IN     (PINB)     //!< input port register for bit-banged I2C port
#endif

#define I2C_ENABLE();         {uint8_t pins = (1 << I2C_PIN_CLOCK) | (1 << I2C_PIN_DATA); \
								I2C_PORT_DDR |= pins; I2C_PORT_OUT |= pins;}
#define I2C_DISABLE()         I2C_PORT_DDR &= ~((1 << I2C_PIN_CLOCK) | (1 << I2C_PIN_DATA))
#define I2C_CLOCK_LOW()       I2C_PORT_OUT &= ~(1 << I2C_PIN_CLOCK)
#define I2C_CLOCK_HIGH()      I2C_PORT_OUT |=  (1 << I2C_PIN_CLOCK)
#define I2C_DATA_LOW()        I2C_PORT_OUT &= ~(1 << I2C_PIN_DATA)
#define I2C_DATA_HIGH()       I2C_PORT_OUT |=  (1 << I2C_PIN_DATA)
#define I2C_DATA_IN()         (I2C_PORT_IN &   (1 << I2C_PIN_DATA))
#define I2C_SET_OUTPUT()      I2C_PORT_DDR |=  (1 << I2C_PIN_DATA)
#define I2C_SET_OUTPUT_HIGH() I2C_SET_OUTPUT(); I2C_DATA_HIGH() 
#define I2C_SET_OUTPUT_LOW()  I2C_SET_OUTPUT(); I2C_DATA_LOW()
#define I2C_SET_INPUT()       I2C_PORT_DDR &= ~(1 << I2C_PIN_DATA)
#define DISABLE_INTERRUPT()   cli()
#define ENABLE_INTERRUPT()    sei()


// A delay of 1 generates a speed of about 320 kHz.
// A delay of 2 generates a speed of about 240 kHz.
// {volatile uint8_t delay = 1; while (delay--);}

// Use __asm__ to achieve speeds greater than 320 kHz.
// Two NOPs generate a speed of about 914 kHz, but to keep the duty cycle
// inside specification we have to use at least three NOPs.

// 400 kHz with an 8-bit AVR running at 16 MHz and compiled with -O1. Re-evaluate when changing the compiler or its settings.
#define I2C_CLOCK_DELAY_WRITE_LOW()    __asm__ volatile ("\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n")
#define I2C_CLOCK_DELAY_WRITE_HIGH()   __asm__ volatile ("\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n")
#define I2C_CLOCK_DELAY_READ_LOW()     __asm__ volatile ("\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n")
#define I2C_CLOCK_DELAY_READ_HIGH()    __asm__ volatile ("\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n")
// This delay plus I2C_CLOCK_HIGH and I2C_CLOCK_LOW takes about 1.3 us.
#define I2C_CLOCK_DELAY_SEND_ACK()     {volatile uint8_t delay = 1; while (delay--);}

// This delay is inserted to make the Start and Stop hold time at least 250 ns.
#define I2C_HOLD_DELAY()   __asm__ volatile ("\n\tnop\n\tnop\n")

//! loop count when waiting for an acknowledgment
#define I2C_ACK_TIMEOUT     (8)

// error codes for physical hardware dependent module
// Codes in the range 0x00 to 0xF7 are shared between physical interfaces (SWI, TWI, SPI).
// Codes in the range 0xF8 to 0xFF are special for the particular interface.
#define I2C_FUNCTION_RETCODE_SUCCESS     ((uint8_t) 0x00) //!< Communication with device succeeded.
#define I2C_FUNCTION_RETCODE_COMM_FAIL   ((uint8_t) 0xF0) //!< Communication with device failed.
#define I2C_FUNCTION_RETCODE_TIMEOUT     ((uint8_t) 0xF1) //!< Communication timed out.
#define I2C_FUNCTION_RETCODE_NACK        ((uint8_t) 0xF8) //!< TWI nack


void    i2c_enable(void);
void    i2c_disable(void);
uint8_t i2c_send_start(void);
uint8_t i2c_send_stop(void);
uint8_t i2c_send_bytes(uint8_t count, uint8_t *data);
uint8_t i2c_receive_byte(uint8_t *data);
uint8_t i2c_receive_bytes(uint8_t count, uint8_t *data);

#endif

