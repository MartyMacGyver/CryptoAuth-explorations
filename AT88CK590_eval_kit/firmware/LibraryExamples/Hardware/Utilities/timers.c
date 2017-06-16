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
 * \date December 13, 2012
*/

#include "timers.h"
#include <avr/wdt.h>

// start value for idle_counter: F_CPU / divisor / counter size / resolution (0.5 s)
#define IDLE_COUNTER_START   (uint8_t) ((float) F_CPU / (float) 1024 / (float) 256 / (float) 2) 

volatile uint8_t GenericTimerFlag = 0;        //Global flag for generic timer
volatile uint8_t IdleTimerFlag = 0;           //Global flag for SHA204 Idle timer
volatile uint8_t WDTFlag = 0;                 //Global flag for WatchDog Timer
volatile uint16_t counter = 0;
volatile uint8_t idle_counter = 0;
volatile uint8_t timer_delay_ms_expired = 0;
volatile uint8_t timer_delay_idle_expired = 0;


/** \brief Function to start Timer 1.
 *
 * \param[in] TimerFlagsA - setting for TCCR1A register.
 * \param[in] TimerFlagsB - setting for TCCR1B register.
 * \param[in] TimerCompare - OCR1A output compare value.
 */
void Timer_set(uint8_t TimerFlagsA, uint8_t TimerFlagsB, uint16_t TimerCompare)
{
  TCCR1A = TimerFlagsA;
  TCCR1B = TimerFlagsB;
  OCR1A = TimerCompare;
  Timer_reset;
  TIFR1 = _BV(OCF1A); // Clear interrupt flag.
}


/** \brief Function to stop Timer 1. */
void Timer_stop()
{
  TCCR1A = 0;
  TCCR1B = 0;
  OCR1A = 0x0FFF;
  Timer_reset;
  TIFR1 = _BV(OCF1A); // Clear interrupt flag.
}


/** \brief Initialization of timer for idling ATSHA204. */
void IdleTimer_Init(void)
{
	idle_counter = IDLE_COUNTER_START;
	TIMSK0 &= ~_BV(TOIE0);  //Disable TC0 overflow interrupt.
	TCCR0A = 0x00;          //Set mode to normal.
	TCCR0B = 0x05;          //Prescale the timer to be clock source / 1024.
	TIFR0 |= _BV(TOV0);     //Clear interrupt flag for TC0 overflow.
	TIMSK0 |= _BV(TOIE0);   //Enable TC0 overflow interrupt.
	timer_delay_idle_expired = 0;
}


/** \brief Disable IdleTimer. */
void IdleTimer_Stop(void)
{
	TIMSK0 &= ~_BV(TOIE0);  //Disable TC0 overflow interrupt.
	TCCR0B = 0x00;          //Disable clock source. Timer stops.
	TIFR0 |= _BV(TOV0);     //Clear interrupt flag for TC0 overflow.
	TIMSK0 |= _BV(TOIE0);   //Enable TC0 overflow interrupt.
}


/** \brief Timer 0 Overflow Interrupt Routine. */
ISR (TIMER0_OVF_vect)
{
	if (--idle_counter > 0)
	  return;

	timer_delay_idle_expired = 1;
	idle_counter = IDLE_COUNTER_START;
}


/** \brief Initialization of timer for general use. */
void GenericTimer_Init(void)
{
	TIMSK3 &= ~_BV(TOIE3);  //Disable TC3 overflow interrupt.
	TCCR3A = 0x00;          //Set two out of three mode bits to normal.
	TCCR3B = 0x02;          //Prescale the timer to be clock source / 8 to make it
                            //0.5 us for every timer step
	TCCR3C = 0x00;          //Set third mode bit to normal.
	TIFR3 |= _BV(TOV3);     //Clear overflow interrupt flag for TC3.
	TIMSK3 |= _BV(TOIE3);   //Enable TC3 overflow interrupt.
}


/** \brief Disable GenericTimer. */
void GenericTimer_Stop(void)
{
	TIMSK3 &= ~_BV(TOIE3);  //Disable TC3 overflow interrupt.
	TCCR3B = 0x00;          //Disable clock source. Timer stops.
	TIFR3 |= _BV(TOV3);     //Clear interrupt flag for TC3.
	TIMSK3 |= _BV(TOIE3);   //Enable TC3 overflow interrupt.
	counter = 0;
}


/** \brief Timer 3 Overflow Interrupt Routine. */
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
	TCCR3B = 0x00;          //Disable clock source. Timer stops.
	timer_delay_ms_expired = 1;
}


/** \brief Generate us delay.
 *
 *  \param delay - delay value in us
 */
void Timer_delay_us(uint16_t delay)
{
  TCNT3 = 0xFFFF - delay * TIMER_FACTOR;
  GenericTimer_Init();
  GenericTimerFlag = 0;           // Clear timer flag.
  while (!GenericTimerFlag);      // Wait for overflow.
  GenericTimer_Stop();
}


/** \brief Generate ms delay.
 *
 *  \param delay - delay value in ms
 */
void Timer_delay_ms(uint16_t delay)
{
  for (uint16_t i = 0; i < delay; i++)
    Timer_delay_us(1000);
}


/** \brief This function starts a timer and returns.
 * \param[in] delay number of ms until the #timer_delay_ms_expired flag is set.
 */
void Timer_delay_ms_without_blocking(uint16_t delay)
{
	TIMSK3 &= ~_BV(TOIE3);  //Disable TC3 overflow interrupt.
	counter = delay;
	timer_delay_ms_expired = 0;
	TCNT3 = 0xFFFF - TIMER_TICKS;//1 ms ticks
	GenericTimer_Init();
}


/** \brief Watchdog Timer Interrupt Service Routine. */
ISR (WDT_vect)
{
  WDTFlag = 1;
}


/** \brief Disable watchdog timer. */
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
