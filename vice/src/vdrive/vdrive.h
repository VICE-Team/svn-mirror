/*
 * vdrive.h - Virtual disk-drive implementation.
 *
 * Written by
 *  Andreas Boose       <boose@linux.rz.fh-hannover.de>
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

#include "attach.h"
#include "diskimage.h"
#include "types.h"
#include "log.h"

/* High level disk formats.  They can be different than the disk image type.  */
#define VDRIVE_IMAGE_FORMAT_1541 0
#define VDRIVE_IMAGE_FORMAT_1571 1
#define VDRIVE_IMAGE_FORMAT_1581 2
#define VDRIVE_IMAGE_FORMAT_8050 3
#define VDRIVE_IMAGE_FORMAT_8250 4

#define BUFFER_NOT_IN_USE		0
#define BUFFER_DIRECTORY_READ		1
#define BUFFER_SEQUENTIAL		2
#define BUFFER_MEMORY_BUFFER		3
#define BUFFER_OTHER			4
#define BUFFER_COMMAND_CHANNEL		5

#define WRITE_BLOCK     	        512

/*
 * At the beginning of each image is header that makes sure
 * only c1541 images are used. There is room for additional
 * information that is not used yet (errors, drive types etc.)
 */

#define HEADER_MAGIC_OFFSET	0	/* Length 4 bytes */

#define HEADER_MAGIC_1		'C'
#define HEADER_MAGIC_2		(0x15)
#define HEADER_MAGIC_3		(0x41)
#define HEADER_MAGIC_4		(0x64)


#define HEADER_VERSION_OFFSET	4	/* Length 2 bytes */

#define HEADER_VERSION_MAJOR	1
#define HEADER_VERSION_MINOR	2

#define HEADER_FLAGS_OFFSET	6	/* Disk Image Flags */
#define HEADER_FLAGS_LEN	4	/* Disk Image Flags */
  /* These 4 bytes are disk type flags (set upon create or format)
   * They contain: Device Type, Max Tracks, Side, and Error Flag.
   */

#define HEADER_LABEL_OFFSET	32	/* Disk Description */
#define HEADER_LABEL_LEN	31

#define HEADER_LENGTH		64

/*
 * Disk Drive Specs
 * For customized disks, the values must fit beteen the NUM_ and MAX_
 * limits. Do not change the NUM_ values, as they define the standard
 * disk geometry.
 */

#define NUM_TRACKS_1541        35
#define NUM_BLOCKS_1541	       683	/* 664 free */
#define EXT_TRACKS_1541        40
#define EXT_BLOCKS_1541        768
#define MAX_TRACKS_1541	       42
#define MAX_BLOCKS_1541	       802
#define DIR_TRACK_1541         18
#define DIR_SECTOR_1541        1
#define BAM_TRACK_1541         18
#define BAM_SECTOR_1541        0
#define BAM_NAME_1541          144
#define BAM_ID_1541            162
#define BAM_EXT_BIT_MAP_1541   192

#define NUM_TRACKS_1571	       70
#define NUM_BLOCKS_1571	       1366	/* 1328 free */
#define MAX_TRACKS_1571	       70
#define MAX_BLOCKS_1571	       1366
#define DIR_TRACK_1571         18
#define DIR_SECTOR_1571        1
#define BAM_TRACK_1571         18
#define BAM_SECTOR_1571        0
#define BAM_NAME_1571          144
#define BAM_ID_1571            162
#define BAM_EXT_BIT_MAP_1571   221

#define NUM_TRACKS_1581	       80
#define NUM_SECTORS_1581       40	/* Logical sectors */
#define NUM_BLOCKS_1581	       3200	/* 3120 free */
#define MAX_TRACKS_1581	       80
#define MAX_BLOCKS_1581	       3200
#define DIR_TRACK_1581         40
#define DIR_SECTOR_1581        3
#define BAM_TRACK_1581         40
#define BAM_SECTOR_1581        0
#define BAM_NAME_1581          4
#define BAM_ID_1581            22

#define NUM_TRACKS_8050	       77
#define NUM_BLOCKS_8050	       2083	/* 2052 free */
#define MAX_TRACKS_8050	       77
#define MAX_BLOCKS_8050	       2083
#define	BAM_TRACK_8050         39
#define	BAM_SECTOR_8050        0
#define	BAM_NAME_8050          6	/* pos. of disk name in 1st BAM blk */
#define	BAM_ID_8050            24	/* pos. of disk id in 1st BAM blk */
#define	DIR_TRACK_8050         39
#define	DIR_SECTOR_8050        1

