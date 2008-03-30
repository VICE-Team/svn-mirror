/*
 *
 * This file is part of Commodore 64 emulator.
 * See README for copyright notice
 *
 * ACIA 6551 register number declarations.
 * Interrupt signals are defined in vmachine.h.
 *
 * Written by
 *    Andre Fachat (a.fachat@physik.tu-chemnitz.de)
 *
 */

extern BYTE REGPARM1 read_acia1(ADDRESS a);
extern BYTE REGPARM1 peek_acia1(ADDRESS a);
extern void REGPARM2 store_acia1(ADDRESS a,BYTE b);
extern void reset_acia1(void);
extern int int_acia1(long offset);

extern int acia1_init_cmdline_options(void);
extern int acia1_init_resources(void);

extern int acia1_read_snapshot_module(snapshot_t *);
extern int acia1_write_snapshot_module(snapshot_t *);

