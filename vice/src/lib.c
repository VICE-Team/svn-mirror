/*
 * lib.c - Library functions.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */

#include "vice.h"


#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef AMIGA_SUPPORT
#ifndef __USE_INLINE__
#define __USE_INLINE__
#endif
#endif

#if defined(AMIGA_SUPPORT) || defined(__VBCC__)
#include <proto/exec.h>
#ifndef AMIGA_SUPPORT
#define AMIGA_SUPPORT
#endif
#endif

#include "types.h"

/* enable memory debugging */
/* #define LIB_DEBUG */

/* enable pinpointing of memory leaks, don't forget to enable in lib.h */
/* #define LIB_DEBUG_PINPOINT */

#ifdef LIB_DEBUG
#define LIB_DEBUG_SIZE  0x10000
#define LIB_DEBUG_GUARD 0x1000

#ifdef LIB_DEBUG_PINPOINT
static char *lib_debug_filename[LIB_DEBUG_SIZE];
static unsigned int lib_debug_line[LIB_DEBUG_SIZE];
static char *lib_debug_pinpoint_filename;
static unsigned int lib_debug_pinpoint_line=0;
#endif

static void *lib_debug_address[LIB_DEBUG_SIZE];
static unsigned int lib_debug_size[LIB_DEBUG_SIZE];
static void *lib_debug_caller[LIB_DEBUG_SIZE];
static unsigned int lib_debug_initialized = 0;

#if LIB_DEBUG_GUARD > 0
static char *lib_debug_guard_base[LIB_DEBUG_SIZE];
static unsigned int lib_debug_guard_size[LIB_DEBUG_SIZE];
#endif

#ifdef __GNUC__
static void *lib_debug_func_level1(void)
{
    return __builtin_return_address(1 + 1);
}

static void *lib_debug_func_level2(void)
{
    return __builtin_return_address(2 + 1);
}

static void *lib_debug_func_level3(void)
{
    return __builtin_return_address(3 + 1);
}
#endif

static void lib_debug_init(void)
{
    memset(lib_debug_address, 0, sizeof(lib_debug_address));
    memset(lib_debug_caller, 0, sizeof(lib_debug_caller));
#if LIB_DEBUG_GUARD > 0
    memset(lib_debug_guard_base, 0, sizeof(lib_debug_guard_base));
#endif
#ifdef LIB_DEBUG_PINPOINT
    memset(lib_debug_line, 0, sizeof(lib_debug_line));
#endif
    lib_debug_initialized = 1;
}

static void lib_debug_alloc(void *ptr, size_t size, int level)
{
    unsigned int index;
    void *func = NULL;

    if (!lib_debug_initialized)
        lib_debug_init();

    index = 0;

    while (index < LIB_DEBUG_SIZE && lib_debug_address[index] != NULL)
        index++;

    if (index == LIB_DEBUG_SIZE) {
        printf("lib_debug_alloc(): Out of debug address slots.\n");
        return;
    }

#ifdef __GNUC__
    switch (level) {
      case 1:
        func = lib_debug_func_level1();
        break;
      case 2:
        func = lib_debug_func_level2();
        break;
      case 3:
        func = lib_debug_func_level3();
        break;
    }
    lib_debug_caller[index] = func;
#endif
#if 0
    if (ptr == (void *)0x85c2c80)
        *(int *)0 = 0;
#endif
#if 0
    printf("lib_debug_alloc(): Alloc address %p size %i slot %i from %p.\n",
           ptr, size, index, func);
#endif
    lib_debug_address[index] = ptr;
    lib_debug_size[index] = (unsigned int)size;
#ifdef LIB_DEBUG_PINPOINT
    lib_debug_filename[index]=lib_debug_pinpoint_filename;
    lib_debug_line[index]=lib_debug_pinpoint_line;
    lib_debug_pinpoint_line=0;
#endif
}

