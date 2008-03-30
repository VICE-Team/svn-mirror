/*
 * pets.c - PET version and resource handling.
 *
 * Written by
 *  André Fachat (fachat@physik.tu-chemnitz.de)
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

#ifndef PET
#define	PET			/* for mkdep */
#endif

#include "vice.h"

#include <stdio.h>

#include "cmdline.h"
#include "interrupt.h"
#include "pets.h"
#include "resources.h"
#include "utils.h"
#include "vsync.h"

#define	DEBUG

extern void video_resize(void);

/*PetInfo petres; *//* resource values for the PET */

PetInfo pet;

/* ------------------------------------------------------------------------- */

#define	PET_CHARGEN_NAME	"chargen"

/*
 * table with Model information
 */
static struct {
    const char *model;
    PetInfo info;
} pet_table[] = {
    {"2001",
      {8, 0x0800, 0, 40, 0, 0, 1, 1, 0,
        PET_CHARGEN_NAME, PET_KERNAL2001NAME, NULL, NULL, NULL, NULL}},
    {"3008",
      {8, 0x0800, 0, 40, 0, 0, 1, 0, 0,
        PET_CHARGEN_NAME, PET_KERNAL3032NAME, NULL, NULL, NULL, NULL}},
    {"3016",
      {16, 0x0800, 0, 40, 0, 0, 1, 0, 0,
        PET_CHARGEN_NAME, PET_KERNAL3032NAME, NULL, NULL, NULL, NULL}},
    {"3032",
      {32, 0x0800, 0, 40, 0, 0, 1, 0, 0,
        PET_CHARGEN_NAME, PET_KERNAL3032NAME, NULL, NULL, NULL, NULL}},
    {"3032B",
      {32, 0x0800, 0, 40, 0, 0, 0, 0, 0,
        PET_CHARGEN_NAME, PET_KERNAL3032NAME, PET_EDITOR2B40NAME,
        NULL, NULL, NULL}},
    {"4016",
      {16, 0x0800, 1, 40, 0, 0, 1, 0, 0,
        PET_CHARGEN_NAME, PET_KERNAL4032NAME, NULL, NULL, NULL, NULL}},
    {"4032",
      {32, 0x0800, 1, 40, 0, 0, 1, 0, 0,
        PET_CHARGEN_NAME, PET_KERNAL4032NAME, NULL, NULL, NULL, NULL}},
    {"4032B",
      {32, 0x0800, 1, 40, 0, 0, 0, 0, 0,
        PET_CHARGEN_NAME, PET_KERNAL4032NAME, PET_EDITOR4B40NAME,
        NULL, NULL, NULL}},
    {"8032",
      {32, 0x0800, 1, 80, 0, 0, 0, 0, 0,
        PET_CHARGEN_NAME, PET_KERNAL4032NAME, PET_EDITOR4B80NAME,
        NULL, NULL, NULL}},
    {"8096",
      {96, 0x0800, 1, 80, 0, 0, 0, 0, 0,
        PET_CHARGEN_NAME, PET_KERNAL4032NAME, PET_EDITOR4B80NAME,
        NULL, NULL, NULL}},
    {"8296",
      {128, 0x0100, 1, 80, 0, 0, 0, 0, 0,
        PET_CHARGEN_NAME, PET_KERNAL4032NAME, PET_EDITOR4B80NAME,
        NULL, NULL, NULL}},
    {"SuperPET",
      {32, 0x0800, 1, 80, 0, 0, 0, 0, 1,
        PET_CHARGEN_NAME, PET_KERNAL4032NAME, PET_EDITOR4B80NAME,
        NULL, NULL, NULL}},
    {NULL}
};


