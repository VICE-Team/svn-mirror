/*
 *
 * This file is part of Commodore 64 emulator.
 * See README for copyright notice
 *
 * ACIA 6551 register number declarations.
 * Interrupt signals are defined in interrupt.h.
 *
 * Written by
 *    Andre Fachat <a.fachat@physik.tu-chemnitz.de>
 *
 */

#include "snapshot.h"
#include "types.h"

#ifndef _C64ACIA_H
#define _C64ACIA_H

extern void acia1_init(void);
extern void acia1_reset(void);
extern BYTE REGPARM1 acia1_read(ADDRESS a);
extern BYTE REGPARM1 acia1_peek(ADDRESS a);
extern void REGPARM2 acia1_store(ADDRESS a,BYTE b);

extern int acia1_init_cmdline_options(void);
extern int acia1_init_resources(void);

extern int acia1_write_snapshot_module(snapshot_t *p);
extern int acia1_read_snapshot_module(snapshot_t *p);

#endif

