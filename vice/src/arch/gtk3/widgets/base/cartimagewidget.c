/** \file   cartimagewidget.c
 * \brief   Widget to control load/save/flush for cart images
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

#include "basedialogs.h"
#include "basewidgets.h"
#include "cartridge.h"
#include "debug_gtk3.h"
#include "lastdir.h"
#include "lib.h"
#include "openfiledialog.h"
#include "resources.h"
#include "resourcewidgetmediator.h"
#include "savefiledialog.h"
#include "ui.h"

#include "cartimagewidget.h"


/** \brief  Cartridge name
 *
 * Used in messages, copied with lib_strdup(), freed with lib_free().
 */
static char *crt_name = NULL;

/** \brief  Cartridge ID
 *
 * Used for various cartridge functions
 */
static int crt_id = 0;  /**< cartridge ID in cartridge_*() calls */

/** \brief  Name of resource containing the cartridge filename
 *
 * Copied with lib_strdup(), freed with lib_free() in the 'destroy' handler.
 */
static char *res_fname;

/** \brief  Name of resource containing the flush-on-write setting
 *
 * Copied with lib_strdup(), freed with lib_free() in the 'destroy' handler.
 */
static char *res_write;

/** \brief  Reference to the filename entry widget */
static GtkWidget *filename_entry;

/** \brief  Last used directory
 */
static char *last_dir = NULL;

/** \brief  Last used file
 */
static char *last_file = NULL;


/** \brief  Handler for the 'destroy' event of the containing grid
 *
 * Clean up copies of resource names and the cart name.
 * The last used directory and filename are cleaned up on emulator shutdown by
 * cart_image_widget_shutdown().
 *
 * \param[in]   widget  widget grid (ignored)
 * \param[in]   data    event data (NULL)
 */
static void on_destroy(GtkWidget *widget, gpointer data)
{
    lib_free(crt_name);
    lib_free(res_fname);
    lib_free(res_write);
    /* set to NULL so the destroy() handler of the secondary image won't attempt
     * to free these if it triggers first. */
    crt_name  = NULL;
    res_fname = NULL;
    res_write = NULL;
}

/** \brief  Callback for the save-dialog
 *
 * \param[in,out]   dialog      save-file dialog
 * \param[in,out]   filename    path to file to save
 * \param[in]       data        extra data (unused)
 */
static void save_filename_callback(GtkDialog *dialog,
                                   gchar     *filename,
                                   gpointer   data)
{
    debug_gtk3("Called with '%s'\n", filename);
    if (filename != NULL) {
        /* write file */
        if (cartridge_save_image(crt_id, filename) < 0) {
            /* oops */
            vice_gtk3_message_error("I/O error", "Failed to save '%s'", filename);
        }
        g_free(filename);
        lastdir_update(GTK_WIDGET(dialog), &last_dir, &last_file);
    }
    gtk_widget_destroy(GTK_WIDGET(dialog));
}

/** \brief  Handler for the 'clicked' event of the "save" button
 *
 * Opens a file chooser to save the cartridge.
 *
 * \param[in]   button      save button
 * \param[in]   user_data   extra event data (unused)
 */
static void on_save_clicked(GtkWidget *button, gpointer user_data)
{
    GtkWidget *dialog;
    char       buffer[256];

    g_snprintf(buffer, sizeof buffer, "Save %s image file", crt_name);
    dialog = vice_gtk3_save_file_dialog(buffer,
                                        NULL,
                                        TRUE,
                                        NULL,
                                        save_filename_callback,
                                        NULL);
    lastdir_set(dialog, &last_dir, &last_file);
    gtk_widget_show(dialog);
}

/** \brief  Handler for the "clicked" event of the "Flush image" button
 *
 * \param[in]   widget      button triggering the event
 * \param[in]   user_data   unused
 */
static void on_flush_clicked(GtkWidget *widget, gpointer user_data)
{
    if (cartridge_flush_image(crt_id) < 0) {
        vice_gtk3_message_error("I/O error", "Failed to flush image");
    }
}

/** \brief  Handler for the 'response' event of the (RAM) image file dialog
 *
 * \param[in]   self        file chooser dialog for the image
 * \param[in]   response_id response ID
 * \param[in]   entry       resource entry
 */
