/*
 * c64-midi.h - C64 specific MIDI (6850 UART) emulation.
 *
 * Written by
 *  Hannu Nuotio <hannu.nuotio@tut.fi>
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

#ifndef _C64_MIDI_H
#define _C64_MIDI_H

#include "midi.h"

/* returns 1 if interface is at $de00 (on C64) */
extern int REGPARM1 c64_midi_base_de00(void);

extern int c64_midi_resources_init(void);
extern int c64_midi_cmdline_options_init(void);

/* Emulated interfaces */
enum { MIDI_MODE_SEQUENTIAL = 0,   /* Sequential Circuits Inc. */
       MIDI_MODE_PASSPORT,         /* Passport & Syntech */
       MIDI_MODE_DATEL,            /* DATEL/Siel/JMS */
       MIDI_MODE_NAMESOFT,         /* Namesoft */
       MIDI_MODE_MAPLIN            /* Electronics - Maplin magazine */
};

#endif
