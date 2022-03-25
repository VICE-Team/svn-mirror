/** \file   joystick_linux.c
 * \brief   Linux joystick support
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

#if defined LINUX_JOYSTICK

#include <fcntl.h>
#include <linux/joystick.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "joyport.h"
#include "joystick.h"
#include "log.h"
#include "types.h"
#include "lib.h"

#define ANALOG_JOY_NUM 8

static log_t joystick_linux_log = LOG_ERR;

typedef struct linux_joystick_priv_s {
    int fd;
} linux_joystick_priv_t;

static void linux_joystick_close(void* priv)
{
    close (((linux_joystick_priv_t *)priv)->fd);
    lib_free(priv);
}

static void linux_joystick(int joyport, void* priv)
{
    linux_joystick_priv_t *joypriv = priv;
    struct js_event e;
    joystick_axis_value_t dir;

    /* Read all queued events. */
    while (read(joypriv->fd, &e, sizeof(struct js_event)) == sizeof(struct js_event)) {
        switch (e.type & ~JS_EVENT_INIT) {
        case JS_EVENT_BUTTON:
            joy_button_event(joyport, e.number, e.value);
            break;
        case JS_EVENT_AXIS:
            if (e.value > 16384) {
                dir = JOY_AXIS_POSITIVE;
            } else if (e.value < -16384) {
                dir = JOY_AXIS_NEGATIVE;
            } else {
                dir = JOY_AXIS_MIDDLE;
            }
            joy_axis_event(joyport, e.number, dir);
             break;
        }
    }
}

static joystick_driver_t linux_joystick_driver = {
    .poll = linux_joystick,
    .close = linux_joystick_close
};

void linux_joystick_init(void)
{
    int i;


    if (joystick_linux_log == LOG_ERR) {
        joystick_linux_log = log_open("Joystick");
    }

    log_message(joystick_linux_log, "Linux joystick interface initialization...");

    /* open analog device files */

    for (i = 0; i < ANALOG_JOY_NUM; i++) {
        char dev[256];
        int fd;
        int ver = 0;
        uint8_t axes;
        uint8_t buttons;
        char name[256];
        struct JS_DATA_TYPE js;

        memset(dev, 0, sizeof(dev));
        memset(name, 0, sizeof(name));

        snprintf(dev, sizeof(dev) - 1, "/dev/js%d", i);
        fd = open(dev, O_RDONLY);
        if (fd < 0) {
            snprintf(dev, sizeof(dev) - 1, "/dev/input/js%d", i);
            fd = open(dev, O_RDONLY);
        }

        if (fd >= 0) {
            if (read (fd, &js, sizeof(struct JS_DATA_TYPE)) < 0) {
                close (fd);
                continue;
            }
            if (ioctl(fd, JSIOCGVERSION, &ver)) {
                log_message(joystick_linux_log, "%s unknown type", dev);
                log_message(joystick_linux_log, "Built in driver version: %d.%d.%d", JS_VERSION >> 16, (JS_VERSION >> 8) & 0xff, JS_VERSION & 0xff);
                log_message(joystick_linux_log, "Kernel driver version  : 0.8 ??");
                log_message(joystick_linux_log, "Please update your Joystick driver!");
                return;
            }
            ioctl(fd, JSIOCGAXES, &axes);
            ioctl(fd, JSIOCGBUTTONS, &buttons);
            ioctl(fd, JSIOCGNAME(sizeof(name) - 1), name);
            log_message(joystick_linux_log, "%s is %s", dev, name);
            log_message(joystick_linux_log, "Built in driver version: %d.%d.%d", JS_VERSION >> 16, (JS_VERSION >> 8) & 0xff, JS_VERSION & 0xff);
            log_message(joystick_linux_log, "Kernel driver version  : %d.%d.%d", ver >> 16, (ver >> 8) & 0xff, ver & 0xff);
            fcntl(fd, F_SETFL, O_NONBLOCK);
            if ((axes == 0) || (buttons == 0)) {
                log_message(joystick_linux_log, "Joystick with invalid geometry found -- ignoring.");
                continue;
            }
            linux_joystick_priv_t *priv = lib_malloc(sizeof(linux_joystick_priv_t));
            priv->fd = fd;
            register_joystick_driver(&linux_joystick_driver, name, priv, axes, buttons, 0);
        } else {
            log_warning(joystick_linux_log, "Cannot open joystick device `%s'.", dev);
        }
    }
}

#    endif  /* LINUX_JOYSTICK */
