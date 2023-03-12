/*
 * drv-mps803.c - MPS803 printer driver.
 *
 * Written by
 *  Thomas Bretz <tbretz@gsi.de>
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

/* #define DEBUG_MPS803 */

#include "vice.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "archdep.h"
#include "driver-select.h"
#include "drv-mps803.h"
#include "log.h"
#include "output-select.h"
#include "output.h"
#include "palette.h"
#include "printer.h"
#include "sysfile.h"
#include "types.h"

#ifdef DEBUG_MPS803
#define DBG(x) log_debug x
#else
#define DBG(x)
#endif

#define PAGE_HEIGHT_CHARACTERS  66

#define PAGE_WIDTH_DOTS     480
#define PAGE_HEIGHT_DOTS    (PAGE_HEIGHT_CHARACTERS * 10)

#define MPS_REVERSE  0x01
#define MPS_CRSRUP   0x02 /* set in gfxmode (default) unset in businessmode */
#define MPS_BITMODE  0x04
#define MPS_DBLWDTH  0x08
#define MPS_REPEAT   0x10
#define MPS_ESC      0x20
#define MPS_QUOTED   0x40 /* odd number of quotes in line (textmode) */

#define MPS_BUSINESS 0x80 /* opened with SA = 7 in business mode */
#define MPS_GRAPHICS 0x00 /* opened with SA = 0 in graphics mode */

struct mps_s {
    uint8_t line[PAGE_WIDTH_DOTS][7];
    int repeatn;
    int pos;
    int tab;
    uint8_t tabc[3];
    int begin_line;
    int mode_global;            /* global mode */
#if 0
    /* FIXME: index is the secondary file number */
    int chargen_select[0x100];
#endif
};
typedef struct mps_s mps_t;

static uint8_t charset[512][7];

static mps_t drv_mps803[NUM_OUTPUT_SELECT];
static palette_t *palette = NULL;

/* Logging goes here.  */
static log_t drv803_log = LOG_ERR;

#ifdef DEBUG_MPS803
static void dump_charset(void)
{
    int ch, row, col, bit;
    for (ch = 0; ch < 512; ch++) {
        printf("%d\n", ch);
        for (row = 0; row < 7; row++) {
            for (col = 0; col < 7; col++) {
                bit = charset[ch][row] & (1 << (7 - col));
                printf("%c", bit ? '*' : '.');
            }
            printf("\n");
        }
    }
}
#endif

/* ------------------------------------------------------------------------- */
/* MPS803 printer engine. */

/*
    "if the logical file number is > 127, any printed lines will be double spaced."

    secondary address 7 -> business mode

    *   The MPS801 is mostly the same as MPS803 Manual, however regarding SA it
    *   says this:
    *
    *   Sa= 0: Print data exactly as received
    *   Sa= 6: Setting spacing between lines
    *   Sa= 7: Select business mode
    *   Sa= S: Select graphic mode
    *   Sa=10: Reset the printer
    *
    *   Interestingly, in another Version it is this:
    *
    *   SA = O: "GRAPH IC" (upper case/graphic) Mode
    *   SA = 7: "BUSINESS" (lower/upper case) Mode


    The manual specifies the following printer control codes:

    10      LF (line feed)
            - all data in buffer is printed and paper is advanced one line
    13      CR (carriage return)
            - all data in buffer is printed and paper is advanced one line
            - turns off reverse and quote mode

    14      enhanced character set ON
            - following characters are 12 pixel wide, 7 pixel high
            - cancels bit image printing, switches to 1/16 inch line feed
    15      enhanced character set OFF
            - following characters are 6 pixel wide, 7 pixel high
            - cancels bit image printing, switches to 1/16 inch line feed

    145     print in graphics mode
            - functions until chr$(17) or CR
    17      print in business mode
            - functions until chr$(145) or CR

    18      reverse ON
            - cancelled by CR
    146     reverse OFF

    16      TAB setting - chr$(16);"nHnL"
            - moves printer head to absolute (char) position in the line
    27      specify dot address - chr$(27);chr$(16);chr$(nH)chr$(nL)
            - set head position in dot units

    8       bit image printing

    26      repeat graphic selected - chr$(26);chr$(image data)

    34      quote
            - if an odd number of quotation marks have been transmitted, control
              characters are mode visible (quote mode = ON)
            - quote mode is cancelled by CR

    line feeds are:
    - 6 lines per inch in character- and double width character mode
    - 9 lines per inch in bit image graphic print mode

    automatic printing:

    - automatic printing occurs under 3 conditions:

    a) when buffer fills up during input of data
    b) when printer "sees" that you have used more than 480 dots per line (chars
       and spaces count as 6 dots each)
    c) when a) and b) happen at the same time

    manual is a bit unclear about how much of the buffer will be printed

    Open Questions:
        - what happens when using other secondary addresses than 0 and 7 ?
        - what is/are the exact conditions when the chargen select is reset to
          what the secondary address implies?
        - is the local state of chargen select (when changed via control chars)
          preserved when printing one line, but using different secondary
          addresses?
 */

