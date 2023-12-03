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
#include "drive.h"
#include "lib.h"
#include "resources.h"
#include "resourcewidgetmediator.h"
#include "vice_gtk3.h"

#include "rommanager.h"


/** \brief  Array length helper
 *
 * \param[in]   arr array
 *
 * \return  length of \a arr in number of elements
 */
#define ARRAY_LEN(arr)  (sizeof arr / sizeof arr[0])

/** \brief  Machine ROM information */
typedef struct machine_rom_s {
    const char   *name;     /**< ROM name for the UI */
    const char   *resource; /**< resource name for the ROM file */
} machine_rom_t;

/** \brief  Drive ROM information */
typedef struct drive_rom_s {
    int         type;       /**< drive type */
    const char *name;       /**< drive name for the UI */
    const char *resource;   /**< resource name for the ROM file */
} drive_rom_t;

/** \brief  Drive expansion ROM information */
typedef struct drive_exp_rom_s {
    const char   *name;     /**< ROM name for the UI */
    const char   *resource; /**< resource name for the ROM file */
} drive_exp_rom_t;


typedef struct rom_resource_s {
    const char *name;
    const char *value;
} rom_resource_t;


static rom_resource_t *expandable_list_get_resources(GtkWidget *list);


/** \brief  List of machine ROM names and resources */
static const machine_rom_t machine_rom_list[] = {
    /* Kernals */

    /* C64, C64DTV, VIC20, Plus/4, PET, CBM II */
    { "Kernal",                 "KernalName" },
    /* C128 kernals in UI description order */
    { "International kernal",   "KernalIntName" },
    { "Finish kernal",          "KernalFIName" },
    { "French kernal",          "KernalFRName" },
    { "German kernal",          "KernalDEName" },
    { "Italian kernal",         "KernalITName" },
    { "Norwegian kernal",       "KernalNOName" },
    { "Swedish kernal",         "KernalSEName" },
    { "Swiss kernal",           "KernalCHName" },
    { "C64 mode kernal",        "Kernal64Name" },

    /* Basics */

    /* C64, C64DTV, VIC20, Plus/4. PET, CBM II */
    { "Basic",                  "BasicName" },
    /* C128 */
    { "Basic low",              "BasicLoName" },
    { "Basic high",             "BasicHiName" },
    { "C64 mode Basic",         "Basic64Name" },

    /* Chargen, others */

    /* C64, C64DTV, VIC20, Plus/4, PET, CBM II */
    { "Chargen",                "ChargenName" },
    { "International chargen",  "ChargenINTName" },     /* C128 */
    { "Finish chargen",         "ChargenFIName" },      /* C128 */
    { "French chargen",         "ChargenFRName" },      /* C128 */
    { "German chargen",         "ChargenDEName" },      /* C128 */
    { "Italian chargen",        "ChargenITName" },      /* C128 */
    { "Norwegian chargen",      "ChargenNOName" },      /* C128 */
    { "Swedish chargen",        "ChargenSEName" },      /* C128 */
    { "Swiss chargen",          "ChargenCHName" },      /* C128 */
    { "Function low",           "FunctionLowName" },    /* Plus/4 */
    { "Function high",          "FunctionHighName" },   /* Plus/4 */
    { "C1 low",                 "c1loName" },           /* Plus/4 */
    { "C1 high",                "c1hiName" },           /* Plus/4 */
    { "C2 low",                 "c2loName" },           /* Plus/4 */
    { "C2 high",                "c2hiName" },           /* Plus/4 */
    { "Editor",                 "EditorName" },         /* PET */
    { "$9000-$9FFF ROM",        "RomModule9Name" },     /* PET */
    { "$A000-$AFFF ROM",        "RomModuleAName" },     /* PET */
    { "$B000-$BFFF ROM",        "RomModuleBName" },     /* PET */
    { "H6809 ROM A",            "H6809RomAName" },      /* PET */
    { "H6809 ROM B",            "H6809RomBName" },      /* PET */
    { "H6809 ROM C",            "H6809RomCName" },      /* PET */
    { "H6809 ROM D",            "H6809RomDName" },      /* PET */
    { "H6809 ROM E",            "H6809RomEName" },      /* PET */
    { "H6809 ROM F",            "H6809RomFName" },      /* PET */
};

