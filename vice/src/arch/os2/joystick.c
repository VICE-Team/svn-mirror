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

#define INCL_DOSFILEMGR         // include needed for DosOpen call
#define INCL_DOSDEVICES         // include needed for DosDevIOCtl call
#define INCL_DOSDEVIOCTL        // include needed for DosDevIOCtl call
#define INCL_DOSSEMAPHORES
#include "vice.h"

#include "cmdline.h"
#include "joystick.h"
#include "keyboard.h"
#include "log.h"
#include "resources.h"

/* Notice that this has to be `int' to make resources work.  */
static int keyset1[9], keyset2[9];

/* ------------------------------------------------------------------------- */
/* Joystick devices.  */
static joystick_device_t joystick_device_1, joystick_device_2;

// maybe this should be made really thread safe??
static int set_joystick_device_1(resource_value_t v)
{
    joystick_device_1 = (joystick_device_t)(int) v;
    joystick_value[1] = 0;
    return 0;
}

static int set_joystick_device_2(resource_value_t v)
{
    joystick_device_2 = (joystick_device_t)(int) v;
    joystick_value[2] = 0;
    return 0;
}

static int joyA_autoCal=FALSE;
static int joyB_autoCal=FALSE;

static int joyA_start=FALSE;
static int joyB_start=FALSE;

int set_joyA_autoCal(const char *value, void *extra_param)
{
    joyA_autoCal = (int)extra_param;
    if (joyA_autoCal) joyA_start = TRUE;
    log_message(LOG_DEFAULT, "Auto A: %i", joyA_autoCal);
    return FALSE;
}

int set_joyB_autoCal(const char *value, void *extra_param)
{
    joyB_autoCal = (int)extra_param;
    if (joyA_autoCal) joyA_start = TRUE;
    log_message(LOG_DEFAULT, "Auto B: %i", joyB_autoCal);
    return FALSE;
}

int get_joyA_autoCal() {return joyA_autoCal;}
int get_joyB_autoCal() {return joyB_autoCal;}

static int joyA_up,    joyA_down;
static int joyA_left,  joyA_right;
static int joyB_up,    joyB_down;
static int joyB_left,  joyB_right;

#define DEFINE_SET_CALDATA(num, dir)                      \
    static int set_joy##num##_##dir##(resource_value_t v) \
    {                                                     \
        joy##num##_##dir## = (int) v;                     \
        return 0;                                         \
    }

DEFINE_SET_CALDATA(A, up)
DEFINE_SET_CALDATA(A, down)
DEFINE_SET_CALDATA(A, left)
DEFINE_SET_CALDATA(A, right)
DEFINE_SET_CALDATA(B, up)
DEFINE_SET_CALDATA(B, down)
DEFINE_SET_CALDATA(B, left)
DEFINE_SET_CALDATA(B, right)

