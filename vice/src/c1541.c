/** \file   src/c1541.c
 * \brief   Stand-alone disk image maintenance program
 *
 *
 * \author  Ettore Perazzoli <ettore@comm2000.it>
 * \author  Teemu Rantanen <tvr@cs.hut.fi>
 * \author  Jouko Valta <jopi@zombie.oulu.fi>
 * \author  Gerhard Wesp <gwesp@cosy.sbg.ac.at>
 * \author  Daniel Sladic <sladic@eecg.toronto.edu>
 * \author  Ricardo Ferreira <storm@esoterica.pt>
 * \author  Andreas Boose <viceteam@t-online.de>
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Patches by
 *  Olaf Seibert <rhialto@falu.nl>
 *  Dirk Schnorpfeil <D.Schnorpfeil@web.de> (GEOS stuff)
 *
 * Zipcode implementation based on `zip2disk' by
 *  Paul David Doherty <h0142kdd@rz.hu-berlin.de>
 */

/*
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

#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <limits.h>
#include <string.h>

#include "attach.h"
#include "diskconstants.h"
#include "diskimage.h"
#include "fsimage.h"
#include "diskcontents-block.h"
#include "machine.h"

#include "version.h"

#include "vicedate.h"

#include "svnversion.h"

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

#include "archdep.h"
#include "cbmdos.h"
#include "cbmimage.h"
#include "charset.h"
#include "cmdline.h"
#include "drive.h"
#include "diskimage.h"
#include "fileio.h"
#include "fsimage-check.h"
#include "gcr.h"
#include "imagecontents.h"
#include "lib.h"
#include "log.h"
#include "serial.h"
#include "tape.h"
#include "util.h"
#include "vdrive-bam.h"
#include "vdrive-command.h"
#include "vdrive-dir.h"
#include "vdrive-iec.h"
#include "vdrive-rel.h"
#include "vdrive.h"
#include "zipcode.h"
#include "p64.h"
#include "fileio/p00.h"

/* we want the NG header, not the old one in arch/gtk3: */
#include "lib/linenoise-ng/linenoise.h"

#ifdef UNIX_COMPILE
#include <unistd.h>
#endif

/* #define DEBUG_DRIVE */

#define MAXARG          256 + 5 /**< maximum number arguments to a command,
                                     the +5 comes from the bpoke() command,
                                     that one uses 5 args for its name,
                                     unit, track, sector, offset. And another
                                     max 256 for its data */

#define RAW_BLOCK_SIZE  256     /**< size of a block/sector, including the
                                     (track,sector) pointer */

/** \brief  Number of bytes to display per line for the `block` command
 *
 * Normally, on 80 character terminals, 16 bytes is a decent number.
 */
#define BLOCK_CMD_WIDTH     16


/** \brief  Track/Sector link object
 *
 * Used in the `chain` command to detect cyclic references. Could perhaps be
 * reused for other commands, in which case it would make sense to add a
 * `prev` node to be able to iterate both ways and print links in their proper
 * order.
 */
typedef struct link_s {
    unsigned int track;     /**< track number */
    unsigned int sector;    /**< sector number */
    struct link_s *next;    /**< next node in linked list */
} link_t;


/** \brief  CBM DOS file type strings
 */
static const char *cbm_filetypes[] = {
    "del", "seq", "prg", "usr", "rel", "cbm", "dir", "007" };

/** \brief  Magic bytes for a P00 header
 */
static const char p00_header[] = "C64File";

/** \brief  Flags for each virtual drive indicating P00 mode
 *
 * When zero, reading files from the host OS is done with FILEIO_MODE_RAW, if
 * non-zero, reading files is done with FILEIO_MODE_P00
 */
static unsigned int p00save[NUM_DISK_UNITS] = { 0, 0, 0, 0 };

/** \brief  Current virtual drive used
 *
 * This is an index into the `drives` array, NOT a device/unit number
 */
static int drive_index = 0;

/** \brief  Flag indicating if c1541 is used in interactive mode
 */
static int interactive_mode = 0;


/*
 * forward declaration of functions
 */

/* helpers */
static int check_drive_unit(int unit);
static int check_drive_index(int index);
static int check_drive_ready(int index);
static int parse_track_sector(const char *trk_str, const char *sec_str,
                              unsigned int *trk_num, unsigned int *sec_num);
static const char *image_format_name(unsigned int type);

#if 0
static int translate_fsimage_error(int err);
#endif


/* command handlers */
static int attach_cmd(int nargs, char **args);
static int bam_cmd(int nargs, char **args);
static int bcopy_cmd(int nargs, char **args);
static int bfill_cmd(int nargs, char **args);
static int block_cmd(int nargs, char **args);
static int bpeek_cmd(int nargs, char **args);
static int bpoke_cmd(int nargs, char **args);
static int bread_cmd(int nargs, char **args);
static int bwrite_cmd(int nargs, char **args);
static int cd_cmd(int nargs, char **args);
static int chain_cmd(int nargs, char **args);
static int copy_cmd(int nargs, char **args);
static int delete_cmd(int nargs, char **args);
static int entry_cmd(int nargs, char **args);
static int extract_cmd(int nargs, char **args);
static int extract_geos_cmd(int nargs, char **args);
static int format_cmd(int nargs, char **args);
static int help_cmd(int nargs, char **args);
static int info_cmd(int nargs, char **args);
static int list_cmd(int nargs, char **args);
static int name_cmd(int nargs, char **args);
static int p00save_cmd(int nargs, char **args);
static int pwd_cmd(int nargs, char **args);
static int quit_cmd(int nargs, char **args);
static int raw_cmd(int nargs, char **args); /* @ */
static int read_cmd(int nargs, char **args);
static int read_geos_cmd(int nargs, char **args);
static int rename_cmd(int nargs, char **args);
static int silent_cmd(int narg, char **args);
static int tape_cmd(int nargs, char **args);
static int unit_cmd(int nargs, char **args);
static int unlynx_cmd(int nargs, char **args);
static int unzip_cmd(int nargs, char **args);
static int validate_cmd(int nargs, char **args);
static int verbose_cmd(int nargs, char **args);
static int version_cmd(int nargs, char **args);
static int write_cmd(int nargs, char **args);
static int write_geos_cmd(int nargs, char **args);

static int disable_libdebug_output_cmd(int nargs, char **args);


/* other functions */
static int open_image(int dev, char *name, int create, int disktype);

int internal_read_geos_file(int unit, FILE* outf, char* src_name_ascii);
static int fix_ts(int unit, unsigned int trk, unsigned int sec,
                  unsigned int next_trk, unsigned int next_sec,
                  unsigned int blk_offset);
static int internal_write_geos_file(int unit, FILE* f);

/* ------------------------------------------------------------------------- */

/** \brief  Array of virtual drives
 *
 * FIXME: this should be in c1541-stubs.c
 *
 */
static vdrive_t *drives[NUM_DISK_UNITS] = { NULL, NULL, NULL, NULL };

struct vdrive_s *file_system_get_vdrive(unsigned int unit)
{
    if (unit < DRIVE_UNIT_MIN || unit > DRIVE_UNIT_MAX) {
        printf("Wrong unit for vdrive");
        return NULL;
    }

    return drives[unit - 8];
}

/* ------------------------------------------------------------------------- */

/** \brief  c1541 command data struct
 */
struct command_s {
    const char *name;                       /**< command name */
    const char *syntax;                     /**< command syntax help */
    const char *description;                /**< command description */
    unsigned int min_args;                  /**< minimum number of arguments */
    unsigned int max_args;                  /**< maximum number of arguments */
    int (*func)(int nargs, char **args);    /**< command handler */
};
typedef struct command_s command_t;


/** \brief  List of commands
 */
const command_t command_list[] = {
    { "@",
      "@ [<command>]",
      "Execute specified CBM DOS command and print the current status of the\n"
      "drive.  If no <command> is specified, just print the status.",
      0, 1,
      raw_cmd },
    { "?",
      "? [<command>]",
      "Explain specified command.  If no command is specified, "
      "list available\nones.",
      0, 1,
      help_cmd },
    { "attach",
      "attach <diskimage> [<unit>]",
      "Attach <diskimage> to <unit> (default unit is 8).",
      1, 2,
      attach_cmd },
    { "bam",
      "bam [<unit>] | <track-min> <track-max> [<unit>]",
      "Show BAM of disk image, optionally limiting the dump to <track-min> - "
      "<track-max>",
      0, 3,
      bam_cmd },
    { "bcopy",
      "bcopy <src-track> <src-sector> <dst-track> <dst-sector> [<src-unit> "
      "[<dst-unit>]]",
      "Copy a block to another block, optionally using different units. "
      "If no unit\nis given, the current unit is used; if only one unit is "
      "given, that unit\nis used for both source and destination.",
      4, 6,
      bcopy_cmd },
    { "bfill",
      "bfill <track> <sector> <value> [<unit>]",
      "Fill a block with a single value.",
      3, 4,
      bfill_cmd },
    { "block",
      "block <track> <sector> [<offset>] [<unit>]",
      "Show specified disk block in hex form.",
      2, 4,
      block_cmd },
    { "bpeek",
      "bpeek [@<unit>:] <track> <sector> [<offset> [<end>]]",
      "Show data of <track,sector> from <offset> to <end>",
      2, 5,
      bpeek_cmd },
    { "bpoke",
      "bpoke [@<unit>:] <track> <sector> <offset> <data ...>",
      "Poke <data> into block at (<track>,<sector>), starting at <offset>",
      4, MAXARG,
      bpoke_cmd },
    { "bread",
      "bread <filename> <track> <sector> [<unit>]",
      "Read block data from (<track>,<sector>) and write as <filename> to "
      "the host file system",
      3, 4,
      bread_cmd },
    { "bwrite",
      "bwrite <filename> <track> <sector> [<unit>]",
      "Write block data to (<track>,<sector>) from <filename> at the host "
      "file system",
      3, 4,
      bwrite_cmd },
    { "cd",
      "cd <hostdir>",
      "Change current host directory path",
      1, 1,
      cd_cmd },
    { "chain",
      "chain <track> <sector> [<unit>] | <filename>",
      "Follow and print block chain starting at (<track>,<sector>)",
      1, 3,
      chain_cmd },
    { "copy",
      "copy <source1> [<source2> ... <sourceN>] <destination>",
      "Copy `source1' ... `sourceN' into destination.  If N > 1, "
      "`destination'\n must be a simple drive specifier (@n:).",
      2, MAXARG,
      copy_cmd },
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
    { "disable-libdebug-output",
      "disable-libdebug-output",
      "Disable output of lib.c when compiled with --enable-debug",
      0, 0,
      disable_libdebug_output_cmd },
    { "entry",
      "entry [+side] <file1> [<file2> ... <fileN>]",
      "Show detailed directory entry of each <file>.",
      1, MAXARG,
      entry_cmd },
    { "exit",
      "exit",
      "Exit (same as `quit').",
      0, 0,
      quit_cmd },
    { "extract",
      "extract [<unit>]",
      "Extract all the files to the file system.",
      0, 1,
      extract_cmd },
    { "format",
      "format <diskname,id> [<type> <imagename>] [<unit>]",
      "If <unit> is specified, format the disk in unit <unit>.\n"
      "If <type> and <imagename> are specified, create a new image named\n"
      "<imagename>, attach it to unit 8 and format it.  <type> is a disk image\n"
      "type, and must be either "
#ifdef HAVE_X64_IMAGE
      "`x64', "
#endif
      "`d64' (both VC1541/2031), `g64' (VC1541/2031,\n"
      "but in GCR coding), `d67' (2040 DOS1), `d71' (VC1571), `g71' (VC1571,\n"
      "but in GCR coding), `d81' (VC1581), `d80' (CBM8050) or `d82' (CBM8250).\n"
      "Otherwise, format the disk in the current unit, if any.",
      1, 4,
      format_cmd },
    { "geosread",
      "geosread <source> [<destination>]",
      "Read GEOS <source> from the disk image and copy it as a Convert file "
      "into \n<destination> in the file system.  If <destination> is not "
      "specified, copy \nit into a file with the same name as <source>."
      "\nPlease note that due to GEOS using ASCII, not PETSCII, the name should"
      " be\bentered in inverted case (ie to read 'rEADmE', use 'ReadMe'",
      1, 2,
      read_geos_cmd },
    { "geoswrite",
      "geoswrite <source>",
      "Write GEOS Convert file <source> from the file system on a disk image.",
      1, 1,
      write_geos_cmd },
    { "geosextract",
      "geosextract <source>",
      "Extract all the files to the file system and GEOS Convert them.",
      0, 1,
      extract_geos_cmd },
    { "help",
      "help [<command>]",
      "Explain specified command.  If no command is specified, list "
      "available\n"      "ones.",
      0, 1,
      help_cmd },
    { "info",
      "info [<unit>]",
      "Display information about unit <unit> (if unspecified, use the "
      "current\none).",
      0, 1,
      info_cmd },
    { "list",
      "list [<pattern>]",
      "List files matching <pattern> (default is all files).",
      0, 1,
      list_cmd },
    { "name",
      "name <diskname>[,<id>] <unit>",
      "Change image name.",
      1, 2,
      name_cmd },
    { "p00save",
      "p00save [<enable> [<unit>]]",
      "Save P00 files to the file system. If no argument is given, print the "
      "state for all drives.\n"
      "The <enable> argument should be either 0 or 1.",
      0, 2,
      p00save_cmd },
    { "pwd",
      "pwd",
      "Show current host directory path",
      0, 0,
      pwd_cmd },
    { "quit",
      "quit",
      "Exit (same as `exit').",
      0, 0,
      quit_cmd },
    { "read",
      "read <source> [<destination>]",
      "Read <source> from the disk image and copy it into <destination> in\n"
      "the file system.  If <destination> is not specified, copy it into a\n"
      "file with the same name as <source>.",
      1, 2,
      read_cmd },
    { "rename",
      "rename <oldname> <newname>",
      "Rename <oldname> into <newname>.  The files must be on the same drive.",
      2, 2,
      rename_cmd },
    { "silent",
      "silent <off>",
      "Disable all logging",
      0, 1,
      silent_cmd },
    { "tape",
      "tape <t64name> [<file1> ... <fileN>]",
      "Extract files from a T64 image into the current drive.",
      1, MAXARG,
      tape_cmd },
    { "unit",
      "unit <number>",
      "Make unit <number> the current unit.",
      1, 1,
      unit_cmd },
    { "unlynx",
      "unlynx <lynxname> [<unit>]",
      "Extract the specified Lynx image file into the specified unit "
      "(default\nis the current unit).",
      1, 2,
      unlynx_cmd },
    { "unzip",
      "unzip <d64name> <zipname> [<label,id>]",
      "Create a D64 disk image out of a set of four Zipcoded files named\n"
      "`1!<zipname>', `2!<zipname>', `3!<zipname>' and `4!<zipname>'.",
      2, 3,
      unzip_cmd },
    { "validate",
      "validate [<unit>]",
      "Validate the disk in unit <unit>.  If <unit> is not specified, "
      "validate\nthe disk in the current unit.",
      0, 1,
      validate_cmd },
    { "verbose",
      "verbose [off]",
      "Enable verbose output. Use 'verbose off' to disable.",
      0, 1,
      verbose_cmd },
    { "version",
      "version",
      "Display C1541 version",
      0, 0,
      version_cmd },
    { "write",
      "write <source> [<destination>]",
      "Write <source> from the file system into <destination> on a disk "
      "image.",
      1, 2,
      write_cmd },
    { "x",
      "x",
      "Exit (same as 'quit', mirrors monitor 'x')",
      0, 0,
      quit_cmd },
    /* FIXME: name is wrong: this doesn't create a zipcoded archive, but
     *        dissolves one, so a better name would be 'unzip' or 'zdecode'.
     * (BW) */
#if 0
    { "zcreate",
      "zcreate <d64name> <zipname> [<label,id>]",
      "Create a D64 disk image out of a set of four Zipcoded files named\n"
      "`1!<zipname>', `2!<zipname>', `3!<zipname>' and `4!<zipname>'.",
      2, 3,
      zcreate_cmd },
#endif
    { NULL, NULL, NULL, 0, 0, NULL }
};

/* ------------------------------------------------------------------------- */


/** \brief  Split \a line into a list of arguments
 *
 * The \a args array is reused on each call, (re)allocating elements when
 * required.
 *
 * \param[in]   line    input string
 * \param[out]  nargs   number of arguments parsed from \a line
 * \param[out]  args    arguments parsed from \a line
 *
 * \return  0 on success, -1 on failure
 */
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
#ifndef WINDOWS_COMPILE
            case '\\':
                begin_of_arg = 0;
                *(d++) = *(++s);
                continue;
#endif
            case ' ':   /* fallthrough */
            case '\t':  /* fallthrough */
            case '\n':  /* fallthrough */
            case '\r':  /* fallthrough */
            case 0:
                if (*s == 0 && in_quote) {
                    fprintf(stderr, "unbalanced quotes\n");
                    return -1;
                }
                if (!in_quote && !begin_of_arg) {
                    if (*nargs == MAXARG) {
                        fprintf(stderr, "too many arguments\n");
                        return -1;
                    } else {
                        size_t len = (size_t)(d - tmp);
                        if (args[*nargs] != NULL) {
                            args[*nargs] = lib_realloc(args[*nargs], len + 1);
                        } else {
                            args[*nargs] = lib_malloc(len + 1);
                        }
                        memcpy(args[*nargs], tmp, len);
                        args[*nargs][len] = 0;
                        begin_of_arg = 1;
                        (*nargs)++;
                        d = tmp;
                    }
                }
                if (*s == 0) {
                    return 0;
                }
                if (!(*s == ' ' && in_quote)) {
                    break;
                }       /* fallthrough */
            default:
                begin_of_arg = 0;
                *(d++) = *s;
        }
    }

    return 0;
}


/** \brief  Convert \a arg to int
 *
 * This function accepts multiple bases, depending on the prefix of the arg.
 *
 * Basically it comes down to this (all resolve to 42 decimal)
 *
 * | prefix | base | example   |
 * |:------:| ----:| --------- |
 * | [none] | 10   | 042       |
 * | %      | 2    | %101010   |
 * | &      | 8    | &52       |
 * | $      | 16   | $2a       |
 * | 0[bB]  | 2    | 0b%101010 |
 * | 0[xX]  | 16   | 0x2a      |
 *
 * \param[in]   arg             string containing a possible integer literal
 * \param[out]  return_value    \a arg convert to int
 *
 * \return  0 on success, -1 on failure
 *
 * \note    on error, \a return_value is set to `INT_MIN`
 */
static int arg_to_int(const char *arg, int *return_value)
{
    char *tailptr;
    int counter = 0;
    int base = 10;

    /* set value for error conditions, keeps compiler happy */
    *return_value = INT_MIN;

    if (arg == NULL || *arg == '\0') {
        return -1;
    }
    /* determine base */
    switch (*arg) {
        case '%':
            base = 2;
            arg++;
            break;
        case '&':
            base = 8;
            arg++;
            break;
        case '$':
            arg++;
            base = 16;
            break;
        case '0':
            arg++;
            if (*arg == 'b' || *arg == 'B') {
                base = 2;
                arg++;
            } else if (*arg == 'x' || *arg == 'X') {
                base = 16;
                arg++;
            }
            break;
        default:
            break;  /* base is already 10 */
    }

    *return_value = (int)strtol(arg, &tailptr, base);

    if (errno == ERANGE) {
        return -1;
    }

    /* Only whitespace is allowed after the last valid character.  */
    if (!util_check_null_string(tailptr)) {
        while (isspace((unsigned char)tailptr[counter])) {
            counter++;
        }
        tailptr += counter;
        if (*tailptr != 0) {
            return -1;
        }
    }
    return 0;
}


/** \brief  Print error message for \a errval on stderr
 *
 * \param[in]   errval  error code
 */
static void print_error_message(int errval)
{
    if (errval < 0) {
        switch (errval) {
            case FD_OK:
                break;
            case FD_NOTREADY:
                fprintf(stderr, "drive not ready\n");
                break;
            case FD_CHANGED:
                fprintf(stderr, "image file has changed on disk\n");
                break;
            case FD_NOTRD:
                fprintf(stderr, "cannot read file\n");
                break;
            case FD_NOTWRT:
                fprintf(stderr, "cannot write file\n");
                break;
            case FD_WRTERR:
                fprintf(stderr, "floppy write failed\n");
                break;
            case FD_RDERR:
                fprintf(stderr, "floppy read failed\n");
                break;
            case FD_INCOMP:
                fprintf(stderr, "incompatible DOS version\n");
                break;
            case FD_BADIMAGE:
                fprintf(stderr, "invalid image\n"); /* Disk or tape */
                break;
            case FD_BADNAME:
                fprintf(stderr, "invalid filename\n");
                break;
            case FD_BADVAL:
                fprintf(stderr, "illegal value\n");
                break;
            case FD_BADDEV:
                fprintf(stderr, "illegal device number\n");
                break;
            case FD_BAD_TS:
                fprintf(stderr, "inaccessible track or sector\n");
                break;
            case FD_BAD_TRKNUM:
                fprintf(stderr, "illegal track number\n");
                break;
            case FD_BAD_SECNUM:
                fprintf(stderr, "illegal sector number\n");
                break;
            default:
                fprintf(stderr, "<unknown error>\n");
        }
    }
}


#if 0
/** \brief  Translate error codes from fsimage-check.c to FD_ error codes
 *
 * \param[in]   err error code from fsimage-check.c
 *
 * \return      translated error code
 *
 * \todo        Use this whenever disk_image_check_sector() is used. Now the
 *              error code returned is usually FD_BAD_TS, which results in an
 *              "Inaccesible track or sector" message, which is confusing.
 */
static int translate_fsimage_error(int err)
{
    switch (err) {
        case FSIMAGE_BAD_TRKNUM:
            return FD_BAD_TRKNUM;
        case FSIMAGE_BAD_SECNUM:
            return FD_BAD_SECNUM;
        default:
            return err;
    }
}
#endif

/** \brief  Return code for lookup_command(): command not found
 */
#define LOOKUP_NOTFOUND -1

/** \brief  Return code for lookup_command(): input matches multiple commands
 */