static void on_response(GtkDialog *self, int response_id, gpointer entry)
{
    char *filename;

    switch (response_id) {
        case GTK_RESPONSE_ACCEPT:
            filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(self));
            if (filename != NULL) {
                if (resources_set_string(res_fname, filename) == 0) {
                    gtk_entry_set_text(GTK_ENTRY(entry), filename);
                }
                g_free(filename);
                lastdir_update(GTK_WIDGET(self), &last_dir, &last_file);
            }
            break;
        default:
            break;
    }
    gtk_widget_destroy(GTK_WIDGET(self));
}

/** \brief  Handler for the 'icon-press' event of the filename entry
 *
 * Show a file choose dialog to select or create a file or directory.
 *
 * \param[in]   self        resource file chooser
 * \param[in]   icon_pos    icon position in \a self (ignored)
 * \param[in]   event       event information (ignored)
 * \param[in]   data        extra event data (unused)
 */
static void on_icon_press(GtkEntry             *self,
                          GtkEntryIconPosition  icon_pos,
                          GdkEvent             *event,
                          gpointer              data)
{
    GtkWidget  *dialog;
    GtkWindow  *parent;
    mediator_t *mediator;
    char        title[256];
    const char *current = NULL;

    mediator = mediator_for_widget(GTK_WIDGET(self));
    current  = mediator_get_resource_string(mediator);
    debug_gtk3("Resource %s value: '%s'", mediator_get_name(mediator), current);

    g_snprintf(title,
               sizeof title,
               "Open or create %s RAM image",
               (const char *)crt_name);
    parent = ui_get_active_window();
    dialog = gtk_file_chooser_dialog_new(title,
                                         parent,
                                         GTK_FILE_CHOOSER_ACTION_SAVE,
                                         "Accept", GTK_RESPONSE_ACCEPT,
                                         "Cancel", GTK_RESPONSE_CANCEL,
                                         NULL);
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);
    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), FALSE);
    lastdir_set(dialog, &last_dir, &last_file);

    if (current != NULL && *current != '\0') {
        gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dialog), current);
    }

    g_signal_connect(G_OBJECT(dialog),
                     "response",
                     G_CALLBACK(on_response),
                     (gpointer)self);
    gtk_widget_show(dialog);
}

/** \brief  Create resource entry with clickable icon
 *
 * Create a resource entry box and add an icon to open a file dialog.
 * We can't use the resource filechooser here since we also want to remember
 * the last used dir and file and the resouce filechooser doesn't do that
 * (yet?).
 *
 * \param[in]   resource    resource name
 *
 * \return  GtkEntry
 */
static GtkWidget *create_filename_entry(const char *resource)
{
    GtkWidget *entry;

    entry = vice_gtk3_resource_entry_new(resource);
    gtk_entry_set_icon_from_icon_name(GTK_ENTRY(entry),
                                      GTK_ENTRY_ICON_SECONDARY,
                                      "document-open-symbolic");
    gtk_entry_set_icon_tooltip_markup(GTK_ENTRY(entry),
                                      GTK_ENTRY_ICON_SECONDARY,
                                      "Select image file");
    g_signal_connect(G_OBJECT(entry),
                     "icon-press",
                     G_CALLBACK(on_icon_press),
                     NULL);
    return entry;
}


/** \brief  Create widget to load/save/flush cart image file
 *
 * Create cartridge widget to do basic operations like saving and flushing.
 *
 * If \a title is `NULL` the title will be set to "\<\a cartname\> image".
 *
 * \param[in]   title           widget title (can be NULL)
 * \param[in]   resource_fname  resource for the image file name
 * \param[in]   resource_write  resource controlling flush-on-exit/detach
 * \param[in]   cart_name       cartridge name to use in dialogs
 * \param[in]   cart_id         cartridge ID to use in save/flush callbacks
 *
 * \note    \a cartname and \a cart_id should be taken from cartridge.h
 *
 *
 * \return  GtkGrid
 */
