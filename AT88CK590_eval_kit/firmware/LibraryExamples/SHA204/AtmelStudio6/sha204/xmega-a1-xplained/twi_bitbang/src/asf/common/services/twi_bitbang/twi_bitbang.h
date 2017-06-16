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
 *  \brief  Header file for TWI bit-banged driver. This driver is not part of original ASF.
 *  \author Atmel Crypto Products
 *  \date   June 6, 2012
*/

#ifndef TWI_BITBANG_H
#   define TWI_BITBANG_H

#include <asf.h>

#define I2C_PORT                            IOPORT_PORTD
#define I2C_PIN_SCL                         0
#define I2C_PIN_SDA							1
#define I2C_PIN_GROUP                       (1 << I2C_PIN_SCL) | (1 << I2C_PIN_SDA)
#define I2C_PIN_OUTPUT_CONFIG               IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH | IOPORT_WIRED_AND_PULL_UP
#define I2C_PIN_INPUT_CONFIG                IOPORT_DIR_INPUT | IOPORT_PULL_UP
#define I2C_SCL_GPIO                        IOPORT_CREATE_PIN(PORTD, I2C_PIN_SCL)
#define I2C_SDA_GPIO                        IOPORT_CREATE_PIN(PORTD, I2C_PIN_SDA)


/** \brief This delay macro uses the ASF delay service. But because it retrieves the
           CPU clock frequency at run-time it adds approx. 35 us to the intended delay
		   when optimization is off. With optimization on it works down to 4 us.
*/ 
#define I2C_CLOCK_DELAY                     cpu_delay_us(4, sysclk_get_cpu_hz())

/** \brief Here, the delay macro is not defined and such no delays are inserted between 
           toggling SCL. The TWI speed is the fastest possible to be achieved with the 
		   ASF GPIO driver. This leads to a TWI speed of approx. 95 kHz. To increase the 
		   TWI speed one has to drop using the ASF GPIO driver and implement a 
		   driver that uses the Xmega I/O definitions directly. This means that one 
		   would have to implement this driver every time it is ported to another MCU.
		   No delay works only with optimization turned off (-O1).
*/
//#define I2C_CLOCK_DELAY                     


void i2c_enable(void);
void i2c_pulse_sda_low(uint16_t duration);
void i2c_send_start(void);
void i2c_send_stop(void);
uint8_t i2c_receive_byte(bool ack);
void i2c_receive_bytes(uint8_t count, uint8_t *data);
uint8_t i2c_send_bytes(uint8_t count, uint8_t *data);

#endif
   
