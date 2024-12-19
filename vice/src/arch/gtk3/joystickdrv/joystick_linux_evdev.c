/** \file   joystick_linux_evdev.c
 * \brief   Linux joystick driver using evdev
 *
 * Experimental Linux evdev joystick driver.
 *
 * The Linux /dev/js* API has been superceeded since many years by evdev. In
 * addition libevdev allows use to easily obtain information on event sources,
 * such as range, deadzone and fuzz and device information such as vendor,
 * product and version.
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
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

#if defined(HAVE_LINUX_EVDEV)

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <libevdev/libevdev.h>
#include <limits.h>
#include <linux/input.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "debug_gtk3.h"
#include "joystick.h"
#include "lib.h"
#include "log.h"


/** \brief  Array length helper */
#define ARRAY_LEN(arr) (sizeof arr / sizeof arr[0])

/** \brief  Minimum valid axis event code */
#define AXIS_CODE_MIN   ABS_X

/** \brief  Maximum valid axis event code */
#define AXIS_CODE_MAX   (ABS_RESERVED - 1u)

/** \brief  Minimum valid button event code */
#define BUTTON_CODE_MIN BTN_JOYSTICK

/** \brief  Maximum valid button event code */
#define BUTTON_CODE_MAX BTN_THUMBR

/** \brief  Maximum number of axis event codes */
#define NUM_AXES_MAX    (AXIS_CODE_MAX - AXIS_CODE_MIN + 1u)

/** \brief  Maximum number of button event codes */
#define NUM_BUTTONS_MAX (BUTTON_CODE_MAX - BUTTON_CODE_MIN + 1u)


/** \brief  Joystick axis object
 */
typedef struct joy_axis_s {
    uint16_t code;      /**< event code */
    int32_t  minimum;   /**< minimum value */
    int32_t  maximum;   /**< maximum value */
} joy_axis_t;

/** \brief  Driver-specific joystick data
 *
 * Contains data required by the evdev driver that isn't stored in the generic
 * joystick code.
 *
 * Unlike the other joystick drivers in VICE, Linux' event device API doesn't
 * use 0-based indexes for axes and buttons, but event types and event codes
 * in a certain range. So we need to keep an array of valid event codes for
 * buttons and axes, as well as axis minimum/maximum values since these aren't
 * always reported in the INT16_MIN to INT16_MAX range.
 */
typedef struct joy_priv_s {
    char            *name;                      /**< device name */
    struct libevdev *evdev;                     /**< evdev instance */
    uint16_t         vendor;                    /**< vendor ID */
    uint16_t         product;                   /**< product ID */
    uint16_t         version;                   /**< product version */
    int              fd;                        /**< file descriptor */
    uint32_t         num_axes;                  /**< number of axes */
    uint32_t         num_buttons;               /**< number of buttons */
    joy_axis_t       axes[NUM_AXES_MAX];        /**< list of axes */
    uint16_t         buttons[NUM_BUTTONS_MAX];  /**< list of buttons */
} joy_priv_t;


/** \brief  Log used for the Linux evdev driver */
static log_t joy_evdev_log;


/** \brief  Initialize joystick axis data
 *
 * \param[in]   axis    joystick axis data
 */
static void joy_axis_init(joy_axis_t *axis)
{
    axis->code    = 0;
    axis->minimum = INT16_MIN;
    axis->maximum = INT16_MAX;
}

/** \brief  Get index of axis event code
 *
 * \param[in]   priv    driver-specific joystick data
 * \param[in]   code    axis event code
 *
 * \return  index in axes array or -1 when not found
 */
static int32_t joy_axis_index(joy_priv_t *priv, uint16_t code)
{
    uint32_t i;

    /* ABS_X (AXIS_CODE_MIN) is 0 and comparing unsigned for < 0 is alway false */
    if (code > AXIS_CODE_MAX) {
        return -1;
    }
    for (i = 0; i < priv->num_axes; i++) {
        if (priv->axes[i].code == code) {
            return (int32_t)i;
        } else if (priv->axes[i].code > code) {
            return -1;  /* axis codes are stored in order */
        }
    }
    return -1;
}

/** \brief  Get index of button event code
 *
 * \param[in]   priv    driver-specific joystick data
 * \param[in]   code    button event code
 *
 * \return  index in buttons array or -1 when not found
 */
static int32_t joy_button_index(joy_priv_t *priv, uint16_t code)
{
    uint32_t i;

    if (code < BUTTON_CODE_MIN || code > BUTTON_CODE_MAX) {
        return -1;
    }
    for (i = 0; i < priv->num_buttons; i++) {
        if (priv->buttons[i] == code) {
            return (int32_t)i;
        } else if (priv->buttons[i] > code) {
            return -1;  /* button codes are stored in order */
        }
    }
    return -1;
}

/** \brief  Allocate and initialize driver-specific joystick data
 *
 * \return  new data instance, free with \c lib_free()
 */