static void set_global_mode(mps_t *mps, unsigned int m)
{
    mps->mode_global |= m;
}

static void unset_global_mode(mps_t *mps, unsigned int m)
{
    mps->mode_global &= ~m;
}

static int get_global_mode(mps_t *mps, unsigned int m)
{
    return mps->mode_global & m;
}

/* FIXME: it is not quite clear how this works exactly */
static int get_chargen_mode(mps_t *mps, unsigned int secondary)
{
    return (secondary == 7) ? MPS_BUSINESS : MPS_GRAPHICS;
    /* return mps->chargen_select[secondary]; */
}

#if 0
static void set_chargen_mode(mps_t *mps, unsigned int secondary, unsigned int m)
{
    mps->chargen_select[secondary] = m;
}
#endif

/******************************************************************************/

static int get_charset_bit(mps_t *mps, int nr, unsigned int col,
                           unsigned int row)
{
    int reverse, result;

    reverse = get_global_mode(mps, MPS_REVERSE);

    result = charset[nr][row] & (1 << (7 - col)) ? !reverse : reverse;

    return result;
}

static void print_cbm_char(mps_t *mps, const uint8_t rawchar)
{
    unsigned int y, x;
    int c, err = 0;

    c = (int)rawchar;

    /* in the ROM, graphics charset comes first, then business */
    if (!get_global_mode(mps, MPS_CRSRUP)) {
        c += 256;
    }

    for (y = 0; y < 7; y++) {
        if (get_global_mode(mps, MPS_DBLWDTH)) {
            for (x = 0; x < 6; x++) {
                if ((mps->pos + x * 2) >= PAGE_WIDTH_DOTS) {
                    err = 1;
                    break;
                }
                mps->line[mps->pos + x * 2][y] = get_charset_bit(mps, c, x, y);
                if ((mps->pos + x * 2 + 1) >= PAGE_WIDTH_DOTS) {
                    err = 1;
                    break;
                }
                mps->line[mps->pos + x * 2 + 1][y] = get_charset_bit(mps, c, x, y);
            }
        } else {
            for (x = 0; x < 6; x++) {
                if ((mps->pos + x) >= PAGE_WIDTH_DOTS) {
                    err = 1;
                    break;
                }
                mps->line[mps->pos + x][y] = get_charset_bit(mps, c, x, y);
            }
        }
    }

    if (err) {
        log_error(drv803_log, "Printing beyond limit of %d dots.", PAGE_WIDTH_DOTS);
    }

    mps->pos += get_global_mode(mps, MPS_DBLWDTH) ? 12 : 6;
}

static void write_line(mps_t *mps, unsigned int prnr)
{
    int x, y;

    for (y = 0; y < 7; y++) {
        for (x = 0; x < 480; x++) {
            output_select_putc(prnr, (uint8_t)(mps->line[x][y]
                                            ? OUTPUT_PIXEL_BLACK : OUTPUT_PIXEL_WHITE));
        }
        output_select_putc(prnr, (uint8_t)(OUTPUT_NEWLINE));
    }

    if (!get_global_mode(mps, MPS_BITMODE)) {
        /* bitmode:  9 rows/inch (7lines/row * 9rows/inch=63 lines/inch) */
        /* charmode: 6 rows/inch (7lines/row * 6rows/inch=42 lines/inch) */
        /*   --> 63lines/inch - 42lines/inch = 21lines/inch missing */
        /*   --> 21lines/inch / 9row/inch = 3lines/row missing */
        output_select_putc(prnr, OUTPUT_NEWLINE);
        output_select_putc(prnr, OUTPUT_NEWLINE);
        output_select_putc(prnr, OUTPUT_NEWLINE);
    }

    mps->pos = 0;
}

