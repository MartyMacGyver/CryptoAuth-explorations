/** \file
 * \date Oct 19, 2009
 * \author G&uuml;nter Fuchs
 * \brief contains board dependent functions (LEDs, switch)
 */

#include <avr\io.h>
#include "delay_x.h"
#include "SA_Status.h"


/** \brief Pin assignments for the STK600 with an AT88CK101 CryptoAuth Daughter Board

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
#define LED1               _BV(0)
#define LED2               _BV(1)
#define LED3               _BV(2)
#define LED1_OFF           LED_PORT_OUT |= LED1
#define LED2_OFF           LED_PORT_OUT |= LED2
#define LED3_OFF           LED_PORT_OUT |= LED3
#define LED1_ON            LED_PORT_OUT &= ~LED1
#define LED2_ON            LED_PORT_OUT &= ~LED2
#define LED3_ON            LED_PORT_OUT &= ~LED3
#define LEDS_OFF           LED_PORT_OUT |= (LED1 | LED2 | LED3)

#define HWB_PORT_DDR       (DDRC)
#define HWB_PORT_IN        (PINC)
#define HWB_PIN            _BV(2)


/** \brief Initializes port pins for LEDs and button.
 *
 */
void Led_Init() {
   // Configure port pins for LEDs as output.
   LED_PORT_DDR |= (LED1 | LED2 | LED3);

   // Configure port pin for button as input.
   HWB_PORT_DDR &= ~HWB_PIN;
}


/** \brief Switches three LEDs on.
 *
 */
void Led_On()
{
   LED1_ON; LED2_ON; LED3_ON;
}


/** \brief Switches three LEDs off.
 *
 */
void Led_Off()
{
   LED1_OFF; LED2_OFF; LED3_OFF;
}


/** \brief Uses the LEDs to display a number in octal.
 *
 * @param number number to display. Only three least significant bits are used.
 */
void Led_DisplayNumber(uint8_t number) {
   static const uint8_t pattern[] = {0xFF, 0xFE, 0xFD, 0xFC, 0xFB, 0xFA, 0xF9, 0xF8};

   number &= 7;
   LEDS_OFF;
   LED_PORT_OUT &= pattern[number];
}


/** \brief Indicates test status by blinking LEDs.
 *
 * @param failure if 0 indicate success
 * @param mode if failure blink with this pattern
 * @param duration blink that many seconds and return. If 0 do not return.
 */
void Led_IndicateStatus(int8_t failure, uint8_t mode, uint8_t duration)
{
   uint8_t i = 1, count = 0;
   uint8_t stayHere = !duration;

   if (failure == SA_SUCCESS) {
      count = duration * 2;
      while (1) {
         Led_DisplayNumber(i);
         _delay_ms(500);
         if (!stayHere)
            if (!count--)
               break;
         i <<= 1;
         if (i > 4)
            i = 1;
      }
   }
   else {
      count = duration;
      while (1) {
         Led_DisplayNumber(mode ? mode >> 4 : 7);
         _delay_ms(500);
         Led_DisplayNumber(0);
         _delay_ms(500);
         if (!stayHere)
            if (!count--)
               break;
      }
   }
   Led_DisplayNumber(0);
   _delay_ms(1000);
   return;
}


/** \brief Checks button press that indicates configuration request.
 *
 * \return whether button is pressed
 */
uint8_t IsConfigurationRequested()
{
   return !(HWB_PORT_IN & HWB_PIN);
}


/** \brief Not used on this daughter board.
 *
 * @param[in] onOff Switches power on if true, otherwise off.
 */
void SwitchClientPower(uint8_t onOff)
{
}


/** \brief Not used on this daughter board.
 *
 * @param[in] onOff Switches power on if true, otherwise off.
 */
void SwitchHostPower(uint8_t onOff)
{
}
