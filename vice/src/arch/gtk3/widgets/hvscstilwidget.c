/** \file   hvscstilwidget.c
 * \brief   High Voltage SID Collection STIL widget
 *
 * Widget providing access to the SID Tune Information List, a document
 * containing extra information of SID tunes (author comments, trivia, which
 * songs were covered (if any)).
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

#include <gtk/gtk.h>

#include "resources.h"
#include "vice_gtk3.h"
#include "debug_gtk3.h"
#include "hvsc.h"

#include "hvscstilwidget.h"


/** \brief  GtkTextArea used to present the STIL entry
 */
static GtkWidget *stil_view;


/** \brief  Create TextView widget for the STIL entry text
 *
 * \return  GtkTextView
 */
static GtkWidget *create_view(void)
{
    GtkWidget *textview = gtk_text_view_new();

    gtk_text_view_set_editable(GTK_TEXT_VIEW(textview), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textview), GTK_WRAP_WORD_CHAR);
    return textview;
}



/** \brief  Create HVSC STIL widget
 *
 * \return  GtkGrid
 */
GtkWidget *hvsc_stil_widget_create(void)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *scroll;

    grid = vice_gtk3_grid_new_spaced(VICE_GTK3_DEFAULT, VICE_GTK3_DEFAULT);

    /* add title label */
    label = gtk_label_new(NULL);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_label_set_markup(GTK_LABEL(label), "<b>STIL entry:</b>");

    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

    /* add view */
    stil_view = create_view();
    scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_size_request(scroll, 400, 500);
    gtk_widget_set_hexpand(scroll, TRUE);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
            GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scroll), stil_view);
    gtk_grid_attach(GTK_GRID(grid), scroll, 0, 1, 1, 1);

    /*gtk_widget_set_valign(grid, GTK_ALIGN_START);
    gtk_widget_set_hexpand(grid, TRUE);
    */
    gtk_widget_show_all(grid);
    return grid;
}


int hvsc_stil_widget_set_psid(const char *psid)
{
    hvsc_stil_t stil;
    size_t t;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(stil_view));
    char line[1024];

    debug_gtk3("attempting to load STIL entry for '%s'.", psid);
    if (!hvsc_stil_get(&stil, psid)) {
        debug_gtk3("failed: %d: %s.", hvsc_errno, hvsc_strerror(hvsc_errno));
        /* check hvsc error code to see if either loading the STIL failed or
         * there was no STIL entry for the tune requested */
        if (hvsc_errno == HVSC_ERR_NOT_FOUND) {
            gtk_text_buffer_set_text(buffer, "No STIL entry found.", -1);
        } else {
            gtk_text_buffer_set_text(buffer, "Failed to load STIL.", -1);
        }
        return 0;
    }

    gtk_text_buffer_set_text(buffer, "", -1);

    if (stil.sid_comment != NULL) {
        gtk_text_buffer_set_text(buffer, stil.sid_comment, -1);
    }

    gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer), "\n\n", -1);

    /* now add info on each subtune */
    for (t = 0; t < stil.blocks_used; t++) {
        size_t f;
        hvsc_stil_block_t *block;

        block = stil.blocks[t];

        g_snprintf(line, 1024, "tune #%d:\n", block->tune);
        gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(buffer), line, -1);

        /* handle fields for the current subtune */
        for (f = 0; f < block->fields_used; f++) {
            g_snprintf(line, 1024, "    %s %s\n",
                    hvsc_get_field_display(block->fields[f]->type),
                    block->fields[f]->text);
            gtk_text_buffer_insert_at_cursor(
                    GTK_TEXT_BUFFER(buffer), line, -1);

            /* timestamp? */
            if (block->fields[f]->timestamp.from >= 0) {
                /* yup */
                long from = block->fields[f]->timestamp.from;
                long to = block->fields[f]->timestamp.to;

                if (to < 0) {
                    g_snprintf(line, 1024,
                            "        {timestamp} %ld:%02ld\n",
                            from / 60, from % 60);
                } else {
                    g_snprintf(line, 1024,
                            "        {timestamp) %ld:%02ld-%ld:%02ld\n",
                            from / 60, from % 60, to / 60, to % 60);
                }
                gtk_text_buffer_insert_at_cursor(
                        GTK_TEXT_BUFFER(buffer), line, -1);
            }

            /* album? */
            if (block->fields[f]->album != NULL) {
                g_snprintf(line, 1024, "            {album} %s\n",
                        block->fields[f]->album);
                gtk_text_buffer_insert_at_cursor(
                        GTK_TEXT_BUFFER(buffer), line, - 1);
            }

        }

    }

    hvsc_stil_close(&stil);
    return 1;
}
