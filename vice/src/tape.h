/*
 * tape.h - Cassette drive and P00.
 *
 * Written by
 *   Jouko Valta (jopi@stekt.oulu.fi)
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

/* FIXME: P00 should be moved from here. */

#ifndef _CBM_TAPE_H_
#define _CBM_TAPE_H_

#include "types.h"

/* Image File Formats */

#define TFF_INDEX	0	/* Special name index file */
#define TFF_CBM		1
#define TFF_SFX		2
#define TFF_LYNX	3
#define TFF_P00		4
#define TFF_TAPE	5
#define TFF_DISK	6	/* Also adjust the format list on tapeunit.c */


/* Emulation Mode */

#define TEM_BYTE	0
#define TEM_ASCII	1	/* convert data */
#define TEM_TONE	2
#define TEM_NORMAL	4
#define TEM_TURBO	8


/* T64 Image File Layout */

#define TAPE_HDR_SIZE 64
#define TAPE_DIR_SIZE 32


/* Cassette Format Constants */

#define CAS_TYPE_OFFSET	0
#define CAS_STAD_OFFSET	1	/* start address */
#define CAS_ENAD_OFFSET	3	/* end address */
#define CAS_NAME_OFFSET	5	/* filename */

#define CAS_TYPE_PRG	1	/* Binary Program */
#define CAS_TYPE_BAS	3	/* Relocatable Program */
#define CAS_TYPE_DATA	4	/* Data Record */
#define CAS_TYPE_EOF	5	/* End of Tape marker */


#ifdef __GNUC__
#define PACK __attribute__ ((packed))	/* pack structures on byte boundaries */
#else
#define PACK		/* pack using a compiler switch instead */
#endif


typedef struct {
    BYTE   type PACK;
    BYTE   start[2] PACK;
    BYTE   end[2] PACK;
    BYTE   name[16] PACK;

} CBM_CASBUF;


typedef struct {
    int    type;		/* emulation type */

    int    ImageFormat;		/* Image File format: X00, FDX, FS, ... */

    FILE  *FileDs;
    char   ActiveName[256];


    /* Emulation */

    int    entries;		/* (maximum) Number of files on image */
    int    FileType;		/* PRG, SEQ, ... */

    BYTE  *directory;		/* Tape directory buffer */
    short  entry;		/* Number of current entry slot */

    int    sense;		/* tape sense */
    int    state;		/* stream state */

} TAPE;

/* ------------------------------------------------------------------------- */

extern int  is_pc64name(const char *name);
extern int  write_pc64header(FILE *fd, const char *name, int reclen);
extern int  read_pc64header(FILE *fd, char *name, int *reclen);
extern char *pc_get_cbmname(FILE *fd, char *fsname);
extern int  DirP00(FILE *fd, char *CbmName, int *start, int *end);

extern int  check_t64_header(FILE *f);
extern int  is_t64name(const char *name);
extern char *read_tape_image_contents(const char *fname);

#endif  /* _CBM_TAPE_H_ */
