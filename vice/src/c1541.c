/*
 * c1541.c - Stand-alone disk image maintenance program.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
 *  Teemu Rantanen   (tvr@cs.hut.fi)
 *  Jouko Valta      (jopi@zombie.oulu.fi)
 *  Gerhard Wesp     (gwesp@cosy.sbg.ac.at)
 *  Daniel Sladic    (sladic@eecg.toronto.edu)
 *  Ricardo Ferreira (storm@esoterica.pt)
 *  Andreas Boose    (boose@unixserv.rz.fh-hannover.de)
 *
 * Patches by
 *  Olaf Seibert     (rhialto@mbfys.kun.nl)
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

#include "vice.h"		/* for convenience */

#ifdef STDC_HEADERS
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <limits.h>
#include <ctype.h>
#include <string.h>
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#include <errno.h>
#ifdef __riscos
#include "ROlib.h"
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifndef _MSC_VER
#include <unistd.h>
#endif
#endif
#include <memory.h>
#include <assert.h>
#endif

#include "archdep.h"
#include "charsets.h"
#include "gcr.h"
#include "info.h"
#include "p00.h"
#include "serial.h"
#include "t64.h"
#include "utils.h"
#include "vdrive-iec.h"
#include "vdrive.h"
#include "zipcode.h"

#define MAXARG		256
#define MAXVAL		0xffff

#define MAXDRIVE	1

#define C1541_VERSION_MAJOR	3
#define C1541_VERSION_MINOR	0

/* Global */

DRIVE *drives[4] = {NULL, NULL, NULL, NULL};

typedef struct {
    signed char ImageFormat;		/* 1541/71/81 */
    int TracksSide;
    int Sides;
    int TotalBks;
} DiskFormats;
static DiskFormats Legal_formats[] = {
    { DT_1541, 35, 0, 683 },
    { DT_1541, 40, 0, 768 },
    /*{ DT_1541, 42, 0,  785 }, */
    { DT_1571, 35, 1, 1366 },
    { DT_1581, 80, 0, 3200 },
    { DT_8050, 77, 0, 2083 },
    { DT_8250, 72, 1, 4166 },
    {-1, 0, 0, 0 }
};

static int drive_number = 0;

/* Local functions.  */

static int check_drive(int dev, int mode);
static int open_image(int dev, char *name, int create, int disktype);
static int create_image(FILE *fd, int devtyp, int tracks, int errb,
			char *label, int disktype);

static int raw_cmd(int nargs, char **args); /* @ */
static int attach_cmd(int nargs, char **args);
static int block_cmd(int nargs, char **args);
static int copy_cmd(int nargs, char **args);
static int create_cmd(int nargs, char **args);
static int delete_cmd(int nargs, char **args);
static int extract_cmd(int nargs, char **args);
static int format_cmd(int nargs, char **args);
static int gcrformat_cmd(int nargs, char **args);
static int help_cmd(int nargs, char **args);
static int info_cmd(int nargs, char **args);
static int list_cmd(int nargs, char **args);
static int name_cmd(int nargs, char **args);
static int quit_cmd(int nargs, char **args);
static int read_cmd(int nargs, char **args);
static int rename_cmd(int nargs, char **args);
static int show_cmd(int nargs, char **args);
static int tape_cmd(int nargs, char **args);
static int unit_cmd(int nargs, char **args);
static int unlynx_cmd(int nargs, char **args);
static int validate_cmd(int nargs, char **args);
static int write_cmd(int nargs, char **args);
static int zcreate_cmd(int nargs, char **args);

extern char sector_map_1541[43];	/* Ugly: FIXME! */
extern int speed_map_1541[42];

/* ------------------------------------------------------------------------- */

struct command {
    const char *name;
    const char *syntax;
    const char *description;
    unsigned int min_args, max_args;
    int (*func) (int nargs, char **args);
};
typedef struct command command_t;

command_t command_list[] = {
    { "@",
      "@ [<command>]",
      "Execute specified CBM DOS command and print the current status of the\n"
      "drive.  If no <command> is specified, just print the status.",
      0, 1, raw_cmd },
    { "?",
      "? [<command>]",
      "Explain specified command.  If no command is specified, list available\n"
      "ones.",
      0, 1, help_cmd },
    { "attach",
      "attach <diskimage> [<unit>]",
      "Attach <diskimage> to <unit> (default unit is 8).",
      1, 2,
      attach_cmd },
    { "block",
      "block <track> <sector> <disp> [<drive>]",
      "Show specified disk block in hex form.",
      3, 4, block_cmd },
    { "create",
      "create <x64name> <d64name>",
      "Create an X64 disk image out of a D64 disk image.",
      2, 2, create_cmd },
    { "copy",
      "copy <source1> [<source2> ... <sourceN>] <destination>",
      "Copy `source1' ... `sourceN' into destination.  If N > 1, `destination'\n"
      "must be a simple drive specifier (@n:).",
      2, MAXARG, copy_cmd },
    { "delete",
      "delete <file1> [<file2> ... <fileN>]",
      "Delete the specified files.",
      1, MAXARG,
      delete_cmd },
    { "dir",
      "dir [<pattern>]",
      "List files matching <pattern> (default is all files).",
      0, 1,
      list_cmd },
    { "exit",
      "exit",
      "Exit (same as `quit').",
      0, 0, quit_cmd },
    { "extract",
      "extract",
      "Extract all the files to the file system.",
      0, 0, extract_cmd },
    { "format",
      "format <diskname,id> [<type> <imagename>] [<unit>]",
      "If <unit> is specified, format the disk in unit <unit>.\n"
      "If <type> and <imagename> are specified, create a new image named\n"
      "<imagename>, attach it to unit 8 and format it.  <type> is a disk image\n"
      "type, and must be either `x64', `d64' (both VC1541/2031), `d71' (VC1571),\n" 
      "`d81' (VC1581), `d80' (CBM8050) or `d82' (CBM8250).\n"
      "Otherwise, format the disk in the current unit, if any.",
      1, 3,
      format_cmd },
    { "gcrformat",
      "gcrformat <diskname,id> <imagename>",
      "Create and format a G64 disk image named <imagename>.",
      1, 2, gcrformat_cmd },
    { "help",
      "help [<command>]",
      "Explain specified command.  If no command is specified, list available\n"
      "ones.",
      0, 1, help_cmd },
    { "info",
      "info [<unit>]",
      "Display information about unit <unit> (if unspecified, use the current\n"
      "one).",
      0, 1, info_cmd },
    { "list",
      "list [<pattern>]",
      "List files matching <pattern> (default is all files).",
      0, 1,
      list_cmd },
    { "name",
      "name <diskname>[,<id>] <unit>",
      "Change image name.",
      1, 2, name_cmd },
    { "quit",
      "quit",
      "Exit (same as `exit').",
      0, 0, quit_cmd },
    { "read",
      "read <source> [<destination>]",
      "Read <source> from the disk image and copy it into <destination> in\n"
      "the file system.  If <destination> is not specified, copy it into a\n"
      "file with the same name as <source>.",
      1, 2, read_cmd },
    { "rename",
      "rename <oldname> <newname>",
      "Rename <oldname> into <newname>.  The files must be on the same drive.",
      2, 2, rename_cmd },
    { "show",
      "show [copying | warranty]",
      "Show conditions for redistributing copies of C1541 (`copying') or the\n"
      "various kinds of warranty you do not have with C1541 (`warranty').",
      1, 1, show_cmd },
    { "tape",
      "tape <t64name> [<file1> ... <fileN>]",
      "Extract files from a T64 image into the current drive.",
      1, MAXARG, tape_cmd },
    { "unit",
      "unit <number>",
      "Make unit <number> the current unit.",
      1, 1, unit_cmd },
    { "unlynx",
      "unlynx <lynxname> [<unit>]",
      "Extract the specified Lynx image file into the specified unit (default\n"
      "is the current unit).",
      1, 2, unlynx_cmd },
    { "validate",
      "validate [<unit>]",
      "Validate the disk in unit <unit>.  If <unit> is not specified, validate\n"
      "the disk in the current unit.",
      0, 1, validate_cmd },
    { "write",
      "write <source> [<destination>]",
      "Write <source> from the file system into <destination> on a disk image.",
      1, 2, write_cmd },
    { "zcreate",
      "zcreate <x64name> <zipname> [<label,id>]",
      "Create an X64 disk image out of a set of four Zipcoded files named\n"
      "`1!<zipname>', `2!<zipname>', `3!<zipname>' and `4!<zipname>'.",
      2, 3, zcreate_cmd },
    { NULL, NULL, NULL, 0, 0, NULL }
};

/* ------------------------------------------------------------------------- */

#ifndef HAVE_READLINE

static char *read_line(char *prompt)
{
    static char line[1024];

    fputs(prompt, stdout);
    fflush(stdout);
    return fgets(line, 255, stdin);
}

#else

extern char *readline(const char *);	/* FIXME: This sucks.  */
extern void add_history(const char *);	/* FIXME: This stinks.  */

