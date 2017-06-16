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
// File Name           : AVR_compat.h
// Object              : AVR register compatibility definitions
// Creation            : SEM 	5/26/2009
// ----------------------------------------------------------------------------

#ifndef AVR_COMPAT_H
#define AVR_COMPAT_H

/*
 * port mappings for AT90USB1287
 */

#define UCSRA	UCSR1A
#define UCSRB	UCSR1B
#define UCSRC	UCSR1C

#define UBRRL	UBRR1L
#define UBRRH	UBRR1H

#define TXEN	TXEN1
#define RXEN	RXEN1

#define UDRE	UDRE1
#define UDR		UDR1

#define RXC		RXC1
#define TXC		TXC1
#define FE		FE1
#define DOR		DOR1

#define U2X		U2X1
#define UCSZ	UCSZ1

#endif

