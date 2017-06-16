/*
 * printUtils.h
 *
 * Created: 10/10/2012 11:51:32 AM
 *  Author: rocendob
 */ 


#ifndef PRINTUTILS_H_
#define PRINTUTILS_H_

unsigned char ConvertShortAsciitoChar(unsigned char * pData);
char ascii_to_hex(char c);
void ShowBuffer(unsigned char * buffer, unsigned int len);
void PrintHash(unsigned char *hash);

#endif /* PRINTUTILS_H_ */