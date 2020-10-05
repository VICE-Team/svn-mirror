/** \file   settings_model.c
 * \brief   Model settings dialog
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES IECReset            x64 x64sc xscpu64
 * $VICERES GlueLogic           x64 x64sc xscpu64
 * $VICERES Go64Mode            x128
 * $VICERES DtvRevision         x64dtv
 * $VICERES VICIINewLuminances  x64dtv
 *
 *  (for more, see used widgets)
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <gtk/gtk.h>

#include "archdep.h"
#include "c128machinetypewidget.h"
#include "cbm2hardwiredswitcheswidget.h"
#include "cbm2memorysizewidget.h"
#include "cbm2rammappingwidget.h"
#include "ciamodelwidget.h"
#include "debug_gtk3.h"
#include "kernalrevisionwidget.h"
#include "machine.h"
#include "machinemodelwidget.h"
#include "petiosizewidget.h"
#include "petram9widget.h"
#include "petramawidget.h"
#include "petkeyboardtypewidget.h"
#include "petmiscwidget.h"
#include "petmodel.h"
#include "petramsizewidget.h"
#include "petvideosizewidget.h"
#include "plus4aciawidget.h"
#include "plus4memhacks.h"
#include "plus4memoryexpansionwidget.h"
#include "plus4memorysizewidget.h"
#include "resourcecheckbutton.h"
#include "resources.h"
#include "sidmodelwidget.h"
#include "superpetwidget.h"
#include "vdcmodelwidget.h"
#include "vic20memoryexpansionwidget.h"
#include "vice_gtk3.h"
#include "videomodelwidget.h"
#include "v364speechwidget.h"

#include "c64model.h"


#include "settings_model.h"


/*
 * Forward declarations
 */
static void plus4_debug_dump_resources(void);


/** \brief  List of C64DTV revisions
 */
static const vice_gtk3_radiogroup_entry_t c64dtv_revisions[] = {
    { "DTV2", 2 },
    { "DTV3", 3 },
    { NULL, -1 }
};


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


static int (*get_model_func)(void);

static const char *(*get_memhack_func)(int hack);


/** \brief  Video model widget
 */
static GtkWidget *video_widget = NULL;

static GtkWidget *ram_widget = NULL;
static GtkWidget *memhack_widget = NULL;
static GtkWidget *acia_widget = NULL;
static GtkWidget *speech_widget = NULL;
static GtkWidget *vdc_widget = NULL;
static GtkWidget *sid_widget = NULL;
static GtkWidget *kernal_widget = NULL;
static GtkWidget *pet_video_size_widget = NULL;
static GtkWidget *pet_keyboard_widget = NULL;
static GtkWidget *pet_misc_widget = NULL;
static GtkWidget *pet_io_widget = NULL;
static GtkWidget *pet_ram9_widget = NULL;
static GtkWidget *pet_rama_widget = NULL;

static GtkWidget *c64dtv_rev_widget = NULL;
static GtkWidget *reset_with_iec_widget = NULL;


static void video_model_callback(int model)
{
#ifdef HAVE_DEBUG_GTK3UI
    int true_model = -1;
#endif

    debug_gtk3("got model %d", model);

    if (get_model_func != NULL) {
#ifdef HAVE_DEBUG_GTK3UI
        true_model = get_model_func();
#endif
        debug_gtk3("got true model %d", true_model);
        machine_model_widget_update(machine_widget);
        if (machine_class == VICE_MACHINE_PLUS4) {
            plus4_debug_dump_resources();
        }
    }
}


/* {{{ C128 glue logic
 *
 * x128-specific callbacks
 */

static void vdc_revision_callback(int revision)
{
    debug_gtk3("got VDC revision %d.", revision);
    if (get_model_func != NULL) {
        machine_model_widget_update(machine_widget);
    }
}


static void vdc_ram_callback(int state)
{    debug_gtk3("Got VDC 64KB RAM state %d.", state);
    if (get_model_func != NULL) {
        machine_model_widget_update(machine_widget);
    }
}

/* }}} */

static void sid_model_callback(int model)
{
#ifdef HAVE_DEBUG_GTK3UI
    int true_model = -1;
#endif

    debug_gtk3("got model %d", model);

    if (get_model_func != NULL) {
#ifdef HAVE_DEBUG_GTK3UI
        true_model = get_model_func();
#endif
        debug_gtk3("got true model %d", true_model);
        machine_model_widget_update(machine_widget);
    }
}


/** \brief  Custom callback for the Kernal Revision widget
 *
 * Triggers an update of the 'machine model' widget when a different kernal rev
 * has been selected. Only valid for x64/x64sc as far as I know.
 *
 * \param[in]   rev     new KERNAL revision
 */
static void kernal_revision_callback(int rev)
{
#ifdef HAVE_DEBUG_GTK3UI
    int true_model = -1;

    if (get_model_func != NULL) {
        true_model = get_model_func();
        debug_gtk3("Got model ID: %d.", true_model);
    }
#endif
    debug_gtk3("Got revision %d.", rev);
    machine_model_widget_update(machine_widget);
}


static void iec_callback(GtkWidget *widget, gpointer data)
{
    debug_gtk3("Called.");
    machine_model_widget_update(machine_widget);
}



/** \brief  Callback for CIA model changes
 *
 * \param[in]   cia_num     CIA number (1 or 2)
 * \param[in]   cia_model   CIA model ID
 */
