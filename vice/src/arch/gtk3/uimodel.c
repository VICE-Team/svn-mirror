/** \file   src/arch/gtk3/uimodel.c
 * \brief   Model settings dialog
 *
 * Controls the following resource(s):
 *  IECReset    (c64, c64sc, scpu64)
 *  GlueLogic   (c64sc, scpu64
 *  Go64Mode    (c128)
 *
 *  (for more, see used widgets)
 *
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
 *
 */

#include "vice.h"

#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>

#include "debug_gtk3.h"
#include "not_implemented.h"
#include "machine.h"
#include "resources.h"
#include "machinemodelwidget.h"
#include "videomodelwidget.h"
#include "vdcmodelwidget.h"
#include "sidmodelwidget.h"
#include "ciamodelwidget.h"
#include "kernalrevisionwidget.h"
#include "c128machinetypewidget.h"
#include "vic20memoryexpansionwidget.h"
#include "petkeyboardtypewidget.h"
#include "petvideosizewidget.h"
#include "petramsizewidget.h"
#include "petiosizewidget.h"
#include "petmiscwidget.h"
#include "superpetwidget.h"
#include "plus4memoryexpansionwidget.h"
#include "cbm2memorysizewidget.h"
#include "cbm2hardwiredswitcheswidget.h"
#include "cbm2rammappingwidget.h"

#include "uimodel.h"


/** \brief  Machine model widget
 *
 * Used by all machines
 */
static GtkWidget *machine_widget = NULL;

/** \brief  CIA model widget
 *
 * Used by C64, C64SC, SCPU64, C64DTV, C128, CBM5x0, CBM-II, VSID
 */
static GtkWidget *cia_widget = NULL;

/** \brief  Video model widget
 */
static GtkWidget *video_widget = NULL;
static GtkWidget *vdc_widget = NULL;
static GtkWidget *sid_widget = NULL;
static GtkWidget *kernal_widget = NULL;


/** \brief  Handler for the "toggled" event of the Reset-to-IEC radio buttons
 *
 * \param[in]   widget      radio button triggering the event
 * \param[in]   user_data   data for event (unused)
 */
static void on_reset_to_iec_toggle(GtkWidget *widget, gpointer user_data)
{
    int state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
    debug_gtk3("setting IECReset to %s\n", state ? "ON" : "OFF");
    resources_set_int("IECReset", state);
}


/** \brief  Handler for the "toggled" event of the C64SC Glue Logic radio buttons
 *
 * \param[in]   widget      radio button triggering the event
 * \param[in]   user_data   glue value (int)
 */
static void on_c64_glue_toggled(GtkWidget *widget, gpointer user_data)
{
    int glue = GPOINTER_TO_INT(user_data);

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
        debug_gtk3("setting GlueLogic to %s\n",
                glue == 0 ? "discrete" : "252535-01");
        resources_set_int("GlueLogic", glue);
    }
}


/** \brief  Handler for the "toggled" event of the C128 Go64 check button
 *
 * \param[in]   widget      check button triggering the event
 * \param[in]   user_data   data for event (unused);
 */
static void on_go64_toggled(GtkWidget *widget, gpointer user_data)
{
    int state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
    debug_gtk3("setting Go64Mode to %s\n", state ? "ON" : "OFF");
    resources_set_int("Go64Mode", state);
}




/** \brief  Create widget to toggle "Reset-to-IEC"
 *
 * \return  GtkGrid
 */
static GtkWidget *create_reset_to_iec_widget(void)
{
    int state;
    GtkWidget *check;

    resources_get_int("IECReset", &state);
    check = gtk_check_button_new_with_label("Reset goes to IEC");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), state);
    g_signal_connect(check, "toggled", G_CALLBACK(on_reset_to_iec_toggle),
            NULL);
    gtk_widget_show(check);
    return check;
}


/** \brief  Create widget to toggle "Go64Mode"
 *
 * \return  GtkGrid
 */
static GtkWidget *create_go64_widget(void)
{
    int state;
    GtkWidget *check;

    resources_get_int("Go64Mode", &state);
    check = gtk_check_button_new_with_label("Always switch to C64 mode on reset");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), state);
    g_signal_connect(check, "toggled", G_CALLBACK(on_go64_toggled),
            NULL);
    gtk_widget_show(check);
    return check;
}



