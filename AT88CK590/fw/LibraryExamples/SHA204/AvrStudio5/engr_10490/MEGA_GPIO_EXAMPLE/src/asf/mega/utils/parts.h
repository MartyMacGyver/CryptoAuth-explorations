/**
 * \file
 *
 * \brief megaAVR device family definitions
 *
 * Copyright (C) 2011 Atmel Corporation. All rights reserved.
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 * Atmel AVR product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

#ifndef MEGA_PARTS_H
#define MEGA_PARTS_H

//! Convenience macro for checking GCC and IAR part definitions
#define part_is_defined(part) \
	(defined(__ ## part ## __) || defined(__AVR_ ## part ## __))

// ATmegaxx0
#define MEGA_XX0 (\
		part_is_defined(ATmega640) || \
		part_is_defined(ATmega1280) || \
		part_is_defined(ATmega2560) \
	)

// ATmegxx1
#define MEGA_XX1 (\
		part_is_defined(ATmega1281) || \
		part_is_defined(ATmega2561) \
	)

// ATmegaxx0/xx1 
#define MEGA_XX0_1	(MEGA_XX0 || MEGA_XX1)


// ATmegaxx4
#define MEGA_XX4 (\
		part_is_defined(ATmega164A) || part_is_defined(ATmega164PA) || \
		part_is_defined(ATmega324) || part_is_defined(ATmega324A) || \
		part_is_defined(ATmega324PA) || part_is_defined(ATmega644) || \
		part_is_defined(ATmega644A) || part_is_defined(ATmega644PA) || \
		part_is_defined(ATmega1284P) \
	)

// ATmegaxx4
#define MEGA_XX4_A (\
		part_is_defined(ATmega164A) || part_is_defined(ATmega164PA) || \
		part_is_defined(ATmega324A) || part_is_defined(ATmega324PA) || \
		part_is_defined(ATmega644A) || part_is_defined(ATmega644PA) || \
		part_is_defined(ATmega1284P) \
	)

// ATmegaxx8
#define MEGA_XX8 (\
		part_is_defined(ATmega48) || part_is_defined(ATmega48A) || \
		part_is_defined(ATmega48PA) || part_is_defined(ATmega88) || \
		part_is_defined(ATmega88A) || part_is_defined(ATmega88PA) || \
		part_is_defined(ATmega168) || part_is_defined(ATmega168A) || \
		part_is_defined(ATmega168PA) || part_is_defined(ATmega328) || \
		part_is_defined(ATmega328P) \
	)

// ATmegaxx8A/P/PA
#define MEGA_XX8_A (\
		part_is_defined(ATmega48A) || part_is_defined(ATmega48PA) || \
		part_is_defined(ATmega88A) || part_is_defined(ATmega88PA) || \
		part_is_defined(ATmega168A) || part_is_defined(ATmega168PA) || \
		part_is_defined(ATmega328P) \
	)

// ATmegaxx
#define MEGA_XX (\
		part_is_defined(ATmega16) || part_is_defined(ATmega16A) || \
		part_is_defined(ATmega32) || part_is_defined(ATmega32A) || \
		part_is_defined(ATmega64) || part_is_defined(ATmega64A) || \
		part_is_defined(ATmega128) || part_is_defined(ATmega128A) \
	)

// ATmegaxxA/P/PA
#define MEGA_XX_A (\
		part_is_defined(ATmega16A) || part_is_defined(ATmega32A) || \
		part_is_defined(ATmega64A) || part_is_defined(ATmega128A) \
	)


// Unspecified 
#define MEGA_UNSPECIFIED (\
		part_is_defined(ATmega16) || part_is_defined(ATmega16A) || \
		part_is_defined(ATmega32) || part_is_defined(ATmega32A) || \
		part_is_defined(ATmega64) || part_is_defined(ATmega64A) || \
		part_is_defined(ATmega128) || part_is_defined(ATmega128A) || \
		part_is_defined(ATmega169P) || part_is_defined(ATmega169PA) || \
		part_is_defined(ATmega329P) || part_is_defined(ATmega329PA) \
	)

// All megaAVR devices
#define MEGA (MEGA_XX0_1 || MEGA_XX4 || MEGA_XX8 || MEGA_XX || MEGA_UNSPECIFIED)

#endif /* MEGA_PARTS_H */
