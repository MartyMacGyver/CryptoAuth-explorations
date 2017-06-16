


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <asf.h>
#include <avr/interrupt.h>
#include "radio.h" 
#include "gpio.h"
#include "util/delay.h"

volatile unsigned char 	RadioPartNum = 0;

#define LIGHT_ADDR      0xFFFF // Broadcast
#define SWITCH_ADDR     0x0000
#define PANID           0xFEA0
#define CHANNEL_RADIO   25


#ifdef RADIO_RF231
#include "rf231.h"
#else
#include "rf230.h"
#endif


#define RSTPIN    IOPORT_CREATE_PIN(PORTD,0)	// RST
#define IRQPIN    IOPORT_CREATE_PIN(PORTD,2)	// IRQ
#define SLPTRPIN  IOPORT_CREATE_PIN(PORTD,3)	// SLP_TR
#define SSPIN     IOPORT_CREATE_PIN(PORTD,4)	// SEL
#define MOSIPIN   IOPORT_CREATE_PIN(PORTD,5)	// MOSI
#define MISOPIN   IOPORT_CREATE_PIN(PORTR,0)	// MISO
#define SCKPIN    IOPORT_CREATE_PIN(PORTR,1)	// SCLK

#define hal_set_slptr_high() gpio_set_pin_high(SLPTRPIN)
#define hal_set_slptr_low() gpio_set_pin_low(SLPTRPIN)

volatile unsigned char RadioPLLLock = 0;
unsigned int  remote_node_address = 0;
unsigned char seq = 0; 
unsigned int  short_address = 0;
unsigned char TxMode = 0;
unsigned char NewMessage = 0;
unsigned char PllUnlock = 0;

void radio_set_channel(unsigned char ch)
{
  unsigned char temp = 0;
  temp = (hal_register_read(PHY_CC_CCA) & ~(CHANNEL_MASK)); //clear channel
  temp = temp | ch;
  hal_register_write(PHY_CC_CCA, temp);
}

unsigned char radio_get_channel(void)
{
  return (hal_register_read(PHY_CC_CCA) & CHANNEL_MASK);
}

void radio_set_panid(unsigned int local_panid)
{
  hal_register_write(PAN_ID_0, (unsigned char)local_panid);
  hal_register_write(PAN_ID_1, (unsigned char)(local_panid>>8));
}

unsigned int radio_get_panid(void)
{
  unsigned int temp = 0;
  
  temp = hal_register_read(PAN_ID_1);
  temp<<=8;
  temp |= hal_register_read(PAN_ID_0);

  return temp;

}

void radio_set_address(unsigned int address)
{
  hal_register_write(SHORT_ADDR_0, (unsigned char)address);
  hal_register_write(SHORT_ADDR_1, (unsigned char)(address>>8));
  short_address = address;
} 

unsigned int radio_get_address(void)
{
  unsigned int temp = 0;
  temp = hal_register_read(SHORT_ADDR_1);
  temp<<=8;
  temp |= hal_register_read(SHORT_ADDR_0);

  return temp;
}

void radio_set_remote_address(unsigned int remote_address)
{
  remote_node_address = remote_address;
}

unsigned int radio_get_remote_address(void)
{
  return remote_node_address;
}


void radio_send_data(unsigned char* tx_data, unsigned char len, unsigned int remote_address)
{
  unsigned int i;	
  unsigned char length = 0;
  ftData  framedata;
  unsigned char * pFrameData;
  
  pFrameData = &framedata;
    
  TxMode = 1;	
  
  disable_radio_ext_int();
	
  // build FCF (Frame Control Field)
  //framedata.fcf = 0x9861; // Short Address, no security, intra-pan, ack-required, no frame pending, data frame. 
  // 1001 1000 0110 0001
  
  framedata.fcf = 0x8841; // Short Address, no security, intra-pan, no-ack-required, no frame pending, data frame. 
                         
  // build Sequence Number (incrementing a global)
  framedata.seq = seq++;
  
  // Use the common global pan id
  framedata.panid = radio_get_panid();
  
  // Destination address
  framedata.destAddr = remote_address;
  
  // Source address
  framedata.srcAddr = radio_get_address();
  
  // build Auxilliary Security Header - not present.
  
  // Now, put the Radio in TX Mode Auto-retry
  while(!(radio_set_trx_state(TX_ARET_ON)));   
  
  //Toggle the SLP_TR, initiate the frame transmission.
  hal_clr_slptr();
 
  hal_set_slptr();
  for(i=0; i<10; i++) {
      asm volatile("nop");
    } 
 
  hal_clr_slptr();
    
  // build Frame Payload
  memcpy(framedata.payload, tx_data, len); 
  
  // The FCS will be built by the radio itself this is default on and set on
  // in main. 

  // Have to add 9 for the preceeding bytes and then 2 at the end as well
  // for the FCS
  length = len + 11;
 
  hal_ss_low(); //Start the SPI transaction by pulling the Slave Select low.

  // SEND FRAME WRITE COMMAND AND FRAME LENGTH.
  spi_write(HAL_TRX_CMD_FW);
  spi_write(length);

  //Download to the Frame Buffer.
    do
    {
        spi_write(*pFrameData++);
        --length;
    } while (length > 0);

   hal_ss_high(); //End the transaction by pulling the Slave Select High.
   
  //wait for TX done
  while((hal_register_read(TRX_STATUS) & TRX_STATUS_MASK) != TX_ARET_ON); 	
	
  // Put Radio Back RX Mode auto-ack
  while(!(radio_set_trx_state(RX_AACK_ON)));  
  
  enable_radio_pio_int();
 
}


