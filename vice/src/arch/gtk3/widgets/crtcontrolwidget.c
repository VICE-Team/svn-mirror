/** \file   crtcontrolwidget.c
 * \brief   GTK3 CRT settings widget
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Provides a list of GtkScale widgets to alter CRT settings.
 *
 * Supported settings per chip/video mode:
 *
 * |Setting             |VICIIP|VICIIN|VDC|VICP|VICN|TEDP|TEDN|CRTC|
 * |--------------------|------|------|---|----|----|----|----|----|
 * |Brightness          | yes  | yes  |yes|yes |yes |yes |yes |yes |
 * |Contrast            | yes  | yes  |yes|yes |yes |yes |yes |yes |
 * |Saturation          | yes  | yes  |yes|yes |yes |yes |yes |yes |
 * |Tint                | yes  | yes  |yes|yes |yes |yes |yes |yes |
 * |Gamma               | yes  | yes  |yes|yes |yes |yes |yes |yes |
 * |Blur                | yes  | yes  |yes|yes |yes |yes |yes |yes |
 * |Scanline shade      | yes  | yes  |yes|yes |yes |yes |yes |yes |
 * |Odd lines phase     | yes  |  no  | no|yes | no |yes | no | no |
 * |Odd lines offset    | yes  |  no  | no|yes | no |yes | no | no |
 *
 * TODO:    Fix display of sliders when switching between PAL and NTSC
 */

/*
 * $VICERES CrtcColorBrightness     xpet xcbm2
 * $VICERES CrtcColorContrast       xpet xcbm2
 * $VICERES CrtcColorGamma          xpet xcbm2
 * $VICERES CrtcColorSaturation     xpet xcbm2
 * $VICERES CrtcColorTint           xpet xcbm2
 * $VICERES CrtcPALBlur             xpet xcbm2
 * $VICERES CrtcPALScanLineShade    xpet xcbm2
 *
 * $VICERES TEDColorBrightness      xplus4
 * $VICERES TEDColorContrast        xplus4
 * $VICERES TEDColorGamma           xplus4
 * $VICERES TEDColorSaturation      xplus4
 * $VICERES TEDColorTint            xplus4
 * $VICERES TEDPALBlur              xplus4
 * $VICERES TEDPALOddLineOffset     xplus4
 * $VICERES TEDPALOddLinePhase      xplus4
 * $VICERES TEDPALScanLineShade     xplus4
 *
 * $VICERES VDCColorBrightness      x128
 * $VICERES VDCColorContrast        x128
 * $VICERES VDCColorGamma           x128
 * $VICERES VDCColorSaturation      x128
 * $VICERES VDCColorTint            x128
 * $VICERES VDCPALBlur              x128
 * $VICERES VDCPALScanLineShade     x128
 *
 * $VICERES VICColorBrightness      xvic
 * $VICERES VICColorContrast        xvic
 * $VICERES VICColorGamma           xvic
 * $VICERES VICColorSaturation      xvic
 * $VICERES VICColorTint            xvic
 * $VICERES VICPALBlur              xvic
 * $VICERES VICPALOddLineOffset     xvic
 * $VICERES VICPALOddLinePhase      xvic
 * $VICERES VICPALScanLineShade     xvic
 *
 * $VICERES VICIIColorBrightness    x64 x64sc x64dtv xscpu64 x128 xcbm5x0
 * $VICERES VICIIColorContrast      x64 x64sc x64dtv xscpu64 x128 xcbm5x0
 * $VICERES VICIIColorGamma         x64 x64sc x64dtv xscpu64 x128 xcbm5x0
 * $VICERES VICIIColorSaturation    x64 x64sc x64dtv xscpu64 x128 xcbm5x0
 * $VICERES VICIIColorTint          x64 x64sc x64dtv xscpu64 x128 xcbm5x0
 * $VICERES VICIIPALBlur            x64 x64sc x64dtv xscpu64 x128 xcbm5x0
 * $VICERES VICIIPALOddLineOffset   x64 x64sc x64dtv xscpu64 x128 xcbm5x0
 * $VICERES VICIIPALOddLinePhase    x64 x64sc x64dtv xscpu64 x128 xcbm5x0
 * $VICERES VICIIPALScanLineShade   x64 x64sc x64dtv xscpu64 x128 xcbm5x0
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
#include <string.h>

#include "lib.h"
#include "log.h"
#include "machine.h"
#include "resources.h"
#include "vice_gtk3.h"

#include "crtcontrolwidget.h"


/** \brief  CSS for the scales
 *
 * This makes the sliders take up less vertical space. The margin can be set
 * to a negative value (in px) to allow the slider to be larger than the scale
 * itself.
 *
 * Probably will require some testing/tweaking to get this to look acceptable
 * with various themes (and OSes).
 */
