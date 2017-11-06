/** \file   src/arch/gtk3/widgets/ioextensionswidget.c
 * \brief   I/O extension settings that don't go/fit into subwidgets
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *  IOCollisionHandling (all except vsid)
 *  CartridgeReset (all except vsid)
 *  SSRamExpansion (x64/x64sc/xscpu64/x128)
 *  SFXSoundSampler (x64/x64sc/xscpu64/x128)
 *  CPMCart (x64/x64sc)
 *  C128FullBanks (x128)
 *  FinalExpansionWriteBack (xvic)
 *  VicFlashPluginWriteBack (xvic)
 *  UltiMemWriteBack (xvic)
 *  IO2RAM (xvic)
 *  IO3RAM (xvic)
 *  VFLImod (xvic)
 *  IEEE488 (xvic)
 *  Acia1Enable (xplus4)
 *  DIGIBLASTER (xplus4)
 *  UserportDAC (xplus4, xpet)
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
#include <gtk/gtk.h>

#include "machine.h"
#include "resources.h"
#include "debug_gtk3.h"
#include "basewidgets.h"
#include "widgethelpers.h"
#include "basedialogs.h"
#include "cartio.h"
#include "cartridge.h"

#include "ioextensionswidget.h"


/** \brief  Methods of handling I/O collisions
 */
static ui_radiogroup_entry_t io_collision_methods[] = {
    { "Detach all", IO_COLLISION_METHOD_DETACH_ALL },
    { "Detach last", IO_COLLISION_METHOD_DETACH_LAST },
    { "AND values", IO_COLLISION_METHOD_AND_WIRES },
    { NULL, -1 }
};


/** \brief  Create widget to specify I/O collision handling method
 *
 * The \a desc is added as a label under the "I/O collision handling" label,
 * in italics and surrounded with parenthesis.
 *
 * \param[in]   desc    description of the I/O areas involved
 *
 * \return  GtkGrid
 */
