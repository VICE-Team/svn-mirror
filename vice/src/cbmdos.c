/*
 * cbmdos.c - Common CBM DOS routines.
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

#include "vice.h"

#include <stdio.h>
#include <string.h>

#include "cbmdos.h"
#include "lib.h"


typedef struct cbmdos_errortext_s {
    unsigned int nr;
    const char *text;
} cbmdos_errortext_t;


static const cbmdos_errortext_t cbmdos_error_messages[] =
{
    {  0, " OK" },
    {  1, "FILES SCRATCHED" },
    {  2, "SELECTED PARTITION" },           /* 1581 */
    {  3, "UNIMPLEMENTED" },
    { 20, "READ ERROR" },
    { 21, "READ ERROR" },
    { 22, "READ ERROR" },
    { 23, "READ ERROR" },
    { 24, "READ ERROR" },
    { 25, "WRITE ERROR" },
    { 26, "WRITE PROTECT ON" },
    { 27, "READ ERROR" },
    { 28, "WRITE ERROR" },
    { 29, "DISK ID MISMATCH" },
    { 30, "SYNTAX ERROR" },
    { 31, "SYNTAX ERROR" },
    { 32, "SYNTAX ERROR" },
    { 33, "SYNTAX ERROR" },
    { 34, "SYNTAX ERROR" },
    { 39, "SYNTAX ERROR" },
    { 50, "RECORD NOT RESENT" },
    { 51, "OVERFLOW IN RECORD" },
    { 60, "WRITE FILE OPEN" },
    { 61, "FILE NOT OPEN" },
    { 62, "FILE NOT FOUND" },
    { 63, "FILE EXISTS" },
    { 64, "FILE TYPE MISMATCH" },
    { 65, "NO BLOCK" },
    { 66, "ILLEGAL TRACK OR SECTOR" },
    { 67, "ILLEGAL SYSTEM T OR S" },
    { 70, "NO CHANNEL" },
    { 72, "DISK FULL" },
    { 73, "VIRTUAL DRIVE EMULATION V2.2" }, /* The program version */
    { 74, "DRIVE NOT READY" },
    { 77, "SELECTED PARTITION ILLEGAL" },   /* 1581 */
    { 80, "DIRECTORY NOT EMPTY" },
    { 81, "PERMISSION DENIED" },
    { 255, NULL }
};

static const char *cbmdos_ft[] = {
    "DEL", "SEQ", "PRG", "USR", "REL", "CBM"
};


const char *cbmdos_errortext(unsigned int code)
{
    unsigned int count = 0;

    while (cbmdos_error_messages[count].nr != 255
        && cbmdos_error_messages[count].nr != code)
        count++;

    if (cbmdos_error_messages[count].nr != 255)
        return cbmdos_error_messages[count].text;

    return "UNKNOWN ERROR NUMBER";
}

const char *cbmdos_filetype_get(unsigned int filetype)
{
   if (filetype > 5)
       return NULL;

   return cbmdos_ft[filetype];
}

unsigned int cbmdos_parse_wildcard_check(const char *name, unsigned int len)
{
    unsigned int index;

    for (index = 0; index < len; index++) {
        if (name[index] == '*' || name[index] == '?')
            return 1;
    }

    return 0;
}

unsigned int cbmdos_parse_wildcard_compare(const BYTE *name1, const BYTE *name2)
{
    unsigned int index;

    for (index = 0; index < CBMDOS_SLOT_NAME_LENGTH; index++) {
        if (name1[index] == '*')
            return 1;
        if (name1[index] != '?' && name1[index] != name2[index])
            return 0;
        if (name1[index] == 0xa0)
            return 1;
    }

    return 1;
}

BYTE *cbmdos_dir_slot_create(const char *name, unsigned int len)
{
    BYTE *slot;

    if (len > CBMDOS_SLOT_NAME_LENGTH)
        len = CBMDOS_SLOT_NAME_LENGTH;

    slot = lib_malloc(CBMDOS_SLOT_NAME_LENGTH);
    memset(slot, 0xa0, CBMDOS_SLOT_NAME_LENGTH);

    memcpy(slot, name, (size_t)len);

    return slot;
}

