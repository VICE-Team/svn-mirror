/*
 * pets.c - PET version handling.
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
#define	PET		/* for mkdep */
#endif

#include "vice.h"

#include <stdio.h>

#include "resources.h"
#include "pets.h"

#undef	DEBUG

extern void video_resize(void);

/* ------------------------------------------------------------------------- */

/*
 * table with ModelName, KernalFile, RAM size, ROM size, video RAM size,
 * I/O size, ieee flag, crtc flag, map flag, relevant crtc address bits, 
 * screen width and keyboard type
 */
static PetInfo pet_table[] = {
	{ "2001",  KERNAL2001NAME,  NULL,      
		0x2000, 0x4000, 0x400, 0x800, 1, 0, 0, 0x3ff, 40, "graphics" },
	{ "3008",  KERNAL3032NAME,  NULL,
		0x2000, 0x4000, 0x400, 0x800, 0, 0, 0, 0x3ff, 40, "graphics" },
	{ "3016",  KERNAL3032NAME,  NULL,
		0x4000, 0x4000, 0x400, 0x800, 0, 0, 0, 0x3ff, 40, "graphics" },
	{ "3032",  KERNAL3032NAME,  NULL,
		0x8000, 0x4000, 0x400, 0x800, 0, 0, 0, 0x3ff, 40, "graphics" },
	{ "3032B", KERNAL3032NAME,  EDITOR2B40NAME,
		0x8000, 0x4000, 0x400, 0x800, 0, 0, 0, 0x3ff, 40, "business" },
	{ "4016",  KERNAL4032NAME,  NULL,
		0x4000, 0x5000, 0x400, 0x800, 0, 1, 0, 0x3ff, 40, "graphics" },
	{ "4032",  KERNAL4032NAME,  NULL,
		0x8000, 0x5000, 0x400, 0x800, 0, 1, 0, 0x3ff, 40, "graphics" },
	{ "4032B", KERNAL4032NAME,  EDITOR4B40NAME,
		0x8000, 0x5000, 0x400, 0x800, 0, 1, 0, 0x3ff, 40, "business" },
	{ "8032",  KERNAL4032NAME,  EDITOR4B80NAME,
		0x8000, 0x5000, 0x800, 0x800, 0, 1, 0, 0x7ff, 80, "business" },
	{ "8096",  KERNAL4032NAME,  EDITOR4B80NAME,
		0x8000, 0x5000, 0x800, 0x800, 0, 1, 1, 0x7ff, 80, "business" },
	{ "8296",  KERNAL4032NAME,  EDITOR4B80NAME,
		0x8000, 0x5000, 0x1000,0x100, 0, 1, 2, 0x1fff,80, "business" },
	{ NULL }
};

PetInfo pet;

void pet_set_model(void) {
	int i;
	char *model = app_resources.petModel;

	i=0;
	while(pet_table[i].model) {
	  if(!strcmp(pet_table[i].model, model)) {
#ifdef DEBUG
	    printf("PET: setting model to PET%s\n",pet_table[i].model);
#endif
	    pet = pet_table[i];
	    resources_set_string(&app_resources.kernalName,pet.kernalName);
	    resources_set_string(&app_resources.petromEditor,pet.editorName);
	    app_resources.videoWidth = pet.video;
	    resources_set_string(&app_resources.keyboardType, pet.keyb);
	    resources_set_string(&app_resources.petrom9Name, NULL);
	    resources_set_string(&app_resources.petromAName, NULL);
	    resources_set_string(&app_resources.petromBName, NULL);
	    resources_set_string(&app_resources.petromBasic, NULL);
	    break;
	  }
	  i++;
	}
	if(!pet_table[i].model) {
	  printf("PET: Unknown PET model.\n");
	  exit(1);
	}
}

#if 0
void pet_set_kernal(void) {
#ifdef DEBUG
	printf("PET: setting kernal ROM to %s.\n",app_resources.kernalName);
#endif
}
#endif

void pet_set_video_width(void) {
	int v = app_resources.videoWidth;

	if(v==0 || v==40 || v==80) {
#ifdef DEBUG
	  printf("PET: setting screen width to %d.\n",v);
#endif
	} else {
	  printf("PET: invalid video mode %d, only 0, 40 & 80 are allowed!\n",
		v);
	  exit(1);
	}
}

void pet_set_keyboard_type(void) {
	char *k = app_resources.keyboardType;

	if( !( (!strcmp(k,"business")) || (!strcmp(k,"graphics")) ) ) {
	  printf("PET: invalid keyboard type %s, only 'business' or "
		"'graphics' allowed\n",k);
	  exit(1);
	}
#ifdef DEBUG
	printf("PET: setting keyboard to %s\n", k);
#endif
}

