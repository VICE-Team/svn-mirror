/*
 * prt-mps803.c
 *
 * Written by
 *  Thomas Bretz <tbretz@gsi.de>
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
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "sysfile.h"
#include "types.h"
#include "utils.h"

#define MPS803_ROM_SIZE 8192
#define MPS803_ROM_FONT_OFFSET 4096

#define MAX_COL 480

struct mps_s
{
    FILE *f;
    char line[MAX_COL][7];
    int  bitcnt;
    int  repeatn;
    int  pos;
    int  tab;
    char tabc[3];
    int  mode;
    char charset[255][7];
};

typedef struct mps_s mps_t;

#define MPS_REVERSE 0x01
#define MPS_CRSRUP  0x02
#define MPS_BITMODE 0x04
#define MPS_DBLWDTH 0x08
#define MPS_REPEAT  0x10
#define MPS_ESC     0x20

void set_mode(mps_t *mps, int m)
{
    mps->mode |= m;
}

void del_mode(mps_t *mps, int m)
{
    mps->mode &= ~m;
}

int is_mode(mps_t *mps, int m)
{
    return mps->mode&m;
}

int get_charset_bit(mps_t *mps, int nr, int col, int row)
{
    int reverse = is_mode(mps, MPS_REVERSE);
    return mps->charset[nr][row]&(1<<(7-col)) ? !reverse : reverse;
}

void print_cbm_char(mps_t *mps, char c)
{
    int y, x;

    if (is_mode(mps, MPS_CRSRUP))
        c += 128;

    for (y=0; y<7; y++)
    {
        if (is_mode(mps, MPS_DBLWDTH))
            for (x=0; x<6; x++)
            {
                mps->line[mps->pos+x*2]  [y] = get_charset_bit(mps, c, x, y);
                mps->line[mps->pos+x*2+1][y] = get_charset_bit(mps, c, x, y);
            }
        else
            for (x=0; x<6; x++)
                mps->line[mps->pos+x][y] = get_charset_bit(mps, c, x, y);
    }

    mps->pos += is_mode(mps, MPS_DBLWDTH) ? 12 : 6;
}

void write_line(mps_t *mps)
{
    int x, y;

    for (y=0; y<7; y++)
    {
        for (x=0; x<480; x++)
            fprintf(mps->f, "%c", mps->line[x][y]?'*':' ');

        fprintf(mps->f, "\n");
    }

    mps->pos=0;
}

void clear_buffer(mps_t *mps)
{
    int x;
    int y;

    for (x=0; x<480; x++)
        for (y=0; y<7; y++)
            mps->line[x][y] = 0;
}

void bitmode_off(mps_t *mps)
{
    int i, x, y;

    for (i=0; i<mps->repeatn; i++)
    {
        for (x=0; x<mps->bitcnt; x++)
            for (y=0; y<7; y++)
                mps->line[mps->pos+x][y] = mps->line[mps->pos-mps->bitcnt+x][y];

        mps->pos += mps->bitcnt;
    }
    del_mode(mps, MPS_BITMODE);
}

void print_bitmask(mps_t *mps, const char c)
{
    int y;

    for (y=0; y<7; y++)
        mps->line[mps->pos][y] = c&(1<<(6-y)) ? 1 : 0;

    mps->bitcnt++;
    mps->pos++;
}

void print_char(mps_t *mps, const char c)
{

    if (mps->pos>479)  // flush buffer
    {
        write_line(mps);
        clear_buffer(mps);
        return;
    }

    if (mps->tab)      // decode tab-number
    {
        mps->tabc[2-mps->tab] = c;

        if (mps->tab==1)
            mps->pos =
                is_mode(mps, MPS_ESC) ?
                mps->tabc[0]<<8 | mps->tabc[1] :
                atoi(mps->tabc)*6;

        mps->tab--;
        return;
    }

    del_mode(mps, MPS_ESC);

    if (is_mode(mps, MPS_REPEAT))
    {
        mps->repeatn = c;
        del_mode(mps, MPS_REPEAT);
        return;
    }

    if (is_mode(mps, MPS_BITMODE) && c&128)
    {
        print_bitmask(mps, c);
        return;
    }

    switch (c)
    {
    case 8:
        set_mode(mps, MPS_BITMODE);
        mps->bitcnt  = 0;
        return;

    case 10:  // LF
        write_line(mps);
        clear_buffer(mps);
        return;

    case 13:  // CR
        mps->pos = 0;
        del_mode(mps, MPS_CRSRUP);
        return;

        // By sending the cursor up code [CHR$(145)] to your printer, folowing charac-
        // ters will be printed in cursor up (graphic) mode until either a carriage
        // return or cursor down code [CHR$(17)] is detected.

        // By sending the cursor down code [CHR$(145)] to your printer, following
        // characters will be printed in business mode until either a carriage return or
        // cursor up code [CHR$(145)] is detected.

        // 1. GRAPHIC MODE Code & Front Table, OMITTED
        // When an old number of CHR$(34) is detected in a line, the control
        // codes $00-$1F and $80-$9F will be made visible by printing a
        // reverse character for each of these controls. This will continue until
        // an even number of quotes [CHR$(34)] has been received or until
        // end of this line.

        // 2. BUSINESS MODE Code & Font Table, OMITTED
        // When an old number of CHR$(34) is detected in a line, the control
        // codes $00-$1F and $80-$9F will be made visible by printing a
        // reverse character for each of these controls. This will continue until
        // an even number of quotes [CHR$(34)] has been received or until
        // end of this line.

    case 14:  // EN on
        set_mode(mps, MPS_DBLWDTH);
        if (is_mode(mps, MPS_BITMODE))
            bitmode_off(mps);
        return;

    case 15:  // EN off
        del_mode(mps, MPS_DBLWDTH);
        if (is_mode(mps, MPS_BITMODE))
            bitmode_off(mps);
        return;

    case 16:  // POS
        mps->tab = 2; // 2 chars (digits) following, number of first char
        return;

    case 17:   // crsr dn
        del_mode(mps, MPS_CRSRUP);
        return;

    case 18:
        set_mode(mps, MPS_REVERSE);
        return;

    case 26:   // repeat last chr$(8) c times.
        set_mode(mps, MPS_REPEAT);
        mps->repeatn = 0;
        mps->bitcnt  = 0;
        return;

    case 27:
        set_mode(mps, MPS_ESC); // followed by 16, and number MSB, LSB
        return;

    case 145: // CRSR up
        set_mode(mps, MPS_CRSRUP);
        return;

    case 146: // 18+128
        del_mode(mps, MPS_REVERSE);
        return;
    }

    if (is_mode(mps, MPS_BITMODE))
        return;

    print_cbm_char(mps, c);
}

int init_charset(mps_t *mps, const char *name)
{
    BYTE romimage[MPS803_ROM_SIZE];

    if (sysfile_load(name, romimage, MPS803_ROM_SIZE, MPS803_ROM_SIZE) < 0)
        log_error(LOG_ERR, "Could not load %s.", name);
        return -1;

    memcpy(mps->charset, &romimage[MPS803_ROM_FONT_OFFSET], 256 * 7);

    return 0;
}

int mps803bmp_open(int dev)
{
    mps_t *mps;

    mps = (mps_t *)xmalloc(sizeof(mps_t));
    memset(mps, 0, sizeof(mps_t));

    mps->f = fopen("output", "w");

    init_charset(mps, "cbm1526");

    return (void*)mps;
}

int mps803bmp_putc(void *param, int fi, BYTE b)
{
    print_char((mps_t*)param, fgetc(fi));
}

int mps803bmp_flush(void *param, int fi)
{
}

void mps803bmp_close(void *param, int fi)
{
    mps_t *mps = (mps_t*)param;
    fclose(mps->f);
    free(mps);
}

/*
printerdrv_t prt_mps803 =
{
    "MPS803BMP",
    mps803bmp_open,
    mps803bmp_putc,
    mps803bmp_flush,
    mps803bmp_close
};
*/