static joy_priv_t *joy_priv_new(void)
{
    joy_priv_t *priv;
    size_t      i;

    priv = lib_malloc(sizeof *priv);
    priv->name        = NULL;
    priv->evdev       = NULL;
    priv->fd          = -1;
    priv->num_axes    = 0;
    priv->num_buttons = 0;
    for (i = 0; i < ARRAY_LEN(priv->buttons); i++) {
        priv->buttons[i] = 0;
    }
    for (i = 0; i < ARRAY_LEN(priv->axes); i++) {
        joy_axis_init(&(priv->axes[i]));
    }

    return priv;
}

/** \brief  Free driver-specific joystick data
 *
 * Close file descriptor, free evdev instance and other data.
 *
 * \param[in]   priv    driver-specific joystick data
 */
static void joy_priv_free(joy_priv_t *priv)
{
    if (priv != NULL) {
        lib_free(priv->name);
        libevdev_free(priv->evdev);
        close(priv->fd);
        lib_free(priv);
    }
}

/** \brief  Dispatcher for joystick events
 *
 * \param[in]   joyport joystick port index
 * \param[in]   priv    driver-specific joystick data
 * \param[in]   event   event data
 */
static void dispatch_event(int joyport, joy_priv_t *priv, struct input_event *event)
{
    int32_t index;

    if (event->type == EV_KEY) {
#if 0
        printf("button %02x (%s): %d\n",
               event->code, libevdev_event_code_get_name(EV_KEY, event->code),
               event->value);
#endif
        index = joy_button_index(priv, event->code);
        if (index >= 0) {
            joy_button_event((uint8_t)joyport, (uint8_t)index, (uint8_t)event->value);
        }
    } else if (event->type == EV_ABS) {
#if 0
        printf("axis %02x (%s): %d\n",
               event->code, libevdev_event_code_get_name(EV_ABS, event->code),
               event->value);
#endif
        index = joy_axis_index(priv, event->code);
        if (index >= 0) {
            int32_t               minimum   = priv->axes[index].minimum;
            int32_t               maximum   = priv->axes[index].maximum;
            int32_t               range     = maximum - minimum + 1;
            joystick_axis_value_t direction = JOY_AXIS_MIDDLE;

            /* ABS_HAT[0-3]XY axes return -1, 0 or 1: */
            if (minimum == -1 && maximum == 1) {
                if (event->value < 0) {
                    direction = JOY_AXIS_NEGATIVE;
                } else if (event->value > 0) {
                    direction = JOY_AXIS_POSITIVE;
                }
            } else if (event->value < (minimum + (range / 4))) {
                direction = JOY_AXIS_NEGATIVE;
            } else if (event->value > (maximum - (range / 4))) {
                direction = JOY_AXIS_POSITIVE;
            }
            joy_axis_event((uint8_t)joyport, (uint8_t)index, direction);
        }
    }
}

/** \brief  Poll callback for the joystick system
 *
 * \param[in]   joyport joystick port index
 * \param[in]   priv    driver-specific joystick data
 */
static void linux_joystick_evdev_poll(int joyport, void *priv)
{
    struct libevdev *evdev;
    int              rc;
    unsigned int     flags = LIBEVDEV_READ_FLAG_NORMAL;

    evdev = ((joy_priv_t *)priv)->evdev;

    while (libevdev_has_event_pending(evdev)) {
        struct input_event event;

        rc = libevdev_next_event(evdev, flags, &event);
        if (rc == LIBEVDEV_READ_STATUS_SYNC) {
            while (rc == LIBEVDEV_READ_STATUS_SYNC) {
                rc = libevdev_next_event(evdev, flags, &event);
            }
        } else if (rc == LIBEVDEV_READ_STATUS_SUCCESS) {
            if (event.type == EV_ABS || event.type == EV_KEY) {
                dispatch_event(joyport, priv, &event);
            }
        }
    }
}

/** \brief  Close callback for the joystick system
 *
 * Release resources associated with the joystick.
 *
 * \param[in]   priv    driver-specific joystick data
 */
static void linux_joystick_evdev_close(void *priv)
{
    joy_priv_free(priv);
}

/** \brief  Filter callback for scandir(3)
 *
 * \param[in]   de  dirent instance
 *
 * \return  \c 1 if the directory entry matches 'event*', \c 0 otherwise
 */
static int sd_filter(const struct dirent *de)
{
    const char *name = de->d_name;
    size_t      len  = strlen(name);

    /* we need "event" plus at least one more character */
    if (len > 5u && memcmp(name, "event", 5u) == 0) {
        return 1;
    }
    return 0;
}

/** \brief  Scan device for available buttons
 *
 * \param[in]   priv    joystick private data
 * \param[in]   evdev   libevdev instance
 */