static GtkWidget *create_collision_widget(const char *desc)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *group;
    char buffer[256];

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 0);

    label = gtk_label_new("I/O collision handling");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    g_object_set(label, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

    group = resource_radiogroup_create("IOCollisionHandling",
            io_collision_methods, GTK_ORIENTATION_HORIZONTAL);
    gtk_grid_set_column_spacing(GTK_GRID(group), 16);
    gtk_grid_attach(GTK_GRID(grid), group, 1, 0, 1, 1);

    label = gtk_label_new(NULL);
    g_snprintf(buffer, 256, "<i>(%s)</i>", desc);
    gtk_label_set_markup(GTK_LABEL(label), buffer);

    gtk_widget_set_halign(label, GTK_ALIGN_START);
    g_object_set(label, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);


    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create check button for "reset-on-cart-change"
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_cart_reset_widget(void)
{
    GtkWidget *check;

    check = resource_check_button_create("CartridgeReset",
            "Reset machine on cartridge change");
    g_object_set(check, "margin-left", 16, NULL);
    return check;
}


/** \brief  Create check button for Supersnapshot 32KB expansion
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_supersnapshot_widget(void)
{
    GtkWidget *check;

    check = resource_check_button_create("SSRamExpansion",
            "Enable 32KB Super Snapshot RAM expansion");
    g_object_set(check, "margin-left", 16, NULL);
    return check;
}


/** \brief  Create check button for SFX Sound Sampler
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_sfx_sound_sampler_widget(void)
{
    GtkWidget *check;

    check = resource_check_button_create("SFXSoundSampler",
            "Enable SFX Sound Sampler");
    g_object_set(check, "margin-left", 16, NULL);
    return check;
}


/** \brief  Create check button for CP/M Cartridge
 *
 * For x64/x64sc, NOT xscu64 or x128, or any others
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_cpm_cartridge_widget(void)
{
    GtkWidget *check;

    check = resource_check_button_create("CPMCart",
            "Enable CP/M Cartridge");
    g_object_set(check, "margin-left", 16, NULL);
    return check;
}


/** \brief  Create check button to select C128 full banks
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_c128_full_banks_widget(void)
{
    GtkWidget *check;

    check = resource_check_button_create("C128FullBanks",
            "Enable RAM banks 2 & 3");
    g_object_set(check, "margin-left", 16, NULL);
    return check;
}


/** \brief  Create check button to enable Final Expansion write back
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_final_expansion_writeback_widget(void)
{
    GtkWidget *check;

    check = resource_check_button_create("FinalExpansionWriteBack",
            "Enable Final Expansion image write back");
    g_object_set(check, "margin-left", 16, NULL);
    return check;
}


/** \brief  Create check button to enable Vic Flash write back
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_vic_flash_writeback_widget(void)
{
    GtkWidget *check;

    check = resource_check_button_create("VicFlashPluginWriteBack",
            "Enable Vic Flash image write back");
    g_object_set(check, "margin-left", 16, NULL);
    return check;
}


/** \brief  Create check button to enable UltiMem write back
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_ultimem_writeback_widget(void)
{
    GtkWidget *check;

    check = resource_check_button_create("UltiMemWriteBack",
            "Enable UltiMem image write back");
    g_object_set(check, "margin-left", 16, NULL);
    return check;
}


/** \brief  Create check button to enable VIC-1112 IEEE-488 interface
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_vic_ieee488_widget(void)
{
    GtkWidget *check;

    check = resource_check_button_create("IEEE488",
            "Enable VIC-1112 IEEE-488 interface");
    g_object_set(check, "margin-left", 16, NULL);
    return check;
}


/** \brief  Create check button to enable VIC-20 I/O-2 RAM Cartridge
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_vic_io2ram_widget(void)
{
    GtkWidget *check;

    check = resource_check_button_create("IO2RAM",
            "Enable I/O-2 RAM Cartridge ($9800-$9BFF)");
    g_object_set(check, "margin-left", 16, NULL);
    return check;
}


/** \brief  Create check button to enable VIC-20 I/O-3 RAM Cartridge
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_vic_io3ram_widget(void)
{
    GtkWidget *check;

    check = resource_check_button_create("IO3RAM",
            "Enable I/O-3 RAM Cartridge ($9C00-$9FFF)");
    g_object_set(check, "margin-left", 16, NULL);
    return check;
}


/** \brief  Create check button to enable VIC-20 VFLI modification
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_vic_vfli_widget(void)
{
    GtkWidget *check;

    check = resource_check_button_create("VFLImod",
            "Enable VFLI modification");
    g_object_set(check, "margin-left", 16, NULL);
    return check;
}


/** \brief  Create check button to enable Plus4 ACIA
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_plus4_acia_widget(void)
{
    GtkWidget *check;

    check = resource_check_button_create("Acia1Enable",
            "Enable ACIA");
    g_object_set(check, "margin-left", 16, NULL);
    return check;
}


/** \brief  Create check button to enable Plus4 DigiBlaster
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_plus4_digiblaster_widget(void)
{
    GtkWidget *check;

    check = resource_check_button_create("DIGIBLASTER",
            "Enable DigiBlaster add-on");
    g_object_set(check, "margin-left", 16, NULL);
    return check;
}


/** \brief  Create check button to enable Plus4 Userport 8-bit DAC
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_plus4_8bitdac_widget(void)
{
    GtkWidget *check;

    check = resource_check_button_create("UserportDAC",
            "Enable Userport 8-bit DAC");
    g_object_set(check, "margin-left", 16, NULL);
    return check;
}


/** \brief  Create check button to enable PET HRE hires
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_pet_hre_widget(void)
{
    GtkWidget *check;

    check = resource_check_button_create("PETHRE",
            "Enable HRE hi-res graphics");
    g_object_set(check, "margin-left", 16, NULL);
    return check;
}


/** \brief  Create check button to enable PET diagnostic pin
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_pet_diagnostic_widget(void)
{
    GtkWidget *check;

    check = resource_check_button_create("DiagPin",
            "Enable userport diagnostic pin");
    g_object_set(check, "margin-left", 16, NULL);
    return check;
}


/** \brief  Create layout for x64/x64sc
 *
 * \param[in,out]   grid    grid to add widgets to
 */
static void create_c64_layout(GtkWidget *grid)
{
    GtkWidget *collision_widget;

    collision_widget = create_collision_widget("$D000-$DFFF");
    gtk_grid_attach(GTK_GRID(grid), collision_widget, 0, 1, 3, 1);
    gtk_grid_attach(GTK_GRID(grid), create_cart_reset_widget(), 0, 2, 3, 1);
    gtk_grid_attach(GTK_GRID(grid), create_supersnapshot_widget(), 0, 3, 3, 1);
    gtk_grid_attach(GTK_GRID(grid), create_sfx_sound_sampler_widget(), 0, 4, 3, 1);
    gtk_grid_attach(GTK_GRID(grid), create_cpm_cartridge_widget(), 0, 5, 3, 1);
}


/** \brief  Create layout for xscpu64
 *
 * \param[in,out]   grid    grid to add widgets to
 */
static void create_scpu64_layout(GtkWidget *grid)
{
    GtkWidget *collision_widget;

    collision_widget = create_collision_widget("$D000-$DFFF");
    gtk_grid_attach(GTK_GRID(grid), collision_widget, 0, 1, 3, 1);
    gtk_grid_attach(GTK_GRID(grid), create_cart_reset_widget(), 0, 2, 3, 1);
    gtk_grid_attach(GTK_GRID(grid), create_supersnapshot_widget(), 0, 3, 3, 1);
    gtk_grid_attach(GTK_GRID(grid), create_sfx_sound_sampler_widget(), 0, 4, 3, 1);
}


/** \brief  Create layout for x128
 *
 * \param[in,out]   grid    grid to add widgets to
 */
static void create_c128_layout(GtkWidget *grid)
{
    GtkWidget *collision_widget;

    collision_widget = create_collision_widget("$D000-$DFFF");
    gtk_grid_attach(GTK_GRID(grid), collision_widget, 0, 1, 3, 1);
    gtk_grid_attach(GTK_GRID(grid), create_cart_reset_widget(), 0, 2, 3, 1);
    gtk_grid_attach(GTK_GRID(grid), create_c128_full_banks_widget(), 0, 3, 3, 1);
    gtk_grid_attach(GTK_GRID(grid), create_supersnapshot_widget(), 0, 4, 3, 1);
    gtk_grid_attach(GTK_GRID(grid), create_sfx_sound_sampler_widget(), 0, 5, 3, 1);
}



/** \brief  Create layout for x64dtv
 *
 * \param[in,out]   grid    grid to add widgets to
 */
static void create_c64dtv_layout(GtkWidget *grid)
{
    GtkWidget *hummer;

    hummer = resource_check_button_create("HummerADC", "Enable Hummer ADC");
    g_object_set(hummer, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), hummer, 0, 1, 3, 1);
}


