/*
 * joystick.c - Joystick support for Vice/2.
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

#define INCL_DOSPROFILE     // DosTmrQueryTime
#define INCL_DOSFILEMGR     // DosOpen
#define INCL_DOSDEVICES     // DosDevIOCtl
#define INCL_DOSDEVIOCTL    // DosDevIOCtl
#define INCL_DOSSEMAPHORES
#include <os2.h>

#include "cmdline.h"
#include "joystick.h"
#include "keyboard.h"
#include "log.h"
#include "resources.h"

/* ------------------------------------------------------------------------- */
extern void archdep_create_mutex_sem(HMTX *hmtx, const char *pszName, int fState);

typedef enum {
    KEYSET_NW   = 0x0,
    KEYSET_N    = 0x1,
    KEYSET_NE   = 0x2,
    KEYSET_E    = 0x3,
    KEYSET_SE   = 0x4,
    KEYSET_S    = 0x5,
    KEYSET_SW   = 0x6,
    KEYSET_W    = 0x7,
    KEYSET_FIRE = 0x8
} joystick_directions_t;

const int cbm_set[9] =
{
    CBM_NORTH|CBM_WEST, CBM_NORTH, CBM_NORTH|CBM_EAST, CBM_EAST,
    CBM_SOUTH|CBM_EAST, CBM_SOUTH, CBM_SOUTH|CBM_WEST, CBM_WEST,
    CBM_FIRE
};

// Notice that this has to be `int' to make resources work.
static int keyset1[9], keyset2[9];

// This variables describes which PC devices are conneted to the CBM Port?
static joystick_device_t cbm_joystick_1, cbm_joystick_2;

static int set_cbm_joystick_1(resource_value_t v, void *param)
{
    cbm_joystick_1 = (joystick_device_t)(int) v;

    joystick_clear(1);
    // joystick_value[1] = 0;

    return 0;
}

static int set_cbm_joystick_2(resource_value_t v, void *param)
{
    cbm_joystick_2 = (joystick_device_t)(int) v;

    joystick_clear(2);
    // joystick_value[2] = 0;

    return 0;
}

struct joycal
{
    int up, dn;
    int lt, rt;

    int xmin, xmax;
    int ymin, ymax;

    int autocal;
    int start;
};

typedef struct joycal joycal_t;

static joycal_t joyA;
static joycal_t joyB;

int set_joyA_autoCal(const char *value, void *extra_param)
{
    joyA.autocal = (int)extra_param;

    if (joyA.autocal)
        joyA.start = TRUE;

    return FALSE;
}

int set_joyB_autoCal(const char *value, void *extra_param)
{
    joyB.autocal = (int)extra_param;

    if (joyB.autocal)
        joyB.start = TRUE;

    return FALSE;
}

int get_joyA_autoCal()
{
    return joyA.autocal;
}

int get_joyB_autoCal()
{
    return joyB.autocal;
}

#define DEFINE_SET_CALDATA(num, dir)                                   \
    static int set_joy##num##_##dir##(resource_value_t v, void *param) \
    {                                                                  \
        joy##num##.##dir## = (int) v;                                  \
        return 0;                                                      \
    }

DEFINE_SET_CALDATA(A, up)
DEFINE_SET_CALDATA(A, dn)
DEFINE_SET_CALDATA(A, lt)
DEFINE_SET_CALDATA(A, rt)
DEFINE_SET_CALDATA(B, up)
DEFINE_SET_CALDATA(B, dn)
DEFINE_SET_CALDATA(B, lt)
DEFINE_SET_CALDATA(B, rt)

#define DEFINE_SET_KEYSET(num, dir)                                       \
    static int set_keyset##num##_##dir##(resource_value_t v, void *param) \
    {                                                                     \
        keyset##num##[KEYSET_##dir##] = (int) v;                          \
        return 0;                                                         \
    }

