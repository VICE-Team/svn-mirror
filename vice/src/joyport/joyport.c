/*
 * joyport.c - joystick port handling.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

#include <string.h>

#include "cmdline.h"
#include "joyport.h"
#include "lib.h"
#include "resources.h"
#include "translate.h"
#include "uiapi.h"
#include "util.h"

static joyport_t joyport_device[JOYPORT_MAX_DEVICES];
static BYTE joyport_display[3] = { 0, 0, 0};

static int joy_port[2] = { JOYPORT_ID_NONE , JOYPORT_ID_NONE };
static int joy_pot_present = 0;
static int pot_port_mask = 1;
static int max_ports = 2;
static int joyport_set_done = 0;

typedef struct resid2transid_s {
    int resid;
    int transid;
} resid2transid_t;

static resid2transid_t ids[] = {
    { JOYPORT_RES_ID_MOUSE, IDGS_HOST_MOUSE },
    { JOYPORT_RES_ID_SAMPLER, IDGS_HOST_SAMPLER },
    { -1, -1 }
};

static char *res2text(int id)
{
    int i;
    char *retval = "Unknown joyport resource";

    for (i = 0; ids[i].resid != -1; ++i) {
        if (ids[i].resid == id) {
            retval = translate_text(ids[i].transid);
        }
    }
    return retval;
}

void set_joyport_pot_mask(int mask)
{
    pot_port_mask = mask;
}

static int joyport_set_device(int port, int id)
{
    /* 1st some sanity checks */
    if (id < JOYPORT_ID_NONE || id >= JOYPORT_MAX_DEVICES) {
        return -1;
    }
    if (port < 0 || port > max_ports) {
        return -1;
    }

    /* Nothing changes */
    if (id == joy_port[port]) {
        return 0;
    }

    /* check if id is registered */
    if (id != JOYPORT_ID_NONE && !joyport_device[id].name) {
        ui_error(translate_text(IDGS_SELECTED_JOYPORT_DEV_NOT_REG), id);
        return -1;
    }

    /* check if id conflicts with device on the other port */
    if (id != JOYPORT_ID_NONE && max_ports != 1 && joy_port[!port] == id) {
        ui_error(translate_text(IDGS_SELECTED_JOYPORT_DEV_ALREADY_ATTACHED), joyport_device[id].name, port + 1, (!port) + 1);
        return -1;
    }

    /* check if input resource conflicts with device on the other port */
    if (id != JOYPORT_ID_NONE && max_ports != 1 && joyport_device[id].resource_id != JOYPORT_RES_ID_NONE && joyport_device[id].resource_id == joyport_device[joy_port[!port]].resource_id) {
        ui_error(translate_text(IDGS_SELECTED_JOYPORT_SAME_INPUT_RES), joyport_device[id].name, port + 1, res2text(joyport_device[id].resource_id), (!port) + 1);
        return -1;
    }

    /* check if device can be connected to this port */
    if (id != JOYPORT_ID_NONE && !(joyport_device[id].port_mask & (1 << port))) {
        ui_error(translate_text(IDGS_SELECTED_DEVICE_NOT_THIS_PORT), joyport_device[id].name, port + 1);
        return -1;
    }

    /* all checks done, now disable the current device and enable the new device */
    if (joyport_device[joy_port[port]].enable) {
        joyport_device[joy_port[port]].enable(0);
    }
    if (joyport_device[id].enable) {
        joyport_device[id].enable(id);
    }
    joy_port[port] = id;

    return 0;
}

BYTE read_joyport_dig(int port)
{
    int id = joy_port[port];

    if (id == JOYPORT_ID_NONE) {
        return 0xff;
    }

    if (!joyport_device[id].read_digital) {
        return 0xff;
    }
    return joyport_device[id].read_digital();
}

void store_joyport_dig(int port, BYTE val)
{
    int id = joy_port[port];

    if (id == JOYPORT_ID_NONE) {
        return;
    }

    if (!joyport_device[id].store_digital) {
        return;
    }

    joyport_device[id].store_digital(val);
}

