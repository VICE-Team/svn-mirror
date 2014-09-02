/*
 * cbm2-common-resources.c - CBM-5x0/6x0/7x0 resources.
 *
 * Written by
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

static const resource_string_t resources_string[] = {
#ifdef COMMON_KBD
    { "KeymapBusinessUKSymFile", KBD_CBM2_SYM_UK, RES_EVENT_NO, NULL,
      &machine_keymap_file_list[0], keyboard_set_keymap_file, (void *)0 },
    { "KeymapBusinessUKPosFile", KBD_CBM2_POS_UK, RES_EVENT_NO, NULL,
      &machine_keymap_file_list[1], keyboard_set_keymap_file, (void *)1 },
    { "KeymapGraphicsSymFile", KBD_CBM2_SYM_GR, RES_EVENT_NO, NULL,
      &machine_keymap_file_list[2], keyboard_set_keymap_file, (void *)2 },
    { "KeymapGraphicsPosFile", KBD_CBM2_POS_GR, RES_EVENT_NO, NULL,
      &machine_keymap_file_list[3], keyboard_set_keymap_file, (void *)3 },
    { "KeymapBusinessDESymFile", KBD_CBM2_SYM_DE, RES_EVENT_NO, NULL,
      &machine_keymap_file_list[4], keyboard_set_keymap_file, (void *)4 },
    { "KeymapBusinessDEPosFile", KBD_CBM2_POS_DE, RES_EVENT_NO, NULL,
      &machine_keymap_file_list[5], keyboard_set_keymap_file, (void *)5 },
#endif
    { NULL }
};

static const resource_int_t resources_int[] = {
    { "CIA1Model", CIA_MODEL_6526, RES_EVENT_SAME, NULL,
      &cia1_model, set_cia1_model, NULL },
#ifdef COMMON_KBD
    { "KeymapIndex", KBD_INDEX_CBM2_DEFAULT, RES_EVENT_NO, NULL,
      &machine_keymap_index, keyboard_set_keymap_index, NULL },
#endif
    { NULL }
};
