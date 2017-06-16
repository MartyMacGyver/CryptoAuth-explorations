#ifndef TWI_BITBANG_H
#define TWI_BITBANG_H

#include <asf.h>

#define I2C_PORT                            IOPORT_PORTD
#define I2C_PIN_SCL                         0
#define I2C_PIN_SDA							1
#define I2C_PIN_GROUP                       (1 << I2C_PIN_SCL) | (1 << I2C_PIN_SDA)
#define I2C_PIN_OUTPUT_CONFIG               IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH | IOPORT_WIRED_AND_PULL_UP
#define I2C_PIN_INPUT_CONFIG                IOPORT_DIR_INPUT | IOPORT_PULL_UP
#define I2C_SCL_GPIO                        IOPORT_CREATE_PIN(PORTD, I2C_PIN_SCL)
#define I2C_SDA_GPIO                        IOPORT_CREATE_PIN(PORTD, I2C_PIN_SDA)
#define I2C_CLOCK_DELAY                     sha204h_delay_us(4)


void i2c_enable(void);
void i2c_pulse_sda_low(uint16_t duration);
void i2c_send_start(void);
void i2c_send_stop(void);
uint8_t i2c_receive_byte(bool ack);
void i2c_receive_bytes(uint8_t count, uint8_t *data);
uint8_t i2c_send_bytes(uint8_t count, uint8_t *data);

#endif
   
