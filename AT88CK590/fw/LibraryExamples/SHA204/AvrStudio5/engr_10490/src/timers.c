// ----------------------------------------------------------------------------
//         ATMEL Crypto_Devices Software Support  -  Colorado Springs, CO -
// ----------------------------------------------------------------------------
// DISCLAIMER:  THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
// DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
// OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// ----------------------------------------------------------------------------

/** \file
 * Timer module for AVR 8-bit micro-controllers.
 * \author Atmel Crypto Products
 * \date August 22, 2011
*/

#include "timers.h"
#include <avr/wdt.h>

volatile uint8_t GenericTimerFlag = 0;        //Global flag
volatile uint8_t WDTFlag = 0;                 //Global flag for WatchDog Timer
volatile uint16_t counter = 0;
volatile uint8_t timer_delay_ms_expired;

void GenericTimer_Init(void);
void GenericTimer_Stop(void);

/** \brief Function to start Timer 1.
 *
 * \param[in] ucTimerFlagsA - setting for TCCR1A register.
 * \param[in] ucTimerFlagsB - setting for TCCR1B register.
 * \param[in] uiTimerCompare - OCR1A output compare value.
 *  \todo Remove timer1 functions.
 */
void Timer_set(uint8_t ucTimerFlagsA, uint8_t ucTimerFlagsB, uint16_t uiTimerCompare)
{
  TCCR1A = ucTimerFlagsA;
  TCCR1B = ucTimerFlagsB;
  OCR1A = uiTimerCompare;
  Timer_reset;
  TIFR1 = 0x02; // clear flag for AT90USB647
}


/** \brief Function to stop Timer 1.
 */
void Timer_stop()
{
  TCCR1A = 0;
  TCCR1B = 0;
  OCR1A = 0x0FFF;
  Timer_reset;
  TIFR1 = 0x02; // clear flag for AT90USB647
}


/** \brief Initialization of timer for general use.
*/
void GenericTimer_Init(void)
{
	TIMSK3 &= ~_BV(ICIE3);  //Disable TC3 interrupts
	TCCR3A = 0x00;          //Normal mode
	TCCR3B = 0x02;          //Prescale the timer to be clock source / 8 to make it
                            //0.5 us for every timer step
	TCCR3C = 0x00;          //Normal mode
	TIFR3 |= _BV(ICF3);     //Clear interrupt flags for TC3
	TIMSK3 |= _BV(TOIE3);   //Set Timer/Counter3 Overflow Interrupt Enable
}


/** \brief Shut down GenericTimer.
 */
void GenericTimer_Stop(void)
{
	TIMSK3 &= ~_BV(ICIE3);//Disable TC3 interrupts
    counter = 0;
	TCCR3A = 0x00;
	TCCR3B = 0x00;        //No clock source / timer stopped
	TCCR3C = 0x00;
	TIFR3 |= _BV(ICF3);   //Clear interrupt flags for TC3
	TIMSK3 |= _BV(ICIE3); //Enable TC3 interrupts
}


/** \brief Timer 3 Overflow Interrupt Routine.
 */
ISR (TIMER3_OVF_vect)
{
	if (counter == 0) {
	  GenericTimerFlag = 2;           //Set TC3 Flag = 2 (overflow)
	  return;
	}
	// A "counter" value greater than zero indicates that the
	// non-blocking version of the Timer_delay_ms has been started.
	if (--counter > 0) {
	  TCNT3 = 0xFFFF - TIMER_TICKS;//1 ms ticks
	  return;
	}
	TCCR3A = 0x00;
	TCCR3B = 0x00;        //No clock source / timer stopped
	TCCR3C = 0x00;
	timer_delay_ms_expired = 1;
}


/** \brief Generate us delay.
 *
 *  \param uiDelay - delay value in us
 */
void Timer_delay_us(uint16_t uiDelay)
{
  TCNT3 = 0xFFFF - uiDelay * TIMER_FACTOR;
  GenericTimer_Init();
  GenericTimerFlag = 0;           // Clear Timer Flag
  while (!GenericTimerFlag);      // Wait for overflow
  GenericTimer_Stop();
}


/** \brief Generate ms delay.
 *
 *  \param[in] delay - delay value in ms
 */
void Timer_delay_ms(uint16_t delay)
{
	for (uint16_t i = 0; i < delay; i++)
		Timer_delay_us(1000);
//	Timer_delay_ms_without_blocking(delay);
//	while(!timer_delay_ms_expired);
}


/** \brief This function starts a timer and returns.
 * \param[in] delay number of ms until the #timer_delay_ms_expired flag is set.
 */
void Timer_delay_ms_without_blocking(uint16_t delay)
{
	TIMSK3 &= ~_BV(ICIE3);//Disable TC3 interrupts
	counter = delay;
	timer_delay_ms_expired = 0;
	TCNT3 = 0xFFFF - TIMER_TICKS;//1 ms ticks
	GenericTimer_Init();
}


/** \brief Watchdog Timer Interrupt Service Routine.
 */
ISR (WDT_vect)
{
  WDTFlag = 1;
}


/** \brief Disable watchdog timer.
 */
void WDT_Off(void)
{
  wdt_reset();
  wdt_disable();
}


/** \brief Start watchdog timer.
 */
void WDT_Start(void)
{
  WDTFlag = 0;
  wdt_intr_enable(WDTO_1S);        //Set Watchdog Timer to 1 second
}
