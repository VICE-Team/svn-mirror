/*
 * $Id: fs_cbm.h,v 1.7 1997/05/22 21:17:38 ettore Exp $
 *
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice
 *
 * Definitions for simulated disk drive.
 *
 *
 * Written by
 *   Teemu Rantanen (tvr@cs.hut.fi)
 *   Dan Miner (dminer@nyx10.cs.du.edu)
 *
 * Patches by
 *   Olaf Seibert     (rhialto@mbfys.kun.nl)
 *
 *
 * $Log: fs_cbm.h,v $
 * Revision 1.7  1997/05/22 21:17:38  ettore
 * c1541 version 2.02.
 *
 * Revision 1.6  1996/07/29 21:08:08  ettore
 * MAX_TRACKS_1541 and MAX_BLOCK_1541 accept 40 track disks.
 *
 * Revision 1.5  1996/04/01  09:01:41  jopi
 * New device type declarations
 *
 * Revision 1.4  1995/11/07  16:51:00  jopi
 * *** empty log message ***
 *
 * Revision 1.3  1995/04/01  07:54:09  jopi
 * X64 0.3 PL 0
 * Structure for passing Disk Header information.
 * Prototypes.
 *
 * Revision 1.2  1994/12/12  16:59:44  jopi
 * *** empty log message ***
 *
 * Revision 1.1  1994/08/10  17:46:41  jopi
 * Initial revision
 *
 *
 */


#ifndef X64_FS_CBM_H
#define X64_FS_CBM_H


#include "serial.h"	/* for DT_1541 */
#include "vdrive.h"


#define BUFFER_NOT_IN_USE		0
#define BUFFER_DIRECTORY_READ		1
#define BUFFER_SEQUENTIAL		2
#define BUFFER_MEMORY_BUFFER		3
#define BUFFER_OTHER			4
#define BUFFER_COMMAND_CHANNEL		5

#define WRITE_BLOCK     	        512


/* Disk Directory Organization */

#define SLOT_TYPE_OFFSET		2
#define SLOT_FIRST_TRACK		3
#define SLOT_FIRST_SECTOR		4
#define SLOT_NAME_OFFSET		5
#define SLOT_SIDE_TRACK			21
#define SLOT_SIDE_SECTOR		22
#define SLOT_NR_BLOCKS			30

#define BAM_FIRST_TRACK			0
#define BAM_FIRST_SECTOR		1
#define BAM_FORMAT_TYPE			2
#define BAM_BIT_MAP			4
#define BAM_DISK_NAME			144
#define BAM_DISK_ID			162
#define BAM_VERSION			165
#define BAM_EXT_BIT_MAP			192

#define BAM_SET(n)    (bamp[1+(n)/8] |= (1 << ((n) % 8)))
#define BAM_CLR(n)    (bamp[1+(n)/8] &= ~(1 << ((n) % 8)))
#define BAM_ISSET(n)  (bamp[1+(n)/8] & (1 << ((n) % 8)))


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

/*
 * XXX Rhialto: These #defines should be replaced by something like
struct FormatData {
    int NumTracks, NumBlocks, MaxTracks, MaxBlocks;
    int NumTracksPerSide, NumBlocksPerSide;
    int *SectorsPerTrack;
    int HdrT, HdrS;
        int FormatOffset, NameOffset, ...
    int DirT, DirS;
    int BamT, BamS;
    ... whatever more is needed
};
 */

#define DEFAULT_DEVICE_TYPE	DT_1541

#define NUM_TRACKS_1541		35
#define NUM_BLOCKS_1541		683	/* 664 free */
#define EXT_TRACKS_1541 	40
#define EXT_BLOCKS_1541 	768
#define MAX_TRACKS_1541		42
#define MAX_BLOCKS_1541		802

#define NUM_TRACKS_1571		70
#define NUM_BLOCKS_1571		1366	/* 1328 free */
#define MAX_TRACKS_1571		70
#define MAX_BLOCKS_1571		1366

#define NUM_TRACKS_1581		80
#define NUM_SECTORS_1581	40	/* Logical sectors */
#define NUM_BLOCKS_1581		3200	/* 3120 free */
#define MAX_TRACKS_1581		80
#define MAX_BLOCKS_1581		3200

#define NUM_TRACKS_8050		77
#define NUM_BLOCKS_8050		2088	/* 2052 free */
#define MAX_TRACKS_8050		77
#define MAX_BLOCKS_8050		2088

#define NUM_TRACKS_8250		144
#define NUM_BLOCKS_8250		4166	/* 4133 free */
#define MAX_TRACKS_8250		144
#define MAX_BLOCKS_8250		4166

#define MAX_TRACKS_ANY		MAX_TRACKS_8250
#define MAX_BLOCKS_ANY		MAX_BLOCKS_8250

#define DSK_DIR_TRACK		18	/* these are not valid for 8{0,2}50 */
#define DSK_DIR_SECTOR		1
#define DSK_BAM_TRACK		18
#define DSK_BAM_SECTOR		0	/* 8{0,2}50 have {2,4} bam sectors */


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
    int  gcr;
    char description[HEADER_LABEL_LEN+1];
} hdrinfo;


extern int utility_flag;


#endif  /* X64_FS_CBM_H */
