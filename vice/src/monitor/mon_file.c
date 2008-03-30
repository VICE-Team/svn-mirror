/*
 * mon_file.c - The VICE built-in monitor file functions.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Daniel Sladic <sladic@eecg.toronto.edu>
 *  Ettore Perazzoli <ettore@comm2000.it>
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

#include "archdep.h"
#include "mem.h"
#include "montypes.h"
#include "mon_util.h"
#include "uimon.h"

#define ADDR_LIMIT(x) ((ADDRESS)(LO16(x)))

void mon_file_load(const char *filename, MON_ADDR start_addr, bool is_bload)
{
    FILE *fp;
    ADDRESS adr;
    int b1 = 0, b2 = 0;
    int ch = 0;
    MEMSPACE mem;

    fp = fopen(filename, MODE_READ);

    if (fp == NULL) {
        mon_out("Loading %s failed.\n", filename);
        return;
    }

    if (is_bload == FALSE) {
        b1 = fgetc(fp);
        b2 = fgetc(fp);
    }

    mon_evaluate_default_addr(&start_addr);
    if (!mon_is_valid_addr(start_addr)) {   /* No Load address given */
        if (is_bload == TRUE) {
            mon_out("No LOAD address given.\n");
            return;
        }

        if (b1 == 1)    /* Load Basic */
            mem_get_basic_text(&adr, NULL);
        else
            adr = (BYTE)b1 | ((BYTE)b2 << 8);
        mem = e_comp_space;
    } else  {
        adr = addr_location(start_addr);
        mem = addr_memspace(start_addr);
    }

    mon_out("Loading %s", filename);
    mon_out(" from %04X\n", adr);

    do {
        BYTE load_byte;

        if (fread((char *)&load_byte, 1, 1, fp) < 1)
            break;
        mon_set_mem_val(mem, ADDR_LIMIT(adr + ch), load_byte);
        ch ++;
    } while(1);
    mon_out("%x bytes\n", ch);

    if (is_bload == FALSE) {
        /* set end of load addresses like kernal load */
        mem_set_basic_text(adr, (ADDRESS)(adr + ch));
    }

    fclose(fp);
}

void mon_file_save(const char *filename, MON_ADDR start_addr,
                   MON_ADDR end_addr, bool is_bsave)
{
    FILE *fp;
    ADDRESS adr, end;
    long len;
    int ch = 0;
    MEMSPACE mem;

    len = mon_evaluate_address_range(&start_addr, &end_addr, TRUE, -1);
    if (len < 0) {
        mon_out("Invalid range.\n");
        return;
    }

    mem = addr_memspace(start_addr);

    adr = addr_location(start_addr);
    end = addr_location(end_addr);

    if (end < adr) {
        mon_out("Start address must be below end address.\n");
        return;
    }

    fp = fopen(filename, MODE_WRITE);

    if (fp == NULL) {
        mon_out("Saving for `%s' failed.\n", filename);
    } else {
        printf("Saving file `%s'...\n", filename);

        if (is_bsave == FALSE) {
            fputc((BYTE)adr & 0xff, fp);
            fputc((BYTE)(adr >> 8) & 0xff, fp);
        }
        do {
            unsigned char save_byte;

            save_byte = mon_get_mem_val(mem, (ADDRESS)(adr + ch));
            if(fwrite((char *)&save_byte, 1, 1, fp) < 1) {
                mon_out("Saving for `%s' failed.\n", filename);
                fclose(fp);
            }
            ch++;
        } while ((adr + ch) <= end);
        fclose(fp);
    }
}

/* Where is the implementation?  */
void mon_file_verify(const char *filename, MON_ADDR start_addr)
{
    mon_evaluate_default_addr(&start_addr);

    mon_out("Verify file %s at address $%04x\n",
              filename, addr_location(start_addr));
}

