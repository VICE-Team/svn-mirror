/*
 * hs-isa.c - Unix specific ISA hardsid driver.
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

#if defined(HAVE_HARDSID) && defined(HAVE_HARDSID_ISA)

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

#include "hardsid.h"
#include "types.h"

#define HARDSID_BASE 0x300

static int hardsid_open_status = 0;
static int hs_available = 0;

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

void hs_isa_store(WORD addr, BYTE value, int chipno)
{
#ifdef HAVE_MMAP_DEVICE_IO
    out8(HARDSID_BASE, value);
    out8(HARDSID_BASE + 1, addr & 0x1f);
#endif
#if defined(HAVE_LIBAMD64) || defined(HAVE_I386_SET_IOPERM)
#ifdef __NetBSD__
    vice_outb(HARDSID_BASE, value);
    vice_outb(HARDSID_BASE + 1, addr & 0x1f);
#else
    outb(HARDSID_BASE, value);
    outb(HARDSID_BASE + 1, addr & 0x1f);
#endif
#endif
#ifdef HAVE_IOPERM
#ifndef HAVE_OUTB_P
    outb(value, HARDSID_BASE);
    outb(addr & 0x1f, HARDSID_BASE + 1);
#else
    outb_p(value, HARDSID_BASE);
    outb_p(addr & 0x1f, HARDSID_BASE + 1);
#endif
#endif
}

BYTE hs_isa_read(WORD addr, int chipno)
{
#ifdef HAVE_MMAP_DEVICE_IO
    out8(HARDSID_BASE + 1, (addr & 0x1f) | 0x20);
    usleep(2);
    return in8(HARDSID_BASE);
#endif
#if defined(HAVE_LIBAMD64) || defined(HAVE_I386_SET_IOPERM)
#ifdef __NetBSD__
    vice_outb(HARDSID_BASE + 1, (addr & 0x1f) | 0x20);
    usleep(2);
    return vice_inb(HARDSID_BASE);
#else
    outb(HARDSID_BASE + 1, (addr & 0x1f) | 0x20);
    usleep(2);
    return inb(HARDSID_BASE);
#endif
#endif
#ifdef HAVE_IOPERM
#ifndef HAVE_INB_P
    outb((addr & 0x1f) | 0x20, HARDSID_BASE + 1);
    usleep(2);
    return inb(HARDSID_BASE);
#else
    outb_p((addr & 0x1f) | 0x20, HARDSID_BASE + 1);
    usleep(2);
    return inb_p(HARDSID_BASE);
#endif
#endif
}

static int detect_sid(void)
{
    int i;

    for (i = 0x18; i >= 0; --i) {
        hs_isa_store((WORD)i, 0, 0);
    }

    hs_isa_store(0x12, 0xff, 0);

    for (i = 0; i < 100; ++i) {
        if (hs_isa_read(0x1b, 0)) {
            return 0;
        }
    }

    hs_isa_store(0x0e, 0xff, 0);
    hs_isa_store(0x0f, 0xff, 0);
    hs_isa_store(0x12, 0x20, 0);

    for (i = 0; i < 100; ++i) {
        if (hs_isa_read(0x1b, 0)) {
            return 1;
        }
    }
    return 0;
}

int hs_isa_open(void)
{
#if defined(HAVE_LIBAMD64) || defined(HAVE_I386_SET_IOPERM)
#  ifndef __FreeBSD__
    int i;
    u_long iomap[32];
#  endif
#endif

#ifdef HAVE_MMAP_DEVICE_IO
    if (mmap_device_io(2, HARDSID_BASE) != MAP_FAILED) {
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
        for (i = 0; i < 2; ++i) {
            setaccess(iomap, HARDSID_BASE + i, 1);
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
    if (vice_i386_set_ioperm(HARDSID_BASE, 2, 1) == 0) {
        return -1;
    }
#  endif
#endif

#ifdef HAVE_IOPERM
    if (ioperm(HARDSID_BASE, 2, 1) == 0) {
       return -1;
    }
#endif

    if (detect_sid()) {
        return 0;
    }
    return -1;
}

int hs_isa_close(void)
{
#if defined(HAVE_LIBAMD64) || defined(HAVE_I386_SET_IOPERM)
#  ifndef __FreeBSD__
    int i;
    u_long iomap[32];
#  endif
#endif

#ifdef HAVE_MMAP_DEVICE_IO
    mmunmap_device_io(2, HARDSID_BASE);
#endif

#if defined(HAVE_LIBAMD64) || defined(HAVE_I386_SET_IOPERM)
#  ifndef __FreeBSD__
#    ifdef HAVE_LIBAMD64
    if (vice_amd64_get_ioperm(iomap) != -1)
#    else
    if (vice_i386_get_ioperm(iomap) != -1)
#    endif
    {
        for (i = 0; i < 2; ++i) {
            setaccess(iomap, HARDSID_BASE + i, 0);
        }
#    ifdef HAVE_LIBAMD64
        vice_amd64_set_ioperm(iomap);
#    else
        vice_i386_set_ioperm(iomap);
#    endif
    }
#  else
    vice_i386_set_ioperm(HARDSID_BASE, 2, 0);
#  endif
#endif

#ifdef HAVE_IOPERM
    ioperm(HARDSID_BASE, 2, 0);
#endif
    return 0;
}

int hs_isa_available(void)
{
    if (hs_available) {
        return 1;
    }

    if (hs_isa_open() < 0) {
        return 0;
    }
    hs_isa_close();
    hs_available = 1;
    return 1;
}

/* ---------------------------------------------------------------------*/

void hs_isa_state_read(int chipno, struct sid_hs_snapshot_state_s *sid_state)
{
    sid_state->hsid_main_clk = 0;
    sid_state->hsid_alarm_clk = 0;
    sid_state->lastaccess_clk = 0;
    sid_state->lastaccess_ms = 0;
    sid_state->lastaccess_chipno = 0;
    sid_state->chipused = 0;
    sid_state->device_map[0] = 0;
    sid_state->device_map[1] = 0;
}

void hs_isa_state_write(int chipno, struct sid_hs_snapshot_state_s *sid_state)
{
}
#endif