/** \brief  Create widget to select C64SC Glue Logic
 *
 * \return  GtkGrid
 */
static GtkWidget *create_c64_glue_widget(void)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *discrete_radio;
    GtkWidget *custom_radio;
    GtkWidget *active;
    GSList *group = NULL;

    int glue;

    resources_get_int("GlueLogic", &glue);

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);

    label = gtk_label_new("Glue logic");
    g_object_set(label, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

    discrete_radio = gtk_radio_button_new_with_label(group, "Discrete");
    custom_radio = gtk_radio_button_new_with_label(group, "Custom IC");
    gtk_radio_button_join_group(GTK_RADIO_BUTTON(custom_radio),
            GTK_RADIO_BUTTON(discrete_radio));

    active = glue == 0 ? discrete_radio : custom_radio;
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(active), TRUE);

    g_signal_connect(discrete_radio, "toggled",
            G_CALLBACK(on_c64_glue_toggled), GINT_TO_POINTER(0));
    g_signal_connect(custom_radio, "toggled",
            G_CALLBACK(on_c64_glue_toggled), GINT_TO_POINTER(1));

    gtk_grid_attach(GTK_GRID(grid), discrete_radio, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), custom_radio, 2, 0, 1, 1);


    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create 'misc' widget for C64/C64SC/SCPU64
 *
 * \return  GtkGrid
 */
static GtkWidget *create_c64_misc_widget(void)
{
    GtkWidget *grid;
    GtkWidget *iec_widget;
    GtkWidget *glue_widget = NULL;

    grid = uihelpers_create_grid_with_label("Miscellaneous", 1);

    iec_widget = create_reset_to_iec_widget();
    g_object_set(iec_widget, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), iec_widget, 0, 1, 1, 1);
    if (machine_class == VICE_MACHINE_C64SC
            || machine_class == VICE_MACHINE_SCPU64) {
        glue_widget = create_c64_glue_widget();
        gtk_grid_attach(GTK_GRID(grid), glue_widget, 0, 2, 1, 1);
    }

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create 'misc' widget for C128
 *
 * \return  GtkGrid
 */