static void clear_buffer(mps_t *mps)
{
    unsigned int x, y;

    for (x = 0; x < PAGE_WIDTH_DOTS; x++) {
        for (y = 0; y < 7; y++) {
            mps->line[x][y] = 0;
        }
    }
}

static void print_bitmask(mps_t *mps, unsigned int prnr, const char c)
{
    unsigned int y;
    unsigned int i;

    if (!mps->repeatn) {
        mps->repeatn=1;
    }

    for (i = 0; i < (unsigned int)(mps->repeatn); i++) {
        if (mps->pos >= PAGE_WIDTH_DOTS) {  /* flush buffer*/
            write_line(mps, prnr);
            clear_buffer(mps);
        }
    for (y = 0; y < 7; y++) {
        mps->line[mps->pos][y] = c & (1 << (y)) ? 1 : 0;
    }

    mps->pos++;
    }
    mps->repeatn=0;
}

static void print_char(mps_t *mps, unsigned int prnr, unsigned int secondary, const uint8_t c)
{
    static int last_secondary = -1;

    if (mps->tab) {     /* decode tab-number*/
        mps->tabc[2 - mps->tab] = c;

        if (mps->tab == 1) {
            mps->pos =
                get_global_mode(mps, MPS_ESC) ?
                mps->tabc[0] << 8 | mps->tabc[1] :
                atoi((char *)mps->tabc) * 6;

            unset_global_mode(mps, MPS_ESC);
        }

        mps->tab--;
        return;
    }

    if (get_global_mode(mps, MPS_ESC) && (c != 16)) {
        unset_global_mode(mps, MPS_ESC);
    }

    if (get_global_mode(mps, MPS_REPEAT)) {
        mps->repeatn = c;
        unset_global_mode(mps, MPS_REPEAT);
        return;
    }

    if (get_global_mode(mps, MPS_BITMODE) && (c & 128)) {
        print_bitmask(mps, prnr, c);
        return;
    }

    /* FIXME: it is not quite clear under what condition the chargen mode is
              reset to what the secondary address implies. right now we do it
              under the following conditions:
              - the current character is the first character in a logical line,
                ie it follows a CR
              - the secondary address is different to the secondary address used
                for the last character.
    */

    if (mps->begin_line || (last_secondary != secondary)) {
        if (get_chargen_mode(mps, secondary) == MPS_BUSINESS) {
            unset_global_mode(mps, MPS_CRSRUP);
        } else {
            set_global_mode(mps, MPS_CRSRUP);
        }
    }
    mps->begin_line = 0;
    last_secondary = secondary;

    /* it seems that CR works even in quote mode */
    switch (c) {
        case 13: /* CR*/
            mps->pos = 0;
#if 0
            if (get_chargen_mode(mps, secondary) == MPS_BUSINESS) {
                unset_global_mode(mps, MPS_CRSRUP);
            } else {
                set_global_mode(mps, MPS_CRSRUP);
            }
#endif
            /* CR resets Quote mode, revers mode, ... */
            unset_global_mode(mps, MPS_QUOTED);
            unset_global_mode(mps, MPS_REVERSE);
            write_line(mps, prnr);
            clear_buffer(mps);
            mps->begin_line = 1;
            return;
    }

    /* in text mode ignore most (?) other control chars when quote mode is active */
    if (!get_global_mode(mps, MPS_QUOTED) || get_global_mode(mps, MPS_BITMODE)) {

        switch (c) {
            case 8:
                set_global_mode(mps, MPS_BITMODE);
                return;

            case 10: /* LF*/
                write_line(mps, prnr);
                clear_buffer(mps);
                return;

#ifdef notyet
            /* Not really sure if the MPS803 recognizes this one... */
            case 13 + 128: /* shift CR: CR without LF (from 4023 printer) */
                mps->pos = 0;
                if (get_chargen_mode(mps, secondary) == MPS_BUSINESS) {
                    unset_global_mode(mps, MPS_CRSRUP);
                } else {
                    set_global_mode(mps, MPS_CRSRUP);
                }
                /* CR resets Quote mode, revers mode, ... */
                unset_global_mode(mps, MPS_QUOTED);
                unset_global_mode(mps, MPS_REVERSE);
                return;
#endif

            case 14: /* EN on*/
                set_global_mode(mps, MPS_DBLWDTH);
                if (get_global_mode(mps, MPS_BITMODE)) {
                    unset_global_mode(mps, MPS_BITMODE);
                }
                return;

            case 15: /* EN off*/
                unset_global_mode(mps, MPS_DBLWDTH);
                if (get_global_mode(mps, MPS_BITMODE)) {
                    unset_global_mode(mps, MPS_BITMODE);
                }
                return;

            case 16: /* POS*/
                mps->tab = 2; /* 2 chars (digits) following, number of first char*/
                return;

            /*
            * By sending the cursor up code [CHR$(145)] to your printer, following
            * characters will be printed in cursor up (graphic) mode until either
            * a carriage return or cursor down code [CHR$(17)] is detected.
            *
            * By sending the cursor down code [CHR$(17)] to your printer,
            * following characters will be printed in business mode until either
            * a carriage return or cursor up code [CHR$(145)] is detected.
            */
            case 17: /* crsr dn, enter businessmode local */
                unset_global_mode(mps, MPS_CRSRUP);
                return;

            case 145: /* CRSR up, enter gfxmode local */
                set_global_mode(mps, MPS_CRSRUP);
                return;

            case 18:
                set_global_mode(mps, MPS_REVERSE);
                return;

            case 146: /* 18+128*/
                unset_global_mode(mps, MPS_REVERSE);
                return;

            case 26: /* repeat last chr$(8) c times.*/
                set_global_mode(mps, MPS_REPEAT);
                mps->repeatn = 1;
                return;

            case 27:
                set_global_mode(mps, MPS_ESC); /* followed by 16, and number MSB, LSB*/
                return;
        }

    }

    if (get_global_mode(mps, MPS_BITMODE)) {
        return;
    }

   /*
    * When an odd number of CHR$(34) is detected in a line, the control
    * codes $00-$1F and $80-$9F will be made visible by printing a
    * reverse character for each of these controls. This will continue
    * until an even number of quotes [CHR$(34)] has been received or until
    * end of this line.
    */
    if (c == 34) {
        mps->mode_global ^= MPS_QUOTED;
    }

    if (mps->pos >= PAGE_WIDTH_DOTS) {  /* flush buffer*/
        write_line(mps, prnr);
        clear_buffer(mps);
    }

    if (get_global_mode(mps, MPS_QUOTED)) {
        if (c <= 0x1f) {
            set_global_mode(mps, MPS_REVERSE);
            print_cbm_char(mps, (uint8_t)(c + 0x40));
            unset_global_mode(mps, MPS_REVERSE);
            return;
        }
        if ((c >= 0x80) && (c <= 0x9f)) {
            set_global_mode(mps, MPS_REVERSE);
            print_cbm_char(mps, (uint8_t)(c - 0x20));
            unset_global_mode(mps, MPS_REVERSE);
            return;
        }
    }

    print_cbm_char(mps, c);
}