static void cia_model_callback(int cia_num, int cia_model)
{
    debug_gtk3("got CIA %d, model %d", cia_num, cia_model);

    if (get_model_func != NULL) {
        machine_model_widget_update(machine_widget);
    }
}


/* {{{ PET glue logic */

/** \brief  Callback for PET RAM size changes
 *
 * \param[in]   size    RAM size in KB
 */
static void pet_ram_size_callback(int size)
{
    debug_gtk3("Called with %d RAM.", size);
    if (get_model_func != NULL) {
        machine_model_widget_update(machine_widget);
    }
}


static void pet_video_size_callback(int size)
{
    debug_gtk3("Called with %d video size.", size);
    if (get_model_func != NULL) {
        machine_model_widget_update(machine_widget);
    }
}


static void pet_keyboard_type_callback(int type)
{
    debug_gtk3("called with keyboard type %d.", type);
    if (get_model_func != NULL) {
        machine_model_widget_update(machine_widget);
    }
}

static void pet_crtc_callback(int state)
{
    debug_gtk3("called with CRTC %d.", state);
    if (get_model_func != NULL) {
        machine_model_widget_update(machine_widget);
    }
}

static void pet_blank_callback(int state)
{
    debug_gtk3("called with EOI-blank %d.", state);
    if (get_model_func != NULL) {
        machine_model_widget_update(machine_widget);
    }
}


static void pet_io_callback(int state)
{
    debug_gtk3("called with IO size %d.", state);
    if (get_model_func != NULL) {
        machine_model_widget_update(machine_widget);
    }
}

static void pet_ram9_callback(int state)
{
    debug_gtk3("called with RAM9 type %d.", state);
    if (get_model_func != NULL) {
        machine_model_widget_update(machine_widget);
    }
}

static void pet_rama_callback(int state)
{
    debug_gtk3("called with RAMA type %d.", state);
    if (get_model_func != NULL) {
        machine_model_widget_update(machine_widget);
    }
}
/* }}} */


/* {{{ Plus4 glue logic and helpers */


static void plus4_debug_dump_resources(void)
{
#ifdef HAVE_DEBUG_GTK3UI
    int model = -1;
    int video = 0;
    int ram = 0;
    int hack = -1;
    const char *rom = NULL;

    const char *vidmodes[] = { "UNKNOWN", "PAL", "NTSC" };

    /* get model */
    if (get_model_func != NULL) {
        model = get_model_func();
    }

    /* get TED PAL/NTSC mode */
    if (resources_get_int("MachineVideoStandard", &video) < 0) {
        video = 0;
    }

    /* get RAM size */
    resources_get_int("RamSize", &ram);
    /* get memory exp hack */
    resources_get_int("MemoryHack", &hack);

    g_print("Plus4 resources dump:\n");
    g_print("    get_model_func()    : %d\n", model);
    g_print("    MachineVideoStandard: %d (%s)\n", video, vidmodes[video]);
    g_print("    RAM size            : %dKB\n", ram);
    g_print("    MemoryHack          : %d (%s)\n",
            hack,
            get_memhack_func != NULL ?
            get_memhack_func(hack) : "get_memhack_func not set");

    /* dump active ROMs */
    resources_get_string("KernalName", &rom);
    g_print("    KernalName          : %s\n", rom);
    resources_get_string("BasicName", &rom);
    g_print("    BasicName           : %s\n", rom);
    resources_get_string("FunctionLoWName", &rom);
    g_print("    FunctionLoWName     : %s\n", rom);
    resources_get_string("FunctionHighName", &rom);
    g_print("    FunctionHighName:   : %s\n", rom);
    resources_get_string("c1loName", &rom);
    g_print("    c1loName            : %s\n", rom);
    resources_get_string("c1hiName", &rom);
    g_print("    c1hiName            : %s\n", rom);
    resources_get_string("c2loName", &rom);
    g_print("    c2loName            : %s\n", rom);
    resources_get_string("c2hiName", &rom);
    g_print("    c2hiName            : %s\n", rom);

#endif
}


/** \brief  Extra calback for the Plus4 memory size widget
 *
 * Triggered when the widget changes value.
 *
 * \param[in,out]   widget      plus4 ram size widget
 * \param[in]       value       new size in KB
 */
static void plus4_mem_size_callback(GtkWidget *widget, int value)
{
    int size = 0;

    resources_get_int("RamSize", &size);
    debug_gtk3("Got new value: %dKB, RamSize = %d", value, size);
    debug_gtk3("Calling plus4_memory_expansion_widget_sync(): ");
    if (plus4_memory_expansion_widget_sync()) {
        debug_gtk3("OK.");
    } else {
        debug_gtk3("failed.");
    }
    machine_model_widget_update(machine_widget);
    plus4_debug_dump_resources();
}


/** \brief  Extra calback for the Plus4 memory expansion hack widget
 *
 * Triggered when the widget changes value.
 *
 * \param[in,out]   widget      plus4 memory expansion hack widget
 * \param[in]       value       new size in KB
 */
