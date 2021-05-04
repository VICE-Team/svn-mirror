/** \file   joy-unix.c
 * \brief   Linux/BSD joystick support
 *
 * \author  Bernhard Kuhn <kuhn@eikon.e-technik.tu-muenchen.de>
 * \author  Ulmer Lionel <ulmer@poly.polytechnique.fr>
 * \author  Marco van den Heuvel <blackystardust68@yahoo.com>
 * \author  Daniel Sladic <sladic@eecg.toronto.edu>
 * \author  Krister Walfridsson <cato@df.lth.se>
 * \author  Luca Montecchiani  <m.luca@usa.net> (http://i.am/m.luca)
 */

/*
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
#include "archdep_defs.h"

#if defined(UNIX_COMPILE) && !defined(MACOSX_SUPPORT)

#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "cmdline.h"
#include "joy.h"
#include "joyport.h"
#include "joystick.h"
#include "keyboard.h"
#include "log.h"
#include "resources.h"
#include "types.h"

int joy_arch_set_device(int port, int new_dev)
{
    if (new_dev < 0 || new_dev > JOYDEV_MAX) {
        return -1;
    }

    return 0;
}

/* Resources.  */


int joy_arch_resources_init(void)
{
    return 0;
}

/* Command-line options.  */

#if 0
static const cmdline_option_t joydev1cmdline_options[] =
{
    { "-joydev1", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "JoyDevice1", NULL,
#ifdef HAS_USB_JOYSTICK
    "<0-13>", "Set device for joystick port 1 (0: None, 1: Numpad, 2: Keyset 1, 3: Keyset 2, 4: Analog joystick 0, 5: Analog joystick 1, 6: Analog joystick 2, 7: Analog joystick 3, 8: Analog joystick 4, 9: Analog joystick 5, 10: Digital joystick 0, 11: Digital joystick 1, 12: USB joystick 0, 13: USB joystick 1)" },
#else
    "<0-9>", "Set device for joystick port 1 (0: None, 1: Numpad, 2: Keyset 1, 3: Keyset 2, 4: Analog joystick 0, 5: Analog joystick 1, 6: Analog joystick 2, 7: Analog joystick 3, 8: Analog joystick 4, 9: Analog joystick 5)" },
#endif
    CMDLINE_LIST_END
};

static const cmdline_option_t joydev2cmdline_options[] =
{
    { "-joydev2", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "JoyDevice2", NULL,
#ifdef HAS_USB_JOYSTICK
    "<0-13>", "Set device for joystick port 2 (0: None, 1: Numpad, 2: Keyset 1, 3: Keyset 2, 4: Analog joystick 0, 5: Analog joystick 1, 6: Analog joystick 2, 7: Analog joystick 3, 8: Analog joystick 4, 9: Analog joystick 5, 10: Digital joystick 0, 11: Digital joystick 1, 12: USB joystick 0, 13: USB joystick 1)" },
#else
    "<0-9>", "Set device for joystick port 2 (0: None, 1: Numpad, 2: Keyset 1, 3: Keyset 2, 4: Analog joystick 0, 5: Analog joystick 1, 6: Analog joystick 2, 7: Analog joystick 3, 8: Analog joystick 4, 9: Analog joystick 5)" },
#endif
    CMDLINE_LIST_END
};

static const cmdline_option_t joydev3cmdline_options[] =
{
    { "-extrajoydev1", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "JoyDevice3", NULL,

#ifdef HAS_USB_JOYSTICK
    "<0-13>", "Set device for extra joystick port 1 (0: None, 1: Numpad, 2: Keyset 1, 3: Keyset 2, 4: Analog joystick 0, 5: Analog joystick 1, 6: Analog joystick 2, 7: Analog joystick 3, 8: Analog joystick 4, 9: Analog joystick 5, 10: Digital joystick 0, 11: Digital joystick 1, 12: USB joystick 0, 13: USB joystick 1)" },
#else
    "<0-9>", "Set device for extra joystick port 1 (0: None, 1: Numpad, 2: Keyset 1, 3: Keyset 2, 4: Analog joystick 0, 5: Analog joystick 1, 6: Analog joystick 2, 7: Analog joystick 3, 8: Analog joystick 4, 9: Analog joystick 5)" },
#endif
    CMDLINE_LIST_END
};

static const cmdline_option_t joydev4cmdline_options[] =
{
    { "-extrajoydev2", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "JoyDevice4", NULL,
#ifdef HAS_USB_JOYSTICK
    "<0-13>", "Set device for extra joystick port 2 (0: None, 1: Numpad, 2: Keyset 1, 3: Keyset 2, 4: Analog joystick 0, 5: Analog joystick 1, 6: Analog joystick 2, 7: Analog joystick 3, 8: Analog joystick 4, 9: Analog joystick 5, 10: Digital joystick 0, 11: Digital joystick 1, 12: USB joystick 0, 13: USB joystick 1)" },
#else
    "<0-9>", "Set device for extra joystick port 2 (0: None, 1: Numpad, 2: Keyset 1, 3: Keyset 2, 4: Analog joystick 0, 5: Analog joystick 1, 6: Analog joystick 2, 7: Analog joystick 3, 8: Analog joystick 4, 9: Analog joystick 5)" },