#define SLIDER_CSS \
    "scale slider {\n" \
    "  min-width: 10px;\n" \
    "  min-height: 10px;\n" \
    "  margin: -3px;\n" \
    "}\n\n" \
    "scale {\n" \
    "  margin-top: -8px;\n" \
    "  margin-bottom: -8px;\n" \
    "}"


/** \brief  CSS for the labels
 *
 * Make font smaller and reduce the vertical size the labels use
 *
 * Here Be Dragons!
 */
#define LABEL_CSS \
    "label {\n" \
    "  font-size: 80%;\n" \
    "  margin-top: -2px;\n" \
    "  margin-bottom: -2px;\n" \
    "}"


/** \brief  Number of valid resources for PAL */
#define RESOURCE_COUNT_PAL  9

/** \brief  Number of valid resources for NTSC */
#define RESOURCE_COUNT_NTSC 5

/** \brief  Size of array required for all resources */
#define RESOURCE_COUNT_MAX  RESOURCE_COUNT_PAL


/** \brief  Video chip identifiers
 *
 * Allows for easier/faster switching than using strcmp()
 */
enum {
    CHIP_CRTC,      /**< CRTC videochip (CBM-II 6x0/7x0, PET) */
    CHIP_TED,       /**< TED videochip (Plus4, C16) */
    CHIP_VDC,       /**< VDC videochip (C128) */
    CHIP_VIC,       /**< VIC videochip (VIC20) */
    CHIP_VICII,     /**< VIC-II videochip (C64, C128, CBM-II 5x0) */

    CHIP_ID_COUNT   /**< Number of CHIP ID's */
};

/** \brief  CRT resource info
 */
typedef struct crt_control_resource_s {
    const char *label;  /**< Displayed name (label) */
    const char *name;   /**< Resource name excluding CHIP prefix */
    int low;            /**< lowest value for resoource */
    int high;           /**< highest value for resource */
    int step;           /**< stepping for the spin button */
} crt_control_resource_t;


/** \brief  CRT resource object
 */
typedef struct crt_control_s {
    crt_control_resource_t res;     /**< resource info */
    GtkWidget *scale;               /**< GtkScale reference */
    GtkWidget *spin;                /**< GtkSpinButton reference */
} crt_control_t;


/** \brief  Object holding internal state of a CRT control widget
 *
 * Since we can have two video chips (C128's VICII+VDC), we cannot use static
 * references to widgets and need to allocate memory for the references and
 * clean that memory up once the widget is destroyed.
 */
typedef struct crt_control_data_s {
    char *chip;                                 /**< video chip name */
    crt_control_t controls[RESOURCE_COUNT_MAX]; /**< list of controls */
} crt_control_data_t;


/** \brief  List of CRT emulation resources
 */
static const crt_control_resource_t resource_table[RESOURCE_COUNT_MAX] = {
    { "Brightness",     "ColorBrightness",  0, 2000, 100 },
    { "Contrast",       "ColorContrast",    0, 2000, 100 },
    { "Saturation",     "ColorSaturation",  0, 2000, 100 },
    { "Tint",           "ColorTint",        0, 2000, 100 },
    { "Gamma",          "ColorGamma",       0, 4000, 200 },
    { "Blur",           "PALBlur",          0, 1000, 50 },
    { "Scanline shade", "PALScanLineShade", 0, 1000, 50 },
    { "Oddline phase",  "PALOddLinePhase",  0, 2000, 100 },
    { "Oddline offset", "PALOddLineOffset", 0, 2000, 100 }
};


/** \brief  Struct mapping chip names to chip IDs
 */
typedef struct chip_id_s {
    const char *name;   /**< chip name (ie VDC, VICII, etc) */
    int id;             /**< chip ID */
} chip_id_t;


/** \brief  Enum mapping CHIP prefixes to CHIP ID's
 */
static const chip_id_t chips[CHIP_ID_COUNT] = {
    { "CRTC",   CHIP_CRTC },
    { "TED",    CHIP_TED },
    { "VDC",    CHIP_VDC },
    { "VIC",    CHIP_VIC },
    { "VICII",  CHIP_VICII }
};


