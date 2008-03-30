/*
 * vdrive.c - Virtual disk-drive implementation.
 *
 * Written by
 *  Teemu Rantanen      (tvr@cs.hut.fi)
 *  Jarkko Sonninen     (sonninen@lut.fi)
 *  Jouko Valta         (jopi@stekt.oulu.fi)
 *  Olaf Seibert        (rhialto@mbfys.kun.nl)
 *  André Fachat        (a.fachat@physik.tu-chemnitz.de)
 *  Ettore Perazzoli    (ettore@comm2000.it)
 *  Martin Pottendorfer (Martin.Pottendorfer@aut.alcatel.at)
 *  Andreas Boose       (boose@unixserv.rz.fh-hannover.de)
 *
 * Patches by
 *  Dan Miner           (dminer@nyx10.cs.du.edu)
 *  Germano Caronni     (caronni@tik.ethz.ch)
 *  Daniel Fandrich     (dan@fch.wimsey.bc.ca)	/DF/
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

/* #define DEBUG_DRIVE */
/* #define DEBUG_FS */		/* Unix FS driver command/error channel */

#ifdef __hpux
#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE
#endif
#endif

#ifdef STDC_HEADERS
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <memory.h>
#include <assert.h>
#include <errno.h>

#include <unistd.h>
#include <dirent.h>
#endif

#include "serial.h"
#include "vdrive.h"
#include "file.h"
#include "zfile.h"
#include "utils.h"
#include "charsets.h"
#include "fsdevice.h"

#ifdef STANDALONE_1541
#include "c1541.h"
#endif

/* ------------------------------------------------------------------------- */

#define DIR_MAXBUF	8192

/* FIXME: `drive.c' needs this arrays too.  */
char sector_map_1541[43] =
{
    0,
    21, 21, 21, 21, 21, 21, 21, 21, 21, 21,	/*  1 - 10 */
    21, 21, 21, 21, 21, 21, 21, 19, 19, 19,	/* 11 - 20 */
    19, 19, 19, 19, 18, 18, 18, 18, 18, 18,	/* 21 - 30 */
    17, 17, 17, 17, 17,                    	/* 31 - 35 */
    17, 17, 17, 17, 17, 17, 17		/* Tracks 36 - 42 are non-standard. */
};

char sector_map_1571[71] =
{
    0,
    21, 21, 21, 21, 21, 21, 21, 21, 21, 21,	/*  1 - 10 */
    21, 21, 21, 21, 21, 21, 21, 19, 19, 19,	/* 11 - 20 */
    19, 19, 19, 19, 18, 18, 18, 18, 18, 18,	/* 21 - 30 */
    17, 17, 17, 17, 17,                    	/* 31 - 35 */
    21, 21, 21, 21, 21, 21, 21, 21, 21, 21,	/* 36 - 45 */
    21, 21, 21, 21, 21, 21, 21, 19, 19, 19,	/* 46 - 55 */
    19, 19, 19, 19, 18, 18, 18, 18, 18, 18,	/* 56 - 65 */
    17, 17, 17, 17, 17                     	/* 66 - 70 */
};

int speed_map_1541[42] = { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
                           3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 1, 1,
                           1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                           0, 0, 0 };

int speed_map_1571[70] = { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
                           3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 1, 1,
                           1, 1, 1, 1, 0, 0, 0, 0, 0,
                           3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
                           3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 1, 1,
                           1, 1, 1, 1, 0, 0, 0, 0, 0 };

static char pet_sector_map[78] =
{
    0,
    29, 29, 29, 29, 29, 29, 29, 29, 29, 29,	/*  1 - 10 */
    29, 29, 29, 29, 29, 29, 29, 29, 29, 29,	/* 11 - 20 */
    29, 29, 29, 29, 29, 29, 29, 29, 29, 29,	/* 21 - 30 */
    29, 29, 29, 29, 29, 29, 29, 29, 29, 27,	/* 31 - 40 */
    27, 27, 27, 27, 27, 27, 27, 27, 27, 27,	/* 41 - 50 */
    27, 27, 27, 25, 25, 25, 25, 25, 25, 25,	/* 51 - 60 */
    25, 25, 25, 25, 23, 23, 23, 23, 23, 23,	/* 61 - 70 */
    23, 23, 23, 23, 23, 23, 23			/* 71 - 77 */
};

int deleted_files;/* Keeps the number of entries deleted with the `S' command */

/* PC64 files need this too */

char *slot_type[] =
{
    "DEL", "SEQ", "PRG", "USR", "REL", "CBM", "DJJ", "FAB"
};


static int  set_error_data(DRIVE *floppy, int flags);
static int  do_block_command ( DRIVE *floppy, char command, char *buffer );
static int  do_memory_command ( DRIVE *floppy, BYTE *buffer, int length );
static int  do_initialize ( DRIVE *floppy );
static int  do_format ( DRIVE *floppy, char *name, BYTE *id, BYTE *minus );

#if defined(__GNUC__) && defined(DEBUG_DRIVE)
static void floppy_error_ ( bufferinfo_t *p, int code, int track, int sector,
						char *fromfunc );
#define	floppy_error(a,b,c,d)	floppy_error_((a),(b),(c),(d),__FUNCTION__)
#else
static void floppy_error ( bufferinfo_t *p, int code, int track, int sector );
#endif

static void floppy_close_all_channels ( DRIVE * );

static int  floppy_create_directory ( DRIVE *floppy, char *name, int length, int filetype, int secondary, BYTE *outputptr );

static int  do_copy ( DRIVE *floppy, char *dest, int length );
static int  do_rename ( DRIVE *floppy, char *dest, int length );

static void set_find_first_slot ( DRIVE *floppy, char *name, int length, int type );
static BYTE *find_next_slot ( DRIVE *floppy );

static void remove_slot ( DRIVE *floppy, BYTE *slot );
static int  write_sequential_buffer ( DRIVE *floppy, bufferinfo_t *bi, int length );

static int  alloc_first_free_sector ( BYTE *bam, int *track, int *sector );
static int  alloc_next_free_sector ( BYTE *bam, int *track, int *sector );
static int  floppy_name_match ( BYTE *slot, char *name, int length, int type );

static int  floppy_read_bam (DRIVE *floppy);
static int  floppy_write_bam (DRIVE *floppy);

static int  mystrncpy ( BYTE *d, BYTE *s, int n );

static int  import_GCR_image(BYTE *header, hdrinfo *hdr);

/* ------------------------------------------------------------------------- */

/*
 * Error messages
 */

errortext_t floppy_error_messages[] =
{
    { 0, "OK"},
    { 1, "FILES SCRATCHED"},
    { 2, "SELECTED PARTITION"},			/* 1581 */
    { 3, "UNIMPLEMENTED"},
    {26, "WRITE PROTECT ON"},
    {30, "SYNTAX ERROR"},
    {31, "SNERR INVALID COMMAND"},
    {32, "SNERR LINE TOO LONG"},
    {33, "SNERR INVAL FILE NAME"},
    {34, "SNERR NO FILE NAME"},
    {60, "WRITE FILE OPEN"},
    {61, "FILE NOT OPEN"},
    {62, "FILE NOT FOUND"},
    {63, "FILE EXISTS"},
    {64, "FILE TYPE MISMATCH"},
    {65, "NO BLOCK"},
    {66, "ILLEGAL TRACK OR SECTOR"},
    {67, "ILLEGAL SYSTEM T OR S"},
    {70, "NO CHANNEL"},
    {72, "DISK FULL"},
    {73, "TVR 1541 EMULATOR V2.1"},		/* The program version */
    {74, "DRIVE NOT READY"},
    {77, "SELECTED PARTITION ILLEGAL"},		/* 1581 */
    {80, "DIRECTORY NOT EMPTY"},
    {81, "PERMISSION DENIED"},
    {-1, 0}

};


/* ------------------------------------------------------------------------- */


int     initialize_1541(int dev, int type,
                        drive_attach_func_t attach_func,
                        drive_detach_func_t detach_func,
                        DRIVE *oldinfo)
{
    DRIVE  *floppy;
    int     i;

    floppy = oldinfo;

    /* Create instances of the disk drive.  */
    if (!floppy) {
	floppy = (DRIVE *)malloc(sizeof(DRIVE));
	assert(floppy);
    }

    memset (floppy, 0, sizeof(DRIVE));  /* init all pointers */

    floppy->type     = type;
    floppy->ActiveFd = -1;
    floppy->unit     = dev;

    for (i = 0; i < 15; i++)
	floppy->buffers[i].mode = BUFFER_NOT_IN_USE;

    floppy->buffers[15].mode = BUFFER_COMMAND_CHANNEL;
    floppy->buffers[15].buffer = (BYTE *) malloc(256);

    /* Initialise format constants.  */
    set_disk_geometry(floppy, type);

    /*
     * 'type' specifies what kind of emulation is selected
     */

    if (!(type & DT_FS)) {
	if (serial_attach_device(dev, (char *)floppy, "1541 Disk Drive",
	    read_1541, write_1541, open_1541, close_1541, flush_1541)) {
	    printf("could not initialize 1541 ????\n");
	    return(-1);
	}
    } else {
	if (attach_fsdevice(dev, (char *)floppy, "1541 FS Drive")) {
	    printf("could not initialize FS 1541 ????\n");
	    return(-1);
	}
    }

    floppy->attach_func = attach_func;
    floppy->detach_func = detach_func;

    floppy_error(&floppy->buffers[15], IPE_DOS_VERSION, 0, 0);
    return 0;
}


/*
 * This routine is used by the standalone version and ML monitor
 * in order to get around standard serial bus connection.
 * Device number is searched in filename if it's not NULL.
 * 'dev' contains the default in range 0...3 or 8...11.
 */

int  find_devno(int dev, const char *name)
{
    int num, len;
    const char *p;

    if (name && *name) {
	p = name;
	while (isspace((int)*p)) ++p;

	if (sscanf(p, "%d%n", &num, &len) == 1) {
	    p += len;
	    while (isspace((int)*p)) ++p;
	    if (*p == ':')
		return (num);
	}
    }	/* name */

    return (dev);	/* Default drive */
}


/* ------------------------------------------------------------------------- */

/*
 * Serial Bus Interface
 */

/*
 * Open a file on the disk image, and store the information on the
 * directory slot.
 */

