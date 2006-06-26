/*
 * uitext_gp2x.c
 *
 * Written by
 *  Mike Dawson <mike@gp2x.org>
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

#include "Char_ROM.h"

void draw_ascii_string(unsigned char *screen, int screen_width, 
		int x, int y, const char *text, 
		unsigned char fg_colour, unsigned char bg_colour) {

	int i,j;

	unsigned char *dst=screen+(y*screen_width)+x;
	unsigned char *char_rom=(unsigned char *)builtin_char_rom;

	char c;
	for(i=0; c=text[i]; i++) {
		if(c>=97&&c<=122) c+=160;
		unsigned char *s=char_rom+(c*8)+0x800;
		unsigned char *d=dst;
		for(j=0; j<8; j++) {
			unsigned char v = *s++;
			d[0] = (v & 0x80) ? fg_colour : bg_colour;
			d[1] = (v & 0x40) ? fg_colour : bg_colour;
			d[2] = (v & 0x20) ? fg_colour : bg_colour;
			d[3] = (v & 0x10) ? fg_colour : bg_colour;
			d[4] = (v & 0x08) ? fg_colour : bg_colour;
			d[5] = (v & 0x04) ? fg_colour : bg_colour;
			d[6] = (v & 0x02) ? fg_colour : bg_colour;
			d[7] = (v & 0x01) ? fg_colour : bg_colour;
			d+=screen_width;
		}
		dst+=8;
	}
	
}

void draw_petscii_string(unsigned char *screen, int screen_width, 
		int x, int y, const char *text, 
		unsigned char fg_colour, unsigned char bg_colour) {

	int i,j;

	unsigned char *dst=screen+(y*screen_width)+x;
	unsigned char *char_rom=(unsigned char *)builtin_char_rom;

	char c;
	for(i=0; c=text[i]; i++) {
		unsigned char *s=char_rom+(c*8)+0x800;
		unsigned char *d=dst;
		for(j=0; j<8; j++) {
			unsigned char v = *s++;
			d[0] = (v & 0x80) ? fg_colour : bg_colour;
			d[1] = (v & 0x40) ? fg_colour : bg_colour;
			d[2] = (v & 0x20) ? fg_colour : bg_colour;
			d[3] = (v & 0x10) ? fg_colour : bg_colour;
			d[4] = (v & 0x08) ? fg_colour : bg_colour;
			d[5] = (v & 0x04) ? fg_colour : bg_colour;
			d[6] = (v & 0x02) ? fg_colour : bg_colour;
			d[7] = (v & 0x01) ? fg_colour : bg_colour;
			d+=screen_width;
		}
		dst+=8;
	}
}
