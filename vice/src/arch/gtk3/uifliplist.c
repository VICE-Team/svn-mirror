/* vim: set et ts=4 sw=4 sts=4 fdm=marker syntax=c.doxygen: */

/** \file   uifliplist.c
 * \brief   Fliplist menu management
 *
 * \author  Michael C. Martin <mcmartin@gmail.com>
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
 */


#include "vice.h"

#include "attach.h"
#include "drive.h"
#include "fliplist.h"
#include "lib.h"
#include "util.h"
#include "filechooserhelpers.h"
#include "ui.h"
#include "uistatusbar.h"

#include "uifliplist.h"

/** \brief  Size of message buffer
 */
#define MSGBUF_SIZE 1024


/** \brief  Callback to add current image to the fliplist
 *
 * \param[in]   widget  widget triggering the event (unused)
 * \param[in]   data    drive unit
 *
 * \return  TRUE to indicate the event has been handled
 */
gboolean ui_fliplist_add_current_cb(GtkWidget *widget, gpointer data)
{
    int unit = GPOINTER_TO_INT(data);
    char buffer[MSGBUF_SIZE];

    if (fliplist_add_image(unit)) {

        g_snprintf(buffer, MSGBUF_SIZE, "Fliplist (#%d): added '%s'",
                unit, fliplist_get_head((unsigned int)unit));
        ui_display_statustext(buffer, 10);
    } else {
        /* Display proper error message once we have a decent
         * get_image_filename(unit) function which returns NULL on non-attached
         * images.
         */
        g_snprintf(buffer, MSGBUF_SIZE, "Fliplist (#%d): oops", unit);
        ui_display_statustext(buffer, 10);
    }
    return TRUE;
}


/** \brief  Remove current image from fliplist
 *
 * \param[in]   widget  unused
 * \param[in]   data    unit number
 *
 * \return  TRUE (make sure GLib 'consumes' the key event so it doesn't end up
 *          in the emulated machine
 */
gboolean ui_fliplist_remove_current_cb(GtkWidget *widget, gpointer data)
{
    int unit = GPOINTER_TO_INT(data);
    char buffer[MSGBUF_SIZE];
    const char *image;

    /* get image filename before removing image */
    image = fliplist_get_head((unsigned int)unit);

    if (image != NULL) {
        g_snprintf(buffer, MSGBUF_SIZE, "Fliplist (#%d): Removed '%s'",
                unit, image);
    } else {
        g_snprintf(buffer, MSGBUF_SIZE, "Fliplist (#%d): Nothing to remove",
                unit);
    }

    fliplist_remove(unit, NULL);
    ui_display_statustext(buffer, 10);
    return TRUE;
}


/** \brief  Select next image in the fliplist
 *
 * \param[in]   widget  unused
 * \param[in]   data    unit number
 *
 * \return  TRUE (make sure GLib 'consumes' the key event so it doesn't end up
 *          in the emulated machine
 */
gboolean ui_fliplist_next_cb(GtkWidget *widget, gpointer data)
{
    int unit = GPOINTER_TO_INT(data);
    char buffer[MSGBUF_SIZE];

    if (fliplist_attach_head(unit, 1)) {
        g_snprintf(buffer, MSGBUF_SIZE,
                "Fliplist (#%d): attached next image: '%s'",
                unit, fliplist_get_head((unsigned int)unit));
    } else {
        g_snprintf(buffer, MSGBUF_SIZE,
                "Fliplist (#%d): failed to attach next image",
                unit);
    }
    ui_display_statustext(buffer, 10);
    return TRUE;
}


/** \brief  Select next previous in the fliplist
 *
 * \param[in]   widget  unused
 * \param[in]   data    unit number
 *
 * \return  TRUE (make sure GLib 'consumes' the key event so it doesn't end up
 *          in the emulated machine
 */
gboolean ui_fliplist_prev_cb(GtkWidget *widget, gpointer data)
{
    int unit = GPOINTER_TO_INT(data);
    char buffer[MSGBUF_SIZE];

    if (fliplist_attach_head(unit, 0)) {
        g_snprintf(buffer, MSGBUF_SIZE,
                "Fliplist (#%d): attached previous image: '%s'",
                unit, fliplist_get_head((unsigned int)unit));
    } else {
        g_snprintf(buffer, MSGBUF_SIZE,
                "Fliplist (#%d): failed to attach previous image",
                unit);
    }
    ui_display_statustext(buffer, 10);
    return TRUE;
}


/** \brief  Handler for attaching an image from the fliplist
 *
 * \param[in]   widget  widget triggering the event
 * \param[in]   data    unit number and file index
 */
