/*
 * ssi2001-drv.c - Unix specific SSI2001 (ISA SID card) driver.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

#ifdef HAVE_SSI2001

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ioctl.h>

#if defined(HAVE_SYS_MMAN_H) && defined(HAVE_HW_INOUT_H)
#include <sys/mman.h>
#include <hw/inout.h>
#endif

#ifdef HAVE_MACHINE_SYSARCH_H
#include <machine/sysarch.h>
#endif

#ifdef HAVE_SYS_IO_H
#include <sys/io.h>
#endif

#ifdef HAVE_MACHINE_PIO_H
#include <machine/pio.h>
#endif

#ifdef HAVE_MACHINE_CPUFUNC_H
#include <machine/cpufunc.h>
#endif

#include "ssi2001.h"
#include "types.h"

#define SSI2008_BASE 0x280

static int ssi2001_open_status = 0;

typedef void (*voidfunc_t)(void);

#if defined(HAVE_LIBAMD64) || defined(HAVE_I386_SET_IOPERM)
#ifndef __FreeBSD__
static void setaccess(u_long * map, u_int bit, int allow)
{
    u_int word;
    u_int shift;
    u_long mask;

    word = bit / 32;
    shift = bit - (word * 32);

    mask = 0x000000001 << shift;
    if (allow) {
        map[word] &= ~mask;
    } else {
        map[word] |= mask;
    }
}
#endif
#if defined(__NetBSD__) && defined(HAVE_I386_SET_IOPERM)
static int vice_i386_set_ioperm(unsigned long *iomap)
{
    struct i386_set_ioperm_args arg;

    arg.iomap = iomap;
    return sysarch(I386_SET_IOPERM, &arg);
}

static int vice_i386_get_ioperm(unsigned long *iomap)
{
    struct i386_get_ioperm_args arg;

    arg.iomap = iomap;
    return sysarch(I386_GET_IOPERM, &arg);
}
#endif
#if defined(__NetBSD__) && defined(HAVE_LIBAMD64)
static int vice_amd64_set_ioperm(unsigned long *iomap)
{
    struct x86_64_set_ioperm_args arg;

    arg.iomap = iomap;
    return sysarch(X86_64_SET_IOPERM, &arg);
}

static int vice_amd64_get_ioperm(unsigned long *iomap)
{
    struct x86_64_get_ioperm_args arg;

    arg.iomap = iomap;
    return sysarch(X86_64_SET_IOPERM, &arg);
}
#endif
#endif

#if defined(HAVE_LIBAMD64) || defined(HAVE_I386_SET_IOPERM)
#ifdef __NetBSD__
static inline void vice_outb(WORD port, BYTE val)
{
    asm volatile("outb %0, %1"
                 : : "a"(val), "Nd"(port));
}

static inline BYTE vice_inb(WORD port)
{
    BYTE ret;

    asm volatile("inb %1, %0"
                 : "=a"(ret) : "Nd"(port));
    return ret;
}
#endif
#endif

void ssi2001_drv_store(WORD addr, BYTE value, int chipno)
{
#ifdef HAVE_MMAP_DEVICE_IO
    out8(SSI2008_BASE + (addr & 0x1f), value);
#endif
#if defined(HAVE_LIBAMD64) || defined(HAVE_I386_SET_IOPERM)
#ifdef __NetBSD__
    vice_outb(SSI2008_BASE + (addr & 0x1f), value);
#else
    outb(SSI2008_BASE + (addr & 0x1f), value);
#endif
#endif
#ifdef HAVE_IOPERM
#ifndef HAVE_OUTB_P
    outb(value, SSI2008_BASE + (addr & 0x1f));
#else
    outb_p(value, SSI2008_BASE + (addr & 0x1f));
#endif
#endif
}

BYTE ssi2001_drv_read(WORD addr, int chipno)
{
#ifdef HAVE_MMAP_DEVICE_IO
    return in8(SSI2008_BASE + (addr & 0x1f));
#endif
#if defined(HAVE_LIBAMD64) || defined(HAVE_I386_SET_IOPERM)
#ifdef __NetBSD__
    return vice_inb(SSI2008_BASE + (addr & 0x1f));
#else
    return inb(SSI2008_BASE + (addr & 0x1f));
#endif
#endif
#ifdef HAVE_IOPERM
#ifndef HAVE_INB_P
    return inb(SSI2008_BASE + (addr & 0x1f));
#else
    return inb_p(SSI2008_BASE + (addr & 0x1f));
#endif
#endif
}

static int detect_sid(void)
{
    int i;

    for (i = 0x18; i >= 0; --i) {
        ssi2001_drv_store((WORD)i, 0, 0);
    }

    ssi2001_drv_store(0x12, 0xff, 0);

    for (i = 0; i < 100; ++i) {
        if (ssi2001_drv_read(0x1b, 0)) {
            return 0;
        }
    }

    ssi2001_drv_store(0x0e, 0xff, 0);
    ssi2001_drv_store(0x0f, 0xff, 0);
    ssi2001_drv_store(0x12, 0x20, 0);

    for (i = 0; i < 100; ++i) {
        if (ssi2001_drv_read(0x1b, 0)) {
            return 1;
        }
    }
    return 0;
}

int ssi2001_drv_open(void)
{

#if defined(HAVE_LIBAMD64) || defined(HAVE_I386_SET_IOPERM)
#  ifndef __FreeBSD__
    int i;
    u_long iomap[32];
#  endif
#endif

#ifdef HAVE_MMAP_DEVICE_IO
    if (mmap_device_io(32, SSI2008_BASE) != MAP_FAILED) {
        return -1;
    }
#endif

#if defined(HAVE_LIBAMD64) || defined(HAVE_I386_SET_IOPERM)
#  ifndef __FreeBSD__
#    ifdef HAVE_LIBAMD64
    if (vice_amd64_get_ioperm(iomap) != -1)
#    else
    if (vice_i386_get_ioperm(iomap) != -1)
#    endif
    {
        for (i = 0; i < 32; ++i) {
            setaccess(iomap, SSI2008_BASE + i, 1);
        }
#    ifdef HAVE_LIBAMD64
        if (vice_amd64_set_ioperm(iomap) != -1)
#    else
        if (vice_i386_set_ioperm(iomap) != -1)
#    endif
        {
            return -1;
        }
    }
#  else
    if (vice_i386_set_ioperm(SSI2008_BASE, 32, 1) == 0) {
        return -1;
    }
#  endif
#endif

#ifdef HAVE_IOPERM
    if (ioperm(SSI2008_BASE, 32, 1) == 0) {
       return -1;
    }
#endif

    if (detect_sid()) {
        return 0;
    }
    return -1;
}

int ssi2001_drv_close(void)
{
#if defined(HAVE_LIBAMD64) || defined(HAVE_I386_SET_IOPERM)
#  ifndef __FreeBSD__
    int i;
    u_long iomap[32];
#  endif
#endif

#ifdef HAVE_MMAP_DEVICE_IO
    mmunmap_device_io(32, SSI2008_BASE);
#endif

#if defined(HAVE_LIBAMD64) || defined(HAVE_I386_SET_IOPERM)
#  ifndef __FreeBSD__
#    ifdef HAVE_LIBAMD64
    if (vice_amd64_get_ioperm(iomap) != -1)
#    else
    if (vice_i386_get_ioperm(iomap) != -1)
#    endif
    {
        for (i = 0; i < 32; ++i) {
            setaccess(iomap, SSI2008_BASE + i, 0);
        }
#    ifdef HAVE_LIBAMD64
        vice_amd64_set_ioperm(iomap);
#    else
        vice_i386_set_ioperm(iomap);
#    endif
    }
#  else
    vice_i386_set_ioperm(SSI2008_BASE, 32, 0);
#  endif
#endif

#ifdef HAVE_IOPERM
    ioperm(SSI2008_BASE, 32, 0);
#endif
    return 0;
}
#endif
