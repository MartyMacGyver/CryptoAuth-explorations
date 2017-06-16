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
 *  \brief Header file for timers.c.
 *  \author Atmel Crypto Products
 *  \date December 13, 2012
 */

#ifndef _TIMER_H_
#define _TIMER_H_

#include <avr/interrupt.h>

#if F_CPU == 16000000
#   define TIMER_TICKS  (2000)
#   define TIMER_FACTOR (2)
#elif F_CPU == 8000000
#   define TIMER_TICKS  (1000)
#   define TIMER_FACTOR (1)
#endif

/** \brief Definition of macro to check Output Compare Match A Interrupt flag for AT90USBx. */
 #define Timer_test  (TIFR1 & 0x02)

/** \brief Definition of macro to reset Timer/Counter register for AT90USBx. */
 #define Timer_reset  TCNT1 = 0

extern volatile uint8_t GenericTimerFlag;
extern volatile uint8_t WDTFlag;
extern volatile uint8_t timer_delay_ms_expired;
extern volatile uint8_t timer_delay_idle_expired;

void Timer_set(uint8_t ucTimerFlagsA, uint8_t ucTimerFlagsB, uint16_t uiTimerCompare);
void Timer_stop(void);
void IdleTimer_Init(void);
void IdleTimer_Stop(void);
void GenericTimer_Init(void);
void GenericTimer_Stop(void);
void Timer_delay_us(uint16_t uiDelay);
void Timer_delay_ms(uint16_t uiDelay);
void Timer_delay_ms_without_blocking(uint16_t delay);
void WDT_Off(void);
void WDT_Start(void);

#define wdt_intr_enable(value)   \
    __asm__ __volatile__ (  \
        "in __tmp_reg__,__SREG__" "\n\t"    \
        "cli" "\n\t"    \
        "wdr" "\n\t"    \
        "sts %0,%1" "\n\t"  \
        "out __SREG__,__tmp_reg__" "\n\t"   \
        "sts %0,%2" \
        : /* no outputs */  \
        : "M" (_SFR_MEM_ADDR(_WD_CONTROL_REG)), \
        "r" (_BV(_WD_CHANGE_BIT) | _BV(WDE)), \
        "r" ((uint8_t) ((value & 0x08 ? _WD_PS3_MASK : 0x00) | \
            _BV(WDIE) | (value & 0x07)) ) \
        : "r0"  \
    )

#endif // _TIMER_H_

