/** \file
 *  \brief Data converter functions
 */

#include <stdlib.h>
#include <avr/pgmspace.h>

#include "utility.h"


/** \brief Converting Nible to ASCII
 *
 * \param ucData is nible data to be converted
 *
 * \return ucData is the ASCII value
**/
U8 Nible_to_ascii ( U8 ucData )
{
    ucData &= 0x0F;
    if (ucData <= 0x09 )
    {
        ucData += '0';
    }
    else
    {
        ucData = ucData - 10 + 'A';
    }
    return ucData;
}


/** \brief Converting ASCII to Nible
 *
 * \param ucData is the ASCII value to be converted
 *
 * \return ucData is the nible value
**/
U8 Ascii_to_nible ( U8 ucData )
{
    if ((ucData <= '9' ) && (ucData >= '0')) 
    {
        ucData -= '0';
    }
    else if ((ucData <= 'F' ) && (ucData >= 'A')) 
    {
        ucData = ucData -'A' + 10;
    }
    else if ((ucData <= 'f' ) && (ucData >= 'a')) 
    {
        ucData = ucData -'a' + 10;
    }
    else 
    {
        ucData = 0;
    }
    return ucData;
}


#ifdef DEBUG /* DEBUG MODE */


/** \brief Put byte to ringbuffer for transmitting via UART
 *
 *  \param ucData is a hexadecimal data byte to be transmitted
 */
void Uart1_put_hex ( uchar ucData )
{
    uchar ucTemp;

    ucTemp = ucData >> 4;
    ucTemp = Nible_to_ascii ( ucTemp );
    Uart1_putc ( ucTemp );

    ucTemp = Nible_to_ascii ( ucData );
    Uart1_putc ( ucTemp );
}

#endif /* DEBUG MODE */


