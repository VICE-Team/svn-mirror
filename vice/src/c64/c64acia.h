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

#ifndef _C64ACIA_H
#define _C64ACIA_H

#include "types.h"

struct snapshot_s;

extern void acia1_init(void);
extern void acia1_reset(void);
extern BYTE REGPARM1 acia1_read(ADDRESS a);
extern BYTE REGPARM1 acia1_peek(ADDRESS a);
extern void REGPARM2 acia1_store(ADDRESS a, BYTE b);

extern int acia1_cmdline_options_init(void);
extern int acia1_resources_init(void);

extern int acia1_snapshot_write_module(struct snapshot_s *p);
extern int acia1_snapshot_read_module(struct snapshot_s *p);

#endif