static void lib_debug_free(void *ptr, unsigned int level, unsigned int fill)
{
    unsigned int index;
    void *func = NULL;

    if (ptr == NULL)
        return;

    index = 0;

    while (index < LIB_DEBUG_SIZE && lib_debug_address[index] != ptr)
        index++;

    if (index == LIB_DEBUG_SIZE) {
#if 0
        printf("lib_debug_free(): Cannot find debug address!\n");
#endif
        return;
    }

#ifdef __GNUC__
    switch (level) {
      case 1:
        func = lib_debug_func_level1();
        break;
      case 2:
        func = lib_debug_func_level2();
        break;
      case 3:
        func = lib_debug_func_level3();
        break;
    }
#endif

#if 0
    printf("lib_debug_free(): Free address %p size %i slot %i from %p.\n",
           ptr, lib_debug_size[index], index, func);
#endif

    if (fill)
        memset(ptr, 0xdd, lib_debug_size[index]);

    lib_debug_address[index] = NULL;
}

/*-----------------------------------------------------------------------*/

#if LIB_DEBUG_GUARD > 0
static void lib_debug_guard_add(char *ptr, unsigned int size)
{
    unsigned int index;

    if (!lib_debug_initialized)
        lib_debug_init();

    index = 0;

    while (index < LIB_DEBUG_SIZE && lib_debug_guard_base[index] != NULL)
        index++;

    if (index == LIB_DEBUG_SIZE) {
        printf("lib_debug_guard_add(): Out of debug address slots.\n");
        return;
    }
#if 0
    printf("ADD BASE %p SLOT %d SIZE %d\n", ptr, index, size);
#endif
    lib_debug_guard_base[index] = ptr;
    lib_debug_guard_size[index] = (unsigned int)size;

    memset(ptr, 0x55, LIB_DEBUG_GUARD);
    memset(ptr + LIB_DEBUG_GUARD + size, 0x55, LIB_DEBUG_GUARD);
}

static void lib_debug_guard_remove(char *ptr)
{
    unsigned int index;
    unsigned int i;

    index = 0;

    while (index < LIB_DEBUG_SIZE
        && lib_debug_guard_base[index] != (ptr - LIB_DEBUG_GUARD))
        index++;

    if (index == LIB_DEBUG_SIZE) {
        printf("lib_debug_guard_remove(): Cannot find debug address %p!\n",
               ptr - LIB_DEBUG_GUARD);
        return;
    }

    for (i = 0; i < LIB_DEBUG_GUARD; i++) {
        if (*(ptr - LIB_DEBUG_GUARD + i) != 0x55)
            printf("Memory corruption in lower part of base %p!\n",
                   ptr - LIB_DEBUG_GUARD);
        if (*(ptr + lib_debug_guard_size[index] + i) != 0x55)
            printf("Memory corruption in higher part of base %p!\n",
                   ptr - LIB_DEBUG_GUARD);
    }
#if 0
    printf("REM BASE %p SLOT %d\n", ptr - LIB_DEBUG_GUARD, index);
#endif
    lib_debug_guard_base[index] = NULL;
}

static unsigned int lib_debug_guard_size_get(char *ptr)
{
    unsigned int index;

    index = 0;

    while (index < LIB_DEBUG_SIZE
        && lib_debug_guard_base[index] != (ptr - LIB_DEBUG_GUARD))
        index++;

    if (index == LIB_DEBUG_SIZE) {
        printf("lib_debug_guard_size(): Cannot find debug address %p!\n",
               ptr - LIB_DEBUG_GUARD);
        return 0;
    }

    return lib_debug_guard_size[index];
}
#endif

static void *lib_debug_libc_malloc(size_t size)
{
#if LIB_DEBUG_GUARD > 0
    char *ptr;

    ptr = (char *)malloc(size + 2 * LIB_DEBUG_GUARD);
    lib_debug_guard_add(ptr, (unsigned int)size);

    return (void *)(ptr + LIB_DEBUG_GUARD);
#else
    return malloc(size);
#endif
}

static void *lib_debug_libc_calloc(size_t nmemb, size_t size)
{
#if LIB_DEBUG_GUARD > 0
    char *ptr;

    ptr = (char *)malloc(nmemb * size + 2 * LIB_DEBUG_GUARD);
    lib_debug_guard_add(ptr, (unsigned int)(nmemb * size));

    memset(ptr + LIB_DEBUG_GUARD, 0, nmemb * size);

    return (void *)(ptr + LIB_DEBUG_GUARD);
#else
    return calloc(nmemb, size);
#endif
}