#define LOOKUP_AMBIGUOUS -2

/** \brief  Look up \a cmd in the command list
 *
 * \param[in]   cmd command name or part of command name
 *
 * \return  index in command array on success, < 0 on failure
 */
static int lookup_command(const char *cmd)
{
    size_t cmd_len = strlen(cmd);
    int match = LOOKUP_NOTFOUND;
    int i;

    for (i = 0; command_list[i].name != NULL; i++) {
        size_t len = strlen(command_list[i].name);

        if (len < cmd_len) {
            /* cmd will never match current command in list */
            continue;
        }
        if (memcmp(command_list[i].name, cmd, cmd_len) == 0) {
            if (match != LOOKUP_NOTFOUND) {
                return LOOKUP_AMBIGUOUS;
            }
            match = i;
            if (len == cmd_len) {
                break;  /* exact match */
            }
        }
    }
    return match;
}


/** \brief  Get image type name from type number
 *
 * \param[in]   type    on of the VDRIVE_FORMAT_TYPE constants
 *
 * \return  image type string or `NULL` when not found
 *
 * XXX:     Should probably be moved to vdrive somewhere
 */
static const char *image_format_name(unsigned int type)
{
    /* I use a switch here since the constants in vdrive.h are defines. Right
     * now they could be an enum, but if someone decides to change the defines
     * with gaps in them, a table-based lookup will fail badly */
    switch (type) {
        case VDRIVE_IMAGE_FORMAT_1541:
            return "1541";
        case VDRIVE_IMAGE_FORMAT_1571:
            return "1571";
        case VDRIVE_IMAGE_FORMAT_1581:
            return "1581";
        case VDRIVE_IMAGE_FORMAT_8050:
            return "8050";
        case VDRIVE_IMAGE_FORMAT_8250:
            return "8250";
        case VDRIVE_IMAGE_FORMAT_2040:
            return "2040";
        case VDRIVE_IMAGE_FORMAT_NP:
            return "Native Partition";
        default:
            return NULL;
    }
}




/** \brief  Look up \a cmd and execute
 *
 * \param[in]   nargs   number of arguments in \a args
 * \param[in]   args    arguments
 *
 * \return  0 on success, -1 on failure
 */
static int lookup_and_execute_command(int nargs, char **args)
{
    int match = lookup_command(args[0]);

    if (match >= 0) {
        const command_t *cp;

        cp = &command_list[match];
        if (nargs - 1 < (int)(cp->min_args)
            || nargs - 1 > (int)(cp->max_args)) {
            fprintf(stderr, "wrong number of arguments\n");
            fprintf(stderr, "syntax: %s\n", cp->syntax);
            return -1;
        } else {
            int retval;

            retval = command_list[match].func(nargs, args);
            print_error_message(retval);
            if (retval == FD_OK) {
                return 0;
            } else {
                return -1;
            }
        }
    } else {
        if (match == LOOKUP_AMBIGUOUS) {
            fprintf(stderr, "command `%s' is ambiguous.  Try `help'\n",
                    args[0]);
        } else {
            fprintf(stderr, "command `%s' unrecognized.  Try `help'\n",
                    args[0]);
        }
        return -1;
    }
}


/** \brief  Parse and validate unit number from a '@<unit>:' string
 *
 * \param[in]   name    string to parse
 * \param[out]  endptr  pointer to string after the '@<unit>:' stuff
 *
 * \return  unit number if successful, 0 if no '@<unit>:' was found, -1 if the
 *          parsed unit number is invalid
 */
static int extract_unit_from_file_name(char *name, char **endptr)
{
    long result;

    if (name == NULL || *name == '\0' || *name != '@') {
        *endptr = name;
        return 0;
    }

    /* try to parse an integer between '@' and ':' */
    result = strtol(name + 1, endptr, 10);
    if (*endptr != NULL && **endptr == ':') {
        /* got something */
        (*endptr)++;
        if (check_drive_unit((int)result) == FD_OK) {
            return (int)result;
        } else {
            return -1;
        }
    }
    *endptr = name;
    return 0;
}




/** \brief  Check if \a name is a valid filename
 *
 * \param[in]   name    filename
 *
 * \return  bool
 */
static int is_valid_cbm_file_name(const char *name)
{
    /* Notice that ':' is the same on PETSCII and ASCII.  */
    return strchr(name, ':') == NULL;
}


/*
 * Simple linked list to keep track of track/sector links, currently only
 * used for the `chain` command to detect cyclic references.
 */

/** \brief  Add (\a track, \a sector) to links
 *
 * \param[in]   link    linked list with track/sector links
 * \param[in]   track   track number
 * \param[in]   sector  sector number
 *
 * \return  new head of the linked list
 */
static link_t *link_add(link_t *link, unsigned int track, unsigned int sector)
{
    link_t *node = lib_malloc(sizeof *node);

    node->track = track;
    node->sector = sector;
    node->next = link;

    return node;
}

/* unused at the moment, but useful for debugging */
#if 0
/** \brief  Dump \a link and its siblings on stdout
 *
 * \param[in]   link    linked list node
 */
static void link_print(link_t *link)
{
    while (link != NULL) {
        printf("(%2u,%2u) -> ", link->track, link->sector);
        link = link->next;
    }
}
#endif

/** \brief  Free linked list
 *
 * \param[in]   link    linked list node
 */
static void link_free(link_t *link)
{
    while (link != NULL) {
        link_t *next = link->next;
        lib_free(link);
        link = next;
    }
}


/** \brief  Find linked list node for (\a track, \a sector)
 *
 * \param[in]   link    linked list node to start looking
 * \param[in]   track   track number
 * \param[in]   sector  track sector
 *
 * \return  node when found or `NULL` when not found
 */
static link_t *link_find(link_t *link, unsigned int track, unsigned int sector)
{
    while (link != NULL) {
        if (link-> track == track && link->sector == sector) {
            break;
        }
        link = link->next;
    }
    return link;
}


/* ------------------------------------------------------------------------- */


/** \brief  Open a disk image
 *
 * Depending on \a name, this either opens a block device (a seekable device,
 * such as an USB stick), a character device (a real drive using OpenCBM)
 * or an image stored on the host file system.
 *
 * \param[in,out]   vdrive  virtual drive
 * \param[in]       name    path to disk image file/data
 * \param[in]       unit    unit to attach disk to
 *
 * \return 0 on success <0 on failure
 */
static int open_disk_image(vdrive_t *vdrive, const char *name,
                           unsigned int unit)
{
    disk_image_t *image;

    image = disk_image_create();
    /*printf("open_disk_image name:%s unit:%d\n", name, unit);*/

    if (archdep_file_is_blockdev(name)) {
        image->device = DISK_IMAGE_DEVICE_RAW;
        serial_device_type_set(SERIAL_DEVICE_RAW, unit);
        serial_realdevice_disable();
    } else if (archdep_file_is_chardev(name)) {
        image->device = DISK_IMAGE_DEVICE_REAL;
        serial_device_type_set(SERIAL_DEVICE_REAL, unit);
        serial_realdevice_enable();
    } else if (strcmp(name, "opencbm") == 0) {
        /*printf("opencbm name:%s unit:%d\n", name, unit);*/
        image->device = DISK_IMAGE_DEVICE_REAL;
        serial_device_type_set(SERIAL_DEVICE_REAL, unit);
        serial_realdevice_enable();
        /* FIXME: this will only work with a 1541 drive */
        image->type = DISK_IMAGE_TYPE_D64;
        image->tracks = MAX_TRACKS_1541;
        image->max_half_tracks = MAX_TRACKS_1541 * 2;
        printf("NOTE: using opencbm is always assuming 1541 disk layout.\n");
    } else {
        image->device = DISK_IMAGE_DEVICE_FS;
        serial_device_type_set(SERIAL_DEVICE_FS, unit);
        serial_realdevice_disable();
    }

    disk_image_media_create(image);

    image->gcr = NULL;
    image->p64 = lib_calloc(1, sizeof(TP64Image));
    P64ImageCreate((PP64Image)image->p64);
    image->read_only = 0;

    disk_image_name_set(image, name);

    if (disk_image_open(image) < 0) {
        P64ImageDestroy((PP64Image)image->p64);
        lib_free(image->p64);
        disk_image_media_destroy(image);
        disk_image_destroy(image);
        fprintf(stderr, "cannot open file `%s'\n", name);
        return -1;
    }

    vdrive_device_setup(vdrive, unit);
    vdrive_attach_image(image, unit, 0, vdrive);
    return 0;
}


/** \brief  Close disk (image) attached to \a vdrive
 *
 * \param[in,out]   vdrive  virtual drive
 * \param[in]       unit    unit number
 */
static void close_disk_image(vdrive_t *vdrive, int unit)
{
    disk_image_t *image;

    image = vdrive->image;

    if (image != NULL) {
        vdrive_detach_image(image, (unsigned int)unit, 0, vdrive);
        P64ImageDestroy((PP64Image)image->p64);
        lib_free(image->p64);
        if (image->device == DISK_IMAGE_DEVICE_REAL) {
            serial_realdevice_disable();
        }
        disk_image_close(image);
        disk_image_media_destroy(image);
        disk_image_destroy(image);
        vdrive->image = NULL;
        /* also clean up buffer used by the vdrive */
        vdrive_device_shutdown(vdrive);
    }
}

/** \brief  Open image or create a new one
 *
 * If the file exists, it must have valid header.
 *
 * The \a dev parameter is also used as a unit number by add 8 to it, meaning
 * units 8-11 can be used.
 *
 * \param[in]   dev         index in the virtual drive array, must be in the
 *                          range [0 .. NUM_DISK_UNITS-1]
 * \param[in]   name        disk/device name
 * \param[in]   create      create image (boolean)
 * \param[in]   disktype    disk type enumerator
 *
 * \return  0 on success, < 0 on failure
 */
static int open_image(int dev, char *name, int create, int disktype)
{
    if (dev < 0 || dev >= NUM_DISK_UNITS) {
        return -1;
    }

    if (create) {
        if (cbmimage_create_image(name, (unsigned int)disktype) < 0) {
            printf("cannot create disk image\n");
            return -1;
        }
    }

    if (open_disk_image(drives[dev], name, (unsigned int)dev + DRIVE_UNIT_MIN) < 0) {
        printf("cannot open disk image\n");
        return -1;
    }
    return 0;
}


/** \brief  Check if \a unit is a valid drive unit number
 *
 * \param[in]   unit number as on the real machine
 *
 * \return  0 (`FD_OK`) on success, < 0 (`FD_BADDEV`) on failure
 */
static int check_drive_unit(int unit)
{
    return (unit >= DRIVE_UNIT_MIN && unit <= DRIVE_UNIT_MAX) ? FD_OK : FD_BADDEV;
}


/** \brief  Check if \a index is a valid index into the vdrive array
 *
 * \param[in]   index   index in vdrive array
 *
 * \return  0 (`FD_OK`) on success, < 0 (`FD_BADDEV`) on failure
 */
static int check_drive_index(int index)
{
    return (index >= 0 && index < NUM_DISK_UNITS) ? FD_OK : FD_BADDEV;
}


/** \brief  Check if vdrive at \a index is ready
 *
 * \param[in]   index   index in the vdrive array
 *
 * \return  0 (`FD_OK`) on success, < 0 (`FD_BADDEV` or `FD_NOTREADY`) on
 *          failure
 */
static int check_drive_ready(int index)
{
    int status = check_drive_index(index);
    if (status == FD_OK) {
        if (drives[index]->image->device == DISK_IMAGE_DEVICE_FS) {
            if ((drives[index] == NULL) || (drives[index]->image == NULL)) {
                status = FD_NOTREADY;
            }
        }
    }
    return status;
}


/** \brief  Parse \a trk_str and \a sec_str for track and sector numbers
 *
 * Parses its string arguments for track and sector numbers, no actual checking
 * against an image is performed, so a track and sector of 100 will pass. Use
 * drive_image_check_sector() to check if the track and sector are valid for
 * a given image.
 *
 * \param[in]   trk_str string containing a track number
 * \param[in]   sec_str string containing a sector number
 * \param[out]  trk_num object to store track number
 * \param[out]  sec_num object to store sector number
 *
 * \return  FD_OK on success, FD_BAD_TRKNUM or FD_BAD_SECNUM on failure
 */
static int parse_track_sector(const char *trk_str, const char *sec_str,
                              unsigned int *trk_num, unsigned int *sec_num)
{
    int tmp;

    if (arg_to_int(trk_str, &tmp) < 0 || tmp < 1) {
        return FD_BAD_TRKNUM;
    }
    *trk_num = (unsigned int)tmp;
    if (arg_to_int(sec_str, &tmp) < 0 || tmp < 0) {
        return FD_BAD_SECNUM;
    }
    *sec_num = (unsigned int)tmp;
    return FD_OK;
}


/** \brief  Parse \a s for a unit number
 *
 * Parses string \a s for a unit number, and checks if it's a valid unit
 * number. No check if performed if the unit is actually ready (wouldn't make
 * sense when creating/attaching to a unit)
 *
 * \param[in]   s   string to parse
 *
 * \return  unit number or FD_BADDEV on error
 */
static int parse_unit_number(const char *s)
{
    int u;

    if (arg_to_int(s, &u) < 0 || check_drive_unit(u) < 0) {
        return FD_BADDEV;
    }
    return u;
}


/* ------------------------------------------------------------------------- */

/* Here are the commands.  */

/* Note: The double ASCII/PETSCII copies of file names we keep in some
   functions are needed because we want to print the names of the files being
   copied in ASCII and we don't trust `charset_petconvstring()' to be
   reliable to get the original value back when we convert ASCII -> PETSCII
   and then PETSCII -> ASCII again.  */


static int use_opencbm(char *name)
{
    size_t n = strlen(name);
    if (n > 6) {
        if (strcmp(&name[n - 7], "opencbm") == 0) {
            return 1;
        }
    }
    return 0;
}

/** \brief  'attach' command handler
 *
 * Attach a disk image to a virtual drive.
 * Syntax: `attach <image-file> [<unit-number>]`, where unit-number must be
 * 8-11.
 *
 * \param[in]   nargs   number of arguments
 * \param[in]   args    argument list
 *
 * \return  0 on success, `FD_BADDEV` on failure
 */
static int attach_cmd(int nargs, char **args)
{
    int dev = 0;
    char *path = NULL;

    switch (nargs) {
        case 2:
            /* attach <image> */
            dev = drive_index;
            break;
        case 3:
            /* attach <image> <unit> */
            if (arg_to_int(args[2], &dev) < 0) {
                return FD_BADDEV;
            }
            if (check_drive_unit(dev) != FD_OK) {
                return FD_BADDEV;
            }
            dev -= DRIVE_UNIT_MIN;
            break;
        default:
            return FD_BADDEV;
    }

    if (use_opencbm(args[1])) {
        path = lib_strdup(args[1]);
    } else {
        archdep_expand_path(&path, args[1]);
    }
    open_disk_image(drives[dev], path, (unsigned int)dev + DRIVE_UNIT_MIN);
    lib_free(path);
    return FD_OK;
}

/** \brief  Maximum sector numbers to print in the sector header */
#define BAM_SECTOR_HEADER_MAX_SECTORS   256
/** \brief  Size of a sector number header line */
#define BAM_SECTOR_HEADER_MAX_STRLEN   (BAM_SECTOR_HEADER_MAX_SECTORS + \
        ((BAM_SECTOR_HEADER_MAX_SECTORS / 8) - 1))


/** \brief  Print a sector numbers header for a BAM dump
 *
 * Prints a two line header with sector numbers, with a space between each
 * block of eight sectors
 *
 * \param[in]   sectors number of sector number to print
 */
static void bam_print_sector_header(int sectors)
{
    char line0[BAM_SECTOR_HEADER_MAX_STRLEN + 1];
    char line1[BAM_SECTOR_HEADER_MAX_STRLEN + 1];
    char line2[BAM_SECTOR_HEADER_MAX_STRLEN + 1];
    char tmp[10];

    int i = 0;
    int p = 0;

    while (i < sectors && p < BAM_SECTOR_HEADER_MAX_STRLEN) {
        snprintf(tmp, 9, "%3d", i % 1000);
        /*
       line0[p] = (char)(((i / 100) % 10) + '0');
       line1[p] = (char)(((i / 10) % 10) + '0');
       line2[p] = (char)((i % 10) + '0');
        */
        line0[p] = tmp[0];
        line1[p] = tmp[1];
        line2[p] = tmp[2];
        i++;
        p++;
        if ((i % 8 == 0) && (i <sectors)) {
            line0[p] = ' ';
            line1[p] = ' ';
            line2[p] = ' ';
            p++;
        }
    }
    line0[p] = '\0';
    line1[p] = '\0';
    line2[p] = '\0';

    if (sectors > 100) {
        printf("     %s\n", line0);
    }
    printf("     %s\n     %s\n", line1, line2);
}


/** \brief  Print BAM bitmap for tracks \a track_min to \a track_max inclusive
 *
 * \param[in]   vdrive      vdrive object
 * \param[in]   track_min   starting track number
 * \param[in]   track_max   last track number
 *
 * \return      FD_OK on success, < 0 on failure
 */
static int bam_print_tracks(vdrive_t *vdrive,
                             unsigned int track_min,
                             unsigned int track_max)
{
    unsigned int track;

    for (track = track_min; track <= track_max; track++) {
        unsigned int sectors = (unsigned int)vdrive_get_max_sectors(vdrive, track);
/*        unsigned char *bitmap = vdrive_bam_get_track_entry(vdrive, track, 0); */
        unsigned int s = 0;

/*
        if (bitmap == NULL) {
            fprintf(stderr,
                    "error: got NULL for bam entry for track %u\n",
                    track);
            return FD_BADVAL;
        }
*/

        printf("%3u  ", track);
        while (s < sectors) {
            putchar(vdrive_bam_is_sector_allocated(vdrive, track, s) ? '*' : '.');
            s++;
            if ((s % 8 == 0) && (s < sectors)) {
                putchar(' ');
            }
        }
        putchar('\n');
    }
    return FD_OK;
}


/** \brief  Show BAM of an attached image
 *
 * Display a bitmap of used/free sectors for each track in the image
 *
 * Syntax:
 *
 * \param[in]   nargs   argument count
 * \param[in]   args    argument list
 *
 * \return  FD_OK on success, < 0 on failure
 */
static int bam_cmd(int nargs, char **args)
{
    int unit = drive_index + DRIVE_UNIT_MIN;
    vdrive_t *vdrive;
    int max_sectors;

    unsigned int track_min = 0; /* 0 means first track in image */
    unsigned int track_max = 0; /* 0 means last track in image */

    int result = FD_OK;

    /* get unit number, if provided by the user */
    if (nargs == 2) {
        unit = parse_unit_number(args[1]);
        if (unit < 0) {
            return unit;
        }
    } else if (nargs > 2) {
        /* default unit, track-min and track-max */
        result = parse_track_sector(args[1], args[2], &track_min, &track_max);
        if (result < 0) {
            return result;
        }
        if (nargs == 4) {
            /* parse unit number */
            unit = parse_unit_number(args[3]);
            if (unit < 0) {
                return unit;
            }
        }
    }

#if 0
    printf("bam_cmd(): unit #%d\n", unit);
#endif
    /* get vdrive instance */
    result = check_drive_ready(unit - DRIVE_UNIT_MIN);
    if (result < 0) {
        return result;
    }
    vdrive = drives[unit - DRIVE_UNIT_MIN];
#if 0
    printf("bam_cmd(): image format: %s\n", image_format_name(vdrive->image_format));
    printf("bam_cmd(): BAM size: $%x\n", vdrive->bam_size);
#endif

    /* set track min and max */
    if (track_min == 0) {
        track_min = 1;
    }
    if (track_max == 0) {
        track_max = vdrive->num_tracks;
    }

    if (track_min < 1 || track_max > vdrive->num_tracks) {
        return FD_BAD_TRKNUM;
    }
    if (track_min > track_max) {
        return FD_BADVAL;
    }

    /* print sector numbers header
     * XXX: this assumes track 1 always has the maximum number of sectors for
     *      an image */
    max_sectors = vdrive_get_max_sectors(vdrive, 1);
    if (max_sectors < 0) {
        return FD_BADVAL;
    }

    /* print sector numbers header and the actual BAM bitmap per track */
    bam_print_sector_header(max_sectors);
    bam_print_tracks(vdrive, track_min, track_max);
    return FD_OK;
}


/** \brief  Copy block to another block
 *
 * Copies a single block (sector) to another block, optionally between different
 * units.
 *
 * Syntax: `bcopy <src_trk> <src_sec> <dst_trk> <dst_sec> [<src_unit> [<dst_unit>]]`
 *
 * \param[in]   nargs   argument count
 * \param[in]   args    argument list
 *
 * \return  FD_OK on success < 0 on failure
 */
