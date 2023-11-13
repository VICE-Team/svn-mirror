/** \file   rommanager.c
 * \brief   Settings dialog to manage ROMs
 *
 * Presents a GtkListBox with expandable rows for machine, drive and drive
 * expansion ROMs, using a resource file chooser widget for each ROM.
 * Drag-n-drop should also work, though I've only tested that on my Debian
 * box with Gnome so far.
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
 */


#include <gtk/gtk.h>
#include <stdbool.h>

#include "debug_gtk3.h"
#include "resourcewidgetmediator.h"
#include "vice_gtk3.h"

#include "rommanager.h"


/** \brief  Patterns for the file chooser widgets */
static const char *chooser_patterns[] = { "*.bin", "*.rom", NULL };

/** \brief  Main GtkListBox */
static GtkWidget *root_list;

/** \brief  Machine ROMs widget
 *
 * GtkListBoxRow containing a GtkExpander containing a GtkListBox.
 */
static GtkWidget *machine_roms;

/** \brief  Drive ROMs widget
 *
 * GtkListBoxRow containing a GtkExpander containing a GtkListBox.
 */
static GtkWidget *drive_roms;

/** \brief  Drive expansion ROMs widget
 *
 * GtkListBoxRow containing a GtkExpander containing a GtkListBox.
 */
static GtkWidget *drive_exp_roms;


/** \brief  Create horizontally aligned label using Pango markup
 *
 * \param[in]   markup  Pango markup text for the label
 * \param[in]   halign  horizontal alignment for the label
 *
 * \return  GtkLabel
 */
static GtkWidget *label_helper(const char *markup, GtkAlign halign)
{
    GtkWidget *label = gtk_label_new(NULL);

    gtk_label_set_markup(GTK_LABEL(label), markup);
    gtk_widget_set_halign(label, halign);
    return label;
}

/** \brief  Create expandable list of file choosers
 *
 * Create a GtkListBoxRow with a GtkExpander that contains a GtkListBox for
 * GtkListBoxRows with file chooser widgets (and labels).
 *
 * \param[in]   title   title for the expander
 *
 * \return  GtkListBoxRow
 */
static GtkWidget *expandable_list_new(const char *title)
{
    GtkWidget *listrow;
    GtkWidget *expander;
    GtkWidget *list;

    listrow  = gtk_list_box_row_new();
    expander = gtk_expander_new(title);
    list     = gtk_list_box_new();

    gtk_container_add(GTK_CONTAINER(expander), list);
    gtk_container_add(GTK_CONTAINER(listrow), expander);
    return listrow;
}


/** \brief  Create ROM manager widget
 *
 * \param[in]   parent  unused
 *
 * \return  GtkGrid
 */
GtkWidget *rom_manager_new(GtkWidget *parent)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *scrolled;
    GtkWidget *expander;
    int        row = 0;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    label = label_helper("<b>ROM Manager (experimental!)</b>", GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, row, 0, 1, 1);
    row++;

    root_list = gtk_list_box_new();
    scrolled  = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_size_request(scrolled, 400, 400);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
                                   GTK_POLICY_NEVER,
                                   GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolled), root_list);
    gtk_grid_attach(GTK_GRID(grid), scrolled, 0, row, 1, 1);

    machine_roms   = expandable_list_new("Machine ROMs");
    drive_roms     = expandable_list_new("Drive ROMs");
    drive_exp_roms = expandable_list_new("Drive expansion ROMs");
    gtk_container_add(GTK_CONTAINER(root_list), machine_roms);
    gtk_container_add(GTK_CONTAINER(root_list), drive_roms);
    gtk_container_add(GTK_CONTAINER(root_list), drive_exp_roms);

    /* add C64 machine ROMs */
    rom_manager_add_machine_rom("Kernal",  "KernalName");
    rom_manager_add_machine_rom("Basic",   "BasicName");
    rom_manager_add_machine_rom("Chargen", "ChargenName");

    /* add C64 drive ROMs */
    rom_manager_add_drive_rom("1540",       "DosName1540");
    rom_manager_add_drive_rom("1541",       "DosName1541");
    rom_manager_add_drive_rom("1541-II",    "DosName1541ii");
    rom_manager_add_drive_rom("1570",       "DosName1570");
    rom_manager_add_drive_rom("1571",       "DosName1571");
    rom_manager_add_drive_rom("1581",       "DosName1581");

    rom_manager_add_drive_rom("FD-2000",    "DosName2000");
    rom_manager_add_drive_rom("FD-4000",    "DosName4000");
    rom_manager_add_drive_rom("CMD HD",     "DosNameCMDHD");

    rom_manager_add_drive_rom("2031",       "DosName2031");
    rom_manager_add_drive_rom("2040",       "DosName2040");
    rom_manager_add_drive_rom("3040",       "DosName3040");
    rom_manager_add_drive_rom("4040",       "DosName4040");
    rom_manager_add_drive_rom("1001",       "DosName1001");
    rom_manager_add_drive_rom("D9060/90",   "DosName9000");