/** \brief  CSS provider for labels
 */
static GtkCssProvider *label_css_provider;

/** \brief  CSS provider for scales
 */
static GtkCssProvider *scale_css_provider;


/** \brief  Find chip ID by \a name
 *
 * \param[in]   name    chip name
 *
 * \return  chip ID or -1 on error
 */
static int get_chip_id(const char *name)
{
    int i;

    for (i = 0; i < CHIP_ID_COUNT; i++) {
        if (strcmp(name, chips[i].name) == 0) {
            return chips[i].id;
        }
    }
    return -1;
}


/** \brief  Reset all sliders to their factory value
 *
 * \param[in]   widget      reset button
 * \param[in]   user_data   extra event data (unused)
 */
static void on_reset_clicked(GtkWidget *widget, gpointer user_data)
{
    GtkWidget *parent;
    crt_control_data_t *data;
    int i;

    parent = gtk_widget_get_parent(widget);
    data = g_object_get_data(G_OBJECT(parent), "InternalState");
#if 0
    debug_gtk3("CHIP: %s.", data->chip);
#endif
    for (i = 0; i < RESOURCE_COUNT_MAX; i++) {
        crt_control_t control = data->controls[i];

        if (control.scale != NULL) {
#if 0
            debug_gtk3("Resetting '%s' to factory value.", control.res.label);
#endif
            vice_gtk3_resource_scale_int_factory(control.scale);
            /* No need to reset the spin button, that gets triggered via
             * the scale widget
             */
        }
    }
}


/** \brief  Clean up memory used by the internal state of \a widget
 *
 * \param[in]   widget      widget
 * \param[in]   user_data   extra event data (unused)
 */
static void on_widget_destroy(GtkWidget *widget, gpointer user_data)
{
    crt_control_data_t *data;

    data = (crt_control_data_t *)(g_object_get_data(
                G_OBJECT(widget), "InternalState"));
    lib_free(data->chip);
    lib_free(data);
}


/** \brief  Handler for the 'value-changed' event of the spin buttons
 *
 * Updates \a scale with the value of \a spin.
 *
 * \param[in]       spin    spin button
 * \param[in,out]   scale   scale widget
 */
static void on_spin_value_changed(GtkWidget *spin, gpointer scale)
{
    gdouble spinval;

    spinval = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spin));
    gtk_range_set_value(GTK_RANGE(scale), (int)spinval);
}


/** \brief  Handler for the 'value-changed' event of the scale widgets
 *
 * Updates \a spin with the value of \a scale
 *
 * \param[in]       scale   scale widget
 * \param[in,out]   spin    spin button
 */
static void on_scale_value_changed(GtkWidget *scale, gpointer spin)
{
    int scaleval;

    scaleval = gtk_range_get_value(GTK_RANGE(scale));
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin), (double)scaleval);
}



/** \brief  Create right-aligned label with a smaller font
 *
 * \param[in]   text    label text
 * \param[in]   minimal reduce label to minimum size
 *
 * \return  GtkLabel
 */
static GtkWidget *create_label(const char *text, gboolean minimal)
{
    GtkWidget *label;

    label = gtk_label_new(text);
    gtk_widget_set_halign(label, GTK_ALIGN_END);

    if (minimal) {
        vice_gtk3_css_provider_add(label, label_css_provider);
    }

    return label;
}


/** \brief  Create a customized GtkScale for \a resource
 *
 * \param[in]   resource    resource name without the video \a chip name prefix
 * \param[in]   chip        video chip name
 * \param[in]   low         lower bound
 * \param[in]   high        upper bound
 * \param[in]   step        step used to increase/decrease slider value
 * \param[in]   minimal     reduced size (for the statusbar widget)
 *
 * \return  GtkScale
 */