static int bcopy_cmd(int nargs, char **args)
{
    unsigned char buffer[RAW_BLOCK_SIZE];
    unsigned int src_trk;
    unsigned int src_sec;
    unsigned int dst_trk;
    unsigned int dst_sec;
    int src_unit = drive_index + DRIVE_UNIT_MIN;
    int dst_unit = drive_index + DRIVE_UNIT_MIN;
    vdrive_t *src_vdrive;
    vdrive_t *dst_vdrive;
    int err;

    /* get source and destination blocks */
    err = parse_track_sector(args[1], args[2], &src_trk, &src_sec);
    if (err < 0) {
        return err;
    }
    err = parse_track_sector(args[3], args[4], &dst_trk, &dst_sec);
    if (err < 0) {
        return err;
    }

    /* get optional unit numbers */
    if (nargs > 5) {
        if (arg_to_int(args[5], &src_unit) < 0
                || check_drive_unit(src_unit) < 0) {
            return FD_BADDEV;
        }
        if (nargs > 6) {
            if (arg_to_int(args[6], &dst_unit) < 0
                    || check_drive_unit(dst_unit) < 0) {
                return FD_BADDEV;
            }
        } else {
            dst_unit = src_unit;
        }
    }

#if 0
    printf("bcopy(): from #%d (%2u,%2u) to #%d (%2u,%2u)\n",
            src_unit, src_trk, src_sec, dst_unit, dst_trk, dst_sec);
#endif
    /* don't do anything if source and dest are the same */
    if ((src_unit == dst_unit) && (src_trk == dst_trk) && (src_sec == dst_sec)) {
        return FD_OK;
    }


    /* check if the units are ready */
    if (check_drive_ready(src_unit - DRIVE_UNIT_MIN) < 0
            || check_drive_ready(dst_unit - DRIVE_UNIT_MIN) < 0) {
        return FD_NOTREADY;
    }

    src_vdrive = drives[src_unit - DRIVE_UNIT_MIN];
    dst_vdrive = drives[dst_unit - DRIVE_UNIT_MIN];

/* don't do this as vdrive does the logical to physical sector mapping */
#if 0
    /* check unit(s) for valid track/sector) */
    err = disk_image_check_sector(src_vdrive->image, src_trk, src_sec);
    if (err < 0) {
        return translate_fsimage_error(err);
    }
    if (dst_unit != src_unit) {
        if (disk_image_check_sector(dst_vdrive->image, dst_trk, dst_sec) < 0) {
            return translate_fsimage_error(err);
        }
    }
#endif

    /* finally we can actually do what this command is supposed to do */
    err = vdrive_read_sector(src_vdrive, buffer, src_trk, src_sec);
    if (err < 0) {
        return err;
    }
    err = vdrive_write_sector(dst_vdrive, buffer, dst_trk, dst_sec);
    if (err < 0) {
        return err;
    }
    return FD_OK;
}


/** \brief  Fill a block using a single value
 *
 * Syntax:  bfill <track> <sector> <value> [<unit>]
 *
 * \param   nargs   number of args (including the command name)
 * \param   args    argument list
 *
 * \return  FD_OK on success, < 0 on failure
 */
static int bfill_cmd(int nargs, char **args)
{
    vdrive_t *vdrive;
    int unit;
    unsigned int track;
    unsigned int sector;
    int err;
    int fill;
    unsigned char buffer[RAW_BLOCK_SIZE];

    /* get track and sector number */
    err = parse_track_sector(args[1], args[2], &track, &sector);
    if (err < 0) {
        return err;
    }

    /* parse and check fill byte */
    if (arg_to_int(args[3], &fill) < 0 || fill < 0 || fill > 255) {
        return FD_BADVAL;
    }

    /* check for optional unit number */
    if (nargs > 4) {
        if (arg_to_int(args[4], &unit) < 0 || check_drive_unit(unit) < 0) {
            return FD_BADDEV;
        }
    } else {
        unit = drive_index + DRIVE_UNIT_MIN;  /* default to current unit */
    }

#if 0
    /* debugging info: */
    printf("bfill_cmd(): track %u, sector %u, fill $%02x, unit %d\n",
            track, sector, (unsigned int)fill, unit);
#endif

    /* check that the drive is ready */
    if (check_drive_ready(unit - DRIVE_UNIT_MIN) < 0) {
        return FD_NOTREADY;
    }

    /* get the virtual drive */
    vdrive = drives[unit - DRIVE_UNIT_MIN];

    /* use this to get a meaningful error message for illegal track,sector
     *
     * XXX: checks sector properly, but lets track numbers larger than the
     *      image's track count pass, leading to vdrive expanding the D64
     *      attached. And the expansion goes wrong: if using (37,2), the
     *      D64 becomes 179968 bytes: 36 full tracks, and three sectors (0-2)
     *      in track 37, so somewhere in vdrive/diskimage/fsimage things go
     *      a little bit wrong.
     * */

/* don't do this as vdrive does the logical to physical sector mapping */
#if 0
    err = disk_image_check_sector(vdrive->image, track, sector);
    if (err < 0) {
        return translate_fsimage_error(err);
    }
#endif

    /* fill and write the block (vdrive doesn't have a vdrive_fill_sector()
     * function, so this will have to do */
    memset(buffer, fill, RAW_BLOCK_SIZE);
    /* should this still fail after al the checks, -1 is returned, which results
     * in an "<unknown error>", which all we can do, -1 is returned for various
     * error conditions */
    return vdrive_write_sector(vdrive, buffer, track, sector);
}


/** \brief  'block' command handler
 *
 * Display a hex dump of a block on a device
 * Syntax: `block <track> <sector> [<offset>] [<unit>]`
 *
 * \param[in]   nargs   number of arguments
 * \param[in]   args    argument list
 *
 * \return  0 on success, < 0 on failure: `FD_BAD_TS`, `FD_BADVAL`, `FD_BADDEC`,
 *          `FD_NOTREADY`, `FD_RDERR`
 */
static int block_cmd(int nargs, char **args)
{
    int drive;  /* index into the drives array */
    int offset = 0;
    unsigned int track;
    unsigned int sector;
    vdrive_t *vdrive;
    uint8_t *buf, chrbuf[BLOCK_CMD_WIDTH + 1], sector_data[RAW_BLOCK_SIZE];
    int cnt;
    int err;

    /* block <track> <sector> [offset] [<drive>] show disk blocks in hex form */
    err = parse_track_sector(args[1], args[2], &track, &sector);
    if (err != FD_OK) {
        return err;
    }

    if (nargs >= 4) {
        if (arg_to_int(args[3], &offset) < 0) {
            return FD_BADVAL;
        }
        if (offset < 0 || offset >= RAW_BLOCK_SIZE) {
            fprintf(stderr,
                    "error: invalid value for `offset` argument: %d, valid "
                    "values are 0-255\n",
                    offset);
            return FD_BADVAL;
        }
    }

    if (nargs == 5) {
        if (arg_to_int(args[4], &drive) < 0) {
            return FD_BADDEV;
        }
        if (check_drive_unit(drive) < 0) {
            return FD_BADDEV;
        }
        drive -= DRIVE_UNIT_MIN;
    } else {
        drive = drive_index;
    }

    if (check_drive_ready(drive) < 0) {
        return FD_NOTREADY;
    }

    vdrive = drives[drive];

/* don't do this as vdrive does the logical to physical sector mapping */
#if 0
    if (disk_image_check_sector(vdrive->image, track, sector) < 0) {
        return FD_BAD_TS;
    }
#endif

    /* Read one block */
    if (vdrive_read_sector(vdrive, sector_data, track, sector) != 0) {
        fprintf(stderr, "cannot read track %u sector %u.", track, sector);
        return FD_RDERR;
    }

    buf = sector_data;

    /* Show block */

    printf("<#%2d: %2u %2u>\n", drive + DRIVE_UNIT_MIN, track, sector);
    while (offset < RAW_BLOCK_SIZE) {
        printf("> %02X ", (unsigned int)offset);
        memset(chrbuf, '\0', BLOCK_CMD_WIDTH + 1);
        for (cnt = 0; cnt < BLOCK_CMD_WIDTH && offset < RAW_BLOCK_SIZE;
                cnt++, offset++) {
            printf(" %02X", buf[offset]);
            chrbuf[cnt] = (buf[offset] < ' ' ?
                        '.' : charset_p_toascii(buf[offset], CONVERT_WITHOUT_CTRLCODES));
        }
        /* fix indentation in case the last line is less than the max width */
        while (cnt++ < BLOCK_CMD_WIDTH) {
            printf("   ");
        }
        printf("  ;%s\n", chrbuf);
    }
    return FD_OK;
}


/** \brief 'poke' some data into a block
 *
 * Syntax: bpoke [unit-specifier] track sector data ...
 *
 * For example: `bpoke @11: 18 0 $90 $43 $4f $4d $50 $58 $59`, this will write
 * 'compyx' to the disk name of a 1541 image attached at unit 11.
 *
 * \param[in]   nargs   argument count
 * \param[in]   args    argument list
 *
 * \return  FD_OK, or < 0 on failure
 */
static int bpoke_cmd(int nargs, char **args)
{
    vdrive_t *vdrive;
    int unit;
    unsigned int track;
    unsigned int sector;
    int offset;
    int arg_idx = 1;
    int err;
    int i;
    char *endptr;
    unsigned char buffer[RAW_BLOCK_SIZE];

    /* first check for a unit number (@<unit>:) */
    unit = extract_unit_from_file_name(args[arg_idx], &endptr);
    if (unit == 0) {
        /* use current unit */
        unit = drive_index + DRIVE_UNIT_MIN;
    } else {
        if (unit < 0) {
            return FD_BADDEV;
        }
        arg_idx++;
    }

    err = parse_track_sector(args[arg_idx], args[arg_idx + 1], &track, &sector);
    if (err < 0) {
        return err;
    }

    if (arg_to_int(args[arg_idx + 2], &offset) < 0) {
        return FD_BADVAL;
    }
#if 0
    printf("bpoke_cmd(): unit #%d: (%2u,%2u), offset %d\n",
            unit, track, sector, offset);
#endif
    arg_idx += 3;

    /* check drive ready */
    if (check_drive_ready(unit - DRIVE_UNIT_MIN) < 0) {
        return FD_NOTREADY;
    }

    vdrive = drives[unit - DRIVE_UNIT_MIN];

/* don't do this as vdrive does the logical to physical sector mapping */
#if 0
    err = disk_image_check_sector(vdrive->image, track, sector);
    if (err < 0) {
        return err;
    }
#endif

    /* get sector data */
    err = vdrive_read_sector(vdrive, buffer, track, sector);
    if (err < 0) {
        return err;
    }

    i = offset;
    while (i < RAW_BLOCK_SIZE && arg_idx < nargs) {
        int b;
        if (arg_to_int(args[arg_idx], &b) < 0) {
            return FD_BADVAL;
        }
        buffer[i++] = (unsigned char)b;
        arg_idx++;
    }

    /* write back block */
    return vdrive_write_sector(vdrive, buffer, track, sector);
}


/** \brief 'peek' some data from a block
 *
 * Syntax: bpeek [unit-specifier] track sector [start=0 [end=$ff]]
 *
 * For example: `bpoke @11: 18 0 0x90 $af
 *
 * \param[in]   nargs   argument count
 * \param[in]   args    argument list
 *
 * \return  FD_OK, or < 0 on failure
 */
static int bpeek_cmd(int nargs, char **args)
{
    vdrive_t *vdrive;
    uint8_t buffer[RAW_BLOCK_SIZE];
    int unit;
    unsigned int track;
    unsigned int sector;
    int arg_idx = 1;
    int err;
    int start = 0;
    int end = 0xff;
    char *endptr;

    /* first check for a unit number (@<unit>:) */
    unit = extract_unit_from_file_name(args[arg_idx], &endptr);
    if (unit == 0) {
        /* use current unit */
        unit = drive_index + DRIVE_UNIT_MIN;
    } else {
        if (unit < 0) {
            return FD_BADDEV;
        }
        arg_idx++;
    }

    /* check track/sector */
    err = parse_track_sector(args[arg_idx], args[arg_idx + 1], &track, &sector);
    if (err < 0) {
        return err;
    }

    arg_idx += 2;

    /* get start */
    if (arg_idx >= nargs) {
        /* default to 00-ff */
    } else {
        if (arg_to_int(args[arg_idx], &start) < 0) {
            return FD_BADVAL;
        }
        if (start < 0 || start > 0xff) {
            return FD_BADVAL;
        }
        arg_idx++;

        if (arg_idx < nargs) {
            if (arg_to_int(args[arg_idx], &end) < 0) {
                return FD_BADVAL;
            }
            if (end <= start || end > 0xff) {
                return FD_BADVAL;
            }
        }
    }
    printf("unit #%d: (%u,%u): $%02x-$%02x\n", unit, track, sector, (unsigned int)start, (unsigned int)end);

    /* read block */
    vdrive = drives[unit - DRIVE_UNIT_MIN];
    if (vdrive_read_sector(vdrive, buffer, track, sector) != 0) {
        fprintf(stderr, "cannot read track %u sector %u.", track, sector);
        return FD_RDERR;
    }

    /* display data */
    int i = start;
    while (i <= end) {
        printf("%02x: ", (unsigned int)i);
        for (int c = 0; c < 16 && i + c <= end; c++) {
            printf(" %02x", buffer[i + c]);
        }
        putchar('\n');
        i += 16;
    }

    return 0;
}


/** \brief  Read a block from an image and write it to the host file system
 *
 * Syntax: bread \<filename\> \<track\> \<sector\> [\<unit\>]
 *
 * \param[in]   nargs   argument count
 * \param[in]   args    argument list
 *
 * \return  FD_OK on success, < 0 on failure
 *
 * \todo    Add tilde expansion on filename for *nix systems
 */
static int bread_cmd(int nargs, char **args)
{
    unsigned char buffer[RAW_BLOCK_SIZE];
    unsigned int track;
    unsigned int sector;
    int unit = drive_index + DRIVE_UNIT_MIN;
    vdrive_t *vdrive;
    FILE *fd;
    char *path;
    int result;

    /* get track & sector */
    result = parse_track_sector(args[2], args[3], &track, &sector);
    if (result < 0) {
        return result;
    }

    /* get unit number, if specified */
    if (nargs == 5) {
        if (arg_to_int(args[4], &unit) < 0 || check_drive_unit(unit) < 0) {
            return FD_BADDEV;
        }
    }

    /* check drive ready */
    if (check_drive_ready(unit - DRIVE_UNIT_MIN) < 0) {
        return FD_NOTREADY;
    }

    vdrive = drives[unit - DRIVE_UNIT_MIN];

/* don't do this as vdrive does the logical to physical sector mapping */
#if 0
    /* check track,sector */
    result = disk_image_check_sector(vdrive->image, track, sector);
    if (result < 0) {
        return result;
    }
#endif

    /* copy sector to buffer */
    if (vdrive_read_sector(vdrive, buffer, track, sector) != 0) {
        fprintf(stderr, "cannot read track %u sector %u.", track, sector);
        return FD_RDERR;
    }

    /* open file and try to write to it */
    result = FD_OK;
    archdep_expand_path(&path, args[1]);
    fd = fopen(path, "wb");
    if (fd == NULL) {
        result = FD_WRTERR;
    } else {
        if (fwrite(buffer, 1, RAW_BLOCK_SIZE, fd) != RAW_BLOCK_SIZE) {
            fclose(fd);
            result = FD_WRTERR;
        }
        fclose(fd);
    }
    lib_free(path);

    return result;
}


/** \brief Read a block from the host file system and write it to an image
 *
 * Syntax: bwrite \<filename\> \<track\> \<sector\> [\<unit\>]
 *
 * \param[in]   nargs   argument count
 * \param[in]   args    argument list
 *
 * \return  FD_OK on success, < 0 on failure
 *
 * \todo    Add tilde expansion on filename for *nix systems
 */
static int bwrite_cmd(int nargs, char **args)
{
    unsigned char buffer[RAW_BLOCK_SIZE];
    unsigned int track;
    unsigned int sector;
    int unit = drive_index + DRIVE_UNIT_MIN;
    vdrive_t *vdrive;
    FILE *fd;
    char *path;
    int result;

    /* get track & sector */
    result = parse_track_sector(args[2], args[3], &track, &sector);
    if (result < 0) {
        return result;
    }

    /* get unit number, if specified */
    if (nargs == 5) {
        if (arg_to_int(args[4], &unit) < 0 || check_drive_unit(unit) < 0) {
            return FD_BADDEV;
        }
    }

    /* check drive ready */
    if (check_drive_ready(unit - DRIVE_UNIT_MIN) < 0) {
        return FD_NOTREADY;
    }

    vdrive = drives[unit - DRIVE_UNIT_MIN];

/* don't do this as vdrive does the logical to physical sector mapping */
#if 0
    /* check track,sector */
    result = disk_image_check_sector(vdrive->image, track, sector);
    if (result < 0) {
        return result;
    }
#endif

    /* open file for reading */
    result = FD_OK;
    archdep_expand_path(&path, args[1]);
    fd = fopen(path, "rb");
    if (fd == NULL) {
        result = FD_RDERR;
    } else {
        /* read data */
        if (fread(buffer, 1, RAW_BLOCK_SIZE, fd) != RAW_BLOCK_SIZE) {
            result = FD_RDERR;
        }
        fclose(fd);
    }
    if (result == FD_OK) {
        /* write to image */
        result = vdrive_write_sector(vdrive, buffer, track, sector);
        if (result < 0) {
            result = FD_WRTERR;
        }
    }

    lib_free(path);

    return result;
}



/** \brief  Follow and print a block chain
 *
 * \param[in]   nargs   number of arguments
 * \param[in]   args    argument list
 *
 * \return  FD_OK on success, < 0 on failure
 *
 * \todo    proper layout, it's a bit ugly now
 */
static int chain_cmd(int nargs, char **args)
{
    int unit = drive_index + DRIVE_UNIT_MIN;
    unsigned int track;
    unsigned int sector;
    vdrive_t *vdrive;
    int err;
    link_t *link;

    if (nargs == 2) {
        /* assume filename, not (track,sector) */
        char *name_ascii;
        char *name_petscii;
        char *p;
        bufferinfo_t buffer;
        uint8_t *slot;

        unit = extract_unit_from_file_name(args[1], &p);
        if (unit <= 0) {
            name_ascii = lib_strdup(args[1]);
            unit = drive_index + DRIVE_UNIT_MIN;
        } else {
            if (check_drive_ready(unit - DRIVE_UNIT_MIN) < 0) {
                return FD_NOTREADY;
            }
            name_ascii = lib_strdup(p);
        }

        if (!is_valid_cbm_file_name(name_ascii)) {
            fprintf(stderr,
                    "error: `%s' is not a valid CBM DOS file name\n",
                    name_ascii);
            lib_free(name_ascii);
            return FD_BADNAME;
        }

        /* get the directory entry so we can get the track,sector */
        name_petscii = lib_strdup(name_ascii);
        charset_petconvstring((uint8_t *)name_petscii, CONVERT_TO_PETSCII);

        if (vdrive_iec_open(drives[unit - DRIVE_UNIT_MIN],
                            (uint8_t *)name_petscii,
                            (unsigned int)strlen(name_petscii),
                            0, NULL) != 0) {
            fprintf(stderr, "error: cannot read '%s'\n", name_ascii);
            lib_free(name_ascii);
            lib_free(name_petscii);
            return FD_RDERR;
        }

        buffer = drives[unit - DRIVE_UNIT_MIN]->buffers[0];
        slot = buffer.slot;
        /* poeh, we're there ;) */
        track = slot[SLOT_FIRST_TRACK];
        sector = slot[SLOT_FIRST_SECTOR];

        lib_free(name_ascii);
        lib_free(name_petscii);
        vdrive_iec_close(drives[unit - DRIVE_UNIT_MIN], 0);

    } else {
        /* parse track and sector number */
        err = parse_track_sector(args[1], args[2], &track, &sector);
        if (err != FD_OK) {
            return err;
        }

        /* get drive index */
        if (nargs == 4) {
            if (arg_to_int(args[3], &unit) < 0) {
                return FD_BADDEV;
            }
            if (check_drive_unit(unit) < 0) {
                return FD_BADDEV;
            }
        }

        /* check drive to see if it's ready */
        if (check_drive_ready(unit - DRIVE_UNIT_MIN) < 0) {
            return FD_NOTREADY;
        }
    }

    /* now check if the (track,sector) is valid for the current image */
    vdrive = drives[unit - DRIVE_UNIT_MIN];
/* don't do this as vdrive does the logical to physical sector mapping */
#if 0
    if (disk_image_check_sector(vdrive->image, track, sector) < 0) {
        return FD_BAD_TS;
    }
#endif

    /* we keep a list of sectors visited so we can detect cyclic references */
    link = link_add(NULL, track, sector);
    do {
        unsigned char buffer[RAW_BLOCK_SIZE];

        printf("(%2u,%2u) -> ", track, sector);
        fflush(stdout);

        /* read sector data */
        err = vdrive_read_sector(vdrive, buffer, track, sector);
        if (err < 0) {
            return err;
        }
        track = buffer[0];
        sector = buffer[1];

        if (link_find(link, track, sector) != NULL) {
            printf("cyclic reference found to (%u,%u)!\n", track, sector);
            fflush(stdout);
            break;
        }
        link = link_add(link, track, sector);

    } while (track > 0);

    printf("%u\n", sector);     /* # bytes used in last block */
#if 0
    printf("Dumping links:\n");
    link_print(link);
    putchar('\n');
#endif
    /* free list of visited sectors */
    link_free(link);

    return FD_OK;
}


/** \brief  Copy one or more files
 *
 * \param[in]   nargs   argument count
 * \param[in]   args    argument list
 *
 * \return  FD_OK on success, or < 0 on failure
 */
