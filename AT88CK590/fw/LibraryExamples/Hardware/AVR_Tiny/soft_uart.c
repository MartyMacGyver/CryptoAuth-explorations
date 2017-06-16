#include "soft_uart.h"

// You have to adjust BIT_CLOCK_COUNT and START_TIMER
// / STOP_TIMER for different baud rates / CPU clocks.
// The current values are for 9600 baud and CPU clock = 8 MHz.
#define BAUDRATE				(9600)
#if BAUDRATE==9600
#   define BIT_CLOCK_COUNT	(104)
#else
#   error You have to define a baud rate.
#endif

// rx
static   uint8_t srx_buffer[SRX_SIZE];
volatile uint8_t srx_in;
volatile uint8_t srx_out;
volatile uint8_t srx_data;
volatile uint8_t srx_state;

// tx
static   uint8_t stx_buffer[TRX_SIZE];
volatile uint8_t stx_in;
volatile uint8_t stx_out;
volatile uint8_t stx_state;
volatile uint8_t stx_data;
volatile uint8_t stx_finished;
volatile uint8_t stx_sending;


#ifdef __AVR_ATtiny85__

#   define	TX_OUT		      SBIT(TCCR1, COM1A0)
#   define  ENABLE_COMPARE    SBIT(TCCR1, CTC1) = 1
#   define  DISABLE_COMPARE   SBIT(TCCR1, CTC1) = 0
// 9600 baud
#   define  START_TIMER       SBIT(TCCR1, CS12) = 1
#   define  STOP_TIMER        SBIT(TCCR1, CS12) = 0


/** \brief This function initializes a software UART. */
void suart_init(void)
{
  ENABLE_COMPARE;

  // rx settings
  OCR1C = BIT_CLOCK_COUNT;					// compare value to reset TCNT
  OCR1A = BIT_CLOCK_COUNT / 2;			// interrupt on 0.5 bit time to sample bit

  srx_in = 1;
  srx_out = 0;									// never reach in pointer

  PCMSK = 1 << RXD_INT; 					// enable pin change interrupt for rx pin

  // tx settings
  //STX_DDR = 0; STX = 1;              // Enable pullup.
  STX_DDR = 1;									// Enable output.
  GTCCR = 1 << FOC1A;						// Force tx state.
  stx_state = stx_in = stx_out = 0;
  TIMSK = 1 << OCIE1A;

#   ifdef DEBUG_SUART
   DEBUG_DDR = 1;
#   endif

  GIMSK = 1 << PCIE;                   // enable pin-change interrupt
  sei();                					// enable interrupts
}


/** \brief This function reads bytes into a buffer.
 *         It blocks until \ref len number of bytes are read.
 *
 *  \param[in] len number of bytes to read
 *  \param[out] rx_buffer pointer to read buffer
 *  \return number of bytes not read, 0 = success
 */
uint8_t suart_receive_bytes(uint8_t len, uint8_t *rx_buffer)
{
	uint8_t idx;

	stx_sending = 0;

	// todo Add timeout loop counter.
	while (len--) {
		idx = srx_out + 1;
		if (idx >= SRX_SIZE)
		 idx = 0;

		while (idx == srx_in);	         // until at least one byte in

		*rx_buffer++ = srx_buffer[idx];	// get byte
		srx_out = idx;							// advance out pointer
	}
	return len;
}


/** \brief This function sends bytes.
 *         It blocks until \ref len number of bytes are sent.
 *  \param[in] len number of bytes to send
 *  \param[out] tx_buffer pointer to send buffer
 */
