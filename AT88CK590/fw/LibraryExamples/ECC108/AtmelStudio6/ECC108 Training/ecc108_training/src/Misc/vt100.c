

#include <avr/pgmspace.h>
#include <stdio.h> 
#include "vt100.h"


// Functions
void vt100Init(void)
{
	// initializes terminal to "power-on" settings
	// ESC c
	printf("\x1B\x63");
}

void vt100ClearScreen(void)
{
	// ESC [ 2 J
	printf("\x1B[2J");
}

void vt100SetAttr(char attr)
{
	// ESC [ Ps m
	printf("\x1B[%dm",attr);
}

void vt100SetCursorMode(char visible)
{
	if(visible)
		// ESC [ ? 25 h
		printf("\x1B[?25h");
	else
		// ESC [ ? 25 l
		printf("\x1B[?25l");
}

void vt100SetCursorPos(char line, char col)
{
	// ESC [ Pl ; Pc H
	printf("\x1B[%d;%dH",line,col);
}

