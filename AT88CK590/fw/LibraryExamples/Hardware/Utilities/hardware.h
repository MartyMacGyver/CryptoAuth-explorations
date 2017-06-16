/**
 * \file
 * \date Nov 20, 2009
 * \brief contains definitions for hardware dependent functions
 */
#include <avr/io.h>

#ifndef _HARDWARE_H
#define _HARDWARE_H

void Led_Init(void);
void Led_On(void);
void Led_Off(void);

void Led1(uint8_t state);
void Led2(uint8_t state);
void Led3(uint8_t state);


void Led_DisplayNumber(uint8_t number);
void Led_IndicateStatus(int8_t failure, uint8_t mode, uint8_t count);
void SwitchClientPower(uint8_t onOff);
void SwitchHostPower(uint8_t onOff);
uint8_t IsConfigurationRequested(void);

#endif
