/*
 * joy.c - Joystick support for Vice/2.
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

#define INCL_DOSERRORS      // ERROR_OPEN_FAILED
#define INCL_DOSPROFILE     // DosTmrQueryTime
#define INCL_DOSFILEMGR     // DosOpen
#define INCL_DOSDEVICES     // DosDevIOCtl
#define INCL_DOSDEVIOCTL    // DosDevIOCtl
#define INCL_DOSSEMAPHORES

#include <os2.h>

#include "cmdline.h"
#include "joy.h"
#include "joystick.h"
#include "keyboard.h"
#include "log.h"
#include "machine.h"
#include "resources.h"
#include "translate.h"

static log_t joylog = LOG_ERR;

/* ------------------------------------------------------------------------- */
extern void archdep_create_mutex_sem(HMTX *hmtx, const char *pszName, int fState);

typedef enum {
    KEYSET_N = 0,
    KEYSET_E = 1,
    KEYSET_S = 2,
    KEYSET_W = 3,
    KEYSET_NW = 4,
    KEYSET_NE = 5,
    KEYSET_SE = 6,
    KEYSET_SW = 7,
    KEYSET_FIRE = 8
} joystick_directions_t;

const int cbm_set[9] = {
    CBM_NORTH,
    CBM_EAST,
    CBM_SOUTH,
    CBM_WEST,
    CBM_NORTH | CBM_WEST,
    CBM_NORTH | CBM_EAST,
    CBM_SOUTH | CBM_EAST,
    CBM_SOUTH | CBM_WEST,
    CBM_FIRE
};

// Notice that this has to be `int' to make resources work.
static int keyset[2][9];

// This variables describes which PC devices are connected to the CBM Port?
static joystick_device_t cbm_joystick[4];

static int set_cbm_joystick(int val, void *param)
{
    int nr = vice_ptr_to_int(param);

    switch (val) {
        case JOYDEV_NONE:
        case JOYDEV_HW1:
        case JOYDEV_HW2:
        case JOYDEV_NUMPAD:
        case JOYDEV_KEYSET1:
        case JOYDEV_KEYSET2:
            break;
        default:
            return -1;
    }

    cbm_joystick[nr] = (joystick_device_t)val;

    joystick_clear(nr + 1);

    return 0;
}

struct joycal
{
    int direction[4];
    int xmin, xmax;
    int ymin, ymax;
    int autocal;
    int start;
};

typedef struct joycal joycal_t;

static joycal_t joy[2];

int set_joyA_autoCal(const char *value, void *extra_param)
{
    joy[0].autocal = vice_ptr_to_int(extra_param);

    if (joy[0].autocal) {
        joy[0].start = TRUE;
    }

    return FALSE;
}

int set_joyB_autoCal(const char *value, void *extra_param)
{
    joy[1].autocal = vice_ptr_to_int(extra_param);

    if (joy[1].autocal) {
        joy[1].start = TRUE;
    }

    return FALSE;
}

int get_joy_autoCal(const int nr)
{
    return joy[nr].autocal;
}

static int set_joy(int v, void *param)
{
    const int i = vice_ptr_to_int(param);

    joy[i >> 5].direction[i & 0xf] = (int)v;
    return 0;
}

#define DEFINE_RES_SET_CALDATA(txt, num, dir, def) \
    { txt, (int) def,                              \
      RES_EVENT_NO, NULL,                          \
      &(joy[num].direction[dir]), set_joy,         \
      (void*)((num << 5) | dir) }

static int set_keyset(int v, void *param)
{
    const int i = vice_ptr_to_int(param);

    keyset[i >> 5][i & 0xf] = (int)v;
    return 0;
}

#define DEFINE_RES_SET_KEYDATA(txt, num, dir)  \
    { txt, K_NONE, RES_EVENT_NO, NULL,         \
      &(keyset[num][dir]), set_keyset,         \
      (void*)((num << 5) | dir) }

static const resource_int_t joy1_resources_int[] = {
    { "JoyDevice1", JOYDEV_NONE, RES_EVENT_NO, NULL,
      &cbm_joystick[0], set_cbm_joystick, (void *)0 },
    NULL
};

static const resource_int_t joy2_resources_int[] = {
    { "JoyDevice2", JOYDEV_NONE, RES_EVENT_NO, NULL,
      &cbm_joystick[1], set_cbm_joystick, (void *)1 },
    NULL
};

