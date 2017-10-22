/** \file   src/arch/gtk3/widgets/sidsoundwidget.c
 * \brief   Settings for SID emulation
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *  SidEngine
 *  SidStereo (int (0-3), enabled extra SIDs)
 *  SidResidSampling
 *  SidFilters
 *  SidStereoAddressStart
 *  SidTripleAddressStart
 *  SidQuadAddressStart
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

#include "lib.h"
#include "machine.h"
#include "resources.h"
#include "basewidgets.h"
#include "debug_gtk3.h"
#include "catweaselmkiii.h"
#include "hardsid.h"
#include "parsid.h"

#include "sidsoundwidget.h"


/** \brief  Values for the "SidEngine" resource
 */
static ui_radiogroup_entry_t sid_engines[] = {
    { "FastSID", 0 },
    { "ReSID", 1 },
    { "Catweasel MKIII", 2 },
    { "HardSID", 3 },
    { "ParSID port 1", 4 },
    { "ParSID port 2", 5 },
    { "ParSID port 3", 6 },
    { NULL, -1 }
};

/** \brief  Values for the "SidResidSampling" resource
 */
static ui_radiogroup_entry_t resid_sampling_modes[] = {
    { "Fast", 0 },
    { "Interpolation", 1 },
    { "Resampling", 2 },
    { "Fast resampling", 3 },
    { NULL, -1 }
};


/** \brief  Values for the "number of sids" widget
 */
static ui_radiogroup_entry_t num_sids[] = {
    { "One", 0 },
    { "Two", 1 },
    { "Three", 2 },
    { "Four" , 3 },
    { NULL, -1 }
};


/** \brief  I/O addresses for extra SID's for the C64
 *
 * \note    Yes, I know I can generate this table
 */
static ui_combo_entry_int_t sid_address_c64[] = {
    { "$d420", 0xd420 }, { "$d440", 0xd440 }, { "$d460", 0xd460 },
    { "$d480", 0xd480 }, { "$d4a0", 0xd4a0 }, { "$d4c0", 0xd4c0 },
    { "$d4e0", 0xd4e0 },

    { "$d500", 0xd500 }, { "$d520", 0xd520 }, { "$d540", 0xd540 },
    { "$d560", 0xd560 }, { "$d580", 0xd580 }, { "$d5a0", 0xd5a0 },
    { "$d5c0", 0xd5c0 }, { "$d5e0", 0xd5e0 },

    { "$d600", 0xd600 }, { "$d620", 0xd620 }, { "$d640", 0xd640 },
    { "$d660", 0xd660 }, { "$d680", 0xd680 }, { "$d6a0", 0xd6a0 },
    { "$d6c0", 0xd6c0 }, { "$d6e0", 0xd6e0 },

    { "$d700", 0xd700 }, { "$d720", 0xd720 }, { "$d740", 0xd740 },
    { "$d760", 0xd760 }, { "$d780", 0xd780 }, { "$d7a0", 0xd7a0 },
    { "$d7c0", 0xd7c0 }, { "$d7e0", 0xd7e0 },

    { "$de00", 0xde00 }, { "$de20", 0xde20 }, { "$de40", 0xde40 },
    { "$de60", 0xde60 }, { "$de80", 0xde80 }, { "$dea0", 0xdea0 },
    { "$dec0", 0xdec0 }, { "$dee0", 0xdee0 },

    { "$df00", 0xdf00 }, { "$df20", 0xdf20 }, { "$df40", 0xdf40 },
    { "$df60", 0xdf60 }, { "$df80", 0xdf80 }, { "$dfa0", 0xdfa0 },
    { "$dfc0", 0xdfc0 }, { "$dfe0", 0xdfe0 }
};


/** \brief  I/O addresses for extra SID's for the C128
 */
static ui_combo_entry_int_t sid_address_c128[] = {
    { "$d420", 0xd420 }, { "$d440", 0xd440 }, { "$d460", 0xd460 },
    { "$d480", 0xd480 }, { "$d4a0", 0xd4a0 }, { "$d4c0", 0xd4c0 },
    { "$d4e0", 0xd4e0 },

    { "$d700", 0xd700 }, { "$d720", 0xd720 }, { "$d740", 0xd740 },
    { "$d760", 0xd760 }, { "$d780", 0xd780 }, { "$d7a0", 0xd7a0 },
    { "$d7c0", 0xd7c0 }, { "$d7e0", 0xd7e0 },

    { "$de00", 0xde00 }, { "$de20", 0xde20 }, { "$de40", 0xde40 },
    { "$de60", 0xde60 }, { "$de80", 0xde80 }, { "$dea0", 0xdea0 },
    { "$dec0", 0xdec0 }, { "$dee0", 0xdee0 },

    { "$df00", 0xdf00 }, { "$df20", 0xdf20 }, { "$df40", 0xdf40 },
    { "$df60", 0xdf60 }, { "$df80", 0xdf80 }, { "$dfa0", 0xdfa0 },
    { "$dfc0", 0xdfc0 }, { "$dfe0", 0xdfe0 }
};


/** \brief  Reference to resid sampling widget
 *
 * Used to enable/disable when the SID engine changes
 */
static GtkWidget *resid_sampling;


/** \brief  Reference to the extra SID address widgets
 *
 * Used to enable/disable depending on the number of SIDs active
 */
static GtkWidget *address_widgets[3];


/** \brief  Extra callback registered to the SidEngine radiogroup
 *
 * \param[in]   widget  widget triggering the event
 * \param[in]   engine  engine ID
 */
static void on_sid_engine_changed(GtkWidget *widget, int engine)
{
    debug_gtk3("SID engine changed to %d\n", engine);
    gtk_widget_set_sensitive(resid_sampling, engine == 1);
}