#ifdef HAVE_EXPERIMENTAL_DEVICES
    rom_manager_add_drive_exp_rom("ProfDOS 1571",   "DriveProfDOS1571Name");
#endif
    rom_manager_add_drive_exp_rom("Supercard",      "DriveSuperCardName");
    rom_manager_add_drive_exp_rom("StarDOS",        "DriveStarDosName");

    /* expand the machine ROMs by default */
    expander = gtk_bin_get_child(GTK_BIN(machine_roms));
    gtk_expander_set_expanded(GTK_EXPANDER(expander), TRUE);

    gtk_widget_show_all(grid);
    return grid;
}

/** \brief  Handler for the 'drag-data-received' event of a resource file chooser
 *
 * \param[in]   self        resource file chooser
 * \param[in]   context     drag context
 * \param[in]   x           ignored
 * \param[in]   y           ignored
 * \param[in]   data        drag data
 * \param[in]   info        ignored
 * \param[in]   time        timestamp of drag event
 * \param[in]   user_data   extra event data (ignored)
 */
static void on_drag_data_received(GtkWidget        *self,
                                  GdkDragContext   *context,
                                  gint              x,
                                  gint              y,
                                  GtkSelectionData *data,
                                  guint             info,
                                  guint             time,
                                  gpointer          user_data)
{
    if (gtk_selection_data_get_length(data) > 0) {
        const char *filename;

        filename = (const char *)gtk_selection_data_get_data(data);
#ifdef HAVE_DEBUG_GTK3UI
        debug_gtk3("Setting resource \"%s\" to \"%s\"",
                   mediator_get_name_w(self), filename);
#endif
        vice_gtk3_resource_filechooser_set(self, filename);
        /* mark the drop finish so we don't end up with weird data like
         * "org.ibus.bla" in the entry */
        gtk_drag_finish(context, TRUE, FALSE /* don't delete source */, time);
    }
}


/** \brief  Add resource file chooser with label to a ROMs section
 *
 * \param[in]   list            ROMs section
 * \param[in]   label_text      text for the label
 * \param[in]   resource_name   resource name for the resource file chooser
 */
static void add_rom_chooser(GtkWidget  *list,
                            const char *label_text,
                            const char *resource_name)
{
    GtkWidget *listrow;
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *chooser;
    GtkWidget *expander;
    GtkWidget *rom_list;

    listrow = gtk_list_box_row_new();
    grid    = gtk_grid_new();
    label   = label_helper(label_text, GTK_ALIGN_START);
    chooser = vice_gtk3_resource_filechooser_new(resource_name,
                                                 GTK_FILE_CHOOSER_ACTION_OPEN);

    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);

    /* set up the label: we need to use xalign here to force left alignment
     * since we set a fixed size and the normal alignment is ignored */
    gtk_widget_set_size_request(label, 100, -1);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0);
    gtk_widget_set_margin_start(label, 8);

    /* set up the file chooser widget */
    vice_gtk3_resource_filechooser_set_filter(chooser,
                                             "ROM files",
                                             chooser_patterns,
                                             TRUE);
    vice_gtk3_resource_filechooser_set_custom_title(chooser, "Select ROM file");
    gtk_widget_set_halign(chooser, GTK_ALIGN_FILL);
    g_signal_connect(G_OBJECT(chooser),
                     "drag-data-received",
                     G_CALLBACK(on_drag_data_received),
                     NULL);

    /* put everthing together */
    expander = gtk_bin_get_child(GTK_BIN(list));
    rom_list = gtk_bin_get_child(GTK_BIN(expander));
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), chooser, 1, 0, 1, 1);
    gtk_container_add(GTK_CONTAINER(listrow), grid);
    gtk_list_box_insert(GTK_LIST_BOX(rom_list), listrow, -1);
}


void rom_manager_add_machine_rom(const char *label_text,
                                 const char *resource_name)
{
    add_rom_chooser(machine_roms, label_text, resource_name);
}


void rom_manager_add_drive_rom(const char *label_text,
                               const char *resource_name)
{
    add_rom_chooser(drive_roms, label_text, resource_name);
}


void rom_manager_add_drive_exp_rom(const char *label_text,
                                   const char *resource_name)
{
    add_rom_chooser(drive_exp_roms, label_text, resource_name);
}