/** \brief  Create layout for xvic
 *
 * \param[in,out]   grid    grid to add widgets to
 */
static void create_vic20_layout(GtkWidget *grid)
{
    GtkWidget *collision_widget;

    collision_widget = create_collision_widget("$9000-$93FF, $9800-$9FFF");
    gtk_grid_attach(GTK_GRID(grid), collision_widget, 0, 1, 3, 1);
    gtk_grid_attach(GTK_GRID(grid), create_cart_reset_widget(), 0, 2, 3, 1);
    gtk_grid_attach(GTK_GRID(grid), create_final_expansion_writeback_widget(),
            0, 3, 3, 1);
    gtk_grid_attach(GTK_GRID(grid), create_vic_flash_writeback_widget(),
            0, 4, 3, 1);
    gtk_grid_attach(GTK_GRID(grid), create_ultimem_writeback_widget(),
            0, 5, 3, 1);
    gtk_grid_attach(GTK_GRID(grid), create_vic_ieee488_widget(),
            0, 6, 3, 1);
    gtk_grid_attach(GTK_GRID(grid), create_vic_io2ram_widget(),
            0, 7, 3, 1);
    gtk_grid_attach(GTK_GRID(grid), create_vic_io3ram_widget(),
            0, 8, 3, 1);
    gtk_grid_attach(GTK_GRID(grid), create_vic_vfli_widget(),
            0, 9, 3, 1);

}


/** \brief  Create layout for xplus4
 *
 * \param[in,out]   grid    grid to add widgets to
 */
static void create_plus4_layout(GtkWidget *grid)
{
    GtkWidget *collision_widget;

    collision_widget = create_collision_widget("$FD00-$FEFF");
    gtk_grid_attach(GTK_GRID(grid), collision_widget, 0, 1, 3, 1);
    gtk_grid_attach(GTK_GRID(grid), create_cart_reset_widget(), 0, 2, 3, 1);
    gtk_grid_attach(GTK_GRID(grid), create_plus4_acia_widget(), 0, 3, 3, 1);
    gtk_grid_attach(GTK_GRID(grid), create_plus4_digiblaster_widget(),
            0, 4, 3, 1);
    gtk_grid_attach(GTK_GRID(grid), create_plus4_8bitdac_widget(),
            0, 5, 3, 1);

}


/** \brief  Create layout for xpet
 *
 * \param[in,out]   grid    grid to add widgets to
 */
static void create_pet_layout(GtkWidget *grid)
{
    GtkWidget *collision_widget;

    collision_widget = create_collision_widget("$8800-$8FFF, $E900-$EEFF");
    gtk_grid_attach(GTK_GRID(grid), collision_widget, 0, 1, 3, 1);
    gtk_grid_attach(GTK_GRID(grid), create_pet_hre_widget(), 0, 2, 3, 1);
    gtk_grid_attach(GTK_GRID(grid), create_pet_diagnostic_widget(), 0, 3, 3, 1);
}


/** \brief  Create widget for generic I/O extension settings
 *
 * Creates a widget for generic I/O extension settings and settings that don't
 * make sense to create a separate widget for (ie stuff that has a single
 * bool resource or a single "open file" dialog").
 *
 * \param[in]   parent  parent widget
 *
 * \return GtkGrid
 */
GtkWidget *ioextensions_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;

    grid = uihelpers_create_grid_with_label("Generic I/O extension settings", 3);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 16);

    switch (machine_class) {

        case VICE_MACHINE_C64:      /* fall through */
        case VICE_MACHINE_C64SC:
            create_c64_layout(grid);
            break;

        case VICE_MACHINE_SCPU64:
            create_scpu64_layout(grid);
            break;

        case VICE_MACHINE_C128:
            create_c128_layout(grid);
            break;

        case VICE_MACHINE_C64DTV:
            create_c64dtv_layout(grid);
            break;

        case VICE_MACHINE_VIC20:
            create_vic20_layout(grid);
            break;

        case VICE_MACHINE_PLUS4:
            create_plus4_layout(grid);
            break;

        case VICE_MACHINE_PET:
            create_pet_layout(grid);
            break;

        default:
            /* NOP */
            break;
    }

    gtk_widget_show_all(grid);
    return grid;
}
