/** \file   uimedia.c
 * \brief   Media recording dialog
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
#include <stdint.h>
#include <stdbool.h>

#include "debug_gtk3.h"
#include "lib.h"
#include "resources.h"
#include "machine.h"
#include "widgethelpers.h"
#include "basewidgets.h"
#include "basedialogs.h"
#include "openfiledialog.h"
#include "savefiledialog.h"
#include "selectdirectorydialog.h"
#include "ui.h"
#include "gfxoutput.h"

#include "uimedia.h"


/** \brief  Custom response IDs for the dialog
 */
enum {
    RESPONSE_SAVE = 1   /**< Save button clicked */
};


/** \brief  Struct to hold information on available video drivers
 */
typedef struct video_driver_info_s {
    const char *display;    /**< display string (used in the UI) */
    const char *name;       /**< driver name */
    const char *ext;        /**< default file extension */
} video_driver_info_t;


typedef struct audio_driver_info_s {
    const char *display;
    const char *name;
    const char *ext;
} audio_driver_info_t;


/** \brief  List of available video drivers, gotten during runtime
 */
static video_driver_info_t *video_driver_list = NULL;


/** \brief  Number of available video drivers
 */
static int video_driver_count = 0;


/** \brief  Index of currently selected video driver
 */
static int video_driver_index = 0;


static audio_driver_info_t audio_driver_list[] = {
    { "WAV", "wav", "wav" },
    { "AIFF", "aiff", "aif" },
    { "VOC", "voc", "voc" },
    { "IFF", "iff", "iff" },
#ifdef USE_LAMEMP3
    { "MP3", "mp3", "mp3" },
#endif
#ifdef USE_FLAC
    { "FLAC", "flac", "flac" },
#endif
#ifdef USE_VORBIS
    { "Ogg/Vorbis", "ogg", "ogg" },
#endif
    { NULL, NULL, NULL }
};


static ui_combo_entry_int_t oversize_modes[] = {
    { "scale down", 0 },
    { "crop left top", 1, },
    { "crop center top", 2 },
    { "crop right top", 3 },
    { "crop left center", 4 },
    { "crop center", 5 },
    { "crop right center", 6 },
    { "crop left bottom (huhu)", 7 },
    { "crop center bottom", 8 },
    { "crop right bottom", 9 },
    { NULL, -1 }
};


static ui_combo_entry_int_t undersize_modes[] = {
    { "scale up", 0 },
    { "border size", 1 },
    { NULL, -1 }
};


static ui_combo_entry_int_t multicolor_modes[] = {
    { "B&W", 0 },
    { "2 colors", 1 },
    { "4 colors", 2 },
    { "Grey scale", 3 },
    { "Best cell colors", 4 },
    { NULL, -1 }
};


static ui_combo_entry_int_t ted_luma_modes[] = {
    { "ignore", 0 },
    { "dither", 1 },
    { NULL, -1 },
};


static ui_combo_entry_int_t crtc_colors[] = {
    { "White", 0 },
    { "Amber", 1 },
    { "Green", 2 },
    { NULL, -1 }
};


static int audio_driver_index = 0;


static GtkWidget *screenshot_options_grid = NULL;
static GtkWidget *oversize_widget = NULL;
static GtkWidget *undersize_widget = NULL;
static GtkWidget *multicolor_widget = NULL;
static GtkWidget *ted_luma_widget = NULL;
static GtkWidget *crtc_textcolor_widget = NULL;


static GtkWidget *create_screenshot_param_widget(const char *prefix);


/*****************************************************************************
 *                              Event handlers                               *
 ****************************************************************************/

static void on_dialog_destroy(GtkWidget *widget, gpointer data)
{
    debug_gtk3("called: cleaning up driver list\n");
    lib_free(video_driver_list);
}


/** \brief  Set new widget for the screenshot options, replacing the old one
 *
 * Destroys any widget present before setting the \a new widget.
 *
 * \param[in]   new     new widget
 */
static void update_screenshot_options_grid(GtkWidget *new)
{
    GtkWidget *old = gtk_grid_get_child_at(GTK_GRID(screenshot_options_grid),
            0, 1);
    if (old != NULL) {
        gtk_widget_destroy(old);
    }
    gtk_grid_attach(GTK_GRID(screenshot_options_grid), new, 0, 1, 1, 1);
}