static GtkWidget *create_c128_misc_widget(void)
{
    GtkWidget *grid;
    GtkWidget *go64_widget;

    grid = uihelpers_create_grid_with_label("Miscellaneous", 1);

    go64_widget = create_go64_widget();
    g_object_set(go64_widget, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), go64_widget, 0, 1, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create widget layout for C64/C64SC
 *
 * \param[in,out]   grid    GtkGrid to use for layout
 *
 * \return  \a grid
 */
static GtkWidget *create_c64_layout(GtkWidget *grid)
{
    /* add machine widget */
    gtk_grid_attach(GTK_GRID(grid), machine_widget, 0, 0, 1, 2);

    /* VIC-II model widget */
    video_widget = video_model_widget_create(machine_widget);
    gtk_grid_attach(GTK_GRID(grid), video_widget, 1, 0, 1, 1);

    /* SID widget */
    sid_widget = sid_model_widget_create(machine_widget);
    gtk_grid_attach(GTK_GRID(grid), sid_widget, 1, 1, 1, 1);

    /* CIA1 & CIA2 widget */
    cia_widget = cia_model_widget_create(machine_widget, 2);
    gtk_grid_attach(GTK_GRID(grid), cia_widget, 0, 2, 2, 1);

    /* Kernal revision widget */
    kernal_widget = kernal_revision_widget_create();
    gtk_grid_attach(GTK_GRID(grid), kernal_widget, 2, 0, 1, 1);

    /* C64 misc. model settings */
    gtk_grid_attach(GTK_GRID(grid), create_c64_misc_widget(),
            2, 1, 1, 1);

    return grid;
}


/** \brief  Create widget layout for C128
 *
 * \param[in,out]   grid    GtkGrid to use for layout
 *
 * \return  \a grid
 */
static GtkWidget *create_c128_layout(GtkWidget *grid)
{
    GtkWidget *video_wrapper;
    GtkWidget *machine_wrapper;

    /* wrap machine model and machine type widgets in a single widget */
    machine_wrapper = gtk_grid_new();

    /* add machine model widget */
    gtk_grid_attach(GTK_GRID(machine_wrapper), machine_widget, 0, 0, 1, 1);
    /* add machine type widget */
    gtk_grid_attach(GTK_GRID(machine_wrapper),
            c128_machine_type_widget_create(),
            0 ,1, 1, 1);
    gtk_widget_show_all(machine_wrapper);
    gtk_grid_attach(GTK_GRID(grid), machine_wrapper, 0, 0, 1, 1);

    /* wrap VIC-II, VDC and CIA1/2 in a single widget */
    video_wrapper = gtk_grid_new();

    /* VIC-II model widget */
    video_widget = video_model_widget_create(machine_widget);
    gtk_grid_attach(GTK_GRID(video_wrapper), video_widget, 0, 0, 1, 1);
    /* VDC model widget */
    vdc_widget = vdc_model_widget_create();
    gtk_grid_attach(GTK_GRID(video_wrapper), vdc_widget, 0, 1, 1, 1);
    /* CIA1 & CIA2 widget */
    cia_widget = cia_model_widget_create(machine_widget, 2);
    gtk_grid_attach(GTK_GRID(video_wrapper), cia_widget, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), video_wrapper, 1, 0, 1, 1);

    /* SID widget */
    sid_widget = sid_model_widget_create(machine_widget);
    gtk_grid_attach(GTK_GRID(grid), sid_widget, 2, 0, 1, 1);

    /* Misc widget */
    gtk_grid_attach(GTK_GRID(grid), create_c128_misc_widget(), 0, 1, 3, 1);
    return grid;
#if 0

    if (machine_class != VICE_MACHINE_CBM6x0 &&
            machine_class != VICE_MACHINE_PET) {

        video_wrapper = gtk_grid_new();

        video_widget = video_model_widget_create();
        gtk_grid_attach(GTK_GRID(video_wrapper), video_widget, 0, 0, 1, 1);

        if (machine_class == VICE_MACHINE_C128) {
            vdc_widget = vdc_model_widget_create();
            gtk_grid_attach(GTK_GRID(video_wrapper), vdc_widget, 0, 1, 1, 1);
        }
        gtk_widget_show_all(video_wrapper);

        gtk_grid_attach(GTK_GRID(layout), video_wrapper, 1, 0, 1, 1);
    }
#endif

    return grid;
}

static GtkWidget *create_c64dtv_layout(GtkWidget *grid)
{
    INCOMPLETE_IMPLEMENTATION();
    return grid;
}


/** \brief  Create VIC20 model settings widget layout
 *
 * \param[in]   grid    GtkGrid to attach widget to
 *
 * \return  \a grid
 */
static GtkWidget *create_vic20_layout(GtkWidget *grid)
{
    GtkWidget *ram_widget;

    /* add machine widget */
    gtk_grid_attach(GTK_GRID(grid), machine_widget, 0, 0, 1, 1);

    /* VIC model widget */
    video_widget = video_model_widget_create(machine_widget);
    gtk_grid_attach(GTK_GRID(grid), video_widget, 1, 0, 1, 1);
#if 0
    /* SID widget */
    sid_widget = sid_model_widget_create(machine_widget);
#endif

    ram_widget = vic20_memory_expansion_widget_create();
    gtk_grid_attach(GTK_GRID(grid), ram_widget, 0, 1, 2, 1);

    gtk_widget_show_all(grid);
    return grid;
}

static GtkWidget *create_plus4_layout(GtkWidget *grid)
{
    GtkWidget *ram_widget;

    /* add machine widget */
    gtk_grid_attach(GTK_GRID(grid), machine_widget, 0, 0, 1, 1);

    /* VIC model widget */
    video_widget = video_model_widget_create(machine_widget);
    gtk_grid_attach(GTK_GRID(grid), video_widget, 1, 0, 1, 1);

    /* RAM size/expansion hacks */
    ram_widget = plus4_memory_expansion_widget_create();
    gtk_grid_attach(GTK_GRID(grid), ram_widget, 2, 0, 1, 1);

    INCOMPLETE_IMPLEMENTATION();
    return grid;
}