static int init_charset(uint8_t chrset[512][7], const char *name)
{
    uint8_t romimage[MPS803_ROM_SIZE];

    if (sysfile_load(name, "PRINTER", romimage, MPS803_ROM_SIZE, MPS803_ROM_SIZE) < 0) {
        log_error(drv803_log, "Could not load MPS-803 charset '%s'.", name);
        return -1;
    }

    memcpy(chrset, romimage, MPS803_ROM_SIZE);

#ifdef DEBUG_MPS803
    dump_charset();
#endif
    return 0;
}

/* ------------------------------------------------------------------------- */
/* Interface to the upper layer.  */

static int drv_mps803_open(unsigned int prnr, unsigned int secondary)
{
    /*
     *  sa = 0: graphic mode.. . (default)
     *  sa = 7: business mode
     * This is *probably* incorrect: I suspect it happens anew for every
     * OPEN CHANNEL SA (each PRINT# statement). Or maybe the state is
     * even remembered for each SA separately.
     */
    if (secondary == 0) {
        /* set_chargen_mode(&drv_mps803[prnr], secondary, MPS_GRAPHICS); */
        set_global_mode(&drv_mps803[prnr], MPS_CRSRUP);
    } else if (secondary == 7) {
        /* set_chargen_mode(&drv_mps803[prnr], secondary, MPS_BUSINESS); */
        unset_global_mode(&drv_mps803[prnr], MPS_CRSRUP);
    } else if (secondary == DRIVER_FIRST_OPEN) {
        /* Is this the first open? */
        output_parameter_t output_parameter;

        output_parameter.maxcol = PAGE_WIDTH_DOTS;
        output_parameter.maxrow = PAGE_HEIGHT_DOTS;
        output_parameter.dpi_x = 60;    /* mps803 has different horizontal & vertical dpi - see pg 49 of the manual part H. */
        output_parameter.dpi_y = 72;    /* NOTE - mixed dpi might not be liked by some image viewers */
        output_parameter.palette = palette;

        return output_select_open(prnr, &output_parameter);
    } else {
        /* any other secondary address */
        log_warning(LOG_DEFAULT, "FIXME: secondary address is %u - what does the real MPS803 do in this case?", secondary);
    }

    return 0;
}