static void lib_debug_libc_free(void *ptr)
{
#if LIB_DEBUG_GUARD > 0
    if (ptr != NULL) {
        lib_debug_guard_remove((char *)ptr);
        free((char *)ptr - LIB_DEBUG_GUARD);
    }
#else
    free(ptr);
#endif
}

static void *lib_debug_libc_realloc(void *ptr, size_t size)
{
#if LIB_DEBUG_GUARD > 0
    char *new_ptr = NULL;

    if (size > 0) {
        new_ptr = lib_debug_libc_malloc(size);

        if (ptr != NULL) {
            size_t old_size;

            old_size = (size_t)lib_debug_guard_size_get((char *)ptr);

            if (size >= old_size)
                memcpy(new_ptr, ptr, old_size);
            else
                memcpy(new_ptr, ptr, size);
        }
    }

    if (ptr != NULL)
        lib_debug_libc_free(ptr);

    return (void *)new_ptr;
#else
    return realloc(ptr, size);
#endif
}
#endif

/*-----------------------------------------------------------------------*/

void lib_debug_check(void)
{
#ifdef LIB_DEBUG
    unsigned int index, count;

    count = 0;

    for (index = 0; index < LIB_DEBUG_SIZE; index++) {
        if (lib_debug_address[index] != NULL) {
            printf("Memory leak %i at %p with size %i from %p.\n", ++count,
                   lib_debug_address[index], lib_debug_size[index],
                   lib_debug_caller[index]);
#ifdef LIB_DEBUG_PINPOINT
            if (lib_debug_line[index]!=0)
              printf("file : %s, line : %d\n", lib_debug_filename[index],
                                               lib_debug_line[index]);
#endif

#if LIB_DEBUG_GUARD > 0
            lib_debug_guard_remove((char *)lib_debug_address[index]);
#endif
        }
    }
#endif
}

/*-----------------------------------------------------------------------*/

void *lib_malloc(size_t size)
{
#ifdef LIB_DEBUG
    void *ptr = lib_debug_libc_malloc(size);
#else
    void *ptr = malloc(size);
#endif

#ifndef __OS2__
    if (ptr == NULL && size > 0)
        exit(-1);
#endif
#ifdef LIB_DEBUG
    lib_debug_alloc(ptr, size, 3);
#endif

    return ptr;
}

#ifdef AMIGA_SUPPORT
void *lib_AllocVec(unsigned long size, unsigned long attributes)
{
#ifdef LIB_DEBUG
    void *ptr;

    if (attributes & MEMF_CLEAR)
        ptr = lib_debug_libc_calloc(1, size);
    else
        ptr = lib_debug_libc_malloc(size);
#else
    void *ptr = AllocVec(size, attributes);
#endif

#ifndef __OS2__
    if (ptr == NULL && size > 0)
        exit(-1);
#endif
#ifdef LIB_DEBUG
    lib_debug_alloc(ptr, size, 1);
#endif

    return ptr;
}

void *lib_AllocMem(unsigned long size, unsigned long attributes)
{
#ifdef LIB_DEBUG
    void *ptr;

    if (attributes & MEMF_CLEAR)
        ptr = lib_debug_libc_calloc(1, size);
    else
        ptr = lib_debug_libc_malloc(size);
#else
    void *ptr = AllocMem(size, attributes);
#endif

#ifndef __OS2__
    if (ptr == NULL && size > 0)
        exit(-1);
#endif
#ifdef LIB_DEBUG
    lib_debug_alloc(ptr, size, 1);
#endif

    return ptr;
}
#endif

/* Like calloc, but abort if not enough memory is available.  */
void *lib_calloc(size_t nmemb, size_t size)
{
#ifdef LIB_DEBUG
    void *ptr = lib_debug_libc_calloc(nmemb, size);
#else
    void *ptr = calloc(nmemb, size);
#endif

#ifndef __OS2__
    if (ptr == NULL && (size * nmemb) > 0)
        exit(-1);
#endif
#ifdef LIB_DEBUG
    lib_debug_alloc(ptr, size * nmemb, 1);
#endif

    return ptr;
}

