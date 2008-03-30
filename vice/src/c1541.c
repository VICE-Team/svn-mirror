/*
 * c1541.c - Stand-alone disk image maintenance program.
 *
 * Written by
 *  Teemu Rantanen   (tvr@cs.hut.fi)
 *  Jouko Valta      (jopi@zombie.oulu.fi)
 *  Gerhard Wesp     (gwesp@cosy.sbg.ac.at)
 *  Daniel Sladic    (sladic@eecg.toronto.edu)
 *  Ricardo Ferreira (storm@esoterica.pt)
 *  Andreas Boose    (boose@unixserv.rz.fh-hannover.de)
 *
 * Patches by
 *  Olaf Seibert     (rhialto@mbfys.kun.nl)
 *  Ettore Perazzoli (ettore@comm2000.it)
 *
 * Zipcode implementation based on `zip2disk' by
 *  Paul David Doherty (h0142kdd@rz.hu-berlin.de)
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

#ifdef __hpux
#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE
#endif
#endif

#include "vice.h"	/* for convenience */

#ifdef STDC_HEADERS
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <memory.h>
#include <assert.h>
#endif

#include "c1541.h"
#include "mshell.h"
#include "serial.h"
#include "vdrive.h"
#include "gcr.h"
#include "file.h"
#include "charsets.h"
#include "tape.h"
#include "utils.h"
#include "zipcode.h"

#define MAXARG		16
#define MAXVAL		0xffff

#define MAXDRIVE	1

#define C1541_VERSION_MAJOR	2
#define C1541_VERSION_MINOR	5

/* Global */

DRIVE *DriveData[4] = { NULL, NULL, NULL, NULL };

static int  DriveNum = 0;

static char *args[MAXARG+1];
static int  values[MAXARG+1];
static int  types[MAXARG+1];
static int  nargs;

char    newname[256];


/* Local functions */

static int  check_drive ( int dev, int mode );
static int  open_image ( int dev, char *name, int create );
static int  create_image ( int fd, int devtyp, int tracks, int errb, char *label );
static void usage( void );

static int  disk_attach ( void );
static int  disk_cdd ( void );
static int  disk_format ( void );
static int  disk_gcrformat ( void );
static int  disk_delete ( void );
static int  disk_list ( void );
static int  disk_validate ( void );
static int  disk_copy ( void );
static int  disk_rename ( void );
static int  disk_read ( void );
static int  disk_write ( void );
static int  disk_import ( void );
static int  disk_import_zipfile ( void );
static int  disk_unlynx ( void );
static int  disk_copy_tape ( void );
static int  disk_sectordump ( void );
static int  disk_extract ( void );
static int  disk_raw_command ( void );
static int  disk_info ( void );
static int  disk_help ( void );
static int  disk_quit ( void );
static int  disk_system ( void );

extern char sector_map_1541[43]; /* Ugly: FIXME! */
extern int speed_map_1541[42];

struct ms_table disk_cmds[] = {
    {"format", 1, 2, disk_format,
    "format  [imagename] 'diskname,id'\t(create X64 disk & format)\n"},
    {"gcrformat", 1, 2, disk_gcrformat,
    "gcrformat  imagename ['diskname,id']\t(create GCR disk & format)\n"},
    {"delete", 1, MAXARG, disk_delete,
    "delete  files\n"},
    {"list", 0, MAXARG, disk_list,
    "list   [pattern]\n"},
    {"validate", 0, 1, disk_validate,
    "validate\n"},
    {"copy", 2, MAXARG, disk_copy,
#ifdef COPY_TO_LEFT
    "copy    newname oldname [oldname2 ...]\n"},
#else
    "copy    oldname [oldname2 ...] newname\n"},
#endif
    {"rename", 2, 2, disk_rename, /* reintroduced 1996/04/08 EP */
#ifdef COPY_TO_LEFT
    "rename  newname oldname\n"},
#else
    "rename  oldname newname\n"},
#endif
    {"attach", 0, 2, disk_attach,
     "attach [drive] imagename\t\t(attaches floppy image for use)\n"},
    {"unit", 1, 1, disk_cdd,
     "unit\t  drive\t\t\t\t(changes default device)\n"},

    {"create", 2, 3, disk_import,
    "create  imagename dumpname ['descr']\t(create disk & import a raw diskdump)\n"},
    {"zcreate", 2, 3, disk_import_zipfile,
    "zcreate  imagename zipname ['descr']\t(create disk & import zipfile images)\n"},
    {"tape", 1, MAXARG, disk_copy_tape,
    "tape\t  tapeimage [files]\t\t(copy files from tape image)\n"},
    {"unlynx", 1, 1, disk_unlynx,
     "unlynx lynxname\t\t\t(unarchive an LNX file to disk image)\n"},
    {"read", 1, 2, disk_read,
    "read\t  1541name [FSname]\t\t(copy file from disk image)\n"},
    {"write", 1, 2, disk_write,
    "write\t  FSname [1541name]\t\t(write CBM or P00 file to disk image)\n"},

#if 0
    {"memory", 0, 2, disk_memdump,
    "memory [addr] [end|+len]\t\t(show memory in hex format)\n"},
    {"hunt", 3, MAXARG, disk_hunt,
    "hunt     start end|+len  bytes|'string'\n"},
#endif

    {"block", 0, 4, disk_sectordump,
    "block [drive:] [track] [sector] [disp] (show disk blocks in hex format)\n"},

    {"extract", 0, 0, disk_extract,
    "extract\t\t\t\t(extract all files to filesystem)\n"},
    {"info", 0, 1, disk_info,
    "info\t ['description']\t\t(show disk image version and info)\n"},

    {"@", 0, 1, disk_raw_command,
    "@\t[disk command]\t\t\t(command for channel 15)\n"},
    {"system", 1, 1, disk_system,
     "system command\t\t\t(execute a system command)\n"},
    {"help", 0, 1, disk_help,
    "help\t [topic]\t\t\t(Describe command)\n"},
    {"?", 0, 1, disk_help,
    "?\t [topic]\t\t\t(Same as help)\n"},
    {"quit", 0, 0, disk_quit,
    "quit\t\t\t\t\t(exits c1541)\n"},
    {"x", 0, 0, disk_quit,
    "x\t\t\t\t\t(exits c1541)\n"},
    {NULL, 0, 0, 0, NULL}
};



/*
 * Create Floppy Image
 */