/** \brief  Extra callback registered to the 'number of SIDs' radiogroup
 *
 * \param[in]   widget  widget triggering the event
 * \param[in]   count   number of extra SIDs (0-3)
 */

static void on_sid_count_changed(GtkWidget *widget, int count)
{
    debug_gtk3("extra SIDs count changed to %d\n", count);

    gtk_widget_set_sensitive(address_widgets[0], count > 0);
    gtk_widget_set_sensitive(address_widgets[1], count > 1);
    gtk_widget_set_sensitive(address_widgets[2], count > 2);
}


/** \brief  Create widget to control the SID engine
 *
 * \return  GtkGrid
 */
static GtkWidget *create_sid_engine_widget(void)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *radio_group;

    grid = gtk_grid_new();
    g_object_set(grid, "margin-left", 8, NULL);

    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), "<b>SID Engine</b>");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    g_object_set(label, "margin-bottom", 8, NULL);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

    radio_group = resource_radiogroup_create("SidEngine", sid_engines,
            GTK_ORIENTATION_VERTICAL);
    g_object_set(radio_group, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), radio_group, 0, 1, 1, 1);

    if (!catweaselmkiii_available()) {
        gtk_widget_set_sensitive(gtk_grid_get_child_at(GTK_GRID(radio_group),
                    0, 2), FALSE);
    }
    if (!hardsid_drv_available()) {
        gtk_widget_set_sensitive(gtk_grid_get_child_at(GTK_GRID(radio_group),
                    0, 3), FALSE);
    }
    if (!parsid_drv_available()) {
        int p;
        for (p = 4; p < 7; p++) {
            gtk_widget_set_sensitive(
                    gtk_grid_get_child_at(GTK_GRID(radio_group), 0, p), FALSE);
        }
    }

    resource_radiogroup_add_callback(radio_group, on_sid_engine_changed);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create widget to control ReSID sampling method
 *
 * \return  GtkGrid
 */
static GtkWidget *create_resid_sampling_widget(void)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *radio_group;

    grid = gtk_grid_new();
    g_object_set(grid, "margin-left", 8, NULL);

    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), "<b>ReSID sampling method</b>");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    g_object_set(label, "margin-bottom", 8, NULL);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

    radio_group = resource_radiogroup_create("SidResidSampling",
            resid_sampling_modes, GTK_ORIENTATION_VERTICAL);
    g_object_set(radio_group, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), radio_group, 0, 1, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create widget to set the number of emulated SID's
 *
 * \return  GtkGrid
 */
static GtkWidget *create_num_sids_widget(void)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *radio_group;

    grid = gtk_grid_new();
    g_object_set(grid, "margin-left", 8, NULL);

    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), "<b>Number of SIDs</b>");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    g_object_set(label, "margin-bottom", 8, NULL);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

    radio_group = resource_radiogroup_create("SidStereo",
            num_sids, GTK_ORIENTATION_VERTICAL);
    g_object_set(radio_group, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), radio_group, 0, 1, 1, 1);

    resource_radiogroup_add_callback(radio_group, on_sid_count_changed);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create widget for extra SID addresses
 *
 * \param[in]   sid     extra SID number (1-3)
 *
 * \return  GtkGrid
 */
static GtkWidget *create_extra_sid_address_widget(int sid)
{
    GtkWidget *widget;
    ui_combo_entry_int_t *entries;
    char label[256];
    const char *resource[3] = {
        "SidStereoAddressStart",
        "SidTripleAddressStart",
        "SidQuadAddressStart"
    };

    g_snprintf(label, 256, "SID #%d address", sid + 1);
    entries = machine_class == VICE_MACHINE_C128
        ? sid_address_c128 : sid_address_c64;

    widget = resource_combo_box_int_create_with_label(resource[sid - 1],
            entries, label);
    gtk_widget_show_all(widget);
    return widget;
}


/** \brief  Create widget to control SID settings
 *
 * \param[in]   parent  parent widget
 *
 * \return  GtkGrid
 */
GtkWidget *sid_sound_widget_create(GtkWidget *parent)
{
    GtkWidget *layout;
    GtkWidget *label;
    GtkWidget *engine;
    GtkWidget *num_sids;
    GtkWidget *filters;
    int current_engine;
    int stereo;
    int i;

    layout = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(layout), 8);
    gtk_grid_set_row_spacing(GTK_GRID(layout), 8);

    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), "<b>SID settings</b>");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(layout), label, 0, 0, 3, 1);

    engine = create_sid_engine_widget();
    gtk_grid_attach(GTK_GRID(layout), engine, 0, 1, 1,1);

    resid_sampling = create_resid_sampling_widget();
    gtk_grid_attach(GTK_GRID(layout), resid_sampling, 1, 1, 1, 1);
    resources_get_int("SidEngine", &current_engine);
    gtk_widget_set_sensitive(resid_sampling, current_engine == 1);


    num_sids = create_num_sids_widget();
    gtk_grid_attach(GTK_GRID(layout), num_sids, 2, 1, 1, 1);

    for (i = 1; i < 4; i++) {
        address_widgets[i - 1] = create_extra_sid_address_widget(i);
        gtk_grid_attach(GTK_GRID(layout), address_widgets[i - 1],
                i - 1, 2, 1, 1);
    }

    filters = resource_check_button_create("SidFilters",
            "Enable SID filter emulation");
    gtk_grid_attach(GTK_GRID(layout), filters, 0, 3, 3, 1);

    /* set sensitivity of address widgets */
    resources_get_int("SidStereo", &stereo);
    on_sid_count_changed(NULL, stereo);

    gtk_widget_show_all(layout);
    return layout;
}
