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

typedef struct PetInfo {
	char		*model;
	char 		*kernalName;
	char 		*editorName;
	int		ramSize;	/* 0 = 2001, 1 = later */
	int		romSize;	/* expected length of ROM */
	int		videoSize;	/* video RAM size (1k or 2k) */
	int		IOSize;		/* 256 Byte / 2k I/O */
	int		pet2k;		/* 1 = 2001, 0 = later */
	int		crtc;		/* 0 = no CRTC, 1 = has one */
	int		map;		/* 0 = linear map, 1 = 8096 mapping */
					/* 2 = 8296 mapping */
	int		vmask;		/* valid CRTC address bits */
	int		video;		/* 0 = autodetect, 40, or 80 */
	char		*keyb;		/* 0 is graphics, 1 is business */
} PetInfo;

extern PetInfo pet;

void pet_set_model(void);
/* void pet_set_kernal(void); */
void pet_set_video_width(void);
void pet_set_keyboard_type(void);

#endif /* _PETS_H */
