/*
 * uifonts.c - common fonts related stuff
 *
 * Written by
 *  groepaz <groepaz@gmx.net>
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

#include <string.h>
#include <stdint.h>

#include "lib.h"
#include "log.h"
#include "machine.h"
#include "sysfile.h"
#include "uifonts.h"

#define MENUCHARSNUM    17
static const unsigned char uichars[8 * MENUCHARSNUM] = {
    0x3f, 0x7f, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x3f, /* 01 header left */
    0xfc, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfc, /* 02 header right */
    0x00, 0x00, 0x00, 0x1f, 0x1f, 0x00, 0x00, 0x00, /* 03 submenu arrow, left char */
    0x00, 0x80, 0xc0, 0xe0, 0xe0, 0xc0, 0x80, 0x00, /* 04 submenu arrow, right char */
    0x00, 0x38, 0x44, 0x82, 0x44, 0x38, 0x00, 0x00, /* 05 radio button inactive */
    0x00, 0x38, 0x7c, 0xfe, 0x7c, 0x38, 0x00, 0x00, /* 06 radio button active */
    0x00, 0x00, 0x3f, 0x20, 0x20, 0x25, 0x24, 0x24, /* 07 top-left edge */
    0x00, 0x00, 0xfc, 0x04, 0x04, 0xe4, 0x24, 0x24, /* 08 top-right edge */
    0x24, 0x24, 0x27, 0x20, 0x20, 0x3f, 0x00, 0x00, /* 0a bottom-left edge */
    0x24, 0x24, 0xe4, 0x04, 0x04, 0xfc, 0x00, 0x00, /* 09 bottom-right edge*/
    0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, /* 0b vertical */
    0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, /* 0c horizontal */
    0x24, 0x24, 0x27, 0x20, 0x20, 0x27, 0x24, 0x24, /* 0d right tee */
    0x24, 0x24, 0xe4, 0x04, 0x04, 0xe4, 0x24, 0x24, /* 0e left tee */
    0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, /* 0f slider inactive */
    0xff, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0xff, 0x00, /* 10 slider active */
    0xff, 0xa0, 0x50, 0xa0, 0x50, 0xa0, 0xff, 0x00, /* 11 slider active (half) */
};

void sdl_ui_copy_ui_font(uint8_t *dest)
{
    memcpy(dest, uichars, 8 * MENUCHARSNUM);
}

#define SDLFONTSIZE (256 * 8)

static unsigned char *sdlfontasc = NULL;
static unsigned char *sdlfontpet = NULL;
static unsigned char *sdlfontmon = NULL;

/* FIXME: tweak to match the appearence of the other chars */
static unsigned char sdlextrachars[8 * 4] = {
    0x0c, 0x18, 0x18, 0x30, 0x18, 0x18, 0x0c, 0x00, /* { */
    0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, /* | */
    0x30, 0x18, 0x18, 0x0c, 0x18, 0x18, 0x30, 0x00, /* } */
    0x60, 0x92, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, /* ~ */
};