static char *read_line(const char *prompt)
{
    static char *line;

    if (line != NULL)
        free(line);
    line = readline(prompt);
    if (line != 0 && *line != 0)
        add_history(line);
    return line;
}

#endif

static int split_args(const char *line, int *nargs, char **args)
{
    const char *s;
    char *d;
    char tmp[256];
    int begin_of_arg, in_quote;

    *nargs = 0;

    in_quote = 0;
    d = tmp;
    begin_of_arg = 1;

    for (s = line;; s++) {
	switch (*s) {
	  case '"':
	      begin_of_arg = 0;
	      in_quote = !in_quote;
	      continue;
	  case '\\':
	      begin_of_arg = 0;
	      *(d++) = *(++s);
	      continue;
	  case ' ':
          case '\t':
          case '\n':
	  case 0:
	      if (*s == 0 && in_quote) {
		  fprintf(stderr, "Unbalanced quotes.\n");
		  return -1;
	      }
	      if (!in_quote && !begin_of_arg) {
		  if (*nargs == MAXARG) {
		      fprintf(stderr, "Too many arguments.\n");
		      return -1;
		  } else {
		      int len;

		      len = d - tmp;
		      if (args[*nargs] != NULL)
			  args[*nargs] = xrealloc(args[*nargs], len + 1);
		      else
			  args[*nargs] = xmalloc(len + 1);
		      memcpy(args[*nargs], tmp, len);
		      args[*nargs][len] = 0;
		      begin_of_arg = 1;
		      (*nargs)++;
		      d = tmp;
		  }
	      } else if (*s != 0) {
		  begin_of_arg = 0;
		  *(d++) = *s;
	      }
	      if (*s == 0)
		  return 0;
	      break;
	  default:
	      begin_of_arg = 0;
	      *(d++) = *s;
	}
    }

    return 0;
}

static int arg_to_int(const char *arg, int *return_value)
{
    char *tailptr;

    errno = 0;
    *return_value = (int) strtol(arg, &tailptr, 10);
    if (errno)			/* Overflow */
	return -1;

    /* Only whitespace is allowed after the last valid character.  */
    if (tailptr != NULL && *tailptr != 0) {
	while (isspace(*tailptr))
	    tailptr++;
	if (*tailptr != 0)
	    return -1;
    }
    return 0;
}

static void print_error_message(int errval)
{
    if (errval < 0)
	switch (errval) {
	  case FD_OK:
            break;
	  case FD_NOTREADY:
            fprintf(stderr, "Drive not ready.\n");
            break;
	  case FD_CHANGED:
            fprintf(stderr, "Image file has changed on disk.\n");
            break;
	  case FD_NOTRD:
            fprintf(stderr, "Cannot read file.\n");
            break;
	  case FD_NOTWRT:
            fprintf(stderr, "Cannot write file.\n");
            break;
	  case FD_WRTERR:
            fprintf(stderr, "Floppy write failed.\n");
            break;
	  case FD_RDERR:
            fprintf(stderr, "Floppy read failed.\n");
            break;
	  case FD_INCOMP:
            fprintf(stderr, "Incompatible DOS version.\n");
            break;
	  case FD_BADIMAGE:
            fprintf(stderr, "Invalid image.\n");	/* Disk or tape */
            break;
	  case FD_BADNAME:
            fprintf(stderr, "Invalid filename.\n");
            break;
	  case FD_BADVAL:
            fprintf(stderr, "Illegal value.\n");
            break;
	  case FD_BADDEV:
            fprintf(stderr, "Illegal device number.\n");
            break;
	  case FD_BAD_TS:
            fprintf(stderr, "Inaccessible Track or Sector.\n");
            break;
	  default:
            fprintf(stderr, "Unknown error.\n");
	}
}

#define LOOKUP_NOTFOUND         -1
#define LOOKUP_AMBIGUOUS        -2
#define LOOKUP_SUCCESSFUL(n)    ((n) >= 0)
static int lookup_command(const char *cmd)
{
    int cmd_len;
    int match;
    int i;

    match = LOOKUP_NOTFOUND;
    cmd_len = strlen(cmd);

    for (i = 0; command_list[i].name != NULL; i++) {
	int len;

	len = strlen(command_list[i].name);
	if (len < cmd_len)
	    continue;

	if (memcmp(command_list[i].name, cmd, cmd_len) == 0) {
	    if (match != -1)
                return LOOKUP_AMBIGUOUS;
	    match = i;
	    if (len == cmd_len)
		break;		/* Exact match.  */
	}
    }

    return match;
}

static int lookup_and_execute_command(int nargs, char **args)
{
    int match;

    match = lookup_command(args[0]);

    if (LOOKUP_SUCCESSFUL(match)) {
        command_t *cp;

        cp = &command_list[match];
        if (nargs - 1 < cp->min_args || nargs - 1 > cp->max_args) {
            fprintf(stderr, "Wrong number of arguments.\n");
            fprintf(stderr, "Syntax: %s\n", cp->syntax);
            return -1;
        } else {
            int retval;

            retval = command_list[match].func(nargs, args);
            print_error_message(retval);
            if (retval == FD_OK)
                return 0;
            else
                return -1;
        }
    } else {
        if (match == LOOKUP_AMBIGUOUS)
            fprintf(stderr, "Command `%s' is ambiguous.  Try `help'.\n", args[0]);
        else
            fprintf(stderr, "Command `%s' unrecognized.  Try `help'.\n", args[0]);
	return -1;
    }
}

static char *extract_unit_from_file_name(const char *name, int *unit_return)
{
    if (name[0] == '@' && name[2] == ':'
        && (name[1] == '8' || name[1] == '9')) {
        *unit_return = (int) (name[1] - '8');
        return (char *) name + 3;
    } else {
        return NULL;
    }
}

static int is_valid_cbm_file_name(const char *name)
{
    /* Notice that ':' is the same on PETSCII and ASCII.  */
    return strchr(name, ':') == NULL;
}

/* ------------------------------------------------------------------------- */

/* A simple pager.  */
/* It would be cool to have it in the monitor too.  */

static int pager_x, pager_y, pager_num_cols, pager_num_lines;

static void pager_init(void)
{
    if (isatty(fileno(stdout))) {
        pager_x = pager_y = 0;
        pager_num_lines = archdep_num_text_lines();
        pager_num_cols = archdep_num_text_columns();
    } else {
        pager_num_lines = pager_num_cols = -1;
    }
}

static void pager_print(const char *text)
{
    const char *p;

    if (pager_num_lines < 0 || pager_num_cols < 0) {
        fputs(text, stdout);
    } else {
        for (p = text; *p != 0; p++) {
            if (*p != '\n') {
                pager_x++;
                if (pager_x > pager_num_cols) {
                    pager_y++;
                    pager_x = 0;
                }
            } else {
                pager_x = 0;
                pager_y++;
            }

            if (pager_y == pager_num_lines - 1) {
                char *s;

                if (*p == '\n')
                    putchar(*p);

                s = read_line("---Type <return> to continue, or q <return> to quit---");
                if (s != NULL && toupper((int) *s) == 'Q')
                    break;

                pager_x = pager_y = 0;
                if (*p != '\n')
                    putchar(*p);
            } else {
                putchar(*p);
            }
        }
    }
}

/* ------------------------------------------------------------------------- */

/* Create a new floppy image.  */
static int create_image(FILE *fd, int devtype, int tracks, int errb,
                        char *label, int disktype)
{
    BYTE header[HEADER_LENGTH];
    BYTE block[256];
    int blks;
    int i;

    memset(header, 0, sizeof(header));
    memset(block, 0, sizeof(block));

    /* Check values */

    if (vdrive_check_track_sector(get_diskformat(devtype), tracks, 1) < 0)
	exit(-1);

    blks = num_blocks(get_diskformat(devtype), tracks);

    if (disktype == DISK_IMAGE_TYPE_X64) {
	header[HEADER_MAGIC_OFFSET + 0] = HEADER_MAGIC_1;
	header[HEADER_MAGIC_OFFSET + 1] = HEADER_MAGIC_2;
	header[HEADER_MAGIC_OFFSET + 2] = HEADER_MAGIC_3;
	header[HEADER_MAGIC_OFFSET + 3] = HEADER_MAGIC_4;
	header[HEADER_VERSION_OFFSET + 0] = HEADER_VERSION_MAJOR;
	header[HEADER_VERSION_OFFSET + 1] = HEADER_VERSION_MINOR;
	header[HEADER_FLAGS_OFFSET + 0] = devtype;
	header[HEADER_FLAGS_OFFSET + 1] = tracks;

	if (label)
	    strncpy((char *) header + HEADER_LABEL_OFFSET, label,
		    HEADER_LABEL_LEN);

	header[HEADER_LABEL_OFFSET + HEADER_LABEL_LEN] = 0;	/* terminator */

	printf("Writing header.\n");
	if (fwrite((char *) header, sizeof(header), 1, fd) < 1) {
	    fprintf(stderr, "Cannot write header.\n");
            return -1;
	}
    }

    printf("Creating blocks...\n");
    for (i = 0; i < blks; i++) {
	if (fwrite((char *) block, sizeof(block), 1, fd) < 1) {
	    fprintf(stderr, "Cannot write block %d.\n", i);
            return -1;
	}
    }

#if 0
    if (errb) {
	printf("Creating error data...\n");
	if (set_error_data(floppy, 5) < 0) {	/* clear and write */
	    fprintf(stderr, "Cannot write error data block.\n");
	    exit(1);
	}
    }
#endif

    return 0;
}