#define NUM_TRACKS_8250	       154
#define NUM_BLOCKS_8250	       4166	/* 4133 free */
#define MAX_TRACKS_8250	       154
#define MAX_BLOCKS_8250	       4166
#define	BAM_TRACK_8250         39
#define	BAM_SECTOR_8250        0
#define	BAM_NAME_8250          6	/* pos. of disk name in 1st BAM blk */
#define	BAM_ID_8250            24	/* pos. of disk id in 1st BAM blk */
#define	DIR_TRACK_8250         39
#define	DIR_SECTOR_8250        1

#define MAX_TRACKS_ANY         MAX_TRACKS_8250
#define MAX_BLOCKS_ANY         MAX_BLOCKS_8250

typedef struct {
    int  v_major;
    int  v_minor;

    int  wprot;		/* From stat() + mtime() */

    int  devtype;
    int  format;
    int  sides;
    int  tracks;
    int  errblk;
    int  d64;
    int  d71;
    int  d81;
    int  d80;	/* 8050 */
    int  d82;	/* 8250 */
    int  gcr;
    char description[HEADER_LABEL_LEN+1];
} hdrinfo;


#define SET_LO_HI(p, val)                       \
    do {                                        \
	*((p)++) = (val) & 0xff;                \
	*((p)++) = ((val)>>8) & 0xff;           \
    } while (0)

#define DRIVE_RAMSIZE		0x400
#define IP_MAX_COMMAND_LEN	128	/* real 58 */

#define DIR_MAXBUF  (40 * 256)

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


/* Run-time data struct for each drive. */
typedef struct {
    disk_image_t *image;

    /* Current image file */
    int mode;         /* Read/Write */
    int image_format; /* 1541/71/81 */
    char ReadOnly;
    int unit;

    int Bam_Track;
    int Bam_Sector;
    int bam_name;       /* Offset from start of BAM to disk name.  */
    int bam_id;         /* Offset from start of BAM to disk ID.  */
    int Dir_Track;
    int Dir_Sector;

    /* Drive information */
    int NumBlocks;		/* Total Count (683) */
    int NumTracks;
    int BSideTrack;		/* First track on the second side (1571: 36) */

    BYTE *dosrom;
    BYTE *dosram;
    /* FIXME: bam sizeof define */
    BYTE bam[5*256];    /* The 1581 uses 3 secs as BAM - but the 8250 uses 5. */
    bufferinfo_t buffers[16];


    /* File information */

    BYTE Dir_buffer[256];  /* Current DIR sector */
    int SlotNumber;

    const char *find_name; /* Search pattern */
    int find_length;
    int find_type;

    int Curr_track;
    int Curr_sector;
} vdrive_t;

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
    int nr;
    const char *text;
} errortext_t;

/* ------------------------------------------------------------------------- */

extern log_t vdrive_log;

extern int vdrive_setup_device(vdrive_t *vdrive, int unit);

extern int vdrive_attach_image(disk_image_t *image, int unit, vdrive_t *floppy);
extern void vdrive_detach_image(disk_image_t *image, int unit,
                                vdrive_t *floppy);

extern int vdrive_check_track_sector(int format, int track, int sector);
extern int get_diskformat(int devtype);
extern int vdrive_calc_num_blocks(int format, int tracks);
extern char *floppy_read_directory(vdrive_t *floppy, const char *pattern);
extern int floppy_parse_name(const char *name, int length, char *realname,
                             int *reallength, int *readmode,
                             int *filetype, int *rl );
extern void floppy_close_all_channels(vdrive_t *floppy);

extern void vdrive_set_disk_geometry(vdrive_t *floppy, int type);
extern int vdrive_calculate_disk_half(int type);
extern int vdrive_get_max_sectors(int type, int track);

/* Drive command related functions.  */
extern int  vdrive_command_execute(vdrive_t *floppy, BYTE *buf, int length);
extern void vdrive_command_set_error(bufferinfo_t *p, int code,
                                     int track, int sector);
extern int  vdrive_command_validate(vdrive_t *floppy);

#endif				/* _VDRIVE_H */