static int loadchar(FILE *f, int initial_offset, int asc_offset, char load_every_second_char)
{
    unsigned int i;
    int res = 0;

    for(i = 0; i < SDLFONTSIZE; i+=2) {
        sdlfontasc[i] = 0x55;
        sdlfontasc[i + 1] = 0xaa;
    }
    memcpy(sdlfontpet, sdlfontasc, SDLFONTSIZE);
    memcpy(sdlfontmon, sdlfontasc, SDLFONTSIZE);

    fseek(f, initial_offset, SEEK_SET);
    if (load_every_second_char) {
       for(i = 0; i < 128; i++) {
           if(fread(&sdlfontpet[i * 8], 1, 8, f) != 8) {
                res = -1;
            }
            fseek(f, 8, SEEK_CUR);
        }
    } else {
        if(fread(sdlfontpet, 1, 0x400, f) != 0x400) {
            res = -1;
        }
    }
    fseek(f, asc_offset, SEEK_SET);
    if (load_every_second_char) {
       for(i = 0; i < 128; i++) {
           if(fread(&sdlfontmon[i * 8], 1, 8, f) != 8) {
                res = -1;
            }
            fseek(f, 8, SEEK_CUR);
        }
    } else {
        if(fread(sdlfontmon, 1, 0x400, f) != 0x400) {
            res = -1;
        }
    }
    fseek(f, asc_offset, SEEK_SET);
    if (load_every_second_char) {
       for(i = 0; i < 128 - 37; i++) {
           if(fread(&sdlfontasc[i * 8], 1, 8, f) != 8) {
                res = -1;
            }
            fseek(f, 8, SEEK_CUR);
        }
    } else {
        if(fread(sdlfontasc, 1, 0x400 - (8 * 37), f) != (0x400 - (8 * 37))) {
            res = -1;
        }
    }
    fclose(f);

    /* create inverted second half */
    for(i = 0; i < 0x400; i++) {
        sdlfontpet[i + 0x400] = sdlfontpet[i] ^ 0xff;
        sdlfontmon[i + 0x400] = sdlfontmon[i] ^ 0xff;
        /* sdlfontasc[i + 0x400] = sdlfontasc[i] ^ 0xff; */
    }
    return res;
}

int sdl_ui_font_init(const char *name, int initial_offset, int asc_offset, char load_every_second_char)
{
    int i;
    FILE *f = sysfile_open(name, machine_name, NULL, "rb");

    if (f == NULL) {
        log_error(LOG_DEFAULT, "could not locate menu charset `%s'.\n", name ? name : "(null)");
        return -1;
    }

    sdlfontasc = lib_malloc(SDLFONTSIZE);
    sdlfontpet = lib_malloc(SDLFONTSIZE);
    sdlfontmon = lib_malloc(SDLFONTSIZE);

    if (loadchar(f, initial_offset, asc_offset, load_every_second_char) != 0) {
        log_error(LOG_DEFAULT, "could not load menu charset '%s'.\n", name ? name : "(null)");
    }

    /* 1f copy _ to arrow-left */
    memset(&sdlfontasc[0x1f * 8], 0, 8);
    sdlfontasc[0x1f * 8 + 6] = 0xff;
    /* 1e fix ^ */
    for (i = 3; i < 8; i++) {
        sdlfontasc[0x1e * 8 + i] &= ~0x18;
    }
    /* 1c fix \ */
    for (i = 0; i < 7; i++) {
        sdlfontasc[0x1c * 8 + i] = sdlfontasc[0x2f * 8 + (7 - i)];
    }
    /* 40 fix ' */
    for (i = 0; i < 3; i++) {
        sdlfontasc[0x40 * 8 + i] = sdlfontasc[0x27 * 8 + (3 - i)];
        sdlfontasc[0x40 * 8 + i + 3] = 0;
    }
   /* fix 7b..7f */
    memcpy(&sdlfontasc[0x5b * 8], &sdlextrachars[0], 8 * 4);

    /* TODO: add extended ascii codes 80-ff */

    /* extra menu gfx goes into unprintable codes 0..1f */
    sdl_ui_copy_ui_font(&sdlfontasc[0x81 * 8]);

    /* init menu font last, since set_menu_font will also make the font active */
    sdl_ui_set_monitor_font(sdlfontmon, 8, 8);
    sdl_ui_set_image_font(sdlfontpet, 8, 8);
    sdl_ui_set_menu_font(sdlfontasc, 8, 8);
    
    return 0;
}

void sdl_ui_font_shutdown(void)
{
    lib_free(sdlfontasc);
    sdlfontasc = NULL;
    lib_free(sdlfontpet);
    sdlfontpet = NULL;
    lib_free(sdlfontmon);
    sdlfontmon = NULL;
}