static int copy_cmd(int nargs, char **args)
{
    char *p;
    char *dest_name_ascii;
    char *dest_name_petscii;
    int dest_unit;
    int i;

    dest_unit = extract_unit_from_file_name(args[nargs - 1], &p);
    if (dest_unit <= 0) {
        if (nargs > 3) {
            fprintf(stderr,
                    "the destination must be a drive if multiple sources are specified\n");
            return FD_BADDEV;
        }
        dest_name_ascii = lib_strdup(args[nargs - 1]);
        dest_name_petscii = lib_strdup(dest_name_ascii);
        charset_petconvstring((uint8_t *)dest_name_petscii, CONVERT_TO_PETSCII);
        dest_unit = drive_index + DRIVE_UNIT_MIN;
    } else {
        if (*p != 0) {
            if (nargs > 3) {
                fprintf(stderr,
                        "the destination must be a drive if multiple sources are specified\n");
                return FD_BADDEV;
            }
            dest_name_ascii = lib_strdup(p);
            dest_name_petscii = lib_strdup(dest_name_ascii);
            charset_petconvstring((uint8_t *)dest_name_petscii, CONVERT_TO_PETSCII);
        } else {
            dest_name_ascii = dest_name_petscii = NULL;
        }
    }

    if (dest_name_ascii != NULL && !is_valid_cbm_file_name(dest_name_ascii)) {
        fprintf(stderr,
                "`%s' is not a valid CBM DOS file name\n", dest_name_ascii);
        return FD_BADNAME;
    }

    if (check_drive_ready(dest_unit - DRIVE_UNIT_MIN) < 0) {
        return FD_NOTREADY;
    }

    for (i = 1; i < nargs - 1; i++) {
        char *src_name_ascii;
        char *src_name_petscii;
        uint8_t *slot;
        unsigned int file_type;
        unsigned int rel_record_length;
        int src_unit;

        src_unit = extract_unit_from_file_name(args[i], &p);
        if (src_unit <= 0) {
            src_name_ascii = lib_strdup(args[i]);
            src_unit = drive_index + DRIVE_UNIT_MIN;
        } else {
            if (check_drive_ready(src_unit - DRIVE_UNIT_MIN) < 0) {
                return FD_NOTREADY;
            }
            src_name_ascii = lib_strdup(p);
        }

        if (!is_valid_cbm_file_name(src_name_ascii)) {
            fprintf(stderr,
                    "`%s' is not a valid CBM DOS file name: ignored\n",
                    src_name_ascii);
            lib_free(src_name_ascii);
            continue;
        }

        src_name_petscii = lib_strdup(src_name_ascii);
        charset_petconvstring((uint8_t *)src_name_petscii, CONVERT_TO_PETSCII);

        if (vdrive_iec_open(drives[src_unit - DRIVE_UNIT_MIN], (uint8_t *)src_name_petscii,
                            (unsigned int)strlen(src_name_petscii), 0, NULL)) {
            fprintf(stderr, "cannot read `%s'\n", src_name_ascii);
            if (dest_name_ascii != NULL) {
                lib_free(dest_name_ascii);
                lib_free(dest_name_petscii);
            }

            lib_free(src_name_ascii);
            lib_free(src_name_petscii);
            return FD_RDERR;
        }

        bufferinfo_t *bufferinfo = &drives[src_unit - DRIVE_UNIT_MIN]->buffers[0];        /* 0 = secadr of src */
        slot = bufferinfo->slot;
        file_type = slot[SLOT_TYPE_OFFSET] & 7;
        rel_record_length = slot[SLOT_RECORD_LENGTH];

        /*
         * If we're copying a REL file, create a proper destination file
         * name including the record length.
         */
        if (file_type == CBMDOS_FT_REL) {
            char *oldname;
            char *newname;
            char *comma;

            if (dest_name_petscii) {
                oldname = dest_name_petscii;
            } else {
                oldname = src_name_petscii;
            }

            /* Append ",L," and the record length to the destination name.
             * If there is a comma in it already, truncate there
             * but restore afterwards.
             */
            comma = strchr(oldname, ',');
            if (comma) {
                *comma = '\0';
            }

            newname = lib_msprintf("%s,L,%c", oldname, (int)rel_record_length);

            if (comma) {
                *comma = ',';
            }

            /* Make sure both dest_name_petscii and dest_name_ascii are set. */
            if (dest_name_petscii) {
                lib_free(dest_name_petscii);
            } else {
                dest_name_ascii = lib_strdup(src_name_ascii);
            }
            dest_name_petscii = newname;
        }

        if (dest_name_ascii != NULL) {
            if (vdrive_iec_open(drives[dest_unit - DRIVE_UNIT_MIN],
                        (uint8_t *)dest_name_petscii,
                        (unsigned int)strlen(dest_name_petscii), 1, NULL)) {
                fprintf(stderr, "cannot write `%s'\n", dest_name_petscii);
                vdrive_iec_close(drives[src_unit - DRIVE_UNIT_MIN], 0);
                lib_free(dest_name_ascii);
                lib_free(dest_name_petscii);
                lib_free(src_name_ascii);
                lib_free(src_name_petscii);
                return FD_WRTERR;
            }
        } else {
            if (vdrive_iec_open(drives[dest_unit - DRIVE_UNIT_MIN],
                        (uint8_t *)src_name_petscii,
                                (unsigned int)strlen(src_name_petscii), 1, NULL)) {
                fprintf(stderr, "cannot write `%s'\n", src_name_petscii);
                vdrive_iec_close(drives[src_unit - DRIVE_UNIT_MIN], 0);
                lib_free(src_name_ascii);
                lib_free(src_name_petscii);
                return FD_WRTERR;
            }
        }

        printf("copying `%s' ...\n", args[i]); /* FIXME */

        if (file_type == CBMDOS_FT_REL) {
            unsigned int num_rel_records = bufferinfo->record_max;
            unsigned int record;
            int dnr = src_unit - DRIVE_UNIT_MIN;
            int status;

            /* First allocate space for the whole file */
            status = vdrive_rel_position(drives[dnr], 1,
                    (num_rel_records & 0xFF), (num_rel_records >> 8) & 0xFF,
                    1);
            if (status && status != CBMDOS_IPE_NO_RECORD) {
                fprintf(stderr, "Cannot Position to record %u (err %d)\n",
                        num_rel_records, status);
            }
            status = vdrive_iec_write(drives[dnr], 0, 1);
            if (status != SERIAL_OK) {
                fprintf(stderr, "Cannot write in record %u (err %d)\n",
                        num_rel_records, status);
            }

            for (record = 1; record <= num_rel_records; record++) {
                int bytes = 0;

                status = vdrive_rel_position(drives[dnr], 1,   /* 1 = secadr dest */
                        (record & 0xFF), (record >> 8) & 0xFF,
                        1);
                if (status && status != CBMDOS_IPE_NO_RECORD) {
                    fprintf(stderr, "Cannot Position to record %u (err %d)\n",
                            record, status);
                }

                do {
                    uint8_t c;

                    status = vdrive_iec_read(drives[dnr], &c, 0);
                    if (status == SERIAL_ERROR && bytes == 0) {
                        fprintf(stderr, "dummy record CR; should not happen.\n");
                        break; /* record after EOF; we get a dummy CR */
                    }
                    if (vdrive_iec_write(drives[dest_unit - DRIVE_UNIT_MIN], c, 1)) {
                        fprintf(stderr, "no space on image ?\n");
                        break;
                    }
                    bytes++;
                } while (status == SERIAL_OK);
            }
        } else {
            uint8_t c;
            int status = 0;

            do {
                status = vdrive_iec_read(drives[src_unit - DRIVE_UNIT_MIN],
                        ((uint8_t *)&c), 0);
                if (vdrive_iec_write(drives[dest_unit - DRIVE_UNIT_MIN], c, 1)) {
                    fprintf(stderr, "no space on image ?\n");
                    break;
                }
            } while (status == SERIAL_OK);
        }

        vdrive_iec_close(drives[src_unit - DRIVE_UNIT_MIN], 0);
        vdrive_iec_close(drives[dest_unit - DRIVE_UNIT_MIN], 1);

        lib_free(src_name_ascii);
        lib_free(src_name_petscii);
    }

    lib_free(dest_name_ascii);
    lib_free(dest_name_petscii);
    return FD_OK;
}

/** \brief  Print a side-sector-group, which is at most 6 side sectors.
 *
 * \param[in]   vdrive  the virtual drive from which we read the sectors
 * \param[in]   track   the track number of the first side sector.
 * \param[in]   sector  the sector number of the first side sector.
 * \param[in]   group   when iterating over a super side sector, this is
 *                      the number of the group within it.
 * \param[in,out] side_sector_number   a pointer to a running counter for
 *                      all side sectors belonging to the same file.
 * \param[in,out] data_block_number    a pointer to a running counter for
 *                      all data blocks belonging to the same file.
 *
 * \return  FD_OK on success, or < 0 on failure
 */
static void print_side_sector_group(vdrive_t *vdrive,
                                    unsigned track, unsigned sector,
                                    int group,
                                    unsigned *side_sector_number,
                                    unsigned *data_block_number)
{
    int count = 0;
    int i;

    while (track != 0 && count++ < SIDE_SECTORS_MAX) {
        uint8_t buffer[256];

/* don't do this as vdrive does the logical to physical sector mapping */
#if 0
        /* check track,sector */
        int result = disk_image_check_sector(vdrive->image, track, sector);
        if (result < 0) {
            fprintf(stderr, "Invalid track %u sector %u.", track, sector);
            break;
        }
#endif

        /* copy sector to buffer */
        if (vdrive_read_sector(vdrive, buffer, track, sector) != 0) {
            fprintf(stderr, "Cannot read track %u sector %u.", track, sector);
            break;
        }

        printf("-------------------------------\n");
        printf("Side sector at T/S: %u/%u", track, sector);
        if (group >= 0) {
            printf(" part of group %d", group);
        }
        printf("\n\nNext side sector T/S: %d/%d\n",
                buffer[OFFSET_NEXT_TRACK], buffer[OFFSET_NEXT_SECTOR]);
        printf("Sector number: %d (counted: %u)\n",
                buffer[OFFSET_SECTOR_NUM],
                *side_sector_number);
        ++*side_sector_number;
        printf("Record length: %d\n", buffer[OFFSET_RECORD_LEN]);

        printf("Side sector group:\n");
        for (i = 0; i < 6; i++) {
            printf("%d: %u/%u  ",
                    i,
                    buffer[OFFSET_SIDE_SECTOR + 2*i],
                    buffer[OFFSET_SIDE_SECTOR + 2*i + 1]);
        }
        printf("\nFile data sectors:\n");

        for (i = OFFSET_POINTER; i < 255; i += 2) {
            if (buffer[i] || buffer[i + 1]) {
                printf("%u: %d/%d  ",
                        *data_block_number,
                        buffer[i], buffer[i + 1]);
                ++*data_block_number;
            }
        }
        printf("\n-------------------------------\n");

        track = buffer[OFFSET_NEXT_TRACK];
        sector = buffer[OFFSET_NEXT_SECTOR];
    }
}


/** \brief  Disable lib.c debug output
 *
 * Debug hook: This should only be used when testing c1541's output with the
 * test bench, so the c1541 test bench can be used on both no-debug and debug
 * builds when parsing the output of c1541, without the output of lib.c getting
 * in the way.
 *
 * \param[in]   nargs   argument count
 * \param[in    args    argument list
 *
 * \return 0
 */
static int disable_libdebug_output_cmd(int nargs, char **args)
{
    lib_debug_set_output(0);
    return 0;
}


/** \brief  Show directory entries of file(s) on disk image(s) in
 * low-level detail.
 *
 * \param[in]   nargs   argument count
 * \param[in]   args    argument list
 *
 * \return  0 on success, < 0 on failure
 */
static int entry_cmd(int nargs, char **args)
{
    int show_side_sector = 0;
    int arg = 1;
    int i, j;

    if (arg < nargs && strcmp(args[arg], "+side") == 0) {
        arg++;
        show_side_sector++;
    }

    for (; arg < nargs; arg++) {
        int unit;   /* unit number */
        int dnr;    /* index in drives array */
        char *p;
        char *name_ascii, *name_petscii;
        const int secadr = 2;
        uint8_t *slot;
        uint8_t v;
        vdrive_t *vdrive;

        unit = extract_unit_from_file_name(args[arg], &p);
        if (unit < 0) {
            /* illegal unit between '@' and ':' */
            return FD_BADDEV;
        }
        if (unit == 0) {
            /* no '@<unit>:' found, use current device */
            dnr = drive_index;
        } else {
            dnr = unit - DRIVE_UNIT_MIN;    /* set proper device index */
        }
        if (check_drive_ready(dnr) < 0) {
            return FD_NOTREADY;
        }
        name_ascii = p;   /* update pointer to name_ascii */

        if (!is_valid_cbm_file_name(name_ascii)) {
            fprintf(stderr,
                    "`%s' is not a valid CBM DOS file name: ignored\n", name_ascii);
            continue;
        }

        name_petscii = lib_strdup(name_ascii);
        charset_petconvstring((uint8_t *)name_petscii, CONVERT_TO_PETSCII);

        if (vdrive_iec_open(drives[dnr], (uint8_t *)name_petscii,
                            (unsigned int)strlen(name_petscii), secadr, NULL)) {
            fprintf(stderr, "cannot read `%s'\n", name_ascii);

            lib_free(name_petscii);
            continue;
        }

        vdrive = drives[dnr];
        bufferinfo_t *bufferinfo = &vdrive->buffers[secadr];
        slot = bufferinfo->slot;

        printf(" 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");
        printf("-----------------------------------------------\n");
        for (j = 0; j < 2; j++) {
            for (i = 0; i < SLOT_SIZE / 2; i++) {
                printf("%02x ", slot[j * (SLOT_SIZE / 2) + i]);
            }
            printf("\n");
        }
        v = slot[SLOT_TYPE_OFFSET];
        printf("\nNext directory T/S: %d/%d\n", slot[0], slot[1]);

        printf("Type: 0x%02x: %s", v, cbm_filetypes[v & 7]);
        if (v & CBMDOS_FT_REPLACEMENT) {
            printf(" @replacement");
        }
        if (v & CBMDOS_FT_LOCKED) {
            printf(" <locked");
        }
        if (!(v & CBMDOS_FT_CLOSED)) {
            printf(" *unclosed");
        }
        printf("\n");
        printf("T/S: %d/%d,  %d blocks\n",
                slot[SLOT_FIRST_TRACK],
                slot[SLOT_FIRST_SECTOR],
                slot[SLOT_NR_BLOCKS] + 256 * slot[SLOT_NR_BLOCKS+1]);
        printf("Name: ");
        for (i = 0; i < CBMDOS_SLOT_NAME_LENGTH; i++) {
            printf("%02x ", slot[SLOT_NAME_OFFSET + i]);
        }
        printf("\n");
        printf("%side sector T/S: %d/%d,  Record length: %d\n",
                (bufferinfo->super_side_sector_track ? "Super s" : "S"),
                slot[SLOT_SIDE_TRACK], slot[SLOT_SIDE_SECTOR],
                slot[SLOT_RECORD_LENGTH]);
        printf("@-replacement T/S: %d/%d\n",
                slot[SLOT_REPLACE_TRACK], slot[SLOT_REPLACE_SECTOR]);

        printf("GEOS: IT/S: %d/%d\n", slot[SLOT_GEOS_ITRACK], slot[SLOT_GEOS_ISECTOR]);
        printf("GEOS: struct: %02x,  type: %02x\n",
                slot[SLOT_GEOS_STRUCT], slot[SLOT_GEOS_TYPE]);
        printf("GEOS: YY/mm/dd hh:mm %d/%d/%d %d:%d\n",
                slot[SLOT_GEOS_YEAR],
                slot[SLOT_GEOS_MONTH],
                slot[SLOT_GEOS_DATE],
                slot[SLOT_GEOS_HOUR],
                slot[SLOT_GEOS_MINUTE]);

        /* If this is a REL file, print the side sectors */
        if (show_side_sector &&
            (slot[SLOT_TYPE_OFFSET] & 7) == CBMDOS_FT_REL &&
            slot[SLOT_SIDE_TRACK] != 0) {
            unsigned int super_track = bufferinfo->super_side_sector_track;
            unsigned int super_sector = bufferinfo->super_side_sector_sector;
            unsigned int side_sector_number = 0;
            unsigned int data_block_number = 0;

            if (super_track) {
                int o, side;

                printf("===============================\n");
                printf("Super side sector at T/S: %u/%u\n\n", super_track, super_sector);
                unsigned int track, sector;

                printf("Next T/S: %u/%u,  seq# or 254: %u,  unused at 255: %u\n",
                        bufferinfo->super_side_sector[0],
                        bufferinfo->super_side_sector[1],
                        bufferinfo->super_side_sector[OFFSET_SUPER_254],
                        bufferinfo->super_side_sector[255]);

                printf("Side sector groups:\n");
                o = OFFSET_SUPER_POINTER;
                for (side = 0; side < SIDE_SUPER_MAX; side++, o += 2) {
                    track = bufferinfo->super_side_sector[o];
                    sector = bufferinfo->super_side_sector[o + 1];

                    if (track || sector) {
                        printf("%d: %u/%u  ", side, track, sector);
                    }
                }

                printf("\n===============================\n");

                o = OFFSET_SUPER_POINTER;
                for (side = 0; side < SIDE_SUPER_MAX; side++, o += 2) {
                    track = bufferinfo->super_side_sector[o];
                    sector = bufferinfo->super_side_sector[o + 1];

                    if (track != 0) {
                        print_side_sector_group(vdrive, track, sector,
                                                side,
                                                &side_sector_number,
                                                &data_block_number);
                    }
                }
            } else {
                unsigned int track = bufferinfo->side_sector_track[0];
                unsigned int sector = bufferinfo->side_sector_sector[0];

                print_side_sector_group(vdrive, track, sector,
                                        -1,
                                        &side_sector_number,
                                        &data_block_number);
            }
        } else if (!show_side_sector &&
            (slot[SLOT_TYPE_OFFSET] & 7) == CBMDOS_FT_REL &&
            slot[SLOT_SIDE_TRACK] != 0) {
            printf("This file seems to have side sector(s). Use the +side option to show them.\n");
        }

        vdrive_iec_close(drives[dnr], secadr);

        lib_free(name_petscii);
    }

    return FD_OK;
}


/** \brief  Delete (scratch) file(s) from disk image(s)
 *
 * Delete one or more files. Each file can have a unit number (@\<unit>:) in
 * front of it to indicate which unit to use.
 *
 * \param[in]   nargs   argument count
 * \param[in]   args    argument list
 *
 * \return  0 on success, < 0 on failure
 */
static int delete_cmd(int nargs, char **args)
{
    int i;

    for (i = 1; i < nargs; i++) {
        int unit;   /* unit number */
        int dnr;    /* index in drives array */
        char *p;
        char *name;
        char *command;
        int status;

        unit = extract_unit_from_file_name(args[i], &p);
        if (unit < 0) {
            /* illegal unit between '@' and ':' */
            return FD_BADDEV;
        }
        if (unit == 0) {
            /* no '@<unit>:' found, use current device */
            dnr = drive_index;
        } else {
            dnr = unit - DRIVE_UNIT_MIN;    /* set proper device index */
        }
        if (check_drive_ready(dnr) < 0) {
            return FD_NOTREADY;
        }
        name = p;   /* update pointer to name */


        if (!is_valid_cbm_file_name(name)) {
            fprintf(stderr,
                    "`%s' is not a valid CBM DOS file name: ignored\n", name);
            continue;
        }

        command = util_concat("s:", name, NULL);
        charset_petconvstring((uint8_t *)command, CONVERT_TO_PETSCII);

        printf("deleting `%s' on unit %d\n", name, unit);

        status = vdrive_command_execute(drives[dnr], (uint8_t *)command,
                                        (unsigned int)strlen(command));

        lib_free(command);

        /* vdrive_command_execute() returns CBMDOS_IPE_DELETED even if no
         * files where actually scratched, so just display error messages that
         * actual mean something, not "ERRORCODE 1" */
        if (status != CBMDOS_IPE_OK && status != CBMDOS_IPE_DELETED) {
            printf("%02d, %s, 00, 00\n",
                    status, cbmdos_errortext((unsigned int)status));
        }
    }

    return FD_OK;
}


#define P00_HDR_LEN         0x1a
#define P00_HDR_MAGIC       0x00
#define P00_HDR_MAGIC_LEN   0x08
#define P00_HDR_NAME        0x08
#define P00_HDR_NAME_LEN    0x10
#define P00_HDR_REL_RECLEN  0x19


static int write_p00_header(FILE *fd, const uint8_t *petname)
{
    uint8_t hdr[P00_HDR_LEN];
    int i;

    /* copy "C64File" and nul char as magic */
    memcpy(hdr + P00_HDR_MAGIC, p00_header, P00_HDR_MAGIC_LEN);
    /* copy CBMDOS filename in PETSCII */
    memcpy(hdr + P00_HDR_NAME, petname, P00_HDR_NAME_LEN);
    /* fix up name padding, P00 uses 0x00 for padding instead of the standard
     * 0xa0 padding of CBMDOS
     */
    i = P00_HDR_NAME_LEN - 1;
    while (i >=0 && hdr[P00_HDR_NAME + i] == 0xa0) {
        hdr[P00_HDR_NAME + i] = 0;
        i--;
    }
    /* REL file info, unsupported for now */
    hdr[0x18] = 0;  /* always 0 */
    hdr[P00_HDR_REL_RECLEN] = 0;

    return fwrite(hdr, 1U, P00_HDR_LEN, fd) == P00_HDR_LEN;
}


