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

#if defined(LINUX_JOYSTICK) && !defined(HAVE_LINUX_EVDEV)

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


/** \brief  Number of device nodes to scan */
#define ANALOG_JOY_NUM 8

/** \brief  Private, driver-specific data of the device */
typedef struct linux_joystick_priv_s {
    int fd;     /**< file descriptor of the joystick device node */
} linux_joystick_priv_t;


/** \brief  Linux joystick log */
static log_t joystick_linux_log = LOG_DEFAULT;


/** \brief  Callback for the joystick system to poll a joystick device
 *
 * \param[in]   joyport joystick device index
 * \param[in]   priv    driver-specific data
 */
static void linux_joystick_poll(int joyport, void *priv)
{
    linux_joystick_priv_t *joypriv = priv;
    struct js_event        event;

    /* Read all queued events. */
    while (read(joypriv->fd, &event, sizeof event) == sizeof event) {
        joystick_axis_value_t direction;

        switch (event.type & ~JS_EVENT_INIT) {

            case JS_EVENT_BUTTON:
                joy_button_event((uint8_t)joyport, event.number, (int16_t)event.value);
                break;

            case JS_EVENT_AXIS:
                if (event.value > 16384) {
                    direction = JOY_AXIS_POSITIVE;
                } else if (event.value < -16384) {
                    direction = JOY_AXIS_NEGATIVE;
                } else {
                    direction = JOY_AXIS_MIDDLE;
                }
                joy_axis_event((uint8_t)joyport, event.number, (int16_t)direction);
                break;

            default:
                break;
        }
    }
}

/** \brief  Callback for the joystick system to close a joystick device
 *
 * \param[in]   priv    driver-specific data
 */
static void linux_joystick_close(void *priv)
{
    linux_joystick_priv_t *joypriv = priv;

    close(joypriv->fd);
    lib_free(joypriv);
}


/** \brief  Object used to register driver for devices
 *
 * The address of this object is used in the joystick code, it isn't copied,
 * so we cannot move this into `linux_joystick_init()` to use for registration.
 */
static joystick_driver_t driver = {
    .poll  = linux_joystick_poll,
    .close = linux_joystick_close
};


/** \brief  Initialize linux joysticks
 *
 * Scan joystick device nodes for valid devices and register them with the
 * joystick system.
 */
void linux_joystick_init(void)
{
    int i;

    if (joystick_linux_log == LOG_DEFAULT) {
        joystick_linux_log = log_open("Joystick");
    }
    log_message(joystick_linux_log, "Linux joystick interface initialization...");

    /* open analog device files */
    for (i = 0; i < ANALOG_JOY_NUM; i++) {
        char dev[256];
        int  fd;

        memset(dev, 0, sizeof dev);
        snprintf(dev, sizeof dev - 1u, "/dev/js%d", i);
        fd = open(dev, O_RDONLY);
        if (fd < 0) {
            snprintf(dev, sizeof dev - 1u, "/dev/input/js%d", i);
            fd = open(dev, O_RDONLY);
        }

        if (fd >= 0) {
            struct JS_DATA_TYPE    js;
            linux_joystick_priv_t *priv;
            char                   name[256];
            uint8_t                axes;
            uint8_t                buttons;
            int                    ver = 0;

            /* read joystick data */
            if (read(fd, &js, sizeof js) < 0) {
                close(fd);
                continue;
            }

            /* check if the runtime drive is at least 1.0 and thus supports
             * events: */
            if (ioctl(fd, JSIOCGVERSION, &ver) != 0) {
                log_message(joystick_linux_log,
                            "%s unknown type", dev);
                log_message(joystick_linux_log,
                            "Built in driver version: %d.%d.%d",
                            JS_VERSION >> 16, (JS_VERSION >> 8) & 0xff, JS_VERSION & 0xff);
                log_message(joystick_linux_log,
                            "Kernel driver version  : 0.8 ??");
                log_message(joystick_linux_log,
                            "Please update your Joystick driver!");
                close(fd);
                return;
            }

            /* get number of axis, number of buttons and name of device */
            memset(name, 0, sizeof name);
            ioctl(fd, JSIOCGAXES, &axes);
            ioctl(fd, JSIOCGBUTTONS, &buttons);
            ioctl(fd, JSIOCGNAME(sizeof name - 1u), name);

            log_message(joystick_linux_log,
                        "%s is %s", dev, name);
            log_message(joystick_linux_log,
                        "Built in driver version: %d.%d.%d",
                        JS_VERSION >> 16, (JS_VERSION >> 8) & 0xff, JS_VERSION & 0xff);
            log_message(joystick_linux_log,
                        "Kernel driver version  : %d.%d.%d",
                        ver >> 16, (ver >> 8) & 0xff, ver & 0xff);

            /* check geometry */
            if ((axes == 0) || (buttons == 0)) {
                log_message(joystick_linux_log,
                            "Joystick with invalid geometry found -- ignoring.");
                close(fd);
                continue;
            }

            /* set file descriptor to non-blocking */
            fcntl(fd, F_SETFL, O_NONBLOCK);

            /* register device with the joystick system */
            priv = lib_malloc(sizeof *priv);
            priv->fd = fd;
            register_joystick_driver(&driver, name, priv, axes, buttons, 0);
        } else {
            log_warning(joystick_linux_log, "Cannot open joystick device `%s'.", dev);
        }
    }
}

#    endif  /* LINUX_JOYSTICK && !HAVE_LINUX_EVDEV */