static int set_label(FILE *fd, const char *label)
{
    int siz = HEADER_LABEL_LEN + 1;
    char buf[HEADER_LABEL_LEN + 2];

    memset(buf, 0, siz);

    if (label)
	strncpy(buf, label, HEADER_LABEL_LEN);

    fseek(fd, (off_t) HEADER_LABEL_OFFSET, SEEK_SET);

    if (fwrite((char *) buf, siz, 1, fd) < 1) {
	return FD_WRTERR;
    }
    return FD_OK;
}

/* These 4 bytes are disk type flags (set upon create or format).  They
   contain: Device Type, Max Tracks, Side, and Error Flag.  */
static int set_disk_size(FILE *fd, int tracks, int sides, int errblk)
{
    int siz = HEADER_FLAGS_LEN;
    char buf[HEADER_FLAGS_LEN + 1];

    buf[0] = DEFAULT_DEVICE_TYPE;
    buf[1] = tracks;
    buf[2] = sides;
    buf[3] = errblk;

    fseek(fd, (off_t) HEADER_FLAGS_OFFSET, SEEK_SET);

    if (fwrite((char *) buf, siz, 1, fd) < 1) {
	return FD_WRTERR;
    }
    return FD_OK;
}

/* Open image or create a new one.  If the file exists, it must have valid
   header.  */
static int open_image(int dev, char *name, int create, int disktype)
{
    DRIVE *floppy;
    FILE *fd;
    int cdev = DT_1541, num_tracks = NUM_TRACKS_1541;

    if (dev < 0 || dev > MAXDRIVE)
	return -1;		/* FD_BADDEV */

    floppy = drives[dev & 3];

    if (create) {
	if ((fd = fopen(name, MODE_READ_WRITE /*, 0666*/)) == NULL) {
	    fprintf(stderr, "Cannot create image `%s': %s.\n", name, strerror(errno));
	    return -1;
	}

	/* Get default geometry.  Make a new image file and format it.  */

	switch (disktype) {
	  case DISK_IMAGE_TYPE_X64:
	      /* FIXME: X64 images can also contain other image types.  */
	      cdev = DT_1541;
	      num_tracks = NUM_TRACKS_1541;
	      break;
	  case DISK_IMAGE_TYPE_D64:
	      cdev = DT_1541;
	      num_tracks = NUM_TRACKS_1541;
	      break;
	  case DISK_IMAGE_TYPE_D71:
	      cdev = DT_1571;
	      num_tracks = NUM_TRACKS_1571;
	      break;
	  case DISK_IMAGE_TYPE_D81:
	      cdev = DT_1581;
	      num_tracks = NUM_TRACKS_1571;
	      break;
	  case DISK_IMAGE_TYPE_D80:
	      cdev = DT_8050;
	      num_tracks = NUM_TRACKS_8050;
	      break;
	  case DISK_IMAGE_TYPE_D82:
	      cdev = DT_8250;
	      num_tracks = NUM_TRACKS_8250;
	      break;
	}

	create_image(fd, cdev, num_tracks, 0, NULL, disktype);
	fclose(fd);
    }
    attach_floppy_image(floppy, name, 0);

    if (floppy->ActiveFd < 0)
	return -1;

    return 0;
}

static int check_drive(int dev, int flags)
{
    DRIVE *floppy;

    dev &= 7;
    if (dev < 0 || dev > 3)
	return FD_BADDEV;

    floppy = drives[dev & 3];

    if (!floppy || (flags != CHK_NUM && floppy->ActiveFd == NULL)) {
	return FD_NOTREADY;
    }
    return FD_OK;
}

/* ------------------------------------------------------------------------- */

/* Here are the commands.  */

/* Note: The double ASCII/PETSCII copies of file names we keep in some
   functions are needed because we want to print the names of the files being
   copied in ASCII and we don't trust `petconvstring()' to be reliable to get
   the original value back when we convert ASCII -> PETSCII and then PETSCII
   -> ASCII again.  */

static int attach_cmd(int nargs, char **args)
{
    int dev;

    switch (nargs) {
      case 2:
        /* attach <image> */
        dev = drive_number;
        break;
      case 3:
        /* attach <image> <unit> */
        if (arg_to_int(args[2], &dev) < 0)
            return FD_BADDEV;
        break;
    }

    if (check_drive(dev, CHK_NUM) < 0)
	return FD_BADDEV;

    attach_floppy_image(drives[dev & 3], args[1], 0);
    return FD_OK;
}

static int block_cmd(int nargs, char **args)
{
    int drive, track, sector, disp;
    DRIVE *floppy;
    BYTE *buf, str[20];
    int cnt;
    int channel = 2;

    /* block <track> <sector> <disp> [<drive>]  show disk blocks in hex form */
    if (arg_to_int(args[1], &track) < 0 || arg_to_int(args[2], &sector) < 0)
        return FD_BAD_TS;
    if (arg_to_int(args[3], &disp) < 0)
        return FD_BADVAL;

    if (nargs == 5) {
        if (arg_to_int(args[4], &drive) < 0)
            return FD_BADDEV;
        if (check_drive(drive, CHK_NUM) < 0)
            return FD_BADDEV;
        drive -= 8;
    } else {
        drive = drive_number;
    }

    if (check_drive(drive, CHK_RDY) < 0)
        return FD_NOTREADY;
    floppy = drives[drive & 3];

    if (vdrive_check_track_sector(floppy->ImageFormat, track, sector) < 0) {
        sector = 0;
        track = floppy->Dir_Track;
        return FD_BAD_TS;
    }
    /* Read one block */

    if (vdrive_open(floppy, "#", 1, channel)) {
        fprintf(stderr,
                "Cannot open buffer #%d in unit %d.\n", channel, drive + 8);
        return FD_RDERR;
    }
    sprintf((char *) str, "B-R:%d 0 %d %d", channel, track, sector);
    if (vdrive_command_execute(floppy, (BYTE *) str,
                               strlen((char *) str)) != 0) {
        track = floppy->Dir_Track;
        sector = 0;
        return FD_RDERR;
    }
    buf = floppy->buffers[channel].buffer;

    /* Show block */

    printf("<%2d: %2d %2d>\n", drive, track, sector);
    str[16] = 0;
    for (; disp < 256;) {
        printf("> %02X ", disp & 255);
        for (cnt = 0; cnt < 16; cnt++, disp++) {
            printf(" %02X", buf[disp & 255]);
            str[cnt] = (buf[disp & 255] < ' ' ?
            '.' : p_toascii(buf[disp & 255], 0));
        }
        printf("  ;%s\n", str);
    }

    /* Find next sector for the file being traced.  */
    if (buf[0] && buf[1]) {
        track = buf[0];
        sector = buf[1];
    } else {
        if (vdrive_check_track_sector(floppy->ImageFormat, track,
            ++sector) < 0) {
            sector = 0;
            if (++track > floppy->NumTracks)
                track = floppy->Dir_Track;
        }
    }
    vdrive_close(floppy, channel);
    return FD_OK;
}

static int create_cmd(int nargs, char **args)
{
    DRIVE *floppy = drives[drive_number];
    DiskFormats *format;
    char tmp[256];
    FILE *fsfd;
    int len, blk, errblk;

    blk = 0;
    errblk = 0;

    /* Open image or create a new one.  If the file exists, it must have
       valid header.  */
    if (open_image(drive_number, args[1], 1, DISK_IMAGE_TYPE_X64) < 0)
	return FD_BADIMAGE;

    if ((fsfd = fopen(args[2], MODE_READ)) == NULL) {
	fprintf(stderr, "Cannot open `%s'.\n", args[2]);
	perror(args[2]);
	return FD_NOTRD;
    }

    set_label(floppy->ActiveFd, "*** Truncated image."); /* Notify of errors */

    /* First copy all available blocks and then check existence of the Error
       Data Block.  */

    printf("Copying blocks.\n");
    fseek(floppy->ActiveFd, HEADER_LENGTH, SEEK_SET);

    while ((len = fread(tmp, 1, 256, fsfd)) == 256) {
	if (++blk > MAX_BLOCKS_ANY) {
	    fprintf(stderr, "\nNice try.\n");
	    break;
	}
	if (fwrite(tmp, 256, 1, floppy->ActiveFd) < 1) {
	    fprintf(stderr, "Cannot write block %d of `%s'.\n", blk, args[2]);
	    return FD_WRTERR;
	}
    }

    /* Now recognize the format and verify block count on it. */

    if (blk < NUM_BLOCKS_1541) {
	fprintf(stderr, "Cannot read block %d of `%s'.\n", blk, args[2]);
	return FD_NOTRD;
    }
    for (format = Legal_formats; format->ImageFormat >= 0; ++format) {
	if (blk == format->TotalBks) {
	    errblk = 0;
	    break;
	}
	if (blk == (format->TotalBks + (format->TotalBks >> 8))) {
	    errblk = 1;
	    break;
	}
    }

    if (format->ImageFormat < 0)
	return FD_BADIMAGE;

    /* Check and write the last (short) sector of error bytes */

    if (len) {
	if (len != (format->TotalBks % 256)) {
	    fprintf(stderr, "Cannot read block %d of `%s'.\n", blk, args[2]);
	    return FD_NOTRD;
	}
	if (fwrite(tmp, len, 1, floppy->ActiveFd) < 1) {
	    fprintf(stderr, "Cannot write block %d of `%s'.\n", blk, args[2]);
	    return FD_WRTERR;
	}
    }
    /* Update Format and Label information on Disk Header */

    fseek(floppy->ActiveFd, (off_t) HEADER_LABEL_OFFSET + 0, SEEK_SET);

    if (fwrite(&(format->ImageFormat), 1, 1, floppy->ActiveFd) < 1)
	return FD_WRTERR;

    set_disk_size(floppy->ActiveFd, format->TracksSide, format->Sides, errblk);

    set_label(floppy->ActiveFd, (args[3] ? args[3] : NULL));	/* Fix the note */

    fclose(fsfd);

    vdrive_command_execute(floppy, (BYTE *) "I", 1);

    return FD_OK;
}