/** \brief  List of drive ROM IDs, names and resources
 *
 * FIXME: shouldn't be here but somewhere in core code, but alas.
 */
static const drive_rom_t drive_rom_list[] = {
    { DRIVE_TYPE_1540,      DRIVE_NAME_1540,    "DosName1540" },
    { DRIVE_TYPE_1541,      DRIVE_NAME_1541,    "DosName1541" },
    { DRIVE_TYPE_1541II,    DRIVE_NAME_1541II,  "DosName1541ii" },
    { DRIVE_TYPE_1551,      DRIVE_NAME_1551,    "DosName1551" },
    { DRIVE_TYPE_1571,      DRIVE_NAME_1571,    "DosName1571" },
    { DRIVE_TYPE_1571CR,    DRIVE_NAME_1571CR,  "DosName1571cr" },
    { DRIVE_TYPE_1581,      DRIVE_NAME_1581,    "DosName1581" },
    { DRIVE_TYPE_2000,      DRIVE_NAME_2000,    "DosName2000" },
    { DRIVE_TYPE_4000,      DRIVE_NAME_4000,    "DosName4000" },
    { DRIVE_TYPE_2031,      DRIVE_NAME_2031,    "DosName2023" },
    { DRIVE_TYPE_2040,      DRIVE_NAME_2040,    "DosName2040" },
    { DRIVE_TYPE_3040,      DRIVE_NAME_3040,    "DosName3040" },
    { DRIVE_TYPE_4040,      DRIVE_NAME_4040,    "DosName4040" },
    { DRIVE_TYPE_1001,      DRIVE_NAME_1001,    "DosName1001" },
    { DRIVE_TYPE_8050,      DRIVE_NAME_8050,    NULL },
    { DRIVE_TYPE_8250,      DRIVE_NAME_8250,    NULL },
    { DRIVE_TYPE_9000,      DRIVE_NAME_9000,    "DosName9000" },
    { DRIVE_TYPE_CMDHD,     DRIVE_NAME_CMDHD,   "DosNameCMDHD" }
};

/** \brief  List of drive expansion ROMs names and resources */
static const drive_exp_rom_t drive_exp_rom_list[] = {
    { "Professional DOS 1571",  "DriveProfDOS1571Name" },
    { "Supercard+",             "DriveSuperCardName" },
    { "StarDOS",                "DriveStarDosName" },
};

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


static void on_load_romset_clicked(GtkButton *self, gpointer data)
{
    debug_gtk3("clicked");
}

static void on_save_romset_clicked(GtkButton *self, gpointer data)
{
    rom_resource_t *roms;
    int             i;

    debug_gtk3("clicked");
    roms = expandable_list_get_resources(drive_roms);
    if (roms == NULL) {
        return;
    }

    for (i = 0; roms[i].name != NULL; i++) {
        debug_gtk3("%s=\"%s\"", roms[i].name, roms[i].value);
    }
    lib_free(roms);
}

static void on_reset_to_defaults_clicked(GtkButton *self, gpointer data)
{
    debug_gtk3("clicked");
}



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

/** \brief  Get list of resource name and value pairs for a ROM list
 *
 * \param[in]   list    ROM list (GtkListBoxRow)
 *
 * \return  list of ROM resources in the list, terminated by \c .name being \c NULL
 *
 * \note    Free the list after use with \c lib_free()
 */