static GtkWidget *create_pet_layout(GtkWidget *grid)
{
    gtk_grid_attach(GTK_GRID(grid),
            machine_widget,
            0, 0, 1, 3);

    gtk_grid_attach(GTK_GRID(grid),
            pet_keyboard_type_widget_create(),
            1, 0, 1, 1);

    gtk_grid_attach(GTK_GRID(grid),
            pet_video_size_widget_create(),
            1, 1, 1, 1);

    gtk_grid_attach(GTK_GRID(grid),
            pet_ram_size_widget_create(),
            2, 0, 1, 1);

    gtk_grid_attach(GTK_GRID(grid),
            pet_io_size_widget_create(),
            2, 1, 1, 1);

    gtk_grid_attach(GTK_GRID(grid),
            pet_misc_widget_create(),
            1, 2, 2, 1);

    /* SuperPET widgets */
    gtk_grid_attach(GTK_GRID(grid),
            superpet_widget_create(),
            0, 3, 3, 1);

    INCOMPLETE_IMPLEMENTATION();
    return grid;
}

static GtkWidget *create_cbm5x0_layout(GtkWidget *grid)
{
    GtkWidget *ram_widget;
    GtkWidget *switches_widget;
    GtkWidget *bank15_widget;

    /* add machine widget */
    gtk_grid_attach(GTK_GRID(grid), machine_widget, 0, 0, 1, 1);

    /* add video widget */
    video_widget = video_model_widget_create(machine_widget);
    gtk_grid_attach(GTK_GRID(grid), video_widget, 1, 0, 1, 1);

    /* SID widget */
    sid_widget = sid_model_widget_create(machine_widget);
    gtk_grid_attach(GTK_GRID(grid), sid_widget, 1, 1, 1, 1);

    /* CIA1 widget */
    cia_widget = cia_model_widget_create(machine_widget, 1);
    gtk_grid_attach(GTK_GRID(grid), cia_widget, 2, 0, 1, 1);

    /* RAM size widget */
    ram_widget = cbm2_memory_size_widget_create();
    gtk_grid_attach(GTK_GRID(grid), ram_widget, 0, 1, 1, 1);

    /* Hardwired I/O port model switches */
    switches_widget = cbm2_hardwired_switches_widget_create();
    gtk_grid_attach(GTK_GRID(grid), switches_widget, 2, 1, 1, 1);

    /* Mapping RAM into bank 15 */
    bank15_widget = cbm2_ram_mapping_widget_create();
    gtk_grid_attach(GTK_GRID(grid), bank15_widget, 0, 2, 1, 1);

    INCOMPLETE_IMPLEMENTATION();
    return grid;
}

static GtkWidget *create_cbm6x0_layout(GtkWidget *grid)
{
    GtkWidget *ram_widget;
    GtkWidget *switches_widget;
    GtkWidget *bank15_widget;

    /* add machine widget */
    gtk_grid_attach(GTK_GRID(grid), machine_widget, 0, 0, 1, 2);
#if 0
    /* add video widget */
    video_widget = video_model_widget_create();
    gtk_grid_attach(GTK_GRID(grid), video_widget, 1, 0, 1, 1);
#endif
    /* SID widget */
    sid_widget = sid_model_widget_create(machine_widget);
    gtk_grid_attach(GTK_GRID(grid), sid_widget, 1, 0, 1, 1);

    /* Hardwired I/O port model switches */
    switches_widget = cbm2_hardwired_switches_widget_create();
    gtk_grid_attach(GTK_GRID(grid), switches_widget, 2, 0, 1, 1);

    /* CIA1 widget */
    cia_widget = cia_model_widget_create(machine_widget, 1);
    gtk_grid_attach(GTK_GRID(grid), cia_widget, 1, 1, 2, 1);

    /* RAM size widget */
    ram_widget = cbm2_memory_size_widget_create();
    gtk_grid_attach(GTK_GRID(grid), ram_widget, 0, 2, 1, 1);

    /* Mapping RAM into bank 15 */
    bank15_widget = cbm2_ram_mapping_widget_create();
    gtk_grid_attach(GTK_GRID(grid), bank15_widget, 1, 2, 2, 1);


    INCOMPLETE_IMPLEMENTATION();
    return grid;
}

static GtkWidget *create_vsid_layout(GtkWidget *grid)
{
    INCOMPLETE_IMPLEMENTATION();
    return grid;
}



