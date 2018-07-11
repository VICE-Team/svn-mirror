/** \file   dirmenupopup.c
 *  \brief  Create a menu to show a directory of a drive or tape deck
 *
 *  \author Bas Wassink <b.wassink@ziggo.nl>
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

#include <gtk/gtk.h>
#include "debug_gtk3.h"
#include "lib.h"
#include "imagecontents/diskcontents.h"
#include "drive.h"
#include "drivetypes.h"
#include "diskimage.h"
#include "diskimage/fsimage.h"
#include "autostart.h"

#include "dirmenupopup.h"


static read_contents_func_type content_func = NULL;

static void (*response_func)(const char *, int);


static const char *disk_image = NULL;



/* FIXME: stole this from arch/unix/x11/gnome/x11ui.c
 */
static unsigned char *convert_utf8(unsigned char *s)
{
    unsigned char *d, *r;

    r = d = lib_malloc((size_t)(strlen((char *)s) * 2 + 1));
    while (*s) {
        if (*s < 0x80) {
            *d = *s;
        } else {
            /* special latin1 character handling */
            if (*s == 0xa0) {
                *d = 0x20;
            } else {
                if (*s == 0xad) {
                    *s = 0xed;
                }
                *d++ = 0xc0 | (*s >> 6);
                *d = (*s & ~0xc0) | 0x80;
            }
        }
        s++;
        d++;
    }
    *d = '\0';
    return r;
}


static void on_item_activate(GtkWidget *item, gpointer data)
{
    int index = GPOINTER_TO_INT(data);

    debug_gtk3("Got index %d, triggering response function", index);
    response_func(disk_image, index);
}


/** \brief  Create a popup menu to select a file to autostart
 *
 * XXX: This is an UNHOLY MESS, and should be refactored
 */
GtkWidget *content_preview_menu_create(
        int dev,
        read_contents_func_type func,
        void (*response)(const char *, int))
{
    GtkWidget *menu;
    GtkWidget *item;
    char buffer[1024];
    struct drive_context_s *drv_ctx;
    struct drive_s *drv_s;
    struct disk_image_s *image;
    image_contents_t *contents = NULL;
    image_contents_file_list_t *entry;
    char *utf8;
    char *tmp;
    const char *autostart_diskimage = NULL;
    int index;

    /* set callback functions */
    content_func = func;
    response_func = response;

    /* create new menu */
    menu = gtk_menu_new();

    /* create menu header */
    g_snprintf(buffer, 1024, "Directory of unit %d:", dev + 8);
    item = gtk_menu_item_new_with_label(buffer);
    gtk_container_add(GTK_CONTAINER(menu), item);

    /*
     * The following is complete horseshit, this needs to be implemented in a
     * function in drive/vdrive somehow. This much dereferencing in UI code
     * is not normal method.
     */
    drv_ctx = drive_context[dev];
    if (drv_ctx != NULL) {
        drv_s = drv_ctx->drive;
        if (drv_s != NULL) {
            image = drv_s->image;
            if (image != NULL) {
                /* this assumes fsimage, not real-image */
                struct fsimage_s *fsimg = image->media.fsimage;
                if (fsimg != NULL) {
                    autostart_diskimage = fsimg->name;
                }
            }
        }
    }

    if (autostart_diskimage != NULL) {
        /* read dir and add them as menu items */
        contents = content_func(autostart_diskimage);
        if (contents == NULL) {
            item = gtk_menu_item_new_with_label(
                    "Failed to read directory");
            gtk_container_add(GTK_CONTAINER(menu), item);
        } else {
            /* DISK name & ID */
            tmp = image_contents_to_string(contents, 0);
            utf8 = (char *)convert_utf8((unsigned char *)tmp);
            item = gtk_menu_item_new_with_label(utf8);
            gtk_container_add(GTK_CONTAINER(menu), item);
            lib_free(tmp);
            lib_free(utf8);

            /* set disk image path (there's gotta be a better way) */
            disk_image = autostart_diskimage;

            /* add files */
            index = 0;
            for (entry = contents->file_list; entry != NULL;
                    entry = entry->next) {

                tmp = image_contents_file_to_string(entry, 0);
                utf8 = (char *)convert_utf8((unsigned char *)tmp);
                item = gtk_menu_item_new_with_label(utf8);
                gtk_container_add(GTK_CONTAINER(menu), item);

                g_signal_connect(item, "activate",
                        G_CALLBACK(on_item_activate), GINT_TO_POINTER(index));
                index++;
                lib_free(tmp);
                lib_free(utf8);
            }
        }
    } else {
        item = gtk_menu_item_new_with_label("NO IMAGE ATTACHED");
        gtk_container_add(GTK_CONTAINER(menu), item);
    }
    gtk_widget_show_all(GTK_WIDGET(menu));


    return menu;
}
