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

/** \file
 *  \brief AVR USART Register Compatibility Definitions
 *  \author Atmel Crypto Products
 *  \date   October 21, 2010
 */

#ifndef AVR_COMPATIBLE_H
#   define AVR_COMPATIBLE_H

// port mappings for AT90USB1287 that has only one UART

// definitions for UART control and status registers
#define UCSRA  UCSR1A   //!< UART control and status register A
#define UCSRB  UCSR1B   //!< UART control and status register B
#define UCSRC  UCSR1C   //!< UART control and status register C

// definition for UART data register
#define UDR    UDR1     //!< UART data register

// definitions for UART baud rate registers
#define UBRRL  UBRR1L   //!< UART baud rate register, low byte
#define UBRRH  UBRR1H   //!< UART baud rate register, high byte

// definitions for bits of UART control and status register A
#define RXC    RXC1     //!< UART receive-complete (bit 7, register A)
#define TXC    TXC1     //!< UART transmit-complete (bit 6, register A)
#define UDRE   UDRE1    //!< UART data-register-empty (bit 5, register A)
#define FE     FE1      //!< UART frame-error (bit 4, register A)
#define DOR    DOR1     //!< UART data-overrun (bit 3, register A)
#define UPE    UPE1     //!< UART parity-error (bit 2, register A)
#define U2X    U2X1     //!< UART double-speed (bit 1, register A)
#define MPCM   MPCM1    //!< UART multi-processor communication (bit 0, register A)

// definitions for bits of UART control and status register B
#define RXCIE  RXCIE1   //!< UART rx complete interrupt enable (bit 7, register B)
#define TXCIE  TXCIE1   //!< UART tx complete interrupt enable (bit 6, register B)
#define UDRIE  UDRIE1   //!< UART data register empty interrupt enable (bit 5, register B)
#define RXEN   RXEN1    //!< UART enable-receiver (bit 4, register B)
#define TXEN   TXEN1    //!< UART enable-transmitter (bit 3, register B)
#define UCSZ_2 UCSZ12   //!< UART msb of number of data bits (bit 2, register B)
#define RXB8   RXB81    //!< UART receive ninth data bit (bit 1, register B)
#define TXB8   TXB81    //!< UART send ninth data bit (bit 0, register B)

// The SHA204 interface module does not use UART control and status register C.

#endif