BYTE read_joyport_potx(void)
{
    int id1 = JOYPORT_ID_NONE;
    int id2 = JOYPORT_ID_NONE;
    BYTE ret1 = 0xff;
    BYTE ret2 = 0xff;

    if (!joy_pot_present) {
        return 0xff;
    }

    if (pot_port_mask == 1 || pot_port_mask == 3) {
        id1 = joy_port[JOYPORT_1];
    }

    if (pot_port_mask == 2 || pot_port_mask == 3) {
        id2 = joy_port[JOYPORT_2];
    }

    if (id1 != JOYPORT_ID_NONE) {
        if (joyport_device[id1].read_potx) {
            ret1 = joyport_device[id1].read_potx();
        }
    }

    if (id2 != JOYPORT_ID_NONE) {
        if (joyport_device[id2].read_potx) {
            ret2 = joyport_device[id2].read_potx();
        }
    }

    switch (pot_port_mask) {
        case 1:
            return ret1;
        case 2:
            return ret2;
        case 3:
            return ret1 & ret2;
    }

    return 0xff;
}

BYTE read_joyport_poty(void)
{
    int id1 = JOYPORT_ID_NONE;
    int id2 = JOYPORT_ID_NONE;
    BYTE ret1 = 0xff;
    BYTE ret2 = 0xff;

    if (!joy_pot_present) {
        return 0xff;
    }

    if (pot_port_mask == 1 || pot_port_mask == 3) {
        id1 = joy_port[JOYPORT_1];
    }

    if (pot_port_mask == 2 || pot_port_mask == 3) {
        id2 = joy_port[JOYPORT_2];
    }

    if (id1 != JOYPORT_ID_NONE) {
        if (joyport_device[id1].read_poty) {
            ret1 = joyport_device[id1].read_poty();
        }
    }

    if (id2 != JOYPORT_ID_NONE) {
        if (joyport_device[id2].read_poty) {
            ret2 = joyport_device[id2].read_poty();
        }
    }

    switch (pot_port_mask) {
        case 1:
            return ret1;
        case 2:
            return ret2;
        case 3:
            return ret1 & ret2;
    }

    return 0xff;
}

int joyport_register(int id, joyport_t *device)
{
    if (!joyport_set_done) {
        joyport_set_done = 1;
        memset(joyport_device, 0, sizeof(joyport_device));
        joyport_device[0].name = "None";
        joyport_device[0].trans_name = IDGS_NONE;
    }
    if (id < 1 || id > JOYPORT_MAX_DEVICES) {
        return -1;
    }

    /* skip pot devices if not pot is present */
    if ((device->read_potx || device->read_poty) && !joy_pot_present) {
        return 0;
    }

    joyport_device[id].name = device->name;
    joyport_device[id].trans_name = device->trans_name;
    joyport_device[id].resource_id = device->resource_id;
    joyport_device[id].port_mask = device->port_mask;
    joyport_device[id].enable = device->enable;
    joyport_device[id].read_digital = device->read_digital;
    joyport_device[id].store_digital = device->store_digital;
    joyport_device[id].read_potx = device->read_potx;
    joyport_device[id].read_poty = device->read_poty;
    return 0;
}

joyport_desc_t *joyport_get_valid_devices(int port)
{
    joyport_desc_t *retval = NULL;
    int i;
    int valid = 0;
    int j = 0;

    for (i = 0; i < JOYPORT_MAX_DEVICES; ++i) {
        if (joyport_device[i].name) {
            ++valid;
        }
    }

    retval = lib_malloc((valid + 1) * sizeof(joyport_desc_t));
    for (i = 0; i < JOYPORT_MAX_DEVICES; ++i) {
        if (joyport_device[i].name) {
            if (joyport_device[i].port_mask & (1 << port)) {
                retval[j].name = joyport_device[i].name;
                retval[j].trans_name = joyport_device[i].trans_name;
                retval[j].id = i;
                ++j;
            }
        }
    }
    retval[j].name = NULL;

    return retval;
}

void joyport_display_joyport(int id, BYTE status)
{
    if (id != joy_port[0] && id != joy_port[1]) {
        return;
    }

    if (id == joy_port[0]) {
        joyport_display[1] = status;
    }

    if (id == joy_port[1]) {
        joyport_display[2] = status;
    }

    ui_display_joyport(joyport_display);
}

