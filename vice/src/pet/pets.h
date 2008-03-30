/*
 * pets.h - PET version handling.
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

#ifndef _PETS_H
#define _PETS_H

#define PET_KERNAL2001NAME  "pet2001"
#define PET_KERNAL3032NAME  "pet3032"
#define PET_KERNAL4032NAME  "pet4032"
#ifdef __riscos
#define PET_EDITOR2B40NAME  "edit2/b"
#define PET_EDITOR4B40NAME  "edit4/b40"
#define PET_EDITOR4B80NAME  "edit4/b80"
#else
#define PET_EDITOR2B40NAME  "edit2.b"
#define PET_EDITOR4B40NAME  "edit4.b40"
#define PET_EDITOR4B80NAME  "edit4.b80"
#endif
#define PET_COLS	80

typedef struct PetInfo {
	/* hardware options (resources) */
	int		ramSize;
	int		IOSize;		/* 256 Byte / 2k I/O */
	int		crtc;		/* 0 = no CRTC, 1 = has one */
	int		video;		/* 0 = autodetect, 40, or 80 */
	int		mem9;		/* 0 = open/ROM, 1 = RAM */
	int		memA;		/* 0 = open/ROM, 1 = RAM */
	int		kbd_type;	/* 1 = graphics, 0 = business (UK) */
	int		pet2k;		/* reset when romName is changed */
        int		superpet;	/* 1 = enable SuperPET I/O */

	/* ROM image resources */
	char		*chargenName;	/* Character ROM */
	char		*kernalName;	/* ROM to load before others */
					/* the following override kernalName
					   for their specific area */
	char		*editorName;	/* $E*** ROM image filename */
	char		*memBname;	/* $B*** ROM image filename */
	char		*memAname;	/* $A*** ROM image filename */
	char		*mem9name;	/* $9*** ROM image filename */

	/* runtime (derived) variables */
	int		videoSize;	/* video RAM size (1k or 2k) */
	int		map;		/* 0 = linear map, 1 = 8096 mapping */
					/* 2 = 8296 mapping */
	int		vmask;		/* valid CRTC address bits */
	int		screen_width;	/* derived from ROM */
} PetInfo;

extern PetInfo pet;

#define	petres	pet

extern int pet_set_model(const char *model_name, void *extra);
extern int pet_set_ramsize(int v);

extern int pet_init_resources(void);
extern int pet_init_cmdline_options(void);

int pet_set_model_info(PetInfo * pi);

extern int pet_init_ok;

#endif /* _PETS_H */

