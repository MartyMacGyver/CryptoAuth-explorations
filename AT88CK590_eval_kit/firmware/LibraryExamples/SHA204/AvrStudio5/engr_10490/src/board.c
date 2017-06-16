/**
 * This module contains functions that use board features, i.e. LEDs.
 *
 * \date 8/2/2011 11:02:47 AM
 * \author gunter.fuchs
 */

#include "board.h"


/** \brief This function configures the GPIO pins for LED1 and LED2 as output.
 * 
 */
void led_init(void)
{
   ENGR_10490_LED_DDR |= (ENGR_10490_LED1 | ENGR_10490_LED2 | ENGR_10490_LED_POWER);	
}


/** \brief This function turns on an LED pattern.
 *  \param[in] led_pattern which LEDs to turn on
 */
void led_on(uint8_t led_pattern)
{
   ENGR_10490_LED_PORT |= led_pattern;	
}


/** \brief This function turns off an LED pattern.
 *  \param[in] led_pattern which LEDs to turn off
 */
void led_off(uint8_t led_pattern)
{
   ENGR_10490_LED_PORT &= ~led_pattern;	
}
