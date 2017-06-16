
#include "usart.h"
#include "conf_usart_cdc_xplained.h"
#include "status_codes.h"

#include <stdint.h>
#include <stdio.h>

#define RX_BUFSIZE 128

int stdio_putchar(char c, FILE *stream)
{

  if (c == '\a')
    {
      fputs("*ring*\n", stderr);
      return 0;
    }

  if (c == '\n')
    stdio_putchar('\r', stream);

  
   usart_putchar(USART_CDC_XPLAINED, c);

  return 0;
}



/*
 * Receive a character from the FTDI USB-Parallel
 *
 * This features a simple line-editor that allows to delete and
 * re-edit the characters entered, until either CR or NL is entered.
 * Printable characters entered will be echoed using uart_putchar().
 *
 * Editing characters:
 *
 * . \b (BS) or \177 (DEL) delete the previous character
 * . ^u kills the entire input buffer
 * . ^w deletes the previous word
 * . ^r sends a CR, and then reprints the buffer
 * . \t will be replaced by a single space
 *
 * All other control characters will be ignored.
 *
 * The internal line buffer is RX_BUFSIZE (80) characters long, which
 * includes the terminating \n (but no terminating \0).  If the buffer
 * is full (i. e., at RX_BUFSIZE-1 characters in order to keep space for
 * the trailing \n), any further input attempts will send a \a to
 * uart_putchar() (BEL character), although line editing is still
 * allowed.
 *
 * Input errors while talking to the UART will cause an immediate
 * return of -1 (error indication).  Notably, this will be caused by a
 * framing error (e. g. serial line "break" condition), by an input
 * overrun, and by a parity error (if parity was enabled and automatic
 * parity recognition is supported by hardware).
 *
 * Successive calls to uart_getchar() will be satisfied from the
 * internal buffer until that buffer is emptied again.
 */

int stdio_getchar(FILE *stream)
{
  char c;
  char *cp, *cp2;
  static char b[RX_BUFSIZE];
  static char *rxp;

  if (rxp == 0)
    for (cp = b;;)
      {
	    c = usart_getchar(USART_CDC_XPLAINED);

	/* behaviour similar to Unix stty ICRNL */

	if (c == '\r')
	  c = '\n';
	if (c == '\n')
	  {
	    *cp = c;
	    stdio_putchar(c, stream);
	    rxp = b;
	    break;
	  }
	else if (c == '\t')
	  c = ' ';

	if ((c >= (uint8_t)' ' && c <= (uint8_t)'\x7e') ||
	    c >= (uint8_t)'\xa0')
	  {
	    if (cp == b + RX_BUFSIZE - 1)
	      stdio_putchar('\a', stream);
	    else
	      {
		*cp++ = c;
		stdio_putchar(c, stream);
	      }
	    continue;
	  }

	switch (c)
	  {
	  case 'c' & 0x1f:
	    return -1;

	  case '\b':
	  case '\x7f':
	    if (cp > b)
	      {
		stdio_putchar('\b', stream);
		stdio_putchar(' ', stream);
		stdio_putchar('\b', stream);
		cp--;
	      }
	    break;

	  case 'r' & 0x1f:
	    stdio_putchar('\r', stream);
	    for (cp2 = b; cp2 < cp; cp2++)
	      stdio_putchar(*cp2, stream);
	    break;

	  case 'u' & 0x1f:
	    while (cp > b)
	      {
		stdio_putchar('\b', stream);
		stdio_putchar(' ', stream);
		stdio_putchar('\b', stream);
		cp--;
	      }
	    break;

	  case 'w' & 0x1f:
	    while (cp > b && cp[-1] != ' ')
	      {
		stdio_putchar('\b', stream);
		stdio_putchar(' ', stream);
		stdio_putchar('\b', stream);
		cp--;
	      }
	    break;
	  }
      }

  c = *rxp++;
  if (c == '\n')
    rxp = 0;

  return c;
}