GtkWidget *cart_image_widget_create(const char *title,
                                    const char *resource_fname,
                                    const char *resource_write,
                                    const char *cart_name,
                                    int         cart_id)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *auto_save;
    GtkWidget *box;
    GtkWidget *save_button;
    GtkWidget *flush_button;
    char       buffer[256];
    int        row = 0;

    res_fname = lib_strdup(resource_fname);
    res_write = lib_strdup(resource_write);
    crt_id    = cart_id;
    if (crt_name == NULL) {
        crt_name = lib_strdup(cart_name);
    }

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
    /* this needs to be set otherwise the label takes too much space, as if
     * homogeneous is set to TRUE (which it shouldn't be) */
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), FALSE);

    /* header */
    if (title == NULL) {
        if (cart_name == NULL) {
            strcpy(buffer, "<b>Cartridge Image</b>");
        } else {
            g_snprintf(buffer, sizeof buffer, "<b>%s Image</b>", cart_name);
        }
    } else {
        strncpy(buffer, title, sizeof buffer);
        buffer[sizeof buffer - 1u] = '\0';
    }
    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), buffer);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 3, 1);
    row++;

    label = gtk_label_new("File name:");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    filename_entry = create_filename_entry(resource_fname);
    gtk_widget_set_hexpand(filename_entry, TRUE);
    gtk_grid_attach(GTK_GRID(grid), label,          0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), filename_entry, 1, row, 2, 1);
    row++;

    auto_save = vice_gtk3_resource_check_button_new(resource_write,
            "Write image on image detach/emulator exit");
    gtk_widget_set_valign(auto_save, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), auto_save, 0, row, 2, 1);

    /* wrap save and flush button in button box */
    box = gtk_button_box_new(GTK_ORIENTATION_VERTICAL);
    gtk_box_set_spacing(GTK_BOX(box), 8);
    save_button = gtk_button_new_with_label("Save image as ..");
    gtk_box_pack_start(GTK_BOX(box), save_button, FALSE, FALSE, 0);
    flush_button = gtk_button_new_with_label("Flush image");
    gtk_box_pack_start(GTK_BOX(box), flush_button, FALSE, FALSE, 0);
    gtk_widget_set_halign(box, GTK_ALIGN_END);
    gtk_widget_set_hexpand(box, TRUE);
    gtk_grid_attach(GTK_GRID(grid), box, 1, row, 1, 1);

    gtk_widget_set_sensitive(flush_button,
                             (gboolean)(cartridge_can_flush_image(cart_id)));
    gtk_widget_set_sensitive(save_button,
                             (gboolean)(cartridge_can_save_image(cart_id)));

    g_signal_connect(save_button,
                     "clicked",
                     G_CALLBACK(on_save_clicked),
                     NULL);
    g_signal_connect(flush_button,
                     "clicked",
                     G_CALLBACK(on_flush_clicked),
                     NULL);
    g_signal_connect_unlocked(G_OBJECT(grid),
                              "destroy",
                              G_CALLBACK(on_destroy),
                              NULL);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Clean up resouces used by the cartridge image widget
 */
void cart_image_widget_shutdown(void)
{
    lastdir_shutdown(&last_dir, &last_file);
}




/******************************************************************************
 *                Widget for the secondary image of a cartridge               *
 *                                                                            *
 * Secondary images are currently not contained in .crt files. They have      *
 * wildly varying names such as "EEPROM", "BIOS", "Flash" etc.                *
 * This code is meant to provide a reusable widget for those images.          *
 *****************************************************************************/

/** \brief  Tag for the secondary image (e.g. BIOS, EEPROM, NvRAM)
 *
 * Defaults to "secondary" if `image_tag` in the constructor is `NULL`.
 */
static char *secondary_tag = NULL;

/** \brief  Resource name of the secondary image file */
static char *secondary_fname_res = NULL;

/** \brief  Reference to the GtkGrid containing optional check buttons */
static GtkWidget *checks_grid = NULL;

static int checks_row = 0;


/** \brief  Handler for the 'destroy' event of the secondary image widget
 *
 * \param[in]   self    GtkGrid making up the widget (ignored)
 * \param[in]   data    event data (NULL)
 */