int     open_1541(void *flp, char *name, int length, int secondary)
{
    DRIVE *floppy = (DRIVE *)flp;
    bufferinfo_t *p = &(floppy->buffers[secondary]);
    char    realname[256];
    int     reallength;
    int     readmode;
    int     filetype, rl;
    int     track, sector;
    BYTE   *slot;		/* Current directory entry */

    if ((!name || !*name) && p->mode != BUFFER_COMMAND_CHANNEL)  /* EP */
	return SERIAL_NO_DEVICE;	/* Routine was called incorrectly. */

    /*
     * No floppy in drive ?
     *
     * On systems with limited memory it may save resources not to keep
     * the image file open all the time.
     * If ActiveName exists, the file may have been temporarily closed.
     */

   if (floppy->ActiveFd < 0
       && p->mode != BUFFER_COMMAND_CHANNEL
       && secondary != 15
       && *name != '#') {
       floppy_error(&floppy->buffers[15], IPE_NOT_READY, 18, 0);
       printf("Drive not ready.\n");
       return SERIAL_ERROR;
   }

#ifdef DEBUG_DRIVE
    printf("OPEN 1541: FD = %d - Name '%s' (%d) on ch %d\n",
	   floppy->ActiveFd, name, length, secondary);
#endif

    /*
     * If channel is command channel, name will be used as write. Return only
     * status of last write ...
     */
    if (p->mode == BUFFER_COMMAND_CHANNEL) {
	int     n;
	int     status = SERIAL_OK;

	for (n = 0; n < length; n++)
	    status = write_1541(floppy, name[n], secondary);
	if (length)
            p->readmode = FAM_WRITE;
	else
            p->readmode = FAM_READ;
	return status;
    }

    /*
     * Clear error flag
     */
    floppy_error(&floppy->buffers[15], IPE_OK, 0, 0);

    /*
     * In use ?
     */
    if (p->mode != BUFFER_NOT_IN_USE) {
#ifdef DEBUG_DRIVE
	printf ("Cannot open channel %d. Mode is %d.\n", secondary, p->mode);
#endif
	floppy_error(&floppy->buffers[15], IPE_NO_CHANNEL, 0, 0);
	return SERIAL_ERROR;
    }

    /*
     * Filemode / type
     */
    if (secondary == 1)
	readmode = FAM_WRITE;
    else
	readmode = FAM_READ;

    filetype = 0;
    rl = 0;  /* REL */

    if (floppy_parse_name(name, length, realname, &reallength,
                          &readmode, &filetype, &rl) != SERIAL_OK)
	return SERIAL_ERROR;

    /* Limit file name to 16 chars.  */
    reallength = (reallength > 16) ? 16 : reallength;

    /*
     * Internal buffer ?
     */
    if (*name == '#') {
	p->mode = BUFFER_MEMORY_BUFFER;
	p->buffer = (BYTE *) malloc(256);
	p->bufptr = 0;
	return SERIAL_OK;
    }

    /*
     * Directory read
     * A little-known feature of the 1541: open 1,8,2,"$" (or even 1,8,1).
     * It gives you the BAM+DIR as a sequential file, containing the data
     * just as it appears on disk.  -Olaf Seibert
     */

    if (*name == '$') {
	if (secondary > 0) {
	    track = floppy->Dir_Track;
	    sector = 0;
	    goto as_if_sequential;
	}

	p->mode = BUFFER_DIRECTORY_READ;

	/* XXX assumes this is enough -Rhialto */
	p->buffer = (BYTE *) xmalloc(DIR_MAXBUF);

	p->length = floppy_create_directory(floppy,
					    realname, reallength, filetype,
					    secondary, p->buffer);

	if (p->length < 0) {
	    /* Directory not valid. */
	    p->mode = BUFFER_NOT_IN_USE;
	    free(p->buffer);
	    p->length = 0;
	    floppy_error(&floppy->buffers[15], IPE_NOT_FOUND, 0, 0);
	    return SERIAL_ERROR;
	}

	p->bufptr = 0;
	return SERIAL_OK;
    }


    /*
     * Now, set filetype according secondary address, if it was not specified
     * on filename
     */

    if (!filetype)
	filetype = (secondary < 2) ? FT_PRG : FT_SEQ;

    /*
     * Check that there is room on directory.
     */
    set_find_first_slot(floppy, realname, reallength, 0);

    /*
     * Find the first non-DEL entry in the directory (if it exists).
     */
    do
        slot = find_next_slot(floppy);
    while (slot && ((slot[SLOT_TYPE_OFFSET] & 0x07) == FT_DEL));

    p->readmode = readmode;
    p->slot = slot;


    /*
     * Open file for reading
     */

    if (readmode == FAM_READ) {
	int     type;
	int     status;

	if (!slot) {
	    close_1541(floppy, secondary);
	    floppy_error(&floppy->buffers[15], IPE_NOT_FOUND, 0, 0);
	    return SERIAL_ERROR;
	}

	type = slot[SLOT_TYPE_OFFSET] & 0x07;

        /*  I don't think that this one is needed - EP
	if (filetype && type != filetype) {
	    floppy_error(&floppy->buffers[15], IPE_BAD_TYPE, 0, 0);
	    return SERIAL_ERROR;
	}
        */

	filetype = type;  /* EP */

	track = (int) slot[SLOT_FIRST_TRACK];
	sector = (int) slot[SLOT_FIRST_SECTOR];


	/*
	 * Del, Seq, Prg, Usr (Rel not yet supported)
	 */
	if (type != FT_REL) {
	  as_if_sequential:
	    p->mode = BUFFER_SEQUENTIAL;
	    p->bufptr = 2;
	    p->buffer = (BYTE *) malloc(256);

	    status = floppy_read_block(floppy->ActiveFd, floppy->ImageFormat,
				       p->buffer, track, sector,
					floppy->D64_Header);

	    if (status < 0) {
		close_1541(floppy, secondary);
		return SERIAL_ERROR;
	    }
	    return SERIAL_OK;
	}
	/*
	 * Unsupported filetype
	 */
	return SERIAL_ERROR;
    }


    /*
     * Write
     */

    if (floppy->ReadOnly) {
	floppy_error(&floppy->buffers[15], IPE_WRITE_PROTECT_ON, 0, 0);
	return SERIAL_ERROR;
    }

    if (slot) {
	if (*name == '@')
	    remove_slot(floppy, slot);
	else {
	    close_1541(floppy, secondary);
	    floppy_error(&floppy->buffers[15], IPE_FILE_EXISTS, 0, 0);
	    return SERIAL_ERROR;
	}
    }


    /*
     * Create slot information.
     * Different front-ends can fetch the real filename from slot info.
     */

    p->slot = (BYTE *) malloc(32);
    memset(p->slot, 0, 32);
    memset(p->slot + SLOT_NAME_OFFSET, 0xa0, 16);
    memcpy(p->slot + SLOT_NAME_OFFSET, realname, reallength);
#ifdef DEBUG_DRIVE
    printf("DIR: name (%d) '%s'\n", reallength, realname);
#endif
    p->slot[SLOT_TYPE_OFFSET] = filetype;	/* unclosed */

    p->buffer = (BYTE *) malloc(256);
    p->mode = BUFFER_SEQUENTIAL;
    p->bufptr = 2;

#if 0
    /* XXX keeping entry until close not implemented */
    /* Write the directory entry to disk as an UNCLOSED file. */

#ifdef DEBUG_DRIVE
    printf("DEBUG: find empty DIR slot.\n");
#endif

    set_find_first_slot(floppy, NULL, -1, 0);
    e = find_next_slot(floppy);

    if (!e) {
	p->mode = BUFFER_NOT_IN_USE;
	free((char *)p->buffer);
	p->buffer = NULL;

	floppy_error(&floppy->buffers[15], IPE_DISK_FULL, 0, 0);
	return SERIAL_ERROR;
    }

    memcpy(&floppy->Dir_buffer[floppy->SlotNumber * 32 + 2],
	   p->slot + 2, 30);

#ifdef DEBUG_DRIVE
    printf("DEBUG: create, write DIR slot (%d %d).\n",
	   floppy->Curr_track, floppy->Curr_sector);
#endif
    floppy_write_block(floppy->ActiveFd, floppy->ImageFormat,
		       floppy->Dir_buffer,
		       floppy->Curr_track,
		       floppy->Curr_sector,
		       floppy->D64_Header);

    /*floppy_write_bam(floppy);*/

#endif  /* BAM write */

    return SERIAL_OK;
}


int     close_1541(void *flp, int secondary)
{
    DRIVE *floppy = (DRIVE *)flp;
    BYTE   *e;
    bufferinfo_t *p = &(floppy->buffers[secondary]);

#ifdef DEBUG_DRIVE
    printf("DEBUG: closing file #%d.\n", secondary);
#endif

    switch (p->mode) {
      case BUFFER_NOT_IN_USE:
	return SERIAL_OK;	/* FIXME: Is this correct? */

      case BUFFER_MEMORY_BUFFER:
      case BUFFER_DIRECTORY_READ:
	free((char *)p->buffer);
	p->mode = BUFFER_NOT_IN_USE;
	p->buffer = NULL;
	p->slot = NULL;
	break;

      case BUFFER_SEQUENTIAL:
	if (p->readmode & (FAM_WRITE | FAM_APPEND)) {
	    /*
	     * Flush bytes and write slot to directory
	     */

	    if (floppy->ReadOnly) {
		floppy_error(&floppy->buffers[15], IPE_WRITE_PROTECT_ON, 0,
			     0);
		return SERIAL_ERROR;
	    }

#ifdef DEBUG_DRIVE
	    printf("DEBUG: flush.\n");
#endif
	    write_sequential_buffer(floppy, p, p->bufptr);

#ifdef DEBUG_DRIVE
	    printf("DEBUG: find empty DIR slot.\n");
#endif
	    set_find_first_slot(floppy, NULL, -1, 0);
	    e = find_next_slot(floppy);

	    if (!e) {
		p->mode = BUFFER_NOT_IN_USE;
		free((char *)p->buffer);
		p->buffer = NULL;

		floppy_error(&floppy->buffers[15], IPE_DISK_FULL, 0, 0);
		return SERIAL_ERROR;
	    }
	    p->slot[SLOT_TYPE_OFFSET] |= 0x80;		/* Closed */

	    memcpy(&floppy->Dir_buffer[floppy->SlotNumber * 32 + 2],
		p->slot + 2, 30);

#ifdef DEBUG_DRIVE
	    printf("DEBUG: closing, write DIR slot (%d %d) and BAM.\n",
		   floppy->Curr_track, floppy->Curr_sector);
#endif
	    floppy_write_block(floppy->ActiveFd, floppy->ImageFormat,
			       floppy->Dir_buffer,
			       floppy->Curr_track,
			       floppy->Curr_sector,
			       floppy->D64_Header);

	    floppy_write_bam(floppy);
	}
	p->mode = BUFFER_NOT_IN_USE;
	free((char *)p->buffer);
	p->buffer = NULL;
	break;

      case BUFFER_COMMAND_CHANNEL:
	/* I'm not sure if this is correct, but really closing the buffer
	   should reset the read pointer to the beginning for the next
	   write! */
        floppy_error(&floppy->buffers[15], IPE_OK, 0, 0);
	floppy_close_all_channels(floppy);
	break;

      default:
	fprintf (stderr, "\nFatal: unknown floppy-close-mode\n");
	exit(-1);
    }

    return SERIAL_OK;
}


int     read_1541(void *flp, BYTE *data, int secondary)
{
    DRIVE *floppy = (DRIVE *)flp;
    bufferinfo_t *p = &(floppy->buffers[secondary]);

#ifdef DEBUG_DRIVE
    if (p->mode == BUFFER_COMMAND_CHANNEL)
	printf("Disk read  %d [%02d %02d]\n", p->mode, 0, 0);
#endif

    switch (p->mode) {
      case BUFFER_NOT_IN_USE:
	floppy_error(&floppy->buffers[15], IPE_NOT_OPEN, 0, 0);
	return SERIAL_ERROR;

      case BUFFER_DIRECTORY_READ:
	/*printf("read %d/%d [%02X]\n", p->bufptr, p->length, p->buffer[p->bufptr]);*/
	if (p->bufptr >= p->length) {
            *data = 0xc7;
	    return SERIAL_EOF;
        }
	*data = p->buffer[p->bufptr];
	p->bufptr++;
	break;

      case BUFFER_MEMORY_BUFFER:
	if (p->bufptr >= 256) {
            *data = 0xc7;
	    return SERIAL_EOF;
        }
	*data = p->buffer[p->bufptr];
	p->bufptr++;
	break;

      case BUFFER_SEQUENTIAL:
	if (p->readmode != FAM_READ)
	    return SERIAL_ERROR;

	/*
	 * Read next block if needed
	 */
	if (p->buffer[0]) {
	    if (p->bufptr >= 256) {
		floppy_read_block(floppy->ActiveFd, floppy->ImageFormat,
				  p->buffer,
				  (int) p->buffer[0],
				  (int) p->buffer[1],
				  floppy->D64_Header);
		p->bufptr = 2;
	    }
	} else {
	    if (p->bufptr > p->buffer[1]) {
                *data = 0xc7;
		return SERIAL_EOF;
            }
	}

	*data = p->buffer[p->bufptr];
	p->bufptr++;
	break;

      case BUFFER_COMMAND_CHANNEL:
	if (p->bufptr > p->length) {
	    floppy_error(&floppy->buffers[15], IPE_OK, 0, 0);
#ifdef DEBUG_DRIVE
	    printf( "end of buffer in command channel\n" );
#endif
            *data = 0xc7;
	    return SERIAL_EOF;
	}
	*data = p->buffer[p->bufptr];
	p->bufptr++;
	break;

      default:
	fprintf (stderr, "\nFatal: unknown buffermode on floppy-read\n");
	exit(-1);
    }

    return SERIAL_OK;
}