/** \brief  Create machine-specific layout
 *
 * Creates a machine-specific layout, including creating the required widgets
 *
 * \return  GtkGrid
 */
static GtkWidget *create_layout(void)
{
    GtkWidget *grid = gtk_grid_new();

    /* Can't add the machine model widget here, since it differs a lot in size
     * depending on the machine, which may result in odd layouts. So we need
     * to determine per machine how many grid rows the machine model widget
     * needs to occupy */

    switch (machine_class) {
        case VICE_MACHINE_C64:      /* fall through */
        case VICE_MACHINE_C64SC:    /* fall through */
        case VICE_MACHINE_SCPU64:
            return create_c64_layout(grid);
        case VICE_MACHINE_C64DTV:
            return create_c64dtv_layout(grid);
        case VICE_MACHINE_C128:
            return create_c128_layout(grid);
        case VICE_MACHINE_VIC20:
            return create_vic20_layout(grid);
        case VICE_MACHINE_PLUS4:
            return create_plus4_layout(grid);
        case VICE_MACHINE_PET:
            return create_pet_layout(grid);
        case VICE_MACHINE_CBM5x0:
            return create_cbm5x0_layout(grid);
        case VICE_MACHINE_CBM6x0:
            return create_cbm6x0_layout(grid);
        case VICE_MACHINE_VSID:
            return create_vsid_layout(grid);
        default:
            /* shouldn't get here */
            fprintf(stderr, "Aargs! machine %d does not exist!", machine_class);
            exit(1);
    }
}


/** \brief  Create 'Model' widget for the settings UI
 *
 * \param[in]   parent  parent widget
 *
 * \return  GtkGrid
 */
GtkWidget *uimodel_create_central_widget(GtkWidget *parent)
{
    GtkWidget *layout;

    machine_widget = NULL;
    cia_widget = NULL;
    video_widget = NULL;
    vdc_widget = NULL;
    sid_widget = NULL;
    kernal_widget = NULL;

    /* every machine has a machine model widget */
    machine_widget = machine_model_widget_create();

    /* create machine-specific layout */
    layout = create_layout();


    /*
     * Connect signal handlers
     */
    machine_model_widget_connect_signals(machine_widget);
    if (machine_class != VICE_MACHINE_CBM6x0) {
        /* CBM6x0 ony has a simple CRTC, so no video widget used */
        video_model_widget_connect_signals(video_widget);
    }

#if 0

    /* create SID widget: every machine either has a SID built-in or can have
     * one via an expansion card, so no need to check machine_class here: */
    sid_widget = sid_model_widget_create(machine_widget);
    gtk_grid_attach(GTK_GRID(layout), sid_widget, 2, 0, 1, 1);


    /* CIA widget(s) */
    switch (machine_class) {

        /* 2x CIA */
        case VICE_MACHINE_C64:      /* fall through */
        case VICE_MACHINE_C64SC:    /* fall through */
        case VICE_MACHINE_SCPU64:   /* fall through */
        case VICE_MACHINE_C128:     /* fall through */
        case VICE_MACHINE_VSID:
            cia_widget = cia_model_widget_create(machine_widget, 2);
            break;

        /* 1x CIA */
        case VICE_MACHINE_CBM5x0:   /* fall through */
        case VICE_MACHINE_CBM6x0:
            cia_widget = cia_model_widget_create(machine_widget, 1);
            break;

        /* no CIA */
        default:
            /* other models don't have CIA's */
            cia_widget = NULL;
    }

    if (cia_widget != NULL) {
        gtk_grid_attach(GTK_GRID(layout), cia_widget, 0, 1, 2, 1);
    }

    kernal_widget = kernal_revision_widget_create();
    gtk_grid_attach(GTK_GRID(layout), kernal_widget, 2, 1, 1, 1);


    /*
     * Connect signals that were not connected in the previous calls
     */
    machine_model_widget_connect_signals(machine_widget);
    if (machine_class != VICE_MACHINE_CBM6x0 &&
            machine_class != VICE_MACHINE_PET) {
        video_model_widget_connect_signals(video_widget);
    }
    if (machine_class == VICE_MACHINE_C128) {
        vdc_model_widget_connect_signals(vdc_widget);
    }
#endif
    gtk_widget_show_all(layout);
    return layout;
}