#endif
    CMDLINE_LIST_END
};

static const cmdline_option_t joydev5cmdline_options[] =
{
    { "-extrajoydev3", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "JoyDevice5", NULL,
#ifdef HAS_USB_JOYSTICK
    "<0-13>", "Set device for extra joystick port 3 (0: None, 1: Numpad, 2: Keyset 1, 3: Keyset 2, 4: Analog joystick 0, 5: Analog joystick 1, 6: Analog joystick 2, 7: Analog joystick 3, 8: Analog joystick 4, 9: Analog joystick 5, 10: Digital joystick 0, 11: Digital joystick 1, 12: USB joystick 0, 13: USB joystick 1)" },
#else
    "<0-9>", "Set device for extra joystick port 3 (0: None, 1: Numpad, 2: Keyset 1, 3: Keyset 2, 4: Analog joystick 0, 5: Analog joystick 1, 6: Analog joystick 2, 7: Analog joystick 3, 8: Analog joystick 4, 9: Analog joystick 5)" },
#endif
    CMDLINE_LIST_END
};
#endif


int joy_arch_cmdline_options_init(void)
{
    /* NOP */
    return 0;
}
#if 0
    if (joyport_get_port_name(JOYPORT_1)) {
        if (cmdline_register_options(joydev1cmdline_options) < 0) {
            return -1;
        }
    }
    if (joyport_get_port_name(JOYPORT_2)) {
        if (cmdline_register_options(joydev2cmdline_options) < 0) {
            return -1;
        }
    }
    if (joyport_get_port_name(JOYPORT_3)) {
        if (cmdline_register_options(joydev3cmdline_options) < 0) {
            return -1;
        }
    }
    if (joyport_get_port_name(JOYPORT_4)) {
        if (cmdline_register_options(joydev4cmdline_options) < 0) {
            return -1;
        }
    }
    if (joyport_get_port_name(JOYPORT_5)) {
        if (cmdline_register_options(joydev5cmdline_options) < 0) {
            return -1;
        }
    }

    return 0;
}
#endif

/* ------------------------------------------------------------------------- */

#    if defined(BSD_JOYSTICK)
#      ifdef HAVE_MACHINE_JOYSTICK_H
#        include <machine/joystick.h>
#      endif
#      ifdef HAVE_SYS_JOYSTICK_H
#        include <sys/joystick.h>
#      endif
#    endif

#    define ANALOG_JOY_NUM (JOYDEV_ANALOG_7 - JOYDEV_ANALOG_0 + 1)

/* file handles for the joystick device files */

#ifdef BSD_JOYSTICK
static int ajoyfd[ANALOG_JOY_NUM] = { -1, -1, -1, -1, -1, -1, -1, -1 };
#    define JOYCALLOOPS 100
#    define JOYSENSITIVITY 5
static int joyxcal[2];
static int joyycal[2];
static int joyxmin[2];
static int joyxmax[2];
static int joyymin[2];
static int joyymax[2];
#endif /* BSD_JOYSTICK */

log_t joystick_log = LOG_ERR;

/* ------------------------------------------------------------------------- */

/** \brief  Struct containing device name and id
 */
typedef struct device_info_s {
    const char *name;   /**< device name */
    int         id;     /**< device ID (\see joy.h) */
} device_info_t;

#ifdef BSD_JOYSTICK
static device_info_t predefined_device_list[] = {
    { "Analog joystick 0",  JOYDEV_ANALOG_0 },
    { "Analog joystick 1",  JOYDEV_ANALOG_1 },
    { "Analog joystick 2",  JOYDEV_ANALOG_2 },
    { "Analog joystick 3",  JOYDEV_ANALOG_3 },
    { "Analog joystick 4",  JOYDEV_ANALOG_4 },
    { "Analog joystick 5",  JOYDEV_ANALOG_5 },
    { "Analog joystick 6",  JOYDEV_ANALOG_6 },
    { "Analog joystick 7",  JOYDEV_ANALOG_7 },
#ifdef HAS_USB_JOYSTICK
    { "USB joystick 0",     JOYDEV_USB_0 },
    { "USB joystick 1",     JOYDEV_USB_1 },
#endif
    { NULL, -1 }
};

static int joystickdeviceidx = 0;

void joystick_ui_reset_device_list(void)
{
    joystickdeviceidx = 0;
}

/* FIXME: proper device names will be returned only when using the "new" API
          and only for "analog" joysticks. */