int     write_1541(void *flp, BYTE data, int secondary)
{
    DRIVE *floppy = (DRIVE *)flp;
    bufferinfo_t *p = &(floppy->buffers[secondary]);

    if (floppy->ReadOnly && p->mode != BUFFER_COMMAND_CHANNEL) {
	floppy_error(&floppy->buffers[15], IPE_WRITE_PROTECT_ON, 0, 0);
	return SERIAL_ERROR;
    }

#ifdef DEBUG_DRIVE
    if (p -> mode == BUFFER_COMMAND_CHANNEL)
	printf("Disk write %d [%02d %02d] data %02x (%c)\n",
	       p->mode, 0, 0, data, (isprint(data) ? data : '.') );
#endif

    switch (p->mode) {
      case BUFFER_NOT_IN_USE:
	floppy_error(&floppy->buffers[15], IPE_NOT_OPEN, 0, 0);
	return SERIAL_ERROR;

      case BUFFER_DIRECTORY_READ:
	floppy_error(&floppy->buffers[15], IPE_NOT_WRITE, 0, 0);
	return SERIAL_ERROR;

      case BUFFER_MEMORY_BUFFER:
	if (p->bufptr >= 256)
	    return SERIAL_ERROR;
	p->buffer[p->bufptr] = data;
	p->bufptr++;
	return SERIAL_OK;

      case BUFFER_SEQUENTIAL:
	if (p->readmode == FAM_READ)
	    return SERIAL_ERROR;

	if (p->bufptr >= 256) {
	    p->bufptr = 2;
	    if (write_sequential_buffer(floppy, p, WRITE_BLOCK) < 0)
		return SERIAL_ERROR;
	}
	p->buffer[p->bufptr] = data;
	p->bufptr++;
	break;

      case BUFFER_COMMAND_CHANNEL:
	if (p->readmode == FAM_READ) {
	    p->bufptr = 0;
	    p->readmode = FAM_WRITE;
	}
	if (p->bufptr >= 256)		/* Limits checked later */
	    return SERIAL_ERROR;
	p->buffer[p->bufptr] = data;
	p->bufptr++;
	break;

      default:
	fprintf (stderr, "\nFatal: Unknown write mode\n");
	exit(-1);
    }

    return SERIAL_OK;
}


void    flush_1541(void *flp, int secondary)
{
    DRIVE *floppy = (DRIVE *)flp;
    bufferinfo_t *p = &(floppy->buffers[secondary]);
    int status;

#ifdef DEBUG_DRIVE
printf("flush_1541, secondary = %d, buffer=%s\n   bufptr=%d, length=%d, "
		"read?=%d\n", secondary, p->buffer, p->bufptr, p->length,
		p->readmode==FAM_READ);
#endif

    if (p->mode != BUFFER_COMMAND_CHANNEL)
	return;

    if (p->readmode == FAM_READ) return;

    if (p->length) {		/* if no command, do nothing - esp. keep
				   error code. */
      status = ip_execute(floppy, p->buffer, p->bufptr);

      p->bufptr = 0;

      if (status == IPE_OK)
	  floppy_error(&floppy->buffers[15], IPE_OK, 0, 0);
    }

}


/* ------------------------------------------------------------------------- */


/*
 * Should set values to somewhere so that they could be read from
 * command channel
 */
#if defined(__GNUC__) && defined(DEBUG_DRIVE)
static void floppy_error_(bufferinfo_t *p, int code, int track, int sector,
			char *fromfunc)
#else
static void floppy_error(bufferinfo_t *p, int code, int track, int sector)
#endif
{
    char   *message;
    errortext_t *e;
    static int last_code;
#if defined(__GNUC__) && defined(DEBUG_DRIVE)
printf("floppy_error: code =%d, last_code =%d, trck =%d, sctr =%d, from=%s\n",
		code, last_code, track, sector, fromfunc);
#else
#ifdef DEBUG_DRIVE
printf("floppy_error: code =%d, last_code =%d, track =%d, sector =%d\n",
		code, last_code, track, sector);
#endif
#endif
    /* Only set an error once per command */
    if (code != IPE_OK && last_code != IPE_OK)
	return;

    last_code = code;

    e = &(floppy_error_messages[0]);
    while (e->nr >= 0 && e->nr != code)
	e++;

    if (e->nr >= 0)
	message = e->text;
    else
	message = "UNKNOWN ERROR NUMBER";

    sprintf((char *)p->buffer, "%02d,%s,%02d,%02d\015", code == IPE_DELETED ? deleted_files : code,
	    message, track, sector);

    /* length points to the last byte, and doesn't give the length... */
    p->length = strlen((char *)p->buffer)-1;
    p->bufptr = 0;

    if (code && code != IPE_DOS_VERSION)
	fprintf(stderr, "1541: ERR = %02d, %s, %02d, %02d\n",
		code == IPE_DELETED ? deleted_files : code,
		message, track, sector);

    p->readmode = FAM_READ;
}


/* ------------------------------------------------------------------------- */

/*
 * Input Processor Simulator. IP interprets incoming commands and generates
 * the error messages according to return values.
 */


int  ip_execute(DRIVE *floppy, BYTE *buf, int length)
{
    int     status = IPE_OK;
    BYTE   *p, *p2;
    char   *name;
    BYTE   *minus, *id;

    if (!length)
	return IPE_OK;
    if (length > IP_MAX_COMMAND_LEN) {
	floppy_error(&floppy->buffers[15], IPE_LONG_LINE, 0, 0);
	return IPE_LONG_LINE;
    }

    p = (BYTE *) malloc(length + 1);
    memcpy(p, buf, length);

    if (p[length-1] == 0x0d) --length;	/* chop CR character */
    p[length] = 0;


    name = (char *)memchr(p, ':', length);
    id   = (BYTE *)memchr(p, ',', length);
    minus= (BYTE *)memchr(p, '-', length);

    if (name)		/* Fix name length */
	for (p2 = p; *p2 && *p2 != ':' && length > 0; p2++, length--);


    switch (*p) {
      case 'C':		/* Copy */
	status = do_copy(floppy, (char *)name, length);
	break;

      case 'D':		/* Backup unused */
	status = IPE_INVAL;
	break;

      case 'R':		/* Rename */
	status = do_rename(floppy, (char *)name, length);
	break;

      case 'S':		/* Scratch */
	{
	    BYTE *slot;
	    char *realname = name;
	    int   reallength = 0, filetype = 0, readmode = 0;

	    /* XXX
	     * Wrong name parser - s0:file1,file2 means scratch
	     * those 2 files.
	     */

	    if (floppy_parse_name(name, length, realname, &reallength,
				  &readmode, &filetype, NULL) != SERIAL_OK) {
		status = IPE_NO_NAME;
	    } else if (floppy->ReadOnly) {
		status = IPE_WRITE_PROTECT_ON;
	    } else {
#ifdef DEBUG_DRIVE
		printf("remove name= '%s' len=%d (%d) type= %d.\n",
		       realname, reallength, length, filetype);
#endif
		deleted_files = 0;

		/* since remove_slot() uses set_find_first_slot() too, we
		 * cannot find the matching files by simply repeating
		 * find_next_slot() calls alone; we have to re-call
		 * set_find_first_slot() each time... EP 1996/04/07 */

		set_find_first_slot(floppy, realname, reallength, 0);
		while ((slot = find_next_slot(floppy))) {
		    remove_slot(floppy, slot);
		    deleted_files++;
		    set_find_first_slot(floppy, realname, reallength, 0);
		}
		if (deleted_files)
		    status = IPE_DELETED;
		else
		    status = IPE_NOT_FOUND;
		floppy_error(&floppy->buffers[15], status, 1, 0);
	    } /* else */
	}
	break;

      case 'I':
	status = do_initialize(floppy);
	break;

      case 'N':
	status = do_format(floppy, name, id, minus);
	break;

      case 'V':
	status = do_validate(floppy);
	break;

      case 'B':		/* Block, Buffer */
        if (!name)	/* B-x does not require a : */
            name = (char *)(p + 2);
        if (!minus)
            status = IPE_INVAL;
	else
	    status = do_block_command(floppy, minus[1], name + 1);
	break;

      case 'M':		/* Memory */
        if (!minus)     /* M-x does not allow a : */
            status = IPE_INVAL;
        else
            status = do_memory_command(floppy, minus +1, length);
	break;

      case 'P':		/* Position */
	/* 4 byte parameters: channel, rec_lo, rec_hi, pos */
	break;

      case 'U':		/* User */
        if (!name)
            name = (char *)(p + 1);
	if (p[1] == '0') {
	    status = IPE_OK;
	} else {
 	    switch ((p[1] - 1) & 0x0f) {
 	      case 0: /* UA */
 	        /* XXX incorrect: U1 is not exactly the same as B-R */
 	        /*      -- should store the buffer pointer */
 	        if (name)
		    status = do_block_command(floppy, 'R', name + 1);
 	        break;

 	      case 1: /* UB */
 	        /* XXX incorrect: U2 is not exactly the same as B-W */
 	        /*      -- should store the buffer pointer */
 	        if (name)
 		    status = do_block_command(floppy, 'W', name + 1);
 	        break;

 	      case 2: /* Jumps */
 	      case 3:
 	      case 4:
 	      case 5:
 	      case 6:
 	      case 7:
 	        status = IPE_NOT_READY;
 	        break;

 	      case 8: /* UI */
 	        if (p[2] == '-' || p[2] == '+') {
 		    status = IPE_OK;	/* Set IEC bus speed */
 	        } else {
 		    floppy_close_all_channels(floppy); /* Warm reset */
 		    status = IPE_DOS_VERSION;
 	        }
 	        break;

 	      case 9: /* UJ */
 	        floppy_close_all_channels(floppy); /* Cold reset */
 	        status = IPE_DOS_VERSION;
 	        break;

 	      case 10: /* UK..UP */
 	      case 11:
 	      case 12:
 	      case 13:
 	      case 14:
 	      case 15:
 	        status = IPE_NOT_READY;
 	        break;
 	    }
	} /* Un */
	break;

      default:
	status = IPE_INVAL;
	break;
    } /* commands */

    if (status == IPE_INVAL)
        fprintf(stderr, "1541: wrong command `%s'\n", p);

    floppy_error(&floppy->buffers[15], status, 0, 0);

    free((char *)p);
    return status;
}

/*
 * This function is modeled after BLKPAR, in the 1541 ROM at CC6F.
 */

static int  get_block_parameters(char *buf, int *p1, int *p2, int *p3, int *p4)
{
    int ip;
    char *bp;
    int *p[4];	/* This is a kludge */
    p[0] = p1;
    p[1] = p2;
    p[2] = p3;
    p[3] = p4;

    bp = buf;

    for (ip = 0; ip < 4; ip++) {
	while (*bp == ' ' || *bp == ')' || *bp == ',')
	    bp++;
	if (*bp == 0)
	    break;
	/* convert and skip over decimal number */
	*p[ip] = strtol(bp, &bp, 10);
    }

    if ((*bp != 0) && (ip == 4))
	return IPE_SYNTAX;
    return -ip;			/* negative of # arguments found */
}

