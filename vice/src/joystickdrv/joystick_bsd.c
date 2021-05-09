/** \file   joystick_bsd.c
 * \brief   NetBSD/FreeBSD/DragonFly USB joystick support
 *
 * \author  Dieter Baron <dillo@nih.at>
 * \author  Marco van den Heuvel <blackystardust68@yahoo.com>
 *
 * \todo    Check if this code also works on OpenBSD.
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


#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "cmdline.h"
#include "joystick.h"
#include "keyboard.h"
#include "lib.h"
#include "log.h"
#include "resources.h"
#include "types.h"


#define ITEM_AXIS   0
#define ITEM_BUTTON 1
#define ITEM_HAT    2

static log_t bsd_joystick_log;

#ifdef HAVE_USB_H
#include <usb.h>
#endif

#ifdef __DragonFly__
/* sys/param.h contains the __DragonFly_version macro */
# include <sys/param.h>
# if __DragonFly_version >= 300200
/* DragonFly >= 3.2 (USB4BSD stack) */
#  include <bus/u4b/usb.h>
#  include <bus/u4b/usbhid.h>
# else
/* DragonFly < 3.2: old USB stack */
#  include <bus/usb/usb.h>
#  include <bus/usb/usbhid.h>
# endif
#else
# ifdef __FreeBSD__
#  include <sys/ioccom.h>
# endif
# include <dev/usb/usb.h>
# include <dev/usb/usbhid.h>
#endif

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#if defined(HAVE_USBHID_H)
#include <usbhid.h>
#elif defined(HAVE_LIBUSB_H)
#include <libusb.h>
#elif defined(HAVE_LIBUSBHID_H)
#include <libusbhid.h>
#endif

#define MAX_DEV 4   /* number of uhid devices to try */

static const uint8_t hat_map[9] = {
    0,
    JOYSTICK_DIRECTION_UP,
    JOYSTICK_DIRECTION_UP|JOYSTICK_DIRECTION_RIGHT,
    JOYSTICK_DIRECTION_RIGHT,
    JOYSTICK_DIRECTION_RIGHT|JOYSTICK_DIRECTION_DOWN,
    JOYSTICK_DIRECTION_DOWN,
    JOYSTICK_DIRECTION_DOWN|JOYSTICK_DIRECTION_LEFT,
    JOYSTICK_DIRECTION_LEFT,
    JOYSTICK_DIRECTION_LEFT|JOYSTICK_DIRECTION_UP
};

struct usb_joy_item {
    struct hid_item item;
    struct usb_joy_item *next;

    int type;
    int min_val;
    int max_val;
    int ordinal_number;
};

typedef struct bsd_joystick_priv_s {
    struct usb_joy_item *usb_joy_item;
    char *usb_joy_buf;
    int usb_joy_fd;
    int usb_joy_size;
} bsd_joystick_priv_t;

static int usb_joy_add_item(struct usb_joy_item **item, struct hid_item *hi, int orval, int type)
{
    struct usb_joy_item *it;
    int w;

    if ((it=malloc(sizeof(*it))) == NULL) {
        /* XXX */
        return -1;
    }

    it->next = *item;
    *item = it;

    memcpy(&it->item, hi, sizeof(*hi));
    it->type = type;
    switch (type) {
        case ITEM_AXIS:
            w = (hi->logical_maximum - hi->logical_minimum) / 3;
            it->ordinal_number = orval;
            it->min_val = hi->logical_minimum + w;
            it->max_val = hi->logical_maximum - w;
            break;
        case ITEM_BUTTON:
            it->min_val = hi->logical_minimum;
            it->ordinal_number = orval;
            it->max_val = hi->logical_maximum - 1;
            break;
    }

    return 0;
}

static void usb_free_item(struct usb_joy_item **item)
{
    struct usb_joy_item *it, *it2;

    it=*item;
    while (it) {
        it2 = it;
        it = it->next;
        free(it2);
    }
    *item = NULL;
}

static void usb_joystick_close(void* priv)
{
    bsd_joystick_priv_t *joypriv = priv;
    close(joypriv->usb_joy_fd);
    usb_free_item(&joypriv->usb_joy_item);
    lib_free(priv);
}

static void usb_joystick(int jp, void* priv)
{
    int val;
    ssize_t ret;
    struct usb_joy_item *it;
    bsd_joystick_priv_t *joypriv = priv;

    val = 0;
    while ((ret = read(joypriv->usb_joy_fd, joypriv->usb_joy_buf, joypriv->usb_joy_size)) == joypriv->usb_joy_size) {
        val = 1;
    }
    if (ret != -1 && errno != EAGAIN) {
        /* XXX */
        printf("strange read return: %zd/%d\n", ret, errno);
        return;
    }
    if (!val) {
        return;
    }

    for (it = joypriv->usb_joy_item; it; it = it->next) {
        val = hid_get_data(joypriv->usb_joy_buf, &it->item);
        if (it->type == ITEM_HAT) {
            if (val >= 0 && val <= 8) {
                printf("ordinal %d val %d map %u\n",it->ordinal_number, val,hat_map[val]);
                joy_hat_event(jp, it->ordinal_number, hat_map[val]);
            }
        } else {
            if (it->type == ITEM_BUTTON) {
                joy_button_event(jp, it->ordinal_number, val);
            } else if (val <= it->min_val) {
                joy_axis_event(jp, it->ordinal_number, JOY_AXIS_NEGATIVE);
            } else if (val > it->max_val) {
                joy_axis_event(jp, it->ordinal_number, JOY_AXIS_POSITIVE);
            } else {
                joy_axis_event(jp, it->ordinal_number, JOY_AXIS_MIDDLE);
            }
        }
    }
}

