/*
 * vic20-midi.c - VIC20 specific MIDI (6850 UART) emulation.
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

#include "cmdline.h"
#include "machine.h"
#include "resources.h"
#include "vic20-midi.h"

midi_interface_t midi_interface[] = {
    /* Electronics - Maplin magazine */
    { "Maplin", 0x9c00, 0, 0, 1, 1, 0xff, 2, 0 },
    { NULL },
};

static const resource_int_t resources_int[] = {
    { "MIDIMode", MIDI_MODE_MAPLIN, RES_EVENT_NO, NULL,
      &midi_mode, midi_set_mode, NULL },
    { NULL }
};

int vic20_midi_resources_init(void)
{
    if (resources_register_int(resources_int) < 0) {
        return -1;
    }

    return midi_resources_init();
}

int vic20_midi_cmdline_options_init(void)
{
    return midi_cmdline_options_init();
}
#endif
