/*
 * uiimage.h - image contents handling on RISC OS
 *
 * Written by
 *  Andreas Dehmel <dehmel@forwiss.tu-muenchen.de>
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

#ifndef _UIIMAGE_H
#define _UIIMAGE_H

#define FileType_D64File	0x164
#define FileType_Data		0xffd

int  ui_image_contents_disk(const char *imagefile);
int  ui_image_contents_tape(const char *imagefile);
int  ui_image_contents_generic(const char *imagefile, int filetype);
void ui_image_contents_close(void);
void ui_image_contents_redraw(int *block);
void ui_image_contents_exit(void);

#endif