/* Extract all files <gwesp@cosy.sbg.ac.at>.  */
/* FIXME: This does not work with non-standard file names.  */
/* FIXME: I added P00 support, but it's a little shitty */
static int extract_cmd_common(int nargs, char **args, int geos)
{
    int dnr = 0;
    unsigned int track, sector;
    vdrive_t *floppy;
    disk_image_t *disk_image;
    unsigned int disk_type;
    size_t disk_size;
    size_t total_written;
    uint8_t *buf, *str;
    unsigned int channel = 2;
    char *p00_name = NULL;

    if (nargs == 2) {
        if (arg_to_int(args[1], &dnr) < 0) {
            return FD_BADDEV;
        }
        if (check_drive_unit(dnr) < 0) {
            return FD_BADDEV;
        }
        dnr -= DRIVE_UNIT_MIN;
    }

    if (check_drive_ready(dnr) < 0) {
        return FD_NOTREADY;
    }

    floppy = drives[dnr];

    /* determine size of image to guard against cyclic blocks later */
    disk_image = floppy->image;
    disk_type = disk_image->type;
    disk_size = (size_t)fsimage_size(disk_image);
    /* adjust size in case of Gxx/P64 images, assumes 42/84 track images are
     * possible to avoid false positives in the guard */
    switch (disk_type) {
        case DISK_IMAGE_TYPE_G64:   /* fall through */
        case DISK_IMAGE_TYPE_P64:
            disk_size = D64_FILE_SIZE_35 + (7 * 17 * 256);    /* 42 tracks */
            break;
        case DISK_IMAGE_TYPE_G71:
            disk_size = (D64_FILE_SIZE_35 + (7 * 17 * 256)) * 2;    /* 84 tracks */
            break;
    }

    if (vdrive_iec_open(floppy, (const uint8_t *)"#", 1, channel, NULL)) {
        fprintf(stderr, "cannot open buffer #%u in unit %d\n", channel,
                dnr + DRIVE_UNIT_MIN);
        return FD_RDERR;
    }

    track = floppy->Dir_Track;
    sector = floppy->Dir_Sector;

    total_written = 0;
    while (1) {
        int i, res;


        str = (uint8_t *)lib_msprintf("B-R:%u 0 %u %u", channel, track, sector);
        res = vdrive_command_execute(floppy, str, (unsigned int)strlen((char *)str));

        lib_free(str);

        if (res) {
            return FD_RDERR;
        }

        buf = floppy->buffers[channel].buffer;

        for (i = 0; i < 256; i += SLOT_SIZE) {
            uint8_t file_type = buf[i + SLOT_TYPE_OFFSET];

            if (((file_type & 7) == CBMDOS_FT_SEQ
                        || (file_type & 7) == CBMDOS_FT_PRG
                        || (file_type & 7) == CBMDOS_FT_USR)
                    && (file_type & CBMDOS_FT_CLOSED)) {
                uint8_t *file_name = buf + i + SLOT_NAME_OFFSET;
                int status = 0;
                uint8_t c;
                uint8_t name[IMAGE_CONTENTS_FILE_NAME_LEN + 1];
                uint8_t cbm_name[IMAGE_CONTENTS_FILE_NAME_LEN + 1 + 2];
                FILE *fd;
                unsigned int len;

                memset(name, 0, sizeof(name));
                memset(cbm_name, 0, sizeof(cbm_name));
                for (len = 0; len < IMAGE_CONTENTS_FILE_NAME_LEN; len++) {
                    if (file_name[len] == 0xa0) {
                        break;
                    } else {
                        name[len] = file_name[len];
                        cbm_name[len] = file_name[len];
                    }
                }

                /* Hack to support SEQ/USR
                 *
                 * FIXME: not sure this is the proper way.
                 */
                if ((file_type & 7) == CBMDOS_FT_SEQ) {
                    memcpy(cbm_name + len, ",S", 2);
                    len +=2;
                }
                if ((file_type & 7) == CBMDOS_FT_USR) {
                    memcpy(cbm_name + len, ",U", 2);
                    len +=2;
                }


                charset_petconvstring((uint8_t *)name, CONVERT_TO_ASCII);

                /* translate illegal chars for the host OS to '_' */
                archdep_sanitize_filename((char *)name);
#if 0
                printf("cbm_name = '%s', len = %u\n", cbm_name, len);
#endif
                if (vdrive_iec_open(floppy, cbm_name, len, 0, NULL)) {
                    fprintf(stderr,
                            "cannot open `%s' on unit %d\n",
                            name, dnr + DRIVE_UNIT_MIN);
                    continue;
                }

                if (p00save[dnr]) {
                    char cwd[ARCHDEP_PATH_MAX];
                    char *total;
                    long idx = 0;

                    p00_name = p00_filename_create((const char *)name,
                            file_type & 7);
                    if (archdep_getcwd(cwd, sizeof(cwd)) == NULL) {
                        fprintf(stderr,
                                "Couldn't get the cwd, all bets are off. "
                                "Aborting to get a stack dump.\n");
                        abort();
                    }
                    total = util_join_paths(cwd, p00_name, NULL);

                    printf("Trying filename '%s'\n", total);
                    while (archdep_file_exists(total) && idx < 100) {
                        /* TODO: clean up this mess */
                        char *endptr;
                        size_t pathlen = strlen(total);
#if 0
                        printf("file exists, increment index\n");
#endif
                        idx = strtol(total + pathlen - 2, &endptr, 10);
#if 0
                        printf("got index %ld\n", idx);
#endif
                        /* Looks weird, but the easiest way to silence GCC's
                         * `-Wformat-truncation` warning about idx+1 possibly
                         * taking more than 3 bytes. Mod 100 fixes the printing
                         * more than 2 digits, and casting to unsigned fixes
                         * the possibility of a negative sign.
                         * The "%2.2u" format specifier doesn't limit the number
                         * of digits output, it'll output 2 digits for 0-99,
                         * but 3 digits for 100.
                         */
                        snprintf(total + pathlen - 2, 3, "%02u",
                                (unsigned int)((idx + 1) % 100));
#if 0
                        printf("new name = '%s'\n", total);
#endif
                    }
                    fd = fopen(total, MODE_WRITE);
                    lib_free(total);

                } else {
                    fd = fopen((const char *)name, MODE_WRITE);
                }
                if (fd == NULL) {
                    fprintf(stderr, "cannot create file `%s': %s.\n",
                            name, strerror(errno));
                    vdrive_iec_close(floppy, 0);
                    continue;
                }
                if (geos) {
                    /* FIXME: status is never read */
#if 0
                    status = internal_read_geos_file(dnr, fd, (char *)name);
#else
                    internal_read_geos_file(dnr, fd, (char *)name);
#endif
                } else {
                    /* do we have P00save? */
                    if (p00save[dnr]) {
                        if (!write_p00_header(fd, cbm_name)) {
                            fprintf(stderr, "failed to write P00 header\n");
                            lib_free(p00_name);
                            return FD_WRTERR;
                        }
                    }
                    do {
                        /* guard against cyclic blocks */
                        if (total_written < disk_size) {
                            status = vdrive_iec_read(floppy, &c, 0);
                            fputc(c, fd);
                            total_written++;
                        } else {
                            fprintf(stderr,
                                    "Error: trying to extract more data than"
                                    " image can contain (%zu), possibly a\n"
                                    "       cyclic T/S chain in file '%s',"
                                    " aborting.\n",
                                    disk_size, name);
                            fclose(fd);
                            if (p00_name != NULL) {
                                lib_free(p00_name);
                            }
                            vdrive_iec_close(floppy, channel);
                            return FD_WRTERR;
                        }
                    } while (status == SERIAL_OK);
                }
                if (p00_name != NULL) {
                    lib_free(p00_name);
                }

                vdrive_iec_close(floppy, 0);

                if (fclose(fd)) {
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
    vdrive_iec_close(floppy, channel);
    return FD_OK;
}

static int extract_cmd(int nargs, char **args)
{
    return extract_cmd_common(nargs, args, 0);
}
static int extract_geos_cmd(int nargs, char **args)
{
    return extract_cmd_common(nargs, args, 1);
}


/** \brief  Format a virtual floppy
 *
 * \param[in]   nargs   argument count
 * \param[in]   args    argument list
 *
 * \return  FD_OK on success, or < 0 on failure
 */
static int format_cmd(int nargs, char **args)
{
    char *command;
    int disk_type;
    int dev = -1;   /* index into the drives array */
    int unit;       /* unit number */

    switch (nargs) {
        case 2:
            /* format <diskname,id> */
            dev = drive_index;
            break;
        case 3:
            /* format <diskname,id> <unit> */
            /* Format the disk image in unit <unit>.  */
            if (arg_to_int(args[2], &unit) >= 0
                && check_drive_unit(unit) >= 0) {
                /* It's a valid unit number.  */
                dev = unit -DRIVE_UNIT_MIN;
            } else {
                return FD_BADDEV;
            }
            break;
        case 4: /* fallthrough */
        case 5:
            /* format <diskname,id> <type> <imagename> [<unit>] */
            /* Create a new image.  */
            /* TODO: change into table lookup */
            *args[2] = util_tolower(*args[2]);
            if (strcmp(args[2], "d64") == 0) {
                disk_type = DISK_IMAGE_TYPE_D64;
            } else if (strcmp(args[2], "d67") == 0) {
                disk_type = DISK_IMAGE_TYPE_D67;
            } else if (strcmp(args[2], "d71") == 0) {
                disk_type = DISK_IMAGE_TYPE_D71;
            } else if (strcmp(args[2], "d81") == 0) {
                disk_type = DISK_IMAGE_TYPE_D81;
            } else if (strcmp(args[2], "d80") == 0) {
                disk_type = DISK_IMAGE_TYPE_D80;
            } else if (strcmp(args[2], "d82") == 0) {
                disk_type = DISK_IMAGE_TYPE_D82;
            } else if (strcmp(args[2], "g64") == 0) {
                disk_type = DISK_IMAGE_TYPE_G64;
            } else if (strcmp(args[2], "g71") == 0) {
                disk_type = DISK_IMAGE_TYPE_G71;
#ifdef HAVE_X64_IMAGE
            } else if (strcmp(args[2], "x64") == 0) {
                disk_type = DISK_IMAGE_TYPE_X64;
#endif
            } else if (strcmp(args[2], "d1m") == 0) {
                disk_type = DISK_IMAGE_TYPE_D1M;
            } else if (strcmp(args[2], "d2m") == 0) {
                disk_type = DISK_IMAGE_TYPE_D2M;
            } else if (strcmp(args[2], "d4m") == 0) {
                disk_type = DISK_IMAGE_TYPE_D4M;
            } else if (strcmp(args[2], "d90") == 0) {
                disk_type = DISK_IMAGE_TYPE_D90;
            } else {
                return FD_BADVAL;
            }
            if (nargs > 4) {
                arg_to_int(args[4], &unit);
                if (check_drive_unit(unit) >= 0) {
                    dev = unit - DRIVE_UNIT_MIN;
                } else {
                    return FD_BADDEV;
                }
            } else {
                dev = 0;
            }
            if (open_image(dev, args[3], 1, disk_type) < 0) {
                return FD_BADIMAGE;
            }
            break;
        default:
            /* Shouldn't happen.  */
            return FD_BADVAL;
    }
#if 0
    printf("Unit: %i\n", unit);
#endif
    if (!strchr(args[1], ',')) {
        fprintf(stderr, "no ID given, use <name,id>\n");
        return FD_BADNAME;
    }

    if (check_drive_ready(dev) < 0) {
        return FD_NOTREADY;
    }

    command = util_concat("n:", args[1], NULL);
    charset_petconvstring((uint8_t *)command, CONVERT_TO_PETSCII);

    printf("formatting in unit %d ...\n", dev + DRIVE_UNIT_MIN);
    vdrive_command_execute(drives[dev], (uint8_t *)command,
            (unsigned int)strlen(command));

    lib_free(command);
    return FD_OK;
}

static int help_cmd(int nargs, char **args)
{
    if (nargs == 1) {
        int i;

        printf("Available commands are:\n");
        for (i = 0; command_list[i].name != NULL; i++) {
            printf("  %s\n", command_list[i].syntax);
        }
    } else {
        int match;

        match = lookup_command(args[1]);
        if (match < 0) {
            fprintf(stderr, "unknown command `%s'\n", args[1]);
        } else {
            printf("Syntax: %s\n%s\n",
                    command_list[match].syntax,
                    command_list[match].description);
        }
    }

    return FD_OK;
}

/** \brief  'info' command handler
 *
 * \param[in]   nargs   argument count
 * \param[in]   args    argument list
 *
 * \return  0 on success, < 0 on failure (`FD_BADDEV`, `FD_NOTREADY`)
 */
static int info_cmd(int nargs, char **args)
{
    vdrive_t *vdrive;
    const char *format_name;
    int dnr;

    if (nargs == 2) {
        int unit;

        if (arg_to_int(args[1], &unit) < 0) {
            return FD_BADDEV;
        }
        if (check_drive_unit(unit) < 0) {
            return FD_BADDEV;
        }
        dnr = unit - DRIVE_UNIT_MIN;
    } else {
        dnr = drive_index;
    }

    if (check_drive_ready(dnr) < 0) {
        return FD_NOTREADY;
    }

    vdrive = drives[dnr];
    format_name = image_format_name(vdrive->image_format);
    if (format_name == NULL) {
        return FD_NOTREADY; /* not quite a proper error code, but it was already
                               here in the code */
    }

    /* pretty useless (BW)
    printf("Description  : %s\n", "None.");
     */
    printf("disk format  : %s\n", format_name);
    /* printf("Sides\t   : %d.\n", hdr.sides);*/
    if (vdrive->num_tracks == vdrive->image->tracks) {
        printf("track count  : %u\n", vdrive->num_tracks);
    } else {
        printf("track count  : %u (%u tracks supported)\n", vdrive->image->tracks, vdrive->num_tracks);
    }
    if (vdrive->image->device == DISK_IMAGE_DEVICE_FS) {
        printf("error block  : %s\n",
                ((vdrive->image->media.fsimage)->error_info.map)
                ? "Yes" : "No");
    }
    printf("write protect: %s\n", vdrive->image->read_only ? "On" : "Off");

    return FD_OK;
}

#if 0
static int list_match_pattern(char *pat, char *str)
{
    int n;

    if (*str == '"') {
        str++;
    }

    if ((*str == 0) && (*pat != 0)) {
        return 0;
    }

    n = strlen(str);
    while (n) {
        n--;
        if (str[n] == '"') {
            str[n] = 0;
            break;
        }
    }

    while (*str) {
        if (*pat == '*') {
            return 1;
        } else if ((*pat != '?') && (*pat != *str)) {
            return 0;
        }
        str++; pat++;
    }
    if ((*pat != 0) && (*pat != '*')) {
        return 0;
    }
    return 1;
}
#endif


/** \brief  Match \a name and \a type against \a pattern
 *
 * Matches \a name against \a pattern and optionally against filetype \a type
 * if present in the \a pattern.
 * The pattern is the standard CBM wildcard pattern: '*' and '?' with optional
 * '=X' where X is one of S, P or R.
 *
 * \param[in]   name    filename
 * \param[in]   type    filetype
 * \param[in]   pattern wildcard pattern
 * \param]in]   plen    length of pattern
 *
 * return   bool
 */
static int match_sub_pattern(const char *name, int type,
                             const char *pattern, int plen)
{
    int n;
    int p;

#if 0
    p = 0;
    printf(".. name = '%s'\n", name);
    printf(".. subpattern = '");
    while (p < plen) {
        putchar(pattern[p]);
        p++;
    }
    printf("'\n");
#endif
    /* first check if we have a filetype specifier */
    if (plen > 2 && pattern[plen - 2] == '=') {
        if (toupper((unsigned char)(pattern[plen -1])) != type) {
            return 0;
        } else {
            /* reduce pattern size (strip off '=X') */
            plen -= 2;
        }
    }

    /* match using * and ? */
    p = 0;
    n = 1;  /* skip '"'" */
    while (p < plen && name[n] != '\0' && name[n] != '"') {
        if (pattern[p] == '*') {
            return 1;
        }
        if (pattern[p] != name[n] && pattern[p] != '?') {
            return 0;
        }
        p++;
        n++;
    }
    if (name[n] != '"' && name[n] != '\0') {
        /* leftover chars in name, but pattern had ended, no match */
        return 0;
    }
    return 1;
}



/** \brief  Test a file \a name and \a type against \a pattern
 *
 * This function can handle multiple patterns separated by commas, and supports
 * specifying a file type per sub pattern.
 *
 * Example: "foo*=p,b?r*=s", will match PRG files against "foo*" and SEQ files
 * against "b?r*". A match is found if any of the sub patterns matches (in other
 * words: OR).
 *
 * WARNING: It appears CBM DOS only allows a single file type test, use two or
 * more results in a `?FILE NOT FOUND  ERROR`. The =X specifier can also only
 * come last, anything after =X results in an error. (In C1541 this doesn't
 * matter, but in VDrive it does).
 *
 * \param[in]   name    filename
 * \param[in]   type    filetype (eg " prg<")
 * \param[in]   pattern pattern to match against
 *
 * \return  bool
 */
static int list_file_matches_pattern(const char *name,
                                     const char *type,
                                     const char *pattern)
{
    int i;
    int ftype;
    int plen;

    /* get filetype as single token */
    ftype = toupper((unsigned char)(type[1]));   /* P, S, D, R, U */

    /* pattern length */
    plen = (int)strlen(pattern);

    i = 0;
    /* check quotes */
    if (pattern[0] == '"') {
        if (pattern[plen - 1] != '"') {
            return 0;   /* unmatched quotes, fail */
        }
        i++;
    }

    while (i < plen) {
        int k = i;
        while (k < plen && pattern[k] != ',') {
            k++;
        }
        /* got sub pattern */
        if (k - i > 0) {
            if (match_sub_pattern(name, ftype, pattern + i, k - i)) {
                return 1;
            }
        } else {
            k++;    /* empty sub pattern, fail or continue */
        }
        if (pattern[k] == ',' || pattern[k] == '"') {
            k++;
        }
        i = k;
    }

    return 0;
}



/** \brief  Show directory listing of a drive
 *
 * \param[in]   nargs   number of arguments
 * \param[in]   args    argument list
 *
 * \return  0 on success, < 0 on failure (`FD_NOTREADY`)
 *
 * FIXME: diskcontents_read internally opens/closes the disk image, including
 *        a complete reset of internal vdrive variables. this makes things like
 *        changing sub partitions and sub directories inside images impossible
 *        from the c1541 shell.
 */
static int list_cmd(int nargs, char **args)
{
    char *pattern;
    char *type;
    image_contents_t *listing;
    int dnr;
    vdrive_t *vdrive;
    int unit;

/*    unsigned int drive = 0; */

    if (nargs > 1) {
        /* use new version call untill all old calls are replaced */
        unit = extract_unit_from_file_name(args[1], &pattern);
        if (unit == 0) {
            dnr = (int)drive_index;
        } else if (unit > 0) {
            dnr = (int)(unit - DRIVE_UNIT_MIN);
        } else {
            return FD_BADDEV;
        }

    } else {
        /* list */
        pattern = NULL;
        dnr = drive_index;
    }

    if (check_drive_index(dnr) < 0) {
        return FD_BADDEV;
    }

    if (check_drive_ready(dnr) < 0) {
        return FD_NOTREADY;
    }

    vdrive = drives[dnr];
/*    name = disk_image_name_get(vdrive->image); */

    listing = diskcontents_block_read(vdrive, 0);
/*    listing = diskcontents_read(name, (unsigned int)(dnr + DRIVE_UNIT_MIN), drive); */

    if (listing != NULL) {
        char *string = image_contents_to_string(listing, IMAGE_CONTENTS_STRING_ASCII);
        image_contents_file_list_t *element = listing->file_list;

        printf("%s\n", string);
        lib_free(string);
        if (element == NULL) {
            printf("Empty image\n");
        } else {
            do {
                string = image_contents_filename_to_string(element, IMAGE_CONTENTS_STRING_ASCII);
                type = image_contents_filetype_to_string(element, IMAGE_CONTENTS_STRING_ASCII);
                if ((pattern == NULL) || list_file_matches_pattern(string,
                            type, pattern)) {
                    lib_free(string);
                    string = image_contents_file_to_string(element, IMAGE_CONTENTS_STRING_ASCII);
                    printf("%s\n", string);
                    fflush(stdout);
                }
                lib_free(string);
                lib_free(type);
            } while ((element = element->next) != NULL);
        }
        if (listing->blocks_free >= 0) {
            printf("%d blocks free.\n", listing->blocks_free);
            fflush(stdout);
        }
        /* free image contents */
        image_contents_destroy(listing);
    }
    return FD_OK;
}


/** \brief  Change disk name and id
 *
 * Syntax: name "diskname[,id]" [unit]
 *
 * \param[in]   nargs   argument count
 * \param[in]   args    argument list
 *
 * \return 0 on succes, < 0 on failure (`FD_BADEV` or `FD_NOTREADY`)
 */
static int name_cmd(int nargs, char **args)
{
    char *id;
    char *name;
    uint8_t *dst;
    int i;
    int unit;
    vdrive_t *vdrive;

    if (nargs > 2) {
        if (arg_to_int(args[2], &unit) < 0) {
            return FD_BADDEV;
        }
        if (check_drive_unit(unit) < 0) {
            return FD_BADDEV;
        }
        unit -= DRIVE_UNIT_MIN;
    } else {
        unit = drive_index;
    }

    if (check_drive_ready(unit) < 0) {
        return FD_NOTREADY;
    }

    vdrive = drives[unit];
    vdrive_bam_read_bam(vdrive);
    name = args[1];
    charset_petconvstring((uint8_t *)name, CONVERT_TO_PETSCII);
    id = strrchr(args[1], ',');
    if (id) {
        *id++ = '\0';
    }

    dst = &vdrive->bam[vdrive->bam_name];
    for (i = 0; i < IMAGE_CONTENTS_NAME_LEN; i++) {
        *dst++ = (unsigned char)(*name ? *name++ : 0xa0);
    }

    if (id) {
        dst = &vdrive->bam[vdrive->bam_id];
        for (i = 0; i < IMAGE_CONTENTS_ID_LEN && *id; i++) {
            *dst++ = (unsigned char)(*id++);
        }
    }

    vdrive_bam_write_bam(vdrive);
    return FD_OK;
}

static int quit_cmd(int nargs, char **args)
{
    int i;

    for (i = 0; i < NUM_DISK_UNITS; i++) {
        if (drives[i] != NULL) {
            close_disk_image(drives[i], i + DRIVE_UNIT_MIN);
            lib_free(drives[i]);
        }

    }

    /* free all elements in args */
    for (i = 0; args[i] != NULL; i++) {
        lib_free(args[i]);
    }

    if (interactive_mode) {
        archdep_shutdown();
        log_close_all();    /* do we need this? */
        linenoiseHistoryFree();
    }
    exit(0);
    return 0;   /* OSF1 cc complains */
}


/** \brief  Be verbose - enable output of extra logging information
 *
 * If 'off' is given as an argument, verbose is turned off again.
 *
 * \param[in]   nargs   argument count
 * \param[in]   args    argument list
 *
 * \return  0
 */
static int verbose_cmd(int nargs, char **args)
{
    if (nargs >= 2 && strcmp(args[1], "off") == 0) {
        return log_set_verbose(0);
    } else {
        return log_set_verbose(1);
    }
}


/** \brief  Disable all logging
 */
static int silent_cmd(int nargs, char **args)
{
    if (nargs >= 2 && strcmp(args[1], "off") == 0) {
        return log_set_silent(0);
    } else {
        return log_set_silent(1);
    }
}

static int read_cmd(int nargs, char **args)
{
    char *src_name_petscii, *src_name_ascii;
    char *dest_name_ascii;
    char *actual_name;
    char *p;
    int dnr;
    int unit;
    FILE *outf = NULL;
    fileio_info_t *finfo = NULL;
    unsigned int format = FILEIO_FORMAT_RAW;
    uint8_t c;
    int status = 0;
    uint8_t *slot;
    uint8_t file_type;

    unit = extract_unit_from_file_name(args[1], &p);
    if (unit <= 0) {
        dnr = drive_index;
    } else {
        dnr = unit - DRIVE_UNIT_MIN;
    }

    if (check_drive_ready(dnr) < 0) {
        return FD_NOTREADY;
    }

    /* check for P00 save mode */
    if (p00save[dnr]) {
        format = FILEIO_FORMAT_P00;
    }

    if (p == NULL) {
        src_name_ascii = lib_strdup(args[1]);
    } else {
        src_name_ascii = lib_strdup(p);
    }

    if (!is_valid_cbm_file_name(src_name_ascii)) {
        fprintf(stderr, "`%s' is not a valid CBM DOS file name\n",
                src_name_ascii);
        lib_free(src_name_ascii);
        return FD_BADNAME;
    }

    src_name_petscii = lib_strdup(src_name_ascii);
    charset_petconvstring((uint8_t *)src_name_petscii, CONVERT_TO_PETSCII);

    if (vdrive_iec_open(drives[dnr], (uint8_t *)src_name_petscii,
                        (unsigned int)strlen(src_name_petscii), 0, NULL)) {
        fprintf(stderr,
                "cannot read `%s' on unit %d\n", src_name_ascii, dnr + 8);
        lib_free(src_name_ascii);
        lib_free(src_name_petscii);
        return FD_BADNAME;
    }

    /* Get real filename from the disk file.  Slot must be defined by
       vdrive_iec_open().  */
    bufferinfo_t *bufferinfo = &drives[dnr]->buffers[0];        /* 0 = secadr */
    slot = bufferinfo->slot;
    actual_name = lib_malloc(IMAGE_CONTENTS_FILE_NAME_LEN + 1);
    memcpy(actual_name, slot + SLOT_NAME_OFFSET, IMAGE_CONTENTS_FILE_NAME_LEN);
    actual_name[IMAGE_CONTENTS_FILE_NAME_LEN] = 0;

    file_type = slot[SLOT_TYPE_OFFSET] & 7;

    if (nargs == 3) {
        if (strcmp(args[2], "-") == 0) {
            dest_name_ascii = NULL;      /* stdout */
        } else {
            char *open_petscii_name;
            int reclen = slot[SLOT_RECORD_LENGTH];

            dest_name_ascii = args[2];
            open_petscii_name = lib_strdup(dest_name_ascii);
            charset_petconvstring((uint8_t *)open_petscii_name, CONVERT_TO_PETSCII);
            finfo = fileio_open(open_petscii_name, NULL, format,
                                FILEIO_COMMAND_OVERWRITE, file_type, &reclen);
            lib_free(open_petscii_name);
        }
    } else {
        int reclen = slot[SLOT_RECORD_LENGTH];
        size_t l;

        dest_name_ascii = actual_name;
        vdrive_dir_no_a0_pads((uint8_t *)dest_name_ascii, CBMDOS_SLOT_NAME_LENGTH);
        l = strlen(dest_name_ascii) - 1;
        while (dest_name_ascii[l] == ' ') {
            dest_name_ascii[l] = 0;
            l--;
        }
        /* remove illegal chars from output filename */
        archdep_sanitize_filename(dest_name_ascii);

        finfo = fileio_open(dest_name_ascii, NULL, format,
                            FILEIO_COMMAND_OVERWRITE, file_type, &reclen);
    }

    if (dest_name_ascii == NULL) {
        outf = stdout;
    } else {
        if (finfo == NULL) {
            fprintf(stderr, "cannot create output file `%s': %s\n",
                    dest_name_ascii, strerror(errno));
            vdrive_iec_close(drives[dnr], 0);
            lib_free(src_name_petscii);
            lib_free(src_name_ascii);
            lib_free(actual_name);
            return FD_NOTWRT;
        }
    }                           /* stdout */

    printf("reading file `%s' from unit %d\n", src_name_ascii, dnr + DRIVE_UNIT_MIN);

    if (file_type == CBMDOS_FT_REL) {
        unsigned int rel_record_length = slot[SLOT_RECORD_LENGTH];
        unsigned int num_rel_records = bufferinfo->record_max;
        unsigned int record;

        for (record = 1; record <= num_rel_records; record++) {
            int bytes = 0;
            do {
                status = vdrive_iec_read(drives[dnr], &c, 0);
                if (status == SERIAL_ERROR && bytes == 0) {
                    fprintf(stderr, "dummy record CR; should not happen.\n");
                    break; /* record after EOF; we get a dummy CR */
                }
                if (dest_name_ascii == NULL) {
                    fputc(c, outf);
                } else {
                    fileio_write(finfo, &c, 1);
                }
                bytes++;
            } while (status == SERIAL_OK);

            /*
             * Pad REL records to full record length.
             * For the last record the drive may in theory know how many
             * bytes are really used (rather than relying on 0-padding),
             * but it has no way of letting the user know.
             */
            c = 0;
            while (bytes < rel_record_length) {
                if (dest_name_ascii == NULL) {
                    fputc(c, outf);
                } else {
                    fileio_write(finfo, &c, 1);
                }
                bytes++;
            }
        }
    } else {
        do {
            status = vdrive_iec_read(drives[dnr], &c, 0);
            if (dest_name_ascii == NULL) {
                fputc(c, outf);
            } else {
                fileio_write(finfo, &c, 1);
            }
        } while (status == SERIAL_OK);
    }

    if (dest_name_ascii != NULL) {
        fileio_close(finfo);
    }

    vdrive_iec_close(drives[dnr], 0);

    lib_free(src_name_petscii);
    lib_free(src_name_ascii);
    lib_free(actual_name);

    return FD_OK;
}

#define SLOT_GEOS_FILE_STRUC  23  /* Offset to geos file structure byte */
#define SLOT_GEOS_FILE_TYPE   24  /* Offset to geos file type
          */

/* Geos file structure sequential (no vlir) */
#define GEOS_FILE_STRUC_SEQ   0
/* Geos file structure VLIR (index block, max. 127 chains) */
#define GEOS_FILE_STRUC_VLIR  1

/* Author:      DiSc
 * Date:        2000-07-28
 *
 * companion to geos_read_cmd.
 * Expects an opened file in drives[unit]->buffers[0].slot
 */
int internal_read_geos_file(int unit, FILE* outf, char* src_name_ascii)
{
    unsigned int n;

    /* Get TS of info block */
    uint8_t infoTrk = drives[unit]->buffers[0].slot[SLOT_SIDE_TRACK];
    uint8_t infoSec = drives[unit]->buffers[0].slot[SLOT_SIDE_SECTOR];

    /* Get TS of first data block or vlir block
       (depends on the geos file type) */
    uint8_t firstTrk = drives[unit]->buffers[0].slot[SLOT_FIRST_TRACK];
    uint8_t firstSec = drives[unit]->buffers[0].slot[SLOT_FIRST_SECTOR];

    /* get geos file structure and geos file type */
    uint8_t geosFileStruc = drives[unit]->buffers[0].slot[SLOT_GEOS_FILE_STRUC];
    /*uint8_t geosFileType = drives[unit]->buffers[0].slot[SLOT_GEOS_FILE_TYPE];*/

    uint8_t infoBlock[256];
    uint8_t vlirBlock[256];
    uint8_t block[256];
    uint8_t vlirTrans[256];

    unsigned int aktTrk, aktSec, vlirIdx, NoOfBlocks, NoOfChains, BytesInLastSector;

    /* the first block in a cvt file is the directory entry padded with
       zeros */
    for (n = 2; n < 32; n++) {
        uint8_t c = drives[unit]->buffers[0].slot[n];
        fputc(c, outf);
    }
    /* signature */
    fprintf(outf, "%s formatted GEOS file V1.0", (geosFileStruc == GEOS_FILE_STRUC_SEQ) ? "SEQ" : "PRG");
    /* pad with zeros */
    for (n = 0x3a; n < 0xfe; n++) {
        fputc(0, outf);
    }

    /* read info block */
    if (vdrive_read_sector(drives[unit], infoBlock, infoTrk, infoSec) != 0) {
        fprintf(stderr,
                "cannot read input file info block `%s': %s\n",
                src_name_ascii, strerror(errno));
        return FD_RDERR;
    }
    /* the next block is the info sector
     * write info block
     */
    for (n = 2; n < 256; n++) {
        fputc(infoBlock[n], outf);
    }

    /* read first data block or vlir block */
    if (vdrive_read_sector(drives[unit], vlirBlock, firstTrk, firstSec) != 0) {
        fprintf(stderr,
                "cannot read input file data `%s': %s\n",
                src_name_ascii, strerror(errno));
        return FD_RDERR;
    }

    if (geosFileStruc == GEOS_FILE_STRUC_SEQ) {
#ifdef DEBUG_DRIVE
        log_debug("DEBUG: GEOS_FILE_STRUC_SEQ (%d:%d)", infoTrk, infoSec);
#endif
        /* sequential file contained in cvt file
         * since vlir block is the first data block simply put it to
         * disk
         */

        for (n = 2; n < 256; n++) {
            fputc(vlirBlock[n], outf);
        }

        /* the rest is like standard cbm file TS-Chains.
           Put them to disk */

        aktTrk = vlirBlock[0];
        aktSec = vlirBlock[1];
        while (aktTrk != 0) {
            if (vdrive_read_sector(drives[unit], block, aktTrk, aktSec) != 0) {
                fprintf(stderr,
                        "cannot read input file data block `%s': %s\n",
                        src_name_ascii, strerror(errno));
                return FD_RDERR;
            }
            aktTrk = block[0];
            aktSec = block[1];
            BytesInLastSector = aktTrk != 0 ? 256 : aktSec + 1;
            for (n = 2; n < BytesInLastSector; n++) {
                fputc(block[n], outf);
            }
        }
    } else if (geosFileStruc == GEOS_FILE_STRUC_VLIR) {
#ifdef DEBUG_DRIVE
        log_debug("DEBUG: GEOS_FILE_STRUC_VLIR (%d:%d)", infoTrk, infoSec);
#endif
        /* The vlir block in cvt files is a conversion of the vlir
         * block on cbm disks.
         * Every non empty or non existent TS pointer is replaced with
         * (NoOfBlocks in Record, Bytes in last Record block).
         * Therefore the vlirBlock
         * is translated to vlirTrans according this rule.
         *
         * Copy vlir block to vlirTrans
         */
        for (n = 2; n < 256; n++) {
            vlirTrans[n] = vlirBlock[n];
        }

#ifdef DEBUG_DRIVE
        log_debug("DEBUG: VLIR scan record chains");
#endif

        /* Replace the TS-chain-origins with NoOfBlocks/BytesInLastSector */

        vlirIdx = 2;
        aktTrk = vlirBlock[vlirIdx];
        aktSec = vlirBlock[vlirIdx + 1];
        NoOfBlocks = 0;
        NoOfChains = 0;
        BytesInLastSector = 255;
        while (aktTrk != 0 && vlirIdx <= 254) {
            if (aktTrk != 0) { /* Record exists and is not empty */
#ifdef DEBUG_DRIVE
                log_debug("DEBUG: VLIR IDX %u", vlirIdx);
#endif
                NoOfChains++;
                while (aktTrk != 0) {
                    /* Read the chain and collect No Of Blocks */
#ifdef DEBUG_DRIVE
                    log_debug("DEBUG: VLIR BLOCK (%u:%u)", aktTrk, aktSec);
#endif

                    if (vdrive_read_sector(drives[unit], block, aktTrk, aktSec) != 0) {
                        fprintf(stderr,
                                "cannot read input file data block `%s': %s\n",
                                src_name_ascii, strerror(errno));
                        return FD_RDERR;
                    }

                    /* Read TS for next sector*/

                    aktTrk = block[0];
                    aktSec = block[1];

                    if (aktTrk != 0) { /* Next Sector exists */
                        NoOfBlocks++;
                    } else { /* Current was last sector of chain */
                        NoOfBlocks++;
                        BytesInLastSector = aktSec;
                    }
                }
            }
            if (NoOfBlocks != 0) {
                /* Entries for empty or non existing entries
                 * in vlir block are the same in cvt and vlir files */
                vlirTrans[vlirIdx] = (unsigned char)NoOfBlocks;
                vlirTrans[vlirIdx + 1] = (unsigned char)BytesInLastSector;
            }

            /* Prepare for next loop iteration */

            vlirIdx += 2;
            if (vlirIdx <= 254) {
                aktTrk = vlirBlock[vlirIdx];
                aktSec = vlirBlock[vlirIdx + 1];
            }
            NoOfBlocks = 0;
            BytesInLastSector = 255;
        }

        /* output transformed vlir block */

        for (n = 2; n < 256; n++) {
            fputc(vlirTrans[n], outf);
        }

#ifdef DEBUG_DRIVE
        log_debug("DEBUG: VLIR output record chains");
#endif
        /* now output the record chains
           (leave the TS-Pointers since they are usesless now) */

        vlirIdx = 2;
        aktTrk = vlirBlock[vlirIdx];
        aktSec = vlirBlock[vlirIdx + 1];
        while (aktTrk != 0 && vlirIdx <= 254) {
            if (aktTrk != 0) {
#ifdef DEBUG_DRIVE
                log_debug("DEBUG: VLIR IDX %u", vlirIdx);
#endif
                NoOfChains--;
                /* Record exists */
                while (aktTrk != 0) {
#ifdef DEBUG_DRIVE
                    log_debug("DEBUG: VLIR BLOCK (%u:%u)", aktTrk, aktSec);
#endif
                    if (vdrive_read_sector(drives[unit], block, aktTrk, aktSec) != 0) {
                        fprintf(stderr,
                                "cannot read input file data block `%s': %s\n",
                                src_name_ascii, strerror(errno));
                        return FD_RDERR;
                    }
                    aktTrk = block[0];
                    aktSec = block[1];
                    BytesInLastSector = ((NoOfChains != 0) || (aktTrk != 0)) ? 256 : aktSec + 1;
                    for (n = 2; n < BytesInLastSector; n++) {
                        fputc(block[n], outf);
                    }
                }
            }
            vlirIdx += 2;
            if (vlirIdx <= 254) {
                aktTrk = vlirBlock[vlirIdx];
                aktSec = vlirBlock[vlirIdx + 1];
            }
        }
    } else {
        fprintf(stderr, "unknown GEOS-File structure\n");
        return FD_RDERR;
    }
    return FD_OK;
}

/* Author:      DiSc
 * Date:        2000-07-28
 * Reads a geos file from the diskimage and writes it to a convert file
 * This code was copied from the write_cmd function.
 *
 *
 * This function was completely broken and assumed GEOS files are PRG files
 * with PETSCII filenames. So when people complain this command doesnt work
 * anymore, they have incorrect GEOS files.
 *
 * Also note that the directory display routine of c1541 assumes PETSCII file
 * names and as such the GEOS file names are display with their case inverted,
 * so to extract a file called 'rEADmE' in the directory, use 'ReadMe' to
 * extract it.
 *
 * -- compyx, 2018-05-18
 */
static int read_geos_cmd(int nargs, char **args)
{
    char *src_name_ascii;
    char *dest_name_ascii;
    char *actual_name;
    char *p;
    FILE *outf;
    int err_code;
    int dev;
    int unit;
    cbmdos_cmd_parse_t *parse_cmd;
    size_t namelen;


    unit = extract_unit_from_file_name(args[1], &p);
    if (unit > 0) {
        dev = unit - DRIVE_UNIT_MIN;
    } else if (unit == 0) {
        /* no @<unit>: found */
        dev = drive_index;
        unit = drive_index + DRIVE_UNIT_MIN;
    } else {
        /* -1, invalid unit number */
        return FD_BADDEV;
    }

    if (check_drive_ready(dev) < 0) {
        return FD_NOTREADY;
    }

    if (p == NULL || *p == '\0') {
        fprintf(stderr,
                "missing filename\n");
        return FD_BADNAME;
    } else {
        src_name_ascii = lib_strdup(p);
    }

    if (!is_valid_cbm_file_name(src_name_ascii)) {
        fprintf(stderr,
                "`%s' is not a valid CBM DOS file name\n", src_name_ascii);
        lib_free(src_name_ascii);
        return FD_BADNAME;
    }


    /*
     * We use this to pass to vdrive_iec_open() as its `cmd_parse_ext` argument
     * to tell the function to look for USR files. Without this the function
     * defaults to looking for PRG files and will fail to locate the GEOS file
     * requested.
     */
    namelen = strlen(src_name_ascii);
    parse_cmd = lib_calloc(1, sizeof *parse_cmd);
    parse_cmd->cmd = (const uint8_t *)src_name_ascii;
    parse_cmd->cmdlength = (unsigned int)namelen;
    parse_cmd->parsecmd = lib_strdup(src_name_ascii); /* freed in
                                                           vdrive_iec_open() */
    parse_cmd->parselength = (unsigned int)namelen;
    parse_cmd->secondary = 0;
    parse_cmd->filetype = CBMDOS_FT_USR;
    parse_cmd->readmode = CBMDOS_FAM_READ;

    if (vdrive_iec_open(drives[dev], (uint8_t *)src_name_ascii,
                        (unsigned int)strlen(src_name_ascii), 0,
                        parse_cmd)) {
        fprintf(stderr,
                "cannot read `%s' on unit %d\n", src_name_ascii, unit);
        lib_free(src_name_ascii);
        lib_free(parse_cmd);
        return FD_BADNAME;
    }

    lib_free(parse_cmd);

    /* Get real filename from the disk file.
       Slot must be defined by vdrive_iec_open().  */
    actual_name = lib_malloc(CBMDOS_SLOT_NAME_LENGTH + 1);
    memcpy(actual_name,
            drives[dev]->buffers[0].slot + SLOT_NAME_OFFSET,
            CBMDOS_SLOT_NAME_LENGTH);
    actual_name[CBMDOS_SLOT_NAME_LENGTH] = '\0';

    if (nargs == 3) {
        dest_name_ascii = args[2];
    } else {
        int l;

        dest_name_ascii = actual_name;
        vdrive_dir_no_a0_pads((uint8_t *) dest_name_ascii, CBMDOS_SLOT_NAME_LENGTH);
        l = (int)strlen(dest_name_ascii) - 1;
        while (dest_name_ascii[l] == ' ') {
            dest_name_ascii[l] = 0;
            l--;
        }
        /*
         * Don't convert, GEOS uses ASCII
         */
#if 0
        charset_petconvstring((uint8_t *)dest_name_ascii, CONVERT_TO_ASCII);
#endif
    }

    outf = fopen(dest_name_ascii, MODE_WRITE);
    if (outf == NULL) {
        fprintf(stderr,
                "cannot create output file `%s': %s\n",
                dest_name_ascii, strerror(errno));
        vdrive_iec_close(drives[dev], 0);
        lib_free(src_name_ascii);
        lib_free(actual_name);
        return FD_NOTWRT;
    }

    printf("reading file `%s' from unit %d\n", src_name_ascii, unit);

    err_code = internal_read_geos_file(dev, outf, src_name_ascii);

    fclose(outf);
    vdrive_iec_close(drives[dev], 0);

    lib_free(src_name_ascii);
    lib_free(actual_name);

    return err_code;
}

/* Author: DiSc
 * Date:   2000-07-28
 * Put the NTS-Chain on a block. Can also be a vlir block if blk_offset is > 1
 */
static int fix_ts(int unit, unsigned int trk, unsigned int sec,
                  unsigned int next_trk, unsigned int next_sec,
                  unsigned int blk_offset)
{
    uint8_t block[256];
    if (vdrive_read_sector(drives[unit], block, trk, sec) == 0) {
        block[blk_offset] = (unsigned char)next_trk;
        block[blk_offset + 1] = (unsigned char)next_sec;
        if (vdrive_write_sector(drives[unit], block, trk, sec) == 0) {
            return 1;
        }
    }
    return 0;
}

/* Author:      DiSc
 * Date:        2000-07-28
 * Companion to write_geos_cmd. Expects an file opened for writing in
 * drives[unit]->buffers[1].slot
 */
static int internal_write_geos_file(int unit, FILE* f)
{
    uint8_t dirBlock[256];
    uint8_t infoBlock[256];
    uint8_t vlirBlock[256];
    uint8_t block[256];
    unsigned int vlirTrk, vlirSec;
    unsigned int aktTrk, aktSec;
    unsigned int lastTrk, lastSec;
    uint8_t geosFileStruc;
    int c = 0;
    unsigned int n;
    int bContinue;
    int numBlks, bytesInLastBlock;

    /* First block of cvt file is the directory entry, rest padded with zeros */

    for (n = 2; n < 256; n++) {
        c = fgetc(f);
        dirBlock[n] = (unsigned char)c;
    }

    /* copy to the already created slot */

    for (n = 2; n < 32; n++) {
        drives[unit]->buffers[1].slot[n] = dirBlock[n];
    }

    geosFileStruc = drives[unit]->buffers[1].slot[SLOT_GEOS_FILE_STRUC];

    /* next is the geos info block */

    infoBlock[0] = 0;
    infoBlock[1] = 0xFF;
    for (n = 2; n < 256; n++) {
        c = fgetc(f);
        infoBlock[n] = (unsigned char)c;
    }

    /* put it on disk */

    if (vdrive_bam_alloc_first_free_sector(drives[unit], &vlirTrk, &vlirSec) < 0) {
        fprintf(stderr, "disk full\n");
        return FD_WRTERR;
    }
    if (vdrive_write_sector(drives[unit], infoBlock, vlirTrk, vlirSec) != 0) {
        fprintf(stderr, "disk full\n");
        return FD_WRTERR;
    }

    /* and put the blk/sec in the dir entry */

    drives[unit]->buffers[1].slot[SLOT_SIDE_TRACK] = (unsigned char)vlirTrk;
    drives[unit]->buffers[1].slot[SLOT_SIDE_SECTOR] = (unsigned char)vlirSec;

    /* now read the first data block. if its a vlir-file its the vlir index
     * block
     * else its already a data block */
    for (n = 2; n < 256; n++) {
        c = fgetc(f);
        vlirBlock[n] = (unsigned char)c;
    }

    /* the vlir index block always has a NTS-chain of (0, FF) */

    if (geosFileStruc == GEOS_FILE_STRUC_VLIR) {
        vlirBlock[0] = 0;
        vlirBlock[1] = 0xFF;
    }

    /* write the block */

    if (vdrive_bam_alloc_next_free_sector(drives[unit], &vlirTrk, &vlirSec) < 0) {
        fprintf(stderr, "disk full\n");
        return FD_WRTERR;
    }

    if (vdrive_write_sector(drives[unit], vlirBlock, vlirTrk, vlirSec) != 0) {
        fprintf(stderr, "disk full\n");
        return FD_WRTERR;
    }

    /* put the TS in the dir entry */

    drives[unit]->buffers[1].slot[SLOT_FIRST_TRACK] = (unsigned char)vlirTrk;
    drives[unit]->buffers[1].slot[SLOT_FIRST_SECTOR] = (unsigned char)vlirSec;

    if (geosFileStruc == GEOS_FILE_STRUC_SEQ) {
#ifdef DEBUG_DRIVE
        log_debug("DEBUG: GEOS_FILE_STRUC_SEQ (%u:%u)", vlirTrk, vlirSec);
#endif
        /* normal seq file (rest like standard files) */
        lastTrk = vlirTrk;
        lastSec = vlirSec;
        aktTrk = vlirTrk;
        aktSec = vlirSec;
        bContinue = (vlirBlock[0] != 0);
        while (bContinue) {
            block[0] = 0;
            block[1] = 255;

            /* read next block */

            for (n = 2; n < 256; n++) {
                c = fgetc(f);
                if (c == EOF) {
                    block[0] = 0;
                    block[1] = (unsigned char)(n - 1);
                    while (n < 256) {
                        block[n++] = 0x00;
                    }
                    bContinue = 0;
                    break;
                }
                block[n] = (unsigned char)c;
            }

            /* allocate it */

            if (vdrive_bam_alloc_next_free_sector(drives[unit], &aktTrk, &aktSec) < 0) {
                fprintf(stderr, "disk full\n");
                return FD_WRTERR;
            }

            /* write it to disk */

            if (vdrive_write_sector(drives[unit], block, aktTrk, aktSec) != 0) {
                fprintf(stderr, "disk full\n");
                return FD_WRTERR;
            }

            /* put the TS of the current block to the predecessor block */

            if (!fix_ts(unit, lastTrk, lastSec, aktTrk, aktSec, 0)) {
                fprintf(stderr, "internal error\n");    /* not clear at all */
                return FD_WRTERR;
            }
            lastTrk = aktTrk;
            lastSec = aktSec;
        }
    } else if (geosFileStruc == GEOS_FILE_STRUC_VLIR) {
#ifdef DEBUG_DRIVE
        log_debug("DEBUG: GEOS_FILE_STRUC_VLIR (%u:%u)", vlirTrk, vlirSec);
#endif
        /* in a cvt file containing a vlir file the vlir block contains
         * a pair (NoOfBlocksForChain, BytesInLastBlock + 2) for every vlir
         * record that exists. Non-existing record have a (0, 0) pair,
         * empty records a (0, 255) pair. There can be empty or non
         * existing records between data records.
         */

        int vlirIdx = 2;
        while (vlirIdx <= 254) {
            if (vlirBlock[vlirIdx] != 0) {
#ifdef DEBUG_DRIVE
                log_debug("DEBUG: VLIR IDX %d (%u:%u)", vlirIdx, vlirTrk, vlirSec);
#endif
                lastTrk = vlirTrk;
                lastSec = vlirSec;
                aktTrk = vlirTrk;
                aktSec = vlirSec;
                numBlks = vlirBlock[vlirIdx];
                /* How many blocks in record */
                bytesInLastBlock = vlirBlock[vlirIdx + 1];
                /* Bytes in last blocks */
                while (numBlks > 0) {
                    /* read next block */

                    block[0] = 0;
                    block[1] = 0xFF;
                    for (n = 2; n < 256; n++) {
                        c = fgetc(f);
                        if (c == EOF) {
                            if (numBlks > 1) {
                                fprintf(stderr, "unexpected EOF encountered\n");
                                return FD_RDERR;
                            }
                            while (n < 256) {
                                block[n++] = 0x00;
                            }
                            break;
                        } else {
                            block[n] = (unsigned char)c;
                        }
                    }
                    if (numBlks == 1) { /* last block */
                        block[0] = 0;
                        block[1] = (unsigned char)bytesInLastBlock;
                    }

                    /* allocate it */

                    if (vdrive_bam_alloc_next_free_sector(drives[unit], &aktTrk, &aktSec) < 0) {
                        fprintf(stderr, "disk full\n");
                        return FD_WRTERR;
                    }

                    /* write it to disk */
#ifdef DEBUG_DRIVE
                    log_debug("DEBUG: VLIR BLOCK (%u:%u)", aktTrk, aktSec);
#endif

                    if (vdrive_write_sector(drives[unit], block, aktTrk, aktSec) != 0) {
                        fprintf(stderr, "disk full\n");
                        return FD_WRTERR;
                    }
                    /*
                     * write the TS to the predecessor block or if this is
                     * the first block of a vlir record write it to the vlir
                     * index block at the correct offset.
                     */
                    if (!fix_ts(unit, lastTrk, lastSec, aktTrk, aktSec,
                                (unsigned int)(lastTrk == vlirTrk
                                    && lastSec == vlirSec
                                    ? vlirIdx : 0))) {
                        fprintf(stderr, "internal error\n");
                        return FD_WRTERR;
                    }
                    lastTrk = aktTrk;
                    lastSec = aktSec;
                    numBlks--;
                }
            }
            vlirIdx += 2;
        }
    }
    return FD_OK;
}

/* do some sanity checks, reject files that are clearly not valid convert files */
static int check_geos_convert_file(FILE *f)
{
    uint8_t dirBlock[256];
    int c = 0;
    int n;

    /* First block of cvt file is the directory entry, rest padded with zeros */
    for (n = 0; n < 254; n++) {
        c = fgetc(f);
        dirBlock[n] = (unsigned char)c;
    }

    /* check if this is a valid convert file */
    if (((memcmp(&dirBlock[0x1e], "PRG", 3) != 0) && (memcmp(&dirBlock[0x1e], "SEQ", 3) != 0)) ||
        (memcmp(&dirBlock[0x21], " formatted GEOS file V1.0", 25) != 0)) {
        fprintf(stderr, "not a valid GEOS convert file\n");
        return -1;
    }

    /* reset file position */
    fseek(f, 0, SEEK_SET);

    return 0;
}

/* Author:      DiSc
 * Date:        2000-07-28
 * Write a geos .cvt file to the diskimage
 * This code was copied from the write_cmd function.
 */
static int write_geos_cmd(int nargs, char **args)
{
    int dev;
    int erg;
    char *dest_name_ascii, *dest_name_petscii;
    FILE *f;
    uint8_t* e;
    char *slashp;
    vdrive_dir_context_t dir;

    /* geoswrite <source> */
    dest_name_ascii = NULL;
    dev = drive_index;

    if (check_drive_ready(dev) < 0) {
        return FD_NOTREADY;
    }

    f = fopen(args[1], MODE_READ);
    if (f == NULL) {
        fprintf(stderr,
                "cannot read file `%s': %s\n", args[1], strerror(errno));
        return FD_NOTRD;
    }

    if (check_geos_convert_file(f) < 0) {
        return FD_NOTRD;
    }

    /* User did not specify a destination name...  Let's try to make an
       educated guess at what she expects.

       Convert file have a directory and info block which contains the
       resulting filename. Because i am using the vdrive functions to create
       a directory entry lets get a temporary name out of the source file for
       initial creating of the USR file.
     */

    slashp = strrchr(args[1], '/');
    if (slashp == NULL) {
        dest_name_ascii = lib_strdup(args[1]);
    } else {
        dest_name_ascii = lib_strdup(slashp + 1);
    }
    dest_name_petscii = lib_strdup(dest_name_ascii);
    charset_petconvstring((uint8_t *)dest_name_petscii, CONVERT_TO_PETSCII);

    if (vdrive_iec_open(drives[dev], (uint8_t *)dest_name_petscii,
                        (unsigned int)strlen(dest_name_petscii), 1, NULL)) {
        fprintf(stderr, "cannot open `%s' for writing on image\n",
                dest_name_ascii);
        fclose(f);
        return FD_WRTERR;
    }

    /* the following function reuses the fresh created dir entry ... */
    erg = internal_write_geos_file(dev, f);
    fclose(f);

    /* Start: copied from vdrive_iec_close
     * The bam and directory entry must be copied to the disk. the code
     * from the vdrive routines does that thing.
     */
    vdrive_dir_find_first_slot(drives[dev], (const uint8_t *)dest_name_petscii,
                               (int)strlen(dest_name_petscii), 0, &dir);
    e = vdrive_dir_find_next_slot(&dir);

    if (!e) {
        drives[dev]->buffers[1].mode = BUFFER_NOT_IN_USE;
        lib_free(drives[dev]->buffers[1].buffer);
        drives[dev]->buffers[1].buffer = NULL;

        vdrive_command_set_error(drives[dev], CBMDOS_IPE_DISK_FULL, 0, 0);
        return SERIAL_ERROR;
    }
    /* The buffer MUST be mark as closed to avoid the vdrive functions
     * to add an empty block at the end of the file that was allocated
     * when the file was created!!!!
     */
    drives[dev]->buffers[1].slot[SLOT_TYPE_OFFSET] |= 0x80; /* Closed */

    memcpy(&dir.buffer[dir.slot * SLOT_SIZE + 2],
           drives[dev]->buffers[1].slot + 2,
           30);

#ifdef DEBUG_DRIVE
    log_debug("DEBUG: closing, write DIR slot (%u %u) and BAM.",
            dir.track, dir.sector);
#endif
    vdrive_write_sector(drives[dev], dir.buffer, dir.track, dir.sector);
    vdrive_bam_write_bam(drives[dev]);
    drives[dev]->buffers[1].mode = BUFFER_NOT_IN_USE;
    lib_free((char *)drives[dev]->buffers[1].buffer);
    drives[dev]->buffers[1].buffer = NULL;

    /* End: copied from vdrive_iec_close */

    lib_free(dest_name_ascii);
    lib_free(dest_name_petscii);

    return erg;
}


/** \brief  Rename a file
 *
 * \param[in]   nargs   argument count
 * \param[in]   args    argument list
 *
 * \return  FD_OK on success, < 0 on failure
 */
static int rename_cmd(int nargs, char **args)
{
    char *src_name;
    char *dest_name;
    int src_unit;
    int dest_unit;
    int dev;
    char *command;
    char *p;
    int unit;

    unit = extract_unit_from_file_name(args[1], &p);
    if (unit > 0) {
        src_unit = unit;
    } else if (unit == 0) {
        src_unit = drive_index + DRIVE_UNIT_MIN;
    } else {
        return FD_BADDEV;
    }
    src_name = lib_strdup(p);


    unit = extract_unit_from_file_name(args[2], &p);
    if (unit > 0) {
        dest_unit = unit;
    } else if (unit == 0) {
        dest_unit = drive_index + DRIVE_UNIT_MIN;
    } else {
        return FD_BADDEV;
    }
    dest_name = lib_strdup(p);

    dev = dest_unit - DRIVE_UNIT_MIN;

    if (dest_unit != src_unit) {
        fprintf(stderr, "source and destination must be on the same unit\n");
        lib_free(src_name);
        lib_free(dest_name);
        return FD_BADDEV;
    }

    if (check_drive_ready(dev) < 0) {
        lib_free(src_name);
        lib_free(dest_name);
        return FD_NOTREADY;
    }

    if (!is_valid_cbm_file_name(src_name)) {
        fprintf(stderr, "`%s' is not a valid CBM DOS file name\n", src_name);
        lib_free(src_name);
        lib_free(dest_name);
        return FD_BADNAME;
    }

    if (!is_valid_cbm_file_name(dest_name)) {
        fprintf(stderr, "`%s' is not a valid CBM DOS file name\n", dest_name);
        lib_free(src_name);
        lib_free(dest_name);
        return FD_BADNAME;
    }

    printf("renaming `%s' to `%s'\n", src_name, dest_name);

    command = util_concat("r:", dest_name, "=", src_name, NULL);
    charset_petconvstring((uint8_t *)command, CONVERT_TO_PETSCII);

    vdrive_command_execute(drives[dev],
                           (uint8_t *)command, (unsigned int)strlen(command));

    lib_free(command);
    lib_free(dest_name);
    lib_free(src_name);

    return FD_OK;
}


#if 0
/** \brief  Show license or warranty information
 *
 * Syntax: show copying|warranty
 *
 * \param[in]   nargs   argument count
 * \param[in]   args    argument list
 *
 * \returns 0
 */
static int show_cmd(int nargs, char **args)
{
    if (util_strcasecmp(args[1], "copying") == 0) {
        printf("%s", info_license_text);
    } else if (util_strcasecmp(args[1], "warranty") == 0) {
        printf("%s", info_warranty_text);
    } else {
        fprintf(stderr, "Use either `show copying' or `show warranty'\n");
        return FD_OK;           /* FIXME? */
    }

    return FD_OK;
}
#endif

/** \brief  Copy files from a tape image to current drive unit
 *
 * \param[in]   nargs   argument count
 * \param[in]   args    argument list
 *
 * \return  FD_OK on success, < 0 on failure
 */
static int tape_cmd(int nargs, char **args)
{
    tape_image_t *tape_image;
    vdrive_t *drive;
    int count;

    if (check_drive_ready(drive_index) < 0) {
        return FD_NOTREADY;
    }

    drive = drives[drive_index];

    tape_image = tape_internal_open_tape_image(args[1], 1);

    if (tape_image == NULL) {
        fprintf(stderr, "cannot read tape file `%s'\n", args[1]);
        return FD_BADNAME;
    }

    for (count = 0; tape_seek_to_next_file(tape_image, 0) >= 0; ) {
        tape_file_record_t *rec;

        rec = tape_get_current_file_record(tape_image);

        if (rec->type) {
            char *dest_name_ascii;
            char *dest_name_petscii;
            uint8_t *buf;
            size_t name_len;
            uint16_t file_size;
            int i;
            int retval;

            /* Ignore traling spaces and 0xa0's.  */
            name_len = strlen((char *)(rec->name));
            while (name_len > 0 && (rec->name[name_len - 1] == 0xa0
                                    || rec->name[name_len - 1] == 0x20)) {
                name_len--;
            }

            dest_name_petscii = lib_calloc(1, name_len + 1);
            memcpy(dest_name_petscii, rec->name, name_len);

            dest_name_ascii = lib_calloc(1, name_len + 1);
            memcpy(dest_name_ascii, dest_name_petscii, name_len);
            charset_petconvstring((uint8_t *)dest_name_ascii, CONVERT_TO_ASCII);

            if (nargs > 2) {
                int k;
                int found;

                for (k = 2, found = 0; k < nargs; k++) {
                    if (name_len == strlen(args[k])
                        && memcmp(args[k], dest_name_ascii, name_len) == 0) {
                        found = 1;
                        break;
                    }
                }

                if (!found) {
                    continue;
                }
            }

            if (rec->type == 1 || rec->type == 3) {
                if (vdrive_iec_open(drive, (uint8_t *)dest_name_petscii,
                                    (unsigned int)name_len, 1, NULL)) {
                    fprintf(stderr,
                            "cannot open `%s' for writing on drive %d\n",
                            dest_name_ascii, drive_index + 8);
                    lib_free(dest_name_petscii);
                    lib_free(dest_name_ascii);
                    continue;
                }

                fprintf(stderr, "writing `%s' ($%04X - $%04X) to drive %d\n",
                        dest_name_ascii, rec->start_addr, rec->end_addr,
                        drive_index + 8);

                vdrive_iec_write(drive, ((uint8_t)(rec->start_addr & 0xff)), 1);
                vdrive_iec_write(drive, ((uint8_t)(rec->start_addr >> 8)), 1);

                file_size = (uint16_t)(rec->end_addr - rec->start_addr);

                buf = lib_calloc((size_t)file_size, 1);

                retval = tape_read(tape_image, buf, file_size);

                if (retval < 0 || retval != (int) file_size) {
                    fprintf(stderr,
                            "unexpected end of tape: file may be truncated\n");
                }

                for (i = 0; i < file_size; i++) {
                    if (vdrive_iec_write(drives[drive_index],
                                         ((uint8_t)(buf[i])), 1)) {
                        tape_internal_close_tape_image(tape_image);
                        lib_free(dest_name_petscii);
                        lib_free(dest_name_ascii);
                        lib_free(buf);
                        return FD_WRTERR;
                    }
                }

                lib_free(buf);
            } else if (rec->type == 4) {
                uint8_t b;
                char *dest_name_plustype;
                dest_name_plustype = util_concat(dest_name_petscii, ",S,W", NULL);
                retval = vdrive_iec_open(drive, (uint8_t *)dest_name_plustype,
                                         (unsigned int)name_len + 4, 2, NULL);
                lib_free(dest_name_plustype);

                if (retval) {
                    fprintf(stderr,
                            "cannot open `%s' for writing on drive %d\n",
                            dest_name_ascii, drive_index + 8);
                    lib_free(dest_name_petscii);
                    lib_free(dest_name_ascii);
                    continue;
                }

                fprintf(stderr, "writing SEQ file `%s' to drive %d\n",
                        dest_name_ascii, drive_index + 8);

                do {
                    retval = tape_read(tape_image, &b, 1);

                    if (retval < 0) {
                        fprintf(stderr,
                                "unexpected end of tape: file may be truncated\n");
                        break;
                    } else if (vdrive_iec_write(drives[drive_index], b, 2)) {
                        tape_internal_close_tape_image(tape_image);
                        lib_free(dest_name_petscii);
                        lib_free(dest_name_ascii);
                        return FD_WRTERR;
                    }
                } while (retval == 1);
            }

            vdrive_iec_close(drive, 1);
            lib_free(dest_name_petscii);
            lib_free(dest_name_ascii);
            count++;
        }
    }

    tape_internal_close_tape_image(tape_image);

    printf("\n%d files copied\n", count);

    return FD_OK;
}


/** \brief  Select current device
 *
 * \param[in]   nargs   number of arguments
 * \param[in]   args    argument list
 *
 * \return  0 on success, < 0 on failure
 */
static int unit_cmd(int nargs, char **args)
{
    int dev;

    if (arg_to_int(args[1], &dev) < 0 || check_drive_unit(dev) < 0) {
        return FD_BADDEV;
    }

    drive_index = dev - DRIVE_UNIT_MIN;
    return FD_OK;
}

static int unlynx_loop(FILE *f, FILE *f2, vdrive_t *vdrive, long dentries)
{
    long lbsize, bsize;
    char cname[20]; /* FIXME: remove magic number */
    int ftype;
    uint8_t val;
    long cnt;
    size_t len;
    char buff[256];
    cbmdos_cmd_parse_t cmd_parse;
    char *endp;

    while (dentries != 0) {
        int filetype, rc;

        /* Read CBM filename */
        cnt = 0;
        while (1) {
            /* FIXME: using fread() to read a single byte? */
            if (fread(&val, 1, 1, f) != 1) {
                return FD_RDERR;
            }
            if (val != 13 && cnt < 20 - 1) {
                cname[cnt++] = (char)val;
            } else {
                break;
            }
        }
        cname[cnt] = 0;

        /* Read the block size */
        cnt = 0;
        while (1) {
            if (fread(&val, 1, 1, f) != 1) {
                return FD_RDERR;
            }
            if (val != 13) {
                buff[cnt++] = (char)val;
            } else {
                break;
            }
        }
        buff[cnt] = 0;

        bsize = strtol(buff, &endp, 10);
        if (buff == endp) {
            fprintf(stderr, "invalid Lynx file\n");
            return FD_RDERR;
        }
        /* Get the file type (P[RG], S[EQ], R[EL], U[SR]) */
        ftype = fgetc(f);
        fgetc(f);

        switch (ftype) {
            case 'D':
                filetype = CBMDOS_FT_DEL;
                break;
            case 'P':
                filetype = CBMDOS_FT_PRG;
                break;
            case 'S':
                filetype = CBMDOS_FT_SEQ;
                break;
            case 'U':
                filetype = CBMDOS_FT_USR;
                break;
            case 'R':
                fprintf(stderr, "REL not supported\n");
                return FD_RDERR;
            default:
                filetype = CBMDOS_FT_PRG;
        }
        /* Get the byte size of the last block +1 */
        cnt = 0;
        while (1) {
            if (fread(&val, 1, 1, f) != 1) {
                return FD_RDERR;
            }
            if (val != 13) {
                buff[cnt++] = (char)val;
            } else {
                break;
            }
        }
        buff[cnt] = 0;

        lbsize = strtol(buff, &endp, 10);
        if (buff == endp) {
            fprintf(stderr, "invalid Lynx file\n");
            return FD_RDERR;
        }
        /* Calculate byte size of file */
        cnt = (bsize - 1) * 254 + lbsize - 1;

        printf("writing file '%s' to image\n", cname);

        cmd_parse.parsecmd = lib_strdup(cname);
        cmd_parse.secondary = 1;
        cmd_parse.parselength = (unsigned int)strlen(cname);
        cmd_parse.readmode = CBMDOS_FAM_WRITE;
        cmd_parse.filetype = (unsigned int)filetype;

        rc = vdrive_iec_open(vdrive, NULL, 0, 1, &cmd_parse);

        if (rc != SERIAL_OK) {
            fprintf(stderr, "error writing file %s\n", cname);
            break;
        }

        while (cnt != 0) {
            if (fread(&val, 1, 1, f2) != 1) {
                return FD_RDERR;
            }
            if (vdrive_iec_write(vdrive, val, 1)) {
                fprintf(stderr, "no space on image ?\n");
                break;
            }
            cnt--;
        }
        vdrive_iec_close(vdrive, 1);

        /* Adjust for the last block, but not for the last file */
        if (lbsize < 255 && dentries > 1) {
            len = (size_t)(254 + 1 - lbsize);
            if (fread(buff, 1, len, f2) != len) {
                return FD_RDERR;
            }
        }
        dentries--;
    }

    return FD_OK;
}


/** \brief  Unlynx a Lynx container onto a virtual device
 *
 * Syntax: unlynx \<filename> [\<unit>]
 *
 * \param[in]   nargs   argument count
 * \param[in]   args    argument list
 *
 * \return  0 on success, < 0 on failure
 */
static int unlynx_cmd(int nargs, char **args)
{
    vdrive_t *vdrive;
    FILE *f, *f2;
    int dev, cnt;
    long dentries, dirsize;
    uint8_t val;
    char buff[256];
    int rc;
    char *path, *endp;

    if (nargs < 3) {
        dev = drive_index;
    } else {
        if (arg_to_int(args[2], &dev) < 0) {
            return FD_BADDEV;
        }
        if (check_drive_unit(dev) < 0) {
            return FD_BADDEV;
        }
        dev -= DRIVE_UNIT_MIN;
    }

    if (check_drive_ready(dev) < 0) {
        return FD_NOTREADY;
    }

    vdrive = drives[dev];
    archdep_expand_path(&path, args[1]);

    f = fopen(path, MODE_READ);

    if (f == NULL) {
        fprintf(stderr, "cannot open `%s' for reading\n", path);
        lib_free(path);
        return FD_NOTRD;
    }

    /* Look for the 0, 0, 0 sign of the end of BASIC.  */
    cnt = 0;
    while (1) {
        if (fread(&val, 1, 1, f) != 1) {
            return FD_RDERR;
        }
        if (val == 0) {
            cnt++;
        } else {
            cnt = 0;
        }
        if (cnt == 3) {
            break;
        }
    }

    /* Bypass the 1st return in the file */
    fgetc(f);

    /* Get the directory block size */
    cnt = 0;
    while (1) {
        if (fread(&val, 1, 1, f) != 1) {
            lib_free(path);
            return FD_RDERR;
        }
        if (val != 13) {
            buff[cnt++] = (char)val;
        } else {
            break;
        }
    }

    buff[cnt] = 0;

    dirsize = strtol(buff, &endp, 10);
    if ((buff == endp) || (dirsize <= 0)) {
        fprintf(stderr, "invalid Lynx file\n");
        fclose(f);
        lib_free(path);
        return FD_RDERR;
    }

    /* Get the number of dir entries */
    cnt = 0;
    while (1) {
        if (fread(&val, 1, 1, f) != 1) {
            lib_free(path);
            return FD_RDERR;
        }
        if (val != 13 && cnt < 256 - 1) {
            buff[cnt++] = (char)val;
        } else {
            break;
        }
    }

    buff[cnt] = 0;

    dentries = strtol(buff, &endp, 10);
    if ((buff == endp) || (dentries <= 0)) {
        fprintf(stderr, "invalid Lynx file\n");
        fclose(f);
        lib_free(path);
        return FD_RDERR;
    }

    /* Open the file for reading of the chained data */
    f2 = fopen(path, MODE_READ);

    if (f2 == NULL) {
        fprintf(stderr, "cannot open `%s' for reading\n", path);
        fclose(f);
        lib_free(path);
        return FD_NOTRD;
    }

    fseek(f2, (dirsize * 254), SEEK_SET);

    rc = unlynx_loop(f, f2, vdrive, dentries);

    fclose(f);
    fclose(f2);
    lib_free(path);
    return rc;
}


/** \brief  Validate contents of a drive
 *
 * \param[in]   nargs   argument count
 * \param[in]   args    argument list
 *
 * \return  0 on success, < 0 on failure
 */
static int validate_cmd(int nargs, char **args)
{
    int dnr = drive_index;

    /* get unit number from args */
    if (nargs >= 2) {
        int unit;

        if (arg_to_int(args[1], &unit) < 0) {
            return FD_BADDEV;
        }
        /* check for valid unit number */
        if (check_drive_unit(unit) < 0) {
            return FD_BADDEV;
        }
        dnr = unit - DRIVE_UNIT_MIN;
    }

    /* check if drive is ready */
    if (check_drive_ready(dnr) < 0) {
        return FD_NOTREADY;
    }

    printf("validating in unit %d ...\n", dnr + 8);
    vdrive_command_validate(drives[dnr]);

    return FD_OK;
}


/** \brief  Display version number
 *
 * \param[in]   nargs   argument count
 * \param[in]   args    argument list
 *
 * \return  FD_OK
 */
static int version_cmd(int nargs, char **args)
{
    printf("c1541 (VICE %s SVN r%d)\n",
            VERSION, VICE_SVN_REV_NUMBER);
    return FD_OK;
}


/** \brief  Write a file from the host FS to a virtual device
 *
 * \param[in]   nargs   argument count
 * \param[in]   args    argument list
 *
 * \return  FD_OK on success, < 0 on failure
 */
static int write_cmd(int nargs, char **args)
{
    int dnr;
    int unit;
    char *dest_name;
    unsigned int dest_len;
    char *p;
    fileio_info_t *finfo;
    char *src_name;
    long rel_record_length = 0;
    int result = FD_OK;

    if (nargs == 3) {
        /* write <source> <dest> */

        unit = extract_unit_from_file_name(args[2], &p);
        if (unit == 0) {
            unit = drive_index + DRIVE_UNIT_MIN;
        } else if (unit < 0) {
            printf("Got unit < 0\n");
            return FD_BADDEV;
        }
        if (p != NULL && *p != '\0') {
            dest_name = lib_strdup(p);
        } else {
            dest_name = NULL;
        }

        if (dest_name != NULL) {
            char *lenptr;

            charset_petconvstring((uint8_t *)dest_name, CONVERT_TO_PETSCII);

            /* Convert "NAME,L,100" to "NAME,L,"+CHR$(100) */
            if ((lenptr = strstr(dest_name, ",L,"))) {
                char *endptr;
                lenptr += 3;      /* after the second comma */

                rel_record_length = strtol(lenptr, &endptr, 10);
                if (endptr > lenptr && endptr[0] == '\0' &&
                    rel_record_length >= 1 && rel_record_length <= 254) {
                    lenptr[0] = (char)(uint8_t)rel_record_length;
                    if ((endptr - lenptr) > 1) {
                        lenptr[1] = '\0';
                    }
                    fprintf(stderr, "Converted record length %lu\n",
                            (unsigned long)rel_record_length);
                } else {
                    rel_record_length = 0;
                }
            }
        }
    } else {
        /* write <source> */
        dest_name = NULL;
        unit = drive_index + DRIVE_UNIT_MIN;
    }

    dnr = unit - DRIVE_UNIT_MIN;

    if (check_drive_index(dnr) < 0) {
        printf("check_drive_index() failed\n");
        if (dest_name != NULL) {
           lib_free(dest_name);
        }
        return FD_BADDEV;
    }
    if (check_drive_ready(dnr) < 0) {
        if (dest_name != NULL) {
           lib_free(dest_name);
        }
        return FD_NOTREADY;
    }

    /* ~ expand path on Unix */
    archdep_expand_path(&src_name, args[1]);

    finfo = fileio_open(src_name, NULL, FILEIO_FORMAT_RAW | FILEIO_FORMAT_P00,
                        FILEIO_COMMAND_READ | FILEIO_COMMAND_FSNAME,
                        FILEIO_TYPE_PRG, NULL);

    if (finfo == NULL) {
        fprintf(stderr, "cannot read file `%s': %s\n", args[1],
                strerror(errno));
        lib_free(src_name);
        if (dest_name != NULL) {
           lib_free(dest_name);
        }
        return FD_NOTRD;
    }

    if (dest_name == NULL) {
        dest_name = lib_strdup((char *)(finfo->name));
        dest_len = finfo->length;
    } else {
        dest_len = (unsigned int)strlen(dest_name);
    }

    if (vdrive_iec_open(drives[dnr], (uint8_t *)dest_name, (unsigned int)dest_len,
                1, NULL)) {
        fprintf(stderr, "cannot open `%s' for writing on image\n",
                dest_name);
        fileio_close(finfo);
        lib_free(dest_name);
        lib_free(src_name);
        return FD_WRTERR;
    }

    if (dest_name == (char *)finfo->name) {
        printf("writing file `%s' to unit %d\n", finfo->name, dnr + 8);
    } else {
        printf("writing file `%s' as `%s' to unit %d\n", finfo->name,
               dest_name, dnr + 8);
    }

    if (rel_record_length == 0) {
        while (1) {
            uint8_t c;

            if (fileio_read(finfo, &c, 1) != 1) {
                break;
            }

            if (vdrive_iec_write(drives[dnr], c, 1)) {
                fprintf(stderr, "no space on image?\n");
                result = FD_WRTERR;
                break;
            }
        }
    } else {
        unsigned int length = fileio_get_bytes_left(finfo);
        /* Records and positions start counting at 1 */
        unsigned int max_record =
            (unsigned int)((length + rel_record_length - 1) / rel_record_length);
        unsigned int record;
        int err;

        /* First allocate space for the whole file */
        err = vdrive_rel_position(drives[dnr], 1,
                (max_record & 0xFF), (max_record >> 8) & 0xFF,
                1);
        if (err && err != CBMDOS_IPE_NO_RECORD) {
            fprintf(stderr, "Cannot Position to record %u (err %d)\n",
                    max_record, err);
        }
        err = vdrive_iec_write(drives[dnr], 0, 1);
        if (err != SERIAL_OK) {
            fprintf(stderr, "Cannot write in record %u (err %d)\n",
                    max_record, err);
        }
        /*
         * Bug/annoyance: given the way that sectors are added at the
         * end of a REL file, filled with as many records as will fit,
         * there may be too many records in the file now. This is not
         * avoided by growing the file piece by piece.
         */

        for (record = 1; record <= max_record; record++) {
            int pos;

            err = vdrive_rel_position(drives[dnr], 1,   /* 1 = secadr */
                    (record & 0xFF), (record >> 8) & 0xFF,
                    1);
            if (err && err != CBMDOS_IPE_NO_RECORD) {
                fprintf(stderr, "Cannot Position to record %u (err %d)\n",
                        record, err);
            }
            for (pos = 0; pos < rel_record_length; pos++) {
                uint8_t c;

                if (fileio_read(finfo, &c, 1) != 1) {
                    break;
                }

                if ((err = vdrive_iec_write(drives[dnr], c, 1)) != SERIAL_OK) {
                    fprintf(stderr, "no space on image? (err %d)\n", err);
                    result = FD_WRTERR;
                    break;
                }
            }
        }
    }

    fileio_close(finfo);
    vdrive_iec_close(drives[dnr], 1);

    lib_free(dest_name);
    lib_free(src_name);
    return result;
}

static int unzip_cmd(int nargs, char **args)
{
    char fname[ARCHDEP_PATH_MAX];
    char dirname[ARCHDEP_PATH_MAX];
    char oname[ARCHDEP_PATH_MAX];
    vdrive_t *vdrive = drives[drive_index];
    FILE *fsfd = NULL;
    unsigned int track, sector;
    unsigned int count;
    int singlefilemode = 0, err;
    uint8_t sector_data[256];
    char *p;

    /* Open image or create a new one.  If the file exists, it must have
       valid header.  */
    if (open_image(drive_index, args[1], 1, DISK_IMAGE_TYPE_D64) < 0) {
        return FD_BADIMAGE;
    }

    p = strrchr(args[2], ARCHDEP_DIR_SEP_CHR);
    if (p == NULL) {
        /* ignore '[0-4]!' if found */
        if (args[2][0] >= '1' && args[2][0] <= '4' && args[2][1] == '!') {
            strcpy(fname + 2, args[2] + 2);
        } else {
            strcpy(fname + 2, args[2]);
        }
        fname[0] = '0';
        fname[1] = '!';
        strcpy(dirname, "");
    } else {
        size_t len_path;

        len_path = (size_t)(p - args[2]);
        if (len_path == strlen(args[2]) - 1) {
            /* FIXME: Close image?  */
            return FD_RDERR;
        }
        strncpy(dirname, args[2], len_path + 1);
        dirname[len_path + 1] = '\0';

        /* ignore '[0-4]!' if found */
        if (args[2][len_path + 1] >= '1' && args[2][len_path + 1] <= '4'
            && args[2][len_path + 1 + 1] == '!') {
            strcpy(fname + 2, &(args[2][len_path + 1]) + 2);
        } else {
            strcpy(fname + 2, &(args[2][len_path + 1]));
        }
        fname[0] = '0';
        fname[1] = '!';
    }

    printf("copying blocks to image\n");

    for (track = 1; track <= 35; track++) {
        if (singlefilemode || track == 1) {
            if (track == 1) {
                /* For now we disable one-file more, because it is not detected
                   correctly.  */
                strcpy(oname, dirname);
                strcat(oname, fname + 2);
                fsfd = fopen(oname, MODE_READ);
                if (fsfd != NULL) {
                    printf("reading zipfile on one-file mode\n");
                    singlefilemode = 1;
                    fseek(fsfd, 4, SEEK_SET);
                }
            } else if (track == 9 || track == 17 || track == 26) {
                fseek(fsfd, 2, SEEK_CUR);
            }
        }
        if (!singlefilemode) {
            switch (track) {
                case 1:     /* fallthrough */
                case 9:     /* fallthrough */
                case 17:    /* fallthrough */
                case 26:    /* fallthrough */
                    fname[0]++;
                    if (fsfd != NULL) {
                        fclose(fsfd);
                    }
                    strcpy(oname, dirname);
                    strcat(oname, fname);
                    if ((fsfd = fopen(oname, MODE_READ)) == NULL) {
                        fprintf(stderr, "cannot open `%s'\n", fname);
                        return FD_NOTRD;
                    }
                    fseek(fsfd, (track == 1) ? 4 : 2, SEEK_SET);
                    break;
                default:
                    break;
            }
        }
        for (count = 0;
             count < disk_image_sector_per_track(DISK_IMAGE_TYPE_D64, track);
             count++) {
            err = zipcode_read_sector(fsfd, (int)track, (int *)&sector,
                                      (char *)sector_data);
            if (err) {
                fclose(fsfd);
                return FD_BADIMAGE;
            }
            /* Write one block */
            if (vdrive_write_sector(vdrive, sector_data, track, sector) < 0) {
                fclose(fsfd);
                return FD_RDERR;
            }
        }
    }

    fclose(fsfd);
    vdrive_command_execute(vdrive, (uint8_t *)"I", 1);

    return FD_OK;
}

static int raw_cmd(int nargs, char **args)
{
    vdrive_t *vdrive = drives[drive_index];

    if (vdrive == NULL) {
        return FD_NOTREADY;
    }

    if (vdrive->image == NULL) {
        return FD_NOTREADY;
    }

    if (vdrive->image->device == DISK_IMAGE_DEVICE_FS) {
        if (vdrive->buffers[15].buffer == NULL) {
            return FD_NOTREADY;
        }
    }

    /* Write to the command channel.  */
    if (nargs >= 2) {
        char *command = lib_strdup(args[1]);

        charset_petconvstring((uint8_t *)command, CONVERT_TO_PETSCII);
        vdrive_command_execute(vdrive, (uint8_t *)command, (unsigned int)strlen(command));
        lib_free(command);
    }

    /* Print the error now.  */
    puts((char *)vdrive->buffers[15].buffer);
    return FD_OK;
}

/* ------------------------------------------------------------------------- */

/** \brief  Program driver
 *
 * \param[in]   argc    argument count
 * \param[in]   argv    argument vector
 *
 * \return  EXIT_SUCCESS or EXIT_FAILURE
 */
int main(int argc, char **argv)
{
    char *args[MAXARG];
    int nargs;
    int i;
    int retval = EXIT_SUCCESS;

    /* Needed to initialize the threading stuff in lib.c when creating a debug
     * build. We don't need any threading for c1541, but c1541 gets built as
     * an emulator, so now we do.
     */
    lib_init();

    archdep_init(&argc, argv);

    /* This causes all the logging messages from the various VICE modules to
       appear on stdout.  */
    log_init_with_fd(stdout);

    serial_iec_bus_init();

    for (i = 0; i < MAXARG; i++) {
        args[i] = NULL;
    }
    nargs = 0;

    for (i = 0; i < NUM_DISK_UNITS; i++) {
        drives[i] = lib_calloc(1, sizeof *drives[i]);
    }

    /* The first arguments without leading `-' are interpreted as disk images
       to attach.  */
    for (i = 1; i < argc && *argv[i] != '-'; i++) {
        if ((i - 1) == NUM_DISK_UNITS) {
            fprintf(stderr, "Ignoring disk image `%s'\n", argv[i]);
        } else {
            if (open_disk_image(drives[i - 1], argv[i], (unsigned int)(i - 1 + 8)) != 0) {
                /* error: clean up and exit */
                while (--i >= 1) {
                    close_disk_image(drives[i - 1], (unsigned int)(i - 1 + 8));
                }
                for (i = 0; i < NUM_DISK_UNITS; i++) {
                    lib_free(drives[i]);
                }
                archdep_shutdown();
                log_close_all();
                return EXIT_FAILURE;
            }
        }
    }

    if (i == argc) {
        char *line;
        char *buf = NULL;

        /* Interactive mode.  */
        interactive_mode = 1;
        /* init linenoise-ng */
        linenoiseHistorySetMaxLen(100);

        /* TODO: Add completions on Windows, somehow, or perhaps not */

        version_cmd(0, NULL);
        printf("Copyright 1995-" VICEDATE_YEAR_STR " The VICE Development Team.\n"
               "C1541 is free software, covered by the GNU General Public License,"
               " and you are\n"
               "welcome to change it and/or distribute copies of it under certain"
               " conditions.\n");
#if 0
        fflush(stdout); /* needs flushing on windows, it seems */
#endif

        while (1) {
            fflush(stderr);
            lib_free(buf);
            buf = lib_msprintf("c1541 #%d> ", drive_index | 8);

            line = linenoise(buf);

            if (line == NULL) {
                putchar('\n');
                fflush(stdout);
                fflush(stderr);
                break;
            }

            if (*line == '!') {
                retval = system(line + 1);
                printf("exit code: %d\n", retval);
            } else {
                split_args(line, &nargs, args);
                if (nargs > 0) {
                    /* This is obviously seriously fucked, but currently the
                     * only way to free `buf` in interactive mode without
                     * rewriting large portions of code.
                     *
                     * It would be better if look_and_execute_command() would
                     * return some value to indicate x/exit/q/quit was the
                     * command, to properly clean up.
                     *
                     * Also: linenoise-ng leaks like hell
                     */
                    if ((strcmp(args[0], "x") == 0)
                        || strcmp(args[0], "q") == 0
                        || strcmp(args[0], "quit") == 0
                        || strcmp(args[0], "exit") == 0) {
                        lib_free(buf);
                    }
                    lookup_and_execute_command(nargs, args);
               }
            }
            linenoiseHistoryAdd(line);
        }
        lib_free(buf);
        /* free memory used by the argument 'parser' */
        for (i = 0; i < MAXARG; i++) {
            if (args[i] != NULL) {
                lib_free(args[i]);
            }
        }
        /* properly clean up GNU readline's history, if used */
        linenoiseHistoryFree();
    } else {
        while (i < argc) {
            int match = 0, minargs = 0/*, maxargs = 0*/;
            int n;
            args[0] = argv[i] + 1;  /* only cmd word without leading - */
            match = lookup_command(args[0]);
            nargs = 1;
            i++;
            if (match >= 0) {
                /* this was a valid command */
                const command_t *cp = &command_list[match];
                minargs = cp->min_args;
                /*maxargs = cp->max_args;*/
            }
            /* first copy mandatory arguments, - is allowed without restrictions */
            for (n = 0; (i < argc) && (n < minargs); n++, i++) {
                args[nargs++] = argv[i];
            }
            /* copy rest of arguments (optional), - is allowed only when not the
               same as a command */
            for (; i < argc; i++) {
                if (*argv[i] == '-') {
                    if (lookup_command(argv[i] + 1) >= 0) {
                        /* same as a command */
                        break;
                    }
                }
                /* valid optional argument */
                args[nargs++] = argv[i];
            }
            if (lookup_and_execute_command(nargs, args) < 0) {
                retval = EXIT_FAILURE;
                break;
            }
        }
    }

    /* free memory used by the virtual drives */
    for (i = 0; i < NUM_DISK_UNITS; i++) {
        if (drives[i]) {
            close_disk_image(drives[i], i + 8);
            lib_free(drives[i]);
        }
    }
    /* free memory used by archdep */
    archdep_shutdown();
    /* free memory used by the log module */
    log_close_all();

    return retval;
}


/** \brief  Enable\\disable saving of files as P00
 *
 * Syntax: p00save [\<enable> [\<unit>]]
 *
 * Where \a enable is either 0 or 1.
 *
 * \param[in]   nargs   argument count
 * \param[in]   args    argument list
 *
 * \return  0 on success, < 0 on failure
 */
static int p00save_cmd(int nargs, char **args)
{
    int dnr = 0, enable = 0;

    if (nargs == 1) {
        /* display `p00save` state for all drives */
        int i;
        for (i = 0; i < NUM_DISK_UNITS; i++) {
            printf("#%2d: %s\n",
                    i + DRIVE_UNIT_MIN, p00save[i] ? "enabled" : "disabled");
        }
        return FD_OK;
    }

    arg_to_int(args[1], &enable);

    if (nargs == 3) {
        if (arg_to_int(args[2], &dnr) < 0) {
            return FD_BADDEV;
        }
        if (check_drive_unit(dnr) < 0) {
            return FD_BADDEV;
        }
        dnr -= 8;
    }

    p00save[dnr] = (unsigned int)enable;
    return FD_OK;
}


/** \brief  Change current host directory path
 *
 * Syntax: cd \<hostdir>
 *
 * Where \a hostdir is the path to directory on the host
 *
 * \param[in]   nargs   number of arguments
 * \param[in]   args    optional arguments (unused
 *
 * \return  0 on success, < 0 on failure
 */
static int cd_cmd(int nargs, char **args)
{
    return archdep_chdir(args[1]) == 0 ? FD_OK : FD_BADNAME;
}


/** \brief  Print current working directory
 *
 * \param[in]   nargs   number of arguments
 * \param[in]   args    optional arguments (unused
 *
 * \return 0
 */
static int pwd_cmd(int nargs, char **args)
{
    char buffer[4096];

    archdep_getcwd(buffer, sizeof(buffer));
    printf("%s\n", buffer);
    return FD_OK;
}
