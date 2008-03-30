/*
 * vdrive.c - Virtual disk-drive implementation.
 *
 * Written by
 *  Andreas Boose       <boose@linux.rz.fh-hannover.de>
 *
 * Based on old code by
 *  Teemu Rantanen      <tvr@cs.hut.fi>
 *  Jarkko Sonninen     <sonninen@lut.fi>
 *  Jouko Valta         <jopi@stekt.oulu.fi>
 *  Olaf Seibert        <rhialto@mbfys.kun.nl>
 *  André Fachat        <a.fachat@physik.tu-chemnitz.de>
 *  Ettore Perazzoli    <ettore@comm2000.it>
 *  Martin Pottendorfer <Martin.Pottendorfer@aut.alcatel.at>
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

#ifdef STDC_HEADERS
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#ifdef __riscos
#include "ROlib.h"
#include "ui.h"
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <memory.h>
#endif
#include <errno.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "archdep.h"
#include "charsets.h"
#include "drive.h"
#include "fsdevice.h"
#include "log.h"
#include "serial.h"
#include "utils.h"
#include "vdrive-bam.h"
#include "vdrive-dir.h"
#include "vdrive-iec.h"
#include "vdrive.h"
#include "zfile.h"

/* ------------------------------------------------------------------------- */

log_t vdrive_log = LOG_ERR;

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

char pet_sector_map[78] =
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

/* Keeps the number of entries deleted with the `S' command */
static int deleted_files;

/* PC64 files need this too */
char const *slot_type[] = {
    "DEL", "SEQ", "PRG", "USR", "REL", "CBM", "DJJ", "FAB"
};

static int  set_error_data(DRIVE *floppy, int flags);
static int  vdrive_command_block(DRIVE *floppy, char command, char *buffer);
static int  vdrive_command_memory(DRIVE *floppy, BYTE *buffer, int length);
static int  vdrive_command_initialize(DRIVE *floppy);
static int  vdrive_command_format(DRIVE *floppy, const char *name, BYTE *id,
            BYTE *minus);
static int  vdrive_command_copy(DRIVE *floppy, char *dest, int length);
static int  vdrive_command_rename(DRIVE *floppy, char *dest, int length);

static int  import_GCR_image(BYTE *header, hdrinfo *hdr);

/* ------------------------------------------------------------------------- */

/*
 * Error messages
 */

errortext_t floppy_error_messages[] =
{
    { 0, "OK"},
    { 1, "FILES SCRATCHED"},
    { 2, "SELECTED PARTITION"},           /* 1581 */
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
    {73, "VIRTUAL DRIVE EMULATION V2.2"}, /* The program version */
    {74, "DRIVE NOT READY"},
    {77, "SELECTED PARTITION ILLEGAL"},   /* 1581 */
    {80, "DIRECTORY NOT EMPTY"},
    {81, "PERMISSION DENIED"},
    {-1, 0}

};


/* ------------------------------------------------------------------------- */


int initialize_1541(int dev, int type,
                    drive_attach_func_t attach_func,
                    drive_detach_func_t detach_func,
                    DRIVE *oldinfo)
{
    DRIVE *floppy;
    int i;

    if (vdrive_log == LOG_ERR)
        vdrive_log = log_open("VDrive");

    floppy = oldinfo;

    /* Create instances of the disk drive.  */
    if (!floppy)
        floppy = (DRIVE *)xmalloc(sizeof(DRIVE));

    memset(floppy, 0, sizeof(DRIVE));  /* init all pointers */

    floppy->type     = type;
    floppy->ActiveFd = NULL;
    floppy->unit     = dev;

    for (i = 0; i < 15; i++)
        floppy->buffers[i].mode = BUFFER_NOT_IN_USE;

    floppy->buffers[15].mode = BUFFER_COMMAND_CHANNEL;
    floppy->buffers[15].buffer = (BYTE *)xmalloc(256);

    /* Initialise format constants.  */
    set_disk_geometry(floppy, type);

    /*
     * 'type' specifies what kind of emulation is selected
     */

    if (!(type & DT_FS)) {
        if (serial_attach_device(dev, (char *)floppy, "1541 Disk Drive",
                                 vdrive_read, vdrive_write,
                                 vdrive_open, vdrive_close,
                                 vdrive_flush)) {
            log_error(vdrive_log,
                      "Could not initialize virtual drive emulation for device #%d.", 
                      floppy->unit);
            return(-1);
        }
    } else {
        if (attach_fsdevice(dev, (char *)floppy, "1541 FS Drive")) {
            log_error(vdrive_log, "Could not initialize FS drive for device #%d.",
                    floppy->unit);
            return(-1);
        }
    }

    floppy->attach_func = attach_func;
    floppy->detach_func = detach_func;
    vdrive_command_set_error(&floppy->buffers[15], IPE_DOS_VERSION, 0, 0);
    return 0;
}

/* ------------------------------------------------------------------------- */

/*
 * Should set values to somewhere so that they could be read from
 * command channel
 */
void vdrive_command_set_error(bufferinfo_t *p, int code, int track, int sector)
{
    const char   *message;
    errortext_t *e;
    static int last_code;

#ifdef DEBUG_DRIVE
    printf("Set error channel: code =%d, last_code =%d, track =%d, sector =%d\n",
           code, last_code, track, sector);
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

    sprintf((char *)p->buffer, "%02d,%s,%02d,%02d\015",
            code == IPE_DELETED ? deleted_files : code,
            message, track, sector);

    /* length points to the last byte, and doesn't give the length... */
    p->length = strlen((char *)p->buffer)-1;
    p->bufptr = 0;

    if (code && code != IPE_DOS_VERSION)
        log_message(vdrive_log, "ERR = %02d, %s, %02d, %02d",
                    code == IPE_DELETED ? deleted_files : code,
                    message, track, sector);

    p->readmode = FAM_READ;
}


/* ------------------------------------------------------------------------- */

/*
 * Input Processor Simulator. IP interprets incoming commands and generates
 * the error messages according to return values.
 */