DEFINE_SET_KEYSET(1, NW)
DEFINE_SET_KEYSET(1, N)
DEFINE_SET_KEYSET(1, NE)
DEFINE_SET_KEYSET(1, E)
DEFINE_SET_KEYSET(1, SE)
DEFINE_SET_KEYSET(1, S)
DEFINE_SET_KEYSET(1, SW)
DEFINE_SET_KEYSET(1, W)
DEFINE_SET_KEYSET(1, FIRE)

DEFINE_SET_KEYSET(2, NW)
DEFINE_SET_KEYSET(2, N)
DEFINE_SET_KEYSET(2, NE)
DEFINE_SET_KEYSET(2, E)
DEFINE_SET_KEYSET(2, SE)
DEFINE_SET_KEYSET(2, S)
DEFINE_SET_KEYSET(2, SW)
DEFINE_SET_KEYSET(2, W)
DEFINE_SET_KEYSET(2, FIRE)

#define DEFINE_RES_SET_CALDATA(txt, num, dir, def)                    \
    { txt, RES_INTEGER, (resource_value_t) def,                       \
    (resource_value_t *) &joy##num##.##dir##, set_joy##num##_##dir##, \
    NULL }

#define DEFINE_RES_SET_KEYDATA(txt, num, dir)                                       \
    { txt, RES_INTEGER, (resource_value_t) K_NONE,                                  \
    (resource_value_t *) &keyset##num##[KEYSET_##dir##], set_keyset##num##_##dir##, \
    NULL }

static resource_t resources[] = {
    { "JoyDevice1", RES_INTEGER, (resource_value_t) JOYDEV_NONE,
      (resource_value_t *) &cbm_joystick_1, set_cbm_joystick_1, NULL },
    { "JoyDevice2", RES_INTEGER, (resource_value_t) JOYDEV_NONE,
      (resource_value_t *) &cbm_joystick_2, set_cbm_joystick_2, NULL },

    DEFINE_RES_SET_CALDATA("JoyAup",    A, up, 200),
    DEFINE_RES_SET_CALDATA("JoyAdown",  A, dn, 600),
    DEFINE_RES_SET_CALDATA("JoyAleft",  A, lt, 200),
    DEFINE_RES_SET_CALDATA("JoyAright", A, rt, 600),
    DEFINE_RES_SET_CALDATA("JoyBup",    B, up, 200),
    DEFINE_RES_SET_CALDATA("JoyBdown",  B, dn, 600),
    DEFINE_RES_SET_CALDATA("JoyBleft",  B, lt, 200),
    DEFINE_RES_SET_CALDATA("JoyBright", B, rt, 600),

    DEFINE_RES_SET_KEYDATA("KeySet1NorthWest", 1, NW),
    DEFINE_RES_SET_KEYDATA("KeySet1North",     1, N),
    DEFINE_RES_SET_KEYDATA("KeySet1NorthEast", 1, NE),
    DEFINE_RES_SET_KEYDATA("KeySet1East",      1, E),
    DEFINE_RES_SET_KEYDATA("KeySet1SouthEast", 1, SE),
    DEFINE_RES_SET_KEYDATA("KeySet1South",     1, S),
    DEFINE_RES_SET_KEYDATA("KeySet1SouthWest", 1, SW),
    DEFINE_RES_SET_KEYDATA("KeySet1West",      1, W),
    DEFINE_RES_SET_KEYDATA("KeySet1Fire",      1, FIRE),

    DEFINE_RES_SET_KEYDATA("KeySet2NorthWest", 2, NW),
    DEFINE_RES_SET_KEYDATA("KeySet2North",     2, N),
    DEFINE_RES_SET_KEYDATA("KeySet2NorthEast", 2, NE),
    DEFINE_RES_SET_KEYDATA("KeySet2East",      2, E),
    DEFINE_RES_SET_KEYDATA("KeySet2SouthEast", 2, SE),
    DEFINE_RES_SET_KEYDATA("KeySet2South",     2, S),
    DEFINE_RES_SET_KEYDATA("KeySet2SouthWest", 2, SW),
    DEFINE_RES_SET_KEYDATA("KeySet2West",      2, W),
    DEFINE_RES_SET_KEYDATA("KeySet2Fire",      2, FIRE),

    { NULL }
};

