/** \file
 * \date Oct 19, 2009
 * \brief contains board dependent functions (LEDs, switch, power)
 */

#include <avr\io.h>

#include "delay_x.h"
#include "hardware.h"
#include "Combined_Discover.h"   // device discovery functions

/** \brief pin assignments for the AT88CK109BK3 CryptoAuth daughter board on an AT88CK109STK3 port

 * Px0 (pin 01) - Vcc for host socket\n
 * Px1 (pin 02) - not connected\n
 * Px2 (pin 03) - not connected\n
 * Px3 (pin 04) - RXD for UART, tied over R11 to P7\n
 * Px4 (pin 05) - Vcc for client socket\n
 * Px5 (pin 06) - not connected\n
 * Px6 (pin 07) - signal host\n
 * Px7 (pin 08) - signal client, TXD for UART\n
 *     (pin 09) - Ground\n
 *     (pin 10) - Vcc / Diode\n
 *
*/


#define LED_PORT_DDR       (DDRD)
#define LED_PORT_OUT       (PORTD)
#define LED_PORT_IN        (PIND)
#define LED1               _BV(4)
#define LED2               _BV(5)
#define LED3               _BV(3)
#define LED1_ON            LED_PORT_OUT |= LED1
#define LED2_ON            LED_PORT_OUT |= LED2
#define LED3_ON            LED_PORT_OUT |= LED3
#define LED1_OFF           LED_PORT_OUT &= ~LED1
#define LED2_OFF           LED_PORT_OUT &= ~LED2
#define LED3_OFF           LED_PORT_OUT &= ~LED3
#define LEDS_OFF           LED_PORT_OUT &= ~(LED1 | LED2 | LED3)

#define HWB_PORT_DDR       (DDRE)
#define HWB_PORT_IN        (PINE)
#define HWB_PIN            _BV(2)

#define VCC_DDR            (DDRB)
#define VCC_OUT            (PORTB)
#define VCC_CLIENT         _BV(4)
#define VCC_HOST           _BV(0)
#define VCC_CLIENT_OFF     VCC_OUT &= ~VCC_CLIENT  //!< Vcc client off
#define VCC_CLIENT_ON      VCC_OUT |= VCC_CLIENT   //!< Vcc client on
#define VCC_HOST_OFF       VCC_OUT &= ~VCC_HOST    //!< Vcc host off
#define VCC_HOST_ON        VCC_OUT |= VCC_HOST     //!< Vcc host on


/** \brief Initializes port pins for LEDs and button.
 *
 */
void Led_Init() {
   // Configure port pins for LEDs as output.
   LED_PORT_DDR |= (LED1 | LED2 | LED3);

   // Configure port pin for button as input.
   HWB_PORT_DDR &= ~HWB_PIN;

   // Configure Vcc for client and host as output and switch on power.
   VCC_DDR |= (VCC_CLIENT | VCC_HOST);
   VCC_CLIENT_ON;
   VCC_HOST_ON;
}


/** \brief Switches all three LEDs on.
 */
void Led_On()
{	
	// Turns all LED's ON if not ECCSIGNER or ECCROOT
	#ifndef ECCROOT
	    #ifndef ECCSIGNER
	       LED1_ON; LED2_ON; LED3_ON;
		#endif
	#endif	   
}


/** \brief Switches all three LEDs off.
 */
void Led_Off()
{
   LED1_OFF; LED2_OFF; LED3_OFF;
}


/** \brief Configure LED1 state.
 * \param[in] state on or off
 */
void Led1(uint8_t state)
{
#ifndef ECCROOT	
   #ifndef ECCSIGNER
       if(state)
         LED1_ON;
       else
         LED1_OFF;
	#endif	 
#else
     LED1_OFF;	   // Always Disable if ECCROOT or ECCSIGNER is defined
#endif	 
}


/** \brief Configure LED2 state.
 * \param[in] state on or off
 */
void Led2(uint8_t state)
{
#ifndef ECCROOT	
   #ifndef ECCSIGNER
       if(state)
         LED2_ON;
       else
         LED2_OFF;
	#endif	 
#else
     LED2_OFF;	   // Always Disable if ECCROOT or ECCSIGNER is defined
#endif	    
}


/** \brief Configure LED3 state.
 * \param[in] state on or off
 */
void Led3(uint8_t state)
{
#ifndef ECCROOT	
   #ifndef ECCSIGNER
       if(state)
         LED3_ON;
       else
         LED3_OFF;
	#endif	 
#else
     LED1_OFF;	   // Always Disable if ECCROOT or ECCSIGNER is defined
#endif	    
}



/** \brief Uses the LEDs to display a number in octal.
 *
 * @param number number to display. Only three least significant bits are used.
 */
void Led_DisplayNumber(uint8_t number) {
   static const uint8_t pattern[] = {0, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70};

   number &= 7;
   LEDS_OFF;
   LED_PORT_OUT |= pattern[number];
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

   // The line below assumes that a failure value of zero indicates success.
   if (!failure) {
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


/** \brief Switches power to client socket on or off.
 *
 * @param[in] onOff Switches power on if true, otherwise off.
 */
void SwitchClientPower(uint8_t onOff)
{
   if (onOff)
      VCC_CLIENT_ON;
   else
      VCC_CLIENT_OFF;
}


/** \brief Switches power to host socket on or off.
 *
 * @param[in] onOff Switches power on if true, otherwise off.
 */
void SwitchHostPower(uint8_t onOff)
{
   if (onOff)
      VCC_HOST_ON;
   else
      VCC_HOST_OFF;
}