int vdrive_command_execute(DRIVE *floppy, BYTE *buf, int length)
{
    int status = IPE_OK;
    BYTE *p, *p2;
    char *name;
    BYTE *minus, *id;

    if (!length)
        return IPE_OK;
    if (length > IP_MAX_COMMAND_LEN) {
        vdrive_command_set_error(&floppy->buffers[15], IPE_LONG_LINE, 0, 0);
        return IPE_LONG_LINE;
    }

    p = (BYTE *)xmalloc(length + 1);
    memcpy(p, buf, length);

    if (p[length - 1] == 0x0d)
        --length; /* chop CR character */
    p[length] = 0;

    name = (char *)memchr(p, ':', length);
    id   = (BYTE *)memchr(p, ',', length);
    minus= (BYTE *)memchr(p, '-', length);

    if (name) /* Fix name length */
        for (p2 = p; *p2 && *p2 != ':' && length > 0; p2++, length--);

    switch (*p) {
      case 'C': /* Copy command.  */
        status = vdrive_command_copy(floppy, (char *)name, length);
        break;

      case 'D':		/* Backup unused */
        status = IPE_INVAL;
        break;

      case 'R':		/* Rename */
        status = vdrive_command_rename(floppy, (char *)name, length);
        break;

      case 'S':		/* Scratch */
        {
            BYTE *slot;
            char *realname = name;
            int reallength = 0, filetype = 0, readmode = 0;

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
                fprintf(errlog, "remove name= '%s' len=%d (%d) type= %d.\n",
                        realname, reallength, length, filetype);
#endif
                deleted_files = 0;

                /* Since vdrive_dir_remove_slot() uses 
                 * vdrive_dir_find_first_slot() too, we cannot find the
                 * matching files by simply repeating 
                 * vdrive_dir find_next_slot() calls alone; we have to re-call
                 * vdrive_dir_find_first_slot() each time... EP 1996/04/07 
                 */

                vdrive_dir_find_first_slot(floppy, realname, reallength, 0);
                while ((slot = vdrive_dir_find_next_slot(floppy))) {
                    vdrive_dir_remove_slot(floppy, slot);
                    deleted_files++;
                    vdrive_dir_find_first_slot(floppy, realname, reallength, 0);
                }
                if (deleted_files)
                    status = IPE_DELETED;
                else
                    status = IPE_NOT_FOUND;
                vdrive_command_set_error(&floppy->buffers[15], status, 1, 0);
            } /* else */
        }
        break;

      case 'I':
	status = vdrive_command_initialize(floppy);
	break;

      case 'N':
	status = vdrive_command_format(floppy, name, id, minus);
	break;

      case 'V':
	status = vdrive_command_validate(floppy);
	break;

      case 'B':		/* Block, Buffer */
        if (!name)	/* B-x does not require a : */
            name = (char *)(p + 2);
        if (!minus)
            status = IPE_INVAL;
	else
	    status = vdrive_command_block(floppy, minus[1], name + 1);
	break;

      case 'M':		/* Memory */
        if (!minus)     /* M-x does not allow a : */
            status = IPE_INVAL;
        else
            status = vdrive_command_memory(floppy, minus +1, length);
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
		    status = vdrive_command_block(floppy, 'R', name + 1);
 	        break;

 	      case 1: /* UB */
 	        /* XXX incorrect: U2 is not exactly the same as B-W */
 	        /*      -- should store the buffer pointer */
 	        if (name)
 		    status = vdrive_command_block(floppy, 'W', name + 1);
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
        log_error(vdrive_log, "Wrong command `%s'.", p);

    vdrive_command_set_error(&floppy->buffers[15], status, 0, 0);

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

static int vdrive_command_block(DRIVE *floppy, char command, char *buffer)
{
    int channel = 0, drive = 0, track = 0, sector = 0, position = 0;
    int l;

    switch (command) {
      case 'R':
      case 'W':
        l = get_block_parameters(buffer, &channel, &drive, &track, &sector);

        if (l < 0) {
#ifdef DEBUG_DRIVE
            fprintf(errfile, "B-R/W parsed ok. (l=%d) channel %d mode %d, "
                    "drive=%d, track=%d sector=%d\n", l, channel,
                    floppy->buffers[channel].mode, drive, track, sector);
#endif

            if (floppy->buffers[channel].mode != BUFFER_MEMORY_BUFFER)
                return IPE_NO_CHANNEL;

            if (command == 'W') {
                if (floppy->ReadOnly)
                    return IPE_WRITE_PROTECT_ON;
                if (floppy_write_block(floppy->ActiveFd, floppy->ImageFormat,
                                       floppy->buffers[channel].buffer,
                                       track, sector, floppy->D64_Header,
                                       floppy->GCR_Header, floppy->unit))
                    return IPE_NOT_READY;
            } else {
                if (floppy_read_block(floppy->ActiveFd, floppy->ImageFormat,
                                      floppy->buffers[channel].buffer,
                                      track, sector, floppy->D64_Header,
                                      floppy->GCR_Header, floppy->unit) < 0)
                    return IPE_NOT_READY;
            }
            floppy->buffers[channel].bufptr = 0;
        }
        break;
      case 'A':
      case 'F':
        l = get_block_parameters(buffer, &drive, &track, &sector, &channel);
        if (l > 0) /* just 3 args used */
            return l;
        if (command == 'A') {
            if (!vdrive_bam_allocate_sector(floppy->ImageFormat, floppy->bam,
                track, sector)) {
                /*
                 * Desired sector not free. Suggest another. XXX The 1541
                 * uses an inferior search function that only looks on
                 * higher tracks and can return sectors in the directory
                 * track.
                 */
                if (vdrive_bam_alloc_next_free_sector(floppy, floppy->bam,
                    &track, &sector) >= 0) {
                    /* Deallocate it and merely suggest it */
                    vdrive_bam_free_sector(floppy->ImageFormat, floppy->bam,
                                           track, sector);
                } else {
                    /* Found none */
                    track = 0;
                    sector = 0;
                }
                vdrive_command_set_error(&floppy->buffers[15], IPE_NO_BLOCK,
                                         track, sector);
                return IPE_NO_BLOCK;
            }
        } else {
            vdrive_bam_free_sector(floppy->ImageFormat, floppy->bam,
                                   track, sector);
        }
        break;
      case 'P':
        l = get_block_parameters(buffer, &channel, &position, &track, &sector);
            if (l > 0) /* just 2 args used */
        return l;
        if (floppy->buffers[channel].mode != BUFFER_MEMORY_BUFFER)
            return IPE_NO_CHANNEL;
        floppy->buffers[channel].bufptr = position;
        break;
      default:
        return IPE_INVAL;
    }
    return IPE_OK;
}


static int vdrive_command_memory(DRIVE *floppy, BYTE *buffer, int length)
{
#if 0
    int addr = 0;

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

int floppy_parse_name(const char *name, int length, char *ptr,
                      int *reallength, int *readmode, int *filetype,
                      int *rl)
{
    const char *p;
    char *c;
    int t;

    if (!name || !*name)
        return FLOPPY_ERROR;

    p = (char *)memchr(name, ':', length);
    if (p)
        p++;
    else {	/* no colon found */
        if (*name != '$')
            p = name;
        else
            p = name + strlen(name);	/* set to null byte */
    }
#ifdef DEBUG_DRIVE
    fprintf(logfile, "Name (%d): '%s'\n", length, p);
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
	fprintf(logfile, "parsing... [%d] %02x  t=%d\n", *reallength, *(ptr-1), t);
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
	    fprintf(logfile, "done. [%d] %02x  t=%d\n", *reallength, *p, t);
	    fprintf(logfile, "No type.\n");
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
	    fprintf(logfile, "No way. p='%s'\n", p);
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
    fprintf (logfile, "Type = %s  %s\n",
	    slot_type[*filetype],
	    (*readmode == FAM_READ ?  "read" : "write"));
#endif


    return FLOPPY_COMMAND_OK;
}

/* ------------------------------------------------------------------------- */


static int vdrive_command_copy(DRIVE *floppy, char *dest, int length)
{
    char *name, *files, *p, c;

    /* Split command line */
    if (!dest || !(files = memchr(dest, '=', length)) )
        return (IPE_SYNTAX);

    *files++ = 0;

    if (strchr (dest, ':'))
        dest = strchr (dest, ':') +1;

#ifdef DEBUG_DRIVE
    fprintf(logfile, "COPY: dest= '%s'\n      orig= '%s'\n", dest, files);
#endif

    if (vdrive_open(floppy, dest, strlen(dest), 1))
        return (IPE_FILE_EXISTS);

    p = name = files;

    while (*name) { /* Loop for given files.  */
        for (; *p && *p != ','; p++);
        *p++ = 0;

        if (strchr (name, ':'))
            name = strchr (name, ':') +1;

#ifdef DEBUG_DRIVE
        fprintf(logfile, "searching for file '%s'\n", name);
#endif
        if (vdrive_open(floppy, name, strlen(name), 0)) {
            vdrive_close(floppy, 1);
            return (IPE_NOT_FOUND);
        }

        while (!vdrive_read(floppy, (BYTE *)&c, 0)) {
            if (vdrive_write(floppy, c, 1)) {
                vdrive_close(floppy, 0); /* No space on disk.  */
                vdrive_close(floppy, 1);
                return (IPE_DISK_FULL);
            }
        }

        vdrive_close(floppy, 0);
        name = p; /* Next file.  */
    }
    vdrive_close(floppy, 1);
    return(IPE_OK);
}


/*
 * Rename disk entry
 */

static int vdrive_command_rename(DRIVE *floppy, char *dest, int length)
{
    char *src;
    char dest_name[256], src_name[256];
    int dest_reallength, dest_readmode, dest_filetype, dest_rl;
    int	src_reallength, src_readmode, src_filetype, src_rl;
    BYTE *slot;

    if (!dest || !(src = memchr(dest, '=', length)) )
        return (IPE_SYNTAX);

    *src++ = 0;

    if (strchr (dest, ':'))
        dest = strchr (dest, ':') +1;

#ifdef DEBUG_DRIVE
    fprintf(logfile, "RENAME: dest= '%s'\n      orig= '%s'\n", dest, src);
#endif

    if (!floppy_parse_name(dest, strlen(dest), dest_name, &dest_reallength,
        &dest_readmode, &dest_filetype, &dest_rl) == FLOPPY_ERROR)
        return IPE_SYNTAX;
    if (!floppy_parse_name(src, strlen(src), src_name, &src_reallength,
        &src_readmode, &src_filetype, &src_rl) == FLOPPY_ERROR)
        return IPE_SYNTAX;

    if (floppy->ReadOnly)
        return IPE_WRITE_PROTECT_ON;

    /* Check if the destination name is already in use.  */

    vdrive_dir_find_first_slot(floppy, dest_name, dest_reallength,
                               dest_filetype);
    slot = vdrive_dir_find_next_slot(floppy);

    if (slot)
        return (IPE_FILE_EXISTS);

    /* Find the file to rename. */

    vdrive_dir_find_first_slot(floppy, src_name, src_reallength, src_filetype);
    slot = vdrive_dir_find_next_slot(floppy);

    if (!slot)
        return (IPE_NOT_FOUND);

    /* Now we can replace the old file name...  */
    /* We write directly to the Dir_buffer.  */

    slot = &floppy->Dir_buffer[floppy->SlotNumber * 32];
    memset(slot + SLOT_NAME_OFFSET, 0xa0, 16);
    memcpy(slot + SLOT_NAME_OFFSET, dest_name, dest_reallength);
    if (dest_filetype)
        slot[SLOT_TYPE_OFFSET] = dest_filetype; /* FIXME: is this right? */

    /* Update the directory.  */
    floppy_write_block(floppy->ActiveFd, floppy->ImageFormat,
                       floppy->Dir_buffer, floppy->Curr_track,
                       floppy->Curr_sector, floppy->D64_Header,
                       floppy->GCR_Header, floppy->unit);
    return(IPE_OK);
}

/* ------------------------------------------------------------------------- */

/*
 * Close all channels. This happens on 'I' -command and on command-
 * channel close.
 */

void floppy_close_all_channels(DRIVE *floppy)
{
    int     i;
    bufferinfo_t *p;


    for (i = 0; i <= 15; i++) {
	p = &(floppy->buffers[i]);

	if (p->mode != BUFFER_NOT_IN_USE && p->mode != BUFFER_COMMAND_CHANNEL)
	    vdrive_close(floppy, i);
    }
}


static int vdrive_command_initialize(DRIVE *floppy)
{
    floppy_close_all_channels(floppy);

    /* Update BAM in memory.  */
    if (floppy->ActiveFd != NULL)
        vdrive_bam_read_bam(floppy);
    if (floppy->ErrFlg)
        set_error_data(floppy, 3); /* Clear or read error data.  */

    return IPE_OK;
}

int vdrive_command_validate(DRIVE *floppy)
{
    int t, s, status;
    /* FIXME: size of BAM define */
    BYTE *b, oldbam[5 * 256];

    status = vdrive_command_initialize(floppy);
    if (status != IPE_OK)
        return status;
    if (floppy->ReadOnly)
        return IPE_WRITE_PROTECT_ON;

    /* FIXME: size of BAM define */
    memcpy(oldbam, floppy->bam, 5 * 256);

    vdrive_bam_clear_all(floppy->ImageFormat, floppy->bam);
    for (t = 1; t <= floppy->NumTracks; t++) {
        int max_sector;
        max_sector = vdrive_get_max_sectors(floppy->ImageFormat, t);
        for (s = 0; s < max_sector; s++)
            vdrive_bam_free_sector(floppy->ImageFormat, floppy->bam, t, s);
    }

    /* First map out the BAM and directory itself.  */
    status = vdrive_bam_allocate_chain(floppy, floppy->Bam_Track,
                                       floppy->Bam_Sector);
    if (status != IPE_OK) {
	/* FIXME: size of BAM define */
        memcpy(floppy->bam, oldbam, 5 * 256);
        return status;
    }

    if (floppy->ImageFormat == 1581) {
        vdrive_bam_allocate_sector(floppy->ImageFormat, floppy->bam,
                                   floppy->Bam_Track, floppy->Bam_Sector + 1);
        vdrive_bam_allocate_sector(floppy->ImageFormat, floppy->bam,
                                   floppy->Bam_Track, floppy->Bam_Sector + 2);
    }

    vdrive_dir_find_first_slot(floppy, "*", 1, 0);

    while ((b = vdrive_dir_find_next_slot(floppy))) {
        char *filetype = (char *)
        &floppy->Dir_buffer[floppy->SlotNumber * 32 + SLOT_TYPE_OFFSET];

        if (*filetype & FT_CLOSED) {
            status = vdrive_bam_allocate_chain(floppy, b[SLOT_FIRST_TRACK],
                                    b[SLOT_FIRST_SECTOR]);
            if (status != IPE_OK) {
                memcpy(floppy->bam, oldbam, 256);
                return status;
            }
            /* The real drive always validates side sectors even if the file
               type is not REL.  */
            status = vdrive_bam_allocate_chain(floppy, b[SLOT_SIDE_TRACK],
                                               b[SLOT_SIDE_SECTOR]);
            if (status != IPE_OK) {
                memcpy(floppy->bam, oldbam, 256);
                return status;
            }
        } else {
            *filetype = FT_DEL;
            floppy_write_block(floppy->ActiveFd, floppy->ImageFormat,
                               floppy->Dir_buffer, floppy->Curr_track,
                               floppy->Curr_sector, floppy->D64_Header,
                               floppy->GCR_Header, floppy->unit);
        }
    }

    /* Write back BAM only if validate was successful.  */
    vdrive_bam_write_bam(floppy);
    return status;
}

static int vdrive_command_format(DRIVE *floppy, const char *name, BYTE *id,
                                 BYTE *minus)
{
    BYTE tmp[256];
    int status;
    BYTE null = 0;

    if (!name)
        return IPE_SYNTAX;

    if (floppy->ReadOnly)
        return IPE_WRITE_PROTECT_ON;

    if (floppy->ActiveFd < 0)
        return IPE_NOT_READY;

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
                       floppy->D64_Header, floppy->GCR_Header, floppy->unit);

    vdrive_bam_create_empty_bam(floppy, name, id);

    vdrive_bam_write_bam(floppy);

    /* Validate is called to clear the BAM.  */
    status = vdrive_command_validate(floppy);
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
	floppy->ErrData = (char *)xmalloc((size_t)MAX_BLOCKS_ANY);
	memset(floppy->ErrData, 0x01, MAX_BLOCKS_ANY);
    }
    else if (flags & 1)		/* clear error data */
	memset(floppy->ErrData, 0x01, MAX_BLOCKS_ANY);


    size   = num_blocks (floppy->ImageFormat, floppy->NumTracks);
    offset = ((off_t)size << 8) + (off_t)(floppy->D64_Header?0:HEADER_LENGTH);
    fseek(floppy->ActiveFd, offset, SEEK_SET);

    if (flags & 2) {		/* read from file */
        if (fread((char *)floppy->ErrData, size, 1, floppy->ActiveFd) < 1) {
            log_error(vdrive_log, "Floppy image read error.");
            return(-2);
        }
    }

    if (flags & 4) {		/* write to file */
        if (fwrite((char *)floppy->ErrData, size, 1, floppy->ActiveFd) < 1) {
            log_error(vdrive_log, "Floppy image write error.");
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

    if ((sectors = vdrive_check_track_sector(floppy->ImageFormat, 
        track, sector)) < 0)
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

int vdrive_check_track_sector(int format, int track, int sector)
{
    int sectors = 0, i;

    if (track < 1 || sector < 0)
        return (-1);

    switch (format) {
      case 1541:
        if (track > MAX_TRACKS_1541 || sector >= sector_map_1541[track])
            return (-1);
        for (i = 1; i < track; i++)
            sectors += sector_map_1541[i];
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
        for (i = 1; i < track; i++)
            sectors += sector_map_1541[i];
        sectors += sector;
        break;
      case 1581:
        if (track > MAX_TRACKS_1581 || sector >=  NUM_SECTORS_1581)
            return (-1);
        sectors = (track - 1) * NUM_SECTORS_1581 + sector;
        break;
      case 8050:
        if (track > MAX_TRACKS_8050 || sector >= pet_sector_map[track])
            return (-1);
        for (i = 1; i < track; i++)
            sectors += pet_sector_map[i];
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
        for (i = 1; i < track; i++)
            sectors += pet_sector_map[i];
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

    if ((sectors = vdrive_check_track_sector(format, track, sector)) < 0)
	return -1;

#ifdef DEBUG_DRIVE
    fprintf(logfile, "DEBUG SECTOR %3d (%2d,%2d) OFF_T: %ld\n",
	   sectors, track, sector,
	   ((off_t)sectors << 8) /* + (off_t)HEADER_LENGTH*/ );
#endif

    return ( ((off_t)sectors << 8) +
		(off_t)(d64?0:HEADER_LENGTH) );

}


/*
 * Read one block
 */

int floppy_read_block(FILE *fd, int format, BYTE *buf, int track,
                      int sector, int d64, int g64, int unit)
{
    if (fd == NULL)
        return -1;

    if (g64 && (unit == 8 || unit == 9)) {
        if (drive_read_block(track, sector, buf, unit - 8) < 0) {
            log_error(vdrive_log, "Error reading from disk image.");
            return(-2);
        }
    } else {
        off_t offset;
        offset = offset_from_track_and_sector(format, track, sector, d64);

        if (offset < 0)
            return -1;

        fseek(fd, offset, SEEK_SET);

        if (fread((char *)buf, 256, 1, fd) < 1) {
            log_error(vdrive_log, "Error reading from disk image.\n");
            return(-2);
        }
        return 0;
    }
    return 0;
}


/*
 * Write one block
 */

int floppy_write_block(FILE *fd, int format, BYTE *buf, int track,
                       int sector, int d64, int g64, int unit)
{
    if (fd == NULL)
        return -1;

    if (g64 && (unit == 8 || unit == 9)) {
        if (drive_write_block(track, sector, buf, unit - 8) < 0) {
            log_error(vdrive_log, "Error writing disk image.\n");
            return(-2);
        }
    } else {
        off_t offset;

        offset = offset_from_track_and_sector(format, track, sector, d64);

        if (offset < 0)
            return -1;

        if (fseek(fd, offset, SEEK_SET) < 0
            || fwrite((char *)buf, 256, 1, fd) < 1) {
            log_error(vdrive_log, "Cannot write T:%d S:%d: %s",
                      track, sector, strerror(errno));
            return -2;
        }
    }
    return 0;
}

/* ------------------------------------------------------------------------- */

int vdrive_calculate_disk_half(int type)
{
    switch (type) {
      case 1541:
        return 17;
      case 1571:
        return 17;
      case 1581:
        return 40;
      case 8050:
      case 8250:
	return 39;
    }
    return -1;
}

int vdrive_get_max_sectors(int type, int track)
{
    switch (type) {
      case 1541:
        return sector_map_1541[track];
      case 1571:
        return sector_map_1571[track];
      case 1581:
        return 40;
      case 8050:
        return pet_sector_map[track];
      case 8250:
	if (track <= NUM_TRACKS_8250 / 2) {
            return pet_sector_map[track];
	} else {
            return pet_sector_map[track - (NUM_TRACKS_8250 / 2)];
	}
    }
    return -1;
}

/* ------------------------------------------------------------------------- */

/*
 * Functions to attach the disk image files.
 */

void detach_floppy_image(DRIVE *floppy)
{
    floppy_close_all_channels(floppy);

    if (floppy->ActiveFd != NULL) {
        log_message(vdrive_log, "Detaching disk image %s from unit %i.",
                    floppy->ActiveName, floppy->unit);
        if (floppy->detach_func != NULL)
            floppy->detach_func(floppy);
        zfclose(floppy->ActiveFd);
        floppy->ActiveFd = NULL;
        floppy->ActiveName[0] = 0; /* Name is used as flag */
    }
}

int attach_floppy_image(DRIVE *floppy, const char *name, int mode)
{
    int DType;
    FILE *fd;
    hdrinfo hdr;


    if (!(floppy->type & DT_DISK)) {
        log_error(vdrive_log,
                  "Incompatible emulator mode for drive %d: FS 1541 Drive.",
                  floppy->unit);
        return (-1);
    }

    if (!name) {
        log_error(vdrive_log, "No name, cannot attach floppyimage.");
        return (-1);
    }

    fd = zfopen(name, MODE_READ_WRITE /*, 0 */);

    /* If we cannot open the image read/write, try to open it read only. */
    if (fd == NULL) {
        fd = zfopen(name, MODE_READ /*, 0 */);
        floppy->ReadOnly = 1;
    } else {
        floppy->ReadOnly = 0;
    }

    if (fd == NULL) {
        log_error(vdrive_log, "Cannot open file `%s'.", name);
        return -1;
    } else{
        if (check_header(fd, &hdr)) {
            log_error(vdrive_log,
                      "File `%s' was not recognized as a disk image in unit %d.",
                      name, floppy->unit);
            zfclose(fd);
            return (-2);
        }
        if (hdr.v_major > HEADER_VERSION_MAJOR
            || (hdr.v_major == HEADER_VERSION_MAJOR
            && hdr.v_minor > HEADER_VERSION_MINOR)) {
            log_error(vdrive_log, "Disk image file `%s' (V %d.%02d) "
                      "version higher than emulator (V %d.%02d).",
                      name, hdr.v_major, hdr.v_minor,
                      HEADER_VERSION_MAJOR, HEADER_VERSION_MINOR);
            zfclose(fd);
            return (-2);
        }


        /*
         * Check that the disk format on the image is compatible.
         */

        DType = get_diskformat(hdr.devtype);
        if (DType < 0) {
            log_error(vdrive_log, "Disk drive type mismatch for unit %d.",
                      floppy->unit);
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

        floppy->NumTracks  = hdr.tracks;
        floppy->NumBlocks  = num_blocks (DType, hdr.tracks);
        floppy->ErrFlg     = hdr.errblk;
        floppy->D64_Header = hdr.d64 | hdr.d71 | hdr.d81 | hdr.d80 | hdr.d82;
        floppy->GCR_Header = hdr.gcr;

        if (floppy->attach_func != NULL) {
            if (floppy->attach_func(floppy) < 0) {
                log_error(vdrive_log, "Unit %d: Wrong disk type.",
                          floppy->unit);
                zfclose(floppy->ActiveFd);
                floppy->ActiveFd = NULL;
                floppy->ActiveName[0] = 0;
                return -1;
            }
        }

        /* Initialise format constants */
        set_disk_geometry(floppy, DType);

        /* Initialize */
        if (hdr.errblk)
            set_error_data(floppy, 3);      /* clear or read error data */

        vdrive_bam_read_bam(floppy);

        if (hdr.d64)
            log_message(vdrive_log, "Unit %d: D64 disk image attached: %s%s.",
                        floppy->unit, name,
                        floppy->ReadOnly ? " (read only)" : "");
        if (hdr.d71)
            log_message(vdrive_log, "Unit %d: D71 disk image attached: %s%s.",
                        floppy->unit, name,
                        floppy->ReadOnly ? " (read only)" : "");
        if (hdr.d81)
            log_message(vdrive_log, "Unit %d: D81 disk image attached: %s%s.",
                        floppy->unit, name,
                        floppy->ReadOnly ? " (read only)" : "");
        if (hdr.d80)
            log_message(vdrive_log, "Unit %d: D80 disk image attached: %s%s.",
                        floppy->unit, name,
                        floppy->ReadOnly ? " (read only)" : "");
        if (hdr.d82)
            log_message(vdrive_log, "Unit %d: D82 disk image attached: %s%s.",
                        floppy->unit, name,
                        floppy->ReadOnly ? " (read only)" : "");
        if (hdr.gcr)
            log_message(vdrive_log, "Unit %d: GCR disk image attached: %s%s.",
                        floppy->unit, name,
                        floppy->ReadOnly ? " (read only)" : "");

        if (!(hdr.d64 | hdr.d71 | hdr.d81 | hdr.d80 | hdr.d82 | hdr.gcr))
            log_message(vdrive_log,
                        "Unit %d: VICE disk image version %d.%02d "
                        "attached (CBM%d format%s): `%s'.",
                        floppy->unit, hdr.v_major, hdr.v_minor, DType,
                        floppy->ReadOnly ? ", read only" : "", name);
    }

    return 0;
}

/* ------------------------------------------------------------------------- */

/*
 * Information Acquisition routines.
 */

int get_std64_header(FILE *fd, BYTE *header)
{
    int devtype = DT_1541;
    int tracks = NUM_TRACKS_1541;
    int blk = NUM_BLOCKS_1541 - 1;
    int len, errblk;
    char block[256];

    memset(header, 0, HEADER_LENGTH);

    /* Check values */

    if (vdrive_check_track_sector(get_diskformat(devtype), tracks, 1) < 0)
        exit (-1);

    header[HEADER_MAGIC_OFFSET + 0] = HEADER_MAGIC_1;
    header[HEADER_MAGIC_OFFSET + 1] = HEADER_MAGIC_2;
    header[HEADER_MAGIC_OFFSET + 2] = HEADER_MAGIC_3;
    header[HEADER_MAGIC_OFFSET + 3] = HEADER_MAGIC_4;

    header[HEADER_VERSION_OFFSET + 0] = HEADER_VERSION_MAJOR;
    header[HEADER_VERSION_OFFSET + 1] = HEADER_VERSION_MINOR;

    header[HEADER_FLAGS_OFFSET + 0] = devtype;
    header[HEADER_FLAGS_OFFSET + 1] = tracks;

    if (fseek(fd, 256*blk, SEEK_SET)<0)
        return FD_BADIMAGE;
    while ((len = fread(block, 1, 256, fd)) == 256) {
        if (++blk > 771) {
            /* FIXME */
            log_message(vdrive_log, "Nice try.");
            break;
        }
    }
    if (blk <  NUM_BLOCKS_1541) {
        log_message(vdrive_log, "Cannot read block %d.", blk);
        return (FD_NOTRD);
    }

    switch (blk) {
      case 683:
        tracks = NUM_TRACKS_1541;
        errblk = 0;
        break;

      case 685:
        if (len != 171) {               /* check if 683 error bytes */
            log_message(vdrive_log, "cannot read block %d.", blk);
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

int get_std71_header(FILE *fd, BYTE *header)
{
    int devtype = DT_1571;
    int tracks = NUM_TRACKS_1571;
    int blk = NUM_BLOCKS_1571-1;
    int len, errblk;
    char block[256];

    memset(header, 0, HEADER_LENGTH);

    /* Check values */

    if (vdrive_check_track_sector(get_diskformat (devtype), tracks, 1) < 0)
        exit (-1);

    header[HEADER_MAGIC_OFFSET + 0] = HEADER_MAGIC_1;
    header[HEADER_MAGIC_OFFSET + 1] = HEADER_MAGIC_2;
    header[HEADER_MAGIC_OFFSET + 2] = HEADER_MAGIC_3;
    header[HEADER_MAGIC_OFFSET + 3] = HEADER_MAGIC_4;

    header[HEADER_VERSION_OFFSET + 0] = HEADER_VERSION_MAJOR;
    header[HEADER_VERSION_OFFSET + 1] = HEADER_VERSION_MINOR;

    header[HEADER_FLAGS_OFFSET + 0] = devtype;
    header[HEADER_FLAGS_OFFSET + 1] = tracks;

    if (fseek(fd, 256*blk, SEEK_SET)<0)
        return FD_BADIMAGE;
    while ((len = fread(block, 1, 256, fd)) == 256) {
        if (++blk > 1372) {
            log_message(vdrive_log, "Nice try.");
            break;
        }
    }
    if (blk <  NUM_BLOCKS_1571) {
            log_message(vdrive_log, "Cannot read block %d.", blk);
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
    header[HEADER_FLAGS_OFFSET+2] = 1;
    header[HEADER_FLAGS_OFFSET+3] = errblk;

    return(0);
}

int get_std81_header(FILE *fd, BYTE *header)
{
    int devtype = DT_1581;
    int tracks = NUM_TRACKS_1581;
    int blk = NUM_BLOCKS_1581-1;
    int len, errblk;
    char block[256];

    memset(header, 0, HEADER_LENGTH);

    /* Check values */

    if (vdrive_check_track_sector(get_diskformat (devtype), tracks, 1) < 0)
        exit (-1);

    header[HEADER_MAGIC_OFFSET + 0] = HEADER_MAGIC_1;
    header[HEADER_MAGIC_OFFSET + 1] = HEADER_MAGIC_2;
    header[HEADER_MAGIC_OFFSET + 2] = HEADER_MAGIC_3;
    header[HEADER_MAGIC_OFFSET + 3] = HEADER_MAGIC_4;

    header[HEADER_VERSION_OFFSET + 0] = HEADER_VERSION_MAJOR;
    header[HEADER_VERSION_OFFSET + 1] = HEADER_VERSION_MINOR;

    header[HEADER_FLAGS_OFFSET + 0] = devtype;
    header[HEADER_FLAGS_OFFSET + 1] = tracks;

    if (fseek(fd, 256*blk, SEEK_SET)<0)
        return FD_BADIMAGE;
    while ((len = fread(block, 1, 256, fd)) == 256) {
        if (++blk > 3213) {
            log_message(vdrive_log, "Nice try.");
            break;
        }
    }
    if (blk <  NUM_BLOCKS_1581) {
            log_message(vdrive_log, "Cannot read block %d.", blk);
            return (FD_NOTRD);
    }
    switch (blk) {
      case 3200:
        tracks = NUM_TRACKS_1581;
        errblk = 0;
        break;
      default:
        return (FD_BADIMAGE);

    }
    header[HEADER_FLAGS_OFFSET+0] = DT_1581;
    header[HEADER_FLAGS_OFFSET+1] = tracks;
    header[HEADER_FLAGS_OFFSET+2] = 1;
    header[HEADER_FLAGS_OFFSET+3] = errblk;

    return(0);
}

int get_std80_header(FILE *fd, BYTE *header)
{
    int devtype = DT_8050;
    int tracks = NUM_TRACKS_8050;
    int blk = NUM_BLOCKS_8050-1;
    int len, errblk;
    char block[256];

    memset(header, 0, HEADER_LENGTH);

    /* Check values */

    if (vdrive_check_track_sector(get_diskformat (devtype), tracks, 1) < 0)
        exit (-1);

    header[HEADER_MAGIC_OFFSET + 0] = HEADER_MAGIC_1;
    header[HEADER_MAGIC_OFFSET + 1] = HEADER_MAGIC_2;
    header[HEADER_MAGIC_OFFSET + 2] = HEADER_MAGIC_3;
    header[HEADER_MAGIC_OFFSET + 3] = HEADER_MAGIC_4;

    header[HEADER_VERSION_OFFSET + 0] = HEADER_VERSION_MAJOR;
    header[HEADER_VERSION_OFFSET + 1] = HEADER_VERSION_MINOR;

    header[HEADER_FLAGS_OFFSET + 0] = devtype;
    header[HEADER_FLAGS_OFFSET + 1] = tracks;

    if (fseek(fd, 256*blk, SEEK_SET)<0)
        return FD_BADIMAGE;
    while ((len = fread(block, 1, 256, fd)) == 256) {
        if (++blk > NUM_BLOCKS_8050 + 6) {
            log_message(vdrive_log, "Nice try.");
            break;
        }
    }
    if (blk <  NUM_BLOCKS_8050) {
            log_message(vdrive_log, "Cannot read block %d.", blk);
            return (FD_NOTRD);
    }
    switch (blk) {
      case NUM_BLOCKS_8050:
        tracks = NUM_TRACKS_8050;
        errblk = 0;
        break;
      default:
        return (FD_BADIMAGE);

    }
    header[HEADER_FLAGS_OFFSET+0] = DT_8050;
    header[HEADER_FLAGS_OFFSET+1] = tracks;
    header[HEADER_FLAGS_OFFSET+2] = 1;	/* should this be 0 for one-sided? */
    header[HEADER_FLAGS_OFFSET+3] = errblk;

    return(0);
}

int get_std82_header(FILE *fd, BYTE *header)
{
    int devtype = DT_8250;
    int tracks = NUM_TRACKS_8250;
    int blk = NUM_BLOCKS_8250-1;
    int len, errblk;
    char block[256];

    memset(header, 0, HEADER_LENGTH);

    /* Check values */

    if (vdrive_check_track_sector(get_diskformat (devtype), tracks, 1) < 0)
        exit (-1);

    header[HEADER_MAGIC_OFFSET + 0] = HEADER_MAGIC_1;
    header[HEADER_MAGIC_OFFSET + 1] = HEADER_MAGIC_2;
    header[HEADER_MAGIC_OFFSET + 2] = HEADER_MAGIC_3;
    header[HEADER_MAGIC_OFFSET + 3] = HEADER_MAGIC_4;

    header[HEADER_VERSION_OFFSET + 0] = HEADER_VERSION_MAJOR;
    header[HEADER_VERSION_OFFSET + 1] = HEADER_VERSION_MINOR;

    header[HEADER_FLAGS_OFFSET + 0] = devtype;
    header[HEADER_FLAGS_OFFSET + 1] = tracks;

    if (fseek(fd, 256*blk, SEEK_SET)<0)
        return FD_BADIMAGE;
    while ((len = fread(block, 1, 256, fd)) == 256) {
        if (++blk > NUM_BLOCKS_8250 + 6) {
            log_message(vdrive_log, "Nice try.");
            break;
        }
    }
    if (blk <  NUM_BLOCKS_8250) {
            log_message(vdrive_log, "Cannot read block %d.", blk);
            return (FD_NOTRD);
    }
    switch (blk) {
      case NUM_BLOCKS_8250:
        tracks = NUM_TRACKS_8250;
        errblk = 0;
        break;
      default:
        return (FD_BADIMAGE);

    }
    header[HEADER_FLAGS_OFFSET+0] = DT_8250;
    header[HEADER_FLAGS_OFFSET+1] = tracks;
    header[HEADER_FLAGS_OFFSET+2] = 1;
    header[HEADER_FLAGS_OFFSET+3] = errblk;

    return(0);
}


int check_header(FILE *fd, hdrinfo *hdr)
{
    BYTE header[HEADER_LENGTH];

    fseek (fd, (off_t) 0, SEEK_SET);
    if (fread((BYTE *)header, sizeof (header), 1, fd) < 1) {
        log_error(vdrive_log, "Cannot read image header.");
        return FD_RDERR;
    }

    memset (hdr, 0, sizeof(hdrinfo));

    hdr->d64 = 0;
    hdr->d71 = 0;
    hdr->d81 = 0;
    hdr->d80 = 0;
    hdr->d82 = 0;
    hdr->gcr = 0;

    if (header[HEADER_MAGIC_OFFSET + 0] != HEADER_MAGIC_1 ||
        header[HEADER_MAGIC_OFFSET + 1] != HEADER_MAGIC_2 ||
        header[HEADER_MAGIC_OFFSET + 2] != HEADER_MAGIC_3 ||
        header[HEADER_MAGIC_OFFSET + 3] != HEADER_MAGIC_4) {

        size_t image_size;

        image_size = file_length(fd);

        if (IS_D64_LEN(image_size)) {
            if (get_std64_header(fd, header))
                return FD_BADIMAGE;
            hdr->d64 = 1;
        } else if (IS_D71_LEN(image_size)) {
            if (get_std71_header(fd, header))
                return FD_BADIMAGE;
            hdr->d71 = 1;
        } else if (IS_D81_LEN(image_size)) {
            if (get_std81_header(fd, header))
                return FD_BADIMAGE;
            hdr->d81 = 1;
        } else if (IS_D80_LEN(image_size)) {
            if (get_std80_header(fd, header))
                return FD_BADIMAGE;
            hdr->d80 = 1;
        } else if (IS_D82_LEN(image_size)) {
            if (get_std82_header(fd, header))
                return FD_BADIMAGE;
            hdr->d82 = 1;
        } else {
            if (import_GCR_image(header, hdr))
                return FD_OK;
            else
                return FD_BADIMAGE;
        } 
    }

    hdr->v_major = header[HEADER_VERSION_OFFSET + 0];
    hdr->v_minor = header[HEADER_VERSION_OFFSET + 1];

  /* Disk type flags: Device Type, Max Tracks, Side, and Error Flag. */

    hdr->devtype = header[HEADER_FLAGS_OFFSET];
    hdr->tracks  = header[HEADER_FLAGS_OFFSET + 1];
    hdr->sides   = (header[HEADER_FLAGS_OFFSET + 2] ? 2 : 1);
    hdr->errblk  = header[HEADER_FLAGS_OFFSET + 3];
    hdr->format  = (get_diskformat(hdr-> devtype)) == 1581 ? 'D' : 'A';

    if (hdr->tracks == 0)
        hdr->tracks = 35;


    if (vdrive_check_track_sector(get_diskformat(hdr-> devtype), hdr-> tracks,
        1) < 0)
        return FD_BADIMAGE;


    strncpy (hdr->description,
             (char *)header + HEADER_LABEL_OFFSET, HEADER_LABEL_LEN);

    return FD_OK;
}

int import_GCR_image(BYTE *header, hdrinfo *hdr)
{
    int trackfield;

    if (strncmp("GCR-1541",(char*)header,8))
	return 0;

    if (header[8] != 0) {
	log_message(vdrive_log, "Import GCR: Wrong GCR image version.");
	return 0;
    }

    if (header[9] < NUM_TRACKS_1541 * 2 || header[9] > MAX_TRACKS_1541 * 2) {
	log_message(vdrive_log, "Import GCR: Invalid number of tracks.");
	return 0;
    }

    trackfield = header[10] + header[11] * 256;
    if (trackfield != 7928) {
        log_message(vdrive_log, "Import GCR: Invalid track field number.");
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

int get_diskformat(int devtype)
{
    int DiskType = -1;

    /*
     * Get the group of compatible disk formats.
     */

    switch (devtype & DT_MASK) {
      case DT_2031:
      case DT_2040:
      case DT_4040:
      case DT_1540:
      case DT_1541:
      case DT_1542:
      case DT_1551:
      case DT_1570:
        DiskType = 1541;
        break;
      case DT_1571:
      case DT_1572:
        DiskType = 1571;
        break;
      case DT_1581:
        DiskType = 1581;
        break;
      case DT_8050:
        DiskType = 8050;
        break;
      case DT_8060:
      case DT_8061:
      case DT_SFD1001:
      case DT_8250:
        DiskType = 8250;
        break;
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
 * Initialise format constants
 */

void set_disk_geometry(DRIVE *floppy, int type)
{
    switch (type) {
      case 1541:
        floppy->Bam_Track  = BAM_TRACK_1541;
        floppy->Bam_Sector = BAM_SECTOR_1541;
        floppy->bam_name   = BAM_NAME_1541;
        floppy->bam_id     = BAM_ID_1541;
        floppy->Dir_Track  = DIR_TRACK_1541;
        floppy->Dir_Sector = DIR_SECTOR_1541;
        break;
      case 1571:
        floppy->Bam_Track  = BAM_TRACK_1571;
        floppy->Bam_Sector = BAM_SECTOR_1571;
        floppy->bam_name   = BAM_NAME_1571;
        floppy->bam_id     = BAM_ID_1571;
        floppy->Dir_Track  = DIR_TRACK_1571;
        floppy->Dir_Sector = DIR_SECTOR_1571;
        break;
      case 1581:
        floppy->Bam_Track  = BAM_TRACK_1581;
        floppy->Bam_Sector = BAM_SECTOR_1581;
        floppy->bam_name   = BAM_NAME_1581;
        floppy->bam_id     = BAM_ID_1581;
        floppy->Dir_Track  = DIR_TRACK_1581;
        floppy->Dir_Sector = DIR_SECTOR_1581;
        break;
      case 8050:
        floppy->Bam_Track  = BAM_TRACK_8050;
        floppy->Bam_Sector = BAM_SECTOR_8050;
        floppy->bam_name   = BAM_NAME_8050;
        floppy->bam_id     = BAM_ID_8050;
        floppy->Dir_Track  = DIR_TRACK_8050;
        floppy->Dir_Sector = DIR_SECTOR_8050;
        break;
      case 8250:
        floppy->Bam_Track  = BAM_TRACK_8250;
        floppy->Bam_Sector = BAM_SECTOR_8250;
        floppy->bam_name   = BAM_NAME_8250;
        floppy->bam_id     = BAM_ID_8250;
        floppy->Dir_Track  = DIR_TRACK_8250;
        floppy->Dir_Sector = DIR_SECTOR_8250;
        break;
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
    if (vdrive_open(floppy, line, 1, 0) != SERIAL_OK)
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
    vdrive_close(floppy, 0);

    /* Add trailing zero. */
    *(outbuf + outbuf_size) = '\0';

    return outbuf;
}
