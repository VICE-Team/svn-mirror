/*
 * uipalemu.c - XAW ui only, CRT emu parameters
 *
 * Written by
 *  pottendo <pottendo@gmx.net>
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

/* #define DEBUG_UI */

#ifdef DEBUG_UI
#define DBG(_x_)        log_debug _x_
#else
#define DBG(_x_)
#endif

#include "vice.h"

#include <stdio.h>

#include "lib.h"
#include "log.h"
#include "machine.h"
#include "resources.h"
#include "uiapi.h"
#include "uimenu.h"
#include "uipalemu.h"
#include "util.h"

#ifndef USE_GNOMEUI

typedef struct {
    char *name;
    char *resname;
    int scale;
} palres_t;

static palres_t palres[9] = {
    { N_("Saturation"), "ColorSaturation", 2 },
    { N_("Contrast"), "ColorContrast", 2 },
    { N_("Brightness"), "ColorBrightness", 2 },
    { N_("Gamma"), "ColorGamma", 1 },
    { N_("Tint"), "ColorTint", 2 },
    { N_("Scanline shade"), "PALScanLineShade", 4 },
    { N_("Blur"), "PALBlur", 4 },
    { N_("Odd lines phase"), "PALOddLinePhase", 2 },
    { N_("Odd lines offset"), "PALOddLineOffset", 2 },
};

typedef struct {
    char *chip;
    int idx;
} palopt_t;

static UI_CALLBACK(PAL_control_cb)
{
    char buf[50];
    char desc[0x100];
    ui_button_t button;
    long res;
    int current;

    palopt_t *p = (palopt_t *)(UI_MENU_CB_PARAM);
    int what;
    char*resource;

    what = p->idx;

    DBG(("PAL_control_cb (%s:%d)", p->chip, p->idx));

    resource = util_concat(p->chip, palres[what].resname, NULL);
    resources_get_int(resource, &current);
    current /= (10 * palres[what].scale);
    sprintf(buf, "%d", current);
    sprintf(desc, "%s %s", palres[what].name, _("in percent"));
    button = ui_input_string(palres[what].resname, desc, buf, 50);

    switch (button) {
        case UI_BUTTON_OK:
            if (util_string_to_long(buf, NULL, 10, &res) != 0) {
                ui_error(_("Invalid value: %s"), buf);
                lib_free(resource);
                return;
            }
            break;
        default:
            break;
    }

    if ((current != res) && (res <= 100) && (res >= 0)) {
        resources_set_int(resource, (int)(res * (10 * palres[what].scale)));
    }
    lib_free(resource);
}

static ui_menu_entry_t PALColor_submenu[] = {
    { N_("Saturation"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)PAL_control_cb, NULL, NULL },
    { N_("Contrast"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)PAL_control_cb, NULL, NULL },
    { N_("Brightness"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)PAL_control_cb, NULL, NULL },
    { N_("Gamma"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)PAL_control_cb, NULL, NULL },
    { N_("Tint"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)PAL_control_cb, NULL, NULL },
    { NULL }
};

#define PALColor_submenu_size	util_arraysize(PALColor_submenu)

static ui_menu_entry_t PALCrt_submenu[] = {
    { N_("Scanline shade"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)PAL_control_cb, NULL, NULL },
    { N_("Blur"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)PAL_control_cb, NULL, NULL },
    { N_("Odd lines phase"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)PAL_control_cb, NULL, NULL },
    { N_("Odd lines offset"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)PAL_control_cb, NULL, NULL },
    { NULL }
};

#define PALCrt_submenu_size	util_arraysize(PALCrt_submenu)

ui_menu_entry_t *build_color_menu(char *cp)
{
    int i;
    palopt_t *p;
    ui_menu_entry_t *m;
    
    m = lib_malloc(sizeof(ui_menu_entry_t) * PALColor_submenu_size);
    DBG(("build_color_menu %s", cp));

    /* Loop over all elements, except the last one */
    for (i = 0; i < PALColor_submenu_size - 1 ; i++) {
        p = lib_malloc(sizeof(palopt_t));
        p->chip = cp;
        p->idx = i;
        PALColor_submenu[i].callback_data = p;
    }
    memcpy (m, PALColor_submenu, sizeof(ui_menu_entry_t) * PALColor_submenu_size);
    return m;
}

ui_menu_entry_t *build_crt_menu(char *cp)
{
    int i;
    palopt_t *p;
    ui_menu_entry_t *m;

    m = lib_malloc(sizeof(ui_menu_entry_t) * PALCrt_submenu_size);
    DBG(("build_crt_menu %s", cp));

    /* Loop over all elements, except the last one */
    for (i = 0; i < PALCrt_submenu_size - 1 ; i++) {
        p = lib_malloc(sizeof(palopt_t));
        p->chip = cp;
        p->idx = i + PALColor_submenu_size - 1;
        PALCrt_submenu[i].callback_data = p;
    }
    memcpy (m, PALCrt_submenu, sizeof(ui_menu_entry_t) * PALCrt_submenu_size);
    return m;
}

void shutdown_color_menu(ui_menu_entry_t *m)
{
    int i;
    DBG(("shutdown_color_menu"));
    if (m) {
        for (i = 0; i < PALColor_submenu_size - 1 ; i++) {
            lib_free(m[i].callback_data);
        }
        lib_free(m);
    }
}

void shutdown_crt_menu(ui_menu_entry_t *m)
{
    int i;
    DBG(("shutdown_crt_menu"));
    if (m) {
        for (i = 0; i < PALCrt_submenu_size - 1 ; i++) {
            lib_free(m[i].callback_data);
        }
        lib_free(m);
    }
}

#endif