static const resource_int_t joy3_resources_int[] = {
    { "JoyDevice3", JOYDEV_NONE, RES_EVENT_NO, NULL,
      &cbm_joystick[2], set_cbm_joystick, (void *)2 },
    NULL
};

static const resource_int_t joy4_resources_int[] = {
    { "JoyDevice4", JOYDEV_NONE, RES_EVENT_NO, NULL,
      &cbm_joystick[3], set_cbm_joystick, (void *)3 },
    NULL
};

static const resource_int_t resources_int[] = {
    DEFINE_RES_SET_CALDATA("JoyAup", 0, KEYSET_N, 200),
    DEFINE_RES_SET_CALDATA("JoyAdown", 0, KEYSET_S, 600),
    DEFINE_RES_SET_CALDATA("JoyAleft", 0, KEYSET_W, 200),
    DEFINE_RES_SET_CALDATA("JoyAright", 0, KEYSET_E, 600),
    DEFINE_RES_SET_CALDATA("JoyBup", 1, KEYSET_N, 200),
    DEFINE_RES_SET_CALDATA("JoyBdown", 1, KEYSET_S, 600),
    DEFINE_RES_SET_CALDATA("JoyBleft", 1, KEYSET_W, 200),
    DEFINE_RES_SET_CALDATA("JoyBright", 1, KEYSET_E, 600),

    DEFINE_RES_SET_KEYDATA("KeySet1NorthWest", 0, KEYSET_NW),
    DEFINE_RES_SET_KEYDATA("KeySet1North", 0, KEYSET_N),
    DEFINE_RES_SET_KEYDATA("KeySet1NorthEast", 0, KEYSET_NE),
    DEFINE_RES_SET_KEYDATA("KeySet1East", 0, KEYSET_E),
    DEFINE_RES_SET_KEYDATA("KeySet1SouthEast", 0, KEYSET_SE),
    DEFINE_RES_SET_KEYDATA("KeySet1South", 0, KEYSET_S),
    DEFINE_RES_SET_KEYDATA("KeySet1SouthWest", 0, KEYSET_SW),
    DEFINE_RES_SET_KEYDATA("KeySet1West", 0, KEYSET_W),
    DEFINE_RES_SET_KEYDATA("KeySet1Fire", 0, KEYSET_FIRE),

    DEFINE_RES_SET_KEYDATA("KeySet2NorthWest", 1, KEYSET_NW),
    DEFINE_RES_SET_KEYDATA("KeySet2North", 1, KEYSET_N),
    DEFINE_RES_SET_KEYDATA("KeySet2NorthEast", 1, KEYSET_NE),
    DEFINE_RES_SET_KEYDATA("KeySet2East", 1, KEYSET_E),
    DEFINE_RES_SET_KEYDATA("KeySet2SouthEast", 1, KEYSET_SE),
    DEFINE_RES_SET_KEYDATA("KeySet2South", 1, KEYSET_S),
    DEFINE_RES_SET_KEYDATA("KeySet2SouthWest", 1, KEYSET_SW),
    DEFINE_RES_SET_KEYDATA("KeySet2West", 1, KEYSET_W),
    DEFINE_RES_SET_KEYDATA("KeySet2Fire", 1, KEYSET_FIRE),

    NULL
};

int joystick_init_resources(void)
{
    switch (machine_class) {
        case VICE_MACHINE_C64:
        case VICE_MACHINE_C64SC:
        case VICE_MACHINE_C128:
        case VICE_MACHINE_C64DTV:
        case VICE_MACHINE_SCPU64:
            if (resources_register_int(joy1_resources_int) < 0) {
                return -1;
            }
            if (resources_register_int(joy2_resources_int) < 0) {
                return -1;
            }
            if (resources_register_int(joy3_resources_int) < 0) {
                return -1;
            }
            if (resources_register_int(joy4_resources_int) < 0) {
                return -1;
            }
            break;
        case VICE_MACHINE_PET:
        case VICE_MACHINE_CBM6x0:
            if (resources_register_int(joy3_resources_int) < 0) {
                return -1;
            }
            if (resources_register_int(joy4_resources_int) < 0) {
                return -1;
            }
            break;
        case VICE_MACHINE_CBM5x0:
            if (resources_register_int(joy1_resources_int) < 0) {
                return -1;
            }
            if (resources_register_int(joy2_resources_int) < 0) {
                return -1;
            }
            break;
        case VICE_MACHINE_PLUS4:
            if (resources_register_int(joy1_resources_int) < 0) {
                return -1;
            }
            if (resources_register_int(joy2_resources_int) < 0) {
                return -1;
            }
            if (resources_register_int(joy3_resources_int) < 0) {
                return -1;
            }
            break;
        case VICE_MACHINE_VIC20:
            if (resources_register_int(joy1_resources_int) < 0) {
                return -1;
            }
            if (resources_register_int(joy3_resources_int) < 0) {
                return -1;
            }
            if (resources_register_int(joy4_resources_int) < 0) {
                return -1;
            }
            break;
    }
    joystick_extra_init_resources();
    return resources_register_int(resources_int);
}