static GtkWidget *create_slider(
        const char *resource, const char *chip,
        int low, int high, int step,
        gboolean minimal)
{
    GtkWidget *scale;

    /* Set stepping to 1, for now, to allow more finegrained control of the
     * sliders. This only works in the settings menu, not in the popup CRT
     * controls since the keyboard is captured for the running emulator.
     */
    step = 1;

    scale = vice_gtk3_resource_scale_int_new_sprintf("%s%s",
            GTK_ORIENTATION_HORIZONTAL, low, high, step,
            chip, resource);
    gtk_widget_set_hexpand(scale, TRUE);
    gtk_scale_set_value_pos(GTK_SCALE(scale), GTK_POS_RIGHT);
    /* Disable tickmarks. This looks nice and could help to quickly set a value,
     * but in reality it screws with the user's mouse control over the sliders
     */
#if 0
    vice_gtk3_resource_scale_int_set_marks(scale, step);
#endif

    /* set up custom CSS to make the scale take up less space */
    if (minimal) {
        vice_gtk3_css_provider_add(scale, scale_css_provider);
    }

#if 0
    /* don't draw the value next to the scale if used a statusbar popup */
    gtk_scale_set_draw_value(GTK_SCALE(scale), !minimal);
#endif
    return scale;
}


/** \brief  Create spin button for \a resource of \a chip
 *
 * \param[in]   resource    resource name, excluding \a chip
 * \param[in]   chip        video chip name
 * \param[in]   low         spinbox lowest value
 * \param[in]   high        spinbox highest value
 * \param[in]   step        spinbox stepping
 * \param[in]   minimal     reduced size (for the statusbar widget)
 *
 * \return  spinbox widget
 */
static GtkWidget *create_spin(
        const char *resource, const char *chip,
        int low, int high, int step,
        gboolean minimal)
{
    GtkWidget *spin;

    spin = vice_gtk3_resource_spin_int_new_sprintf(
            "%s%s",
            low, high, step,
            chip, resource);
    return spin;
}


/** \brief  Add GtkScale sliders to \a grid
 *
 * \param[in,out]   grid    grid to add widgets to
 * \param[in,out]   data    internal data of the main widget
 * \param[in]       minimal minimize size of the slider, no spinboxes
 *
 * \return  row number of last widget added
 */
static void add_sliders(GtkGrid *grid,
                        crt_control_data_t *data,
                        gboolean minimal)
{
    GtkWidget *label;
    const char *chip;
    int video_standard;
    int row = 1;
    int chip_id;
    size_t i;

    chip = data->chip;
    chip_id = get_chip_id(chip);
    if (chip_id < 0) {
        log_error(LOG_ERR, "failed to get chip ID for '%s'.", chip);
        return;
    }

    /* get PAL/NTSC mode */
    if (resources_get_int("MachineVideoStandard", &video_standard) < 0) {
        log_error(LOG_ERR, "failed to get 'MachineVideoStandard' resource value.");
        return;
    }

    if (!minimal) {
        for (i = 0; i < RESOURCE_COUNT_MAX; i ++) {
            crt_control_t *control = &(data->controls[i]);
            label = create_label(control->res.label, minimal);
            gtk_grid_attach(grid, label, 0, row, 1, 1);
            control->scale = create_slider(control->res.name, chip,
                    control->res.low, control->res.high, control->res.step,
                    minimal);
            gtk_grid_attach(grid, control->scale, 1, row, 1, 1);

            control->spin = create_spin(control->res.name, chip,
                    control->res.low, control->res.high, control->res.step,
                    minimal);
            gtk_grid_attach(grid, control->spin, 2, row, 1, 1);
            /* hook up signal handlers */
            g_signal_connect(control->scale, "value-changed",
                    G_CALLBACK(on_scale_value_changed),
                    (gpointer)(control->spin));
            g_signal_connect(control->spin, "value-changed",
                    G_CALLBACK(on_spin_value_changed),
                    (gpointer)(control->scale));
            row++;
        }
    } else {
        /* Add sliders for statusbar popup */
        for (i = 0; i < RESOURCE_COUNT_MAX; i ++) {
            int col = (i % 2) * 2;
            crt_control_t *control = &(data->controls[i]);
            label = create_label(control->res.label, minimal);
            gtk_grid_attach(grid, label, col + 0, row, 1, 1);
            control->scale = create_slider(control->res.name, chip,
                    control->res.low, control->res.high, control->res.step,
                    minimal);
            gtk_grid_attach(grid, control->scale, col + 1, row, 1, 1);
            if (col > 0) {
                row++;
            }
        }
    }

    /* TODO: make this work again
     *
     * What the hell am I doing here?
     * --compyx, 2020-06-14
     */
    int is_pal = ((video_standard == 0 /* PAL */
                || video_standard == 1 /* Old PAL */
                || video_standard == 4 /* PAL-N/Drean */
                ) && chip_id != CHIP_CRTC && chip_id != CHIP_VDC);

    if (!is_pal) {
        for (i = 0; i < RESOURCE_COUNT_MAX; i ++) {
            crt_control_t *control = &(data->controls[i]);
            int is_ntsc = strncmp(control->res.name, "PAL", 3) != 0;

            if (control->scale != NULL) {
                gtk_widget_set_sensitive(control->scale, is_ntsc);
                if (control->spin != NULL) {
                    gtk_widget_set_sensitive(control->spin, is_ntsc);
                }
            }
        }
    }
}


