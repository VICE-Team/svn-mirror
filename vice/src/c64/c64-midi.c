/*
 * c64-midi.c - C64 specific MIDI (6850 UART) emulation.
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

#include "vice.h"

#ifdef HAVE_MIDI

#include "types.h"

#include "c64-midi.h"
#include "cmdline.h"
#include "machine.h"
#include "resources.h"
#include "translate.h"

midi_interface_t midi_interface[] = {
    /* Sequential Circuits Inc. */
    { "Sequential", 0xde00, 0, 2, 1, 3, 0xff, 1, 1 },
    /* Passport & Syntech */
    { "Passport", 0xde00, 8, 8, 9, 9, 0xff, 1, 1 },
    /* DATEL/Siel/JMS */
    { "DATEL", 0xde00, 4, 6, 5, 7, 0xff, 2, 1 },
    /* Namesoft */
    { "Namesoft", 0xde00, 0, 2, 1, 3, 0xff, 1, 2 },
    /* Electronics - Maplin magazine */
    { "Maplin", 0xdf00, 0, 0, 1, 1, 0xff, 2, 0 },
    { NULL}
};

static const resource_int_t resources_int[] = {
    { "MIDIMode", MIDI_MODE_SEQUENTIAL, RES_EVENT_NO, NULL,
      &midi_mode, midi_set_mode, NULL },
    { NULL }
};

int c64_midi_resources_init(void)
{
    if (resources_register_int(resources_int) < 0) {
        return -1;
    }

    return midi_resources_init();
}

static const cmdline_option_t cmdline_options[] = {
    { "-miditype", SET_RESOURCE, 1,
      NULL, NULL, "MIDIMode", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_SPECIFY_C64_MIDI_TYPE,
      "<0-4>", NULL },
    { NULL }
};

int c64_midi_cmdline_options_init(void)
{
    if (cmdline_register_options(cmdline_options) < 0) {
        return -1;
    }

    return midi_cmdline_options_init();
}

int REGPARM1 c64_midi_base_de00(void)
{
    return (midi_interface[midi_mode].base_addr == 0xde00)?1:0;
}
#endif
