/*
 * cbmdos.h - Common CBM DOS routines.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
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

#ifndef _CBMDOS_H
#define _CBMDOS_H

/* Input Processor Error Codes.  */
#define CBMDOS_IPE_OK                      0
#define CBMDOS_IPE_DELETED                 1
#define CBMDOS_IPE_SEL_PARTN               2   /* 1581 */
#define CBMDOS_IPE_UNIMPL                  3
#define CBMDOS_IPE_MEMORY_READ             4

#define CBMDOS_IPE_WRITE_ERROR             25
#define CBMDOS_IPE_WRITE_PROTECT_ON        26
#define CBMDOS_IPE_SYNTAX                  30
#define CBMDOS_IPE_INVAL                   31
#define CBMDOS_IPE_LONG_LINE               32
#define CBMDOS_IPE_BAD_NAME                33
#define CBMDOS_IPE_NO_NAME                 34

#define CBMDOS_IPE_NO_RECORD               50
#define CBMDOS_IPE_OVERFLOW                51

#define CBMDOS_IPE_NOT_WRITE               60
#define CBMDOS_IPE_NOT_OPEN                61
#define CBMDOS_IPE_NOT_FOUND               62
#define CBMDOS_IPE_FILE_EXISTS             63
#define CBMDOS_IPE_BAD_TYPE                64
#define CBMDOS_IPE_NO_BLOCK                65
#define CBMDOS_IPE_ILLEGAL_TRACK_OR_SECTOR 66

#define CBMDOS_IPE_NO_CHANNEL              70
#define CBMDOS_IPE_DISK_FULL               72
#define CBMDOS_IPE_DOS_VERSION             73
#define CBMDOS_IPE_NOT_READY               74
#define CBMDOS_IPE_BAD_PARTN               77  /* 1581 */

#define CBMDOS_IPE_NOT_EMPTY               80  /* dir to remove not empty */
#define CBMDOS_IPE_PERMISSION              81  /* permission denied */

/* File Types */
#define CBMDOS_FT_DEL         0
#define CBMDOS_FT_SEQ         1
#define CBMDOS_FT_PRG         2
#define CBMDOS_FT_USR         3
#define CBMDOS_FT_REL         4
#define CBMDOS_FT_CBM         5       /* 1581 partition */
#define CBMDOS_FT_REPLACEMENT 0x20
#define CBMDOS_FT_LOCKED      0x40
#define CBMDOS_FT_CLOSED      0x80

/* Access Control Methods */
#define CBMDOS_FAM_READ   0
#define CBMDOS_FAM_WRITE  1
#define CBMDOS_FAM_APPEND 2


struct cbmdos_cmd_parse_s {
    const char *cmd;
    unsigned int cmdlength;
    char *parsecmd;
    unsigned int secondary;
    unsigned int parselength;
    unsigned int readmode;
    unsigned int filetype;
    unsigned int recordlength;
};
typedef struct cbmdos_cmd_parse_s cbmdos_cmd_parse_t;


extern const char *cbmdos_errortext(unsigned int code);
extern unsigned int cbmdos_parse_wildcard_check(const char *name,
                                                unsigned int len);
extern unsigned int cbmdos_command_parse(cbmdos_cmd_parse_t *cmd_parse);

#endif

