// ----------------------------------------------------------------------------
//         ATMEL Microcontroller Software Support  -  Colorado Springs, CO -
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
// File Name           : AVR_Javan_usb.c
// Object              : main application written in C
// Creation            : SEM	2/3/2009
// ----------------------------------------------------------------------------

//_____  I N C L U D E S ___________________________________________________

#include "conf/config.h"
#include "common/modules/scheduler/scheduler.h"
#include "common/lib_mcu/wdt/wdt_drv.h"
#include "lib_mcu/power/power_drv.h"
#include "lib_mcu/util/start_boot.h"
#include "SA_API.h"
#include "hardware.h"
#include <util/delay.h>
//_____ M A C R O S ________________________________________________________

//_____ D E F I N I T I O N S ______________________________________________



/*
Change the following size:
				#define DATA_TRANSFER_SIZE 0x08

to effectively change
			- The report IN and report OUT lengths are defined in the file usb_descriptor.h:

									#define LENGTH_OF_REPORT_IN 8
									#define LENGTH_OF_REPORT_OUT 8

									#define EP_IN_LENGTH 8
									#define EP_OUT_LENGTH 8

**** Also Change

				- And the SIZE_n parameters from the file usb_specific_request.c

							void usb_user_endpoint_init(U8 conf_nb)
							{
							   usb_configure_endpoint( EP_HID_IN,   \
							                           TYPE_INTERRUPT,\
							                           DIRECTION_IN,  \
							                           SIZE_128,        \
							                           ONE_BANK,      \
							                           NYET_ENABLED);

							   usb_configure_endpoint( EP_HID_OUT,    \
							                           TYPE_INTERRUPT,\
							                           DIRECTION_OUT, \
							                           SIZE_128,        \
							                           ONE_BANK,      \
							                           NYET_ENABLED);
							}
*/


int main(void)
{
   U8 i;
   start_boot_if_required();
   wdtdrv_disable();
   //Clear_prescaler();
   clock_prescale_set(0);


	Led_Init();

  if (TARGET_BOARD == JAVAN_PLUS) {
     DDRB  &=~_BV(4);		//	set PORTB pin4 to zero as input
     PORTB |=_BV(4);	   	//	enable pull up
   }

   for (i = 0; i < 3; i++)
   {
	   Led_On();
      _delay_ms(200);
	   Led_Off();
	   _delay_ms(200);
	}


//POWER LED
	#ifdef AT88CK427
		Led1(1);
	#endif


   SA_Init(0);
   scheduler();
   return 0;
}


//! \name Procedure to speed up the startup code
//! This one increment the CPU clock before RAM initialisation
//! @{
#ifdef  __GNUC__
// Locate low level init function before RAM init (init3 section)
// and remove std prologue/epilogue
char __low_level_init(void) __attribute__ ((section (".init3"), naked));
#endif

#ifdef __cplusplus
extern "C" {
#endif
char __low_level_init()
{
  //Clear_prescaler();
  clock_prescale_set(0);
  return 1;
}
#ifdef __cplusplus
}
#endif
//! @}




