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
 * |U-only delayline    | yes  |  no  | no|yes | no |yes | no | no |
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
 * $VICERES TEDPALDelaylineType     xplus4
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
 * $VICERES VICPALDelaylineType     xvic
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
 * $VICERES VICIIPALDelaylineType   x64 x64sc x64dtv xscpu64 x128 xcbm5x0
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


/** \brief  CSS for the scale widgets in the status bar CRT widget
 *
 * This makes the sliders take up less vertical space. The margin can be set
 * to a negative value (in px) to allow the slider to be larger than the scale
 * itself.
 *
 * Probably will require some testing/tweaking to get this to look acceptable
 * with various themes (and OSes).
 */
#define SCALE_CSS_STATUSBAR \
    "scale slider {\n" \
    "  min-width: 10px;\n" \
    "  min-height: 10px;\n" \
    "  margin: -3px;\n" \
    "}\n\n" \
    "scale value {\n" \
    "  font-family: monospace;\n" \
    "  font-size: 80%;\n" \
    "}\n" \
    "scale {\n" \
    "  margin-top: -8px;\n" \
    "  margin-bottom: -8px;\n" \
    "}"


/** \brief  CSS used to tweak looks of CRT sliders in the settings dialog
 *
 * We use monospace for the value labels to keep them aligned on the decimal
 * point and to keep the width of the sliders consistent.
 */
#define SCALE_CSS_DIALOG \
    "scale value {\n" \
    "  font-family: monospace;\n" \
    "}"


/** \brief  CSS for the "U-only delayline" check button on the status bar
 */
#define CHECKBUTTON_CSS_STATUSBAR \
    "checkbutton {\n" \
    "  font-size: 80%;\n" \
    "}\n" \
    "checkbutton check {\n" \
    "  min-width: 12px;\n" \
    "  min-height: 12px;\n" \
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
    int low;            /**< lowest value for resource */
    int high;           /**< highest value for resource */
    int step;           /**< stepping for the spin button */
    gdouble disp_low;   /**< display low */
    gdouble disp_high;  /**< display high */
    gdouble disp_step;  /**< stepping for the displayed values */
    const char *disp_fmt;   /**< format string for the displayed value */
} crt_control_resource_t;


/** \brief  CRT resource object
 */
typedef struct crt_control_s {
    crt_control_resource_t res;     /**< resource info */
    GtkWidget *scale;               /**< GtkScale reference */
#if 0
    GtkWidget *spin;                /**< GtkSpinButton reference */
#endif
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
    GtkWidget *delayline;
} crt_control_data_t;


/** \brief  List of CRT emulation resources
 */
