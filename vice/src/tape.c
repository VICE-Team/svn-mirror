/*
 * tape.h - Cassette drive and P00.
 *
 * Written by:
 *  Dan Fandrich	 (dan@fch.wimsey.bc.ca)
 *  Jouko Valta		 (jopi@stekt.oulu.fi)
 *  Martin Pottendorfer  (Martin.Pottendorfer@autalcatel.at)
 *  Ettore Perazzoli	 (ettore@comm2000.it)
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

#include "vice.h"

#ifdef STDC_HEADERS
#ifdef __riscos
#include "ROlib.h"
#else
#include <sys/types.h>
#include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#endif

#include "vdrive.h"
#include "tape.h"
#include "zfile.h"
#include "utils.h"
#include "charsets.h"

#ifndef SEEK_SET
#define SEEK_SET 0
typedef long       fpos_t;
#endif

#ifdef __GNUC__
#define PACK __attribute__ ((packed))	/* pack structures on byte boundaries */
#else
#define PACK		/* pack using a compiler switch instead */
#endif


typedef struct {
	BYTE Magic[8] PACK;
	char CbmName[17] PACK;
	BYTE RecordSize PACK;		/* REL file record size */
} X00HDR;


/* Global */

extern char *slot_type[];

static const BYTE MagicHeaderP00[8] = "C64File\0";


/*---------------------------------------------------------------------------*/

/*
 * Check if given filename has valid P00 extension.
 * Return the filetype or -1 if no match.
 */

int  is_pc64name (const char *name)
{
    int   t = -1;
    char *p;


    if (name == NULL || (p = strrchr(name, '.')) == NULL || strlen(++p) != 3)
	return (-1);

    if (!isdigit((int)p[1]) || !isdigit((int)p[2]))
	return (-1);

    switch (toupper(*p)) {
      case 'D': t = FT_DEL; break;
      case 'S': t = FT_SEQ; break;
      case 'P': t = FT_PRG; break;
      case 'U': t = FT_USR; break;
      case 'R': t = FT_REL; break;
    }

    return (t);
}


/*
 * The read pointer must be positioned at the beginning of the
 * file before calling this routine. (stdin cannot be fseek(2)'ed)
 */

int  read_pc64header (FILE *fd, char *name, int *reclen)
{
    X00HDR  hdr;


    if (fread (&hdr, sizeof(X00HDR), 1, fd) != 1) {
	fprintf (errfile, "\nP00 header read failed.\n");
	return (FD_RDERR);
    }

    if (memcmp (hdr.Magic, MagicHeaderP00, 8) != 0)
	return (FD_BADIMAGE);

    strncpy (name, hdr.CbmName, 16);
    *reclen = hdr.RecordSize;		/* REL file record size */

    return (FD_OK);
}


int  write_pc64header (FILE *fd, const char *name, int reclen)
{
    X00HDR  hdr;


    memset (&hdr, 0, sizeof(X00HDR));
    strncpy ((char *)hdr.Magic, (char *)MagicHeaderP00, 8);
    strncpy (hdr.CbmName, name, 16);

    hdr.RecordSize = reclen;		/* REL file record size */

    if (fseek(fd, 0, SEEK_SET) != 0)
	return (FD_WRTERR);

    return
	(fwrite(&hdr, sizeof(X00HDR), 1, fd));
}


/*
 * If the file is a PC64 file, read the actual filename from the header.
 * Otherwise, return the filename converted to appropriate PETSCII
 * representation.
 *
 * This function cannot be used on stdin.
 */

char  *pc_get_cbmname (FILE *fd, char *fsname)
{
    static char cbmname[24];
    char  *p;
    int    type;
    int    reclen;


    type = is_pc64name (fsname);

    fseek(fd, 0, SEEK_SET);

    if (type >= 0 && read_pc64header (fd, cbmname, &reclen) == FD_OK) {
	for (p = cbmname; *p && (BYTE)*p != 0xA0; ++p);
	sprintf (p, ",%s", slot_type[type]);
    }
    else {
	rewind (fd);		/* There is no header */


	/* name */

	if ((p = strrchr(fsname, '/')) == NULL || !p++)
	    p = fsname;

	if (!p || !*p)
	    return (NULL);


	/* Get Basename and extension from FSname */

	strncpy (cbmname, p, 16);

	if ((p = strrchr(fsname, '.')) != NULL && strlen(++p) == 3) {
	    ;
	}

	if (type >= 0) {
	    p = cbmname + strlen(cbmname);
	    sprintf (p, ",%s", slot_type[type]);
	}
	else
	    strcat (cbmname, ",prg");		/* Best default */
    } /* !p00 */


    return (cbmname);
}