int pet_set_model(const char *model_name, void *extra)
{
    int i, kindex = 0;

    i = 0;
    while (pet_table[i].model) {
	if (!strcmp(pet_table[i].model, model_name)) {
#ifdef DEBUG
	    printf("PET: setting model to PET %s\n", pet_table[i].model);
#endif
	    resources_set_value("RamSize",
                                (resource_value_t) pet_table[i].info.ramSize);
	    resources_set_value("IOSize",
                                (resource_value_t) pet_table[i].info.IOSize);
	    resources_set_value("Crtc",
                                (resource_value_t) pet_table[i].info.crtc);
	    resources_set_value("VideoSize",
                                (resource_value_t) pet_table[i].info.video);
	    resources_set_value("Ram9",
                                (resource_value_t) pet_table[i].info.mem9);
	    resources_set_value("RamA",
                                (resource_value_t) pet_table[i].info.memA);
	    resources_set_value("SuperPET",
                                (resource_value_t) pet_table[i].info.superpet);

	    resources_get_value("KeymapIndex",
                                (resource_value_t *) &kindex);
	    resources_set_value("KeymapIndex",
                                (resource_value_t) ((kindex & 1)
                                                    + 2 * pet_table[i].info.kbd_type));

	    resources_set_value("ChargenName",
                                (resource_value_t) pet_table[i].info.chargenName);
	    resources_set_value("KernalName",
                                (resource_value_t) pet_table[i].info.kernalName);
	    resources_set_value("EditorName",
                                (resource_value_t) pet_table[i].info.editorName);
	    resources_set_value("RomModule9Name",
                                (resource_value_t) pet_table[i].info.mem9name);
	    resources_set_value("RomModuleAName",
                                (resource_value_t) pet_table[i].info.memAname);
	    resources_set_value("RomModuleBName",
                                (resource_value_t) pet_table[i].info.memBname);

	    /* hm, does this belong to a resource? */
	    pet.pet2k = pet_table[i].info.pet2k;

	    mem_load();
	    suspend_speed_eval();
	    maincpu_trigger_reset();
	    return 0;
	}
	i++;
    }

    return -1;
}

/************************** PET resource handling ************************/

/* check PetInfo struct for consistency after change */

static void check_info(PetInfo * pi)
{
    if (pi->superpet) {
	pi->ramSize = 128;
	pi->map = 0;
    }

    if (pi->video == 40 || (pi->video == 0 && pi->screen_width == 40)) {
	pi->vmask = 0x3ff;
	pi->videoSize = 0x400;
    } else {
	pi->vmask = 0x7ff;
	pi->videoSize = 0x800;
    }
    if (pi->ramSize == 128) {
	pi->vmask = 0x1fff;
	pi->videoSize = 0x1000;
    }
}

/* hardware config */

static int set_iosize(resource_value_t v)
{
#ifdef DEBUG
    printf("Setting I/O size to $%x\n", (int) v);
#endif
    petres.IOSize = (int) v;

    initialize_memory();
    return 0;
}

static int set_crtc_enabled(resource_value_t v)
{
    petres.crtc = (int) v;
    return 0;
}

static int set_kbd_type(resource_value_t v)
{
    petres.kbd_type = (int) v;
    return 0;
}

static int set_ramsize(resource_value_t v)
{
    int size = (int) v;
    int i, sizes[] = {4, 8, 16, 32, 96, 128};

#ifdef DEBUG
    printf("Setting ramsize to %d kB\n", size);
#endif
    for (i = 0; i < 6; i++) {
	if (size <= sizes[i])
	    break;
    }
    if (i > 5)
	i = 5;
    size = sizes[i];

    petres.ramSize = size;
    petres.map = 0;
    if (size == 96) {
	petres.map = 1;		/* 8096 mapping */
    } else if (size == 128) {
	petres.map = 2;		/* 8296 mapping */
    }
    check_info(&petres);
    initialize_memory();

    return 0;
}

static int set_video(resource_value_t v)
{
    int col = (int) v;

    if (col != petres.video) {
	printf("Setting screen width to %d columns.\n", col);

	if (col == 0 || col == 40 || col == 80) {
	    petres.video = col;
	}
	check_info(&petres);
	initialize_memory();
	set_screen();
    }
    return 0;
}

static int set_superpet_enabled(resource_value_t v)
{
    if ((unsigned int) v < 2)
	petres.superpet = (unsigned int) v;
    initialize_memory();
    return 0;
}

static int set_ram_9_enabled(resource_value_t v)
{
    if ((unsigned int) v < 2)
	petres.mem9 = (unsigned int) v;
    initialize_memory();
    return 0;
}

static int set_ram_a_enabled(resource_value_t v)
{
    if ((unsigned int) v < 2)
	petres.memA = (unsigned int) v;
    initialize_memory();
    return 0;
}

/* ROM filenames */

static int set_chargen_rom_name(resource_value_t v)
{
    string_set(&petres.chargenName, (char *) v);
    return 0;
}

static int set_kernal_rom_name(resource_value_t v)
{
    string_set(&petres.kernalName, (char *) v);

    petres.pet2k = 0;

    return 0;
}

static int set_editor_rom_name(resource_value_t v)
{
    string_set(&petres.editorName, (char *) v);
    return 0;
}

static int set_rom_module_9_name(resource_value_t v)
{
    string_set(&petres.mem9name, (char *) v);
    return 0;
}

static int set_rom_module_a_name(resource_value_t v)
{
    string_set(&petres.memAname, (char *) v);
    return 0;
}

static int set_rom_module_b_name(resource_value_t v)
{
    string_set(&petres.memBname, (char *) v);
    return 0;
}

/* resources */

