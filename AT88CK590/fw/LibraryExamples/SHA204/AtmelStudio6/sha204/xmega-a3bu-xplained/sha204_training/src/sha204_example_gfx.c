/*
 * \file
 *
 * \brief LCD module for ATSHA204 examples
 *
 * This file contains helper functions that use the Xplained board LCD to display
 * the progress and buffer content for an exercise.
 *
 *
 * Copyright (c) 2011-2012 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 */

#include <stdio.h>
#include <string.h>
#include <asf.h>
#include "compiler.h"
#include "preprocessor.h"
#include "sha204_example_gfx.h"
#include "sha204_timer.h"

// 21 characters * 4 lines + 3 * '\n' + '\0'
static char gfx_string_buffer[88];

static char *gfx_title = "   A T S H A 2 0 4  \n";


/** \brief This function initializes the mono-graphics library.
*/
void gfx_init(void)
{
	// Initialize graphics library.
	gfx_mono_init();

	// Enable back-light.
	ioport_set_pin_high(LCD_BACKLIGHT_ENABLE_PIN);
}

/** \brief This function clears the screen.
*/
void gfx_clear_display(void)
{
	gfx_mono_generic_draw_filled_rect(0, 0, LCD_WIDTH_PIXELS, LCD_HEIGHT_PIXELS, GFX_PIXEL_CLR);
}


/** \brief This function waits for user to press a button to continue.
 */
void gfx_wait_for_continue(void)
{
	while (gpio_pin_is_high(GPIO_PUSH_BUTTON_0));
}


/** \brief This function draws a string using the system font.
    \param[in] buffer pointer to string
*/
void gfx_display_string(char *buffer)
{
	gfx_clear_display();
	gfx_mono_draw_string(buffer, 0, 0, &sysfont);
}


/** \brief This function displays an exercise title.
 * \param[in] msg pointer to exercise title
 */
void gfx_display_exercise_title(char *msg)
{
	sprintf(gfx_string_buffer,
	"%s"
	"Training Exercise\n"
	"%s\n"
	"Press SWO to start\n"
	, gfx_title, msg);
	gfx_display_string(gfx_string_buffer);
	gfx_wait_for_continue();
}


/** \brief This function displays a buffer in hex-ascii.
    \param[in] len number of bytes in buffer
	\param[in] buffer pointer to buffer
	\param[in] msg pointer to text for first line
*/
void gfx_display_buffer(uint8_t len, uint8_t *buffer, char *msg)
{
	uint8_t i;
	char *out_buffer = gfx_string_buffer;
	
	if (len > 32)
		len = 32;

	if (strlen(msg) > GFX_CHARS_PER_LINE)
		msg[GFX_CHARS_PER_LINE] = '\0';
	sprintf(gfx_string_buffer,
		"%s\n"
		"Press SW0 for buffer\n"
		"Press SWO again\n"
		"to continue", msg
		);
	gfx_display_string(gfx_string_buffer);
	gfx_wait_for_continue();
		
	for (i = 1; i <= len; i++, buffer++) {
		out_buffer += sprintf(out_buffer, "%02X", *buffer);
		if (i % 2 == 0) {
			// Add a separating space for better readability.
			out_buffer = strcat(out_buffer, " ");
			out_buffer++;
		}			
		if (i % 8 == 0) {
			// Add a newline every eight bytes so that eight bytes are displayed on one line.
			out_buffer = strcat(out_buffer, "\n");
			out_buffer++;
		}	
	}
	gfx_display_string(gfx_string_buffer);
	gfx_wait_for_continue();
}


/** \brief This function displays a status message and the status.
    \param[in] msg pointer to message string
	\param[in] status status byte
*/
void gfx_display_status(char *msg, uint8_t status)
{
	if (strlen(msg) > GFX_CHARS_PER_LINE)
		msg[GFX_CHARS_PER_LINE] = '\0';
	
	sprintf(gfx_string_buffer, 
		"%s"
		"\n"
		"%s\n"
		"Status: %02X"
		, gfx_title, msg, status);
	gfx_display_string(gfx_string_buffer);
	gfx_wait_for_continue();
}


/** \brief This function displays the lock status of Data / OTP and Config zone.
 * \param[in] lock_data lock status of Data / OTP zone
 * \param[in] lock_config lock status of Configuration zone
 */
void gfx_display_lock_status(uint8_t lock_data, uint8_t lock_config)
{
	char *locked_string =   "  Locked";
	char *unlocked_string = "Unlocked";
	sprintf(gfx_string_buffer,
		"%s" 
		"     Lock Status\n"
		"Data - OTP: %s\n"
		"Config:     %s"
		, gfx_title 
		, lock_data ? unlocked_string : locked_string
		, lock_config ? unlocked_string : locked_string);
	gfx_display_string(gfx_string_buffer);
	gfx_wait_for_continue();
}


/** \brief This function displays information about connected ATSHA204 devices. 
 * \param[in] device_present_mask Which ATSHA204 devices are present.
 * \param[in] sha204_revision revision of the ATSHA204 devices
 */
void gfx_display_revision(uint8_t device_present_mask, uint8_t sha204_revision)
{
	uint8_t i, j;
	uint8_t shifter = 1;
	uint8_t address[] = {0, 0};

	// Find out how many devices are present.
	for (i = 0, j = 0; shifter < 0x04; shifter <<= 1, j++) {
		address[j] = sha204_i2c_address(j);
		if (shifter & device_present_mask)
			i++;
	}					
	// Display info about devices.
	if (i > 0) {
		sprintf(gfx_string_buffer, 
			"%s"
			"found %u device(s)\n"
			"revision:  %u\n"
			"addresses: %02X, %02X", 
			gfx_title, i, sha204_revision, address[0], address[1]);
		gfx_display_string(gfx_string_buffer);
	}		
	else
		gfx_display_string("no devices found");

	gfx_wait_for_continue();
}
