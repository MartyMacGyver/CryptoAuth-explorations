/**
 * This module interfaces with a SHA204 SWI device.
 *
 * \date 8/5/2011 5:01:48 PM
 * \author gunter.fuchs
 */ 

#include <stdint.h>
#include "uart_config.h"
#include "sha204_physical.h"
#include "sha204_lib_return_codes.h"
#include "timers.h"


//! delay after Wakeup pulse in 100 us.
static uint8_t wakeup_delay = 10; // 1 ms

//! SWI communication timeout in ms
static uint16_t comm_timeout = 500;


/** \brief This function sets the Wakeup delay.
 *  \param[in] delay communication delay after Wakeup pulse in 100 us.
 */
void set_wakeup_delay(uint8_t delay)
{
	wakeup_delay = delay;
}


/** \brief This function returns the Wakeup delay.
 *  \return Wakeup delay in ms
 */
uint8_t get_wakeup_delay(void)
{
	return wakeup_delay;
}


/** \brief This function enables or disables the GPIO internal pull-up on the SWI signal line.
 *  \param[in] enable enables pull-up resistor if true, otherwise disables it
 */
void set_pullup(uint8_t enable)
{
   // We use the RX pin since its GPIO equivalent is anyway used as input.
   // todo Investigate whether we need to disable the USART first.
   UART_GPIO_DDR &= ~UART_GPIO_PIN_RX;
   
   if (enable)
	   UART_GPIO_OUT |= UART_GPIO_PIN_RX;
	else
	   UART_GPIO_OUT &= ~UART_GPIO_PIN_RX;
}


/** \brief This function sets the SWI communication timeout.
 *  \param[in] timeout SWI communication timeout in 10 ms units
 */
void set_timeout(uint8_t timeout)
{
	comm_timeout = timeout * 10;
}


/** \brief This function sends the data part received through the I2C slave
 *         to a SHA204 device and receives its response. It surrounds the
 *         transaction with a Wakeup pulse and an Idle flag.
 *  \param[in] tx_buffer pointer to transmit buffer
 *  \param[out] rx_buffer pointer to receive buffer
 *  \param[in] rx_count number of bytes to receive
 *  \return status of the operation
 */
uint8_t sha204_communicate(uint8_t *tx_buffer, uint8_t *rx_buffer, uint8_t rx_count)
{
	sha204p_wakeup();

    // Send TX buffer.
	uint8_t status = sha204p_send_command(tx_buffer[SHA204_BUFFER_POS_COUNT], tx_buffer);
	if (status != SHA204_SUCCESS) {
   		sha204p_idle();
	    return status;	
	}
	
	// Poll device and, once it is ready, receive its response.
	Timer_delay_ms_without_blocking(comm_timeout);
	while (!timer_delay_ms_expired) {
		status = sha204p_receive_response(rx_count, rx_buffer);
		if (status == SHA204_SUCCESS) {
		   break;
		}		   
	}
	GenericTimer_Stop();

    sha204p_idle();

    return status;	
}