static int  do_block_command(DRIVE *floppy, char command, char *buffer)
{
    int     channel = 0;
    int     drive = 0;
    int     track = 0;
    int     sector = 0;
    int     position = 0;
    int     l;

    switch (command) {
      case 'R':
      case 'W':
        l = get_block_parameters(buffer, &channel, &drive, &track, &sector);

        if (l < 0) {

#ifdef DEBUG_DRIVE
	    printf("B-R/W parsed ok. (l=%d) channel %d mode %d, drive=%d, track=%d sector=%d\n",
		   l, channel, floppy->buffers[channel].mode, drive, track, sector);
#endif

	    if (floppy->buffers[channel].mode != BUFFER_MEMORY_BUFFER) {
		return IPE_NO_CHANNEL;
	    }

	    if (command == 'W') {
		if (floppy->ReadOnly)
		    return IPE_WRITE_PROTECT_ON;
		if (floppy_write_block(floppy->ActiveFd, floppy->ImageFormat,
                                       floppy->buffers[channel].buffer,
                                       track, sector,
                                       floppy->D64_Header))
                    return IPE_NOT_READY;
	    } else {
		if (floppy_read_block(floppy->ActiveFd, floppy->ImageFormat,
                                      floppy->buffers[channel].buffer,
                                      track, sector, floppy->D64_Header) < 0)
                    return IPE_NOT_READY;
	    }
	    floppy->buffers[channel].bufptr = 0;
	}
	break;

      case 'A':
      case 'F':
  	l = get_block_parameters(buffer, &drive, &track, &sector, &channel);
  	if (l > 0)	/* just 3 args used */
  	    return l;

  	if (command == 'A') {
  	    if (!allocate_sector(floppy->bam, track, sector)) {
  		/*
  		 * Desired sector not free. Suggest another. XXX The 1541
  		 * uses an inferior search function that only looks on
  		 * higher tracks and can return sectors in the directory
  		 * track.
  		 */
  		if (alloc_next_free_sector(floppy->bam, &track, &sector) >= 0) {
  		    /* Deallocate it and merely suggest it */
  		    free_sector(floppy->bam, track, sector);
  		} else {
  		    /* Found none */
  		    track = 0;
  		    sector = 0;
  		}
  		floppy_error(&floppy->buffers[15], IPE_NO_BLOCK, track, sector);
  		return IPE_NO_BLOCK;
  	    }
  	} else
  	    free_sector(floppy->bam, track, sector);
  	break;

      case 'P':
  	l = get_block_parameters(buffer, &channel, &position, &track, &sector);
  	if (l > 0)	/* just 2 args used */
  	    return l;

  	if (floppy->buffers[channel].mode != BUFFER_MEMORY_BUFFER) {
  	    return IPE_NO_CHANNEL;
  	}

  	floppy->buffers[channel].bufptr = position;
  	break;

      default:
	return IPE_INVAL;
    }
    return IPE_OK;
}


static int  do_memory_command(DRIVE *floppy, BYTE *buffer, int length)
{
#if 0
    int     addr = 0;


    if (length < 3)		/* All commands at least 3 bytes */
      return IPE_SYNTAX;

    addr = (buffer[1] & 0xff) | ((buffer[2] & 0xff) << 8);

    switch (*buffer) {
      case 'W':
	if (length < 5)
	    return IPE_SYNTAX;

	count = buffer[3];
	/* data= buffer[4 ... 4+34]; */

	if (floppy->buffers[addrlo].mode != BUFFER_MEMORY_BUFFER) {
	    return IPE_SYNTAX;

	    memcpy ( ... , buffer + 4, buffer[3]);
	}
	break;

      case 'R':
	floppy->buffers[channel].bufptr = addr;
	break;

      case 'E':
	floppy->buffers[channel].bufptr = addr;
	break;

      default:
	return IPE_SYNTAX;
    }

    return IPE_OK;
#else
    return IPE_UNIMPL;
#endif
}


/* ------------------------------------------------------------------------- */

/*
 * Parse name realname, type and read/write mode. '@' on write must
 * be checked elsewhere
 */

int floppy_parse_name(char *name, int length, char *ptr,
		      int *reallength, int *readmode, int *filetype,
		      int *rl)
{
    char   *c, *p;
    int     t;

    if (!name || !*name)
	return FLOPPY_ERROR;

    p = (char *)memchr(name, ':', length);
    if (p)
	p++;
    else {	/* no colon found */
	if (*name != '$') p = name;
	else p = name + strlen(name);	/* set to null byte */
    }
#ifdef DEBUG_DRIVE
	    printf("Name (%d): '%s'\n", length, p);
#endif

#if 0
    if (*name == '@' && p == name)
	p++;
#endif

    t = strlen(p);
    *reallength = 0;

    while (*p != ',' && t-- > 0) {
	(*reallength)++;
	*(ptr++) = *(p++);	/* realname pointer */
#ifdef DEBUG_DRIVE
	printf("parsing... [%d] %02x  t=%d\n", *reallength, *(ptr-1), t);
#endif
    }  /* while */


    *filetype = 0;		/* EP */

    /*
     * Change modes ?
     */
    while (t > 0) {
	t--;
	p++;

	if (t == 0) {
#ifdef DEBUG_DRIVE
	    printf("done. [%d] %02x  t=%d\n", *reallength, *p, t);
	    printf("No type.\n");
#endif
	    return FLOPPY_ERROR;
	}

	switch (*p) {
	  case 'S':
	    *filetype = FT_SEQ;
	    break;
	  case 'P':
	    *filetype = FT_PRG;
	    break;
	  case 'U':
	    *filetype = FT_USR;
	    break;
	  case 'L':			/* L,(#record length)  max 254 */
	    if (rl && p[1] == ',') {
		*rl = p[2];		/* Changing RL causes error */

		if (*rl > 254)
		    return FLOPPY_ERROR;
	    }
	    *filetype = FT_REL;
	    break;


	    /* 1581 types */

	  case 'C':
	    *filetype = FT_CBM;		/* 1581 partition */
	    break;
	  case 'J':
	    *filetype = FT_DJJ;
	    break;
	  case 'F':
	    *filetype = FT_FAB;		/* Fred's format */
	    break;


	    /* Access Control Methods */

	  case 'R':
	    *readmode = FAM_READ;
	    break;
	  case 'W':
	    *readmode = FAM_WRITE;
	    break;
	  case 'A':
	    if (*filetype != FT_SEQ)
		return FLOPPY_ERROR;
	    *readmode = FAM_APPEND;		/* Append on a SEQ file */
	    break;

	  default:
#ifdef DEBUG_DRIVE
	    printf("No way. p='%s'\n", p);
#endif
	    return FLOPPY_ERROR;
	}

	c = (char *)memchr(p, ',', t);

	if (c) {
	    t -= (c - p);
	    p = c;
	} else
	    t = 0;
    }  /* while (t) */


#ifdef DEBUG_DRIVE
    printf ("Type = %s  %s\n",
	    slot_type[*filetype],
	    (*readmode == FAM_READ ?  "read" : "write"));
#endif


    return FLOPPY_COMMAND_OK;
}


/*
 * Find next slot with the same name for match.
 * If type is 0, check the file name only.
 * If type is nonzero, check that it matches the id of the file found.
 * Note: If the type is not given on command line, the drive adds it
 * itself, according to the channel number (secondary address) used.
 *
 * Just like MSDOS, the CBM drive DOS also uses '?' for any character
 * and '*' to skip all the remainder.
 */

static int  floppy_name_match (BYTE *slot, char *name, int length, int type)
{
    int     i;

    if (length < 0) {
	if (slot[SLOT_TYPE_OFFSET])
	    return 0;
	else
	    return 1;
    }
    if (!slot[SLOT_TYPE_OFFSET])
	return 0;

    if (length > 16)
	length = 16;

    for (i = 0; i < length; i++) {
	switch (name[i]) {
	  case '?':		/* Match any character */
	    break;

	  case '*':		/* Make a match */
	    i = 16;
	    break;

	  default:
	    if ((BYTE)name[i] != slot[i + SLOT_NAME_OFFSET])
		return 0;
	}
    }

    /*
     * Got name match ?
     */
    if (i < 16 && slot[SLOT_NAME_OFFSET + i] != 0xa0)
	return 0;

    if (type && type != (slot[SLOT_TYPE_OFFSET] & 0x07))
	return 0;

    return 1;
}


/* ------------------------------------------------------------------------- */

/*
 * Return the number of free blocks on disk.
 */

static int  floppy_free_block_count (DRIVE *floppy)
{
    int blocks, i;

    for (blocks = 0, i = 1; i <= floppy->NumTracks; i++) {
	if (i != floppy->Dir_Track)
	    blocks += (i <= NUM_TRACKS_1541) ?
		floppy->bam[BAM_BIT_MAP + 4 * (i - 1)] :
		floppy->bam[BAM_EXT_BIT_MAP + 4 * (i - NUM_TRACKS_1541 - 1)];
    }

    return blocks;
}

/*
 * Create directory listing. (called from open_1541)
 * If filetype is 0, match for all files.
 * Return the length in bytes if successful, -1 if the directory is not
 * valid.
 */

static int  floppy_create_directory (DRIVE *floppy, char *name,
				     int length, int filetype, int secondary,
				     BYTE *outputptr)
{
    BYTE   *l, *p;
    BYTE   *origptr = outputptr;
    int     blocks;
    int     addr, i;

    if (length) {
	if (*name == '$') { ++name; --length; }
	if (*name == ':') { ++name; --length; }
    }
    if (!*name || length < 1) { name = "*\0"; length = 1; }


    /*
     * Start Address, Line Link and Line number 0
     */

    l = outputptr;
    addr = 0x401;
    SET_LO_HI(l, addr);

    l += 2;			/* Leave space for Next Line Link */
    *l++ = 0;
    *l++ = 0;

    *l++ = (BYTE) 0x12;		/* Reverse on */

    *l++ = '"';
    memcpy(l, &floppy->bam[BAM_DISK_NAME], 16);
    no_a0_pads(l, 16);
    l += 16;
    *l++ = '"';
    *l++ = ' ';
    memcpy(l, &floppy->bam[BAM_DISK_ID], 5);
    no_a0_pads(l, 5);
    l += 5;

    *l++ = 0;


    /*
     * Pointer to the next line
     */

    addr += ((l - outputptr) - 2);

    outputptr[2] = 1;	/* addr & 0xff; */
    outputptr[3] = 1;	/* (addr >>8) & 0xff; */
    outputptr = l;


    /*
     * Now, list files that match the pattern.
     * Wildcards can be used too.
     */

    set_find_first_slot(floppy, name, length, filetype);

    while ((p = find_next_slot(floppy))) {
	BYTE *tl;

	/* Check whether the directory exceeds the malloced memory.  We make
           sure there is enough space for two lines because we also have to
           add the final ``...BLOCKS FREE'' line.  */
	if ((l - origptr) >= DIR_MAXBUF - 64) {
	    fprintf(stderr, "Directory too long: giving up.\n");
	    return -1;
	}

	if (p[SLOT_TYPE_OFFSET]) {

            tl = l;

	    l += 2;

	    /*
	     * Length and spaces
	     */

	    blocks = p[SLOT_NR_BLOCKS] + p[SLOT_NR_BLOCKS + 1] * 256;
	    SET_LO_HI(l, blocks);

	    if (blocks < 10)
		*l++ = ' ';
	    if (blocks < 100)
		*l++ = ' ';

	    /*
	     * Filename
	     */

	    *l++ = ' ';
	    *l++ = '"';

	    memcpy(l, &p[SLOT_NAME_OFFSET], 16);

	    for (i = 0; (i < 16) && (p[SLOT_NAME_OFFSET + i] != 0xa0);)
		i++;

	    no_a0_pads(l, 16);

	    l[16] = ' ';
	    l[i] = '"';
	    l += 17;

	    /*
	     * Type + End
	     * There are 3 spaces or < and 2 spaces after the filetype.
	     * Well, not exactly - the whole directory entry is 32 byte long
	     * (including nullbyte).
	     * Depending on the file size, there are more or less spaces
	     */

	    sprintf ((char *)l, "%c%s%c%c",
		     (p[SLOT_TYPE_OFFSET] & FT_CLOSED ? ' ' : '*'),
		     slot_type[p[SLOT_TYPE_OFFSET] & 0x07],
		     (p[SLOT_TYPE_OFFSET] & FT_LOCKED ? '<' : ' '),
		     0);
	    l += 5;

	    i = l - tl;

	    while (i < 31) {
		*l++ = ' ';
		i++;
	    }

	    *l++ = '\0';

	    /*
	     * New address
	     */
	    addr += l - outputptr;

	    outputptr[0] = 1;	/* addr & 0xff; */
	    outputptr[1] = 1;	/* (addr >> 8) & 0xff; */

	    outputptr = l;
	}
    }

    blocks = floppy_free_block_count(floppy);

    *l++ = 0;
    *l++ = 0;
    SET_LO_HI(l, blocks);
    memcpy(l, "BLOCKS FREE.", 12);
    l += 12;
    memset(l, ' ', 13);
    l += 13;
    *l++ = (char) 0;

    /* Line Address */
    addr += l - outputptr;
    outputptr[0] = 1;	/* addr & 0xff; */
    outputptr[1] = 1;	/* (addr / 256) & 0xff; */

    /*
     * end
     */
    *l++ = (char) 0;
    *l++ = (char) 0;
    *l   = (char) 0;

    return (l - origptr);
}