static void plus4_mem_hack_callback(GtkWidget *widget, int value)
{
    int size = 0;

    resources_get_int("RamSize", &size);
    debug_gtk3("Got new MemoryHack value: %d, RamSize = %d", value, size);

    plus4_memory_size_widget_sync();
    debug_gtk3("Setting RamSize widget sensitivity to %s",
            value == MEMORY_HACK_NONE ? "TRUE" : "FALSE");
    gtk_widget_set_sensitive(ram_widget, value == MEMORY_HACK_NONE);
    debug_gtk3("Calling plus4_memory_size_widget_sync(): ");
    if (plus4_memory_size_widget_sync()) {
        debug_gtk3("OK.");
    } else {
        debug_gtk3("failed.");
    }

    machine_model_widget_update(machine_widget);
    plus4_debug_dump_resources();
}

/* }}} */


/*
 * C64(sc) model change handling
 */
static void c64_misc_widget_sync(void);

static void machine_model_handler_c64(int model)
{
    GtkWidget *sid_group;

    debug_gtk3("Got model change for C64: %d", model);

    /* synchronize video chip widget */
    video_model_widget_update(video_widget);

    /* synchronize SID chip widget */
    sid_group = gtk_grid_get_child_at(GTK_GRID(sid_widget), 0, 1);
    if (sid_group != NULL) {
        vice_gtk3_resource_radiogroup_sync(sid_group);
    }

    /* synchronize CIA widget */
    cia_model_widget_sync(cia_widget);

    /* synchronize kernal-revision widget */
    kernal_revision_widget_sync(kernal_widget);
    /* synchronize misc widget */
    c64_misc_widget_sync();
}


/** \brief  Callback triggered on changing machine model
 *
 * \param[in]   model   machine model
 */
static void machine_model_handler_c128(int model)
{
    GtkWidget *sid_group;
#ifdef HAVE_DEBUG_GTK3UI
    int res_board_type = -1;
    int res_vdc_revision = -1;
    int res_vdc_64kb = -1;
    int res_machine_type = -1;
    int res_video_standard = -1;
    int res_cia1 = -1;
    int res_cia2 = -1;
    int res_sid = -1;

    debug_gtk3("Got model change for C128: %d.", model);

    resources_get_int("BoardType",      &res_board_type);
    resources_get_int("VDCRevision",    &res_vdc_revision);
    resources_get_int("VDC64KB",        &res_vdc_64kb);
    resources_get_int("MachineType",    &res_machine_type);
    resources_get_int("MachineVideoStandard",    &res_video_standard);
    resources_get_int("CIA1Model",      &res_cia1);
    resources_get_int("CIA2Model",      &res_cia2);
    resources_get_int("SIDModel",       &res_sid);

    printf("=== %s ===\n", __func__);
    printf("    BoardType             : %d\n", res_board_type);
    printf("    VDCRevision           : %d\n", res_vdc_revision);
    printf("    VDC64KB               : %d\n", res_vdc_64kb);
    printf("    MachineType           : %d\n", res_machine_type);
    printf("    MachineVideoStandard: : %d\n", res_video_standard);
    printf("    CIA1                  : %d\n", res_cia1);
    printf("    CIA2                  : %d\n", res_cia2);
    printf("    SIDModel              : %d\n", res_sid);
#endif

    /* sync video chip (VICIIe) widget */
    video_model_widget_update(video_widget);

    /* sync VDC widget */
    vdc_model_widget_update(vdc_widget);

    /* sync SID chip widget */
    sid_group = gtk_grid_get_child_at(GTK_GRID(sid_widget), 0, 1);
    if (sid_group != NULL) {
        vice_gtk3_resource_radiogroup_sync(sid_group);
    }

    /* synchronize CIA widget */
    cia_model_widget_sync(cia_widget);
}


/*
 * C64DTV widget glue logic
 */

/** \brief  Callback for the DTV revision
 *
 * Calls model widget update
 *
 * \param[in]   widget      radio button triggering the callback (unused)
 * \param[in]   revision    new DTV revision
 */
static void dtv_revision_callback(GtkWidget *widget, int revision)
{
    debug_gtk3("got revision %d.", revision);
    machine_model_widget_update(machine_widget);
}


/** \brief  Callback for the DTV VIC-II model (sync factor)
 *
 * Calls model widget update
 *
 * \param[in]   model   new VIC-II model
 */
static void dtv_video_callback(int model)
{
    debug_gtk3("got video model %d.", model);
    machine_model_widget_update(machine_widget);
}


/** \brief  Callback for DTV machine model changes
 *
 * Updates the DTV revision and VIC-II model widgets
 *
 * \param[in]   model   DTV model
 */
static void machine_model_handler_c64dtv(int model)
{
    int rev = 3;
    GtkWidget *group;

    switch (model) {
        case 0: /* V2 PAL */
            rev = 2;
            break;
        case 1: /* V2 NTSC */
            rev = 2;
            break;
        case 3: /* V3 NTSC */
            break;
        case 4: /* Hummer */
            break;
        default:
            /* 3: V3 PAL */
            break;
    }
    debug_gtk3("setting revision to %d", rev);

    /* update revision widget */
    group = gtk_grid_get_child_at(GTK_GRID(c64dtv_rev_widget), 0, 1);
    if (group != NULL && GTK_IS_GRID(group)) {
        vice_gtk3_resource_radiogroup_set(group, rev);
    }

    /* update VIC-II model widget */
    video_model_widget_update(video_widget);
}


/*
 * VIC-20 glue logic
 */


/** \brief  Callback for the VIC-2- VIC model (sync factor)
 *
 * Calls model widget update
 *
 * \param[in]   model   new VIC model
 */