void suart_send_bytes(uint8_t len, uint8_t *tx_buffer)
{
   volatile uint8_t i;

   stx_sending = 1;

   // Switch to transmit.
   STX_DDR = 1;
   STX = TX_HIGH;
   // Give time for the following code to finish, up to while(i == stx_out),
   // before a compare interrupt can fire.
   TCNT1 = 80;
   OCR1A = 0;
   OCR1C = BIT_CLOCK_COUNT;					      // Trigger at every edge.

   // Set tx pin when triggered first time and start T1.
   SBIT(TCCR1, COM1A1) = 1;
   TX_OUT = TX_HIGH;
   START_TIMER;

   // todo Add timeout loop counter.
   while (len--) {
      i = stx_in + 1;
      if (i >= TRX_SIZE)
        i = 0;
      stx_buffer[stx_in] = ~(*tx_buffer++);	// complement for stop bit after data
      while (i == stx_out);				         // until at least one byte free
      												   // stx_out modified by interrupt!
      stx_in = i;
      stx_finished = 0;
   }

   // Wait for tx to finish.
   while (stx_finished == 0);

   // Switch to receive.
   // Sample after half bit.
   OCR1A = BIT_CLOCK_COUNT / 2;

   // Reset COM1A.
   SBIT(TCCR1, COM1A1) = 0;
   SBIT(TCCR1, COM1A0) = 0;

   stx_sending = 0;
}


/** \brief This function handles a pin-change interrupt.
 *  \param[in] PCINT0_vect interrupt vector
 */
ISR(PCINT0_vect)
{
  if (get_rxd() == 0) {					// if start bit
    TCNT1 = 0;								// interrupt after 0.5 bit time
    START_TIMER;
    PCMSK = 0;								// disable start detection
    srx_state = 10;
  }
}


/** \brief This function handles a timer1 compare A interrupt.
 *  \param[in] TIM1_COMPA_vect interrupt vector
 */
ISR(TIM1_COMPA_vect)
{
	uint8_t i;

#ifdef DEBUG_SUART
  DEBUG_OUT = 1;
#endif

  if (stx_sending) {
    // transmit
	  if (stx_state) {
		 stx_state--;
		 TX_OUT = TX_HIGH;      			// Set output mode for data bit.
		 // We invert the data first so that the stop bit is
       // automatically high. After having shifted stx_data
       // it is 0, not 1.
       if (stx_data & 1)        			// lsb first
			TX_OUT = TX_LOW;     			// Set output mode for data bit.
		 // The line below costs four more bytes than the three lines above.
		 //TX_OUT = !(stx_data & 1); // Set output mode.
		 stx_data >>= 1;
	  }
	  else if (stx_in != stx_out) {				// next byte to sent
		 stx_data = stx_buffer[stx_out];
       if (++stx_out >= TRX_SIZE)
         stx_out = 0;
		 stx_state = 9;
		 TX_OUT = TX_LOW;						// start bit
	  }

     else {
        stx_finished = 1;
     }
  }

  else switch (--srx_state) {
    // receive
    default:
   	i = srx_data >> 1;				// LSB first
		if (get_rxd() == 1)
		  i |= 0x80;						// data bit = 1
		srx_data = i;
		break;

    case 0:
   	PCMSK = 1 << RXD_INT;			// enable start detect
      STOP_TIMER;
  		if (get_rxd() == 1) {			// Stop bit valid
		  if (srx_in != srx_out) {		// no buffer overflow
		    srx_buffer[srx_in] = srx_data;
		    srx_in++;						// advance in pointer
		    if (srx_in >= SRX_SIZE)	// roll over
		      srx_in = 0;
		  }
		}
		break;

   case 9:
	  if (get_rxd() == 1) {				// no valid start bit
		  PCMSK = 1 << RXD_INT;			// enable start detect
        STOP_TIMER;
	  }
	  else
	    srx_buffer[srx_in] = 0;

	  break;
  }

#   ifdef DEBUG_SUART
  DEBUG_OUT = 0;
#   endif
}

#elif __AVR_ATtiny44__

#   define	TX_OUT		      SBIT(TCCR1A, COM1B0)
#   define  ENABLE_COMPARE    SBIT(TCCR1B, WGM12) = 1
#   define  DISABLE_COMPARE   SBIT(TCCR1B, WGM12) = 0
// 9600 baud
#   define  START_TIMER       SBIT(TCCR1B, CS11) = 1
#   define  STOP_TIMER        SBIT(TCCR1B, CS11) = 0