/** \brief  Create heap-allocated CRT controls state object
 *
 * We need this since we share this code with the CRT controls available via
 * the statusbar.
 *
 * \param[in]   chip    video chip name
 *
 * \return  heap-allocated CRT controls state object
 */
static crt_control_data_t *create_control_data(const char *chip)
{
    crt_control_data_t *data = lib_malloc(sizeof *data);
    size_t i;

    data->chip = lib_strdup(chip);
    for (i = 0; i < RESOURCE_COUNT_MAX; i++) {
        crt_control_t *control = &(data->controls[i]);
        control->res.label = resource_table[i].label;
        control->res.name = resource_table[i].name;
        control->res.low = resource_table[i].low;
        control->res.high = resource_table[i].high;
        control->res.step = resource_table[i].step;
        control->scale = NULL;
        control->spin = NULL;
    }
    return data;
}


/** \brief  Create CRT control widget for \a chip
 *
 * \param[in]   parent  parent widget
 * \param[in]   chip    video chip name
 * \param[in]   minimal reduce slider sizes to be used attached to the status
 *              bar
 *
 * \return  GtkGrid
 */
GtkWidget *crt_control_widget_create(GtkWidget *parent,
                                     const char *chip,
                                     gboolean minimal)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *button;
    gchar buffer[256];
    crt_control_data_t *data;

    /* create reusable CSS providers */
    label_css_provider = vice_gtk3_css_provider_new(LABEL_CSS);
    if (label_css_provider == NULL) {
        return NULL;
    }
    scale_css_provider = vice_gtk3_css_provider_new(SLIDER_CSS);
    if (scale_css_provider == NULL) {
        return NULL;
    }

    data = create_control_data(chip);

    grid = vice_gtk3_grid_new_spaced(16, 0);
    /* g_object_set(grid, "font-size", 9, NULL); */
    g_object_set(grid, "margin-left", 8, "margin-right", 8, NULL);

    if (minimal) {
        g_snprintf(buffer, 256, "<small><b>CRT settings (%s)</b></small>", chip);
    } else {
        g_snprintf(buffer, 256, "<b>CRT settings (%s)</b>", chip);
    }
    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), buffer);
    gtk_widget_set_halign(label, GTK_ALIGN_CENTER);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

    add_sliders(GTK_GRID(grid), data, minimal);

    button = gtk_button_new_with_label("Reset");
    gtk_widget_set_halign(button, GTK_ALIGN_END);
    gtk_grid_attach(GTK_GRID(grid), button, minimal ? 3 : 1, 0, 1, 1);
    g_signal_connect(button, "clicked", G_CALLBACK(on_reset_clicked), NULL);

    g_object_set_data(G_OBJECT(grid), "InternalState", (gpointer)data);
    g_signal_connect_unlocked(grid, "destroy", G_CALLBACK(on_widget_destroy), NULL);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Custom callback for the resource widget manager
 *
 * This calls the reset methods on the various CRT sliders. It assumes the
 * widget was created for the settings UI ('minimal' argument set to false).
 *
 * \param[in]   widget  CRT control widget
 *
 * \return  bool
 *
 * FIXME:   When using the CRT widget in the settings UI and not as a widget
 *          controlled from the statusbar, the "Reset" shouldn't be there.
 *          So this code is not quite correct yet. I could hide the button,
 *          leaving all code intact and working, but that's lame.
 */
gboolean crt_control_widget_reset(GtkWidget *widget)
{
    GtkWidget *button;

    /* this assumes the CRT widget was created with 'minimal' set to False */
    button = gtk_grid_get_child_at(GTK_GRID(widget), 1, 0);
    if (GTK_IS_BUTTON(button)) {
        /* abuse event handler to reset widgets */
        on_reset_clicked(button, NULL);
        return TRUE;
    } else {
        return FALSE;
    }
}