static int  create_image (int fd, int devtype, int tracks, int errb, char *label)
{
    BYTE    header[HEADER_LENGTH];
    BYTE    block[256];
    int     blks;
    int     i;

    memset(header, 0, sizeof (header));
    memset(block, 0, sizeof (block));


    /* Check values */

    if (check_track_sector(get_diskformat (devtype), tracks, 1) < 0)
	exit (-1);

    blks = num_blocks (get_diskformat (devtype), tracks);


    header[HEADER_MAGIC_OFFSET + 0] = HEADER_MAGIC_1;
    header[HEADER_MAGIC_OFFSET + 1] = HEADER_MAGIC_2;
    header[HEADER_MAGIC_OFFSET + 2] = HEADER_MAGIC_3;
    header[HEADER_MAGIC_OFFSET + 3] = HEADER_MAGIC_4;

    header[HEADER_VERSION_OFFSET + 0] = HEADER_VERSION_MAJOR;
    header[HEADER_VERSION_OFFSET + 1] = HEADER_VERSION_MINOR;

    header[HEADER_FLAGS_OFFSET + 0] = devtype;
    header[HEADER_FLAGS_OFFSET + 1] = tracks;

    if (label)
	strncpy((char *)header + HEADER_LABEL_OFFSET, label, HEADER_LABEL_LEN);

    header[HEADER_LABEL_OFFSET + HEADER_LABEL_LEN] = 0;	  /* terminator */

    printf("writing header\n");
    if (write(fd, (char *)header, sizeof (header)) != sizeof (header)) {
	printf("cannot write header\n");
	exit(1);
    }

    printf("creating blocks\n");
    for (i = 0; i < blks; i++) {
	if (write(fd, (char *)block, sizeof (block)) != sizeof (block)) {
	    printf("cannot write block %d\n", i);
	    exit(1);
	}
    }


    if (errb) {
	printf("creating error data\n");
#if 0
	if (set_error_data(floppy, 5) < 0) {	/* clear and write */
	    printf("cannot write error data block.\n");
	    exit(1);
	}
#endif
    }

    return (0);
}


static int  set_label(int fd, char *label)
{
    int     siz = HEADER_LABEL_LEN +1;
    char    buf[HEADER_LABEL_LEN +2];


    memset(buf, 0, siz);

    if (label)
	strncpy(buf, label, HEADER_LABEL_LEN);

    lseek(fd, (off_t)HEADER_LABEL_OFFSET, SEEK_SET);

    if (write(fd, (char *)buf, siz) != siz) {
	return (FD_WRTERR);
    }
    return (FD_OK);
}


  /*
   * These 4 bytes are disk type flags (set upon create or format)
   * They contain: Device Type, Max Tracks, Side, and Error Flag.
   */

static int  set_disk_size(int fd, int tracks, int sides, int errblk)
{
    int     siz = HEADER_FLAGS_LEN;
    char    buf[HEADER_FLAGS_LEN +1];

    buf[0] = DEFAULT_DEVICE_TYPE;
    buf[1] = tracks;
    buf[2] = sides;
    buf[3] = errblk;

    lseek(fd, (off_t)HEADER_FLAGS_OFFSET, SEEK_SET);

    if (write(fd, (char *)buf, siz) != siz) {
	return (FD_WRTERR);
    }
    return (FD_OK);
}


/*
 * Open image or create a new one.
 * If the file exists, it must have valid header.
 */

static int  open_image(int dev, char *name, int create)
{
    DRIVE  *floppy;
    int     fd;


    if (dev < 0 || dev > MAXDRIVE)
	return (-1);  /* FD_BADDEV */


    floppy = DriveData[dev & 3];

    if (create) {
	if ((fd = open(name, O_RDWR | O_CREAT, 0666)) < 0) {
	    printf("could not create image %s\n", name);
	    return (-1);
	}


	/*
	 * Get default geometry
	 * make a new image file and format it
	 */

	create_image(fd, DEFAULT_DEVICE_TYPE, NUM_TRACKS_1541, 0, NULL);
	close(fd);
    }


    attach_floppy_image(floppy, name, 0);

    if (floppy->ActiveFd < 0)
	return (-1);

    return (0);
}


/*
 * This routine is used in order to get around standard serial bus
 * connection.
 * It checks privileges for the requested operation on the specified
 * disk unit. Return value < 0 means requested action is not allowed,
 * i.e. there is no disk in the drive, or that drive is not available.
 */


static int  check_drive(int dev, int flags)
{
    DRIVE *floppy;

    dev &= 7;
    if (dev < 0 || dev > 3)
	return (FD_BADDEV);

    floppy = DriveData[dev & 3];

    if (!floppy || (flags != CHK_NUM && floppy->ActiveFd < 0)) {
	return (FD_NOTREADY);
    }

    return (FD_OK);
}


/* ------------------------------------------------------------------------- */

static int  disk_attach(void)
{
    char    *name = NULL;
    int      dev  = DriveNum;

    switch (nargs) {
      case 3:
	dev  = values[1];
	name = args[2];
	break;
      case 2:
	name = args[1];
    }

    if (check_drive(dev, CHK_NUM) < 0)
	return (FD_BADDEV);

    attach_floppy_image(DriveData[dev & 3], name, 0);
    return (0);
}


static int  disk_cdd(void)
{
    int      dev  = values[1];
    if (check_drive(dev, CHK_NUM) < 0)
	return (FD_BADDEV);

    DriveNum = (dev & 3);
    return (0);
}

/*
 * Creates a raw GCR disk image.
 */