int joystick_init_resources(void)
{
    return resources_register(resources);
}

/* ------------------------------------------------------------------------- */

static cmdline_option_t cmdline_options[] = {
    { "-joydev1", SET_RESOURCE, 1, NULL, NULL,
      "JoyDevice1", NULL,
      "<number>", "Set input device for CBM joystick port #1" },
    { "-joydev2", SET_RESOURCE, 1, NULL, NULL,
      "JoyDevice2", NULL,
      "<number>", "Set input device for CBM joystick port #2" },
    { "-joy1cal", CALL_FUNCTION, 0, &set_joyA_autoCal,
      (void *) TRUE, NULL, 0, NULL, "Start auto calibration for PC joystick #1" },
    { "-joy2cal", CALL_FUNCTION, 0, &set_joyB_autoCal,
      (void *) TRUE, NULL, 0, NULL, "Start auto calibration for PC joystick #2" },
    { NULL }
};

int joystick_init_cmdline_options(void)
{
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
void joystick_init(void)
{
    ULONG action;    // return value from DosOpen
    APIRET rc;

    if (SWhGame)
        return;

    archdep_create_mutex_sem(&hmtxJoystick, "Joystick", TRUE);

    number_joysticks = JOYDEV_NONE;
    if (rc=DosOpen("GAME$", &SWhGame, &action, 0,
                   FILE_READONLY, OPEN_ACTION_OPEN_IF_EXISTS/*FILE_OPEN*/,
                   OPEN_ACCESS_READONLY | OPEN_SHARE_DENYNONE, NULL))
        log_message(LOG_DEFAULT, "joystick.c: DosOpen (rc=%i)", rc);
    else
    {
    	GAME_PARM_STRUCT parms;
        ULONG dataLen = sizeof(parms); // length of gameStatus
	if (rc=DosDevIOCtl(SWhGame, IOCTL_CAT_USER, GAME_GET_PARMS, NULL, 0, NULL,
                           &parms, dataLen, &dataLen))
        {
            number_joysticks = JOYDEV_HW1 & JOYDEV_HW2;
            log_message(LOG_DEFAULT, "joystick.c: DosDevIOCtl (rc=%i)", rc);
        }
        else
        {
            if (parms.useA) number_joysticks |= JOYDEV_HW1;
            if (parms.useB) number_joysticks |= JOYDEV_HW2;
        }
    }
    DosReleaseMutexSem(hmtxJoystick);
}

/* Strange! If video_init fails this is called without calling DosOpen before! */
void joystick_close(void)
{
    APIRET rc ;
    DosRequestMutexSem(hmtxJoystick, SEM_INDEFINITE_WAIT);
    rc=DosClose(SWhGame);
    DosCloseMutexSem(hmtxJoystick);

    if (rc)
        log_message(LOG_DEFAULT, "joystick.c: DosClose (rc=%i)", rc);
}

static void handle_joystick_movement(const GAME_2DPOS_STRUCT *joy,
                                     joycal_t *cal,
                                     const int pc_device, const int buttons)
{
    int value = buttons ? CBM_FIRE : 0;

    if (!(number_joysticks & pc_device & (cbm_joystick_1 | cbm_joystick_2)))
        return;

    if (cal->start)
    {
        cal->xmin  = cal->xmax = joy->x;
        cal->ymin  = cal->ymax = joy->y;
        cal->up    =  9*joy->y/10;
        cal->dn    = 11*joy->y/10;
        cal->lt    =  9*joy->x/10;
        cal->rt    = 11*joy->x/10;
        cal->start = FALSE;
    }

    if (joy->y < cal->up)
    {
        value |= CBM_NORTH;
        if (cal->autocal && joy->y<cal->ymin)
        {
            cal->ymin = joy->y;
            cal->up   = (3*cal->ymin+cal->ymax)/4;
            cal->dn   = 3*(cal->ymin+cal->ymax)/4;
        }
    }

    if (joy->y > cal->dn)
    {
        value |= CBM_SOUTH;
        if (cal->autocal && joy->y > cal->ymax)
        {
            cal->ymax = joy->y;
            cal->up   = (3*cal->ymin+cal->ymax)/4;
            cal->dn   = 3*(cal->ymin+cal->ymax)/4;
        }
    }

    if (joy->x < cal->lt)
    {
        value |= CBM_WEST;
        if (cal->autocal && joy->x<cal->xmin)
        {
            cal->xmin = joy->x;
            cal->lt   = (3*cal->xmin+cal->xmax)/4;
                cal->rt   = 3*(cal->xmin+cal->xmax)/4;
        }
    }

    if (joy->x > cal->rt)
    {
        value |= CBM_EAST;
        if (cal->autocal && joy->x > cal->xmax)
        {
            cal->xmax = joy->x;
            cal->lt   = (3*cal->xmin+cal->xmax)/4;
            cal->rt   = 3*(cal->xmin+cal->xmax)/4;
        }
    }

    if (cbm_joystick_1 & pc_device) joystick_set_value_absolute(1, value); //joystick_value[1] = value;
    if (cbm_joystick_2 & pc_device) joystick_set_value_absolute(1, value);
}


/* Update the `joystick_value' variables according to the joystick status.  */
void joystick_update(void)
{
    APIRET rc;
    static GAME_STATUS_STRUCT gameStatus;      // joystick readings
    static ULONG dataLen = sizeof(gameStatus); // length of gameStatus

    if (!number_joysticks || !SWhGame)
        return;

    DosRequestMutexSem(hmtxJoystick, SEM_INDEFINITE_WAIT);
    rc=DosDevIOCtl(SWhGame, IOCTL_CAT_USER, GAME_GET_STATUS, NULL, 0, NULL,
                   &gameStatus, dataLen, &dataLen);
    DosReleaseMutexSem(hmtxJoystick);

    if (rc)
        return;

    handle_joystick_movement(&(gameStatus.curdata.A), &joyA, JOYDEV_HW1,
                             ~gameStatus.curdata.butMask & JOYA_BUTTONS);
    handle_joystick_movement(&(gameStatus.curdata.B), &joyB, JOYDEV_HW2,
                             ~gameStatus.curdata.butMask & JOYB_BUTTONS);
}

/* ------------------------------------------------------------------------- */

static int handle_keyset_mapping(joystick_device_t pc_device, const int *set,
                                 kbd_code_t kcode, int pressed)
{
    int i;

    if (!(cbm_joystick_1 & pc_device) && !(cbm_joystick_2 & pc_device))
        return 0;

    for (i=0; i<9; i++)
    {
        if (kcode == set[i])
        {
            if (pressed)
            {
                if (cbm_joystick_1&pc_device)
                    joystick_set_value_or(1, cbm_set[i]); //joystick_value[1] |=  cbm_set[i];
                if (cbm_joystick_2&pc_device)
                    joystick_set_value_or(2, cbm_set[i]);
            }
            else
            {
                if (cbm_joystick_1&pc_device)
                    joystick_set_value_and(1, ~cbm_set[i]);
                if (cbm_joystick_2&pc_device)
                    joystick_set_value_and(2, ~cbm_set[i]);
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
    const int numpad[] =
    {
        K_KP7, K_KP8, K_KP9, K_KP6, K_KP3, K_KP2, K_KP1, K_KP4, K_KP0
    };

    /* (Notice we have to handle all the keysets even when one key is used
       more than once (the most intuitive behavior), so we use `|' instead of
       `||'.)  */
    return (
            handle_keyset_mapping(JOYDEV_NUMPAD,  numpad,  kcode, pressed) |
            handle_keyset_mapping(JOYDEV_KEYSET1, keyset1, kcode, pressed) |
            handle_keyset_mapping(JOYDEV_KEYSET2, keyset2, kcode, pressed)
           );
}

/* ------------------------------------------------------------------------- */