static void vic20_video_callback(int model)
{
    debug_gtk3("got video model %d.", model);
    machine_model_widget_update(machine_widget);
}


/** \brief  Callback for VIC-20 machine model changes
 *
 * \param[in]   model   VIC-20 model
 */
static void machine_model_handler_vic20(int model)
{
    /* update VIC model widget */
    video_model_widget_update(video_widget);

    /* FIXME: */
    /* vic20_memory_expansion_widget_sync(); */
}


/*
 * Plus4 glue logic
 *
 * This logic won't appear to work very well due to the Plus4 model depending
 * on other resources that aren't presented in the model setting widgets, but
 * in separate widgets/dialogs such as KernalName, ACIA1Enable, Speech, etc.
 *
 * For example having the model 'Plus4 PAL' enabled and selecting NTSC won't
 * select the model 'Plus 4 NTSC', but 'Unknown' due to the NSTC Plus4 using
 * a different Kernal.
 */

/** \brief  Callback for the Plus4 TED model (sync factor)
 *
 * Calls model widget update
 *
 * \param[in]   model   new TED model
 */
static void plus4_video_callback(int model)
{
    debug_gtk3("got video model %d.", model);
    machine_model_widget_update(machine_widget);
    plus4_debug_dump_resources();
}

#if 0
/** \brief  Callback for the Plus4 memory size/hack
 *
 * Calls model widget update
 *
 * \param[in]   ram     ram size in KB
 * \param[in]   hack    memory hack type
 */
static void plus4_memory_callback(int ram, int hack)
{
    machine_model_widget_update(machine_widget);
}
#endif

static void machine_model_handler_plus4(int model)
{
    debug_gtk3("called with model %d.", model);
    video_model_widget_update(video_widget);
    plus4_debug_dump_resources();
}



/*
 * CBM-II glue logic
 */


/** \brief  Callback for the CBM-II 5x0 VIC-II model (sync factor)
 *
 * Calls model widget update
 *
 * \param[in]   model   new VIC-II model
 */
static void cbm5x0_video_callback(int model)
{
    debug_gtk3("got video model %d.", model);
    machine_model_widget_update(machine_widget);
}


/** \brief  Callback for the CBM-II 6x0/7x0 CRTC model (sync factor)
 *
 * Calls model widget update
 *
 * \param[in]   model   new VIC-II model
 */
static void cbm2_video_callback(int model)
{
    debug_gtk3("got video model %d.", model);
    machine_model_widget_update(machine_widget);
}


static void cbm2_switches_callback(GtkWidget *widget, int model_line)
{
    debug_gtk3("called with model_line %d.", model_line);
    machine_model_widget_update(machine_widget);
}


static void cbm2_memory_size_callback(GtkWidget *widget, int size)
{
    debug_gtk3("called with memory size %d.", size);
    machine_model_widget_update(machine_widget);
}


static void machine_model_handler_cbm5x0(int model)
{
    video_model_widget_update(video_widget);
    cbm2_memory_size_widget_update(ram_widget);
}


static void machine_model_handler_cbm6x0(int model)
{
    video_model_widget_update(video_widget);
    cbm2_memory_size_widget_update(ram_widget);
}


/** \brief  Set sensitivity of PET Ram9 and RamA widgets
 *
 * Only the 8296 model has the Ram9 and RamA resources
 */
static void pet_set_ram9a_sensitivity(void)
{
    gboolean model_is_8296;

    if (get_model_func != NULL) {
        int true_model = get_model_func();

        model_is_8296 = true_model == PETMODEL_8296;
        gtk_widget_set_sensitive(pet_ram9_widget, model_is_8296);
        gtk_widget_set_sensitive(pet_rama_widget, model_is_8296);
    }
}


static void machine_model_handler_pet(int model)
{
    debug_gtk3("Called.");
    pet_ram_size_widget_sync(ram_widget);
    pet_video_size_widget_sync(pet_video_size_widget);
    pet_keyboard_type_widget_sync(pet_keyboard_widget);
    pet_misc_widget_sync(pet_misc_widget);
    pet_io_size_widget_sync(pet_io_widget);
    pet_ram9_widget_sync(pet_ram9_widget);
    pet_rama_widget_sync(pet_rama_widget);
    pet_set_ram9a_sensitivity();
}



/** \brief  Generic callback for machine model changes
 *
 * \param[in]   model
 */
static void machine_model_callback(int model)
{
    debug_gtk3("got model %d.", model);

    switch (machine_class) {
        case VICE_MACHINE_C64:      /* fall through */
        case VICE_MACHINE_C64SC:    /* fall through */
        case VICE_MACHINE_SCPU64:
            machine_model_handler_c64(model);
            break;
        case VICE_MACHINE_C64DTV:
            machine_model_handler_c64dtv(model);
            break;
        case VICE_MACHINE_VIC20:
            machine_model_handler_vic20(model);
            break;
        case VICE_MACHINE_PLUS4:
            machine_model_handler_plus4(model);
            break;
        case VICE_MACHINE_CBM5x0:
            machine_model_handler_cbm5x0(model);
            break;
        case VICE_MACHINE_CBM6x0:
            machine_model_handler_cbm6x0(model);
            break;
        case VICE_MACHINE_PET:
            machine_model_handler_pet(model);
            break;
        case VICE_MACHINE_C128:
            machine_model_handler_c128(model);
            break;
        default:
            debug_gtk3("unsupported machine_class %d.", machine_class);
            break;
    }
}