/* ------------------------------------------------------------------------- */

/*
 * Read directory
 *
 * P00 is just a regular file with a simple header prepended, so just read the
 * header and return the name and addresses evaluated.
 *
 * This function cannot be used on stdin.
 */


int  DirP00(FILE *fd, char *CbmName, int *start, int *end)
{
    X00HDR  hdr;
    unsigned int  FileLength;
    int     realtype = -1;
    char   *FileExt;
    BYTE    buf[2];


    if (fseek(fd, 0, SEEK_SET) != 0)
	return (FD_RDERR);

    if (fread(&hdr, sizeof(hdr), 1, fd) != 1) {
	return (FD_RDERR);
    }


    /* If archive type is unknown, see if file is REL */

    if (/*(realtype == X00) && */ (hdr.RecordSize > 0))
	realtype = FT_REL;


    switch (realtype) {
	case FT_DEL: FileExt = "DEL"; break;
	case FT_SEQ: FileExt = "SEQ"; break;
	case FT_PRG: FileExt = "PRG"; break;
	case FT_USR: FileExt = "USR"; break;
	case FT_REL: FileExt = "REL"; break;
	default:  FileExt = "???";
    }

    strncpy(CbmName, (char *) hdr.CbmName, 16);
    CbmName[16] = 0;		/* never need this on a good P00 file */

    strcat (CbmName, ",");
    strcat (CbmName, FileExt);


    /* Start and End Addresses */

    FileLength = file_length(fileno(fd)) - sizeof(hdr) -2; /* skip start addr */

    if (fread(buf, 2, 1, fd) != 1) {
	return (FD_RDERR);
    }

    *start= *buf | (buf[1] << 8);
    *end = *start + FileLength;

    return (FD_OK);
}


/* ------------------------------------------------------------------------- */

/*
 * T64 Tape Image handling functions
 */

#define TAPE_HDR_SIZE	64
#define TAPE_DIR_SIZE	32

#define TAPE64_HEADERS	2

static const char *MagicHeaderT64[] = {
  "C64 tape image file",
  "C64S tape file",
  "C64S tape image file" /* EP */
};


/* ------------------------------------------------------------------------- */

/*
 * Check if given filename has valid .T64 extension.
 * Return the filetype or -1 if no match.
 */

int  is_t64name (const char *name)
{
    char *p;


    if (name == NULL || (p = strrchr(name, '.')) == NULL || strlen(++p) != 3)
	return (-1);

    if (toupper(*p) == 'T' && p[1] == '6' && p[2] == '4')
	return (1);

    return (-1);
}


/*
 * Check the image for a valid T64 header and return the
 * number of files it carries.
 */

int  check_t64_header (FILE *fd)
{
    BYTE  buf[TAPE_HDR_SIZE];
    int   maxentries = 0;


    if (fseek(fd, 0, SEEK_SET) != 0)
	return (FD_RDERR);

    fread(buf, 1, TAPE_HDR_SIZE, fd);

    if (memcmp(buf, (BYTE *)MagicHeaderT64[0], strlen(MagicHeaderT64[0])) &&
	memcmp(buf, (BYTE *)MagicHeaderT64[1], strlen(MagicHeaderT64[1])) &&
        memcmp(buf, (BYTE *)MagicHeaderT64[2], strlen(MagicHeaderT64[2])))
	return (FD_BADIMAGE);


    if (buf[32] != 0 || buf[33] != 1)
	fprintf (errfile, "Warning: Unknown tape version.\n");

    maxentries = buf[34];
    maxentries |= (buf[35] << 8);

    if (!maxentries) {
	fprintf(logfile, "Cannot locate any files on the tape.\n");
	return (FD_BADIMAGE);		/* tapeimage, that is */
    }

    fprintf(logfile, "Tape version %d.%.2d with %2d out of %d files.\n",
	   buf[33], buf[32], (buf[37] << 8) | buf[36], maxentries);

    return (maxentries);
}