/* ------------------------------------------------------------------------- */

static const cmdline_option_t cmdline_options[] = {
    { "-joy1cal", CALL_FUNCTION, 0,
      &set_joyA_autoCal, (void *) TRUE, NULL, 0,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      NULL, "Start auto calibration for PC joystick #1" },
    { "-joy2cal", CALL_FUNCTION, 0,
      &set_joyB_autoCal, (void *) TRUE, NULL, 0,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      NULL, "Start auto calibration for PC joystick #2" },
    { NULL }
};

static const cmdline_option_t joydev1cmdline_options[] = {
    { "-joydev1", SET_RESOURCE, 1,
      NULL, NULL, "JoyDevice1", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      "<number>", "Set input device for CBM joystick port #1 (0: None, 1: Joystick 1, 2: Joystick 2, 4: Numpad, 8: Keyset 1, 16: Keyset 2)" },
    { NULL }
};

static const cmdline_option_t joydev2cmdline_options[] = {
    { "-joydev2", SET_RESOURCE, 1,
      NULL, NULL, "JoyDevice2", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      "<number>", "Set input device for CBM joystick port #2 (0: None, 1: Joystick 1, 2: Joystick 2, 4: Numpad, 8: Keyset 1, 16: Keyset 2)" },
    { NULL }
};

static const cmdline_option_t joydev3cmdline_options[] = {
    { "-extrajoydev1", SET_RESOURCE, 1,
      NULL, NULL, "JoyDevice3", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      "<number>", "Set input device for extra CBM joystick port #1 (0: None, 1: Joystick 1, 2: Joystick 2, 4: Numpad, 8: Keyset 1, 16: Keyset 2)" },
    { NULL }
};

static const cmdline_option_t joydev4cmdline_options[] = {
    { "-extrajoydev2", SET_RESOURCE, 1,
      NULL, NULL, "JoyDevice4", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      "<number>", "Set input device for extra CBM joystick port #2 (0: None, 1: Joystick 1, 2: Joystick 2, 4: Numpad, 8: Keyset 1, 16: Keyset 2)" },
    { NULL }
};