/* Like realloc, but abort if not enough memory is available.  */
void *lib_realloc(void *ptr, size_t size)
{
#ifdef LIB_DEBUG
    void *new_ptr = lib_debug_libc_realloc(ptr, size);
#else
    void *new_ptr = realloc(ptr, size);
#endif

#ifndef __OS2__
    if (new_ptr == NULL)
        exit(-1);
#endif
#ifdef LIB_DEBUG
    lib_debug_free(ptr, 1, 0);
    lib_debug_alloc(new_ptr, size, 1);
#endif

    return new_ptr;
}

void lib_free(const void *constptr)
{
    void * ptr = (void*) constptr;
#ifdef LIB_DEBUG
    lib_debug_free(ptr, 1, 1);
#endif

#ifdef LIB_DEBUG
    lib_debug_libc_free(ptr);
#else
    free(ptr);
#endif
}

#ifdef AMIGA_SUPPORT
void lib_FreeVec(void *ptr)
{
#ifdef LIB_DEBUG
    lib_debug_free(ptr, 1, 1);
    lib_debug_libc_free(ptr);
#else
    FreeVec(ptr);
#endif
}

void lib_FreeMem(void *ptr, unsigned long size)
{
#ifdef LIB_DEBUG
    lib_debug_free(ptr, 1, 1);
    lib_debug_libc_free(ptr);
#else
    FreeMem(ptr, size);
#endif
}
#endif

/*-----------------------------------------------------------------------*/

/* Malloc enough space for `str', copy `str' into it and return its
   address.  */
char *lib_stralloc(const char *str)
{
    size_t size;
    char *ptr;

    if (str == NULL)
        exit(-1);

    size = strlen(str) + 1;
    ptr = (char *)lib_malloc(size);

    memcpy(ptr, str, size);
    return ptr;
}

/* xmsprintf() is like sprintf() but lib_malloc's the buffer by itself.  */

#define xmvsprintf_is_digit(c) ((c) >= '0' && (c) <= '9')

static int xmvsprintf_skip_atoi(const char **s)
{
    int i = 0;

    while (xmvsprintf_is_digit(**s))
        i = i * 10 + *((*s)++) - '0';
    return i;
}

#define ZEROPAD 1               /* pad with zero */
#define SIGN    2               /* unsigned/signed long */
#define PLUS    4               /* show plus */
#define SPACE   8               /* space if plus */
#define LEFT    16              /* left justified */
#define SPECIAL 32              /* 0x */
#define LARGE   64              /* use 'ABCDEF' instead of 'abcdef' */

static inline int xmvsprintf_do_div(long *n, unsigned int base)
{
    int res;

    res = ((unsigned long)*n) % (unsigned)base;
    *n = ((unsigned long)*n) / (unsigned)base;
    return res;
}

static size_t xmvsprintf_strnlen(const char * s, size_t count)
{
    const char *sc;

    for (sc = s; count-- && *sc != '\0'; ++sc)
        /* nothing */;
    return sc - s;
}

static void xmvsprintf_add(char **buf, unsigned int *bufsize,
                           unsigned int *position, char write)
{
    if (*position == *bufsize) {
        *bufsize *= 2;
        *buf = lib_realloc(*buf, *bufsize);
    }
    (*buf)[*position] = write;
    *position += 1;
}