/** \brief  Handler for the "toggled" event of the C64 Glue Logic radio buttons
 *
 * \param[in]   widget      radio button triggering the event
 * \param[in]   user_data   glue value (int)
 */
static void on_c64_glue_toggled(GtkWidget *widget, gpointer user_data)
{
    int glue = GPOINTER_TO_INT(user_data);

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
        debug_gtk3("setting GlueLogic to %s.",
                glue == 0 ? "discrete" : "252535-01");
        resources_set_int("GlueLogic", glue);
        machine_model_widget_update(machine_widget);
    }
}


/** \brief  Sync "Reset-to-IEC" widget with the associated resource
 *
 */
static void c64_reset_with_iec_sync(void)
{
    int iecreset = 0;
    resources_get_int("IECReset", &iecreset);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(reset_with_iec_widget), iecreset);
}

/** \brief  Create widget to toggle "Reset-to-IEC"
 *
 * \return  GtkGrid
 */
static GtkWidget *create_reset_with_iec_widget(void)
{
    reset_with_iec_widget = vice_gtk3_resource_check_button_new("IECReset",
            "Reset goes to IEC");
    g_signal_connect(GTK_WIDGET(reset_with_iec_widget), "toggled",
            G_CALLBACK(iec_callback), NULL);

    return reset_with_iec_widget;
}

/** \brief  Create widget to toggle "Go64Mode"
 *
 * \return  GtkGrid
 */
static GtkWidget *create_go64_widget(void)
{
    return vice_gtk3_resource_check_button_new("Go64Mode",
            "Always switch to C64 mode on reset");
}


GtkWidget *c64_discrete_radio = NULL;
GtkWidget *c64_custom_radio = NULL;

/** \brief  Sync "Glue Logic" widget with the associated resource
 *
 */
static void c64_glue_widget_sync(void)
{
    int glue;
    GtkWidget *radio;

    resources_get_int("GlueLogic", &glue);
    radio = (glue == 0) ? c64_discrete_radio : c64_custom_radio;
    if (radio) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio), TRUE);
    }
}

/** \brief  Create widget to select C64SC Glue Logic
 *
 * \return  GtkGrid
 */
static GtkWidget *create_c64_glue_widget(void)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *radio;
    GSList *group = NULL;

    int glue;

    resources_get_int("GlueLogic", &glue);

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);

    label = gtk_label_new("Glue logic");
    g_object_set(label, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

    c64_discrete_radio = gtk_radio_button_new_with_label(group, "Discrete");
    c64_custom_radio = gtk_radio_button_new_with_label(group, "Custom IC");
    gtk_radio_button_join_group(GTK_RADIO_BUTTON(c64_custom_radio),
            GTK_RADIO_BUTTON(c64_discrete_radio));

    radio = glue == 0 ? c64_discrete_radio : c64_custom_radio;
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio), TRUE);

    g_signal_connect(c64_discrete_radio, "toggled",
            G_CALLBACK(on_c64_glue_toggled), GINT_TO_POINTER(0));
    g_signal_connect(c64_custom_radio, "toggled",
            G_CALLBACK(on_c64_glue_toggled), GINT_TO_POINTER(1));

    gtk_grid_attach(GTK_GRID(grid), c64_discrete_radio, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), c64_custom_radio, 2, 0, 1, 1);


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

    iec_widget = create_reset_with_iec_widget();
    g_object_set(iec_widget, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), iec_widget, 0, 1, 1, 1);

    /*
     * GlueLogic seems to cause timing issues when set to 'custom' on x64, so
     * don't show the widget for x64 until it is fixed (if ever)
     */
    if (machine_class == VICE_MACHINE_C64SC ||
            machine_class == VICE_MACHINE_SCPU64) {
        glue_widget = create_c64_glue_widget();
        gtk_grid_attach(GTK_GRID(grid), glue_widget, 0, 2, 1, 1);
    }

    gtk_widget_show_all(grid);
    return grid;
}