/* ------------------------------------------------------------------------- */


static int  do_copy( DRIVE *floppy, char *dest, int length)
{
    char   *name, *files, *p, c;


    /* Split command line */

    if (!dest || !(files = memchr(dest, '=', length)) )
	return (IPE_SYNTAX);

    *files++ = 0;

    if (strchr (dest, ':'))
	dest = strchr (dest, ':') +1;

#ifdef DEBUG_DRIVE
    printf("COPY: dest= '%s'\n      orig= '%s'\n", dest, files);
#endif

    if (open_1541(floppy, dest, strlen(dest), 1))
	return (IPE_FILE_EXISTS);

    p = name = files;

    while (*name) {				/* loop for given files */
	for (; *p && *p != ','; p++);
	*p++ = 0;

    if (strchr (name, ':'))
	name = strchr (name, ':') +1;

#ifdef DEBUG_DRIVE
	printf("searching for file '%s'\n", name);
#endif
	if (open_1541(floppy, name, strlen(name), 0)) {
	    close_1541(floppy, 1);
	    return (IPE_NOT_FOUND);
	}

	while (!read_1541(floppy, (BYTE *)&c, 0)) {
	    if (write_1541(floppy, c, 1)) {
		close_1541(floppy, 0);		/* no space on disk */
		close_1541(floppy, 1);
		return (IPE_DISK_FULL);
	    }
	}

	close_1541(floppy, 0);
	name = p;				/* next file */
    } /* while */

    close_1541(floppy, 1);
    return(IPE_OK);
}


/*
 * Rename disk entry
 */

static int do_rename( DRIVE *floppy, char *dest, int length)
{
    char   *src;
    char   dest_name[256], src_name[256];
    int    dest_reallength, dest_readmode, dest_filetype, dest_rl;
    int	   src_reallength, src_readmode, src_filetype, src_rl;
    BYTE   *slot;

    if (!dest || !(src = memchr(dest, '=', length)) )
	return (IPE_SYNTAX);

    *src++ = 0;

    if (strchr (dest, ':'))
	dest = strchr (dest, ':') +1;

#ifdef DEBUG_DRIVE
    printf("RENAME: dest= '%s'\n      orig= '%s'\n", dest, src);
#endif

    if (!floppy_parse_name(dest, strlen(dest), dest_name, &dest_reallength, &dest_readmode,
			   &dest_filetype, &dest_rl) == FLOPPY_ERROR)
        return IPE_SYNTAX;
    if (!floppy_parse_name(src, strlen(src), src_name, &src_reallength, &src_readmode,
			   &src_filetype, &src_rl) == FLOPPY_ERROR)
        return IPE_SYNTAX;

    if (floppy->ReadOnly)
	return IPE_WRITE_PROTECT_ON;

    /* check if the destination name is already in use */

    set_find_first_slot(floppy, dest_name, dest_reallength, dest_filetype);
    slot = find_next_slot(floppy);

    if (slot)
        return (IPE_FILE_EXISTS);

    /* find the file to rename */

    set_find_first_slot(floppy, src_name, src_reallength, src_filetype);
    slot = find_next_slot(floppy);

    if (!slot)
	return (IPE_NOT_FOUND);

    /* now we can replace the old file name... */
    /* we write directly to the Dir_buffer */

    slot = &floppy->Dir_buffer[floppy->SlotNumber * 32];
    memset(slot + SLOT_NAME_OFFSET, 0xa0, 16);
    memcpy(slot + SLOT_NAME_OFFSET, dest_name, dest_reallength);
    if (dest_filetype)
	slot[SLOT_TYPE_OFFSET] = dest_filetype; /* XXX: is this right? */

    /* update the directory */
    floppy_write_block(floppy->ActiveFd, floppy->ImageFormat,
		       floppy->Dir_buffer,
		       floppy->Curr_track,
		       floppy->Curr_sector,
		       floppy->D64_Header);
    return(IPE_OK);
}


/* ------------------------------------------------------------------------- */

/*
 * Close all channels. This happens on 'I' -command and on command-
 * channel close.
 */

static void floppy_close_all_channels(DRIVE *floppy)
{
    int     i;
    bufferinfo_t *p;


    for (i = 0; i <= 15; i++) {
	p = &(floppy->buffers[i]);

	if (p->mode != BUFFER_NOT_IN_USE && p->mode != BUFFER_COMMAND_CHANNEL)
	    close_1541(floppy, i);
    }
}


static int  do_initialize(DRIVE *floppy)
{
    floppy_close_all_channels(floppy);

    /* Update BAM in memory */
    if (floppy->ActiveFd > 0)
	floppy_read_bam(floppy);

    if (floppy->ErrFlg)
	set_error_data(floppy, 3);	/* clear or read error data */

    return IPE_OK;
}

static int allocate_chain(DRIVE *floppy, int t, int s)
{
    BYTE    tmp[256];

    while (t) {
	if (check_track_sector(floppy->ImageFormat, t, s) < 0) {
	    floppy_error(&floppy->buffers[15], IPE_ILLEGAL_TRACK_OR_SECTOR, s, t);
	    return IPE_ILLEGAL_TRACK_OR_SECTOR;
	}
	if (!allocate_sector(floppy->bam, t, s)) {
	    /* The 1541 does not seem to catch this error... */
	    floppy_error(&floppy->buffers[15], IPE_NO_BLOCK, s, t);
	    return IPE_NO_BLOCK;
	}
	floppy_read_block(floppy->ActiveFd, floppy->ImageFormat, tmp, t, s,
					floppy->D64_Header);
	t = (int) tmp[0];
	s = (int) tmp[1];
    }
    return IPE_OK;
}

int  do_validate(DRIVE *floppy)
{
    int     t, s;
    BYTE   *b;
    int     status;
    BYTE oldbam[256];

    status = do_initialize(floppy);
    if (status != IPE_OK)
	return status;
    if (floppy->ReadOnly)
	return IPE_WRITE_PROTECT_ON;
    memcpy(oldbam, floppy->bam, 256);

    for (t = 1; t <= floppy->NumTracks; t++) {

	b = (t <= NUM_TRACKS_1541) ?
	(BYTE *) floppy->bam + BAM_BIT_MAP + 4 * (t - 1) :
	(BYTE *) floppy->bam + BAM_EXT_BIT_MAP +
	    4 * (t - NUM_TRACKS_1541 - 1);

	*b++ = 0;
	*b++ = 0;
	*b++ = 0;
	*b++ = 0;

	for (s = 0; s < sector_map_1541[t]; s++)
	    free_sector(floppy->bam, t, s);
    }

    /*
     * First map out the BAM and directory itself.
     */

    /* XXX  -- advanced drives ... */

    status = allocate_chain(floppy, DSK_BAM_TRACK, DSK_BAM_SECTOR);
    if (status != IPE_OK) {
	memcpy(floppy->bam, oldbam, 256);
	return status;
    }

    set_find_first_slot(floppy, "*", 1, 0);

    while ((b = (BYTE *) find_next_slot(floppy))) {
	char *filetype = (char *)
	    &floppy->Dir_buffer[floppy->SlotNumber * 32 + SLOT_TYPE_OFFSET];

	if (*filetype & FT_CLOSED) {
	    status = allocate_chain(floppy, b[SLOT_FIRST_TRACK],
	                                    b[SLOT_FIRST_SECTOR]);
	    if (status != IPE_OK) {
		memcpy(floppy->bam, oldbam, 256);
		return status;
	    }
	    /* The 1541 drive always validates side sectors even if the file
	       type is not REL.  */
	    status = allocate_chain(floppy, b[SLOT_SIDE_TRACK],
	                                    b[SLOT_SIDE_SECTOR]);
	    if (status != IPE_OK) {
		memcpy(floppy->bam, oldbam, 256);
		return status;
	    }
	} else {
	    *filetype = FT_DEL;

	    floppy_write_block(floppy->ActiveFd, floppy->ImageFormat,
			       floppy->Dir_buffer,
			       floppy->Curr_track,
			       floppy->Curr_sector,
			       floppy->D64_Header);
	}
    }

    /* Write back BAM only if validate was successful.  */
    floppy_write_bam(floppy);

    return status;

}


static int  do_format(DRIVE *floppy, char *name, BYTE *id, BYTE *minus)
{
    BYTE    tmp[256];
    int     status;
    BYTE    null = 0;

    if (!name)
	return IPE_SYNTAX;

    if (floppy->ReadOnly)
	return IPE_WRITE_PROTECT_ON;

    /*
     * If id, skip comma
     */

    if (id)
	*id++ = 0;
    else
	id = &null;

    if (floppy->ErrFlg)
	set_error_data(floppy, 5);	/* clear and write error data */

    /*
     * Make the first dir-entry
     */
    memset(tmp, 0, 256);
    tmp[1] = 255;
    floppy_write_block(floppy->ActiveFd, floppy->ImageFormat,
		       tmp, floppy->Dir_Track, floppy->Dir_Sector,
			floppy->D64_Header);

    /*
     * Create Disk Format of type '2A'
     */
    memset(floppy->bam, 0, 256);
    floppy->bam[0] = DSK_DIR_TRACK;
    floppy->bam[1] = DSK_DIR_SECTOR;
    floppy->bam[2] = 65;

    memset(floppy->bam + BAM_DISK_NAME, 0xa0, 27);
    mystrncpy(floppy->bam + BAM_DISK_NAME, (BYTE *)name + 1, 16);
    mystrncpy(floppy->bam + BAM_DISK_ID, id, 2);
    floppy->bam[BAM_VERSION] = 50;
    floppy->bam[BAM_VERSION + 1] = 65;

    /* Write BAM so that Initialise during Validate will work -Olaf Seibert */
    floppy_write_block(floppy->ActiveFd, floppy->ImageFormat,
		       floppy->bam,  DSK_BAM_TRACK, DSK_BAM_SECTOR,
			floppy->D64_Header);

    /*
     * Validate is called to clear the BAM.
     */
    status = do_validate(floppy);

    return status;
}


/* ------------------------------------------------------------------------- */
/*
 * Floppy Disc Controller Simulator. FDC executes tasks in the work queue
 * and puts return values there.
 */


/*
 * Sync (update) the Error Block image in memory
 */

static int  set_error_data(DRIVE *floppy, int flags)
{
    int    size;
    off_t  offset;


    if (!(floppy->ErrData)) {
	floppy->ErrData = (char *)malloc((size_t)MAX_BLOCKS_ANY);
	assert (floppy->ErrData);
	memset(floppy->ErrData, 0x01, MAX_BLOCKS_ANY);
    }
    else if (flags & 1)		/* clear error data */
	memset(floppy->ErrData, 0x01, MAX_BLOCKS_ANY);


    size   = num_blocks (floppy->ImageFormat, floppy->NumTracks);
    offset = ((off_t)size << 8) + (off_t)(floppy->D64_Header?0:HEADER_LENGTH);
    lseek(floppy->ActiveFd, offset, SEEK_SET);

    if (flags & 2) {		/* read from file */
	if (read(floppy->ActiveFd, (char *)floppy->ErrData, size) < size) {
	    fprintf(stderr, "\nfloppy image read error\n");
	    return(-2);
	}
    }

    if (flags & 4) {		/* write to file */
	if (write(floppy->ActiveFd, (char *)floppy->ErrData, size) < size) {
	    fprintf(stderr, "\nfloppy image write error\n");
	    return(-2);
	}
    }

    return (0);
}


#if 0				/* unused... */

/*
 * Return 'error' according to track and sector
 */

static int  get_disk_error(DRIVE *floppy, int track, int sector)
{
    int     sectors, i;

    if ((sectors = check_track_sector(floppy->ImageFormat, track, sector)) < 0)
	return -1;

    if (floppy->ErrFlg) {
	i = floppy->ErrData[sectors];
    }
    else
	i = 0x01;


    return (i);
}

#endif


/* ------------------------------------------------------------------------- */

/*
 * Check that requested Track and Sector are within limits.
 * Return the index number of the block.
 */

