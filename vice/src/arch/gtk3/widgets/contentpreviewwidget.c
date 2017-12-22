/*
 * contentpreviewwidget.c - GTK3 disk/tape/archive preview widget
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
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
 */


#include "vice.h"

#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

#include "debug_gtk3.h"
#include "lib.h"
#include "imagecontents.h"
#include "imagecontents/diskcontents.h"

#include "contentpreviewwidget.h"


static GtkWidget *content_view = NULL;


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


static GtkListStore *create_model(const char *path)
{
    GtkListStore *model;
    GtkTreeIter iter;
    image_contents_t *contents;
    image_contents_file_list_t *entry;
    char *tmp;
    char *utf8;
    int row;

    model = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_INT);
    if (path == NULL) {
        return model;
    }

    contents = diskcontents_filesystem_read(path);
    if (contents == NULL) {
        debug_gtk3("not a disk image\n");
        return model;
    }

    row = 0;

    /* disk name & ID */
    tmp = image_contents_to_string(contents, 0);
    utf8 = (char *)convert_utf8((unsigned char *)tmp);
    gtk_list_store_append(model, &iter);
    gtk_list_store_set(model, &iter, 0, utf8, 1, row, -1);
    row++;
    lib_free(tmp);
    lib_free(utf8);

    /* files, if any */
    for (entry = contents->file_list; entry != NULL; entry = entry->next) {
        tmp = image_contents_file_to_string(entry, 0);
        utf8 = (char *)convert_utf8((unsigned char *)tmp);
        gtk_list_store_append(model, &iter);
        gtk_list_store_set(model, &iter, 0, utf8, 1, row, -1);
        row++;
        lib_free(tmp);
        lib_free(utf8);
    }

    /* blocks free */
    tmp = lib_msprintf("%d BLOCKS FREE.", contents->blocks_free);
    utf8 = (char *)convert_utf8((unsigned char *)tmp);
    gtk_list_store_append(model, &iter);
    gtk_list_store_set(model, &iter, 0, utf8, 1, row, -1);
    lib_free(tmp);
    lib_free(utf8);

    image_contents_destroy(contents);
    return model;
}


static GtkWidget *create_view(const char *path)
{
    GtkTreeView *view;
    GtkTreeViewColumn *column;
    GtkListStore *model;
    GtkCellRenderer *renderer;

    model = create_model(path);

    view = GTK_TREE_VIEW(gtk_tree_view_new_with_model(GTK_TREE_MODEL(model)));
    g_object_unref(model);
    renderer = gtk_cell_renderer_text_new();
    g_object_set(renderer, "font", "CBM 10", NULL);
    column = gtk_tree_view_column_new_with_attributes("Contents", renderer,
            "text", 0, NULL);
    gtk_tree_view_append_column(view, column);

    gtk_tree_view_set_headers_clickable(view, FALSE);
    gtk_tree_view_set_headers_visible(view, FALSE);
    gtk_tree_selection_set_mode(gtk_tree_view_get_selection(view),
            GTK_SELECTION_SINGLE);

    gtk_widget_set_vexpand(GTK_WIDGET(view), TRUE);

    return GTK_WIDGET(view);
}






/** \brief  Create widget to show image contents in a file dialog
 *
 * \param[in]   selected    callback used when the user double-clicked a file
 *
 * \return  GtkGrid
 */
GtkWidget *content_preview_widget_create(void (*selected)(GtkWidget *, void *))
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *scroll;

    grid = gtk_grid_new();
    gtk_widget_set_hexpand(grid, TRUE);

    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), "<b>Image contents</b>");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

    scroll = gtk_scrolled_window_new(NULL, NULL);
    content_view = create_view(NULL);
    gtk_container_add(GTK_CONTAINER(scroll), content_view);

    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
            GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_set_focus_vadjustment(GTK_CONTAINER(content_view),
            gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW (scroll)));
    gtk_container_set_focus_hadjustment(GTK_CONTAINER(content_view),
            gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW (scroll)));
    gtk_grid_attach(GTK_GRID(grid), scroll, 0, 1, 1, 1);
    gtk_widget_set_size_request(scroll, 420, -1);
    gtk_widget_set_vexpand(scroll, TRUE);


    gtk_widget_show_all(grid);
    return grid;
}


void content_preview_widget_set_image(GtkWidget *widget, const char *path)
{
    GtkListStore *model;

    model = create_model(path);
    gtk_tree_view_set_model(GTK_TREE_VIEW(content_view), GTK_TREE_MODEL(model));
    g_object_unref(model);
}