static int  disk_gcrformat (void)
{
    int fd, track, sector;
    BYTE gcr_header[12], id[2];
    char name[16], *idptr;
    DWORD gcr_track_p[MAX_TRACKS_1541 * 2];
    DWORD gcr_speed_p[MAX_TRACKS_1541 * 2];
    BYTE gcr_track[7930], rawdata[260];
    BYTE *gcrptr;

    if (nargs < 2)
	return 0;

    memset(name, 0xa0, 16);
    id[0] = id[1] = 0xa0;

    if (nargs > 2) {
	strcpy(newname, args[2]);
	petconvstring(newname, 1);
	idptr = memchr(newname, ',', strlen(newname));
	if (idptr == NULL) {
	    int i;
	    for (i = 0; i < 16 && newname[i] != '\0'; i++)
		name[i] = newname[i];
	} else {
	    int i;
	    *idptr = '\0';
	    for (i = 0; i < 16 && newname[i] != '\0'; i++)
		name[i] = newname[i];
	    strncpy(id, ++idptr, 2);
	    if (idptr[1] != '\0') {
		id[0] = idptr[1];
		if (idptr[2] != '\0')
		    id[1] = idptr[2];
	    }
	}
    }

    if ((fd = open(args[1], O_RDWR | O_CREAT, 0666)) < 0) {
	printf("Could not create image `%s'.\n", args[1]);
	return (FD_BADIMAGE);
    }

    strcpy(gcr_header, "GCR-1541");

    gcr_header[8] = 0;
    gcr_header[9] = MAX_TRACKS_1541 * 2;
    gcr_header[10] = 7928 % 256;
    gcr_header[11] = 7928 / 256;

    if (write(fd, (char *)gcr_header, sizeof(gcr_header))
        != sizeof(gcr_header)) {
	printf("Cannot write header.\n");
	close(fd);
	return 0;
    }

    for (track = 0; track < MAX_TRACKS_1541; track++) {
	gcr_track_p[track * 2] = 12 + MAX_TRACKS_1541 * 16 + track * 7930;
	gcr_track_p[track * 2 + 1] = 0;
	gcr_speed_p[track * 2] = speed_map_1541[track];
	gcr_speed_p[track * 2 + 1] = 0;
    }

    if (write_dword(fd, gcr_track_p, sizeof(gcr_track_p)) < 0) {
	printf("Cannot write track header.\n");
	close(fd);
	return 0;
    }

    if (write_dword(fd, gcr_speed_p, sizeof(gcr_speed_p)) < 0) {
	printf("Cannot write speed header.\n");
	close(fd);
	return 0;
    }

    for (track = 0; track < MAX_TRACKS_1541; track++) {

	int raw_track_size[4] = { 6250, 6666, 7142, 7692 };

	memset(&gcr_track[2], 0xff, 7928);
	gcr_track[0] = raw_track_size[speed_map_1541[track]] % 256;
	gcr_track[1] = raw_track_size[speed_map_1541[track]] / 256;
	gcrptr = &gcr_track[2];

	for (sector = 0; sector < sector_map_1541[track+1]; sector++) {
	    BYTE chksum;
	    int i;
	    memset(rawdata, 0, 260);
	    if (track == 17 && sector == 0) {
		BYTE *rdat = &rawdata[1];
		int s, t;
		memset(rdat + BAM_DISK_NAME, 0xa0, 27);
		rdat[0] = DSK_DIR_TRACK;
		rdat[1] = DSK_DIR_SECTOR;
		rdat[2] = 65;
		rdat[BAM_VERSION] = 50;
		rdat[BAM_VERSION + 1] = 65;
		memcpy(rdat + BAM_DISK_NAME, (BYTE *)name, 16);
		rdat[BAM_DISK_ID] = id[0];
		rdat[BAM_DISK_ID + 1] = id[1];
		for (t = 1; t <= 35; t++)
		    for (s = 0; s < sector_map_1541[t]; s++)
			free_sector(rdat, t, s);
		allocate_sector(rdat, DSK_BAM_TRACK, 0);
		allocate_sector(rdat, DSK_BAM_TRACK, 1);
	    }
	    rawdata[0] = 7;
	    chksum = rawdata[1];
	    for (i = 1; i < 256; i++)
		chksum ^= rawdata[i + 1];
	    rawdata[257] = chksum;

	    convert_sector_to_GCR(rawdata, gcrptr, track+1, sector, id[0], id[1]);
	    gcrptr += 360;
	}

	if (write(fd, (char *)gcr_track, sizeof(gcr_track))
            != sizeof(gcr_track)) {
	    printf("Cannot write track data.\n");
	    close(fd);
	    return 0;
	}
    }

    close(fd);
    return 0;
}

/*
 * Simulate the 1541 DOS commands
 */

static int  disk_format (void)
{

    strcpy(newname, "N:");
    strcpy(newname + 2, ((nargs > 2) ? args[2] : args[1]) );

    if (!memchr(newname, ',', strlen(newname))) {
	printf("There must be ID on the name\n");
	return (0);
    }

    /* Open image or create a new one.
     * If the file exists, it must have valid header.
     * Note: Syntax check from command line is NOT passed
     * without the image name.
     */

    /*printf("DriveNum = %d\n", DriveNum);*/
    if (nargs > 2) {
	if (open_image(DriveNum, args[1], 1) < 0)
	    return (FD_BADIMAGE);
    }

    printf("formatting image\n");
    petconvstring(newname +2, 1);

    ip_execute(DriveData[DriveNum], (BYTE *)newname, strlen(newname));

    return(0);
}


static int  disk_delete (void)
{
    DRIVE *floppy;
    int   dev, err, i = 1;
    char *name, tmp[20];

    do {
	name = args[i];
	dev =  find_devno(DriveNum, name);

	if ((err = check_drive(dev, CHK_RDY)) < 0)
	    return (err);
	floppy = DriveData[dev & 3];

	petconvstring(name, 1);
	strcpy(tmp, "S:");
	strcat(tmp, name);

	ip_execute(floppy, (BYTE *)tmp, strlen(tmp));
    } while (++i < nargs);

    return(0);
}


static int  disk_validate (void)
{
    int   dev, err;

    dev =  (nargs > 1 ? values[1] : DriveNum);

    if ((err = check_drive(dev, CHK_RDY)) < 0)
	return (err);

    printf("validating\n");
    do_validate(DriveData[dev & 3]);

    return(0);
}


static int  disk_list (void)
{
    DRIVE *floppy;
    int   dev, err, i = 1;
    char *name, *listing;

    do {
	name = args[i];
	dev =  find_devno(DriveNum, name);

	if ((err = check_drive(dev, CHK_RDY)) < 0)
	    return (err);
	floppy = DriveData[dev & 3];

	if (name) {
	    if (*name && strchr (name, ':'))
		name = strchr (name, ':') +1;
	    petconvstring(name, 1);
	    listing = floppy_read_directory(floppy, name);
	}
	else
	    listing = floppy_read_directory(floppy, NULL);

	if (listing != NULL) {
	    printf("\n === Listing for drive %d ===\n\n%s", dev | 8,
		   listing);
	    free(listing);
	}
    } while (++i < nargs);

    return(0);
}


