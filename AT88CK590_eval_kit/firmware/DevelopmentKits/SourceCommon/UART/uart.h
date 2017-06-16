/** \file
 *  \brief Header file for uart.c.
 * Author: gunter.fuchs
 *
 * Created on June 16, 2008, 12:07 PM
 */

#ifndef _LL_UART_H
#define	_LL_UART_H

#ifdef	__cplusplus
extern "C" {
#endif

#ifdef UART_LOGGING /* DEBUG MODE */

#include "config.h"

void uart_Init(void);
void uart_Enable(uchar ucOnOff);
RETURN_CODE uart_PutChar(schar scChar);
RETURN_CODE uart_PutData(puchar pucData, uchar ucLength);
RETURN_CODE uart_PutStringProgMem(const char* pscData);
RETURN_CODE uart_PutString(const pschar pscData);
void uart_WaitForBufferEmptyTx(void);

#endif // UART_LOGGING

#ifdef	__cplusplus
}
#endif

#endif // _LL_UART_H