#define DEFINE_SET_KEYSET(num, dir)                          \
    static int set_keyset##num##_##dir##(resource_value_t v) \
    {                                                        \
        keyset##num##[KEYSET_##dir##] = (int) v;             \
        return 0;                                            \
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

#define DEFINE_RES_SET_CALDATA(txt, num, dir, def) \
    { txt, RES_INTEGER, (resource_value_t) def, \
    (resource_value_t *) &joy##num##_##dir##, set_joy##num##_##dir##}

static resource_t resources[] = {
    { "JoyDevice1", RES_INTEGER, (resource_value_t) JOYDEV_NONE,
      (resource_value_t *) &joystick_device_1, set_joystick_device_1 },
    { "JoyDevice2", RES_INTEGER, (resource_value_t) JOYDEV_NONE,
      (resource_value_t *) &joystick_device_2, set_joystick_device_2 },
    DEFINE_RES_SET_CALDATA("JoyAup",    A, up,    200),
    DEFINE_RES_SET_CALDATA("JoyAdown",  A, down,  600),
    DEFINE_RES_SET_CALDATA("JoyAleft",  A, left,  200),
    DEFINE_RES_SET_CALDATA("JoyAright", A, right, 600),
    DEFINE_RES_SET_CALDATA("JoyBup",    B, up,    200),
    DEFINE_RES_SET_CALDATA("JoyBdown",  B, down,  600),
    DEFINE_RES_SET_CALDATA("JoyBleft",  B, left,  200),
    DEFINE_RES_SET_CALDATA("JoyBright", B, right, 600),
    { "KeySet1NorthWest", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset1[KEYSET_NW], set_keyset1_NW },
    { "KeySet1North", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset1[KEYSET_N], set_keyset1_N },
    { "KeySet1NorthEast", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset1[KEYSET_NE], set_keyset1_NE },
    { "KeySet1East", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset1[KEYSET_E], set_keyset1_E },
    { "KeySet1SouthEast", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset1[KEYSET_SE], set_keyset1_SE },
    { "KeySet1South", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset1[KEYSET_S], set_keyset1_S },
    { "KeySet1SouthWest", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset1[KEYSET_SW], set_keyset1_SW },
    { "KeySet1West", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset1[KEYSET_W], set_keyset1_W },
    { "KeySet1Fire", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset1[KEYSET_FIRE], set_keyset1_FIRE },
    { "KeySet2NorthWest", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset2[KEYSET_NW], set_keyset2_NW },
    { "KeySet2North", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset2[KEYSET_N], set_keyset2_N },
    { "KeySet2NorthEast", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset2[KEYSET_NE], set_keyset2_NE },
    { "KeySet2East", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset2[KEYSET_E], set_keyset2_E },
    { "KeySet2SouthEast", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset2[KEYSET_SE], set_keyset2_SE },
    { "KeySet2South", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset2[KEYSET_S], set_keyset2_S },
    { "KeySet2SouthWest", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset2[KEYSET_SW], set_keyset2_SW },
    { "KeySet2West", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset2[KEYSET_W], set_keyset2_W },
    { "KeySet2Fire", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset2[KEYSET_FIRE], set_keyset2_FIRE },

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
      "<number>", "Set input device for joystick port #1" },
    { "-joydev2", SET_RESOURCE, 1, NULL, NULL,
      "JoyDevice2", NULL,
      "<number>", "Set input device for joystick port #2" },
    { "-joy1cal", CALL_FUNCTION, 0, &set_joyA_autoCal,
      (void *) TRUE, NULL, 0, NULL, "Start auto calibration for joystick #1" },
    { "-joy2cal", CALL_FUNCTION, 0, &set_joyB_autoCal,
      (void *) TRUE, NULL, 0, NULL, "Start auto calibration for joystick #2" },
    { NULL }
};

int joystick_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

/* Flag: is joystick present?  */
int number_joysticks = 0;

/* ------------------------------------------------------------------------- */

int handle_keyset_mapping(joystick_device_t device, int *set,
                          kbd_code_t kcode, int pressed)
{
    if (joystick_device_1 & device || joystick_device_2 & device) {
        BYTE value = 0;

        if (kcode == set[KEYSET_NW])    /* North-West */
            value = 5;
        else if (kcode == set[KEYSET_N]) /* North */
            value = 1;
        else if (kcode == set[KEYSET_NE]) /* North-East */
            value = 9;
        else if (kcode == set[KEYSET_E]) /* East */
            value = 8;
        else if (kcode == set[KEYSET_SE]) /* South-East */
            value = 10;
        else if (kcode == set[KEYSET_S]) /* South */
            value = 2;
        else if (kcode == set[KEYSET_SW]) /* South-West */
            value = 6;
        else if (kcode == set[KEYSET_W]) /* West */
            value = 4;
        else if (kcode == set[KEYSET_FIRE]) /* Fire */
            value = 16;
        else
            return 0;

        if (pressed) {
            if (joystick_device_1 & device) joystick_value[1] |= value;
            if (joystick_device_2 & device) joystick_value[2] |= value;
        } else {
            if (joystick_device_1 & device) joystick_value[1] &= ~value;
            if (joystick_device_2 & device) joystick_value[2] &= ~value;
        }
        return 1;
    }

    return 0;
}

/* ------------------------------------------------------------------------- */

static HFILE SWhGame = NULL;
static HMTX  hmtxJoystick;

/* Initialize joystick support.  */
void joystick_init(void)
{
    ULONG action;    // return value from DosOpen
    APIRET rc;

    if (SWhGame) return;
    DosCreateMutexSem("\\SEM32\\ViceJoystick", &hmtxJoystick, 0, TRUE);
    if (rc=DosOpen("GAME$", &SWhGame, &action, 0, FILE_READONLY, FILE_OPEN,
                 OPEN_ACCESS_READONLY | OPEN_SHARE_DENYNONE, NULL))
    {
        number_joysticks = 0;
        log_message(LOG_DEFAULT, "joystick.c: DosOpen (rc=%i)", rc);
    }
    else
        number_joysticks = 1;  // how to get number of joysticks?
    DosReleaseMutexSem(hmtxJoystick);
}

/* Update the `joystick_value' variables according to the joystick status.  */
void joystick_update(void)
{
    APIRET rc;
    static int joyA_xmin, joyA_xmax;
    static int joyA_ymin, joyA_ymax;
    static int joyB_xmin, joyB_xmax;
    static int joyB_ymin, joyB_ymax;
    static GAME_STATUS_STRUCT gameStatus;      // joystick readings
    static ULONG dataLen = sizeof(gameStatus); // length of gameStatus

    if (!number_joysticks) return;
    // if (SWhGame == 0) return FALSE; // exit if game port is not opened

    DosRequestMutexSem(hmtxJoystick, SEM_INDEFINITE_WAIT);
    rc=DosDevIOCtl(SWhGame, IOCTL_CAT_USER, GAME_GET_STATUS, NULL, 0, NULL,
                   &gameStatus, dataLen, &dataLen);
    DosReleaseMutexSem(hmtxJoystick);

    if (rc) return;

    if (joystick_device_1&JOYDEV_HW1 || joystick_device_2&JOYDEV_HW1)
    {
        int value = 0;
        GAME_2DPOS_STRUCT *A = &(gameStatus.curdata.A);
        if (joyA_start) {
            joyA_xmin = joyA_xmax = A->x;
            joyA_ymin = joyA_ymax = A->y;
            joyA_up    = 9*A->y/10;
            joyA_down  =11*A->y/10;
            joyA_left  = 9*A->x/10;
            joyA_right =11*A->x/10;
            joyA_start =FALSE;
        }
        if (A->y < joyA_up)
        {
            value |= 1;
            if (joyA_autoCal && A->y < joyA_ymin)
            {
                joyA_ymin=A->y;
                joyA_up   =(3*joyA_ymin+joyA_ymax)/4;
                joyA_down =3*(joyA_ymin+joyA_ymax)/4;
                //                log_message(LOG_DEFAULT,"ymin: %i  up: %i",joyA_ymin, joyA_up);
            }
        }
        if (A->y > joyA_down)
        {
            value |= 2;
            if (joyA_autoCal && A->y > joyA_ymax)
            {
                joyA_ymax=A->y;
                joyA_up   =(3*joyA_ymin+joyA_ymax)/4;
                joyA_down =3*(joyA_ymin+joyA_ymax)/4;
                //                log_message(LOG_DEFAULT,"ymax: %i  dn: %i",joyA_ymax, joyA_down);
            }
        }
        if (A->x < joyA_left)
        {
            value |= 4;
            if (joyA_autoCal && A->x < joyA_xmin)
            {
                joyA_xmin=A->x;
                joyA_left =(3*joyA_xmin+joyA_xmax)/4;
                joyA_right=3*(joyA_xmin+joyA_xmax)/4;
                //                log_message(LOG_DEFAULT,"xmin: %i  lt: %i",joyA_xmin, joyA_left);
            }
        }
        if (A->x > joyA_right)
        {
            value |= 8;
            if (joyA_autoCal && A->x > joyA_xmax)
            {
                joyA_xmax =A->x;
                joyA_left =(3*joyA_xmin+joyA_xmax)/4;
                joyA_right=3*(joyA_xmin+joyA_xmax)/4;
                //                log_message(LOG_DEFAULT,"xmax: %i  rt: %i",joyA_xmax, joyA_right);
            }
        }
        if (~gameStatus.curdata.butMask & JOYA_BUTTONS) value |= 16;
        if (joystick_device_1 & JOYDEV_HW1) joystick_value[1] = value;
        if (joystick_device_2 & JOYDEV_HW1) joystick_value[2] = value;
    }

    if (number_joysticks >= 2 &&
        (joystick_device_1&JOYDEV_HW2 || joystick_device_2&JOYDEV_HW2))
    {
        int value = 0;
        GAME_2DPOS_STRUCT *B=&(gameStatus.curdata.A);
        if (B->y < joyB_up)    value |= 1;
        if (B->y > joyB_down)  value |= 2;
        if (B->x < joyB_left)  value |= 4;
        if (B->x > joyB_right) value |= 8;
        if (~gameStatus.curdata.butMask & JOYB_BUTTONS) value |= 16;
        if (joystick_device_1 & JOYDEV_HW2) joystick_value[1] = value;
        if (joystick_device_2 & JOYDEV_HW2) joystick_value[2] = value;
    }
}

/* Strange! If video_init fails this is called without calling DosOpen before! */
void joystick_close(void)
{
    APIRET rc ;
    DosRequestMutexSem(hmtxJoystick, SEM_INDEFINITE_WAIT);
    rc=DosClose(SWhGame);
    DosCloseMutexSem(hmtxJoystick);
    if (rc) log_message(LOG_DEFAULT, "joystick.c: DosClose (rc=%i)", rc);
}

/* Handle keys to emulate the joystick.  Warning: this is called within the
   keyboard interrupt, so take care when modifying this code!  */
int joystick_handle_key(kbd_code_t kcode, int pressed)
{
    int value = 0;

    /* The numpad case is handled specially because it allows users to use
       both `5' and `2' for "down".  */
    if (joystick_device_1 & JOYDEV_NUMPAD
        || joystick_device_2 & JOYDEV_NUMPAD) {

        switch (kcode) {
          case K_KP7:               /* North-West */
            value = 5;
            break;
          case K_KP8:               /* North */
            value = 1;
            break;
          case K_KP9:               /* North-East */
            value = 9;
            break;
          case K_KP6:               /* East */
            value = 8;
            break;
          case K_KP3:               /* South-East */
            value = 10;
            break;
          case K_KP2:               /* South */
          case K_KP5:
            value = 2;
            break;
          case K_KP1:               /* South-West */
            value = 6;
            break;
          case K_KP4:               /* West */
            value = 4;
            break;
          case K_KP0:
          case K_RIGHTCTRL:
            value = 16;
            break;
          default:
            /* (make compiler happy) */
            break;
        }

        if (pressed) {
            if (joystick_device_1 & JOYDEV_NUMPAD) joystick_value[1] |= value;
            if (joystick_device_2 & JOYDEV_NUMPAD) joystick_value[2] |= value;
        } else {
            if (joystick_device_1 & JOYDEV_NUMPAD) joystick_value[1] &= ~value;
            if (joystick_device_2 & JOYDEV_NUMPAD) joystick_value[2] &= ~value;
        }
    }

    /* (Notice we have to handle all the keysets even when one key is used
       more than once (the most intuitive behavior), so we use `|' instead of
       `||'.)  */
    return (value
            | handle_keyset_mapping(JOYDEV_KEYSET1, keyset1, kcode, pressed)
            | handle_keyset_mapping(JOYDEV_KEYSET2, keyset2, kcode, pressed));
}

/* ------------------------------------------------------------------------- */

const char *joystick_direction_to_string(joystick_direction_t direction)
{
    static char *s[] = {
        "NorthWest", "North", "NorthEast", "East",
        "SouthEast", "South", "SouthWest", "West",
        "Fire"
    };

    return s[(int) direction];
}