const char *joystick_ui_get_next_device_name(int *id)
{
    const char *name;

    /* printf("joystick_ui_get_next_device_name  id: %d\n", joystickdeviceidx); */

    if ((name = predefined_device_list[joystickdeviceidx].name)) {
        *id = predefined_device_list[joystickdeviceidx].id;
        joystickdeviceidx++;

        /* return name from the predefined list instead */
        return name;
    }
    return NULL;
}

/**********************************************************
 * Older Joystick routine BSD driver                      *
 **********************************************************/
static void bsd_joystick_init(void)
{
    int i;

    joystick_log = log_open("Joystick");

    /* close all device files */
    for (i = 0; i < 2; i++) {
        if (ajoyfd[i] != -1) {
            close(ajoyfd[i]);
        }
    }

    /* open analog device files */
    for (i = 0; i < 2; i++) {

        const char *dev;
        dev = (i == 0) ? "/dev/joy0" : "/dev/joy1";

        ajoyfd[i] = open(dev, O_RDONLY);
        if (ajoyfd[i] < 0) {
            log_warning(joystick_log, "Cannot open joystick device `%s'.", dev);
        } else {
            int j;

            /* calibration loop */
            for (j = 0; j < JOYCALLOOPS; j++) {
                struct joystick js;
                ssize_t status = read(ajoyfd[i], &js, sizeof(struct joystick));

                if (status != sizeof(struct joystick)) {
                    log_warning(joystick_log, "Error reading joystick device `%s'.", dev);
                } else {
                    /* determine average */
                    joyxcal[i] += js.x;
                    joyycal[i] += js.y;
                }
            }

            /* correct average */
            joyxcal[i] /= JOYCALLOOPS;
            joyycal[i] /= JOYCALLOOPS;

            /* determine tresholds */
            joyxmin[i] = joyxcal[i] - joyxcal[i] / JOYSENSITIVITY;
            joyxmax[i] = joyxcal[i] + joyxcal[i] / JOYSENSITIVITY;
            joyymin[i] = joyycal[i] - joyycal[i] / JOYSENSITIVITY;
            joyymax[i] = joyycal[i] + joyycal[i] / JOYSENSITIVITY;

            log_message(joystick_log, "Hardware joystick calibration for device `%s':", dev);
            log_message(joystick_log, "  X: min: %i , mid: %i , max: %i.", joyxmin[i], joyxcal[i], joyxmax[i]);
            log_message(joystick_log, "  Y: min: %i , mid: %i , max: %i.", joyymin[i], joyycal[i], joyymax[i]);
        }
    }
}

static void bsd_joystick_close(void)
{
    if (ajoyfd[0] > 0) {
        close(ajoyfd[0]);
    }
    if (ajoyfd[1] > 0) {
        close(ajoyfd[1]);
    }
}

static void bsd_joystick(void)
{
    int i;

    for (i = 1; i <= 5; i++) {
        int joyport = joystick_port_map[i - 1];

        if (joyport == JOYDEV_ANALOG_0 || joyport == JOYDEV_ANALOG_1) {
            ssize_t status;
            struct joystick js;
            int ajoyport = joyport - JOYDEV_ANALOG_0;

            if (ajoyfd[ajoyport] > 0) {
                status = read(ajoyfd[ajoyport], &js, sizeof(struct joystick));
                if (status != sizeof(struct joystick)) {
                    log_error(joystick_log, "Error reading joystick device.");
                } else {
                    joystick_set_value_absolute(i, 0);

                    if (js.y < joyymin[ajoyport]) {
                        joystick_set_value_or(i, 1);
                    }
                    if (js.y > joyymax[ajoyport]) {
                        joystick_set_value_or(i, 2);
                    }
                    if (js.x < joyxmin[ajoyport]) {
                        joystick_set_value_or(i, 4);
                    }
                    if (js.x > joyxmax[ajoyport]) {
                        joystick_set_value_or(i, 8);
                    }
                    if (js.b1 || js.b2) {
                        joystick_set_value_or(i, 16);
                    }
                }
            }
        }
    }
}

#    endif  /* BSD_JOYSTICK/LINUX_JOYSTICK */

/**********************************************************
 * Generic high level joy routine                         *
 **********************************************************/
int joy_arch_init(void)
{
#ifdef BSD_JOYSTICK
    bsd_joystick_init();
#endif
#    ifdef HAS_USB_JOYSTICK
    usb_joystick_init();
#    endif
    return 0;
}

#ifdef BSD_JOYSTICK
void joystick_close(void)
{
    bsd_joystick_close();
#    ifdef HAS_USB_JOYSTICK
    usb_joystick_close();
#endif
}

void joystick(void)
{
    bsd_joystick();
#    ifdef HAS_USB_JOYSTICK
    usb_joystick();
#    endif
}
#    endif

#endif