static int  disk_copy (void)
{
    DRIVE *floppy1, *floppy2;		/* may point to the same data */
    int    dev, err, i;
    char  *name, *dest;
    char   c;

#ifdef COPY_TO_LEFT
    /* copy    newname oldname [oldname2 ...] */
    dest = args[1];
#else
    /* copy    oldname [oldname2 ...] newname */
    dest = args[nargs -1];	/* EP */
#endif

    dev =  find_devno(DriveNum, dest);

    if ((err = check_drive(dev, CHK_RDY)) < 0)
	return (err);
    floppy2 = DriveData[dev & 3];

	if (strchr (dest, ':'))
	    dest = strchr (dest, ':') +1;

    strcpy((char *)newname, dest);
    petconvstring(newname, 1);

    if (open_1541(floppy2, newname, (int)strlen(newname), 1)) {
	printf("cannot open `%s' for writing on image.\n", dest);
	return (FD_WRTERR);
    }

#ifdef COPY_TO_LEFT
    for (i = 2; i < nargs; i++) {
#else
    for (i = 1; i < nargs -1; i++) {
#endif

	dev =  find_devno(DriveNum, args[i]);

	if ((err = check_drive(dev, CHK_RDY)) < 0)
	    return (err);
	floppy1 = DriveData[dev & 3];

	if (strchr (args[i], ':'))
	    name = strchr (args[i], ':') +1;
	else
	    name = args[i];

	strcpy((char *)newname, name);
	petconvstring(newname, 1);
	if (open_1541(floppy1, newname, strlen(newname), 0)) {
	    printf("u%d: cannot open `%s' on image.\n", dev, name);
	    close_1541(floppy2, 1);
	    return (FD_RDERR);
	}

	printf("copying %s\n", args[i]);

	while (!read_1541(floppy1, (BYTE *)&c, 0)) {
	    if (write_1541(floppy2, c, 1)) {
		printf("No space on image ?\n");
		break;
	    }
	}

	close_1541(floppy1, 0);
    } /* for */

    close_1541(floppy2, 1);
    return(0);
}


static int  disk_rename (void)
{
    DRIVE *floppy2;
    int    dev, err;
    char  *src, *dest;

#if defined (COPY_TO_LEFT)
    /* rename newname oldname */
    dest = args[1];
    src = args[nargs - 1];
#else
    /* rename oldname newname */
    src = args[1];
    dest = args[nargs - 1];
#endif

    dev =  find_devno(DriveNum, dest);

    if ((err = check_drive(dev, CHK_RDY)) < 0)
	return (err);
    floppy2 = DriveData[dev & 3];

    if (strchr (dest, ':'))
	dest = strchr (dest, ':') +1;

    printf("renaming `%s' to `%s'\n", src, dest);

    sprintf (newname, "R:%s=%s", dest, src);		/* Rename File */
    petconvstring(newname +2, 1);

    ip_execute(floppy2, (BYTE *)newname, strlen(newname));

    return(0);
}


/*
 * Additional Disk Maintenance commands
 */

static int  disk_read (void)
{
    DRIVE *floppy;
    FILE  *f;
    char  *name, *fsname;
    int    dev, err;
    char   c;


    if (strchr (args[1], ':'))
	name = strchr (args[1], ':') +1;
    else
	name = args[1];

    dev =  find_devno(DriveNum, args[1]);

    if ((err = check_drive(dev, CHK_RDY)) < 0)
	return (err);
	floppy = DriveData[dev & 3];

    strcpy((char *)newname, name);
    petconvstring(newname, 0);			/* To PETscii */

    if (open_1541(floppy, newname, strlen(newname), 0)) {
	printf("u%d: cannot open `%s' on image.\n", dev, name);
	return (FD_BADNAME);
    }


    /* Get real filename from the disk file.
     * slot must be defined by open_1541().
     */

    memcpy (newname, floppy->buffers[0].slot + SLOT_NAME_OFFSET, 16);
    if (nargs > 2)
	fsname = args[2];
    else {
	int l;

	fsname = newname;
	no_a0_pads((BYTE *)fsname, 16);
	l = strlen (fsname) - 1; /* EP */
	while (fsname[l] == ' ') {
	    fsname[l] = '\0';
	    l--;
	}

	petconvstring(fsname, 1);
    }


    if (*fsname == '-')
	f = stdout;
    else {
	if (!(f = fopen(fsname, WRITE))) {
	    close_1541(floppy, 0);
	    printf("cannot open %s for writing.\n", fsname);
	    return (FD_NOTWRT);
	}

	/* Write P00 header ? */

	if (nargs > 2 && is_pc64name(fsname) >= 0) {
	    printf("writing PC64 header.\n");
	    write_pc64header(f, newname, 0);
	}
    } /* stdout */


    fprintf(stderr, "reading file `%s' from image.\n", name);

    while (!read_1541(floppy, (BYTE *)&c, 0))
	fputc(c, f);

    fclose(f);
    close_1541(floppy, 0);

    return(0);
}


static int  disk_write (void)
{
    DRIVE *floppy;
    FILE  *f;
    int    dev, err, c;
    int    realtype, reclen = 0;
    char  *p, realname[20];


    if (!(f = fopen(args[1], READ))) {
	printf("cannot open %s for reading\n", args[1]);
	return (FD_NOTRD);
    }


    if (nargs > 2)
	p = args[2];
    else if ((p = strrchr(args[1], '/')) == NULL || !p++)
	p = args[1];

    if (!p || !*p)
	return (FD_BADNAME);

    strcpy((char *)newname, p);
    dev =  find_devno(DriveNum, newname);
    petconvstring(newname, 1);


    /* Check if P00 header and read the actual filename and type */

    if ((realtype = is_pc64name(args[1])) >= 0 &&
	read_pc64header(f, realname, &reclen) == FD_OK) {

	if (nargs < 3)
	    strcpy (newname, pc_get_cbmname (f, args[1]));
    }
    else
	rewind (f);		/* There is no P00 header */


    if ((err = check_drive(dev, CHK_RDY)) < 0)
	return (err);
	floppy = DriveData[dev & 3];


    if (open_1541(floppy, newname, strlen(newname), 1)) {
	printf("cannot open %s for writing on image.\n", newname);
	return (FD_WRTERR);
    }

    printf("writing file `%s' to image\n", newname);

    while (EOF != (c = fgetc(f))) {
	if (write_1541(floppy, c, 1)) {
	    printf("No space on image ?\n");
	    break;
	}
    }

    fclose(f);
    close_1541(floppy, 1);
    return(0);
}


typedef struct {
  char    ImageFormat;		/* 1541/71/81 */
  int     TracksSide;
  int     Sides;
  int     TotalBks;
} DiskFormats;

static DiskFormats  Legal_formats[] = {
  {DT_1541, 35, 0,  683 },
  {DT_1541, 40, 0,  768 },
/*{DT_1541, 42, 0,  785 }, */
  {DT_1571, 35, 1, 1366 },
  {DT_1581, 80, 0, 3200 },
  {DT_8050, 77, 0, 2088 },
  {DT_8250, 72, 1, 4166 },
  {  -1,     0, 0,    0 }
};

static int  disk_import_zipfile (void)
{
    DRIVE  *floppy = DriveData[DriveNum];
    DiskFormats  *format = Legal_formats;
    char    tmp[256];
    int     fsfd=-1, errblk = 0;
    int track, sector, count;
    char *fname;
    int    channel = 2;
    BYTE  str[20];
    static int  drive = 8;
    int		singlefilemode = 0;

    /*
     * Open image or create a new one.
     * If the file exists, it must have valid header.
     */
    if (open_image(DriveNum, args[1], 1) < 0) {
	return (FD_BADIMAGE);
    }

    fname = (char *) malloc (sizeof(char) * strlen(args[2]) + 3);
    /* ignore '[0-4]!' if found */
    if (args[2][0] >= '1' && args[2][0] <= '4' && args[2][1] == '!')
	strcpy(fname+2, args[2]+2);
    else
	strcpy(fname+2, args[2]);
    fname[0] = '0';
    fname[1] = '!';

    set_label(floppy->ActiveFd, "*** Truncated image."); /* Notify of errors */

    printf("copying blocks to image\n");
    lseek(floppy->ActiveFd, HEADER_LENGTH, SEEK_SET);

    /* Write out all the sectors */
    for(count=0;count<format->TotalBks;count++) {
       if (write(floppy->ActiveFd, tmp, 256) != 256) {
          printf("cannot write block %d of %s\n", count, args[2]);
          return (FD_WRTERR);
       }
    }

    if (open_1541(floppy, "#", 1, channel)) {
       printf("u%d: cannot open buffer #%d.\n", drive, channel);
       return (FD_RDERR);
    }

    for (track = 1; track <= 35; track++) {
	if (singlefilemode || track == 1) {
	    if (track == 1) {
		fsfd = open(fname + 2, O_RDONLY);
		if (fsfd >= 0) {
		    printf("reading zipfile on onefile -mode\n");
		    singlefilemode = 1;
		    lseek(fsfd, 4, SEEK_SET);
		}
	    } else if (track == 9 || track == 17 || track == 26) {
		lseek(fsfd, 2, SEEK_CUR);
	    }
	}
	if (!singlefilemode) {
	    switch (track) {
	    case 1:
	    case 9:
	    case 17:
	    case 26:
		fname[0]++;
		if (fsfd >= 0)
		    close(fsfd);
		if ((fsfd = open(fname, O_RDONLY)) < 0) {
		    printf("cannot open %s\n", fname);
		    perror (fname);
		    return (FD_NOTRD);
		}
		lseek(fsfd, (track == 1) ? 4 : 2, SEEK_SET);
		break;
	    }
	}

       for (count = 0; count < sector_map_1541[track]; count++) {
          if ( (zipcode_read_sector(fsfd, track, &sector, floppy->buffers[channel].buffer)) != 0) {
	      close(fsfd);
	      return (FD_BADIMAGE);
          }

          /* Write one block */

          sprintf((char *)str, "B-W:%d 0 %d %d", channel, track, sector);
          if (ip_execute(floppy, (BYTE *)str, strlen((char *)str)) != 0) {
	      track = DSK_DIR_TRACK;
	      sector= 0;
	      close(fsfd);
	      return (FD_RDERR);
          }
       }
    }
    close_1541(floppy, channel);

    /* Update Format and Label information on Disk Header */

    lseek(floppy->ActiveFd, (off_t)HEADER_LABEL_OFFSET + 0, SEEK_SET);

    if (write(floppy->ActiveFd, &(format->ImageFormat), 1) != 1) {
	return (FD_WRTERR);
    }

    set_disk_size(floppy->ActiveFd, format->TracksSide, format->Sides, errblk);

    set_label(floppy->ActiveFd, (args[3] ? args[3] : NULL)); /* Fix the note */

    close(fsfd);

    ip_execute(floppy, (BYTE *)"I", 1);

    return(0);
}


static int  disk_import (void)
{
    DRIVE  *floppy = DriveData[DriveNum];
    DiskFormats  *format;
    char    tmp[256];
    int     fsfd, len, blk = 0, errblk = 0;


    /*
     * Open image or create a new one.
     * If the file exists, it must have valid header.
     */
    if (open_image(DriveNum, args[1], 1) < 0)
	return (FD_BADIMAGE);

    if ((fsfd = open(args[2], O_RDONLY)) < 0) {
	printf("cannot open %s\n", args[2]);
	perror (args[2]);
	return (FD_NOTRD);
    }

    set_label(floppy->ActiveFd, "*** Truncated image."); /* Notify of errors */


    /* First copy all available blocks and then check existence of
     * Error Data Block */

    printf("copying blocks to image\n");
    lseek(floppy->ActiveFd, HEADER_LENGTH, SEEK_SET);

    while ((len = read(fsfd, tmp, 256)) == 256) {
	if (++blk > MAX_BLOCKS_ANY) {
	    printf("\nNice try.\n");
	    break;
	}
	if (write(floppy->ActiveFd, tmp, 256) != 256) {
	    printf("cannot write block %d of %s\n", blk, args[2]);
	    return (FD_WRTERR);
	}
    }


    /* Now recognize the format and verify block count on it. */

    if (blk <  NUM_BLOCKS_1541) {
	    printf("cannot read block %d of %s\n", blk, args[2]);
	    return (FD_NOTRD);
	}

    for (format = Legal_formats; format->ImageFormat >= 0; ++format) {
	if (blk == format->TotalBks) {
	    errblk = 0;
	    break;
	}
	if (blk == (format->TotalBks + (format->TotalBks >> 8)) ) {
	    errblk = 1;
	    break;
	}
    }

    if (format->ImageFormat < 0)
	return (FD_BADIMAGE);


    /* Check and write the last (short) sector of error bytes */

    if (len) {
	if (len != (format->TotalBks % 256)) {
	    printf("cannot read block %d of %s\n", blk, args[2]);
	    return (FD_NOTRD);
	}
	if (write(floppy->ActiveFd, tmp, len) != len) {
	    printf("cannot write block %d of %s\n", blk, args[2]);
	    return (FD_WRTERR);
	}
    }


    /* Update Format and Label information on Disk Header */

    lseek(floppy->ActiveFd, (off_t)HEADER_LABEL_OFFSET + 0, SEEK_SET);

    if (write(floppy->ActiveFd, &(format->ImageFormat), 1) != 1) {
	return (FD_WRTERR);
    }

    set_disk_size(floppy->ActiveFd, format->TracksSide, format->Sides, errblk);

    set_label(floppy->ActiveFd, (args[3] ? args[3] : NULL)); /* Fix the note */

    close(fsfd);

    ip_execute(floppy, (BYTE *)"I", 1);

    return(0);
}


/*
 * Copy files from *.t64 tape image. An already formatted
 * X64 disk image is required
 */

#define TAPE_HDR_SIZE 64
#define TAPE_DIR_SIZE 32

static int  disk_copy_tape (void)
{
    DRIVE *floppy = DriveData[DriveNum];
    FILE *f;
    BYTE *dirp, *tapebuf = NULL;
    char  asciiname[20], *p;
    int   maxentries = 0, ccount = 0;
    int   c, i, len, loc;


    if (!(f = fopen(args[1], READ))) {
	printf("cannot open %s for reading\n", args[1]);
	return (FD_NOTRD);
    }

    if ((maxentries = check_t64_header (f)) <= 0)
      return (maxentries);


    /* Read the tape directory entries into memory */

    tapebuf = (BYTE *)malloc(TAPE_DIR_SIZE * maxentries);
    assert(tapebuf);


    fread(tapebuf, TAPE_DIR_SIZE, maxentries, f);
    dirp = tapebuf;

    while (maxentries--) {
	if (*dirp == 1) {    /* found a file */

	    /* Get filename and remove trailing spaces */
	    memccpy(newname, &dirp[16], 0xa0, 16);
	    newname[16] = 0;
	    {
		char *cf = &newname[15];
		while (*cf == 0x20)  *cf-- = 0;
	    }
	    strcpy(asciiname, newname);
	    petconvstring(asciiname, 1);	/* to ascii */


	    /* Check filename match against [files] list */

	    /* if (nargs > 4) { */
	    if (nargs > 2) {
		for (i = 2; i < nargs &&
		     !compare_filename(asciiname, args[i]); ++i);

		if (i >= nargs) {
		    dirp += TAPE_DIR_SIZE;	/* No match, skip the file */
		    continue;
		}
	    }
	    else {
		if (!strcmp(asciiname, "file")) {
		    /* Invalid filename, try the imagename instead. */
		    strncpy(asciiname, args[1], 16);
		    asciiname[16] = 0;

		    if ((p = strstr(asciiname, ".t64")) != NULL)
		      *p = 0;
		    strcpy(newname, asciiname);
		    petconvstring(newname, 0);		/* to petcii */
		}
	    }


	    printf("  \"%-16s\"\t%02x%02x to %02x%02x\n",
		   asciiname, dirp[3], dirp[2], dirp[5], dirp[4]);

	    loc = dirp[8] | (dirp[9]<<8);
	    len = (dirp[4] | (dirp[5]<<8)) - (dirp[2] | (dirp[3]<<8));


	    if (open_1541(floppy, newname, strlen(newname), 1)) {
		printf("Cannot open `%s' for writing on image.\n", asciiname);
		free(tapebuf);
		return (FD_WRTERR);
	    }


	    printf("Writing file to image. %d bytes\n", len);

	    /* PRG file */
	    write_1541(floppy, dirp[2], 1);
	    write_1541(floppy, dirp[3], 1);

	    fseek(f, loc, 0);
	    while ((len--) > 0 && (EOF != (c = fgetc(f))))
		if (write_1541(floppy, c, 1)) {
		    printf("No space on image ?\n");
		    close_1541(floppy, 1);
		    free(tapebuf);
		    return (FD_WRTERR);
		}

	    close_1541(floppy, 1);

	    if (len > 0)
		fprintf(stderr,
			"T64 Error: Unexpected end of tape. File may be truncated.\n");
	    ++ccount;
	} /* if dirp */
	dirp += TAPE_DIR_SIZE;
    }

    printf("\n%d files copied.\n", ccount);
    fclose(f);
    free(tapebuf);
    return(0);
}


/*
 * Execute selection of BLOCK commands
 */

static int  disk_sectordump(void)
{
    static int  drive = 8, track = DSK_DIR_TRACK, sector = DSK_DIR_SECTOR;
    DRIVE *floppy;
    BYTE  *buf, str[20];
    int    err, cnt;
    int    channel = 2, disp = 0;


  /* block [drive:] [track] [sector] [disp]   show disk blocks in hex form */

    if ((nargs > 1) && (types[1] != T_NUMBER)) {
	switch (nargs) {
	  case 5:
	    disp = values[4];
	  case 4:
	    sector = values[3];
	  case 3:
	    track = values[2];
	}
	drive = find_devno(drive, args[1]);
    }
    else
	switch (nargs) {
	  case 4:
	    disp = values[3];
	  case 3:
	    sector = values[2];
	  case 2:
	    track = values[1];
	}

    if ((err = check_drive(drive, CHK_RDY)) < 0)
	return (err);
    floppy = DriveData[drive & 3];

    if (check_track_sector(floppy->ImageFormat, track, sector) < 0) {
	sector = 0;
	track = DSK_DIR_TRACK;
	return FD_BAD_TS;
    }


    /* Read one block */

    if (open_1541(floppy, "#", 1, channel)) {
	printf("u%d: cannot open buffer #%d.\n", drive, channel);
	return (FD_RDERR);
    }
    sprintf((char *)str, "B-R:%d 0 %d %d", channel, track, sector);
    if (ip_execute(floppy, (BYTE *)str, strlen((char *)str)) != 0) {
	track = DSK_DIR_TRACK;
	sector= 0;
	return (FD_RDERR);
    }

    buf = floppy->buffers[channel].buffer;


  /* Show block */

    printf("<%2d: %2d %2d>\n", drive, track, sector);
    str[16] = 0;
    for (; disp < 256;) {
	printf("> %02X ", disp &255);
	for (cnt = 0; cnt < 16; cnt++, disp++) {
	    printf(" %02X", buf[disp &255]);
	    str[cnt] = (buf[disp &255] < ' ' ?
			'.' : p_toascii(buf[disp &255], 0));
	}
	printf("  ;%s\n", str);
    }

    /* Find next sector for the file being traced */
    if (buf[0] && buf[1]) {
	track  = buf[0];
	sector = buf[1];
    }
    else if (check_track_sector(floppy->ImageFormat, track, ++sector) < 0) {
	sector = 0;
	if (++track > floppy->NumTracks)
	    track = DSK_DIR_TRACK;
    }

    close_1541(floppy, channel);
    return (FD_OK);
}


/*
 * Extract all files (gwesp@cosy.sbg.ac.at)
 */

/* FIXME!!!  This code is non-portable!!!  */

/* This structure is from cbmargc.c of fvcbm by Dan Fandrich */

#ifdef __GNUC__
#define PACK __attribute__ ((packed))	/* pack structures on byte boundaries */
#else
#define PACK			/* pack using a compiler switch instead */
#endif

typedef struct {
    BYTE Link[2];		/* On the 1st entry only */

    BYTE FileType PACK;
    BYTE FirstTrack PACK;
    BYTE FirstSector PACK;
    BYTE FileName[16] PACK;
    BYTE FirstSideTrack PACK;
    BYTE FirstSideSector PACK;
    BYTE RecordSize PACK;
    BYTE Filler[4] PACK;
    BYTE FirstReplacementTrack PACK;
    BYTE FirstReplacementSector PACK;
    BYTE FileBlocks[2] PACK;
} dirslot;

static int  disk_extract(void)
{
    int  drive = 8, track = DSK_DIR_TRACK, sector = DSK_DIR_SECTOR;
    DRIVE *floppy;
    BYTE  *buf, str[20];
    int    err;
    int    channel = 2;

    if ((err = check_drive(drive, CHK_RDY)) < 0)
	return (err);
    floppy = DriveData[drive & 3];

    if (open_1541(floppy, "#", 1, channel)) {
	printf("u%d: cannot open buffer #%d.\n", drive, channel);
	return (FD_RDERR);
    }

    while(1)
    {
	int i;
	/* Read one block */

	sprintf((char *)str, "B-R:%d 0 %d %d", channel, track, sector);
	if (ip_execute(floppy, (BYTE *)str, strlen((char *)str))) {
	    return (FD_RDERR);
	}

	buf = floppy->buffers[channel].buffer;

	for(i=0; i < 256; i+= 32) {
	    dirslot *entry=  (dirslot *) (buf + i);
	    if ((((entry->FileType & 7) == (FT_SEQ))
		 || ((entry->FileType & 7) == (FT_PRG))
		 || ((entry->FileType & 7) == (FT_USR))
		) && (entry->FileType & FT_CLOSED)) {
		int len;
		BYTE c;
		FILE *fd;
		BYTE name[17];
		BYTE *l= memccpy(name,entry->FileName,0xa0,16);

		if (l) {
		    *(l - 1)= 0;	/* terminate by 0 */
		    len= l - 1 - name;
		} else {
		    name[16]= 0;
		    len= 16;
		}

		petconvstring(name,1);
		printf("%s\n",name);

		unix_filename(name);	/* for now, convert '/' to '_' */

		if (open_1541(floppy,entry->FileName,len,0)) {
		    printf("u%d: cannot open `%s' on image.\n", drive, name);
		    continue;
		}

		if (!(fd= fopen(name,"wb"))) {
		    perror("Couldn't open unix file");
		    close_1541(floppy,0);
		    continue;
		}

		while (!read_1541(floppy,&c,0))
		    fputc(c,fd);

		close_1541(floppy,0);

		if (fclose(fd)) {
		    perror("fclose");
		    return FD_RDERR;
		}
	    }
	}
	if (buf[0] && buf[1]) {
	    track  = buf[0];
	    sector = buf[1];
	}
	else break;
    }

    close_1541(floppy, channel);
    return (FD_OK);
}


#define OPEN15	0

static int disk_raw_command(void)
{
    DRIVE *floppy = DriveData[DriveNum & 3];
#if OPEN15
    char c;
#endif

#if OPEN15
    if (open_1541(floppy, "", 0, 15)) {
	printf("Can't open channel 15\n");
	return (FD_NOTREADY);
    }
#endif

    /* Write to the command channel */
    if (nargs >= 2) {
#if OPEN15
        char *cp;
#endif

	strcpy((char *)newname, args[1]);
	petconvstring(newname, 1);
#if OPEN15
	for (cp = newname; *cp; cp++) {
	    if (write_1541(floppy, *cp, 15)) {
		printf("Error writing to channel 15\n");
		break;
	    }
	}
	flush_1541(floppy, 15);
#else
        ip_execute(floppy, (BYTE *)newname, strlen(newname));
#endif
    }

    /* Print the error now */
#if OPEN15
    while (!read_1541(floppy, (BYTE *)&c, 15)) {
	putchar(c);
    }

    close_1541(floppy, 15);
#else
    printf("%s\n", floppy->buffers[15].buffer);
#endif
    return FD_OK;
}


static int  disk_info (void)
{
    DRIVE *floppy;
    hdrinfo hdr;
    int     err;


    if ((err = check_drive(DriveNum, CHK_RDY)) < 0)
	return (err);

    floppy = DriveData[DriveNum & 3];
    if ((err = check_header(floppy -> ActiveFd, &hdr)) < 0)
	return (err);


    /* Check if there's a new description provided. */

    if (args[1]) {
	return (set_label(floppy -> ActiveFd, args[1]));
    }
    else {
	printf ("\nDescription: %s\n",
		(*hdr.description ? hdr.description : "None."));
	printf ("Drive Type : %d.\n", floppy->ImageFormat);  /* Compatible drive */
	printf ("Disk Format: %c.\n", hdr.format);
	printf ("Sides\t   : %d.\n", hdr.sides);
	printf ("Tracks\t   : %d.\n", hdr.tracks);
	printf ((hdr.errblk ? "Error Block present.\n" : "No Error Block.\n"));
	printf ("Write protect: %s.\n",  hdr.wprot ? "On" : "Off");
    }

    return(0);
}


static void usage()
{
    fprintf (stderr, "\nUsage: following commands are supported on c1541\n\
    -format image 'name,id'        Format image. Create if needed\n\
    -delete image files            Delete files from image\n\
    -list image                    List files on image\n\
    -validate image                Validate disk\n    %s\n\
    -create image dump ['descr']   Build image out of raw blockdump\n\
                                    (683 256-byte blocks, e.g. *.d64 file)\n\
    -zcreate image zipfile         Create image out of a ZIPcoded file\n\
    -unlynx image lynxfile         Extract contents of a LYNX file\n\
    -tape image tapeimage [files]  Transfer files from *.t64 image\n\
    -read image 1541name [FSname]  Read 1541name on image to file FSname\n\
    -extract image                 Extract all files on image\n\
    -write image FSname [1541name] Write bin or P00 file FSname to image\n\
    -block [drive:] [track] [sector] [disp] \n\
                                   Show disk blocks in hex format\n\
    -info image ['description']    Show image and 1541 version\n\
    [image list]                   Enter interactive mode\n\n\
\tA '-' in place of FSname stands for stdin/stdout.\n\n",
#ifdef COPY_TO_LEFT
	     "-copy image newname oldname [oldname2 ...]"
#else
	     "-copy image oldname [oldname2 ...] newname"
#endif
	     );

    exit(1);
}


/*
 * Help
 */

static int  disk_help(void)
{
    int     i,n;

    if (nargs == 2) {
	n = strlen(args[1]);
	for (i = 0; disk_cmds[i].command; i++)
	    if (!strncmp(disk_cmds[i].command, args[1], n)) {
		printf("\n  %s\n", disk_cmds[i].help_line);
		/* add help page here */
		break;
	    }
    }
    else {
	printf("\nFollowing commands are available:\n\n");
	    for (i = 0; disk_cmds[i].command; i++)
		printf("  %s", disk_cmds[i].help_line);
	printf("\n\tAll commands may be abbreviated\n");
    }
    return (0);
}


static int  disk_quit(void)
{
    return 1;
}


/* Lynx support added by Riccardo Ferreira (storm@esoterica.pt) --
   1998-02-07.  */
static int disk_unlynx(void)
{
    DRIVE *floppy;
    FILE *f, *f2;
    int err, dev, cnt=0;
    long dentries, lbsize, bsize, dirsize;
    BYTE val;
    char buff[256] = { 0 }, cname[20] = { 0 }, ftype;

    if (!(f=fopen(args[1], READ))) {
	printf("cannot open %s for reading\n", args[1]);
	return (FD_NOTRD);
    }

    /* Look for the 0, 0, 0 sign of the end of BASIC */
    while (1) {
	fread(&val, 1, 1, f);
	if (val == 0)
	    cnt++;
	else
	    cnt = 0;
	if (cnt == 3)
	    break;
    }

    /* Bypass the 1st return in the file */
    fgetc(f);

    /* Get the directory block size */
    cnt = 0;
    while (1) {
	fread(&val, 1, 1, f);
	if (val != 13)
	    buff[cnt++] = val;
	else
	    break;
    }
    buff[cnt] = 0;
    if (string_to_long(buff, NULL, 10, &dirsize) < 0
	|| dirsize <= 0) {
	printf("invalid LyNX file.\n");
	return (FD_RDERR);
    }

    /* Get the number of dir entries */
    cnt = 0;
    while (1) {
	fread(&val, 1, 1, f);
	if (val != 13)
	    buff[cnt++] = val;
	else
	    break;
    }
    buff[cnt] = 0;
    if (string_to_long(buff, NULL, 10, &dentries) < 0
	|| dentries <= 0) {
	printf("invalid LyNX file.\n");
	return (FD_RDERR);
    }

    /* Open the file for reading of the chained data */
    f2 = fopen(args[1], READ);
    fseek(f2, (dirsize*254), SEEK_SET);

    /* Loop */
    while (dentries != 0) {
	/* Read CBM filename */
	cnt = 0;
	while (1) {
	    fread(&val, 1, 1, f);
	    if (val != 13) cname[cnt++] = val; else break;
	}
	cname[cnt] = 0;

	/* Read the block size */
	cnt = 0;
	while (1) {
	    fread(&val, 1, 1, f);
	    if (val != 13) buff[cnt++] = val; else break;
	}
	buff[cnt] = 0;

	if (string_to_long(buff, NULL, 10, &bsize) < 0) {
	    printf("invalid LyNX file.\n");
	    return (FD_RDERR);
	}

	/* Get the file type (P[RG], S[EQ], R[EL], U[SR]) */
	ftype = fgetc(f);
	fgetc(f);

	/* FIXME: REL type files unsupported */
	if (ftype == 'R') {
	    printf("REL not supported.\n");
	    return (FD_RDERR);
	}

	/* FIXME: This is a temporary hack!  How can we persuade `open_1541()'
	   to write `DEL' files without breaking compatibility with CBM
	   DOS?  -- [EP] 98.04.17  */
	if (ftype == 'D')
	    ftype = 'P';

	/* Add the file type to the name */
	cnt = strlen(cname);
	cname[cnt++] = ',';
	cname[cnt++] = ftype;
	cname[cnt] = 0;

	/* Get the byte size of the last block +1 */
	cnt = 0;
	while (1) {
	    fread(&val, 1, 1, f);
	    if (val != 13)
		buff[cnt++] = val;
	    else
		break;
	}
	buff[cnt] = 0;

	if (string_to_long(buff, NULL, 10, &lbsize) < 0) {
	    printf("invalid LyNX file.\n");
	    return (FD_RDERR);
	}

	/* Calculate byte size of file */
	cnt = bsize*254;

	/* Get the device number */
	dev = find_devno(DriveNum, (char *)NULL);

	if ((err = check_drive(dev, CHK_RDY))<0)
	    return(err);

	floppy = DriveData[dev & 3];

	if (open_1541(floppy, cname, strlen(cname), 1)) {
	    printf("couldn't open '%s' for writing.\n", cname);
	    return (FD_WRTERR);
	}

	printf("writing file '%s' to image.\n", cname);

	while (cnt != 0) {
	    fread(&val, 1, 1, f2);
	    if (write_1541(floppy, val, 1)) {
		printf("no space on image ?\n");
		break;
	    }
	    cnt--;
	}
	close_1541(floppy, 1);

	/* Adjust for the last block */
	fread(buff, 1, 254 - lbsize, f2);
	dentries--;
    }
    fclose(f);
    fclose(f2);
    return(0);
}

/* `system' command added by Riccardo Ferreira (storm@esoterica.pt) --
   1998-02-07.  */
int disk_system(void)
{
	return system(args[1]);
}


/*
 * Disk Maintenance main program
 */

int     main(argc, argv)
     int     argc;
     char   *argv[];
{
    char   *progname, *line;
    int     flg = 0, i;

#if defined __MSDOS__ || defined WIN32
    /* Set the default file mode.  */
    _fmode = O_BINARY;
#endif

    progname = argv[0];
    nargs    = 0;

    initialize_1541(8, DT_DISK | DT_1541, NULL, NULL, NULL);
    initialize_1541(9, DT_DISK | DT_1541, NULL, NULL, NULL);


    /* Direct mode */

    if (argc > 1 && argv[1][0] == '-') {
	char *ImageName;

	if (argc < 3)
	    usage();

	args[0]  = argv[1] + 1;				/* Command */
	ImageName = argv[2];

	if (!strncmp(args[0], "f", 1) || !strncmp(args[0], "cr", 2) ||
	    !strncmp(args[0], "zcr", 3)) {
	    --argc;
	    ++argv;
	} else {
	    argc -= 2;
	    argv += 2;
	}

	for (nargs = 1; nargs < argc; nargs++) {	/* Parameters */
	    if (nargs >= MAXARG) {
		fprintf(stderr, "%s: Bailing out: Too many arguments.\n\n",
		    progname);
		usage();
	    }
	    args[nargs] = argv[nargs];
	    values[nargs] = strlen(args[nargs]);
	}

	if (strncmp(args[0], "f", 1) && strncmp(args[0], "cr", 2))
	    if (open_image(0, ImageName, 0) < 0) {
		printf ("Cannot open image\n");
		exit (1);
	    }

	printf("\n");
	if (eval_command(args[0], nargs, disk_cmds) < 0) {	/* any error */
	    printf(" I won't do that.\n");
	    usage();
	}
    }

    /* Mainloop: parse & execute */

    else {
	char buf[16];

	fprintf (stderr, "\nC1541 V %d.%02d  Image %d.%02d\n\n",
		 C1541_VERSION_MAJOR, C1541_VERSION_MINOR,
		 HEADER_VERSION_MAJOR, HEADER_VERSION_MINOR);


	while (--argc && ++argv && nargs <= MAXDRIVE) {		/* Images */
	    if (*argv[0])
		if (open_image(nargs, argv[0], 0) < 0) {	/* open each */
		    printf ("Invalid image\n");
		    exit (1);
		}
	    ++nargs;
	}


	while (!flg) {
	    sprintf (buf, "c1541 #%d> ", (DriveNum | 8) );
	    if ((line = read_line(buf, MODE_MON)) == NULL)
		break;

	    if (*line) {
		nargs = split_args(line, 0,
				   MAXARG, MAXVAL, args, values, types);

		switch ( eval_command(args[0], nargs, disk_cmds) ) {
		  case FD_EXIT:
		    ++flg;
		  case FD_OK:
		    break;

		  case FD_NOTREADY:
		    printf("Drive not ready.\n");
		    break;
		  case FD_CHANGED:
		    printf("Image file has changed on disk.\n");
		    break;
		  case FD_NOTRD:
		    printf("Cannot read file.\n");
		    break;
		  case FD_NOTWRT:
		    printf("Cannot write file.\n");
		    break;
		  case FD_WRTERR:
		    printf("Floppy write failed.\n");
		    break;
		  case FD_RDERR:
		    printf("Floppy read failed.\n");
		    break;
		  case FD_INCOMP:
		    printf("Incompatible DOS version.\n");
		    break;
		  case FD_BADIMAGE:
		    printf("Invalid image.\n");		/* Disk or tape */
		    break;
		  case FD_BADNAME:
		    printf("Invalid filename.\n");
		    break;
		  case FD_BADVAL:
		    printf("Illegal value.\n");
		    break;
		  case FD_BADDEV:
		    printf("Illegal device number.\n");
		    break;
		  case FD_BAD_TS:
		    printf("Inaccessible Track or Sector.\n");
		    break;

		  default:
		    printf(" Unknown command. Try 'help'\n");
		}
		printf("\n");

	    } /* line */
	}  /* while */
    } /* Mainloop */


    for (i = 0; i <= MAXDRIVE; i++) {
	if (DriveData[i])
	    detach_floppy_image(DriveData[i]);
    }

    printf("\n");
    return (0);
}

int attach_fsdevice(int device, char *var, char *name)
{
    DriveData[device & 3] = (DRIVE *)var;
    return 0;
}

int serial_attach_device(int device, char *var, char *name,
                         int (*getf)(void *, BYTE * , int),
                         int (*putf)(void *, BYTE , int),
                         int (*openf)(void *, char *, int , int),
                         int (*closef)(void *, int),
                         void (*flushf)(void *, int))
{
    DriveData[device & 3] = (DRIVE *)var;
    return 0;
}