/* ------------------------------------------------------------------------- */

/* Read the list of files in the T64 image and return it as a long malloc'ed
   ASCII string. */
char *read_tape_image_contents(const char *fname)
{
    BYTE inbuf[TAPE_HDR_SIZE + 1];
    char linebuf[512], *outbuf;
    int outbuf_size, max_outbuf_size;
    int no_entries, max_entries, res, len, i;
    file_desc_t fd;

    fd = zopen(fname, O_RDONLY, 0);
    if (fd == ILLEGAL_FILE_DESC)
	return NULL;

    /* Check whether this really looks like a tape image. */
    if (read (fd, inbuf, TAPE_HDR_SIZE) != TAPE_HDR_SIZE
	|| ((memcmp(inbuf, (BYTE *)MagicHeaderT64[0],
		    strlen(MagicHeaderT64[0]))
	     && memcmp(inbuf, (BYTE *)MagicHeaderT64[1],
		       strlen(MagicHeaderT64[1]))
	     && memcmp(inbuf, (BYTE *)MagicHeaderT64[2],
		       strlen(MagicHeaderT64[2]))))) {
	zclose(fd);
	return NULL;
    }

    max_entries = (unsigned int)inbuf[34] + (unsigned int)inbuf[35] * 256;
    no_entries = (unsigned int)inbuf[36] + (unsigned int)inbuf[37] * 256;

    /* Check a little bit... */
    if (no_entries > max_entries) {
	fprintf(errfile, "Tape inconsistency, giving up!\n");
	zclose(fd);
	return NULL;
    }

    /* Many T64 images are broken... */
    if (!no_entries)
	no_entries = 1;

    /* Seek to start of file records. */
    if (lseek (fd, 64, SEEK_SET) < 0) {
	perror("lseek to file records failed");
	zclose(fd);
	return NULL;
    }

    /* Allocate the buffer. */
    outbuf_size = 0;
    max_outbuf_size = 4096;
    outbuf = xmalloc(max_outbuf_size);

    inbuf[40 + 24] = '\0';
    len = sprintf(linebuf, "0 .\"%s\"\n", &inbuf[40]);
    bufcat(outbuf, &outbuf_size, &max_outbuf_size, linebuf, len);

    for (i = 0; i < no_entries; i++) {
	char *ext;
	int start_addr, end_addr;
	int blocks;

	res = read(fd, inbuf, 32);
	if (res != 32) {
	    perror("read of file record failed");
	    zclose(fd);
	    free(outbuf);
	    return NULL;
	}
	if (inbuf[0] == 0)
	    /* Don't print free slots. */
	    continue;
	else if (inbuf[0] == 1)
	    ext = "PRG";
	else
	    ext = "???";

	/* Cut name. */
	inbuf[32] = '\0';

        start_addr = (unsigned int)inbuf[2] + (unsigned int)inbuf[3] * 256;
        end_addr = (unsigned int)inbuf[4] + (unsigned int)inbuf[5] * 256;
        blocks = (end_addr - start_addr) / 256 + 1;

	if (blocks <= 0)
	    len = sprintf(linebuf, "???   \"%s\" %s\n", &inbuf[16], ext);
	else
	    len = sprintf(linebuf, "%-6d\"%s\" %s\n", blocks, &inbuf[16], ext);

	bufcat(outbuf, &outbuf_size, &max_outbuf_size, linebuf, len);
    }

    len = sprintf(linebuf, "%d/%d ENTRIES USED.\n", no_entries, max_entries);
    bufcat(outbuf, &outbuf_size, &max_outbuf_size, linebuf, len);

    /* Add trailing zero. */
    *(outbuf + outbuf_size) = '\0';

    /* Convert from PETSCII to ASCII. */
    petconvstring(outbuf, 1);

    zclose(fd);
    return outbuf;
}