static resource_t resources[] =
{
    {"RamSize", RES_INTEGER, (resource_value_t) 32,
     (resource_value_t *) & petres.ramSize, set_ramsize},
    {"IOSize", RES_INTEGER, (resource_value_t) 0x800,
     (resource_value_t *) & petres.IOSize, set_iosize},
    {"Crtc", RES_INTEGER, (resource_value_t) 1,
     (resource_value_t *) & petres.crtc, set_crtc_enabled},
    {"VideoSize", RES_INTEGER, (resource_value_t) 1,
     (resource_value_t *) & petres.video, set_video},
/*
   { "KeyboardType", RES_INTEGER, (resource_value_t) 1,
   (resource_value_t *) &petres.kbd_type, set_kbd_type },
 */
    {"Ram9", RES_INTEGER, (resource_value_t) 0,
     (resource_value_t *) & petres.mem9, set_ram_9_enabled},
    {"RamA", RES_INTEGER, (resource_value_t) 0,
     (resource_value_t *) & petres.memA, set_ram_a_enabled},
    {"SuperPET", RES_INTEGER, (resource_value_t) 0,
     (resource_value_t *) & petres.superpet, set_superpet_enabled},

    {"ChargenName", RES_STRING, (resource_value_t) "chargen",
     (resource_value_t *) & petres.chargenName, set_chargen_rom_name},
    {"KernalName", RES_STRING, (resource_value_t) PET_KERNAL4032NAME,
     (resource_value_t *) & petres.kernalName, set_kernal_rom_name},
    {"EditorName", RES_STRING, (resource_value_t) PET_EDITOR4B80NAME,
     (resource_value_t *) & petres.editorName, set_editor_rom_name},

    {"RomModule9Name", RES_STRING, (resource_value_t) NULL,
     (resource_value_t *) & petres.mem9name, set_rom_module_9_name},
    {"RomModuleAName", RES_STRING, (resource_value_t) NULL,
     (resource_value_t *) & petres.memAname, set_rom_module_a_name},
    {"RomModuleBName", RES_STRING, (resource_value_t) NULL,
     (resource_value_t *) & petres.memBname, set_rom_module_b_name},
    {NULL}
};

int pet_init_resources(void)
{
    return resources_register(resources);
}

/* ------------------------------------------------------------------------- */


/* PET-specific command-line options.  */

static cmdline_option_t cmdline_options[] = {
    {"-model", CALL_FUNCTION, 1, pet_set_model, NULL, NULL, NULL,
     "<modelnumber>", "Specify PET model to emulate"},
    {"-kernal", SET_RESOURCE, 1, NULL, NULL, "KernalName", NULL,
     "<name>", "Specify name of Kernal ROM image"},
    {"-editor", SET_RESOURCE, 1, NULL, NULL, "EditorName", NULL,
     "<name>", "Specify name of Editor ROM image"},
    {"-chargen", SET_RESOURCE, 1, NULL, NULL, "ChargenName", NULL,
     "<name>", "Specify name of character generator ROM image"},
    {"-rom9", SET_RESOURCE, 1, NULL, NULL, "RomModule9Name", NULL,
     "<name>", "Specify 4K extension ROM name at $9***"},
    {"-romA", SET_RESOURCE, 1, NULL, NULL, "RomModuleAName", NULL,
     "<name>", "Specify 4K extension ROM name at $A***"},
    {"-romB", SET_RESOURCE, 1, NULL, NULL, "RomModuleBName", NULL,
     "<name>", "Specify 4K extension ROM name at $B***"},
    {"-petram9", SET_RESOURCE, 0, NULL, NULL, "Ram9", (resource_value_t) 1,
     NULL, "Enable PET8296 4K RAM mapping at $9***"},
    {"+petram9", SET_RESOURCE, 0, NULL, NULL, "Ram9", (resource_value_t) 0,
     NULL, "Disable PET8296 4K RAM mapping at $9***"},
    {"-petramA", SET_RESOURCE, 0, NULL, NULL, "RamA", (resource_value_t) 1,
     NULL, "Enable PET8296 4K RAM mapping at $A***"},
    {"+petramA", SET_RESOURCE, 0, NULL, NULL, "RamA", (resource_value_t) 0,
     NULL, "Disable PET8296 4K RAM mapping at $A***"},
    {"-superpet", SET_RESOURCE, 0, NULL, NULL, "SuperPET", (resource_value_t) 1,
     NULL, "Enable SuperPET I/O"},
    {"+superpet", SET_RESOURCE, 0, NULL, NULL, "SuperPET", (resource_value_t) 0,
     NULL, "Disable SuperPET I/O"},
    {NULL}
};

int pet_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}