static void ui_fliplist_select_cb(GtkWidget *widget, gpointer data)
{
    int unit = GPOINTER_TO_INT(data) & 0xff;
    int index = (GPOINTER_TO_INT(data) >> 8) & 0xff;
    int i;
    if (index == 0) {
        fliplist_t list = fliplist_init_iterate(unit);
        const char *image = fliplist_get_image(list);
        file_system_attach_disk(unit, 0, image);
    } else {
        for (i = 0; i < index; ++i) {
            fliplist_attach_head(unit, 1);
        }
    }
}

/** \brief Fill in a menu with controls for fliplist control.
 *
 * Fliplist controls are placed at the end of a menu, after a
 * separator. Any previously-existing fliplist controls within the
 * menu will be removed.
 *
 * \param menu            The menu to be edited.
 * \param unit            The drive unit (8-11) that this menu
 *                        will control.
 * \param separator_count The number of menu separators in the
 *                        part of the menu that does not involve
 *                        the fliplist.
 */
void ui_populate_fliplist_menu(GtkWidget *menu, int unit, int separator_count)
{
    const char *fliplist_string;
    GtkWidget *menu_item;
    GList *children = gtk_container_get_children(GTK_CONTAINER(menu));
    GList *child_iter = g_list_first(children);
    int separators_so_far = 0;
    while (child_iter) {
        if (GTK_IS_SEPARATOR_MENU_ITEM(child_iter->data)) {
            ++separators_so_far;
        }
        if (separators_so_far > separator_count) {
            gtk_container_remove(GTK_CONTAINER(menu), child_iter->data);
        }
        child_iter = child_iter->next;
    }
    g_list_free(children);

    /* Fliplist controls in GTK2/GNOME are next/previous and then the
     * full list of entries within it. For GTK3 we only show these if
     * the fliplist isn't empty for this drive. */
    /* TODO: Add/Remove current image to/from fliplist should really
     * be here too. */
    fliplist_string = fliplist_get_next(unit);
    if (fliplist_string) {
        char buf[128];
        char *basename = NULL;
        fliplist_t fliplist_iterator;
        int index;
        gchar *utf8;

        gtk_container_add(GTK_CONTAINER(menu), gtk_separator_menu_item_new());
        util_fname_split(fliplist_string, NULL, &basename);

        if (basename != NULL) {
            utf8 = file_chooser_convert_from_locale(basename);
            lib_free(basename);
            basename = NULL;
        } else {
            utf8 = file_chooser_convert_from_locale(fliplist_string);
        }

        snprintf(buf, 128, "Next: %s", utf8);
        g_free(utf8);

        buf[127] = 0;
        menu_item = gtk_menu_item_new_with_label(buf);
        g_signal_connect(menu_item, "activate",
                G_CALLBACK(ui_fliplist_next_cb), GINT_TO_POINTER(unit));
        gtk_container_add(GTK_CONTAINER(menu), menu_item);
        fliplist_string = fliplist_get_prev(unit);
        if (fliplist_string) {
            util_fname_split(fliplist_string, NULL, &basename);

            if (basename != NULL) {
                utf8 = file_chooser_convert_from_locale(basename);
                lib_free(basename);
                basename = NULL;
            } else {
                utf8 = file_chooser_convert_from_locale(fliplist_string);
            }

            snprintf(buf, 128, "Previous: %s", utf8);
            g_free(utf8);

            buf[127] = 0;
            menu_item = gtk_menu_item_new_with_label(buf);
            g_signal_connect(menu_item, "activate",
                    G_CALLBACK(ui_fliplist_prev_cb), GINT_TO_POINTER(unit));
            gtk_container_add(GTK_CONTAINER(menu), menu_item);
        }
        gtk_container_add(GTK_CONTAINER(menu), gtk_separator_menu_item_new());
        fliplist_iterator = fliplist_init_iterate(unit);
        index = 0;
        while (fliplist_iterator) {
            fliplist_string = fliplist_get_image(fliplist_iterator);
            util_fname_split(fliplist_string, NULL, &basename);

            if (basename != NULL) {
                utf8 = file_chooser_convert_from_locale(basename);
                lib_free(basename);
                basename = NULL;
            } else {
                utf8 = file_chooser_convert_from_locale(fliplist_string);
            }

            menu_item = gtk_menu_item_new_with_label(utf8);
            g_free(utf8);

            g_signal_connect(menu_item, "activate",
                    G_CALLBACK(ui_fliplist_select_cb),
                    GINT_TO_POINTER(unit+(index << 8)));
            gtk_container_add(GTK_CONTAINER(menu), menu_item);
            fliplist_iterator = fliplist_next_iterate(unit);
            ++index;
        }
    }
}