static void drv_mps803_close(unsigned int prnr, unsigned int secondary)
{
    output_select_close(prnr);
}

/*
 * We would like to have calls for LISTEN and UNLISTEN as well...
 * this may be important for emulating the proper cursor up/down
 * mode associated with SA=0 or 7.
 */

static int drv_mps803_putc(unsigned int prnr, unsigned int secondary, uint8_t b)
{
    DBG(("drv_mps803_putc(%u,%u:$%02x)", prnr, secondary, b));
    print_char(&drv_mps803[prnr], prnr, secondary, b);
    return 0;
}

static int drv_mps803_getc(unsigned int prnr, unsigned int secondary, uint8_t *b)
{
    DBG(("drv_mps803_getc(%u,%u)", prnr, secondary));
    return output_select_getc(prnr, b);
}

static int drv_mps803_flush(unsigned int prnr, unsigned int secondary)
{
    DBG(("drv_mps803_flush(%u,%u)", prnr, secondary));
    return output_select_flush(prnr);
}

static int drv_mps803_formfeed(unsigned int prnr)
{
    DBG(("drv_mps803_formfeed(%u)", prnr));
    return output_select_formfeed(prnr);;
}

int drv_mps803_init_resources(void)
{
    driver_select_t driver_select;

    driver_select.drv_name = "mps803";
    driver_select.drv_open = drv_mps803_open;
    driver_select.drv_close = drv_mps803_close;
    driver_select.drv_putc = drv_mps803_putc;
    driver_select.drv_getc = drv_mps803_getc;
    driver_select.drv_flush = drv_mps803_flush;
    driver_select.drv_formfeed = drv_mps803_formfeed;

    driver_select_register(&driver_select);

    return 0;
}

int drv_mps803_init(void)
{
    const char *color_names[2] = {"Black", "White"};

    drv803_log = log_open("MPS-803");

    init_charset(charset, MPS803_ROM_NAME);

    palette = palette_create(2, color_names);

    if (palette == NULL) {
        return -1;
    }

    if (palette_load("mps803.vpl", "PRINTER", palette) < 0) {
        log_error(drv803_log, "Cannot load palette file `%s'.",
                  "mps803.vpl");
        return -1;
    }

    return 0;
}

void drv_mps803_shutdown(void)
{
    DBG(("drv_mps803_shutdown"));
    palette_free(palette);
}
