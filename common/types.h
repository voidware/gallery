/**
 *
 *    _    __        _      __                           
 *   | |  / /____   (_)____/ /_      __ ____ _ _____ ___ 
 *   | | / // __ \ / // __  /| | /| / // __ `// ___// _ \
 *   | |/ // /_/ // // /_/ / | |/ |/ // /_/ // /   /  __/
 *   |___/ \____//_/ \__,_/  |__/|__/ \__,_//_/    \___/ 
 *                                                       
 * Copyright (c) 2020 Voidware Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License (LGPL) as published
 * by the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 * for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 * 
 * contact@voidware.com
 */

#pragma once

typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned char uchar;

#include <stdint.h>

#ifdef _WIN32

#ifdef __MINGW32__

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#define _PRId64 PRId64
#define _PRIX64 PRIX64
#define _PRIx64 PRIx64

#elif defined(_MSC_VER)

// this in base member initialisation
#pragma warning(disable: 4355)

// Have our own versions of `PRId64`.
// these are meant to be in inttypes.h, but they're not always (eg mingw)
#define _PRId64 "I64d"
#define _PRIX64 "I64X"
#define _PRIx64 "I64x"

#else
#error unknown WIN32 compiler
#endif // MINGW

#elif defined(__GNUC__)

#include <inttypes.h>

#ifdef PRIX64

#define _PRId64 PRId64
#define _PRIX64 PRIX64
#define _PRIx64 PRIx64

#else

// missing (eg android build), define our own
# if __WORDSIZE == 64
#define _PRId64 "ld"
#define _PRIX64 "lX"
#define _PRIx64 "lx"
#else
#define _PRId64 "lld"
#define _PRIX64 "llX"
#define _PRIx64 "llx"
#endif

#endif // PRIX64

#else
#error compiler unknown
#endif  // WIN32

typedef uint64_t uint64;
typedef int64_t int64;


 