int joystick_arch_cmdline_options_init(void)
{
    switch (machine_class) {
        case VICE_MACHINE_C64:
        case VICE_MACHINE_C64SC:
        case VICE_MACHINE_C128:
        case VICE_MACHINE_C64DTV:
        case VICE_MACHINE_SCPU64:
            if (cmdline_register_options(joydev1cmdline_options) < 0) {
                return -1;
            }
            if (cmdline_register_options(joydev2cmdline_options) < 0) {
                return -1;
            }
            if (cmdline_register_options(joydev3cmdline_options) < 0) {
                return -1;
            }
            if (cmdline_register_options(joydev4cmdline_options) < 0) {
                return -1;
            }
            break;
        case VICE_MACHINE_PET:
        case VICE_MACHINE_CBM6x0:
            if (cmdline_register_options(joydev3cmdline_options) < 0) {
                return -1;
            }
            if (cmdline_register_options(joydev4cmdline_options) < 0) {
                return -1;
            }
            break;
        case VICE_MACHINE_CBM5x0:
            if (cmdline_register_options(joydev1cmdline_options) < 0) {
                return -1;
            }
            if (cmdline_register_options(joydev2cmdline_options) < 0) {
                return -1;
            }
            break;
        case VICE_MACHINE_PLUS4:
            if (cmdline_register_options(joydev1cmdline_options) < 0) {
                return -1;
            }
            if (cmdline_register_options(joydev2cmdline_options) < 0) {
                return -1;
            }
            if (cmdline_register_options(joydev3cmdline_options) < 0) {
                return -1;
            }
            break;
        case VICE_MACHINE_VIC20:
            if (cmdline_register_options(joydev1cmdline_options) < 0) {
                return -1;
            }
            if (cmdline_register_options(joydev3cmdline_options) < 0) {
                return -1;
            }
            if (cmdline_register_options(joydev4cmdline_options) < 0) {
                return -1;
            }
            break;
    }
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

/* Flag: is joystick present?
 FIXME: Use this flag to disable the dialog
 */
int number_joysticks = 0;

/* ------------------------------------------------------------------------- */

static HFILE SWhGame = 0;
static HMTX  hmtxJoystick;

/* Initialize joystick support.  */
int joy_arch_init(void)
{
    ULONG action;    // return value from DosOpen
    APIRET rc;

    if (SWhGame) {
        return 0;
    }

    joylog = log_open("Joystick");

    archdep_create_mutex_sem(&hmtxJoystick, "Joystick", TRUE);

    number_joysticks = JOYDEV_NONE;
    if (rc = DosOpen("GAME$", &SWhGame, &action, 0, FILE_READONLY, OPEN_ACTION_OPEN_IF_EXISTS, OPEN_ACCESS_READONLY | OPEN_SHARE_DENYNONE, NULL)) {
        if (rc == ERROR_OPEN_FAILED) {
            log_warning(joylog, "Cannot open device 'GAME$'.");
            log_warning(joylog, "Have you installed a MPPM/2 joystick device driver?\n");
        } else {
            log_error(joylog, "DosOpen 'GAME$' (rc=%i)", rc);
        }
    } else {
        GAME_PARM_STRUCT parms;
        ULONG dataLen = sizeof(parms); // length of gameStatus

        if (rc = DosDevIOCtl(SWhGame, IOCTL_CAT_USER, GAME_GET_PARMS, NULL, 0, NULL, &parms, dataLen, &dataLen)) {
            log_error(joylog, "DosDevIOCtl (rc=%i)", rc);
        } else {
            if (parms.useA) {
                log_message(joylog, "Joystick A found.");
                number_joysticks |= JOYDEV_HW1;
            }
            if (parms.useB) {
                log_message(joylog, "Joystick B found.");
                number_joysticks |= JOYDEV_HW2;
            }
            if (number_joysticks == JOYDEV_NONE) {
                log_message(joylog, "Sorry, no joystick found!");
            }
        }
    }

    DosReleaseMutexSem(hmtxJoystick);
    return 0;
}

/* Strange! If video_init fails this is called without calling DosOpen before! */
void joystick_close(void)
{
    APIRET rc;

    DosRequestMutexSem(hmtxJoystick, SEM_INDEFINITE_WAIT);
    rc = DosClose(SWhGame);
    DosCloseMutexSem(hmtxJoystick);

    if (!rc) {
        return;
    }

    log_error(joylog, "DosClose 'GAME$' (rc=%i)", rc);
}

static void handle_joystick_movement(const GAME_2DPOS_STRUCT *joy, joycal_t *cal, const int pc_device, const int buttons)
{
    int value = buttons ? CBM_FIRE : 0;

    if (!(number_joysticks & pc_device & (cbm_joystick[0] | cbm_joystick[1] | cbm_joystick[2] | cbm_joystick[3]))) {
        return;
    }

    if (cal->start) {
        cal->xmin = cal->xmax = joy->x;
        cal->ymin = cal->ymax = joy->y;
        cal->direction[KEYSET_N] = 9 * joy->y / 10;
        cal->direction[KEYSET_S] = 11 * joy->y / 10;
        cal->direction[KEYSET_W] = 9 * joy->x / 10;
        cal->direction[KEYSET_E] = 11 * joy->x / 10;
        cal->start = FALSE;
    }

    if (joy->y < cal->direction[KEYSET_N]) {
        value |= CBM_NORTH;
        if (cal->autocal && joy->y < cal->ymin) {
            cal->ymin = joy->y;
            cal->direction[KEYSET_N] = (3 * cal->ymin + cal->ymax) / 4;
            cal->direction[KEYSET_S] = 3 * (cal->ymin + cal->ymax) / 4;
        }
    }

    if (joy->y > cal->direction[KEYSET_S]) {
        value |= CBM_SOUTH;
        if (cal->autocal && joy->y > cal->ymax) {
            cal->ymax = joy->y;
            cal->direction[KEYSET_N] = (3 * cal->ymin + cal->ymax) / 4;
            cal->direction[KEYSET_S] = 3 * (cal->ymin + cal->ymax) / 4;
        }
    }

    if (joy->x < cal->direction[KEYSET_W]) {
        value |= CBM_WEST;
        if (cal->autocal && joy->x < cal->xmin) {
            cal->xmin = joy->x;
            cal->direction[KEYSET_W] = (3 * cal->xmin + cal->xmax) / 4;
            cal->direction[KEYSET_E] = 3 * (cal->xmin + cal->xmax) / 4;
        }
    }

    if (joy->x > cal->direction[KEYSET_E]) {
        value |= CBM_EAST;
        if (cal->autocal && joy->x > cal->xmax) {
            cal->xmax = joy->x;
            cal->direction[KEYSET_W] = (3 * cal->xmin + cal->xmax) / 4;
            cal->direction[KEYSET_E] = 3 * (cal->xmin + cal->xmax) / 4;
        }
    }

    if (cbm_joystick[0] & pc_device) {
        joystick_set_value_absolute(1, value);
    }

    if (cbm_joystick[1] & pc_device) {
        joystick_set_value_absolute(2, value);
    }

    if (cbm_joystick[2] & pc_device) {
        joystick_set_value_absolute(3, value);
    }

    if (cbm_joystick[3] & pc_device) {
        joystick_set_value_absolute(4, value);
    }
}


/* Update the `joystick_value' variables according to the joystick status.  */
void joystick_update(void)
{
    APIRET rc;
    static GAME_STATUS_STRUCT gameStatus;      // joystick readings
    static ULONG dataLen = sizeof(gameStatus); // length of gameStatus

    if (!number_joysticks || !SWhGame) {
        return;
    }

    DosRequestMutexSem(hmtxJoystick, SEM_INDEFINITE_WAIT);
    rc = DosDevIOCtl(SWhGame, IOCTL_CAT_USER, GAME_GET_STATUS, NULL, 0, NULL, &gameStatus, dataLen, &dataLen);
    DosReleaseMutexSem(hmtxJoystick);

    if (rc) {
        return;
    }

    handle_joystick_movement(&(gameStatus.curdata.A), &joy[0], JOYDEV_HW1, ~gameStatus.curdata.butMask & JOYA_BUTTONS);
    handle_joystick_movement(&(gameStatus.curdata.B), &joy[1], JOYDEV_HW2, ~gameStatus.curdata.butMask & JOYB_BUTTONS);
}

/* ------------------------------------------------------------------------- */

static int handle_keyset_mapping(joystick_device_t pc_device, const int *set, kbd_code_t kcode, int pressed)
{
    int i;
    int nr;

    if (cbm_joystick[0] & pc_device) {
        nr = 1;
    } else {
        if (cbm_joystick[1] & pc_device) {
            nr = 2;
        } else {
            if (cbm_joystick[2] & pc_device) {
                nr = 3;
            } else {
                if (cbm_joystick[3] & pc_device) {
                    nr = 4;
                } else {
                    return 0;
                }
            }
        }
    }

    for (i = 0; i < 9; i++) {
        if (kcode == set[i]) {
            if (pressed) {
                joystick_set_value_or(nr, cbm_set[i]);
            } else {
                joystick_set_value_and(nr, ~cbm_set[i]);
            }

            return 1;
        }
    }

    return 0;
}

/* Handle keys to emulate the joystick.  Warning: this is called within the
   keyboard interrupt, so take care when modifying this code!  */
int joystick_handle_key(kbd_code_t kcode, int pressed)
{
    const int numpad[] = { K_KP8, K_KP6, K_KP2, K_KP4, K_KP7, K_KP9, K_KP3, K_KP1, K_KP0 };

    /* (Notice we have to handle all the keysets even when one key is used
       more than once (the most intuitive behavior), so we use `|' instead of
       `||'.)  */
    return (handle_keyset_mapping(JOYDEV_NUMPAD, numpad, kcode, pressed) |
            handle_keyset_mapping(JOYDEV_KEYSET1, keyset[0], kcode, pressed) |
            handle_keyset_mapping(JOYDEV_KEYSET2, keyset[1], kcode, pressed));
}

/* ------------------------------------------------------------------------- */