/* ------------------------------------------------------------------------- */

static int set_joyport1_device(int val, void *param)
{
    return joyport_set_device(0, val);
}

static int set_joyport2_device(int val, void *param)
{
    return joyport_set_device(1, val);
}

static const resource_int_t resources_int_port1[] = {
    { "JoyPort1Device", 0, RES_EVENT_NO, (resource_value_t)JOYPORT_ID_NONE,
      &joy_port[0], set_joyport1_device, NULL },
    { NULL }
};

static const resource_int_t resources_int_port2[] = {
    { "JoyPort2Device", 0, RES_EVENT_NO, (resource_value_t)JOYPORT_ID_NONE,
      &joy_port[1], set_joyport2_device, NULL },
    { NULL }
};

int joyport_resources_init(int pot_present, int ports)
{
    joy_pot_present = pot_present;
    max_ports = ports;

    if (!joyport_set_done) {
        joyport_set_done = 1;
        memset(joyport_device, 0, sizeof(joyport_device));
        joyport_device[0].name = "None";
        joyport_device[0].trans_name = IDGS_NONE;
        joyport_device[0].port_mask = JOYPORT_MASK_12;
    }

    if (ports == JOYPORT_PORTS_2) {
        if (resources_register_int(resources_int_port2) < 0) {
            return -1;
        }
    }

    return resources_register_int(resources_int_port1);
}

/* ------------------------------------------------------------------------- */

struct joyport_opt_s {
    const char *name;
    int id;
};

static struct joyport_opt_s id_match[] = {
    { "0",               JOYPORT_ID_NONE },
    { "none",            JOYPORT_ID_NONE },
    { "1",               JOYPORT_ID_JOY1 },
    { "joy1",            JOYPORT_ID_JOY1 },
    { "2",               JOYPORT_ID_JOY2 },
    { "joy2",            JOYPORT_ID_JOY2 },
    { "3",               JOYPORT_ID_JOY3 },
    { "joy3",            JOYPORT_ID_JOY3 },
    { "4",               JOYPORT_ID_JOY4 },
    { "joy4",            JOYPORT_ID_JOY4 },
    { "5",               JOYPORT_ID_PADDLES },
    { "paddles",         JOYPORT_ID_PADDLES },
    { "6",               JOYPORT_ID_MOUSE_1351 },
    { "1351",            JOYPORT_ID_MOUSE_1351 },
    { "1351mouse",       JOYPORT_ID_MOUSE_1351 },
    { "7",               JOYPORT_ID_MOUSE_NEOS },
    { "neos",            JOYPORT_ID_MOUSE_NEOS },
    { "neosmouse",       JOYPORT_ID_MOUSE_NEOS },
    { "8",               JOYPORT_ID_MOUSE_AMIGA },
    { "amiga",           JOYPORT_ID_MOUSE_AMIGA },
    { "amigamouse",      JOYPORT_ID_MOUSE_AMIGA },
    { "9",               JOYPORT_ID_MOUSE_CX22 },
    { "cx22",            JOYPORT_ID_MOUSE_CX22 },
    { "cx22mouse",       JOYPORT_ID_MOUSE_CX22 },
    { "10",              JOYPORT_ID_MOUSE_ST },
    { "st",              JOYPORT_ID_MOUSE_ST },
    { "atarist",         JOYPORT_ID_MOUSE_ST },
    { "stmouse",         JOYPORT_ID_MOUSE_ST },
    { "ataristmouse",    JOYPORT_ID_MOUSE_ST },
    { "11",              JOYPORT_ID_MOUSE_SMART },
    { "smart",           JOYPORT_ID_MOUSE_SMART },
    { "smartmouse",      JOYPORT_ID_MOUSE_SMART },
    { "12",              JOYPORT_ID_MOUSE_MICROMYS },
    { "micromys",        JOYPORT_ID_MOUSE_MICROMYS },
    { "micromysmouse",   JOYPORT_ID_MOUSE_MICROMYS },
    { "13",              JOYPORT_ID_KOALAPAD },
    { "koalapad",        JOYPORT_ID_KOALAPAD },
    { "14",              JOYPORT_ID_LIGHTPEN_U },
    { "lpup",            JOYPORT_ID_LIGHTPEN_U },
    { "lightpenup",      JOYPORT_ID_LIGHTPEN_U },
    { "15",              JOYPORT_ID_LIGHTPEN_L },
    { "lpleft",          JOYPORT_ID_LIGHTPEN_L },
    { "lightpenleft",    JOYPORT_ID_LIGHTPEN_L },
    { "16",              JOYPORT_ID_LIGHTPEN_DATEL },
    { "lpdatel",         JOYPORT_ID_LIGHTPEN_DATEL },
    { "lightpendatel",   JOYPORT_ID_LIGHTPEN_DATEL },
    { "datellightpen",   JOYPORT_ID_LIGHTPEN_DATEL },
    { "17",              JOYPORT_ID_LIGHTGUN_Y },
    { "magnum",          JOYPORT_ID_LIGHTGUN_Y },
    { "18",              JOYPORT_ID_LIGHTGUN_L },
    { "stack",           JOYPORT_ID_LIGHTGUN_L },
    { "slr",             JOYPORT_ID_LIGHTGUN_L },
    { "19",              JOYPORT_ID_LIGHTPEN_INKWELL },
    { "lpinkwell",       JOYPORT_ID_LIGHTPEN_INKWELL },
    { "lightpeninkwell", JOYPORT_ID_LIGHTPEN_INKWELL },
    { "inkwelllightpen", JOYPORT_ID_LIGHTPEN_INKWELL },
    { NULL, -1 }
};