static void c64_misc_widget_sync(void)
{
    c64_glue_widget_sync();
    c64_reset_with_iec_sync();
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


/** \brief  Create widget to select DTV revision
 *
 * Creates a grid with a label and a vice_gtk3_resource_radiogroup widget.
 *
 * \return  GtkGrid
 */
static GtkWidget *create_c64dtv_revision_widget(void)
{
    GtkWidget *grid;
    GtkWidget *group;
    GtkWidget *label;

    grid = vice_gtk3_grid_new_spaced(8, 8);
    g_object_set(G_OBJECT(grid), "margin-left", 8, NULL);

    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), "<b>DTV Revision</b>");
    gtk_widget_set_halign(label, GTK_ALIGN_START);

    group = vice_gtk3_resource_radiogroup_new("DtvRevision",
            c64dtv_revisions, GTK_ORIENTATION_VERTICAL);
    vice_gtk3_resource_radiogroup_add_callback(group, dtv_revision_callback);
    g_object_set(group, "margin-left", 16, NULL);

    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), group, 0, 1, 1, 1);

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
    video_model_widget_set_callback(video_widget, video_model_callback);
    gtk_grid_attach(GTK_GRID(grid), video_widget, 1, 0, 1, 1);

    /* SID widget */
    sid_widget = sid_model_widget_create(machine_widget);
    sid_model_widget_set_callback(sid_widget, sid_model_callback);
    gtk_grid_attach(GTK_GRID(grid), sid_widget, 1, 1, 1, 1);

    /* CIA1 & CIA2 widget */
    cia_widget = cia_model_widget_create(machine_widget, 2);
    cia_model_widget_set_callback(cia_widget, cia_model_callback);
    gtk_grid_attach(GTK_GRID(grid), cia_widget, 0, 2, 2, 1);

    /* Kernal revision widget */
    if (machine_class != VICE_MACHINE_SCPU64) {
        kernal_widget = kernal_revision_widget_create();
        gtk_grid_attach(GTK_GRID(grid), kernal_widget, 2, 0, 1, 1);
        /* add custom callback */
        kernal_revision_widget_add_callback(kernal_revision_callback);
    }

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
    video_model_widget_set_callback(video_widget, video_model_callback);
    gtk_grid_attach(GTK_GRID(video_wrapper), video_widget, 0, 0, 1, 1);

    /* VDC model widget */
    vdc_widget = vdc_model_widget_create();
    /* XXX: I'm sure I had/have a reason for this: */
    vdc_model_widget_connect_signals(vdc_widget);
    vdc_model_widget_set_revision_callback(vdc_widget, vdc_revision_callback);
    vdc_model_widget_set_ram_callback(vdc_widget, vdc_ram_callback);
    /* align with other widgets */
    g_object_set(vdc_widget, "margin-left", 8, NULL);
    gtk_grid_attach(GTK_GRID(video_wrapper), vdc_widget, 0, 1, 1, 1);

    /* CIA1 & CIA2 widget */
    cia_widget = cia_model_widget_create(machine_widget, 2);
    cia_model_widget_set_callback(cia_widget, cia_model_callback);
    gtk_grid_attach(GTK_GRID(video_wrapper), cia_widget, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), video_wrapper, 1, 0, 1, 1);

    /* SID widget */
    sid_widget = sid_model_widget_create(machine_widget);
    sid_model_widget_set_callback(sid_widget, sid_model_callback);
    gtk_grid_attach(GTK_GRID(grid), sid_widget, 2, 0, 1, 1);

    /* Misc widget */
    gtk_grid_attach(GTK_GRID(grid), create_c128_misc_widget(), 0, 1, 3, 1);
    return grid;
}



/** \brief  Create C64DTV model settings widget layout
 *
 * \param[in]   grid    GtkGrid to attach widgets to
 *
 * \return  \a grid
 */
static GtkWidget *create_c64dtv_layout(GtkWidget *grid)
{
    GtkWidget *luma_widget;

    /** add machine widget */
    gtk_grid_attach(GTK_GRID(grid), machine_widget, 0, 0, 1, 2);

    /* add video widget */
    video_widget = video_model_widget_create(machine_widget);
    video_model_widget_set_callback(video_widget, dtv_video_callback);
    gtk_grid_attach(GTK_GRID(grid), video_widget, 1, 0, 1, 1);

    /* create revision widget */
    c64dtv_rev_widget = create_c64dtv_revision_widget();
    gtk_grid_attach(GTK_GRID(grid), c64dtv_rev_widget, 1, 1, 1, 1);

    /* SID widget */
    sid_widget = sid_model_widget_create(machine_widget);
    sid_model_widget_set_callback(sid_widget, sid_model_callback);
    g_object_set(G_OBJECT(sid_widget), "margin-left", 8, NULL);
    gtk_grid_attach(GTK_GRID(grid), sid_widget, 0, 2, 1, 1);

    /* Luma fix widget */
    luma_widget = vice_gtk3_resource_check_button_new("VICIINewLuminances",
            "Enable LumaFix (use new VICII luminances)");
    g_object_set(G_OBJECT(luma_widget), "margin-left", 8, "margin-top", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), luma_widget, 0, 3, 2, 1);
    return grid;
}


/** \brief  Create VIC20 model settings widget layout
 *
 * \param[in]   grid    GtkGrid to attach widgets to
 *
 * \return  \a grid
 */
