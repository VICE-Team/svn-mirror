/*
 * rsuser.h - Daniel Dallmann's 9600 baud RS232 userport interface
 *
 * Written by
 *  André Fachat        (a.fachat@physik.tu-chemnitz.de)
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

#ifndef _RSUSER_H_
#define _RSUSER_H_

#define	RTS_OUT		0x02
#define	DTR_OUT		0x04

#define	DCD_IN		0x10
#define	CTS_IN		0x40
#define	DSR_IN		0x80

int rsuser_enabled;

void rsuser_init(long cycles_per_sec, void (*start_bit_trigger)(void),
                 void (*byte_rx_func)(BYTE));
int rsuser_init_resources(void);
int rsuser_init_cmdline_options(void);

void rsuser_tx_byte(BYTE);
void rsuser_write_ctrl(BYTE);
BYTE rsuser_read_ctrl(void);

int int_rsuser(long offset);
void rsuser_reset(void);

void rsuser_prevent_clk_overflow(CLOCK sub);

BYTE rsuser_get_rx_bit(void);
void rsuser_set_tx_bit(int);

#endif