int  check_track_sector(int format, int track, int sector)
{
    int     sectors = 0, i;


    if (track < 1 || sector < 0)
	return (-1);


    switch (format) {
      case 1541:
	if (track > MAX_TRACKS_1541 || sector >= sector_map_1541[track])
	    return (-1);

	for (i = 1; i < track; i++) {
	    sectors += sector_map_1541[i];
	}

	sectors += sector;
	break;

      case 1571:
	if (track > MAX_TRACKS_1571)
	    return (-1);

	if (track > NUM_TRACKS_1541) {		/* The second side */
	    track -= NUM_TRACKS_1541;
	    sectors = NUM_BLOCKS_1541;
	}

	if (sector >= sector_map_1541[track])
	    return (-1);

	for (i = 1; i < track; i++) {
	    sectors += sector_map_1541[i];
	}

	sectors += sector;
	break;

      case 1581:
	if (track > MAX_TRACKS_1581 || sector >=  NUM_SECTORS_1581)
	    return (-1);

	sectors = track * NUM_SECTORS_1581 + sector;
	break;


      /* PET IEEE488 Drives. */

      case 8050:
	if (track > MAX_TRACKS_8050 || sector >= pet_sector_map[track])
	    return (-1);

	for (i = 1; i < track; i++) {
	    sectors += pet_sector_map[i];
	}

	sectors += sector;
	break;

      case 8250:
	if (track > MAX_TRACKS_8250)
	    return (-1);

	if (track > NUM_TRACKS_8050) {		/* The second side */
	    track -= NUM_TRACKS_8050;
	    sectors = NUM_BLOCKS_8050;
	}

	if (sector >= pet_sector_map[track])
	    return (-1);

	for (i = 1; i < track; i++) {
	    sectors += pet_sector_map[i];
	}

	sectors += sector;
	break;


      default:
	sectors = -1;
    }

    return (sectors);
}


/*
 * Return block offset on 'disk file' according to track and sector
 */

static off_t offset_from_track_and_sector(int format, int track,
					  int sector, int d64)
{
    int     sectors;

    if ((sectors = check_track_sector(format, track, sector)) < 0)
	return -1;

#ifdef DEBUG_DRIVE
    printf("DEBUG SECTOR %3d (%2d,%2d) OFF_T: %ld\n",
	   sectors, track, sector,
	   ((off_t)sectors << 8) /* + (off_t)HEADER_LENGTH*/ );
#endif

    return ( ((off_t)sectors << 8) +
		(off_t)(d64?0:HEADER_LENGTH) );

}


/*
 * Read one block
 */

int  floppy_read_block(int fd, int format, BYTE *buf, int track,
		       int sector, int d64)
{
    off_t   offset;

    offset = offset_from_track_and_sector(format, track, sector, d64);

    if (offset < 0)
	return -1;

    lseek(fd, offset, SEEK_SET);

    if (read(fd, (char *)buf, 256) < 256) {
	fprintf (stderr, "Floppy image file read failed.\n");
	return(-2);
    }
    return 0;
}


/*
 * Write one block
 */

int  floppy_write_block(int fd, int format, BYTE *buf, int track,
			int sector, int d64)
{
    off_t   offset;

    offset = offset_from_track_and_sector(format, track, sector, d64);

    if (offset < 0)
	return -1;

    if (lseek(fd, offset, SEEK_SET) < 0
	|| write(fd, (char *)buf, 256) < 0) {
	perror("floppy_write_block");
	return(-2);
    }

    return 0;
}


static int  write_sequential_buffer(DRIVE *floppy, bufferinfo_t *bi,
				    int length )
{
    int     t_new, s_new;
    int     e;
    BYTE   *buf = bi->buffer;
    BYTE   *slot = bi->slot;

    /*
     * First block of a file ?
     */
    if (slot[SLOT_FIRST_TRACK] == 0) {
	e = alloc_first_free_sector(floppy->bam, &t_new, &s_new);
	if (e < 0) {
	    floppy_error(&floppy->buffers[15], IPE_DISK_FULL, 0, 0);
	    return -1;
	}
	slot[SLOT_FIRST_TRACK]  = bi->track  = t_new;
	slot[SLOT_FIRST_SECTOR] = bi->sector = s_new;
    }

    if (length == WRITE_BLOCK) {
	/*
	 * Write current sector and allocate next
	 */
	t_new = bi->track;
	s_new = bi->sector;
	e = alloc_next_free_sector(floppy->bam, &t_new, &s_new);
	if (e < 0) {
	    floppy_error(&floppy->buffers[15], IPE_DISK_FULL, 0, 0);
	    return -1;
	}
	buf[0] = t_new;
	buf[1] = s_new;

	floppy_write_block(floppy->ActiveFd, floppy->ImageFormat, buf,
				bi->track, bi->sector, floppy->D64_Header);

	bi->track = t_new;
	bi->sector = s_new;
    } else {
	/*
	 * Write last block
	 */
	buf[0] = 0;
	buf[1] = length - 1;

	floppy_write_block(floppy->ActiveFd, floppy->ImageFormat, buf,
				bi->track, bi->sector, floppy->D64_Header);
    }

    if (!(++slot[SLOT_NR_BLOCKS]))
	++slot[SLOT_NR_BLOCKS + 1];

    return 0;
}


/* ------------------------------------------------------------------------- */

/*
 * Disk Block availability management
 */


/*
 * Initialize Directory Slot find
 */

static void set_find_first_slot(DRIVE *floppy, char *name, int length, int type)
{
    floppy->find_name   = name;
    floppy->find_type   = type;
    floppy->find_length = length;


#ifdef DEBUG_DRIVE
    if (name) printf ("name = >%s<  *name = $%02x\n", name, *name);
    printf("Prepare seeking entry for '%s' of type %d. (len =%d)\n",
	   (name ? name : "<noname>"),
	   type, length);
#endif

    /*
     * Make sure find_next_slot() loads first directory block ...
     * Emulate chaining behaviour of the 1541  -- GEC
     */

    floppy->Curr_track  = floppy->Dir_Track;
    floppy->Curr_sector = floppy->Dir_Sector;
    floppy->SlotNumber = -1;

    floppy_read_block(floppy->ActiveFd, floppy->ImageFormat,
		      floppy->Dir_buffer,
		      floppy->Dir_Track, floppy->Dir_Sector,
		      floppy->D64_Header);
}


static BYTE *find_next_slot(DRIVE *floppy)
{
    static BYTE return_slot[32];
    int     status;

    floppy->SlotNumber++;

#ifdef DEBUG_DRIVE
    printf("find next slot for '%s' of type %d. slot: %d\n",
	   (floppy->find_name ? floppy->find_name : "<noname>"),
	   floppy->find_type, floppy->SlotNumber);
#endif

    /*
     * Loop all directory blocks starting from track 18, sector 1 (1541).
     */

    do {
	/*
	 * Load next(first) directory block ?
	 */

	if (floppy->SlotNumber >= 8) {
	    if (!(*(floppy->Dir_buffer))) {
#ifdef DEBUG_DRIVE
		printf("error.\n");
#endif
		return NULL;
	    }

	    floppy->SlotNumber = 0;

	    floppy->Curr_track  = (int) floppy->Dir_buffer[0];
	    floppy->Curr_sector = (int) floppy->Dir_buffer[1];

	    status = floppy_read_block(floppy->ActiveFd, floppy->ImageFormat,
				       floppy->Dir_buffer,
				       floppy->Curr_track,
				       floppy->Curr_sector,
				       floppy->D64_Header);
	}

	while (floppy->SlotNumber < 8) {
	    if (floppy_name_match(&floppy->Dir_buffer[floppy->SlotNumber * 32],
				  floppy->find_name, floppy->find_length,
				  floppy->find_type)) {
		memcpy(return_slot,
		    &floppy->Dir_buffer[floppy->SlotNumber * 32], 32);
#ifdef DEBUG_DRIVE
		printf("found slot %d  on %d %d.\n",
		       floppy->SlotNumber,
		       floppy->Curr_track,
		       floppy->Curr_sector);
#endif
		return return_slot;
	    }
	    floppy->SlotNumber++;
	}
    } while (*(floppy->Dir_buffer));


    /*
     * If length < 0, create new directory-entry if possible
     */

    if (floppy->find_length < 0) {
	int     s;

#ifdef DEBUG_DRIVE
		printf("create a new entry.\n");
#endif

	for (s = 1; s < sector_map_1541[DSK_DIR_TRACK]; s++) {
	    if (allocate_sector(floppy->bam, DSK_DIR_TRACK, s)) {
		floppy->Dir_buffer[0] = DSK_DIR_TRACK;
		floppy->Dir_buffer[1] = s;
		floppy_write_block(floppy->ActiveFd, floppy->ImageFormat,
				   floppy->Dir_buffer,
				   floppy->Curr_track,
				   floppy->Curr_sector,
				   floppy->D64_Header);

#ifdef DEBUG_DRIVE
		printf("Found (%d %d) TR = %d SE = %d.\n",
		       DSK_DIR_TRACK, s,
		       floppy->Curr_track,
		       floppy->Curr_sector);
#endif

		floppy->SlotNumber = 0;
		memset(floppy->Dir_buffer, 0, 256);
		floppy->Dir_buffer[1] = 0xff;
		floppy->Curr_sector = s;
		return floppy->Dir_buffer;
	    }
	}

    } /* length */

    return NULL;
}


static void free_chain(DRIVE *floppy, int t, int s)
{
    BYTE buf[256];

    while (t) {
#ifdef DEBUG_DRIVE
	printf("free_chain free %d,%d\n", t, s);
#endif
	if (check_track_sector(floppy->ImageFormat, t, s) < 0) {
	    /* ILLEGAL TRACK OR SECTOR */
	    break;
	}
	if (!free_sector(floppy->bam, t, s)) {
	    /* NO BLOCK */
	    break;
	}

	free_sector(floppy->bam, t, s);
	floppy_read_block(floppy->ActiveFd, floppy->ImageFormat, buf, t, s,
						floppy->D64_Header);
	t = (int) buf[0];
	s = (int) buf[1];
    }
}


/*
 * remove_slot() is called from open_1541() (in 'save and replace')
 * and from ip_execute() for 'SCRATCH'.
 */

static void remove_slot(DRIVE *floppy, BYTE *slot)
{
    int     tmp;
    int     t, s;

    /*
     * Find slot
     */
    for (tmp = 0; (tmp < 16) && slot[SLOT_NAME_OFFSET + tmp] != 0xa0; tmp++)
	;

    set_find_first_slot(floppy,
			(char *)&slot[SLOT_NAME_OFFSET], tmp,
			slot[SLOT_TYPE_OFFSET] & 0x07);

    /*
     * If slot slot found, remove
     */
    if (find_next_slot(floppy)) {

	/*
	 * Free all buffers from bam
	 */
	t = (int) floppy->Dir_buffer[floppy->SlotNumber * 32
				     + SLOT_FIRST_TRACK];
	s = (int) floppy->Dir_buffer[floppy->SlotNumber * 32
				     + SLOT_FIRST_SECTOR];

	free_chain(floppy, t, s);

	t = (int) floppy->Dir_buffer[floppy->SlotNumber * 32
				     + SLOT_SIDE_TRACK];
	s = (int) floppy->Dir_buffer[floppy->SlotNumber * 32
				     + SLOT_SIDE_SECTOR];

	free_chain(floppy, t, s);

	/* Update bam */
	floppy_write_bam(floppy);

	/* Update directory entry */
	floppy->Dir_buffer[floppy->SlotNumber * 32 + SLOT_TYPE_OFFSET] = 0;
	floppy_write_block(floppy->ActiveFd, floppy->ImageFormat,
			   floppy->Dir_buffer,
			   floppy->Curr_track,
			   floppy->Curr_sector,
				   floppy->D64_Header);
    }
}


/* ------------------------------------------------------------------------- */

#ifndef MAX
#define MAX(a, b)	((a) > (b)? (a) : (b))
#endif

/* XXX warning XXX
 * These BAM functions ignore formats other than 1541 because
 * in their current form they could not possibly support the
 * 1571/8050/8250/etc: DSK_DIR_TRACK is different, they have multiple
 * BAM sectors, 8250 allocation strategy is different, etc.
 */

/* How many tracks there can be at one side of the directory track */