static void on_secondary_destroy(GtkWidget *self, gpointer data)
{
    lib_free(crt_name);
    lib_free(secondary_tag);
    lib_free(secondary_fname_res);
    /* set to NULL so the destroy() handler of the primary image won't attempt
     * to free these if it triggers first. */
    crt_name            = NULL;
    checks_grid         = NULL;
    checks_row          = 0;
    secondary_tag       = NULL;
    secondary_fname_res = NULL;
}

/** \brief  Callback for the save-as dialog
 *
 * Save secondary cartridge image to \a filename.
 * Show error dialog on failure or update last used directory/filename on
 * success.
 *
 * \param[in]   dialog      save-as dialog
 * \param[in]   filename    filename returned by dialog (free with g_free())
 * \param[in]   data        event data (NULL)
 */
static void save_secondary_callback(GtkDialog *dialog,
                                    gchar     *filename,
                                    gpointer   data)
{
    if (filename != NULL) {
        if (cartridge_save_secondary_image(crt_id, filename) < 0) {
            char title[256];

            g_snprintf(title, sizeof title, "%s Error", crt_name);
            vice_gtk3_message_error(title,
                                    "Failed to save %s image as '%s'.",
                                    secondary_tag, filename);
        } else {
            lastdir_update(GTK_WIDGET(dialog), &last_dir, &last_file);
        }
        g_free(filename);
    }
    gtk_widget_destroy(GTK_WIDGET(dialog));
}

/** \brief  Handler for the 'clicked' event of the save-as button
 *
 * Show a file dialog to save the secondary image.
 *
 * \param[in]   button  flush button (ignored)
 * \param[in]   data    even data (NULL)
 */
static void on_secondary_save_clicked(GtkWidget *button, gpointer data)
{
    GtkWidget *dialog;
    char       title[256];

    debug_gtk3("called");
    g_snprintf(title, sizeof title, "Save %s %s image", crt_name, secondary_tag);
    dialog = vice_gtk3_save_file_dialog(title,
                                        NULL,
                                        TRUE,
                                        NULL,
                                        save_secondary_callback,
                                        NULL);
    lastdir_set(GTK_WIDGET(dialog), &last_dir, &last_file);
    gtk_widget_show(dialog);
}

/** \brief  Handler for the 'clicked' event of the flush button
 *
 * Attempt to flush the secondary image file, show error dialog on failure.
 *
 * \param[in]   button  flush button (ignored)
 * \param[in]   data    even data (NULL)
 */
static void on_secondary_flush_clicked(GtkWidget *button, gpointer data)
{
    debug_gtk3("called");
    if (cartridge_flush_secondary_image(crt_id) < 0) {
        char title[256];

        g_snprintf(title, sizeof title, "%s Error", crt_name);
        vice_gtk3_message_error(title,
                                "Failed to flush %s image.",
                                secondary_tag);
    }
}


/*
 * Secondary cart image widget public API
 */

/** \brief  Create widget for secondary images of cartridges
 *
 * Create grid with widgets to set, save and flush secondary cartridge image.
 * Optionally check buttons can be added to the grid using
 * cart_image_secondary_widget_append_check().
 *
 * \param[in]   image_tag   tag to use for the secondary image (can be `NULL`)
 * \param[in]   resource    resource name of the secondary image file
 * \param[in]   cart_name   cartridge name (as defined in `cartridge.h`)
 * \param[in]   cart_id     cartridge ID (as defined in `cartridge.h`)
 *
 * Layout of the grid:
 * <pre>
 * +-------------------------------------------------------------------------+
 * | "{cartname} {image_tag} image" (bold label)
 * +-------------------------------------------------------------------------+
 * | "{image_tag} image file" [ file chooser widget for image file      [=]] |
 * +-------------------------------------------------------------------------+
 * | [v] Optional check buttons                          ["Save image as.."] |
 * | [v] for things like write-on-change                 ["  Flush image  "] |
 * | [v] or write-on-detach/emu-exit                                         |
 * +-------------------------------------------------------------------------+
 * </pre>
 *
 * \return  GtkGrid
 */