/** \brief  Handler for the "response" event of the \a dialog
 *
 * \param[in,out]   dialog      dialog triggering the event
 * \param[in]       response_id response ID
 * \param[in]       data        extra event data (unused)
 */
static void on_response(GtkDialog *dialog, gint response_id, gpointer data)
{
    debug_gtk3("got response ID %d\n", response_id);

    switch (response_id) {
        case GTK_RESPONSE_DELETE_EVENT:
            debug_gtk3("destroying dialog\n");
            gtk_widget_destroy(GTK_WIDGET(dialog));
            break;
    }
}


/** \brief  Handler for the "toggled" event of the screenshot driver radios
 *
 * \param[in]       widget      radio button riggering the event
 * \param[in]       data        extra event data (unused)
 */
static void on_screenshot_driver_toggled(GtkWidget *widget, gpointer data)
{
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
        int index = GPOINTER_TO_INT(data);
        debug_gtk3("screenshot driver %d (%s) selected\n",
                index, video_driver_list[index].name);
        video_driver_index = index;
        update_screenshot_options_grid(
                create_screenshot_param_widget(video_driver_list[index].name));
    }
}


/** \brief  Handler for the "toggled" event of the audio driver radios
 *
 * \param[in]       widget      radio button riggering the event
 * \param[in]       data        extra event data (unused)
 */
static void on_audio_driver_toggled(GtkWidget *widget, gpointer data)
{
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
        int index = GPOINTER_TO_INT(data);
        debug_gtk3("audio driver %d (%s) selected\n",
                index, audio_driver_list[index].name);
        audio_driver_index = index;
    }
}
/** \brief  Create heap-allocated list of available video drivers
 *
 * Queries the gfxoutputdrv subsystem and builds a list of currently available
 * drivers and stores it in `driver_list`. Must be freed with lib_free() after
 * use.
 */
static void create_video_driver_list(void)
{
    gfxoutputdrv_t *driver;
    int index;

    video_driver_count = gfxoutput_num_drivers();
    debug_gtk3("got %d output drivers\n", video_driver_count);

    video_driver_list = lib_malloc((size_t)(video_driver_count + 1) *
            sizeof *video_driver_list);

    index = 0;
    driver = gfxoutput_drivers_iter_init();
    while (driver != NULL) {
        debug_gtk3(".. adding driver '%s'\n", driver->name);
        video_driver_list[index].display = driver->displayname;
        video_driver_list[index].name = driver->name;
        video_driver_list[index].ext = driver->default_extension;
        index++;
        driver = gfxoutput_drivers_iter_next();
    }
    video_driver_list[index].name = NULL;
    video_driver_list[index].ext = NULL;
}


/** \brief  Determine if driver \a name is a video driver
 *
 * \param[in]   name    driver name
 *
 * \return  bool
 *
 * \todo    There has to be a better, more reliable way than this
 */
static bool driver_is_video(const char *name)
{
    return (bool)(strcmp(name, "FFMPEG") == 0 || strcmp(name, "QuickTime") == 0);
}


/** \brief  Create widget to control screenshot parameters
 *
 * This widget exposes controls to alter screenshot parameters, depending on
 * machine class and output file format
 *
 * \param[in]   prefix  resource prefix (either "Doodle", "Koala", or ""/NULL)
 */
