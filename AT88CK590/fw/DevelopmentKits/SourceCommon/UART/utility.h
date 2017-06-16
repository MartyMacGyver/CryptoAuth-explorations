/** \file
 *  \brief Header file for utility.c.
 */

#ifndef _UTILITY_H_
#define _UTILITY_H_

#include "config.h"
#include "uart.h"

uchar util_ConvertAsciiToNibble(uchar ucData);
uchar util_ConvertNibbleToAscii(uchar ucData);

#ifdef UART_LOGGING /* DEBUG MODE */
  RETURN_CODE uart_PutCharHex(uchar ucData);
  RETURN_CODE uart_PutDataHex(puchar pucData, uchar ucLength);
  RETURN_CODE uart_LogBowline(puchar pucTx, uchar ucTxLen, puchar pucRx, uchar ucRxLen, uchar ucStatus, uchar ucExpected);
  #define uart_PutCrLf() uart_PutString((pschar) "\r\n")
#endif /* DEBUG MODE */

#endif // UTILITY_H