static int set_joyport_cmdline_device(const char *param, void *extra_param)
{
    int temp = -1;
    int i = 0;
    int port = vice_ptr_to_int(extra_param);

    if (!param) {
        return -1;
    }

    do {
        if (strcmp(id_match[i].name, param) == 0) {
            temp = id_match[i].id;
        }
        i++;
    } while ((temp == -1) && (id_match[i].name != NULL));

    if (temp == -1) {
        return -1;
    }

    if (port == 1) {
        return set_joyport1_device(temp, NULL);
    }
    return set_joyport2_device(temp, NULL);
}

/* ------------------------------------------------------------------------- */

static char *build_joyport_string(int port)
{
    int i = 0;
    char *tmp1;
    char *tmp2;
    char number[4];

    if (port == JOYPORT_1) {
        tmp1 = lib_stralloc(translate_text(IDGS_SET_JOYPORT1_DEVICE));
    } else {
        tmp1 = lib_stralloc(translate_text(IDGS_SET_JOYPORT2_DEVICE));
    }

    for (i = 1; i < JOYPORT_MAX_DEVICES; ++i) {
        if (joyport_device[i].name) {
            if (joyport_device[i].port_mask & (1 << port)) {
                sprintf(number, "%d", i);
                tmp2 = util_concat(tmp1, ", ", number, ": ", translate_text(joyport_device[i].trans_name), NULL);
                lib_free(tmp1);
                tmp1 = tmp2;
            }
        }
    }
    tmp2 = util_concat(tmp1, ")", NULL);
    lib_free(tmp1);
    return tmp2;
}

static cmdline_option_t cmdline_options_port1[] =
{
    { "-joyport1device", CALL_FUNCTION, 1,
      set_joyport_cmdline_device, (void *)JOYPORT_1, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_DYN,
      IDGS_DEVICE, 1,
      NULL, NULL },
    { NULL }
};

static cmdline_option_t cmdline_options_port2[] =
{
    { "-joyport2device", CALL_FUNCTION, 1,
      set_joyport_cmdline_device, (void *)JOYPORT_2, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_DYN,
      IDGS_DEVICE, 2,
      NULL, NULL },
    { NULL }
};

int joyport_cmdline_options_init(void)
{
    union char_func cf;

    cf.f = build_joyport_string;
    cmdline_options_port1[0].description = cf.c;
    if (cmdline_register_options(cmdline_options_port1) < 0) {
        return -1;
    }

    if (max_ports == JOYPORT_PORTS_2) {
        cf.f = build_joyport_string;
        cmdline_options_port2[0].description = cf.c;
        if (cmdline_register_options(cmdline_options_port2) < 0) {
            return -1;
        }
    }
    return 0;
}