static GtkWidget *create_screenshot_param_widget(const char *prefix)
{
    GtkWidget *grid;
    GtkWidget *label;
    int row;
    bool doodle = false;
    bool koala = false;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    /* according to the standard, doing a strcmp() with one or more `NULL`
     * arguments is implementation-defined, so better safe than sorry */
    if (prefix == NULL) {
        debug_gtk3("some idiot passed NULL\n");
        return grid;
    }

    if ((strcmp(prefix, "DOODLE") == 0)
            || (strcmp(prefix, "DOODLE_COMPRESSED") == 0)) {
        prefix = "Doodle";  /* XXX: not strictly required since resource names
                                    seem to be case insensitive, but better
                                    safe than sorry */
        doodle = true;
    } else if ((strcmp(prefix, "KOALA") == 0)
            || (strcmp(prefix, "KOALA_COMPRESSED") == 0)) {
        prefix = "Koala";
        koala = true;
    }

    if (!koala && !doodle) {
        label = gtk_label_new("No parameters required");
        g_object_set(label, "margin-left", 16, NULL);
        gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);
        gtk_widget_show_all(grid);
        return grid;
    }

    /* ${FORMAT}OversizeHandling */
    label = gtk_label_new("Oversize handling");
    g_object_set(label, "margin-left", 16, NULL);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    oversize_widget = resource_combo_box_int_create_sprintf(
            "%sOversizeHandling", oversize_modes, prefix);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), oversize_widget, 1, 0, 1, 1);

    /* ${FORMAT}UndersizeHandling */
    label = gtk_label_new("Undersize handling");
    g_object_set(label, "margin-left", 16, NULL);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    undersize_widget = resource_combo_box_int_create_sprintf(
            "%sUndersizeHandling", undersize_modes, prefix);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), undersize_widget, 1, 1, 1, 1);

    /* ${FORMAT}MultiColorHandling */

    row = 2;    /* from now on, the widgets depend on machine and image type */

    /* DoodleMultiColorHandling */
    if (doodle) {
        label = gtk_label_new("Multi color handling");
        g_object_set(label, "margin-left", 16, NULL);
        gtk_widget_set_halign(label, GTK_ALIGN_START);
        multicolor_widget = resource_combo_box_int_create_sprintf(
                "%sMultiColorHandling", multicolor_modes, prefix);
        gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), multicolor_widget, 1, row, 1, 1);
        row++;
    }

    /* ${FORMAT}TEDLumaHandling */
    if (machine_class == VICE_MACHINE_PLUS4) {
        label = gtk_label_new("TED luma handling");
        g_object_set(label, "margin-left", 16, NULL);
        gtk_widget_set_halign(label, GTK_ALIGN_START);
        ted_luma_widget = resource_combo_box_int_create_sprintf(
                "%sTEDLumHandling", ted_luma_modes, prefix);
        gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), ted_luma_widget, 1, row, 1, 1);
        row++;
    }

    /* ${FORMAT}CRTCTextColor */
    if (machine_class == VICE_MACHINE_PET
            || machine_class == VICE_MACHINE_CBM6x0) {
        label = gtk_label_new("CRTC text color");
        g_object_set(label, "margin-left", 16, NULL);
        gtk_widget_set_halign(label, GTK_ALIGN_START);
        crtc_textcolor_widget = resource_combo_box_int_create_sprintf(
                "%sCRTCTextColor", crtc_colors, prefix);
        gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), crtc_textcolor_widget, 1, row, 1, 1);
        row++;
    }

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create the main 'screenshot' widget
 *
 * \return  GtkGrid
 */
static GtkWidget *create_screenshot_widget(void)
{
    GtkWidget *grid;
    GtkWidget *drv_grid;
    GtkWidget *radio;
    GtkWidget *last;
    GSList *group = NULL;
    int index;
    int grid_index;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    drv_grid = uihelpers_create_grid_with_label("Driver", 1);
    grid_index = 1;
    last = NULL;
    for (index = 0; video_driver_list[index].name != NULL; index++) {
        const char *display = video_driver_list[index].display;
        const char *name = video_driver_list[index].name;

        if (!driver_is_video(name)) {
            radio = gtk_radio_button_new_with_label(group, display);
            g_object_set(radio, "margin-left", 8, NULL);
            gtk_radio_button_join_group(GTK_RADIO_BUTTON(radio),
                    GTK_RADIO_BUTTON(last));
            gtk_grid_attach(GTK_GRID(drv_grid), radio, 0, grid_index, 1, 1);

            g_signal_connect(radio, "toggled",
                    G_CALLBACK(on_screenshot_driver_toggled),
                    GINT_TO_POINTER(index));

            last = radio;
            grid_index++;
        }
    }

    /* this is where the various options go per screenshot driver (for example
     * Koala or Doodle) */
    screenshot_options_grid = uihelpers_create_grid_with_label(
            "Driver options", 1);

    gtk_grid_attach(GTK_GRID(grid), drv_grid, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), screenshot_options_grid, 1, 0, 1, 1);

    update_screenshot_options_grid(create_screenshot_param_widget(""));

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create the main 'sound recording' widget
 *
 * \return  GtkGrid
 */