static int copy_cmd(int nargs, char **args)
{
    char *p;
    char *dest_name_ascii, *dest_name_petscii;
    int dest_unit, src_unit;
    int i;

    p = extract_unit_from_file_name(args[nargs - 1], &dest_unit);
    if (p == NULL) {
        if (nargs > 3) {
            fprintf(stderr, "The destination must be a drive if multiple sources are specified.\n");
            return FD_OK;           /* FIXME */
        }
        dest_name_ascii = stralloc(args[nargs - 1]);
        dest_name_petscii = stralloc(dest_name_ascii);
        petconvstring(dest_name_petscii, 0);
        dest_unit = drive_number;
    } else {
        if (*p != 0) {
            if (nargs > 3) {
                fprintf(stderr, "The destination must be a drive if multiple sources are specified.\n");
                return FD_OK;           /* FIXME */
            }
            dest_name_ascii = stralloc(p);
            dest_name_petscii = stralloc(dest_name_ascii);
            petconvstring(dest_name_petscii, 0);
        } else {
            dest_name_ascii = dest_name_petscii = NULL;
        }
    }

    if (dest_name_ascii != NULL && !is_valid_cbm_file_name(dest_name_ascii)) {
        fprintf(stderr, "`%s' is not a valid CBM DOS file name.\n", dest_name_ascii);
        return FD_OK;               /* FIXME */
    }

    if (check_drive(dest_unit, CHK_RDY) < 0)
	return FD_NOTREADY;

    for (i = 1; i < nargs - 1; i++) {
        char *src_name_ascii, *src_name_petscii;

        p = extract_unit_from_file_name(args[i], &src_unit);

        if (p == NULL) {
            src_name_ascii = stralloc(args[i]);
            src_unit = drive_number;
        } else {
            if (check_drive(src_unit, CHK_RDY) < 0)
                return FD_NOTREADY;
            src_name_ascii = stralloc(p);
        }

        if (!is_valid_cbm_file_name(src_name_ascii)) {
            fprintf(stderr, "`%s' is not a valid CBM DOS file name: ignored.\n",
                   src_name_ascii);
            free(src_name_ascii);
            continue;
        }

        src_name_petscii = stralloc(src_name_ascii);
	petconvstring(src_name_petscii, 0);

	if (vdrive_open(drives[src_unit],
                        src_name_petscii, strlen(src_name_petscii), 0)) {
	    fprintf(stderr, "Cannot read `%s'.\n", src_name_ascii);
            if (dest_name_ascii != NULL)
                free(dest_name_ascii), free(dest_name_petscii);
            free(src_name_ascii), free(src_name_petscii);
	    return FD_RDERR;
	}

        if (dest_name_ascii != NULL) {
            if (vdrive_open(drives[dest_unit],
                            dest_name_petscii, strlen(dest_name_petscii), 1)) {
                fprintf(stderr, "Cannot write `%s'.\n", dest_name_petscii);
                vdrive_close(drives[src_unit], 0);
                free(dest_name_ascii), free(dest_name_petscii);
                free(src_name_ascii), free(src_name_petscii);
                return FD_OK;
            }
        } else {
            if (vdrive_open(drives[dest_unit],
                            src_name_petscii, strlen(src_name_petscii), 1)) {
                fprintf(stderr, "Cannot write `%s'.\n", src_name_petscii);
                vdrive_close(drives[src_unit], 0);
                free(src_name_ascii), free(src_name_petscii);
                return FD_OK;
            }
        }

	printf("Copying `%s'...\n", args[i]); /* FIXME */

        {
            BYTE c;

            while (!vdrive_read(drives[src_unit], (BYTE *) &c, 0)) {
                if (vdrive_write(drives[dest_unit], c, 1)) {
                    fprintf(stderr, "No space on image ?\n");
                    break;
                }
            }
        }

	vdrive_close(drives[src_unit], 0);
        vdrive_close(drives[dest_unit], 1);

        free(src_name_ascii);
        free(src_name_petscii);
    }

    free(dest_name_ascii);
    free(dest_name_petscii);
    return FD_OK;
}

static int delete_cmd(int nargs, char **args)
{
    int i = 1;

    if (check_drive(drive_number, CHK_RDY) < 0)
        return FD_NOTREADY;

    for (i = 1; i < nargs; i++) {
        int unit;
        char *p, *name;
        char *command;

        p = extract_unit_from_file_name(args[i], &unit);
        if (p == NULL) {
            unit = drive_number;
            name = args[i];
        } else {
            name = p;
        }

        if (!is_valid_cbm_file_name(name)) {
            fprintf(stderr, "`%s' is not a valid CBM DOS file name: ignored.\n", name);
            continue;
        }

        command = concat("s:", name, NULL);
        petconvstring(command, 0);

        printf("Deleting `%s' on unit %d.\n", name, unit + 8);
        vdrive_command_execute(drives[unit], (BYTE *)command, strlen(command));
        free(command);
    }

    return FD_OK;
}

/* Extract all files (gwesp@cosy.sbg.ac.at).  */
/* FIXME: This does not work with non-standard file names.  */

static int extract_cmd(int nargs, char **args)
{
    int drive = 8, track, sector;
    DRIVE *floppy;
    BYTE *buf, str[20];
    int err;
    int channel = 2;

    if ((err = check_drive(drive, CHK_RDY)) < 0)
        return err;
    floppy = drives[drive & 3];

    if (vdrive_open(floppy, "#", 1, channel)) {
        fprintf(stderr, "Cannot open buffer #%d in unit %d.\n", channel,
                drive + 8);
        return FD_RDERR;
    }

    track = floppy->Dir_Track;
    sector = floppy->Dir_Sector;

    while (1) {
        int i;

        sprintf((char *) str, "B-R:%d 0 %d %d", channel, track, sector);
        if (vdrive_command_execute(floppy, (BYTE *) str, strlen((char *) str)))
            return FD_RDERR;

        buf = floppy->buffers[channel].buffer;

        for (i = 0; i < 256; i += 32) {
            BYTE file_type = buf[i + SLOT_TYPE_OFFSET];

            if (((file_type & 7) == FT_SEQ
                || (file_type & 7) == FT_PRG
                || (file_type & 7) == FT_USR)
                && (file_type & FT_CLOSED)) {
                int len;
                BYTE *file_name = buf + i + SLOT_NAME_OFFSET;
                BYTE c, name[17], cbm_name[17];
                FILE *fd;

                memset(name, 0, 17);
                memset(cbm_name, 0, 17);
                for (len = 0; len < 16; len++) {
                    if (file_name[len] == 0xa0) {
                        break;
                    } else {
                        name[len] = file_name[len];
                        cbm_name[len] = file_name[len];
                    }
                }

                petconvstring((char *) name, 1);
                printf("%s\n", name);
                unix_filename((char *) name); /* For now, convert '/' to '_'. */

                if (vdrive_open(floppy, (char *) cbm_name, len, 0)) {
                    fprintf(stderr,
                            "Cannot open `%s' on unit %d.\n", name, drive + 8);
                    continue;
                }
                fd = fopen((char *) name, "wb");
                if (fd == NULL) {
                    fprintf(stderr, "Cannot create file `%s': %s.",
                    name, strerror(errno));
                    vdrive_close(floppy, 0);
                    continue;
                }
                while (!vdrive_read(floppy, &c, 0))
                    fputc(c, fd);

                vdrive_close(floppy, 0);

                if (fclose(fd)) {
                    perror("fclose");
                    return FD_RDERR;
                }
            }
        }
        if (buf[0] && buf[1]) {
            track = buf[0];
            sector = buf[1];
        } else {
            break;
        }
    }
    vdrive_close(floppy, channel);
    return FD_OK;
}

