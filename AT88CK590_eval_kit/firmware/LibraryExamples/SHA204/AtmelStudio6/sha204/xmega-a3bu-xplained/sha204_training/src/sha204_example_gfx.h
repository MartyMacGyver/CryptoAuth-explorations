/**
 * \file
 *
 * \brief GFX module for ATSHA204 examples
 *
 * This file defines helper functions that use Xplained board LCD to indicate
 * success or failure and byte values (device version for instance).
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
 *
 */

#ifndef SHA204_EXAMPLE_GFX_H_
#define SHA204_EXAMPLE_GFX_H_

#define GFX_CHARS_PER_LINE   21
#define GFX_LINE_COUNT        4

/**
 * \defgroup sha204_example_gfx_group ATSHA204 Component Example - status LCD
 *functions
 *
 * @{
 */
void gfx_init(void);
void gfx_clear_display(void);
void gfx_wait_for_continue(void);
void gfx_display_string(char *buffer);
void gfx_display_exercise_title(char *msg);
void gfx_display_buffer(uint8_t len, uint8_t *buffer, char *msg);
void gfx_display_lock_status(uint8_t lock_data, uint8_t lock_config);
void gfx_display_status(char *msg, uint8_t status);
void gfx_display_revision(uint8_t device_present_mask, uint8_t sha204_revision);
/* ! @} */

#endif /* SHA204_EXAMPLE_GFX_H_ */
