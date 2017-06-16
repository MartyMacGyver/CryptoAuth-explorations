#ifndef COMPILER_H
#define COMPILER_H

/** \file       compiler.h
 *  \brief      This file holds macro definitions for different compilers.
 *
 *  \date       April 6, 2010
*/

#if defined (__GNUC__)
	#define INLINE_ATTRIBUTE   __attribute__((always_inline))
	#define PACKED_ATTRIBUTE1
	#define PACKED_ATTRIBUTE2  __attribute__((packed))
#elif defined (__CODEVISIONAVR__)
	#define INLINE_ATTRIBUTE
	#define PACKED_ATTRIBUTE1
	#define PACKED_ATTRIBUTE2
#elif defined (__IAR__)
	#define INLINE_ATTRIBUTE
	#define PACKED_ATTRIBUTE1  __packed
	#define PACKED_ATTRIBUTE2
#else // other compiler
   #define INLINE_ATTRIBUTE
   #define PACKED_ATTRIBUTE1
   #define PACKED_ATTRIBUTE2
#endif

#endif