static int format_cmd(int nargs, char **args)
{
    char *command;
    int disk_type;
    int unit = -1;

    switch (nargs) {
      case 2:
        /* format <diskname,id> */
        unit = drive_number;
        break;
      case 3:
        /* format <diskname,id> <unit> */
        /* Format the disk image in unit <unit>.  */
        if (arg_to_int(args[2], &unit) >= 0
            && check_drive(unit, CHK_NUM) >= 0)
            /* It's a valid unit number.  */
            unit -= 8;
        else
            return FD_BADDEV;
        break;
      case 4:
        /* format <diskname,id> <type> <imagename> */
        /* Create a new image.  */
        /* FIXME: I want a unit number here too.  */
        *args[2] = tolower(*args[2]);
        if (strcmp(args[2], "x64") == 0)
            disk_type = DISK_IMAGE_TYPE_X64;
        else if (strcmp(args[2], "d64") == 0)
            disk_type = DISK_IMAGE_TYPE_D64;
        else if (strcmp(args[2], "d71") == 0)
            disk_type = DISK_IMAGE_TYPE_D71;
        else if (strcmp(args[2], "d81") == 0)
            disk_type = DISK_IMAGE_TYPE_D81;
        else if (strcmp(args[2], "d80") == 0)
            disk_type = DISK_IMAGE_TYPE_D80;
        else if (strcmp(args[2], "d82") == 0)
            disk_type = DISK_IMAGE_TYPE_D82;
        else
            return FD_BADVAL;
        if (open_image(drive_number, args[3], 1, disk_type) < 0)
            return FD_BADIMAGE;
        unit = 0;
        break;
      default:
        /* Shouldn't happen.  */
        return FD_BADVAL;
    }

    if (!strchr(args[1], ',')) {
	fprintf(stderr, "There must be ID on the name.\n");
	return FD_OK;
    }

    if (check_drive(unit, CHK_RDY) < 0)
        return FD_NOTREADY;

    command = concat("n:", args[1], NULL);
    petconvstring(command, 0);

    printf("Formatting in unit %d...\n", unit + 8);
    vdrive_command_execute(drives[unit], (BYTE *) command, strlen(command));

    free(command);
    return FD_OK;
}

static int gcrformat_cmd(int nargs, char **args)
{
    FILE *fd;
    int track, sector;
    BYTE gcr_header[12], id[2];
    char name[16], *idptr;
    DWORD gcr_track_p[MAX_TRACKS_1541 * 2];
    DWORD gcr_speed_p[MAX_TRACKS_1541 * 2];
    BYTE gcr_track[7930], rawdata[260];
    BYTE *gcrptr;
    char newname[256];

    if (nargs < 2)
	return FD_OK;

    memset(name, 0xa0, 16);
    id[0] = id[1] = 0xa0;

    if (nargs > 2) {
	strcpy(newname, args[1]);
	petconvstring(newname, 0);
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
	    strncpy((char *) id, ++idptr, 2);
	    if (idptr[1] != '\0') {
		id[0] = idptr[1];
		if (idptr[2] != '\0')
		    id[1] = idptr[2];
	    }
	}
    }

    if ((fd = fopen(args[2], MODE_READ_WRITE /*, 0666*/)) == NULL) {
	fprintf(stderr, "Cannot create image `%s': %s.\n", args[2], strerror(errno));
	return FD_BADIMAGE;
    }
    strcpy((char *) gcr_header, "GCR-1541");

    gcr_header[8] = 0;
    gcr_header[9] = MAX_TRACKS_1541 * 2;
    gcr_header[10] = 7928 % 256;
    gcr_header[11] = 7928 / 256;

    if (fwrite((char *) gcr_header, sizeof(gcr_header), 1, fd) < 1) {
        fprintf(stderr, "Cannot write header.\n");
        fclose(fd);
        return FD_OK;
    }
    for (track = 0; track < MAX_TRACKS_1541; track++) {
	gcr_track_p[track * 2] = 12 + MAX_TRACKS_1541 * 16 + track * 7930;
	gcr_track_p[track * 2 + 1] = 0;
	gcr_speed_p[track * 2] = speed_map_1541[track];
	gcr_speed_p[track * 2 + 1] = 0;
    }

    if (write_dword(fd, gcr_track_p, sizeof(gcr_track_p)) < 0) {
	fprintf(stderr, "Cannot write track header.\n");
	fclose(fd);
	return FD_OK;
    }
    if (write_dword(fd, gcr_speed_p, sizeof(gcr_speed_p)) < 0) {
	fprintf(stderr, "Cannot write speed header.\n");
	fclose(fd);
	return FD_OK;
    }
    for (track = 0; track < MAX_TRACKS_1541; track++) {
	int raw_track_size[4] = { 6250, 6666, 7142, 7692 };

	memset(&gcr_track[2], 0xff, 7928);
	gcr_track[0] = raw_track_size[speed_map_1541[track]] % 256;
	gcr_track[1] = raw_track_size[speed_map_1541[track]] / 256;
	gcrptr = &gcr_track[2];

	for (sector = 0; sector < sector_map_1541[track + 1]; sector++) {
	    BYTE chksum;
	    int i;
	    memset(rawdata, 0, 260);
	    if (track == 17 && sector == 0) {
		BYTE *rdat = &rawdata[1];
		int s, t;
		memset(rdat + BAM_NAME_1541, 0xa0, 27);
		rdat[0] = DIR_TRACK_1541;
		rdat[1] = DIR_SECTOR_1541;
		rdat[2] = 65;
		rdat[BAM_VERSION_1541] = 50;
		rdat[BAM_VERSION_1541 + 1] = 65;
		memcpy(rdat + BAM_NAME_1541, (BYTE *) name, 16);
		rdat[BAM_ID_1541] = id[0];
		rdat[BAM_ID_1541 + 1] = id[1];
		for (t = 1; t <= 35; t++)
		    for (s = 0; s < sector_map_1541[t]; s++)
			vdrive_bam_free_sector(1541, rdat, t, s);
		vdrive_bam_allocate_sector(1541, rdat, BAM_TRACK_1541, 0);
		vdrive_bam_allocate_sector(1541, rdat, BAM_TRACK_1541, 1);
	    }
	    rawdata[0] = 7;
	    chksum = rawdata[1];
	    for (i = 1; i < 256; i++)
		chksum ^= rawdata[i + 1];
	    rawdata[257] = chksum;

	    convert_sector_to_GCR(rawdata, gcrptr, track + 1, sector, id[0], id[1]);
	    gcrptr += 360;
	}

	if (fwrite((char *) gcr_track, sizeof(gcr_track), 1, fd) < 1 ) {
	    fprintf(stderr, "Cannot write track data.\n");
	    fclose(fd);
	    return FD_OK;
	}
    }

    fclose(fd);
    return FD_OK;
}

static int help_cmd(int nargs, char **args)
{
    if (nargs == 1) {
        int i;

        pager_init();
        pager_print("Available commands are:");
        for (i = 0; command_list[i].name != NULL; i++) {
            pager_print("\n  ");
            pager_print(command_list[i].syntax);
        }
        pager_print("\n");
    } else {
        int match;

        match = lookup_command(args[1]);
        switch (match) {
          case LOOKUP_AMBIGUOUS:
            fprintf(stderr, "Command `%s' is ambiguous.\n", args[1]);
            break;
          case LOOKUP_NOTFOUND:
            fprintf(stderr, "Unknown command `%s'.\n", args[1]);
            break;
          default:
            if (LOOKUP_SUCCESSFUL(match)) {
                printf("Syntax: %s\n%s\n",
                       command_list[match].syntax,
                       command_list[match].description);
            }
        }
    }

    return FD_OK;
}

static int info_cmd(int nargs, char **args)
{
    DRIVE *floppy;
    hdrinfo hdr;
    int err;
    int unit;

    if (nargs == 2) {
        if (arg_to_int(args[1], &unit) < 0)
            return FD_BADDEV;
        if (check_drive(unit, CHK_NUM) < 0)
            return FD_BADDEV;
        unit -= 8;
    } else {
        unit = drive_number;
    }

    if (check_drive(unit, CHK_RDY) < 0)
	return FD_NOTREADY;

    floppy = drives[drive_number];
    if ((err = check_header(floppy->ActiveFd, &hdr)) < 0)
	return err;

    printf("Description: %s\n",
           (*hdr.description ? hdr.description : "None."));
    printf("Drive Type : %d.\n", floppy->ImageFormat);	/* Compatible drive */
    printf("Disk Format: %c.\n", hdr.format);
    printf("Sides\t   : %d.\n", hdr.sides);
    printf("Tracks\t   : %d.\n", hdr.tracks);
    printf((hdr.errblk ? "Error Block present.\n" : "No Error Block.\n"));
    printf("Write protect: %s.\n", hdr.wprot ? "On" : "Off");

    return FD_OK;
}