/** \brief This function initializes a software UART. */
void suart_init(void)
{
	ENABLE_COMPARE;

	// rx settings
	OCR1A = BIT_CLOCK_COUNT;				// Compare value to reset TCNT.
	OCR1B = BIT_CLOCK_COUNT / 2;			// Interrupt on 0.5 bit time to sample bit.

	srx_in = 1;
	srx_out = 0;								// Never reach in-pointer.

	PCMSK0 = 1 << RXD_INT; 					// Enable pin-change interrupt for rx pin.
	TIMSK1 = 1 << OCIE1B;               // Enable compare interrupt.

	// tx settings
	//STX_DDR = 0; STX = 1;               // Enable pullup.
	STX_DDR = 1;								// Enable output.
   SBIT(TCCR1A, COM1B1) = 1;           // Set tx state to "set".
	TCCR1C = 1 << FOC1B;						// Force tx state.
	stx_state = stx_in = stx_out = 0;

#   ifdef DEBUG_SUART
   DEBUG_DDR = 1;
#   endif

	GIMSK = 1 << PCIE0;                  // enable pin-change interrupt for pins 0 to 7 (port A)
	sei();                					 // enable interrupts
}


/** \brief This function reads bytes into a buffer.
 *         It blocks until \ref len number of bytes are read.
 *
 *  \param[in] len number of bytes to read
 *  \param[out] rx_buffer pointer to read buffer
 *  \return number of bytes not read, 0 = success
 */
uint8_t suart_receive_bytes(uint8_t len, uint8_t *rx_buffer)
{
	uint8_t idx;

	stx_sending = 0;

	// todo Add timeout loop counter.
	while (len--) {
		idx = srx_out + 1;
		if (idx >= SRX_SIZE)
			idx = 0;

		while (idx == srx_in);	         // until at least one byte in

		*rx_buffer++ = srx_buffer[idx];	// get byte
		srx_out = idx;							// advance out pointer
	}
	return len;
}


/** \brief This function sends bytes.
 *         It blocks until \ref len number of bytes are sent.
 *  \param[in] len number of bytes to send
 *  \param[out] tx_buffer pointer to send buffer
 */
void suart_send_bytes(uint8_t len, uint8_t *tx_buffer)
{
   volatile uint8_t i;

   stx_sending = 1;

   // Switch to transmit.
	PCMSK0 = 0;							// Disable start detection.
   STX = TX_HIGH;
   // Give time for the following code to finish, up to while(i == stx_out),
   // before a compare interrupt can fire.
   TCNT1 = BIT_CLOCK_COUNT / 2;

   OCR1B = 0;							// OCR1A resets TCNT, OCR1B triggers COMPB interrupt.
   // Set mode of tx pin when triggered first time and start T1.
   SBIT(TCCR1A, COM1B1) = 1;
   TX_OUT = TX_HIGH;
   START_TIMER;

   // todo Add timeout loop counter.
   while (len--) {
      i = stx_in + 1;
      if (i >= TRX_SIZE)
        i = 0;
      stx_buffer[stx_in] = ~(*tx_buffer++);	// complement for stop bit after data
      while (i == stx_out);				      // until at least one byte free
      												   // stx_out modified by interrupt!
      stx_in = i;
      stx_finished = 0;
   }

   // Wait for tx to finish.
   while (stx_finished == 0);

   // Switch to receive.
	OCR1B = BIT_CLOCK_COUNT / 2;			// Interrupt on 0.5 bit time to sample bit.
   // Reset COM1B.
   SBIT(TCCR1A, COM1B1) = 0;
   SBIT(TCCR1A, COM1B0) = 0;
	PCMSK0 = 1 << RXD_INT; 					// Enable pin-change interrupt for rx pin.

   stx_sending = 0;
}


