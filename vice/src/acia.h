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

#define   ACIA_DR   0		/* Data register */
#define   ACIA_SR   1		/* R: status register W: programmed Reset */
#define   ACIA_CMD  2		/* Command register */
#define   ACIA_CTRL 3		/* Control register */

