/** \file
 * *****************************************************************************
*
* Atmel Corporation
*
* File              : USI_TWI_Master.h
* Compiler          : IAR EWAAVR 2.28a/3.10a
* Revision          : $Revision: 1.11 $
* Date              : $Date: Tuesday, September 13, 2005 09:09:36 UTC $
* Updated by        : $Author: jtyssoe $
*
* Support mail      : avr@atmel.com
*
* Supported devices : All device with USI module can be used.
*                     The example is written for the ATmega169, ATtiny26 and ATtiny2313
*
* AppNote           : AVR310 - Using the USI module as a TWI Master
*
* Description       : This is an implementation of an TWI master using
*                     the USI module as basis. The implementation assumes the AVR to
*                     be the only TWI master in the system and can therefore not be
*                     used in a multi-master system.
* Usage             : Initialize the USI module by calling the USI_TWI_Master_Initialise()
*                     function. Hence messages/data are transceived on the bus using
*                     the USI_TWI_Start_Transceiver_With_Data() function. If the transceiver
*                     returns with a fail, then use USI_TWI_Get_Status_Info to evaluate the
*                     couse of the failure.
*
* Adapted by Atmel Crypto Products Group to use with ATSHA204 library.
****************************************************************************/

#include <avr/io.h>

//********** Defines **********//

// Defines controlling timing limits
#define TWI_FAST_MODE

//#define SYS_CLK   4000.0  // [kHz]
//#define SYS_CLK   (F_CPU / 1000)  // [kHz]

#ifdef TWI_FAST_MODE               // TWI FAST mode timing limits. SCL = 100-400kHz
//  #define T2_TWI    ((SYS_CLK * 1300) / 1000000) + 1 // >= 1.3us
//  #define T4_TWI    ((SYS_CLK *  600) / 1000000) + 1 // >= 0.6us
  #define T2_TWI    (2) // >= 1.3us
  #define T4_TWI    (1) // >= 0.6us
  #define I2C_CLOCK (400000)
#else                              // TWI STANDARD mode timing limits. SCL <= 100kHz
//  #define T2_TWI    ((SYS_CLK * 4700) /1000000) + 1 // >= 4.7us
//  #define T4_TWI    ((SYS_CLK * 4000) /1000000) + 1 // >= 4.0us
  #define T2_TWI    (5) // >= 4.7us
  #define T4_TWI    (4) // >= 4.0us
  #define I2C_CLOCK (100000)
#endif

// Defines controlling code generating
//#define PARAM_VERIFICATION
//#define NOISE_TESTING
//#define SIGNAL_VERIFY

/****************************************************************************
  Bit and byte definitions
****************************************************************************/
#define TWI_READ_BIT  0       // Bit position for R/W bit in "address byte".
#define TWI_ADR_BITS  1       // Bit position for LSB of the slave address bits in the init byte.
#define TWI_NACK_BIT  0       // Bit position for (N)ACK bit.

#define USI_TWI_NO_DATA             0x20  // Transmission buffer is empty
#define USI_TWI_DATA_OUT_OF_BOUND   0x21  // Transmission buffer is outside SRAM space
#define USI_TWI_UE_START_CON        0x22  // Unexpected Start Condition
#define USI_TWI_UE_STOP_CON         0x23  // Unexpected Stop Condition
#define USI_TWI_UE_DATA_COL         0x24  // Unexpected Data Collision (arbitration)
#define USI_TWI_NO_ACK_ON_DATA      0x25  // The slave did not acknowledge  all data
#define USI_TWI_NO_ACK_ON_ADDRESS   0x26  // The slave did not acknowledge  the address
#define USI_TWI_MISSING_START_CON   0x27  // Generated Start Condition not detected on bus
#define USI_TWI_MISSING_STOP_CON    0x28  // Generated Stop Condition not detected on bus

// Device dependent defines

#if defined(__AVR_AT90Mega169__) | defined(__AVR_ATmega169__) | \
    defined(__AVR_AT90Mega165__) | defined(__AVR_ATmega165__) | \
    defined(__AVR_ATmega325__) | defined(__AVR_ATmega3250__) | \
    defined(__AVR_ATmega645__) | defined(__AVR_ATmega6450__) | \
    defined(__AVR_ATmega329__) | defined(__AVR_ATmega3290__) | \
    defined(__AVR_ATmega649__) | defined(__AVR_ATmega6490__)
    #define DDR_USI             DDRE
    #define PORT_USI            PORTE
    #define PIN_USI             PINE
    #define PORT_USI_SDA        PORTE5
    #define PORT_USI_SCL        PORTE4
    #define PIN_USI_SDA         PINE5
    #define PIN_USI_SCL         PINE4
#elif defined(__AVR_ATtiny25__) | defined(__AVR_ATtiny45__) | \
	   defined(__AVR_ATtiny85__) | defined(__AVR_AT90Tiny26__) | \
	   defined(__AVR_ATtiny26__)
    #define DDR_USI             DDRB
    #define PORT_USI            PORTB
    #define PIN_USI             PINB
    #define PORT_USI_SDA        PORTB0
    #define PORT_USI_SCL        PORTB2
    #define PIN_USI_SDA         PINB0
    #define PIN_USI_SCL         PINB2
#elif defined(__AVR_AT90Tiny2313__) | defined(__AVR_ATtiny2313__)
    #define DDR_USI             DDRB
    #define PORT_USI            PORTB
    #define PIN_USI             PINB
    #define PORT_USI_SDA        PORTB5
    #define PORT_USI_SCL        PORTB7
    #define PIN_USI_SDA         PINB5
    #define PIN_USI_SCL         PINB7
#elif defined(__AVR_ATtiny44__)
    #define DDR_USI             DDRA
    #define PORT_USI            PORTA
    #define PIN_USI             PINA
    #define PORT_USI_SDA        PORTA6
    #define PORT_USI_SCL        PORTA4
    #define PIN_USI_SDA         PINA6
    #define PIN_USI_SCL         PINA4
#else
	#error This driver does not support the selected target.
#endif


// General defines
#define TRUE  1
#define FALSE 0

//********** Prototypes **********//

void          USI_TWI_Master_Initialise( void );
void          USI_TWI_Disable(void);
unsigned char USI_TWI_Start_Transceiver_With_Data( unsigned char * , unsigned char );
unsigned char USI_TWI_Get_State_Info( void );
