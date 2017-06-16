/** \file
 * \date June 30, 2011
 * \author Atmel Crypto Products
 * \brief contains board dependent functions (LEDs, switch)
 */

#include <avr\io.h>


/** \brief Pin assignments for the AT88CK427 (Rhino Green) USB dongle

 * Px0 (pin 06) - not connected\n
 * Px1 (pin 01) - not connected\n
 * Px2 (pin 02) - not connected\n
 * Px3 (pin 03) - RXD for UART applications, tied over R11 to PB7\n
 * Px4 (pin 07) - Vcc for client socket\n
 * Px5 (pin 08) - not connected\n
 * Px6 (pin 05) - not connected\n
 * Px7 (pin 04) - TXD for UART, Signal Client\n
 *     (pin 09) - Ground\n
 *     (pin 10) - Vcc / Diode\n
*/

#define LED_PORT_DDR       (DDRD)
#define LED_PORT_OUT       (PORTD)
#define LED_PORT_IN        (PIND)
#define LED1               _BV(3)
#define LED2               _BV(4)

#define LED1_ON            LED_PORT_OUT |= LED1
#define LED2_ON            LED_PORT_OUT |= LED2

#define LED1_OFF           LED_PORT_OUT &= ~LED1
#define LED2_OFF           LED_PORT_OUT &= ~LED2

#define LEDS_OFF           LED_PORT_OUT |= (LED1 | LED2)


/** \brief Initializes port pins for LEDs and button.
 *
 */
void Led_Init() {
   // Configure port pins for LEDs as output.
   LED_PORT_DDR |= (LED1 | LED2);

}


/** \brief Switches three LEDs on.
 *
 */
void Led_On()
{
   LED1_ON; LED2_ON;
}


/** \brief Switches two LEDs off.
 *
 */
void Led_Off()
{
   LED1_OFF; LED2_OFF;;
}

/** \brief Configure LED1 state.
 * \param[in] state on or off
 */
void Led1(uint8_t state)
{
   if(state)
     LED1_ON;
   else
     LED1_OFF;
}


/** \brief Configure LED2 state.
 * \param[in] state on or off
 */
void Led2(uint8_t state)
{
   if(state)
     LED2_ON;
   else
     LED2_OFF;
}