static GtkWidget *create_sound_widget(void)
{
    GtkWidget *grid;
    GtkWidget *drv_grid;
    GtkWidget *radio;
    GtkWidget *last;
    GSList *group = NULL;
    int index;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    drv_grid = uihelpers_create_grid_with_label("Driver", 1);
    last = NULL;
    for (index = 0; audio_driver_list[index].name != NULL; index++) {
        const char *display = audio_driver_list[index].display;

        radio = gtk_radio_button_new_with_label(group, display);
        g_object_set(radio, "margin-left", 8, NULL);
        gtk_radio_button_join_group(GTK_RADIO_BUTTON(radio),
                GTK_RADIO_BUTTON(last));
        gtk_grid_attach(GTK_GRID(drv_grid), radio, 0, index + 1, 1, 1);

        g_signal_connect(radio, "toggled",
                G_CALLBACK(on_audio_driver_toggled),
                GINT_TO_POINTER(index));

        last = radio;
    }

    gtk_grid_attach(GTK_GRID(grid), drv_grid, 0, 0, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create the main 'video recording' widget
 *
 * \return  GtkGrid
 */
static GtkWidget *create_video_widget(void)
{
    GtkWidget *grid;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create the content widget for the dialog
 *
 * Contains a GtkStack and GtkStackSwitcher to switch between 'screenshot',
 * 'sound' and 'video' sub widgets.
 *
 * \return  GtkGrid
 */
static GtkWidget *create_content_widget(void)
{
    GtkWidget *grid;
    GtkWidget *stack;
    GtkWidget *switcher;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    stack = gtk_stack_new();
    gtk_stack_add_titled(GTK_STACK(stack), create_screenshot_widget(),
            "Screenshot", "Screenshot");
    gtk_stack_add_titled(GTK_STACK(stack), create_sound_widget(),
            "Sound", "Sound recording");
    gtk_stack_add_titled(GTK_STACK(stack), create_video_widget(),
            "Video", "Video recording");
    gtk_stack_set_transition_type(GTK_STACK(stack),
            GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    gtk_stack_set_transition_duration(GTK_STACK(stack), 1000);
    /* avoid resizing the dialog when switching */
    gtk_stack_set_homogeneous(GTK_STACK(stack), TRUE);

    switcher = gtk_stack_switcher_new();
    gtk_widget_set_halign(switcher, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(switcher, TRUE);
    gtk_stack_switcher_set_stack(GTK_STACK_SWITCHER(switcher), GTK_STACK(stack));
    /* make all titles of the switcher the same size */
    gtk_box_set_homogeneous(GTK_BOX(switcher), TRUE);

    gtk_widget_show_all(stack);
    gtk_widget_show_all(switcher);

    gtk_grid_attach(GTK_GRID(grid), switcher, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), stack, 0, 1, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}



/** \brief  Show dialog to save screenshot, record sound and/or video
 *
 * \param[in]   parent  parent widget (unused)
 * \param[in]   data    extra data (unused)
 */
void uimedia_dialog_show(GtkWidget *parent, gpointer data)
{
    GtkWidget *dialog;
    GtkWidget *content;

    /* create driver list */
    create_video_driver_list();

    dialog = gtk_dialog_new_with_buttons(
            "Record media file",
            ui_get_active_window(),
            GTK_DIALOG_MODAL,
            "Save", RESPONSE_SAVE,
            "Close", GTK_RESPONSE_DELETE_EVENT,
            NULL);

    /* add content widget */
    content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    gtk_container_add(GTK_CONTAINER(content), create_content_widget());

    gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);
    g_signal_connect(dialog, "response", G_CALLBACK(on_response), NULL);
    g_signal_connect(dialog, "destroy", G_CALLBACK(on_dialog_destroy), NULL);

    gtk_widget_show_all(dialog);
}
