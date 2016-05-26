/*
 * io-access.c - Unix specific I/O code.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
 *
 * This file is part of VICE, modified from the sidplay2 sources.  It is
 * a one for all driver with real timing support via real time kernel
 * extensions or through the hardware buffering.  It supports the hardsid
 * isa/pci single/quattro and also the catweasel MK3/4.
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

#include "types.h"

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

void io_access_store(WORD addr, BYTE value)
{
#ifdef HAVE_MMAP_DEVICE_IO
    out8(addr, value);
#endif
#if defined(HAVE_LIBAMD64) || defined(HAVE_I386_SET_IOPERM)
#ifdef __NetBSD__
    vice_outb(addr, value);
#else
    outb(addr, value);
#endif
#endif
#ifdef HAVE_IOPERM
#ifndef HAVE_OUTB_P
    outb(value, addr);
#else
    outb_p(value, addr);
#endif
#endif
}

BYTE io_access_read(WORD addr)
{
#ifdef HAVE_MMAP_DEVICE_IO
    return in8(addr);
#endif
#if defined(HAVE_LIBAMD64) || defined(HAVE_I386_SET_IOPERM)
#ifdef __NetBSD__
    return vice_inb(addr);
#else
    return inb(addr);
#endif
#endif
#ifdef HAVE_IOPERM
#ifndef HAVE_INB_P
    return inb(addr);
#else
    return inb_p(addr);
#endif
#endif
}

int io_access_map(WORD addr, WORD space)
{
#if defined(HAVE_LIBAMD64) || defined(HAVE_I386_SET_IOPERM)
#  ifndef __FreeBSD__
    int i;
    u_long iomap[32];
#  endif
#endif

#ifdef HAVE_MMAP_DEVICE_IO
    if (mmap_device_io(space, addr) != MAP_FAILED) {
        return 0;
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
        for (i = 0; i < space; ++i) {
            setaccess(iomap, addr + i, 1);
        }
#    ifdef HAVE_LIBAMD64
        if (vice_amd64_set_ioperm(iomap) != -1)
#    else
        if (vice_i386_set_ioperm(iomap) != -1)
#    endif
        {
            return 0;
        }
    }
#  else
    if (vice_i386_set_ioperm(addr, space, 1) != -1) {
        return 0;
    }
#  endif
#endif

#ifdef HAVE_IOPERM
    if (ioperm(addr, space, 1) != -1) {
       return 0;
    }
#endif

    return -1;
}

void io_access_unmap(WORD addr, WORD space)
{
#if defined(HAVE_LIBAMD64) || defined(HAVE_I386_SET_IOPERM)
#  ifndef __FreeBSD__
    int i;
    u_long iomap[32];
#  endif
#endif

#ifdef HAVE_MMAP_DEVICE_IO
    mmunmap_device_io(space, addr);
#endif

#if defined(HAVE_LIBAMD64) || defined(HAVE_I386_SET_IOPERM)
#  ifndef __FreeBSD__
#    ifdef HAVE_LIBAMD64
    if (vice_amd64_get_ioperm(iomap) != -1)
#    else
    if (vice_i386_get_ioperm(iomap) != -1)
#    endif
    {
        for (i = 0; i < space; ++i) {
            setaccess(iomap, addr + i, 0);
        }
#    ifdef HAVE_LIBAMD64
        vice_amd64_set_ioperm(iomap);
#    else
        vice_i386_set_ioperm(iomap);
#    endif
    }
#  else
    vice_i386_set_ioperm(addr, space, 0);
#  endif
#endif

#ifdef HAVE_IOPERM
    ioperm(addr, space, 0);
#endif
    return 0;
}
#endif
