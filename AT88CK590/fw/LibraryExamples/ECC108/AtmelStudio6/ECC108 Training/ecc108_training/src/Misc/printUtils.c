/*
 * ascii2hex.c
 *
 * Created: 10/10/2012 11:50:45 AM
 *  Author: rocendob
 */ 



#include <stdio.h>
#include <string.h>


char ascii_to_hex(char c){
	char tmp = 0;
	if((c >= 0x30) & (c <= 0x39)) {
		tmp =  (c - 0x30);
	} else {
		tmp = (c - 0x37);
	}
	return tmp;
}

unsigned char ConvertShortAsciitoChar(unsigned char * pData)
{
	unsigned char bh = 0;
	unsigned char bl = 0;
	
	if((*pData >= 0x30) && (*pData <= 0x39)) {  // 0-9
		bh =  (*pData - 0x30);
	} 
	else if ((*pData >= 0x61) && (*pData <= 0x66)) { // A-F
	    bh = (*pData - 0x57);
	} 
	else {		
		bh = (*pData - 0x37); // a-f
	}
	
	
	if((*(pData+1) >= 0x30) && (*(pData+1) <= 0x39)) {
		bl =  (*(pData+1) - 0x30);
     } 
	 else if ((*(pData+1) >= 0x61) && (*(pData+1) <= 0x66)) { // A-F
        bl = (*(pData+1) - 0x57);
	}
	 else {
		bl = (*(pData+1) - 0x37);
	}
	
	bh = bh << 4;
	
	return (bh | bl);
}



void PrintHash(unsigned char *hash)
{
	printf("%02X%02X %02X%02X %02X%02X %02X%02X "
	"%02X%02X %02X%02X %02X%02X %02X%02X "
	"%02X%02X %02X%02X %02X%02X %02X%02X "
	"%02X%02X %02X%02X %02X%02X %02X%02X\r\n",
	hash[0], hash[1], hash[2], hash[3],
	hash[4], hash[5], hash[6], hash[7],
	hash[8], hash[9], hash[10], hash[11],
	hash[12], hash[13], hash[14], hash[15],
	hash[16], hash[17], hash[18], hash[19],
	hash[20], hash[21], hash[22], hash[23],
	hash[24], hash[25], hash[26], hash[27],
	hash[28], hash[29], hash[30], hash[31]);
}


//------------------------------------------------------------------------------
/// Display the buffer, 8 byte per line
/// \param buffer   Pointer to the data location
/// \param len      Size of the data
//------------------------------------------------------------------------------
void ShowBuffer(unsigned char * buffer, unsigned int len)
{
	unsigned int i,j;
	unsigned int HexCnt = 0;
	unsigned int CharCnt = 0;
	
	for (j = 0; j < len; j=j+8) {
		// print hex
		for (i=0; i<8; i++) {
			
			printf(" %02X", buffer[i+j]);
			if (++HexCnt >= len) {
			     while(++i < 8) {
				    printf("   ");
				 }					 
			 	
			     break;
			}				 
		}
		
		printf("    ");
		
		//print chars
		for (i=0; i<8; i++) {
			if ((buffer[i+j] > 0x1F) & (buffer[i+j] < 0x7F)) {
				
				printf(" %c", buffer[i+j]);
				if (++CharCnt >= len)
				     break;				
					 					 
			} else {
				printf(" .");
				if (++CharCnt >= len)
				    break;
	
			}
		}	
		printf("\n\r");	
	}
	printf("\n\r");
}