static void scan_buttons(joy_priv_t *priv, struct libevdev *evdev)
{
    if (libevdev_has_event_type(evdev, EV_KEY)) {
        unsigned int code;

        for (code = BUTTON_CODE_MIN; code <= BUTTON_CODE_MAX; code++) {
            if (libevdev_has_event_code(evdev, EV_KEY, code)) {
                priv->buttons[priv->num_buttons++] = (uint16_t)code;
            }
        }
    }
}

/** \brief  Scan device for available axes
 *
 * \param[in]   priv    joystick private data
 * \param[in]   evdev   libevdev instance
 */
static void scan_axes(joy_priv_t *priv, struct libevdev *evdev)
{
    if (libevdev_has_event_type(evdev, EV_ABS)) {
        unsigned int code;

        for (code = AXIS_CODE_MIN; code <= AXIS_CODE_MAX; code++) {
            if (libevdev_has_event_code(evdev, EV_ABS, code)) {
                const struct input_absinfo *info;
                joy_axis_t                 *axis;

                info = libevdev_get_abs_info(evdev, code);
                axis = &(priv->axes[priv->num_axes++]);
                axis->code = (uint16_t)code;
                if (info != NULL) {
                    axis->minimum = info->minimum;
                    axis->maximum = info->maximum;
                }
            }
        }
    }
}

/** \brief  Scan possible joystick device for capabilities
 *
 * Try to open \a node and process with libevdev to determine its capabilities.
 *
 * Determines supported button event codes and axis event codes, along with
 * axis minimum/maximum values for correct translation into axis direction.
 *
 * \param[in]   node    node name in \c /dev/input/
 *
 * \return  new \c joy_priv_t instance on success, \c NULL on failure
 */
static joy_priv_t *scan_device(const char *node)
{
    struct libevdev *evdev;
    joy_priv_t      *priv;
    char             path[256];
    int              fd;
    int              rc;

    snprintf(path, sizeof path, "/dev/input/%s", node);

    fd = open(path, O_RDONLY|O_NONBLOCK);
    if (fd < 0) {
        return NULL;
    }

    rc = libevdev_new_from_fd(fd, &evdev);
    if (rc < 0) {
        log_error(LOG_DEFAULT, "failed to initialize libevdev: %s", strerror(rc));
        close(fd);
        return NULL;
    }

    priv = joy_priv_new();
    priv->name    = lib_strdup(libevdev_get_name(evdev));
    priv->vendor  = (uint16_t)libevdev_get_id_vendor(evdev);
    priv->product = (uint16_t)libevdev_get_id_product(evdev);
    priv->version = (uint16_t)libevdev_get_id_version(evdev);
    priv->evdev   = evdev;
    priv->fd      = fd;

    scan_axes(priv, evdev);
    scan_buttons(priv, evdev);

    return priv;
}


/** \brief  Object used to register driver for devices
 *
 * The address of this object is used in the joystick code, it isn't copied,
 * so we cannot move this into `linux_joystick_init()` to use for registration.
 */
static joystick_driver_t driver = {
    .poll  = linux_joystick_evdev_poll,
    .close = linux_joystick_evdev_close
};


/** \brief  Initialize Linux evdev joystick driver
 *
 * Scan device nodes in \c /dev/input/ for supported joysticks/gamepads.
 * Devices with less than one button or less than two axes are rejected.
 */
void linux_joystick_evdev_init(void)
{
    struct dirent **namelist = NULL;
    int             sd_result;
    int             i;

    joy_evdev_log = log_open("evdev Joystick");

    log_message(joy_evdev_log, "Initializing Linux evdev joystick driver.");

    sd_result = scandir("/dev/input", &namelist, sd_filter, alphasort);
    if (sd_result < 0) {
        log_error(LOG_DEFAULT, "scandir() failed on /dev/input: %s", strerror(errno));
        return;
    }

    for (i = 0; i < sd_result; i++) {
        joy_priv_t *priv;

        //log_message(joy_evdev_log, "Possible device '%s'", namelist[i]->d_name);
        priv = scan_device(namelist[i]->d_name);
        if (priv != NULL) {
            if (priv->num_axes < 2u || priv->num_buttons < 1u) {
                /* reject device */
                log_message(joy_evdev_log,
                            "Invalid geometry for %s: axes: %u, buttons: %u",
                            priv->name,
                            (unsigned int)priv->num_axes,
                            (unsigned int)priv->num_buttons);
                joy_priv_free(priv);
            } else {
#if 0
                log_message(joy_evdev_log,
                            "Adding device: %s [%04x:%04x] (%u axes, %u buttons)",
                            priv->name,
                            (unsigned int)priv->vendor, (unsigned int)priv->product,
                            (unsigned int)priv->num_axes, (unsigned int)priv->num_buttons);
                joy_priv_free(priv);
#endif
                register_joystick_driver(&driver, priv->name, priv, priv->num_axes, priv->num_buttons, 0);
            }
        }
        free(namelist[i]);
    }
    free(namelist);
}

#endif  /* ifdef HAVE_LINUX_EVDEV */