static int list_cmd(int nargs, char **args)
{
    char *listing;
    char *pattern;
    int unit;

    if (nargs > 1) {
        /* list <pattern> */
        pattern = extract_unit_from_file_name(args[1], &unit);
        if (pattern == NULL)
            unit = drive_number;
        else if (*pattern == 0)
            pattern = NULL;
    } else {
        /* list */
        pattern = NULL;
        unit = drive_number;
    }

    if (check_drive(unit, CHK_RDY) < 0)
        return FD_NOTREADY;

    listing = floppy_read_directory(drives[unit], pattern);
    if (listing != NULL) {
        pager_init();
        pager_print(listing);
	free(listing);
    }

    return FD_OK;
}

static int name_cmd(int nargs, char **args)
{
    char *id;
    char *name;
    BYTE *dst;
    int i;
    int unit;
    DRIVE *floppy;

    if (nargs > 2) {
        if (arg_to_int(args[2], &unit) < 0)
            return FD_BADDEV;
        if (check_drive(unit, CHK_NUM) < 0)
            return FD_BADDEV;
        unit -= 8;
    } else {
        unit = drive_number;
    }

    if (check_drive(unit, CHK_RDY) < 0)
       return FD_NOTREADY;

    floppy = drives[unit];
    vdrive_bam_read_bam(floppy);
    name = args[1];
    petconvstring(name, 0);
    id = strrchr(args[1], ',');
    if (id)
       *id++ = '\0';

    dst = &floppy->bam[floppy->bam_name];
    for (i = 0; i < 16; i++)
       *dst++ = *name ? *name++ : 0xa0;

    if (id) {
       dst = &floppy->bam[floppy->bam_id];
       for (i = 0; i < 5 && *id; i++)
           *dst++ = *id++;
    }

    vdrive_bam_write_bam(floppy);
    return FD_OK;
}

static int quit_cmd(int nargs, char **args)
{
    int i;

    for (i = 0; i <= MAXDRIVE; i++)
	if (drives[i] != NULL)
	    detach_floppy_image(drives[i]);

    exit(0);

    return 0; 	/* OSF1 cc complains */
}

static int read_cmd(int nargs, char **args)
{
    char *src_name_petscii, *src_name_ascii;
    char *dest_name_ascii;
    char *actual_name;
    char *p;
    int unit;
    int is_p00;
    FILE *outf;

    p = extract_unit_from_file_name(args[1], &unit);
    if (p == NULL)
        unit = drive_number;

    if (check_drive(unit, CHK_RDY) < 0)
	return FD_NOTREADY;

    if (p == NULL)
        src_name_ascii = stralloc(args[1]);
    else
        src_name_ascii = stralloc(p);

    if (!is_valid_cbm_file_name(src_name_ascii)) {
        fprintf(stderr, "`%s' is not a valid CBM DOS file name.\n", src_name_ascii);
        free(src_name_ascii);
        return FD_OK;               /* FIXME */
    }

    src_name_petscii = stralloc(src_name_ascii);
    petconvstring(src_name_petscii, 0);

    if (vdrive_open(drives[unit],
                    src_name_petscii, strlen(src_name_petscii), 0)) {
	fprintf(stderr, "Cannot read `%s' on unit %d.\n", src_name_ascii, unit + 8);
        free(src_name_ascii), free(src_name_petscii);
	return FD_BADNAME;
    }

    /* Get real filename from the disk file.  Slot must be defined by
       vdrive_open().  */
    actual_name = xmalloc(17);  /* FIXME: Should be a #define.  */
    memcpy(actual_name, drives[unit]->buffers[0].slot + SLOT_NAME_OFFSET, 16);
    actual_name[16] = 0;

    if (nargs == 3) {
        if (strcmp(args[2], "-") == 0) {
            dest_name_ascii = NULL;      /* stdout */
            is_p00 = 0;
        } else {
            dest_name_ascii = args[2];
            is_p00 = (p00_check_name(args[2]) >= 0);
        }
    } else {
	int l;

	dest_name_ascii = actual_name;
	vdrive_dir_no_a0_pads((BYTE *) dest_name_ascii, 16);
	l = strlen(dest_name_ascii) - 1;
	while (dest_name_ascii[l] == ' ') {
	    dest_name_ascii[l] = 0;
	    l--;
	}
	petconvstring(dest_name_ascii, 1);
        is_p00 = 0;
    }

    if (dest_name_ascii == NULL)
	outf = stdout;
    else {
        outf = fopen(dest_name_ascii, MODE_WRITE);
	if (outf == NULL) {
	    fprintf(stderr, "Cannot create output file `%s': %s.\n",
                   dest_name_ascii, strerror(errno));
	    vdrive_close(drives[unit], 0);
            free(src_name_petscii), free(src_name_ascii), free(actual_name);
	    return FD_NOTWRT;
	}
        if (is_p00) {
	    if (p00_write_header(outf, (BYTE *)dest_name_ascii, 0) < 0)
                fprintf(stderr, "Cannot write P00 header.\n");
            else
                printf("Written P00 header.\n");
	}
    }				/* stdout */

    printf("Reading file `%s' from unit %d.\n", src_name_ascii, unit + 8);

    /* Copy.  */
    {
        BYTE c;

        while (!vdrive_read(drives[unit], (BYTE *) & c, 0))
            fputc(c, outf);
    }

    if (outf != stdout)
        fclose(outf);
    vdrive_close(drives[unit], 0);

    free(src_name_petscii), free(src_name_ascii), free(actual_name);

    return FD_OK;
}

static int rename_cmd(int nargs, char **args)
{
    char *src_name, *dest_name;
    int src_unit, dest_unit;
    char *command;
    char *p;

    p = extract_unit_from_file_name(args[1], &src_unit);
    if (p == NULL) {
        src_unit = drive_number;
        src_name = stralloc(args[1]);
    } else {
        src_name = stralloc(p);
    }

    p = extract_unit_from_file_name(args[2], &dest_unit);
    if (p == NULL) {
        dest_unit = drive_number;
        dest_name = stralloc(args[2]);
    } else {
        dest_name = stralloc(p);
    }

    if (dest_unit != src_unit) {
        fprintf(stderr, "Source and destination must be on the same unit.\n");
        free(src_name), free(dest_name);
        return FD_OK;               /* FIXME */
    }

    if (check_drive(dest_unit, CHK_RDY) < 0) {
        free(src_name), free(dest_name);
	return FD_NOTREADY;
    }

    if (!is_valid_cbm_file_name(src_name)) {
        fprintf(stderr, "`%s' is not a valid CBM DOS file name.\n", src_name);
        free(src_name), free(dest_name);
        return FD_OK;               /* FIXME */
    }

    if (!is_valid_cbm_file_name(dest_name)) {
        fprintf(stderr, "`%s' is not a valid CBM DOS file name.\n", dest_name);
        free(src_name), free(dest_name);
        return FD_OK;               /* FIXME */
    }

    printf("Renaming `%s' to `%s'\n", src_name, dest_name);

    command = concat("r:", dest_name, "=", src_name, NULL);
    petconvstring(command, 0);

    vdrive_command_execute(drives[dest_unit],
                           (BYTE *) command, strlen(command));

    free(command), free(dest_name), free(src_name);

    return FD_OK;
}

static int show_cmd(int nargs, char **args)
{
    const char *text;

    if (strcasecmp(args[1], "copying") == 0) {
        text = license_text;
    } else if (strcasecmp(args[1], "warranty") == 0) {
        text = warranty_text;
    } else {
        fprintf(stderr, "Use either `show copying' or `show warranty'.\n");
        return FD_OK;           /* FIXME? */
    }

    pager_init();
    pager_print(text);

    return FD_OK;
}

/* Copy files from a T64 tape image.  */
static int tape_cmd(int nargs, char **args)
{
    t64_t *t64;
    DRIVE *drive;
    int count;

    if (check_drive(drive_number, CHK_RDY) < 0)
        return FD_NOTREADY;
    drive = drives[drive_number];

    t64 = t64_open(args[1]);
    if (t64 == NULL) {
        fprintf(stderr, "Cannot read T64 file `%s'.\n", args[1]);
        return FD_BADNAME;
    }

    for (count = 0; t64_seek_to_next_file(t64, 0) >= 0;) {
        t64_file_record_t *rec;

        rec = t64_get_current_file_record(t64);

	if (rec->entry_type == T64_FILE_RECORD_NORMAL) {
            char *dest_name_ascii;
            char *dest_name_petscii;
            BYTE *buf;
            int name_len;
            WORD file_size;
            int retval;

            /* Ignore traling spaces and 0xa0's.  */
            name_len = T64_REC_CBMNAME_LEN;
            while (name_len > 0 && (rec->cbm_name[name_len - 1] == 0xa0
                                    || rec->cbm_name[name_len - 1] == 0x20))
                name_len--;

            dest_name_petscii = xmalloc(name_len + 1);
            memcpy(dest_name_petscii, rec->cbm_name, name_len);
            dest_name_petscii[name_len] = 0;

            dest_name_ascii = xmalloc(name_len + 1);
            memcpy(dest_name_ascii, dest_name_petscii, name_len);
            dest_name_ascii[name_len] = 0;
            petconvstring(dest_name_ascii, 1);

            if (nargs > 2) {
                int i, found;

                for (i = 2, found = 0; i < nargs; i++)
                    if (name_len == strlen(args[i])
                        && memcmp(args[i], dest_name_ascii, name_len) == 0) {
                        found = 1;
                        break;
                    }

                if (!found)
                    continue;
            }

            /* FIXME: This does not write the actual file type.  */
	    if (vdrive_open(drive, dest_name_petscii, name_len, 1)) {
		fprintf(stderr, "Cannot open `%s' for writing on drive %d.\n",
                       dest_name_ascii, drive_number + 8);
                free(dest_name_petscii), free(dest_name_ascii);
                continue;
	    }

	    fprintf(stderr, "Writing `%s' ($%04X - $%04X) to drive %d.\n",
                   dest_name_ascii, rec->start_addr, rec->end_addr,
                   drive_number + 8);

	    vdrive_write(drive, rec->start_addr & 0xff, 1);
	    vdrive_write(drive, rec->start_addr >> 8, 1);

            file_size = rec->end_addr - rec->start_addr;
            buf = alloca((unsigned int) file_size);
            memset(buf, 0, (size_t) file_size);
            retval = t64_read(t64, buf, file_size);
            if (retval < 0 || retval != (int) file_size)
                fprintf(stderr, "Unexpected end of tape: file may be truncated.\n");

            {
                int i;

                for (i = 0; i < file_size; i++)
                    if (vdrive_write(drives[drive_number], buf[i], 1)) {
                        t64_close(t64);
                        free(dest_name_petscii), free(dest_name_ascii);
                        return FD_WRTERR;
                    }
            }

	    vdrive_close(drive, 1);
            free(dest_name_petscii), free(dest_name_ascii);

            count++;
	}
    }

    t64_close(t64);

    printf("\n%d files copied.\n", count);

    return FD_OK;
}

