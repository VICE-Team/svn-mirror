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
#include "attach.h"
#include "mem.h"
#include "montypes.h"
#include "mon_util.h"
#include "serial.h"
#include "uimon.h"
#include "vdrive-iec.h"
#include "vdrive.h"


#define ADDR_LIMIT(x) ((ADDRESS)(LO16(x)))


static FILE *fp;
static vdrive_t *vdrive;


static int mon_file_open(const char *filename, unsigned int secondary,
                         int device)
{
    switch (device) {
      case 0:
        if (secondary == 0)
            fp = fopen(filename, MODE_READ);
        else
            fp = fopen(filename, MODE_WRITE);
        break;
      case 8:
      case 9:
      case 10:
      case 11:
        vdrive = file_system_get_vdrive((unsigned int)device);
        if (vdrive == NULL)
            return -1;

        if (vdrive_iec_open(vdrive, filename, (int)strlen(filename),
            secondary) != SERIAL_OK)
            return -1;
        break;
    }
    return 0;
}

static int mon_file_read(BYTE *data, unsigned int secondary, int device)
{
    switch (device) {
      case 0:
        if (fread((char *)data, 1, 1, fp) < 1)
            return -1;
        break;
      case 8:
      case 9:
      case 10:
      case 11:
        if (vdrive_iec_read(vdrive, data, secondary) != SERIAL_OK)
            return -1;
        break;
    }
    return 0;
}

static int mon_file_write(BYTE data, unsigned int secondary, int device)
{
    switch (device) {
      case 0:
        if (fwrite((char *)&data, 1, 1, fp) < 1)
            return -1;
        break;
      case 8:
      case 9:
      case 10:
      case 11:
        if (vdrive_iec_write(vdrive, data, secondary) != SERIAL_OK)
            return -1;
        break;
    }
    return 0;
}

static int mon_file_close(unsigned int secondary, int device)
{
    switch (device) {
      case 0:
        if (fclose(fp) != 0)
            return -1;
        break;
      case 8:
      case 9:
      case 10:
      case 11:
        if (vdrive_iec_close(vdrive, secondary) != SERIAL_OK)
            return -1;
        break;
    }
    return 0;
}


void mon_file_load(const char *filename, int device, MON_ADDR start_addr,
                   bool is_bload)
{
    ADDRESS adr;
    BYTE b1 = 0, b2 = 0;
    int ch = 0;
    MEMSPACE mem;

    if (mon_file_open(filename, 0, device) < 0) {
        mon_out("Cannot open %s.\n", filename);
        return;
    }

    if (is_bload == FALSE) {
        mon_file_read(&b1, 0, device);
        mon_file_read(&b2, 0, device);
    }

    mon_evaluate_default_addr(&start_addr);

    if (!mon_is_valid_addr(start_addr)) {   /* No Load address given */
        if (is_bload == TRUE) {
            mon_out("No LOAD address given.\n");
            mon_file_close(0, device);
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

        if (mon_file_read(&load_byte, 0, device) < 0)
            break;
        mon_set_mem_val(mem, ADDR_LIMIT(adr + ch), load_byte);
        ch ++;
    } while(1);
    
    mon_out("%x bytes\n", ch);

    if (is_bload == FALSE && (adr & 0xff) == 1) {
        /* set end of load addresses like kernal load */
        mem_set_basic_text(adr, (ADDRESS)(adr + ch));
    }

    mon_file_close(0, device);
}

void mon_file_save(const char *filename, int device, MON_ADDR start_addr,
                   MON_ADDR end_addr, bool is_bsave)
{
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

    if (mon_file_open(filename, 1, device) < 0) {
        mon_out("Cannot open %s.\n", filename);
        return;
    }

    printf("Saving file `%s'...\n", filename);

    if (is_bsave == FALSE) {
        if (mon_file_write((BYTE)adr & 0xff, 1, device) < 0
            || mon_file_write((BYTE)(adr >> 8) & 0xff, 1, device) < 0) {
            mon_out("Saving for `%s' failed.\n", filename);
            mon_file_close(1, device);
            return;
        }
    }

    do {
        unsigned char save_byte;

        save_byte = mon_get_mem_val(mem, (ADDRESS)(adr + ch));
        if (mon_file_write(save_byte, 1, device) < 0) {
            mon_out("Saving for `%s' failed.\n", filename);
            break;
        }
        ch++;
    } while ((adr + ch) <= end);

    mon_file_close(1, device);
}

/* Where is the implementation?  */
void mon_file_verify(const char *filename, int device, MON_ADDR start_addr)
{
    mon_evaluate_default_addr(&start_addr);

    mon_out("Verify file %s at address $%04x\n",
              filename, addr_location(start_addr));
}