#define DSK_HALF_MAX_TRACKS	MAX(DSK_DIR_TRACK-1, MAX_TRACKS_1541-DSK_DIR_TRACK)

/*
 * This algorithm is used to select a free first sector
 */
static int  alloc_first_free_sector(BYTE *bam, int *track, int *sector)
{
    int     t, s;
    int     d;

    for (d = 1; d <= DSK_HALF_MAX_TRACKS; d++) {
	t = DSK_DIR_TRACK - d;
#ifdef DEBUG_DRIVE
	printf("alloc_first_free_sector on track %d?\n", t);
#endif
	if (t < 1)
	    continue;
	for (s = 0; s < sector_map_1541[t]; s++) {
	    if (allocate_sector(bam, t, s)) {
		*track = t;
		*sector = s;
#ifdef DEBUG_DRIVE
		printf("alloc_first_free_sector: %d,%d\n", t, s);
#endif
		return 0;
	    }
	}
	t = DSK_DIR_TRACK + d;
#ifdef DEBUG_DRIVE
	printf("alloc_first_free_sector on track %d?\n", t);
#endif
	if (t > MAX_TRACKS_1541)
	    continue;
	for (s = 0; s < sector_map_1541[t]; s++) {
	    if (allocate_sector(bam, t, s)) {
		*track = t;
		*sector = s;
#ifdef DEBUG_DRIVE
		printf("alloc_first_free_sector: %d,%d\n", t, s);
#endif
		return 0;
	    }
	}
    }
    return -1;
}


/*
 * This algorithm is used to select a continuation sector.
 * track and sector must be given.
 * XXX the interleave is not taken into account yet.
 */
static int  alloc_next_free_sector(BYTE *bam, int *track, int *sector)
{
    int     t, s;
    int     d;
    int     dir;
    int     diskhalf;

    if (*track < DSK_DIR_TRACK) {
	dir = -1;
	d = DSK_DIR_TRACK - *track;
    } else {
	dir = 1;
	d = *track - DSK_DIR_TRACK;
    }

    for (diskhalf = 0; diskhalf < 2; diskhalf++) {
	for (; d <= DSK_HALF_MAX_TRACKS; d++) {
	    t = DSK_DIR_TRACK + dir * d;
#ifdef DEBUG_DRIVE
	    printf("alloc_next_free_sector on track %d?\n", t);
#endif
	    if (t < 1 || t > MAX_TRACKS_1541) {
		dir = -dir;
		d = 1;
		break;
	    }
	    for (s = 0; s < sector_map_1541[t]; s++) {
		if (allocate_sector(bam, t, s)) {
		    *track = t;
		    *sector = s;
#ifdef DEBUG_DRIVE
		    printf("alloc_next_free_sector: %d,%d\n", t, s);
#endif
		    return 0;
		}
	    }
	}
    }
    return -1;
}

int  allocate_sector(BYTE *bam, int track, int sector)
{
    BYTE   *bamp;	/* Macros use this */

    bamp = (track <= NUM_TRACKS_1541) ?
	&bam[BAM_BIT_MAP + 4 * (track - 1)] :
	&bam[BAM_EXT_BIT_MAP + 4 * (track - NUM_TRACKS_1541 - 1)];

    if (BAM_ISSET(sector)) {
	(*bamp)--;
	BAM_CLR(sector);

	return 1;
    }
    return 0;
}


int  free_sector(BYTE *bam, int track, int sector)
{
    BYTE   *bamp;

    bamp = (track <= NUM_TRACKS_1541) ?
	&bam[BAM_BIT_MAP + 4 * (track - 1)] :
	&bam[BAM_EXT_BIT_MAP + 4 * (track - NUM_TRACKS_1541 - 1)];

    if (!(BAM_ISSET(sector))) {
	BAM_SET(sector);
	(*bamp)++;
	return 1;
    }
    return 0;
}


/* ------------------------------------------------------------------------- */

/*
 * Load/Store BAM Image.
 */


static int  floppy_read_bam (DRIVE *floppy)
{
  return floppy_read_block(floppy->ActiveFd, floppy->ImageFormat,
			   floppy->bam, DSK_BAM_TRACK, DSK_BAM_SECTOR,
			   floppy->D64_Header);
}


static int  floppy_write_bam (DRIVE *floppy)
{
  return floppy_write_block(floppy->ActiveFd, floppy->ImageFormat,
			    floppy->bam, DSK_BAM_TRACK, DSK_BAM_SECTOR,
			    floppy->D64_Header);
}


/* ------------------------------------------------------------------------- */

/*
 * Functions to attach the disk image files.
 */


void    detach_floppy_image(DRIVE *floppy)
{
    floppy_close_all_channels(floppy);

    if (floppy->ActiveFd >= 0) {
	printf("Detaching disk image %s\n", floppy->ActiveName);
	if (floppy->detach_func != NULL)
	    floppy->detach_func(floppy);
	zclose(floppy->ActiveFd);
	floppy->ActiveFd = -1;
	floppy->ActiveName[0] = 0;		/* Name is used as flag */
    }
}


int     attach_floppy_image(DRIVE *floppy, const char *name, int mode)
{
    int     DType = -1;
    int     fd;
    hdrinfo hdr;


    if ( !(floppy -> type & DT_DISK)) {
	printf("Incompatible emulator mode: FS 1541 Drive.\n");
	return (-1);
    }

    if (!name) {
	printf("No name, detaching floppyimage\n");
	return (-1);
    }

    fd = zopen(name, O_RDWR, 0);

    /* If we cannot open the image read/write, try to open it read only. */
    if (fd < 0) {
	fd = zopen(name, O_RDONLY, 0);
	floppy->ReadOnly = 1;
    } else {
	floppy->ReadOnly = 0;
    }

    if (fd >= 0) {
	if (check_header(fd, &hdr)) {
	    printf("File '%s' was not recognized as a disk image\n", name);
	    zclose(fd);
	    return (-2);
	}
	if (hdr.v_major > HEADER_VERSION_MAJOR
	    || (hdr.v_major == HEADER_VERSION_MAJOR
		&& hdr.v_minor > HEADER_VERSION_MINOR)) {
	    printf("Disk image file %s (V %d.%02d) version higher than emulator (V %d.%02d)\n",
		   name, hdr.v_major, hdr.v_minor,
		   HEADER_VERSION_MAJOR, HEADER_VERSION_MINOR);

	    zclose(fd);
	    return (-2);
	}


	/*
	 * Check that the disk format on the image is compatible.
	 */

	DType = get_diskformat (hdr.devtype);
	if (DType < 0 || DType != get_diskformat (hdr.devtype)) {

	    fprintf(stderr, "\nError: Disk drive type mismatch.\n");

	    /*
	     * mode defines what to do if the attached disk image is
	     * altered or incompatible
	     */

	    return (-2);
	}

	detach_floppy_image(floppy);

	floppy->ImageFormat = DType;
	floppy->ActiveFd = fd;
	strcpy(floppy->ActiveName, name);
	/* floppy->changed  = 0; */

        floppy->NumTracks  = hdr.tracks;
        floppy->NumBlocks  = num_blocks (DType, hdr.tracks);
        floppy->ErrFlg     = hdr.errblk;
        floppy->D64_Header = hdr.d64 | hdr.d71 | hdr.d81;
        floppy->GCR_Header = hdr.gcr;

        if (!hdr.gcr) {
            /* Initialise format constants */
            set_disk_geometry(floppy, DType);

            /* Initialize */
            if (hdr.errblk)
                set_error_data(floppy, 3);      /* clear or read error data */

            floppy_read_bam (floppy);
        }

        if (hdr.d64)
            printf ("D64 disk image attached: %s%s\n", name,
                    floppy->ReadOnly ? " (read only)" : "");
        if (hdr.d71)
            printf ("D71 disk image attached: %s%s\n", name,
                    floppy->ReadOnly ? " (read only)" : "");
        if (hdr.d81)
            printf ("D81 disk image attached: %s%s\n", name,
                    floppy->ReadOnly ? " (read only)" : "");
        if (hdr.gcr)
            printf ("GCR disk image attached: %s%s\n", name,
                    floppy->ReadOnly ? " (read only)" : "");

        if (!(hdr.d64 | hdr.d71 | hdr.d81 | hdr.gcr))
            printf ("VICE disk image version %d.%02d attached (CBM%d format%s):\n%s\n",
                    hdr.v_major, hdr.v_minor, DType,
                    floppy->ReadOnly ? ", read only" : "",
                    name);

    } else {
	fprintf(stderr, "Couldn't open file %s\n",name);
	return -1;
    }

    if (floppy->attach_func != NULL)
        floppy->attach_func(floppy);

    return (0);
}


/* ------------------------------------------------------------------------- */

/*
 * Information Acquisition routines.
 */

int get_std64_header(int fd, BYTE *header)
{
    int devtype = DEFAULT_DEVICE_TYPE;
    int tracks = NUM_TRACKS_1541;
    int blk = NUM_BLOCKS_1541-1;
    int len, errblk;
    char block[256];

    memset(header, 0, HEADER_LENGTH);

    /* Check values */

    if (check_track_sector(get_diskformat (devtype), tracks, 1) < 0)
        exit (-1);

    header[HEADER_MAGIC_OFFSET + 0] = HEADER_MAGIC_1;
    header[HEADER_MAGIC_OFFSET + 1] = HEADER_MAGIC_2;
    header[HEADER_MAGIC_OFFSET + 2] = HEADER_MAGIC_3;
    header[HEADER_MAGIC_OFFSET + 3] = HEADER_MAGIC_4;

    header[HEADER_VERSION_OFFSET + 0] = HEADER_VERSION_MAJOR;
    header[HEADER_VERSION_OFFSET + 1] = HEADER_VERSION_MINOR;

    header[HEADER_FLAGS_OFFSET + 0] = devtype;
    header[HEADER_FLAGS_OFFSET + 1] = tracks;

    if (lseek(fd, 256*blk, SEEK_SET)<0)
	return FD_BADIMAGE;
    while ((len = read(fd, block, 256)) == 256) {
        if (++blk > 771) {
            printf("Nice try.\n");
            break;
        }
    }
    if (blk <  NUM_BLOCKS_1541) {
            printf("Cannot read block %d\n", blk);
            return (FD_NOTRD);
    }

    switch (blk) {
      case 683:
        tracks = NUM_TRACKS_1541;
        errblk = 0;
        break;

      case 685:
        if (len != 171) {               /* check if 683 error bytes */
            printf("cannot read block %d\n", blk);
            return (FD_NOTRD);
        }

        tracks = NUM_TRACKS_1541;
        errblk = 1;
        break;

      case 768:
        tracks = EXT_TRACKS_1541;
        errblk = 0;
        break;

      case 771:
        tracks = EXT_TRACKS_1541;
        errblk = 1;
        break;

      default:
        return (FD_BADIMAGE);

    }
    header[HEADER_FLAGS_OFFSET+0] = DEFAULT_DEVICE_TYPE;
    header[HEADER_FLAGS_OFFSET+1] = tracks;
    header[HEADER_FLAGS_OFFSET+2] = 0;
    header[HEADER_FLAGS_OFFSET+3] = errblk;

    return(0);
}

int get_std71_header(int fd, BYTE *header)
{
    int devtype = DT_1571;
    int tracks = NUM_TRACKS_1571;
    int blk = NUM_BLOCKS_1571-1;
    int len, errblk;
    char block[256];

    memset(header, 0, HEADER_LENGTH);

    /* Check values */

    if (check_track_sector(get_diskformat (devtype), tracks, 1) < 0)
        exit (-1);

    header[HEADER_MAGIC_OFFSET + 0] = HEADER_MAGIC_1;
    header[HEADER_MAGIC_OFFSET + 1] = HEADER_MAGIC_2;
    header[HEADER_MAGIC_OFFSET + 2] = HEADER_MAGIC_3;
    header[HEADER_MAGIC_OFFSET + 3] = HEADER_MAGIC_4;

    header[HEADER_VERSION_OFFSET + 0] = HEADER_VERSION_MAJOR;
    header[HEADER_VERSION_OFFSET + 1] = HEADER_VERSION_MINOR;

    header[HEADER_FLAGS_OFFSET + 0] = devtype;
    header[HEADER_FLAGS_OFFSET + 1] = tracks;

    if (lseek(fd, 256*blk, SEEK_SET)<0)
        return FD_BADIMAGE;
    while ((len = read(fd, block, 256)) == 256) {
        if (++blk > 1372) {
            printf("Nice try.\n");
            break;
        }
    }
    if (blk <  NUM_BLOCKS_1571) {
            printf("Cannot read block %d\n", blk);
            return (FD_NOTRD);
    }
    switch (blk) {
      case 1366:
        tracks = NUM_TRACKS_1571;
        errblk = 0;
        break;
      default:
        return (FD_BADIMAGE);

    }
    header[HEADER_FLAGS_OFFSET+0] = DT_1571;
    header[HEADER_FLAGS_OFFSET+1] = tracks;
    header[HEADER_FLAGS_OFFSET+2] = 0;
    header[HEADER_FLAGS_OFFSET+3] = errblk;

    return(0);
}