static joystick_driver_t bsd_joystick_driver = {
    .poll = usb_joystick,
    .close = usb_joystick_close
};

void usb_joystick_init(void)
{
    int j, id = 0, fd;
    report_desc_t report;
    struct hid_item h;
    struct hid_data *d;
    char dev[32];
    char name[20];
    int next_ordinal_to_assign;
    int ordinal_to_assign;
    int found_x;
    int found_y;
    struct usb_joy_item *it;
    char axes = 0, buttons = 0, hats = 0;
    bsd_joystick_priv_t *priv;
    int usb_joy_size;

    for (j=0; j<MAX_DEV; j++) {
        axes = 0;
        buttons = 0;
        hats = 0;
        next_ordinal_to_assign = 0;

        sprintf(dev, "/dev/uhid%d", j);
        fd = open(dev, O_RDONLY | O_NONBLOCK);
        if (fd < 0) {
            continue;
        }

#if defined(USB_GET_REPORT_IDusb_joy_add_item) && !defined(__DragonFly__)
        if (ioctl(fd, USB_GET_REPORT_ID, &id) < 0) {
            log_warning(bsd_joystick_log, "Cannot get report id for joystick device `%s'.", dev);
            close(fd);
        }
#endif

        if ((report=hid_get_report_desc(fd)) == NULL) {
            log_warning(bsd_joystick_log, "Cannot report description for joystick device `%s'.", dev);
            close(fd);
            continue;
        }
        usb_joy_size = hid_report_size(report, hid_input, id);

        next_ordinal_to_assign = 2;
        found_x = 0;
        found_y = 0;
        priv = NULL;
#if !defined(HAVE_USBHID_H) && !defined(HAVE_LIBUSB_H) && defined(HAVE_LIBUSBHID)
        for (d = hid_start_parse(report, id);
#else
        for (d = hid_start_parse(report, 1 << hid_input, id);
#endif
        hid_get_item(d, &h);) {

            if (h.kind == hid_collection && HID_PAGE(h.usage) == HUP_GENERIC_DESKTOP && (HID_USAGE(h.usage) == HUG_JOYSTICK || HID_USAGE(h.usage) == HUG_GAME_PAD)) {
                if (priv == NULL) {
                    priv = lib_malloc(sizeof(bsd_joystick_priv_t));
                    priv->usb_joy_size = usb_joy_size;
                    priv->usb_joy_item = NULL;
                }
                continue;
            }
            if (!priv) {
                continue;
            }

            switch (HID_PAGE(h.usage)) {
                case HUP_GENERIC_DESKTOP:
                    switch (HID_USAGE(h.usage)) {
                        case HUG_X:
                        case HUG_RX:
                            if (!found_x) {
                                ordinal_to_assign = 0;
                            } else {
                                ordinal_to_assign = next_ordinal_to_assign;
                            }
                            if (usb_joy_add_item(&priv->usb_joy_item, &h, ordinal_to_assign, ITEM_AXIS) == 0) {
                               axes++;
                                 if (!found_x) {
                                   found_x = 1;
                                  } else {
                                    next_ordinal_to_assign++;
                                }
                            }
                            break;
                        case HUG_Y:
                        case HUG_RY:
                            if (!found_y) {
                                ordinal_to_assign = 1;
                            } else {
                                ordinal_to_assign = next_ordinal_to_assign;
                            }
                            if (usb_joy_add_item(&priv->usb_joy_item, &h, ordinal_to_assign, ITEM_AXIS) == 0) {
                                axes++;
                                 if (!found_y) {
                                   found_y = 1;
                                  } else {
                                    next_ordinal_to_assign++;
                                }
                            }
                            break;
                        case HUG_HAT_SWITCH:
                            if (usb_joy_add_item(&priv->usb_joy_item, &h, hats, ITEM_HAT) == 0) {
                                hats++;
                            }
                            break;
                    }
                    break;
                case HUP_BUTTON:
                    if (usb_joy_add_item(&priv->usb_joy_item, &h, buttons, ITEM_BUTTON) == 0) {
                        buttons++;
                    }
                    break;
            }
        }

        hid_end_parse(d);
        if (!priv) {
            continue;
        }
        if (!found_x || !found_y) {
            next_ordinal_to_assign = 0;
            for (it = priv->usb_joy_item; it; it = it->next) {
                if (it->type == ITEM_AXIS) {
                    it->ordinal_number = next_ordinal_to_assign++;
                }
            }
        }

        if ((priv->usb_joy_buf = malloc(priv->usb_joy_size)) == NULL) {
            log_warning(bsd_joystick_log, "Cannot allocate buffer for joystick device `%s'.", dev);
            close(fd);
            usb_free_item(&priv->usb_joy_item);
            lib_free(priv);
            continue;
        }

        log_message(bsd_joystick_log, "USB joystick found: `%s'.", dev);
        priv->usb_joy_fd = fd;
        snprintf(name, sizeof(name), "Joystick %d", j);
        register_joystick_driver(&bsd_joystick_driver, name, priv, axes, buttons, hats);
    }
}