// retrieve Frame when using RX_START for critical timing
void frame_read_RX_START(char *rx_data, unsigned char *len, unsigned char *lqi)
{
    unsigned char frame_length;

    hal_ss_low(); //Start the SPI transaction by pulling the Slave Select low.

    // Send frame read command.
    spi_write(HAL_TRX_CMD_FR);

    // Read frame length.
    frame_length = spi_read();

    // Check for correct frame length.
    if ((frame_length >= 3) && (frame_length <= 127))
    {
        *len = frame_length; //Store frame length.
        do
        {   
			while(gpio_pin_is_high(IRQPIN)); // only read OTA data while IRQ is low
              *rx_data++ = spi_read();  // read 1 byte from OTA frame buffer 
        } while (--frame_length > 0);

        // Read LQI value for this frame.
		while(gpio_pin_is_high(IRQPIN)); // only read OTA data while IRQ is low
        *lqi = spi_read();

        hal_ss_high(); //End the transaction by pulling the Slave Select High.
    }
}


// retrieve Frame when using TRX_END 
void frame_read_TRX_END(char *rx_data, unsigned char *len, unsigned char *lqi)
{
    unsigned char frame_length;

    hal_ss_low(); //Start the SPI transaction by pulling the Slave Select low.

    // Send frame read command.
    spi_write(HAL_TRX_CMD_FR);

    // Read frame length.
    frame_length = spi_read();

    // Check for correct frame length.
    if ((frame_length >= 3) && (frame_length <= 127))
    {
        *len = frame_length; //Store frame length.
        do
        {
            *rx_data++ = spi_read(); 
        } while (--frame_length > 0);

        // Read LQI value for this frame.
        *lqi = spi_read();

        hal_ss_high(); //End the transaction by pulling the Slave Select High.
    }

}


unsigned char radio_set_trx_state(unsigned char new_state)
{
   unsigned char original_state;
   unsigned char temp = 0;
   
   original_state = (hal_register_read(TRX_STATUS) & TRX_STATUS_MASK);
  
    if (new_state == original_state) { return 1; }
                        
    if( new_state == TRX_TRX_OFF )
    {
        //Go to TRX_TRX_OFF from any state.		
		hal_register_write(TRX_STATE, CMD_FORCE_TRX_OFF ); 
		while((hal_register_read(TRX_STATUS) & TRX_STATUS_MASK) != TRX_TRX_OFF);
    } 
    else 
    {  
        //It is not allowed to go from RX_AACK_ON or TX_AACK_ON and directly to
        //TX_AACK_ON or RX_AACK_ON respectively. Need to go via RX_ON or PLL_ON.
        if ((new_state == TX_ARET_ON) && (original_state == RX_AACK_ON)) 
        {           
            //First do intermediate state transition to PLL_ON, then to TX_ARET_ON.
            //The final state transition to TX_ARET_ON 
		   temp = hal_register_read(TRX_STATE) & ~(TRX_CMD_MASK);
           temp = temp | CMD_PLL_ON;		   
           hal_register_write(TRX_STATE, temp);
		   while((hal_register_read(TRX_STATUS) & TRX_STATUS_MASK) == TRANSITION_IN_PROGRESS);
		   
		   // Now Go to TX_ARET_ON
		   temp = hal_register_read(TRX_STATE) & ~(TRX_CMD_MASK);
           temp = temp | CMD_TX_ARET_ON;		   
           hal_register_write(TRX_STATE, temp);
		   while((hal_register_read(TRX_STATUS) & TRX_STATUS_MASK) == TRANSITION_IN_PROGRESS);
		   
     
        } 
        else if ((new_state == RX_AACK_ON) && (original_state == TX_ARET_ON)) 
        {
            //First do intermediate state transition to RX_ON, then to RX_AACK_ON.
            //The final state transition to RX_AACK_ON
		   temp = hal_register_read(TRX_STATE) & ~(TRX_CMD_MASK);
           temp = temp | CMD_PLL_ON;			
		   hal_register_write(TRX_STATE, temp);
		   while((hal_register_read(TRX_STATUS) & TRX_STATUS_MASK) == TRANSITION_IN_PROGRESS);
			
		   // Now go to RX_AACK_ON
		   temp = hal_register_read(TRX_STATE) & ~(TRX_CMD_MASK);
           temp = temp | CMD_RX_AACK_ON;		   
           hal_register_write(TRX_STATE, temp);
		   while((hal_register_read(TRX_STATUS) & TRX_STATUS_MASK) == TRANSITION_IN_PROGRESS);
			   	
			
        }
		else {
			          
        //Any other state transition can be done directly.    
 		   temp = hal_register_read(TRX_STATE) & ~(TRX_CMD_MASK);
           temp = temp | new_state;			
		   hal_register_write(TRX_STATE, temp);  
		   while(hal_register_read(TRX_STATUS) == TRANSITION_IN_PROGRESS);
		}   
		   
    } 
        
    if( (hal_register_read(TRX_STATUS) & TRX_STATUS_MASK) == new_state ){
      	return 1;
	} else {
	    return 0;
	}
	
}