static const crt_control_resource_t resource_table[RESOURCE_COUNT_MAX] = {
    { "Brightness",     "ColorBrightness",  0, 2000, 100,   0.0, 200.0,  0.1, "%5.1f%%" },
    { "Contrast",       "ColorContrast",    0, 2000, 100,   0.0, 200.0,  0.1, "%5.1f%%" },
    { "Saturation",     "ColorSaturation",  0, 2000, 100,   0.0, 200.0,  0.1, "%5.1f%%" },
    { "Tint",           "ColorTint",        0, 2000, 100, -25.0,  25.0,  0.1, "%+5.1f%%" },
    { "Gamma",          "ColorGamma",       0, 4000, 200,   0.0,   4.0, 0.01, "%6.2f" },
    { "Blur",           "PALBlur",          0, 1000,  50,   0.0, 100.0,  0.1, "%5.1f%%" },
    { "Scanline shade", "PALScanLineShade", 0, 1000,  50,   0.0, 100.0,  0.1, "%5.1f%%" },
    { "Oddline phase",  "PALOddLinePhase",  0, 2000, 100, -25.0,  25.0,  0.1, "%+5.1f\u00b0" },
    { "Oddline offset", "PALOddLineOffset", 0, 2000, 100, -50.0,  50.0,  0.1, "%+4.1f%%" }
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

/** \brief  CSS provider for scales in the CRT widget for the statusbar
 */
static GtkCssProvider *scale_css_statusbar;

/** \brief  CSS provider for scales in the settings dialog
 */
static GtkCssProvider *scale_css_dialog;


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


/** \brief  Determine if the PAL-specific controls must be enabled
 *
 * Check video standard and \a chip for PAL support.
 *
 * \param[in]   chip    video chip name
 *
 * \return  TRUE if PAL controls must be enabled
 */
static gboolean is_pal(const char *chip)
{
    int standard = 0;
    int chip_id;

    resources_get_int("MachineVideoStandard", &standard);
    chip_id = get_chip_id(chip);

    if ((standard == MACHINE_SYNC_PAL || standard == MACHINE_SYNC_PALN) &&
            (chip_id != CHIP_CRTC && chip_id != CHIP_VDC)) {
        return TRUE;
    }
    return FALSE;
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
    for (i = 0; i < RESOURCE_COUNT_MAX; i++) {
        crt_control_t control = data->controls[i];

        if (control.scale != NULL) {
            vice_gtk3_resource_scale_custom_factory(control.scale);
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
 * \param[in]   resource        resource name without the \a chip prefix
 * \param[in]   chip            video chip name
 * \param[in]   resource_low    resource value lower bound
 * \param[in]   resource_high   resource value upper bound
 * \param[in]   display_low     display value lower bound
 * \param[in]   display_high    display value upper bound
 * \param[in]   display_step    display value stepping
 * \param[in]   display_format  format string for displaying value
 * \param[in]   minimal         reduced size (for the statusbar widget)
 *
 * \return  GtkScale
 */
static GtkWidget *create_slider(
        const char *resource, const char *chip,
        int resource_low, int resource_high,
        gdouble display_low, gdouble display_high, gdouble display_step,
        const char *display_format,
        gboolean minimal)
{
    GtkWidget *scale;

    scale = vice_gtk3_resource_scale_custom_new_printf(
            "%s%s",
            GTK_ORIENTATION_HORIZONTAL,
            resource_low, resource_high,
            display_low, display_high, display_step,
            display_format, chip, resource);
    gtk_widget_set_hexpand(scale, TRUE);
    gtk_scale_set_value_pos(GTK_SCALE(scale), GTK_POS_RIGHT);

    /* set up custom CSS to make the scale take up less space */
    if (minimal) {
        vice_gtk3_css_provider_add(scale, scale_css_statusbar);
    } else {
        vice_gtk3_css_provider_add(scale, scale_css_dialog);
    }

    return scale;
}

/** \brief  Create "U-only delayline" check button
 *
 * \param[in]   chip    video chip name
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_delayline_widget(const char *chip)
{
    GtkWidget *check;

    check = vice_gtk3_resource_check_button_new_sprintf(
            "%sPALDelaylineType", "U-only Delayline", chip);
    return check;
}

/** \brief  Add GtkScale sliders to \a grid
 *
 * \param[in,out]   grid    grid to add widgets to
 * \param[in,out]   data    internal data of the main widget
 * \param[in]       minimal minimize size of the slider, no spinboxes
 *
 * \return  row number of last widget added
 */
static int add_sliders(GtkGrid *grid,
                        crt_control_data_t *data,
                        gboolean minimal)
{
    GtkWidget *label;
    const char *chip;
    int row = 1;
    int chip_id;
    size_t i;

    chip = data->chip;
    chip_id = get_chip_id(chip);
    if (chip_id < 0) {
        log_error(LOG_ERR, "failed to get chip ID for '%s'.", chip);
        return 0;
    }

    if (!minimal) {
        for (i = 0; i < RESOURCE_COUNT_MAX; i ++) {
            crt_control_t *control = &(data->controls[i]);
            label = create_label(control->res.label, minimal);
            gtk_grid_attach(grid, label, 0, row, 1, 1);
            control->scale = create_slider(control->res.name, chip,
                    control->res.low, control->res.high,
                    control->res.disp_low, control->res.disp_high, control->res.disp_step,
                    control->res.disp_fmt,
                    minimal);
            gtk_grid_attach(grid, control->scale, 1, row, 1, 1);
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
                    control->res.low, control->res.high,
                    control->res.disp_low, control->res.disp_high, control->res.disp_step,
                    control->res.disp_fmt,
                    minimal);
            gtk_grid_attach(grid, control->scale, col + 1, row, 1, 1);
            if (col > 0) {
                row++;
            }
        }
    }

    /* Determine if we're using PAL or NTSC: the *PAL* resource sliders should be
     * disabled when the video standard is NTSC: */
    if (!is_pal(chip)) {
        for (i = 0; i < RESOURCE_COUNT_MAX; i ++) {
            crt_control_t *control = &(data->controls[i]);
            int is_ntsc = strncmp(control->res.name, "PAL", 3) != 0;

            if (control->scale != NULL) {
                gtk_widget_set_sensitive(control->scale, is_ntsc);
            }
        }
    }

    return row + 1;
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
        control->res.disp_low = resource_table[i].disp_low;
        control->res.disp_high = resource_table[i].disp_high;
        control->res.disp_step = resource_table[i].disp_step;
        control->res.disp_fmt = resource_table[i].disp_fmt;
        control->scale = NULL;
    }
    data->delayline = NULL;
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
    int row;

    /* create reusable CSS providers */
    label_css_provider = vice_gtk3_css_provider_new(LABEL_CSS);
    if (label_css_provider == NULL) {
        return NULL;
    }
    scale_css_statusbar = vice_gtk3_css_provider_new(SCALE_CSS_STATUSBAR);
    if (scale_css_statusbar == NULL) {
        return NULL;
    }
    scale_css_dialog = vice_gtk3_css_provider_new(SCALE_CSS_DIALOG);
    if (scale_css_dialog == NULL) {
        return NULL;
    }

    data = create_control_data(chip);

    grid = vice_gtk3_grid_new_spaced(16, 0);
    gtk_widget_set_margin_start(grid, 8);
    gtk_widget_set_margin_end(grid, 8);
    if (minimal) {
        g_snprintf(buffer, sizeof buffer,
                   "<small><b>CRT settings (%s)</b></small>", chip);
    } else {
        g_snprintf(buffer, sizeof buffer, "<b>CRT settings (%s)</b>", chip);
    }

    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), buffer);
    gtk_widget_set_halign(label, GTK_ALIGN_CENTER);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

    /* add scales and spin buttons */
    row = add_sliders(GTK_GRID(grid), data, minimal);

    /* add U-only delayline check button */
    data->delayline = create_delayline_widget(chip);
    if (minimal) {
        vice_gtk3_css_add(data->delayline, CHECKBUTTON_CSS_STATUSBAR);
        gtk_grid_attach(GTK_GRID(grid), data->delayline, 2, row - 1, 2, 1);
    } else {
        gtk_grid_attach(GTK_GRID(grid), data->delayline, 0, row, 3, 1);
    }
    /* enable if PAL */
    gtk_widget_set_sensitive(data->delayline, is_pal(chip));
    row++;

    button = gtk_button_new_with_label("Reset");
    gtk_widget_set_halign(button, GTK_ALIGN_END);
    gtk_grid_attach(GTK_GRID(grid), button, minimal ? 3 : 2, 0, 1, 1);
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