static int unit_cmd(int nargs, char **args)
{
    int dev;

    if (arg_to_int(args[1], &dev) < 0 || check_drive(dev, CHK_NUM) < 0)
	return FD_BADDEV;

    drive_number = dev & 3;
    return FD_OK;
}

/* Lynx support added by Riccardo Ferreira (storm@esoterica.pt) --
   1998-02-07.  Various fixes by Andreas Boose.  */
static int unlynx_cmd(int nargs, char **args)
{
    DRIVE *floppy;
    FILE *f, *f2;
    int dev, cnt = 0;
    long dentries, lbsize, bsize, dirsize;
    BYTE val;
    char buff[256] = {0}, cname[20] = {0}, ftype;

    if (nargs < 3)
        dev = drive_number;
    else {
        if (arg_to_int(args[2], &dev) < 0)
            return FD_BADDEV;
        if (check_drive(dev, CHK_NUM) < 0)
            return FD_BADDEV;
        dev -= 8;
    }

    if (check_drive(dev, CHK_RDY) < 0)
        return FD_NOTREADY;

    if (!(f = fopen(args[1], MODE_READ))) {
	fprintf(stderr, "Cannot open `%s' for reading.\n", args[1]);
	return FD_NOTRD;
    }

    /* Look for the 0, 0, 0 sign of the end of BASIC.  */
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
    if (string_to_long(buff, NULL, 10, &dirsize) < 0 || dirsize <= 0) {
	fprintf(stderr, "Invalid Lynx file.\n");
	return FD_RDERR;
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
    if (string_to_long(buff, NULL, 10, &dentries) < 0 || dentries <= 0) {
	fprintf(stderr, "Invalid Lynx file.\n");
	return FD_RDERR;
    }

    /* Open the file for reading of the chained data */
    f2 = fopen(args[1], MODE_READ);
    fseek(f2, (dirsize * 254), SEEK_SET);

    /* Loop */
    while (dentries != 0) {
        int filetype = FT_PRG;

	/* Read CBM filename */
	cnt = 0;
	while (1) {
	    fread(&val, 1, 1, f);
	    if (val != 13)
		cname[cnt++] = val;
	    else
		break;
	}
	cname[cnt] = 0;

	/* Read the block size */
	cnt = 0;
	while (1) {
	    fread(&val, 1, 1, f);
	    if (val != 13)
		buff[cnt++] = val;
	    else
		break;
	}
	buff[cnt] = 0;

	if (string_to_long(buff, NULL, 10, &bsize) < 0) {
	    fprintf(stderr, "Invalid Lynx file.\n");
	    return FD_RDERR;
	}
	/* Get the file type (P[RG], S[EQ], R[EL], U[SR]) */
	ftype = fgetc(f);
	fgetc(f);

        switch (ftype) {
          case 'D':
            filetype = FT_DEL;
            break;
          case 'P':
            filetype = FT_PRG;
            break;
          case 'S':
            filetype = FT_SEQ;
            break;
          case 'U':
            filetype = FT_USR;
            break;
          case 'R':
            fprintf(stderr, "REL not supported.\n");
            return FD_RDERR;
        }

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
	    fprintf(stderr, "Invalid Lynx file.\n");
	    return FD_RDERR;
	}
        /* Calculate byte size of file */
        cnt = (bsize - 1) * 254 + lbsize - 1;

	printf("Writing file '%s' to image.\n", cname);

	floppy = drives[dev & 3];

	/* We cannot use normal vdrive_open as it does not allow to
	   create invalid file names.  */
	floppy->buffers[1].readmode = FAM_WRITE;
	vdrive_open_create_dir_slot(&(floppy->buffers[1]), cname,
				    strlen(cname), filetype);

	while (cnt != 0) {
	    fread(&val, 1, 1, f2);
	    if (vdrive_write(floppy, val, 1)) {
		fprintf(stderr, "No space on image ?\n");
		break;
	    }
	    cnt--;
	}
	vdrive_close(floppy, 1);

	/* Adjust for the last block */
        if (lbsize < 255)
            fread(buff, 1, 254 + 1 - lbsize, f2);
	dentries--;
    }
    fclose(f);
    fclose(f2);

    return FD_OK;
}

static int validate_cmd(int nargs, char **args)
{
    int unit;

    switch (nargs) {
      case 1:
        /* validate */
        unit = drive_number;
        break;
      case 2:
        /* validate <unit> */
	if (arg_to_int(args[1], &unit) < 0)
	    return FD_BADDEV;
        unit -= 8;
        break;
      default:
        return FD_BADVAL;
    }

    if (check_drive(unit, CHK_RDY) < 0)
	return FD_NOTREADY;

    printf("Validating in unit %d...\n", unit + 8);
    vdrive_command_validate(drives[unit]);

    return FD_OK;
}

static int write_cmd(int nargs, char **args)
{
    int unit;
    char *dest_name_ascii, *dest_name_petscii;
    char *p;
    FILE *f;

    if (nargs == 3) {
        /* write <source> <dest> */
        p = extract_unit_from_file_name(args[2], &unit);
        if (p == NULL) {
            unit = drive_number;
            dest_name_ascii = stralloc(args[2]);
        } else {
            if (*p != 0)
                dest_name_ascii = stralloc(args[2]);
            else
                dest_name_ascii = NULL;
        }
    } else {
        /* write <source> */
        dest_name_ascii = NULL;
        unit = drive_number;
    }

    if (check_drive(unit, CHK_RDY) < 0)
	return FD_NOTREADY;

    f = fopen(args[1], MODE_READ);
    if (f == NULL) {
	fprintf(stderr, "Cannot read file `%s': %s.\n", args[1], strerror(errno));
	return FD_NOTRD;
    }

    if (dest_name_ascii == NULL) {
        char realname[17];      /* FIXME: Should be a #define.  */
        unsigned int reclen;    /* And we don't really need this stuff!  */

        /* User did not specify a destination name...  Let's try to make an
           educated guess at what she expects.  */
        /* FIXME: We should create files according to the P00 file type.  */
        if (p00_check_name(args[1]) >= 0
            && p00_read_header(f, (BYTE *)realname, &reclen) >= 0) {
            dest_name_petscii = stralloc(realname);
            dest_name_ascii = stralloc(dest_name_petscii);
            petconvstring(dest_name_ascii, 0);
        } else {
            char *slashp;

            rewind(f);		/* There is no P00 header.  */
            slashp = strrchr(args[1], '/');
            if (slashp == NULL)
                dest_name_ascii = stralloc(args[1]);
            else
                dest_name_ascii = stralloc(slashp + 1);
            dest_name_petscii = stralloc(dest_name_ascii);
            petconvstring(dest_name_petscii, 0);
        }
    } else {
        dest_name_petscii = stralloc(dest_name_ascii);
        petconvstring(dest_name_petscii, 0);
    }

    if (vdrive_open(drives[unit],
                    dest_name_petscii, strlen(dest_name_petscii), 1)) {
	fprintf(stderr, "Cannot open `%s' for writing on image.\n", dest_name_ascii);
	return FD_WRTERR;
    }

    printf("Writing file `%s' to unit %d.\n", dest_name_ascii, unit + 8);
    {
        int c;

        while (EOF != (c = fgetc(f))) {
            if (vdrive_write(drives[unit], (BYTE) c, 1)) {
                fprintf(stderr, "No space on image ?\n");
                break;
            }
        }
    }

    fclose(f);
    vdrive_close(drives[unit], 1);

    free(dest_name_ascii), free(dest_name_petscii);

    return FD_OK;
}