static rom_resource_t *expandable_list_get_resources(GtkWidget *list)
{
    GtkWidget      *expander;
    GtkWidget      *listbox;
    GList          *rows;
    GList          *row;
    rom_resource_t *roms;
    guint           num_rows;
    guint           i;

    expander = gtk_bin_get_child(GTK_BIN(list));
    listbox  = gtk_bin_get_child(GTK_BIN(expander));
    rows     = gtk_container_get_children(GTK_CONTAINER(listbox));
    if (rows == NULL) {
        return NULL;
    }
    num_rows = g_list_length(rows);
    roms     = lib_malloc((num_rows + 1u) * sizeof *roms);

    i = 0;
    for (row = rows; row != NULL; row = row->next) {
        GtkWidget  *grid;
        GtkWidget  *chooser;
        mediator_t *mediator;

        grid          = gtk_bin_get_child(GTK_BIN(row->data));
        chooser       = gtk_grid_get_child_at(GTK_GRID(grid), 1, 0);
        mediator      = mediator_for_widget(chooser);
        roms[i].name  = mediator_get_name(mediator);
        roms[i].value = mediator_get_resource_string(mediator);
        i++;
    }
    roms[i].name  = NULL;
    roms[i].value = NULL;

    g_list_free(rows);

    return roms;
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
    GtkWidget *button_box;
    GtkWidget *button;
    size_t     i;
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
    row++;

    machine_roms   = expandable_list_new("Machine ROMs");
    drive_roms     = expandable_list_new("Drive ROMs");
    drive_exp_roms = expandable_list_new("Drive expansion ROMs");
    gtk_container_add(GTK_CONTAINER(root_list), machine_roms);
    gtk_container_add(GTK_CONTAINER(root_list), drive_roms);
    gtk_container_add(GTK_CONTAINER(root_list), drive_exp_roms);

    /* add machine ROMs */
    for (i = 0; i < ARRAY_LEN(machine_rom_list); i++) {
        if (resources_exists(machine_rom_list[i].resource)) {
            rom_manager_add_machine_rom(machine_rom_list[i].name,
                                        machine_rom_list[i].resource);
        }
    }

    /* add drive ROMs */
    for (i = 0; i < ARRAY_LEN(drive_rom_list); i++) {
        if (resources_exists(drive_rom_list[i].resource)) {
            rom_manager_add_drive_rom(drive_rom_list[i].name,
                                      drive_rom_list[i].resource);
        }
    }

    /* add drive expansion ROMs */
    for (i = 0; i < ARRAY_LEN(drive_exp_rom_list); i++) {
        if (resources_exists(drive_exp_rom_list[i].resource)) {
            rom_manager_add_drive_exp_rom(drive_exp_rom_list[i].name,
                                          drive_exp_rom_list[i].resource);
        }
    }

    /* expand the machine ROMs by default */
    expander = gtk_bin_get_child(GTK_BIN(machine_roms));
    gtk_expander_set_expanded(GTK_EXPANDER(expander), TRUE);

    /* add buttons */
    button_box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    button = gtk_button_new_with_label("Load ROM set");
    g_signal_connect(G_OBJECT(button),
                     "clicked",
                     G_CALLBACK(on_load_romset_clicked),
                     NULL);
    gtk_box_pack_start(GTK_BOX(button_box), button, TRUE, FALSE, 8);
    button = gtk_button_new_with_label("Save ROM set");
    g_signal_connect(G_OBJECT(button),
                     "clicked",
                     G_CALLBACK(on_save_romset_clicked),
                     NULL);
    gtk_box_pack_start(GTK_BOX(button_box), button, TRUE, FALSE, 8);
    button = gtk_button_new_with_label("Reset to defaults");
    g_signal_connect(G_OBJECT(button),
                     "clicked",
                     G_CALLBACK(on_reset_to_defaults_clicked),
                     NULL);
    gtk_box_pack_start(GTK_BOX(button_box), button, TRUE, FALSE, 8);

    gtk_grid_attach(GTK_GRID(grid), button_box, 0, row, 1, 1);

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
    gtk_widget_set_size_request(label, 150, -1);
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