static GtkWidget *create_vic20_layout(GtkWidget *grid)
{
    /* add machine widget */
    gtk_grid_attach(GTK_GRID(grid), machine_widget, 0, 0, 1, 1);

    /* VIC model widget */
    video_widget = video_model_widget_create(machine_widget);
    video_model_widget_set_callback(video_widget, vic20_video_callback);
    gtk_grid_attach(GTK_GRID(grid), video_widget, 1, 0, 1, 1);

    ram_widget = vic20_memory_expansion_widget_create();
    gtk_grid_attach(GTK_GRID(grid), ram_widget, 0, 1, 2, 1);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create Plus4 model settings widget layout
 *
 * \param[in]   grid    GtkGrid to attach widgets to
 *
 * \return  \a grid
 */
static GtkWidget *create_plus4_layout(GtkWidget *grid)
{
    int hack;

    /* add machine widget */
    gtk_grid_attach(GTK_GRID(grid), machine_widget, 0, 0, 1, 1);

    /* Plus4 model widget */
    video_widget = video_model_widget_create(machine_widget);
    video_model_widget_set_callback(video_widget, plus4_video_callback);
    gtk_grid_attach(GTK_GRID(grid), video_widget, 1, 0, 1, 1);

    /* memory expansion hacks */
    memhack_widget = plus4_memory_expansion_widget_create();
    plus4_memory_expansion_widget_add_callback(plus4_mem_hack_callback);
    gtk_grid_attach(GTK_GRID(grid), memhack_widget, 0, 1, 1, 1);

    /* memory size */
    ram_widget = plus4_memory_size_widget_create();
    plus4_memory_size_widget_add_callback(plus4_mem_size_callback);
    gtk_grid_attach(GTK_GRID(grid), ram_widget, 1, 1, 1, 1);

    resources_get_int("MemoryHack", &hack);
    gtk_widget_set_sensitive(memhack_widget, hack == MEMORY_HACK_NONE);

    acia_widget = plus4_acia_widget_create(machine_widget);
    gtk_grid_attach(GTK_GRID(grid), acia_widget, 0, 2, 1, 1);

    speech_widget = v364_speech_widget_create(machine_widget);
    gtk_grid_attach(GTK_GRID(grid), speech_widget, 1, 2, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Creat PET layout
 *
 * Uses a GtkStack and GtkStackSwitcher to reduce vertical space
 *
 * \return  \a grid
 */
static GtkWidget *create_pet_layout(GtkWidget *grid)
{
    GtkWidget *stack;
    GtkWidget *switcher;
    GtkWidget *pet_grid;
    GtkWidget *superpet_grid;

    pet_grid = gtk_grid_new();

    /* PET model list */
    gtk_grid_attach(GTK_GRID(pet_grid),
            machine_widget,
            0, 0, 1, 3);

    pet_keyboard_widget = pet_keyboard_type_widget_create();
    pet_keyboard_type_widget_set_callback(pet_keyboard_widget,
                                          pet_keyboard_type_callback);
    gtk_grid_attach(GTK_GRID(pet_grid), pet_keyboard_widget, 1, 0, 1, 1);

    pet_video_size_widget = pet_video_size_widget_create();
    pet_video_size_widget_set_callback(pet_video_size_widget, pet_video_size_callback);
    gtk_grid_attach(GTK_GRID(pet_grid), pet_video_size_widget, 1, 1, 1, 1);

    ram_widget = pet_ram_size_widget_create();
    pet_ram_size_widget_set_callback(ram_widget, pet_ram_size_callback);
    gtk_grid_attach(GTK_GRID(pet_grid), ram_widget, 2, 0, 1, 1);

    pet_io_widget = pet_io_size_widget_create();
    pet_io_size_widget_set_callback(pet_io_widget, pet_io_callback);
    gtk_grid_attach(GTK_GRID(pet_grid), pet_io_widget, 2, 1, 1, 1);

    pet_ram9_widget = pet_ram9_widget_create();
    pet_ram9_widget_set_callback(pet_ram9_widget, pet_ram9_callback);
    gtk_grid_attach(GTK_GRID(pet_grid), pet_ram9_widget, 3, 0, 1, 1);

    pet_rama_widget = pet_rama_widget_create();
    pet_rama_widget_set_callback(pet_rama_widget, pet_rama_callback);
    pet_set_ram9a_sensitivity();
    gtk_grid_attach(GTK_GRID(pet_grid), pet_rama_widget, 3, 1, 1, 1);

    pet_misc_widget = pet_misc_widget_create();
    pet_misc_widget_set_crtc_callback(pet_misc_widget, pet_crtc_callback);
    pet_misc_widget_set_blank_callback(pet_misc_widget, pet_blank_callback);
    gtk_grid_attach(GTK_GRID(pet_grid), pet_misc_widget, 1, 2, 2, 1);

    /* SuperPET widgets */
    superpet_grid = superpet_widget_create();

    stack = gtk_stack_new();

    gtk_stack_add_titled(GTK_STACK(stack), pet_grid, "PET", "PET");
    gtk_stack_add_titled(GTK_STACK(stack), superpet_grid, "SuperPET", "SuperPET");
    gtk_stack_set_transition_type(GTK_STACK(stack),
            GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    gtk_stack_set_transition_duration(GTK_STACK(stack), 1000);
    gtk_stack_set_homogeneous(GTK_STACK(stack), TRUE);

    switcher = gtk_stack_switcher_new();
    gtk_stack_switcher_set_stack(GTK_STACK_SWITCHER(switcher), GTK_STACK(stack));
    gtk_widget_set_halign(switcher, GTK_ALIGN_CENTER);
    gtk_orientable_set_orientation(GTK_ORIENTABLE(switcher),
            GTK_ORIENTATION_HORIZONTAL);

    gtk_widget_show_all(stack);
    gtk_widget_show_all(switcher);

    gtk_stack_set_visible_child_name(GTK_STACK(stack), "PET");
    gtk_grid_attach(GTK_GRID(grid), switcher, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), stack, 0, 1, 1, 1);

    gtk_widget_show_all(grid);
    debug_gtk3("Done creating PET model dialog");
    return grid;
}


/** \brief  Create CBM-II/5x0 model settings widget layout
 *
 * \param[in]   grid    GtkGrid to attach widgets to
 *
 * \return  \a grid
 */
static GtkWidget *create_cbm5x0_layout(GtkWidget *grid)
{
    GtkWidget *switches_widget;
    GtkWidget *bank15_widget;

    /* add machine widget */
    gtk_grid_attach(GTK_GRID(grid), machine_widget, 0, 0, 1, 1);

    /* add video widget */
    video_widget = video_model_widget_create(machine_widget);
    video_model_widget_set_callback(video_widget, cbm5x0_video_callback);
    gtk_grid_attach(GTK_GRID(grid), video_widget, 1, 0, 1, 1);

    /* SID widget */
    sid_widget = sid_model_widget_create(machine_widget);
    sid_model_widget_set_callback(sid_widget, sid_model_callback);
    gtk_grid_attach(GTK_GRID(grid), sid_widget, 1, 1, 1, 1);

    /* CIA1 widget */
    cia_widget = cia_model_widget_create(machine_widget, 1);
    cia_model_widget_set_callback(cia_widget, cia_model_callback);
    gtk_grid_attach(GTK_GRID(grid), cia_widget, 2, 0, 1, 1);

    /* RAM size widget */
    ram_widget = cbm2_memory_size_widget_create();
    cbm2_memory_size_widget_set_callback(ram_widget, cbm2_memory_size_callback);
    gtk_grid_attach(GTK_GRID(grid), ram_widget, 0, 1, 1, 1);

    /* Hardwired I/O port model switches */
    switches_widget = cbm2_hardwired_switches_widget_create();
    cbm2_hardwired_switches_widget_set_callback(switches_widget,
            cbm2_switches_callback);
    gtk_grid_attach(GTK_GRID(grid), switches_widget, 2, 1, 1, 1);

    /* Mapping RAM into bank 15 */
    bank15_widget = cbm2_ram_mapping_widget_create();
    gtk_grid_attach(GTK_GRID(grid), bank15_widget, 0, 2, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create CBM-II/6x0-7x0 model settings widget layout
 *
 * \param[in]   grid    GtkGrid to attach widgets to
 *
 * \return  \a grid
 */
static GtkWidget *create_cbm6x0_layout(GtkWidget *grid)
{
    GtkWidget *switches_widget;
    GtkWidget *bank15_widget;

    /* add video widget */
    video_widget = video_model_widget_create(machine_widget);
    video_model_widget_set_callback(video_widget, cbm2_video_callback);
    gtk_grid_attach(GTK_GRID(grid), video_widget, 0, 3, 1, 1);

    /* add machine widget */
    gtk_grid_attach(GTK_GRID(grid), machine_widget, 0, 0, 1, 2);

    /* SID widget */
    sid_widget = sid_model_widget_create(machine_widget);
    sid_model_widget_set_callback(sid_widget, sid_model_callback);
    gtk_grid_attach(GTK_GRID(grid), sid_widget, 1, 0, 1, 1);

    /* Hardwired I/O port model switches */
    switches_widget = cbm2_hardwired_switches_widget_create();
    cbm2_hardwired_switches_widget_set_callback(switches_widget,
            cbm2_switches_callback);
    gtk_grid_attach(GTK_GRID(grid), switches_widget, 2, 0, 1, 1);

    /* CIA1 widget */
    cia_widget = cia_model_widget_create(machine_widget, 1);
    cia_model_widget_set_callback(cia_widget, cia_model_callback);
    gtk_grid_attach(GTK_GRID(grid), cia_widget, 1, 1, 2, 1);

    /* RAM size widget */
    ram_widget = cbm2_memory_size_widget_create();
    cbm2_memory_size_widget_set_callback(ram_widget, cbm2_memory_size_callback);
    gtk_grid_attach(GTK_GRID(grid), ram_widget, 0, 2, 1, 1);

    /* Mapping RAM into bank 15 */
    bank15_widget = cbm2_ram_mapping_widget_create();
    gtk_grid_attach(GTK_GRID(grid), bank15_widget, 1, 2, 2, 1);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create VSID layout
 *
 * \param[in,out]   grid    GtkGrid to use for layout
 *
 * \return  \a grid
 */
static GtkWidget *create_vsid_layout(GtkWidget *grid)
{
    /* VIC-II model widget */
    video_widget = video_model_widget_create(machine_widget);
    gtk_grid_attach(GTK_GRID(grid), video_widget, 1, 0, 1, 1);

    sid_widget = sid_model_widget_create(machine_widget);
    gtk_grid_attach(GTK_GRID(grid), sid_widget, 0, 0, 1, 1);
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
            archdep_vice_exit(1);
            return NULL;    /* unlike with ordinary exit(), GCC doesn't see
                               this will never be reached */
    }
}


/** \brief  Create 'Model' widget for the settings UI
 *
 * \param[in]   parent  parent widget
 *
 * \return  GtkGrid
 */
GtkWidget *settings_model_widget_create(GtkWidget *parent)
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
    if ((machine_class != VICE_MACHINE_PET)) {
        /*
         * PET only has a simple CRTC, so no video widget used
         * (Is this still valid? I had to remove the cbm2 from this branch)
         */
        video_model_widget_connect_signals(video_widget);
    }

    /* add callback */
    machine_model_widget_set_callback(machine_model_callback);

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


/** \brief  Set function pointer to function that determines if the model
 *          settings indicate a valid model
 *
 * \param[in]   func    function pointer
 */
void settings_model_widget_set_model_func(int (*func)(void))
{
    get_model_func = func;
}


/** \brief  Set function to get a memory hack description
 *
 * \param[in]   func    function to get a string for a memhack ID (int)
 */
void settings_model_widget_set_memhack_func(const char *(func)(int))
{
    get_memhack_func = func;
}