/* Parse command `parsecmd', type and read/write mode from the given string
   `cmd' with `cmdlength. '@' on write must be checked elsewhere.  */

unsigned int cbmdos_command_parse(cbmdos_cmd_parse_t *cmd_parse)
{
    const char *p;
    char *parsecmd, *c;
    int cmdlen;

    cmd_parse->parsecmd = NULL;
    cmd_parse->readmode = (cmd_parse->secondary == 1)
                          ? CBMDOS_FAM_WRITE : CBMDOS_FAM_READ;

    if (cmd_parse->cmd == NULL || cmd_parse->cmdlength == 0)
        return CBMDOS_IPE_NO_NAME;

    p = (char *)memchr(cmd_parse->cmd, ':', cmd_parse->cmdlength);

    if (p) {
        p++;
    } else {      /* no colon found */
        if (*(cmd_parse->cmd) != '$')
            p = cmd_parse->cmd;
        else
            p = cmd_parse->cmd + cmd_parse->cmdlength; /* set to null byte */
    }

#if 0
    if (*(cmd_parse->cmd) == '@' && p == cmd_parse->cmd)
        p++;
#endif

    cmdlen = cmd_parse->cmdlength - (p - cmd_parse->cmd);
    cmd_parse->parselength = 0;

    /* Temporary hack.  */
    cmd_parse->parsecmd = (char *)lib_calloc(1, cmdlen + 2);

    parsecmd = cmd_parse->parsecmd;

    while (*p != ',' && cmdlen-- > 0) {
        (cmd_parse->parselength)++;
        *(parsecmd++) = *(p++);
    }

    cmd_parse->filetype = 0;

    /*
     * Change modes ?
     */
    while (cmdlen > 0) {
        cmdlen--;
        p++;

        if (cmdlen == 0)
            return CBMDOS_IPE_INVAL;

        switch (*p) {
          case 'S':
            cmd_parse->filetype = CBMDOS_FT_SEQ;
            break;
          case 'P':
            cmd_parse->filetype = CBMDOS_FT_PRG;
            break;
          case 'U':
            cmd_parse->filetype = CBMDOS_FT_USR;
            break;
          case 'L':                     /* L,(#record length)  max 254 */
            if (p[1] == ',') {
                cmd_parse->recordlength = p[2]; /* Changing RL causes error */

                if (cmd_parse->recordlength > 254)
                    return CBMDOS_IPE_OVERFLOW;
            }
            cmd_parse->filetype = CBMDOS_FT_REL;
            break;
          case 'R':
            cmd_parse->readmode = CBMDOS_FAM_READ;
            break;
          case 'W':
            cmd_parse->readmode = CBMDOS_FAM_WRITE;
            break;
          default:
            if (cmd_parse->readmode != CBMDOS_FAM_READ
                && cmd_parse->readmode != CBMDOS_FAM_WRITE)
                return CBMDOS_IPE_INVAL;
        }

        c = (char *)memchr(p, ',', cmdlen);
        if (c) {
            cmdlen -= (c - p);
            p = c;
        } else {
            cmdlen = 0;
        }
    }

    /* Override read mode if secondary is 0 or 1.  */
    if (cmd_parse->secondary == 0)
        cmd_parse->readmode = CBMDOS_FAM_READ;
    if (cmd_parse->secondary == 1)
        cmd_parse->readmode = CBMDOS_FAM_WRITE;

    /* Set filetype according secondary address, if it was not specified.  */
    if (cmd_parse->filetype == 0)
        cmd_parse->filetype = (cmd_parse->secondary < 2)
                              ? CBMDOS_FT_PRG : CBMDOS_FT_SEQ;

    return CBMDOS_IPE_OK;
}