/** \brief This function handles a pin-change interrupt.
 *  \param[in] PCINT0_vect interrupt vector
 */
ISR(PCINT0_vect)
{
   if (get_rxd() != 0)
	   return;

   // if start bit
	TCNT1 = 0;							// interrupt after 0.5 bit time
	START_TIMER;
	PCMSK0 = 0;							// disable start detection
	srx_state = 10;
}


/** \brief This function handles a timer1 compare B interrupt.
 *  \param[in] TIM1_COMPA_vect interrupt vector
 */
ISR(TIM1_COMPA_vect)
{
  if (stx_state) {
#   ifdef DEBUG_SUART
  DEBUG_OUT = 1;
#   endif

	 stx_state--;
	 TX_OUT = TX_HIGH;      			// Set output mode for data bit.
	 // We invert the data first so that the stop bit is
    // automatically high. After having shifted stx_data
    // it is 0, not 1.
    if (stx_data & 1)        			// lsb first
		TX_OUT = TX_LOW;     			// Set output mode for data bit.

	 stx_data >>= 1;
  }
  else if (stx_in != stx_out) {		// next byte to sent
#   ifdef DEBUG_SUART
  DEBUG_OUT = 1;
#   endif

	 stx_data = stx_buffer[stx_out];
    if (++stx_out >= TRX_SIZE)
      stx_out = 0;
	 stx_state = 9;
	 TX_OUT = TX_LOW;						// start bit
  }

  else {
     stx_finished = 1;
  }

#   ifdef DEBUG_SUART
  DEBUG_OUT = 0;
#   endif
}


/** \brief This function handles a timer1 compare B interrupt.
 *  \param[in] TIM1_COMPB_vect interrupt vector
 */
ISR(TIM1_COMPB_vect)
{
	uint8_t i;

#   ifdef DEBUG_SUART
	DEBUG_OUT = 1;
#   endif

	if (stx_sending) {
		// transmit
		if (stx_state) {
			stx_state--;
			TX_OUT = TX_HIGH;      			// Set output mode for data bit.
			// We invert the data first so that the stop bit is
			// automatically high. After having shifted stx_data
			// it is 0, not 1.
			if (stx_data & 1)        			// lsb first
				TX_OUT = TX_LOW;     			// Set output mode for data bit.

			stx_data >>= 1;
		}
		else if (stx_in != stx_out) {		// next byte to sent
#   ifdef DEBUG_SUART
		  DEBUG_OUT = 1;
#   endif
			stx_data = stx_buffer[stx_out];
			if (++stx_out >= TRX_SIZE)
			stx_out = 0;
			stx_state = 9;
			TX_OUT = TX_LOW;						// start bit
		}
		else
			stx_finished = 1;

#   ifdef DEBUG_SUART
		DEBUG_OUT = 0;
#   endif
	}

	else switch (--srx_state) {
   // receive
   default:
		i = srx_data >> 1;				// LSB first
		if (get_rxd() == 1)
			i |= 0x80;						// data bit = 1
		srx_data = i;
		break;

   case 0:
		PCMSK0 = 1 << RXD_INT;			// enable start detect
		STOP_TIMER;
		if (get_rxd() == 1) {			// Stop bit valid
			if (srx_in != srx_out) {	// no buffer overflow
				srx_buffer[srx_in] = srx_data;
				srx_in++;					// advance in-pointer
				if (srx_in >= SRX_SIZE)	// roll over
					srx_in = 0;
			}
		}
		break;

   case 9:
		if (get_rxd() == 1) {			// no valid start bit
			PCMSK0 = 1 << RXD_INT;		// enable start detect
			STOP_TIMER;
		}
		else
			srx_buffer[srx_in] = 0;

		break;
   }

#   ifdef DEBUG_SUART
	DEBUG_OUT = 0;
#   endif
}


#else
#   error This module does not support the target CPU.
#endif