static void xmvsprintf_number(char **buf, unsigned int *bufsize,
                              unsigned int *position, long num, int base,
                              int size, int precision, int type)
{
    char c, sign, tmp[66];
    const char *digits = "0123456789abcdefghijklmnopqrstuvwxyz";
    int i;

    if (type & LARGE)
        digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    if (type & LEFT)
        type &= ~ZEROPAD;
    if (base < 2 || base > 36)
        return;
    c = (type & ZEROPAD) ? '0' : ' ';
    sign = 0;
    if (type & SIGN) {
        if (num < 0) {
            sign = '-';
            num = -num;
            size--;
        } else if (type & PLUS) {
            sign = '+';
            size--;
        } else if (type & SPACE) {
            sign = ' ';
            size--;
        }
    }
    if (type & SPECIAL) {
        if (base == 16)
            size -= 2;
        else if (base == 8)
            size--;
    }

    i = 0;
    if (num == 0)
        tmp[i++] = '0';
    else while (num != 0)
        tmp[i++] = digits[xmvsprintf_do_div(&num, base)];
    if (i > precision)
        precision = i;
        size -= precision;
    if (!(type & (ZEROPAD + LEFT)))
        while(size-->0)
            xmvsprintf_add(buf, bufsize, position, ' ');
    if (sign)
        xmvsprintf_add(buf, bufsize, position, sign);
    if (type & SPECIAL) {
        if (base == 8)
            xmvsprintf_add(buf, bufsize, position, '0');
        else if (base == 16) {
            xmvsprintf_add(buf, bufsize, position, '0');
            xmvsprintf_add(buf, bufsize, position, digits[33]);
        }
    }
    if (!(type & LEFT))
        while (size-- > 0)
            xmvsprintf_add(buf, bufsize, position, c);
    while (i < precision--)
        xmvsprintf_add(buf, bufsize, position, '0');
    while (i-- > 0)
        xmvsprintf_add(buf, bufsize, position, tmp[i]);
    while (size-- > 0)
        xmvsprintf_add(buf, bufsize, position, ' ');
}

char *lib_mvsprintf(const char *fmt, va_list args)
{
    char *buf;
    unsigned int position, bufsize;

    int len, i, base;
    unsigned long num;
    const char *s;

    int flags;        /* flags to number() */
    int field_width;  /* width of output field */
    int precision;    /* min. # of digits for integers; max
                         number of chars for from string */
    int qualifier;    /* 'h', 'l', or 'L' for integer fields */

    /* Setup the initial buffer.  */
    buf = lib_malloc(10);
    position = 0;
    bufsize = 10;

    for ( ; *fmt ; ++fmt) {
        if (*fmt != '%') {
            xmvsprintf_add(&buf, &bufsize, &position, *fmt);
            continue;
        }

        /* process flags */
        flags = 0;
repeat:
        ++fmt;  /* this also skips first '%' */
        switch (*fmt) {
          case '-':
            flags |= LEFT;
            goto repeat;
          case '+':
            flags |= PLUS;
            goto repeat;
          case ' ':
            flags |= SPACE;
            goto repeat;
          case '#':
            flags |= SPECIAL;
            goto repeat;
          case '0':
            flags |= ZEROPAD;
            goto repeat;
        }

        /* get field width */
        field_width = -1;
        if (xmvsprintf_is_digit(*fmt))
            field_width = xmvsprintf_skip_atoi(&fmt);
        else if (*fmt == '*') {
            ++fmt;
            /* it's the next argument */
            field_width = va_arg(args, int);
            if (field_width < 0) {
                field_width = -field_width;
                flags |= LEFT;
            }
        }

        /* get the precision */
        precision = -1;
        if (*fmt == '.') {
            ++fmt;
            if (xmvsprintf_is_digit(*fmt))
                precision = xmvsprintf_skip_atoi(&fmt);
            else if (*fmt == '*') {
                ++fmt;
                 /* it's the next argument */
                 precision = va_arg(args, int);
            }
            if (precision < 0)
                precision = 0;
        }

        /* get the conversion qualifier */
        qualifier = -1;
        if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L') {
            qualifier = *fmt;
            ++fmt;
        }

        /* default base */
        base = 10;

        switch (*fmt) {
          case 'c':
            if (!(flags & LEFT))
                while (--field_width > 0)
                    xmvsprintf_add(&buf, &bufsize, &position, ' ');
            xmvsprintf_add(&buf, &bufsize, &position,
                           (unsigned char) va_arg(args, int));
            while (--field_width > 0)
                xmvsprintf_add(&buf, &bufsize, &position, ' ');
            continue;

          case 's':
            s = va_arg(args, char *);
            if (!s)
                s = "<NULL>";

            len = xmvsprintf_strnlen(s, precision);

            if (!(flags & LEFT))
                while (len < field_width--)
                    xmvsprintf_add(&buf, &bufsize, &position, ' ');
            for (i = 0; i < len; ++i)
                xmvsprintf_add(&buf, &bufsize, &position, *s++);
            while (len < field_width--)
                xmvsprintf_add(&buf, &bufsize, &position, ' ');
            continue;

          case 'p':
            if (field_width == -1) {
                field_width = 2*sizeof(void *);
                flags |= ZEROPAD;
            }
            xmvsprintf_number(&buf, &bufsize, &position,
                              (unsigned long) va_arg(args, void *), 16,
                              field_width, precision, flags);
            continue;

          case '%':
            xmvsprintf_add(&buf, &bufsize, &position, '%');
            continue;

          /* integer number formats - set up the flags and "break" */
          case 'o':
            base = 8;
            break;
          case 'X':
            flags |= LARGE;
          case 'x':
            base = 16;
            break;
          case 'd':
          case 'i':
            flags |= SIGN;
          case 'u':
            break;

          default:
            xmvsprintf_add(&buf, &bufsize, &position, '%');
            if (*fmt)
                xmvsprintf_add(&buf, &bufsize, &position, *fmt);
            else
                --fmt;
            continue;
        }
        if (qualifier == 'l')
            num = va_arg(args, unsigned long);
        else if (qualifier == 'h') {
            num = (unsigned short) va_arg(args, int);
            if (flags & SIGN)
            num = (short) num;
        } else if (flags & SIGN)
            num = va_arg(args, int);
        else
            num = va_arg(args, unsigned int);
        xmvsprintf_number(&buf, &bufsize, &position, num, base, field_width,
                          precision, flags);
    }
    xmvsprintf_add(&buf, &bufsize, &position, '\0');

    /* Trim buffer to final size.  */
    buf = lib_realloc(buf, strlen(buf) + 1);

    return buf;
}