// Radio initializing function
void radio_init(void)
{     
	
// Init Radio IO, SLPTR, RESET, IRQ
 	
// Configure PORTD PINS to be for the RADIO interface
   ioport_configure_pin(RSTPIN, IOPORT_DIR_OUTPUT | IOPORT_PULL_UP);	    // PIN 0
   ioport_configure_pin(IRQPIN, IOPORT_DIR_INPUT | IOPORT_PULL_UP);		    // PIN 2
   ioport_configure_pin(SLPTRPIN, IOPORT_DIR_OUTPUT | IOPORT_PULL_DOWN);	// PIN 3
   ioport_configure_pin(SSPIN, IOPORT_DIR_OUTPUT | IOPORT_PULL_UP);		    // PIN 4
   ioport_configure_pin(MOSIPIN, IOPORT_DIR_OUTPUT | IOPORT_PULL_UP);		// PIN 5
   ioport_configure_pin(MISOPIN, IOPORT_DIR_INPUT | IOPORT_PULL_UP);		// PIN 6
   ioport_configure_pin(SCKPIN, IOPORT_DIR_OUTPUT | IOPORT_PULL_DOWN);		// PIN 7 
   
   gpio_set_pin_high(SSPIN);
   gpio_set_pin_high(MOSIPIN);
   gpio_set_pin_low(SCKPIN);
   
 // Reset output
 hal_set_reset(); 
  
 // SLPTR Output
 hal_clr_slptr();
 
 // Reset Transceiver
 hal_clr_reset();
 _delay_ms(2);
 hal_set_reset(); 
 _delay_ms(2);
 
 // SLPTR Low
 hal_clr_slptr();
 _delay_ms(2);
 
 // just for debug, get the part-number
 RadioPartNum = hal_register_read(PART_NUM);
 
 // Put Radio in TRX-OFF
 while(!(radio_set_trx_state(TRX_TRX_OFF))); 
  
 // 3. Turn on automatic CRC (or FCS) 
 #ifdef RADIO_RF231
 hal_register_write(TRX_CTRL_1, (1<<TX_AUTO_CRC_ON)); 
 #else
 hal_register_write(PHY_TX_PWR, (1<<TX_AUTO_CRC_ON)); 
 #endif
 
 
 #ifdef RADIO_RF231
 // For this application only want data frames, all reserved frames are filtered  
 // Dont process reserved frames
 
 hal_register_write(XAH_CTRL_1 ,(0<<AACK_UPLD_RES_FT));
   
 // Extended Mode Control
 // Max Frame Retries
 // Max CSMA retries
 // No slotted operation

 hal_register_write(XAH_CTRL_0, (MAX_TX_RETRIES << MAX_FRAME_RETRIES) | (7 << MAX_CSMA_RETRIES)); 

 // Accept frames type:
 // 0 -> Beacon
 // 1 -> Data 

 hal_register_write(CSMA_SEED_1,(1<<AACK_FVN_MODE));

#else // RF230

 // Extended Mode Control
 // Max Frame Retries
 // Max CSMA retries
 // No slotted operation

 hal_register_write(XAH_CTRL, (MAX_TX_RETRIES << MAX_FRAME_RETRIES) | (7 << MAX_CSMA_RETRIES)); 

#endif

 // Put Radio Back RX Mode auto-ack
  while(!(radio_set_trx_state(RX_AACK_ON))); 	
		
 // Set Radio on default channel	 
  radio_set_channel(DEFAULT_CHANNEL);
  
 // IRQ Masks TX_END, PLL_UNLOCK, RX_START, AWAKE_EN
  hal_register_write(IRQ_MASK, (1<<TRX_END) | (1<<PLL_UNLOCK));
 
//  enable_radio_pio_int();
  
  radio_set_address(SWITCH_ADDR);
  radio_set_channel(CHANNEL_RADIO);
  radio_set_panid(PANID);
                           
}


