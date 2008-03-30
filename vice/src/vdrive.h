/*
 * vdrive.h - Virtual disk-drive implementation.
 *
 * Written by
 *  Teemu Rantanen      (tvr@cs.hut.fi)
 *  Jarkko Sonninen     (sonninen@lut.fi)
 *  Jouko Valta         (jopi@stekt.oulu.fi)
 *  Olaf Seibert        (rhialto@mbfys.kun.nl)
 *  Andre Fachat        (a.fachat@physik.tu-chemnitz.de)
 *  Ettore Perazzoli    (ettore@comm2000.it)
 *  Martin Pottendorfer (Martin.Pottendorfer@aut.alcatel.at)
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

#ifndef _VDRIVE_H
#define _VDRIVE_H

#ifdef STDC_HEADERS
#include <time.h>
#endif

#include "types.h"
#include "fs_cbm.h"

#define UPPER(ad)		(((ad)>>8)&0xff)
#define LOWER(ad)		((ad)&0xff)

#define SET_LO_HI(p, val)                       \
    do {                                        \
	*((p)++) = (val) & 0xff;                \
	*((p)++) = ((val)>>8) & 0xff;           \
    } while (0)

#define DRIVE_RAMSIZE		0x400
#define IP_MAX_COMMAND_LEN	128	/* real 58 */

/* File Types */

#define FT_DEL		0
#define FT_SEQ		1
#define FT_PRG		2
#define FT_USR		3
#define FT_REL		4
#define FT_CBM		5	/* 1581 partition */
#define FT_DJJ		6	/* 1581 */
#define FT_FAB		7	/* 1581 - Fred's format */
#define FT_REPLACEMENT	0x20
#define FT_LOCKED	0x40
#define FT_CLOSED	0x80

/* Access Control Methods */

#define FAM_READ	0
#define FAM_WRITE	1
#define FAM_APPEND	2
#define FAM_M		4
#define FAM_F		8

typedef struct bufferinfo_s {
    int mode;			/* Mode on this buffer */
    int readmode;		/* Is this channel for reading or writing */
    BYTE *buffer;		/* Use this to save data */
    BYTE *slot;			/* Save data for directory-slot */
    int bufptr;			/* Use this to save/read data to disk */
    int track;			/* which track is allocated for this sector */
    int sector;			/*   (for write files only) */
    int length;			/* Directory-read length */
} bufferinfo_t;


typedef struct _DRIVE DRIVE;
typedef int (*drive_attach_func_t)(DRIVE *);
typedef int (*drive_detach_func_t)(DRIVE *);

/* Run-time data struct for each drive. */
struct _DRIVE {
    int type;			/* Device */

    /* Current image file */

    int mode;			/* Read/Write */
    int ImageFormat;		/* 1541/71/81 */
    int ActiveFd;
    char ActiveName[256];	/* Image name */
    char ReadOnly;
    int unit;

    /* Function to call after the image is attached.  */
    drive_attach_func_t attach_func;

    /* Function to call before the image is detached.  */
    drive_detach_func_t detach_func;

    /* Disk Format Constants */

    int D64_Header;		/* flag if file has header! */
    int GCR_Header;		/* flag if file is GCR image.  */
    int Bam_Track;
    int Bam_Sector;
    int Dir_Track;
    int Dir_Sector;


    /* Drive information */

    int NumBlocks;		/* Total Count (683) */
    int NumTracks;
    int BSideTrack;		/* First track on the second side (1571: 36) */

    int ErrFlg;			/* Flag if Error Data is available */
    char *ErrData;

    BYTE *dosrom;
    BYTE *dosram;
    BYTE bam[256];
    bufferinfo_t buffers[16];


    /* File information */

    BYTE Dir_buffer[256];	/* Current DIR sector */
    int SlotNumber;

    char *find_name;		/* Search pattern */
    int find_length;
    int find_type;

    int Curr_track;
    int Curr_sector;
};

/* Actually, serial-code errors ... */

#define FLOPPY_COMMAND_OK	0
#define FLOPPY_ERROR		(2)


/* Return values used around. */

#define FD_OK		0
#define FD_EXIT		1	/* -1,0, 1 are fixed values */

#define FD_NOTREADY	-2
#define FD_CHANGED	-3	/* File has changed on disk */
#define FD_NOTRD	-4
#define FD_NOTWRT	-5
#define FD_WRTERR	-6
#define FD_RDERR	-7
#define FD_INCOMP	-8	/* DOS Format Mismatch */
#define FD_BADIMAGE	-9	/* ID mismatch (Disk or tape) */
#define FD_BADNAME	-10	/* Illegal filename */
#define FD_BADVAL	-11	/* Illegal value */
#define FD_BADDEV	-12
#define FD_BAD_TS	-13	/* Track or sector */