char *lib_msprintf(const char *fmt, ...)
{
    va_list args;
    char *buf;

    va_start(args, fmt);
    buf = lib_mvsprintf(fmt, args);
    va_end(args);

    return buf;
}

#ifdef LIB_DEBUG_PINPOINT
void *lib_malloc_pinpoint(size_t size, char *name, unsigned int line)
{
  lib_debug_pinpoint_filename=name;
  lib_debug_pinpoint_line=line;
  return lib_malloc(size);
}

void *lib_calloc_pinpoint(size_t nmemb, size_t size, char *name, unsigned int line)
{
  lib_debug_pinpoint_filename=name;
  lib_debug_pinpoint_line=line;
  return lib_calloc(nmemb, size);
}

void *lib_realloc_pinpoint(void *p, size_t size, char *name, unsigned int line)
{
  lib_debug_pinpoint_filename=name;
  lib_debug_pinpoint_line=line;
  return lib_realloc(p, size);
}

char *lib_stralloc_pinpoint(const char *str, char *name, unsigned int line)
{
  lib_debug_pinpoint_filename=name;
  lib_debug_pinpoint_line=line;
  return lib_stralloc(str);
}

#ifdef AMIGA_SUPPORT
void *lib_AllocVec_pinpoint(unsigned long size, unsigned long attributes, char *name, unsigned int line)
{
  lib_debug_pinpoint_filename=name;
  lib_debug_pinpoint_line=line;
  return lib_AllocVec(size, attributes);
}

void lib_FreeVec_pinpoint(void *ptr, char *name, unsigned int line)
{
  lib_debug_pinpoint_filename=name;
  lib_debug_pinpoint_line=line;
  return lib_FreeVec(ptr);
}

void *lib_AllocMem_pinpoint(unsigned long size, unsigned long attributes, char *name, unsigned int line)
{
  lib_debug_pinpoint_filename=name;
  lib_debug_pinpoint_line=line;
  return lib_AllocMem(size, attributes);
}

void lib_FreeMem_pinpoint(void *ptr, unsigned long size, char *name, unsigned int line)
{
  lib_debug_pinpoint_filename=name;
  lib_debug_pinpoint_line=line;
  return lib_FreeMem(ptr, size);
}
#endif
#endif