void recalibrate_pll(void) {
	// change channel to recalibrate pll
	char ch = radio_get_channel();
	
	if (ch == 11) {
	       radio_set_channel(12); // just jump up 1 channel
	    } 
	else if (ch == 26) {
		    radio_set_channel(25);
	     } 
    else {
		    radio_set_channel(ch+1);
	 }
			 
	// change back to original channel
	radio_set_channel(ch);
}



//////////////////////////////////////////////
//   HAL

void enable_radio_pio_int(void) {

	PORTD_INTCTRL = PORT_INT0LVL_gm;                    // Set the radio interrupt to the highest level.
	PORTD_INT0MASK = PIN2_bm;                           // Set pin 2 as the interrupt.
	PORTD_PIN2CTRL = PORT_ISC0_bm;       // rising edge config
	
	Enable_global_interrupt();

}



void disable_radio_ext_int(void){

PORTD_INT0MASK = 0;

}


void spi_init(void){

}


unsigned char hal_register_read(unsigned char address)
{
   unsigned char result = 0;
   
    //Add the register read command to the register address.
    address &= HAL_TRX_CMD_RADDRM;
    address |= HAL_TRX_CMD_RR;

    hal_ss_low(); //Start the SPI transaction by pulling the Slave Select low.

    // Transmission is started by writing the transmit data.
    spi_write(address);

    // Read data.
    result = spi_read();

    hal_ss_high(); //End the transaction by pulling the Slave Select High.

    return result;
}


void hal_register_write(unsigned char address, unsigned char val)
{
    // Add the Register Write command to the address.
    address = HAL_TRX_CMD_RW | (HAL_TRX_CMD_RADDRM & address);

    hal_ss_low(); //Start the SPI transaction by pulling the Slave Select low.

    // Transmission is started by writing the transmit data.
    spi_write(address);
    spi_write(val);

    hal_ss_high(); //End the transaction by pulling the Slave Select High.

}


unsigned char spi_read(void)
{
  unsigned char rdr = 0;
  unsigned char d = 0xAA;
  unsigned char x;

	for(x=8; x; x--) {
		
		rdr <<= 1;
		
		if(d & 0x80) {
			gpio_set_pin_high(MOSIPIN);
		} else {
			gpio_set_pin_low(MOSIPIN);
		}
		
		gpio_set_pin_high(SCKPIN);
		_delay_us(1);
		
		if(gpio_pin_is_high(MISOPIN)) {
			rdr |= 1;
		}
				
		gpio_set_pin_low(SCKPIN);
		_delay_us(1);
		
		d <<= 1;
	}
	
	return rdr;
}



void spi_write(unsigned char d) {
	unsigned char x;

	for(x=8; x; x--) {		
		if(d & 0x80) {
			gpio_set_pin_high(MOSIPIN);
		} else {
			gpio_set_pin_low(MOSIPIN);
		}
		
		gpio_set_pin_high(SCKPIN);
		_delay_us(1);
		gpio_set_pin_low(SCKPIN);
		_delay_us(1);
		
		d <<= 1;
	}
}

void hal_ss_high(void){

 gpio_set_pin_high(SSPIN);

}

void hal_ss_low(void) {

 gpio_set_pin_low(SSPIN);

}

void hal_clr_slptr(void){

 gpio_set_pin_low(SLPTRPIN);

}

void hal_set_slptr(void){

 gpio_set_pin_high(SLPTRPIN);

}

void hal_clr_reset(void){

 gpio_set_pin_low(RSTPIN);

}

void hal_set_reset(void){

 gpio_set_pin_high(RSTPIN);

}



ISR(PORTD_INT1_vect)
{
	// Reset the interrupt flag
	PORTD_INTFLAGS = PORT_INT1IF_bm;

	unsigned char status;
	status = hal_register_read(IRQ_STATUS);
	
	if (TxMode) {
		TxMode = 0;
	}
	else if (status & (1<<TRX_END)) {
		     NewMessage = 1;
		}			 
	
	if (status & (1<<PLL_UNLOCK)) {
	   PllUnlock = 1;
	 }
	 
	if (status & (1<<PLL_LOCK)) {
		RadioPLLLock = 1;
	} 
}


