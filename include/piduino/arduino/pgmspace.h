/* Copyright (c) 2002-2007  Marek Michalkiewicz
   Copyright (c) 2006, Carlos Lamas
   Copyright (c) 2009-2010, Jan Waclawek
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   * Neither the name of the copyright holders nor the names of
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE. */

/* $Id$ */

#pragma once

#ifndef __DOXYGEN__

#define __need_size_t
#include <inttypes.h>
#include <stddef.h>
#include <string.h>

#ifndef __ATTR_CONST__
#define __ATTR_CONST__ __attribute__((__const__))
#endif

#ifndef __ATTR_PROGMEM__
#define __ATTR_PROGMEM__
#endif

#ifndef __ATTR_PURE__
#define __ATTR_PURE__ __attribute__((__pure__))
#endif

#define PROGMEM __ATTR_PROGMEM__

#ifdef __cplusplus
extern "C" {
#endif

#define __LPM_ptr(addr)    (*((void *)(addr)))
#define __LPM(addr)        (*((uint8_t *)(void *)(addr)))
#define __LPM_word(addr)   (*((uint16_t *)(void *)(addr)))
#define __LPM_dword(addr)  (*((uint32_t *)(void *)(addr)))
#define __LPM_float(addr)  (*((float *)(void *)(addr)))

#define __LPM_classic__(addr) __LPM(addr)
#define __LPM_tiny__(addr) __LPM(addr)
#define __LPM_enhanced__(addr) __LPM(addr)
#define __LPM_word_classic__(addr)  __LPM_word(addr)
#define __LPM_word_tiny__(addr) __LPM_word(addr)
#define __LPM_word_enhanced__(addr) __LPM_word(addr)
#define __LPM_dword_classic__(addr) __LPM_dword(addr)
#define __LPM_dword_tiny__(addr) __LPM_dword(addr)
#define __LPM_dword_enhanced__(addr) __LPM_dword(addr)
#define __LPM_float_classic__(addr) __LPM_float(addr)
#define __LPM_float_tiny__(addr) __LPM_float(addr)
#define __LPM_float_enhanced__(addr) __LPM_float(addr)

typedef void PROGMEM prog_void;
typedef char PROGMEM prog_char;
typedef unsigned char PROGMEM prog_uchar;
typedef int8_t PROGMEM prog_int8_t;
typedef uint8_t PROGMEM prog_uint8_t;
typedef int16_t PROGMEM prog_int16_t;
typedef uint16_t PROGMEM prog_uint16_t;
typedef int32_t PROGMEM prog_int32_t;
typedef uint32_t PROGMEM prog_uint32_t;
typedef int64_t PROGMEM prog_int64_t;
typedef uint64_t PROGMEM prog_uint64_t;

#ifndef PGM_P
#define PGM_P const char *
#endif

#ifndef PGM_VOID_P
#define PGM_VOID_P const void *
#endif

#define PSTR(s) ((const PROGMEM char *)(s))

#define pgm_read_byte_near(address_short) __LPM(address_short)
#define pgm_read_word_near(address_short) __LPM_word(address_short)
#define pgm_read_dword_near(address_short) __LPM_dword(address_short)
#define pgm_read_float_near(address_short) __LPM_float(address_short)
#define pgm_read_ptr_near(address_short) __LPM_ptr(address_short)
#define pgm_read_byte(address_short)    pgm_read_byte_near(address_short)
#define pgm_read_word(address_short)    pgm_read_word_near(address_short)
#define pgm_read_dword(address_short)   pgm_read_dword_near(address_short)
#define pgm_read_float(address_short)   pgm_read_float_near(address_short)
#define pgm_read_ptr(address_short)     pgm_read_ptr_near(address_short)
#define pgm_get_far_address(var) (&(var))

#define strlcat_P strlcat
#define strlcpy_P strlcpy

#define memchr_P memchr
#define memcmp_P memcmp
#define memccpy_P memccpy
#define memcpy_P memcpy
#define memmem_P memmem
#define memrchr_P memrchr
#define strcat_P strcat
#define strchr_P strchr
#define strcmp_P strcmp
#define strcpy_P strcpy
#define strcasecmp_P strcasecmp
#define strcspn_P strcspn
#define strncmp_P strncmp
#define strncasecmp_P strncasecmp
#define strncat_P strncat
#define strncpy_P strncpy
#define strpbrk_P strpbrk
#define strrchr_P strrchr
#define strsep_P strsep
#define strspn_P strspn
#define strstr_P strstr
#define strcasestr_P strcasestr
#define strtok_P strtok
#define strtok_rP strtok_r
#define strlen_P strlen
#define strnlen_P strnlen
#define strchrnul_P strchrnul

#ifdef __cplusplus
}
#endif

#endif /* __DOXYGEN__ not defined */