GtkWidget *cart_secondary_image_widget_create(const char *image_tag,
                                              const char *resource,
                                              const char *cart_name,
                                              int         cart_id)
{
#define NUM_COLS 3
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *chooser;
    GtkWidget *button_box;
    GtkWidget *save;
    GtkWidget *flush;
    gboolean   can_save;
    gboolean   can_flush;
    char       buffer[256];
    int        row = 0;

    if (image_tag != NULL) {
        secondary_tag = lib_strdup(image_tag);
    } else {
        secondary_tag = lib_strdup("secondary");
    }
    crt_id = cart_id;
    if (crt_name == NULL) {
        crt_name = lib_strdup(cart_name);
    }
    secondary_fname_res = lib_strdup(resource);

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    /* we can use row spacing here since the optional check buttons will be
     * packed into a another grid with no row spacing */
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
#if 0
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), FALSE);
#endif

    /* header */
    g_snprintf(buffer, sizeof buffer,
               "<b>%s %s image</b>",
               cart_name, secondary_tag);
    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), buffer);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, NUM_COLS, 1);
    row++;

    /* filechooser with label */
    g_snprintf(buffer, sizeof buffer, "%s image file", secondary_tag);
    label = gtk_label_new(buffer);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_widget_set_hexpand(label, FALSE);

    g_snprintf(buffer, sizeof buffer,
               "Select %s %s image file",
               cart_name, secondary_tag);
    chooser = vice_gtk3_resource_filechooser_new(secondary_fname_res,
                                                 GTK_FILE_CHOOSER_ACTION_SAVE);
    vice_gtk3_resource_filechooser_set_custom_title(chooser, buffer);
    gtk_widget_set_hexpand(chooser, TRUE);

    gtk_grid_attach(GTK_GRID(grid), label,   0, row, 1,            1);
    gtk_grid_attach(GTK_GRID(grid), chooser, 2, row, NUM_COLS - 1, 1);
    row++;

    /* grid for the optional check buttons */
    checks_grid = gtk_grid_new();
    gtk_widget_set_halign(checks_grid, GTK_ALIGN_START);
    gtk_widget_set_valign(checks_grid, GTK_ALIGN_START);
    checks_row = 0;

    /* buttons to save/flush */
    button_box = gtk_button_box_new(GTK_ORIENTATION_VERTICAL);
    gtk_box_set_spacing(GTK_BOX(button_box), 8);
    gtk_widget_set_hexpand(button_box, TRUE);
    gtk_widget_set_halign(button_box, GTK_ALIGN_END);
    gtk_widget_set_valign(button_box, GTK_ALIGN_START);

    save = gtk_button_new_with_label("Save image as..");
    g_signal_connect(G_OBJECT(save),
                     "clicked",
                     G_CALLBACK(on_secondary_save_clicked),
                     NULL);
    flush = gtk_button_new_with_label("Flush image");
    g_signal_connect(G_OBJECT(flush),
                     "clicked",
                     G_CALLBACK(on_secondary_flush_clicked),
                     NULL);
    gtk_box_pack_start(GTK_BOX(button_box), save,  FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(button_box), flush, FALSE, FALSE, 0);

    can_save  = (gboolean)cartridge_can_save_secondary_image(cart_id);
    can_flush = (gboolean)cartridge_can_flush_secondary_image(cart_id);
    gtk_widget_set_sensitive(save,  can_save);
    gtk_widget_set_sensitive(flush, can_flush);

    gtk_grid_attach(GTK_GRID(grid), checks_grid, 0, row, NUM_COLS - 1, 1);
    gtk_grid_attach(GTK_GRID(grid), button_box,  2, row, 1,            1);

    g_signal_connect_unlocked(G_OBJECT(grid),
                              "destroy",
                              G_CALLBACK(on_secondary_destroy),
                              NULL);
    gtk_widget_show_all(grid);
    return grid;
#undef NUM_COLS
}


/** \brief  Append check button for a resource to the secondary image widget
 *
 * Append a check button for \a resource to the secondary image widget, below
 * the image file chooser and to the left of the "Save" and "Flush" buttons.
 *
 * \param[in]   resource    resource name
 * \param[in]   text        text for the check button
 */
void cart_secondary_image_widget_append_check(const char *resource,
                                              const char *text)
{
    GtkWidget *check;

    check = vice_gtk3_resource_check_button_new(resource, text);
    gtk_grid_attach(GTK_GRID(checks_grid), check, 0, checks_row, 1, 1);
    checks_row++;
}