#define CHK_NUM		0
#define CHK_RDY		1
#define CHK_EMU		2	/* Is image block based */

#define D64_FILE_SIZE_35  174848        /* D64 image, 35 tracks */
#define D64_FILE_SIZE_35E 175531        /* D64 image, 35 tracks with errors */
#define D64_FILE_SIZE_40  196608        /* D64 image, 40 tracks */
#define D64_FILE_SIZE_40E 197376        /* D64 image, 40 tracks with errors */
#define D71_FILE_SIZE     349696        /* D71 image, 70 tracks */
#define D81_FILE_SIZE     819200        /* D81 image, 80 tracks */

#define IS_D64_LEN(x) ((x) == D64_FILE_SIZE_35 || (x) == D64_FILE_SIZE_35E || \
		       (x) == D64_FILE_SIZE_40 || (x) == D64_FILE_SIZE_40E)
#define IS_D71_LEN(x) ((x) == D71_FILE_SIZE)
#define IS_D81_LEN(x) ((x) == D81_FILE_SIZE)
/*
 * Input Processor Error Codes
 */

#define IPE_OK                          0
#define IPE_DELETED                     1
#define IPE_SEL_PARTN                   2       /* 1581 */
#define IPE_UNIMPL                      3

#define IPE_WRITE_PROTECT_ON            26
#define IPE_SYNTAX                      30
#define IPE_INVAL                       31
#define IPE_LONG_LINE                   32
#define IPE_BAD_NAME                    33
#define IPE_NO_NAME                     34

#define IPE_NOT_WRITE                   60
#define IPE_NOT_OPEN                    61
#define IPE_NOT_FOUND                   62
#define IPE_FILE_EXISTS                 63
#define IPE_BAD_TYPE                    64
#define IPE_NO_BLOCK                    65
#define IPE_ILLEGAL_TRACK_OR_SECTOR     66

#define IPE_NO_CHANNEL                  70
#define IPE_DISK_FULL                   72
#define IPE_DOS_VERSION                 73
#define IPE_NOT_READY                   74
#define IPE_BAD_PARTN                   77      /* 1581 */

#define IPE_NOT_EMPTY                   80      /* dir to remove not empty */
#define IPE_PERMISSION                  81      /* permission denied */


/*
 * Error messages
 */

typedef struct errortext_s {
    int     nr;
    char   *text;
}     errortext_t;

/* ------------------------------------------------------------------------- */

extern int initialize_1541(int dev, int type,
                           drive_attach_func_t attach_func,
                           drive_detach_func_t detach_func,
                           DRIVE *oldinfo);
extern int find_devno(int dev, const char *name);
extern int attach_floppy_image(DRIVE *floppy, const char *name, int mode);
extern void detach_floppy_image(DRIVE *floppy);

extern int open_1541(void *floppy, char *name, int length, int secondary);
extern int close_1541(void *floppy, int secondary);
extern int read_1541(void *floppy, BYTE *data, int secondary);
extern int write_1541(void *floppy, BYTE data, int secondary);
extern void flush_1541(void *floppy, int secondary);

extern int write_fs(void *floppy, BYTE data, int secondary);
extern int read_fs(void *floppy, BYTE *data, int secondary);
extern int open_fs(void *floppy, char *name, int length, int secondary);
extern int close_fs(void *floppy, int secondary);
extern void flush_fs(void *floppy, int secondary);

extern int ip_execute(DRIVE *floppy, BYTE *buf, int length);
extern int do_validate(DRIVE *floppy);
extern int check_track_sector(int format, int track, int sector);

extern int floppy_read_block(int fd, int format, BYTE *buf, int track,
			     int sector, int d64);
extern int floppy_write_block(int fd, int format, BYTE *buf, int track,
			      int sector, int d64);

extern int get_std64_header(int fd, BYTE *header);
extern int check_header(int fd, hdrinfo *hdr);
extern int get_diskformat(int devtype);
extern int num_blocks(int format, int tracks);
extern void no_a0_pads(BYTE *ptr, int l);
extern int free_sector (BYTE *bam, int track, int sector);
extern int allocate_sector (BYTE *bam, int track, int sector);

extern char *floppy_read_directory(DRIVE *floppy, const char *pattern);
extern int floppy_parse_name ( char *name, int length, char *realname,
		int *reallength, int *readmode, int *filetype, int *rl );

void set_disk_geometry(DRIVE *floppy, int type);

int compare_filename(char *name, char *pattern);
void fs_error(int code);

char *read_disk_image_contents(const char *fname);

#endif				/* _VDRIVE_H */