/* FIXME: 1541 only? */
static int zcreate_cmd(int nargs, char **args)
{
    DRIVE *floppy = drives[drive_number];
    DiskFormats *format = Legal_formats;
    char tmp[256];
    FILE *fsfd = NULL;
    int errblk = 0;
    int track, sector, count;
    char fname[MAXPATHLEN], dirname[MAXPATHLEN], oname[MAXPATHLEN];
    char *p;
    int channel = 2;
    BYTE str[20];
    static int drive = 8;
    int singlefilemode = 0;

    /* Open image or create a new one.  If the file exists, it must have
       valid header.  */
    if (open_image(drive_number, args[1], 1, DISK_IMAGE_TYPE_X64) < 0)
        return FD_BADIMAGE;

    p = strrchr(args[2], FSDEV_DIR_SEP_CHR);
    if (p == NULL) {
        /* ignore '[0-4]!' if found */
        if (args[2][0] >= '1' && args[2][0] <= '4' && args[2][1] == '!')
            strcpy(fname + 2, args[2] + 2);
        else
            strcpy(fname + 2, args[2]);
        fname[0] = '0';
        fname[1] = '!';
        strcpy(dirname, "");
    } else {
        int len_path;
        len_path = (int)(p - args[2]);
        if (len_path == strlen(args[2]) - 1)
            return FD_RDERR;
        strncpy(dirname, args[2], len_path + 1);
        dirname[len_path + 1] = '\0';

        /* ignore '[0-4]!' if found */
        if (args[2][len_path + 1] >= '1' && args[2][len_path + 1] <= '4' 
            && args[2][len_path + 1 + 1] == '!')
            strcpy(fname + 2, &(args[2][len_path + 1]) + 2);
        else
            strcpy(fname + 2, &(args[2][len_path + 1]));
        fname[0] = '0';
        fname[1] = '!';
    }
    set_label(floppy->ActiveFd, "*** Truncated image."); /* Notify of errors */

    printf("Copying blocks to image\n");
    fseek(floppy->ActiveFd, HEADER_LENGTH, SEEK_SET);

    /* Write out all the sectors */
    for (count = 0; count < format->TotalBks; count++) {
        if (fwrite(tmp, 1, 256, floppy->ActiveFd) != 256) {
            fprintf(stderr, "Cannot write block %d of `%s'\n", count, args[2]);
            return FD_WRTERR;
        }
    }

    if (vdrive_open(floppy, "#", 1, channel)) {
        fprintf(stderr, "Cannot open buffer #%d on unit %d.\n", channel,
                drive + 8);
        return FD_RDERR;
    }
    for (track = 1; track <= 35; track++) {
        if (singlefilemode || track == 1) {
            if (track == 1) {
                /* For now we disable one-file more, because it is not detected
                   correctly.  */
                strcpy(oname, dirname);
                strcat(oname, fname + 2);
                fsfd = fopen(oname, MODE_READ);
                if (fsfd != NULL) {
                    printf("Reading zipfile on one-file mode\n");
                    singlefilemode = 1;
                    fseek(fsfd, 4, SEEK_SET);
                }
            } else if (track == 9 || track == 17 || track == 26) {
                fseek(fsfd, 2, SEEK_CUR);
            }
        }
        if (!singlefilemode) {
            switch (track) {
              case 1:
              case 9:
              case 17:
              case 26:
                fname[0]++;
                if (fsfd != NULL)
                    fclose(fsfd);
                strcpy(oname, dirname);
                strcat(oname, fname);
                if ((fsfd = fopen(oname, MODE_READ)) == NULL) {
                    fprintf(stderr, "Cannot open `%s'.\n", fname);
                    perror(fname);
                    return FD_NOTRD;
                }
                fseek(fsfd, (track == 1) ? 4 : 2, SEEK_SET);
                break;
            }
        }
        for (count = 0; count < sector_map_1541[track]; count++) {
            if ((zipcode_read_sector(fsfd, track, &sector,
                (char *) (floppy->buffers[channel].buffer))) != 0) {
                fclose(fsfd);
                return FD_BADIMAGE;
            }
            /* Write one block */

            sprintf((char *) str, "B-W:%d 0 %d %d", channel, track, sector);
            if (vdrive_command_execute(floppy, (BYTE *) str,
                strlen((char *) str)) != 0) {
                track = DIR_TRACK_1541;
                sector = 0;
                fclose(fsfd);
                return FD_RDERR;
            }
        }
    }
    vdrive_close(floppy, channel);

    /* Update Format and Label information on Disk Header */
    fseek(floppy->ActiveFd, (off_t) HEADER_LABEL_OFFSET + 0, SEEK_SET);

    if (fwrite(&(format->ImageFormat), 1, 1, floppy->ActiveFd) < 1)
        return FD_WRTERR;

    set_disk_size(floppy->ActiveFd, format->TracksSide, format->Sides, errblk);
    set_label(floppy->ActiveFd, (args[3] ? args[3] : NULL)); /* Fix the note */
    fclose(fsfd);

    vdrive_command_execute(floppy, (BYTE *) "I", 1);
    return FD_OK;
}

static int raw_cmd(int nargs, char **args)
{
    DRIVE *floppy = drives[drive_number];

    /* Write to the command channel.  */
    if (nargs >= 2) {
        char *command = stralloc(args[1]);

	petconvstring(command, 0);
	vdrive_command_execute(floppy, (BYTE *) command, strlen(command));
        free(command);
    }

    /* Print the error now.  */
    puts((char *)floppy->buffers[15].buffer);
    return FD_OK;
}

/* ------------------------------------------------------------------------- */

int main(int argc, char **argv)
{
    char *args[MAXARG];
    int nargs;
    int i;
    int retval;

#if defined __MSDOS__ || defined WIN32
    /* Set the default file mode.  */
    _fmode = O_BINARY;
#endif

    /* This causes all the logging messages from the various VICE modules to
       appear on stdout.  */
    log_init_with_fd(stdout);

    for (i = 0; i < MAXARG; i++)
	args[i] = NULL;
    nargs = 0;

    initialize_1541(8, DT_DISK | DT_1541, NULL, NULL, NULL);
    initialize_1541(9, DT_DISK | DT_1541, NULL, NULL, NULL);

    retval = 0;

    /* The first arguments without leading `-' are interpreted as disk images
       to attach.  */
    for (i = 1; i < argc && *argv[i] != '-'; i++) {
        if (i - 1 > MAXDRIVE)
            fprintf(stderr, "Ignoring disk image `%s'.\n", argv[i]);
        else
            attach_floppy_image(drives[i - 1], argv[i], 0);
    }

    if (i == argc) {
        char *line;
	char buf[16];

        /* Interactive mode.  */
	printf("C1541 Version %d.%02d.\n",
               C1541_VERSION_MAJOR, C1541_VERSION_MINOR);
        printf("Copyright 1995-1999 The VICE Development Team.\n"
               "C1541 is free software, covered by the GNU General Public License, and you are\n"
               "welcome to change it and/or distribute copies of it under certain conditions.\n"
               "Type `show copying' to see the conditions.\n"
               "There is absolutely no warranty for C1541.  Type `show warranty' for details.\n");

	while (1) {
	    sprintf(buf, "c1541 #%d> ", drive_number | 8);
	    line = read_line(buf);

            if (line == NULL) {
                putchar('\n');
                fflush(stdout), fflush(stderr);
		break;
            }

            if (*line=='!') {
                retval = system(line + 1);
                printf("Exit code: %d.\n", retval);
            } else {
                split_args(line, &nargs, args);
                if (nargs > 0)
                    lookup_and_execute_command(nargs, args);
            }
	}
    } else {
        while (i < argc) {
            args[0] = argv[i] + 1;
            nargs = 1;
            i++;
            for (; i < argc && *argv[i] != '-'; i++)
                args[nargs++] = argv[i];
            if (lookup_and_execute_command(nargs, args) < 0) {
                retval = 1;
                break;
            }
        }
    }

    for (i = 0; i <= MAXDRIVE; i++) {
	if (drives[i])
	    detach_floppy_image(drives[i]);
    }

    return retval;
}

/* ------------------------------------------------------------------------- */

/* FIXME: Can we get rid of this stuff?  */

int attach_fsdevice(int device, char *var, const char *name)
{
    drives[device & 3] = (DRIVE *) var;
    return 0;
}

int serial_attach_device(int device, char *var, char const *name,
			 int (*getf) (void *, BYTE *, int),
			 int (*putf) (void *, BYTE, int),
			 int (*openf) (void *, const char *, int, int),
			 int (*closef) (void *, int),
			 void (*flushf) (void *, int))
{
    drives[device & 3] = (DRIVE *) var;
    return 0;
}

int drive_read_block(int track, int sector, BYTE *readdata, int dnr)
{
    /* We don't have support for G64 images.  */
    return -1;
}

int drive_write_block(int track, int sector, BYTE *readdata, int dnr)
{
    /* We don't have support for G64 images.  */
    return -1;
}

void enable_text(void)
{
}

void disable_text(void)
{
}