/** \brief  Handler for the 'response' event of the 'load fliplist' dialog
 *
 * \param[in]   widget      dialog
 * \param[in]   response_id response ID
 * \param[in]   user_data   unit number
 */
static void fliplist_load_response(GtkWidget *widget,
                                   gint response_id,
                                   gpointer user_data)
{
    int unit = GPOINTER_TO_INT(user_data);
    gchar *filename;
    char buffer[MSGBUF_SIZE];

    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(widget));
    if (response_id == GTK_RESPONSE_ACCEPT) {
        /* TODO: read autoattach out of an extra widget. */
        /* TODO: Autoattach looks slightly buggy in fliplist.c */
        fliplist_load_list(unit, filename, 0);
        g_snprintf(buffer, MSGBUF_SIZE, "Fliplist (#%d) loaded: '%s'",
                unit, filename);
        ui_display_statustext(buffer, 10);
    }
    gtk_widget_destroy(widget);
}


/** \brief   Create and show the "load fliplist" dialog.
 *
 *  \param   parent     Widget that sent the event
 *  \param   data       Drive to load fliplist to, or FLIPLIST_ALL_UNITS
 *
 *  \return TRUE
 */
gboolean ui_fliplist_load_callback(GtkWidget *parent, gpointer data)
{
    GtkWidget *dialog;
    unsigned int unit = (unsigned int)GPOINTER_TO_INT(data);
    if (unit != FLIPLIST_ALL_UNITS &&
            (unit < DRIVE_UNIT_MIN || unit > DRIVE_UNIT_MAX)) {
        return TRUE;
    }
    dialog = gtk_file_chooser_dialog_new(
        "Select flip list file",
        ui_get_active_window(),
        GTK_FILE_CHOOSER_ACTION_OPEN,
        /* buttons */
        "Open", GTK_RESPONSE_ACCEPT,
        "Cancel", GTK_RESPONSE_REJECT,
        NULL, NULL);
    /* TODO: add a separate "extra widget" that will let the user
     * select autoattach */
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog),
            create_file_chooser_filter(file_chooser_filter_fliplist, FALSE));
     gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog),
            create_file_chooser_filter(file_chooser_filter_all, FALSE));

    g_signal_connect(dialog, "response", G_CALLBACK(fliplist_load_response), data);
    gtk_widget_show_all(dialog);
    return TRUE;
}


/** \brief  Handler for the 'response' event of the 'save fliplist' dialog
 *
 * \param[in]   widget      dialog
 * \param[in]   response_id response ID
 * \param[in]   user_data   unit number
 */
static void fliplist_save_response(GtkWidget *widget,
                                   gint response_id,
                                   gpointer user_data)
{
    int unit = GPOINTER_TO_INT(user_data);
    gchar *filename;
    char buffer[MSGBUF_SIZE];

    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(widget));
    if (response_id == GTK_RESPONSE_ACCEPT) {
        fliplist_save_list(unit, filename);
        g_snprintf(buffer, MSGBUF_SIZE, "Fliplist (#%d) saved: '%s'",
                unit, filename);
        ui_display_statustext(buffer, 10);

    }
    gtk_widget_destroy(widget);
}


/** \brief   Create and show the "save fliplist" dialog.
 *
 *  \param   parent     Widget that sent the event
 *  \param   data       Drive to save fliplist from, or FLIPLIST_ALL_UNITS
 *
 *  \return TRUE
 */
gboolean ui_fliplist_save_callback(GtkWidget *parent, gpointer data)
{
    GtkWidget *dialog;
    unsigned int unit = (unsigned int)GPOINTER_TO_INT(data);
    if (unit != FLIPLIST_ALL_UNITS &&
            (unit < DRIVE_UNIT_MIN || unit > DRIVE_UNIT_MAX)) {
        return TRUE;
    }
    dialog = gtk_file_chooser_dialog_new(
        "Select flip list file",
        ui_get_active_window(),
        GTK_FILE_CHOOSER_ACTION_SAVE,
        /* buttons */
        "Save", GTK_RESPONSE_ACCEPT,
        "Cancel", GTK_RESPONSE_REJECT,
        NULL, NULL);
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog),
            create_file_chooser_filter(file_chooser_filter_fliplist, FALSE));
    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);
    g_signal_connect(dialog, "response", G_CALLBACK(fliplist_save_response), data);
    gtk_widget_show_all(dialog);

    return TRUE;
}