int    check_header(int fd, hdrinfo *hdr)
{
    BYTE    header[HEADER_LENGTH];

    lseek (fd, (off_t) 0, SEEK_SET);
    if (read(fd, (BYTE *)header, sizeof (header)) != sizeof (header)) {
        fprintf(stderr, "\nCannot read image header\n");
        return FD_RDERR;
    }

    memset (hdr, 0, sizeof(hdrinfo));

    hdr->d64 = 0;
    hdr->d71 = 0;
    hdr->d81 = 0;
    hdr->gcr = 0;

    if (header[HEADER_MAGIC_OFFSET + 0] != HEADER_MAGIC_1 ||
        header[HEADER_MAGIC_OFFSET + 1] != HEADER_MAGIC_2 ||
        header[HEADER_MAGIC_OFFSET + 2] != HEADER_MAGIC_3 ||
        header[HEADER_MAGIC_OFFSET + 3] != HEADER_MAGIC_4) {

        struct stat s;

        if (!fstat (fd, &s)) {
            if (IS_D64_LEN(s.st_size)) {
                if (get_std64_header(fd, header))
                    return FD_BADIMAGE;
                hdr->d64 = 1;
            } else if (IS_D71_LEN(s.st_size)) {
                if (get_std71_header(fd, header))
                    return FD_BADIMAGE;
                hdr->d71 = 1;
            } else {
                if (import_GCR_image(header, hdr))
                    return FD_OK;
                else
                    return FD_BADIMAGE;
            }
        } else {
            perror ("stat");
            return FD_BADIMAGE;
        }
    }

    hdr-> v_major = header[HEADER_VERSION_OFFSET + 0];
    hdr-> v_minor = header[HEADER_VERSION_OFFSET + 1];

  /* Disk type flags: Device Type, Max Tracks, Side, and Error Flag. */

    hdr-> devtype = header[HEADER_FLAGS_OFFSET];
    hdr-> tracks  = header[HEADER_FLAGS_OFFSET + 1];
    hdr-> sides   = (header[HEADER_FLAGS_OFFSET + 2] ? 2 : 1);
    hdr-> errblk  = header[HEADER_FLAGS_OFFSET + 3];
    hdr-> format  = (get_diskformat(hdr-> devtype)) == 1581 ? 'D' : 'A';

    if (hdr-> tracks == 0)
        hdr-> tracks = 35;


    if (check_track_sector(get_diskformat(hdr-> devtype), hdr-> tracks, 1) < 0)
        return FD_BADIMAGE;


    strncpy (hdr->description,
             (char *)header + HEADER_LABEL_OFFSET, HEADER_LABEL_LEN);

    return FD_OK;
}

int import_GCR_image(BYTE *header, hdrinfo *hdr)
{
    int trackfield;

    if (strncmp("GCR-1541",header,8))
	return 0;

    if (header[8] != 0) {
	printf("Import GCR: Wrong GCR image version.\n");
	return 0;
    }

    if ((header[9] < (NUM_TRACKS_1541 * 2))
			|| (header[9] > (MAX_TRACKS_1541 * 2))) {
	printf("Import GCR: invalid number of tracks.\n");
	return 0;
    }

    trackfield = header[10] + header[11] * 256;
    if (trackfield != 7928) {
	printf("Import GCR: invalid track field number.\n");
	return 0;
    }

    hdr->tracks = header[9] / 2;
    hdr->format = 1541;
    hdr->gcr = 1;
    hdr->v_major = HEADER_VERSION_MAJOR;
    hdr->v_minor = HEADER_VERSION_MINOR;
    hdr->devtype = DEFAULT_DEVICE_TYPE;

    return 1;
}

/*
 * This routine is from fvcbm by Dan Fandrich.
 */

int  get_diskformat (int devtype)
{
    int     DiskType = -1;


    /*
     * Get the group of compatible disk formats.
     */

    switch (devtype & DT_MASK) {
      case DT_2031:
	/*case DT_4031:*/
      case DT_2040:
	/*case DT_3040:*/
      case DT_2041:
      case DT_4040:
      case DT_1540:
      case DT_1541:
      case DT_1542:
      case DT_1551:
      case DT_1570:	DiskType = 1541; break;

      case DT_1571:
      case DT_1572:	DiskType = 1571; break;

      case DT_1581:	DiskType = 1581; break;


      case DT_8050:	DiskType = 8050; break;

      case DT_8060:
      case DT_8061:	break;

      case DT_SFD1001:
      case DT_8250:	DiskType = 8250; break;

      /*case DT_8280:*/
    }

    return (DiskType);
}


/*
 * Calculate and return the total number of blocks available on a disk.
 */

int  num_blocks (int format, int tracks)
{
    int     blocks = -1;


    switch (format) {
      case 1541:
	if (tracks > MAX_TRACKS_1541)
	    tracks = MAX_TRACKS_1541;
	blocks = NUM_BLOCKS_1541 + (tracks - 35) * 17;
	break;

      case 1571:
	if (tracks > MAX_TRACKS_1571)
	    tracks = MAX_TRACKS_1571;
	blocks = NUM_BLOCKS_1571 + (tracks - 70) * 17;
	break;

      case 1581:
	blocks = NUM_BLOCKS_1581;
	break;

      case 8050:
        blocks = NUM_BLOCKS_8050;
	break;

      case 8250:
        blocks = NUM_BLOCKS_8250;
	break;

    }

    return (blocks);
}

/* ------------------------------------------------------------------------- */

/*
 * Misc functions
 */

static int  mystrncpy(BYTE *d, BYTE *s, int n)
{
    while (n-- && *s)
	*d++ = *s++;
    return (n);
}


void no_a0_pads(BYTE *ptr, int l)
{
    while (l--) {
	if (*ptr == 0xa0)
	    *ptr = 0x20;
	ptr++;
    }
}

/* ------------------------------------------------------------------------- */

/*
 * Initialise format constants
 */

void set_disk_geometry(DRIVE *floppy, int type)
{
    if (get_diskformat(type) == 1581) {
	floppy->Bam_Track  = 38;
	floppy->Bam_Sector = 1;
	floppy->Dir_Track  = 38;
	floppy->Dir_Sector = 3;
    } else {
	floppy->Bam_Track  = 18;
	floppy->Bam_Sector = 0;
	floppy->Dir_Track  = 18;
	floppy->Dir_Sector = 1;
    }
}

/* ------------------------------------------------------------------------- */

/* Wild-card match routine
 *
 * This routine takes two string parameters, the path to be macthed and
 * a pattern to match it with. There are two wild-cards - the ? and *.
 * A question mark will match exactly one character. An asterisk will
 * match any number (or zero) characters. Each '*' introduces one level
 * of recursion. A backslash forces the next character to be taken verbatim.
 * However, a null character cannot be protected.
 */

int  compare_filename (char *name, char *pattern)
{
    char *p, *q;
    int   literal = 0;


    p = pattern;
    q = name;

    while (*p && *q) {

	if (!literal) {
	    if (*p == '?') {
		++p;
		++q;
		continue;
	    }
	    if (*p == '*') {
		while (*++p == '*');
		if (!*p)
		    return (1);		/* End of pattern -> matches */

		while (!compare_filename (q, p) && *++q);
		return (*q);		/* if *q > 0 it must have matched */
	    }
	    if (*p == '\\') {
		++p;
		++literal;
		continue;
	    }
	}  /* literal */
	else {
	    literal = 0;
	}

	if (*p == *q) {
	    ++p;
	    ++q;
	}
	else
	    return (0);			/* No match */

    }  /* while */

    return (!*p && !*q);		/* Match */
}

/* ------------------------------------------------------------------------- */

/* Read the directory and return it as a long malloc'ed ASCII string.
   FIXME: Should probably be made more robust.  */
char *floppy_read_directory(DRIVE *floppy, const char *pattern)
{
    BYTE *p;
    int outbuf_size, max_outbuf_size, len;
    char line[256], *outbuf;

    /* Open the directory. */
    if (pattern != NULL)
	sprintf(line, "$:%s", pattern);
    else
	sprintf(line, "$");
    if (open_1541(floppy, line, 1, 0) != SERIAL_OK)
	return NULL;

    /* Allocate a buffer. */
    max_outbuf_size = 4096;
    outbuf = xmalloc(max_outbuf_size);
    outbuf_size = 0;

    p = floppy->buffers[0].buffer + 2; /* Skip load address. */
    while ((p[0] | (p[1] << 8)) != 0) {
	len = sprintf(line, "%d ", p[2] | (p[3] << 8));
	p += 4;
	while (*p != '\0') {
	    line[len++] = p_toascii(*p, 0);
	    p++;
	}
	p++;
	line[len++] = '\n';
	bufcat(outbuf, &outbuf_size, &max_outbuf_size, line, len);
    }
    close_1541(floppy, 0);

    bufcat(outbuf, &outbuf_size, &max_outbuf_size, "\n", 1);

    /* Add trailing zero. */
    *(outbuf + outbuf_size) = '\0';

    return outbuf;
}

/* Read the directory of a named disk image and return it as a long malloc'ed
   ASCII string. */
char *read_disk_image_contents(const char *fname)
{
    static BYTE fake_command_buffer[256];
    DRIVE floppy;
    char *buf;
    hdrinfo hdr;
    int fd;

    fd = zopen(fname, O_RDONLY, 0);
    if (fd < 0)
	return NULL;
    if (check_header(fd, &hdr))
	return NULL;

    if (hdr.v_major > HEADER_VERSION_MAJOR
	|| (hdr.v_major == HEADER_VERSION_MAJOR
	    && hdr.v_minor > HEADER_VERSION_MINOR)) {
	printf("Disk image file %s (V %d.%02d) version higher than emulator (V %d.%02d)\n",
	       fname, hdr.v_major, hdr.v_minor,
	       HEADER_VERSION_MAJOR, HEADER_VERSION_MINOR);

	zclose(fd);
	return 0;
    }

    /* Initialize floppy. */
    floppy.ImageFormat = get_diskformat(hdr.devtype);
    if (floppy.ImageFormat < 0) {
	fprintf(stderr, "Error: unknown image format.\n");
	zclose(fd);
	return NULL;
    }
    floppy.ActiveFd = fd;
    floppy.NumTracks = hdr.tracks;
    floppy.NumBlocks = num_blocks(floppy.ImageFormat, hdr.tracks);
    floppy.ErrFlg    = hdr.errblk;
    floppy.D64_Header= hdr.d64 | hdr.d71 | hdr.d81;
    floppy.ReadOnly = 1;	/* Just to be sure... */
    if (floppy_read_bam(&floppy) < 0) {
	fprintf(stderr, "Error: cannot read BAM.\n");
	zclose(fd);
	return NULL;
    }

    /* This fake is necessary to `open_1541'...  Ugly, but that is the only
       way I know with the existing functions. */
    floppy.buffers[15].mode = BUFFER_COMMAND_CHANNEL;
    floppy.buffers[15].buffer = fake_command_buffer;
    floppy.buffers[0].mode = BUFFER_NOT_IN_USE;

    /* Initialize format constants. */
    set_disk_geometry(&floppy, floppy.ImageFormat);

    /* Now we can actually read. */
    buf = floppy_read_directory(&floppy, NULL);

    zclose(fd);
    return buf;
}

