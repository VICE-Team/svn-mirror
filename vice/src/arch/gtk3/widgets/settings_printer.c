/** \file   settings_printer.c
 * \brief   Widget to control printer settings
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/* FIXME:   Some of the resources mentioned here are actually controlled by
 *          widgets in other files:
 *
 * $VICERES VirtualDevice4              -vsid
 * $VICERES VirtualDevice5              -vsid
 * $VICERES VirtualDevice6              -vsid
 * $VICERES VirtualDevice7              -vsid
 * $VICERES IECDevice4                  -vsid -xvic
 * $VICERES IECDevice5                  -vsid -xvic
 * $VICERES IECDevice6                  -vsid -xvic
 * $VICERES IECDevice7                  -vsid -xvic
 * $VICERES Printer4                    -vsid
 * $VICERES Printer5                    -vsid
 * $VICERES Printer6                    -vsid
 * $VICERES Printer7                    -vsid
 * $VICERES Printer4Driver              -vsid
 * $VICERES Printer5Driver              -vsid
 * $VICERES Printer6Driver              -vsid
 * $VICERES Printer4Output              -vsid
 * $VICERES Printer5Output              -vsid
 * $VICERES Printer6Output              -vsid
 * $VICERES PrinterTextDevice1          -vsid
 * $VICERES PrinterTextDevice2          -vsid
 * $VICERES PrinterTextDevice3          -vsid
 * $VICERES Printer4TextDevice          -vsid
 * $VICERES Printer5TextDevice          -vsid
 * $VICERES Printer6TextDevice          -vsid
 * $VICERES Printer7TextDevice          -vsid
 * $VICERES PrinterUserport             -vsid
 * $VICERES PrinterUserportTextDevice   -vsid
 * $VICERES PrinterUserportDriver       -vsid
 * $VICERES PrinterUserportOutput       -vsid
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
#include <stdbool.h>

#include "vice_gtk3.h"
#include "debug_gtk3.h"
#include "resources.h"
#include "machine.h"
#include "printer.h"

/* widgets */
#include "printeremulationtypewidget.h"
#include "printerdriverwidget.h"
#include "printeroutputmodewidget.h"
#include "printeroutputdevicewidget.h"
#include "userportprinterwidget.h"

#include "settings_printer.h"


#define PRINTER_NUM 4   /**< number of printer devices supported */
#define PRINTER_MIN 4   /**< lowest device number for a printer */
#define PRINTER_MAX 7   /**< highest device number for a printer */


/** \brief  GtkStack child data */
typedef struct child_s {
    const char *title;          /**< stack child title */
    const char *name;           /**< stack child name */
    bool        has_type;       /**< can set emulation type */
    bool        has_virtdev;    /**< virtual device support */
    bool        has_iec;        /**< can have IEC device support (depends on machine) */
    bool        has_formfeed;   /**< can send FF */
    bool        has_realdev;    /**< real device (OpenCBM) support */
    bool        has_driver;     /**< has driver selection */
    bool        has_outmode;    /**< has output mode selection */
    bool        has_outdev;     /**< has output device selection */
} child_t;

/** \brief  Stack child widget info */
static const child_t children[] = {
    {
        .title        = "Printer 4",
        .name         = "printer4",
        .has_type     = true,
        .has_virtdev  = true,
        .has_iec      = true,
        .has_formfeed = true,
        .has_driver   = true,
        .has_outmode  = true,
        .has_outdev   = true
    },
    {
        .title        = "Printer 5",
        .name         = "printer5",
        .has_type     = true,
        .has_virtdev  = true,
        .has_iec      = true,
        .has_formfeed = true,
        .has_driver   = true,
        .has_outmode  = true,
        .has_outdev   = true
    },
    {
        .title        = "Plotter 6",
        .name         = "plotter6",
        .has_type     = true,
        .has_virtdev  = true,
        .has_iec      = true,
        .has_formfeed = true,
        .has_driver   = true,
        .has_outmode  = true,
        .has_outdev   = true,
    },
    {
        .title        = "OpenCBM 7",
        .name         = "opencbm7",
        .has_virtdev  = true,
        .has_iec      = true,
        .has_realdev  = true,
    },
    {
        .title        = "Userport",
        .name         = "userport",
        .has_formfeed = true
    }
};

/** \brief  Current machine has IEC bus support
 *
 * \return  `true` if machine supportS IEC
 *
 * \note    Returns `false` for xvic due to xvic having its own iecbus code.
 */
static bool machine_has_iec(void)
{
    switch (machine_class) {
        /* these machines have IEC */
        case VICE_MACHINE_C64:      /* fall through */
        case VICE_MACHINE_C64SC:    /* fall through */
        case VICE_MACHINE_SCPU64:   /* fall through */
        case VICE_MACHINE_C128:     /* fall through */
        case VICE_MACHINE_C64DTV:   /* fall through */
#if 0
        /* FIXME: xvic does not use the generic IEC bus code in src/iecbus/iecbus.c yet */
        case VICE_MACHINE_VIC20:    /* fall through */
#endif
        case VICE_MACHINE_PLUS4:
            return true;

        default:
            /* No IEC */
            return false;
    }
}

/** \brief  Current machine has userport
 *
 * \return  `true` if userport is capable of supporting printers/plotters
 *
 * \note    Returns `false` for Plus/4 although it has a userport.
 */
static bool machine_has_userport(void)
{
    switch (machine_class) {
        case VICE_MACHINE_C64:      /* fall through */
        case VICE_MACHINE_C64SC:    /* fall through */
        case VICE_MACHINE_SCPU64:   /* fall through */
        case VICE_MACHINE_C128:     /* fall through */
        case VICE_MACHINE_VIC20:    /* fall through */
        case VICE_MACHINE_PET:      /* fall through */
        case VICE_MACHINE_CBM6x0:
            return true;
        default:
            /* No userport (C64DTV, CBM-II 5x0/P, VSID) or not usable for
             * printer (Plus/4) */
            return false;
    }
}

/** \brief  Handler for the "toggled" event of the Printer7 checkbox
 *
 * Switches between `PRINTER_DEVICE_NONE` and `PRINTER_DEVICE_REAL`.
 *
 * \param[in]   check       check button
 * \param[in]   user_data   extra data (unused)
 */
static void on_real_device7_toggled(GtkCheckButton *check, gpointer user_data)
{
    int state;

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check))) {
        state = PRINTER_DEVICE_REAL;
    } else {
        state = PRINTER_DEVICE_NONE;
    }
    resources_set_int("Printer7", state);
}

#if 0
/** \brief  Set PrinterTextDevice[1-3] resource
 *
 * \param[in]   entry       text entry
 * \param[in]   user_data   resource index (1-3)
 */
static void on_text_device_changed(GtkEntry *entry, gpointer user_data)
{
    int num = GPOINTER_TO_INT(user_data);
    const gchar *text = gtk_entry_get_text(entry);

    resources_set_string_sprintf("PrinterTextDevice%d", text, num);
}
#endif

/** \brief  Create Virtual Device check button
 *
 * \param[in]   device  printer device
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_virtual_device_check_button(int device)
{
    GtkWidget *check;

    check = vice_gtk3_resource_check_button_new_sprintf("VirtualDevice%d",
                                                        "Enable Virtual Device",
                                                        device);
    return check;
}

/** \brief  Create IEC device emulation check button
 *
 * \param[in]   device  printer device
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_iec_check_button(int device)
{
    GtkWidget *check;

    check = vice_gtk3_resource_check_button_new_sprintf("IECDevice%d",
                                                        "Enable IEC device",
                                                        device);
    return check;
}

/** \brief  Create checkbox to switch between NONE/REAL emu mode for Printer7
 *
 * NOTE: Cannot use resourcecheckbutton here since this toggle button switches
 *       between PRINTER_DEVICE_NONE (0) and PRINTER_DEVICE_REAL (2).
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_real_device7_check_button(void)
{
    GtkWidget *check;
    int        value = 0;

    check = gtk_check_button_new_with_label("Real device access");
    resources_get_int("Printer7", &value);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), value ? TRUE : FALSE);
    g_signal_connect(check,
                     "toggled",
                     G_CALLBACK(on_real_device7_toggled),
                     NULL);
    return check;
}


/** \brief  Handler for the 'clicked event of the "formfeed" button
 *
 * \param[in]   widget  button
 * \param[in]   data    device number (4-7)
 */
static void on_formfeed_clicked(GtkWidget *widget, gpointer data)
{
    int device = GPOINTER_TO_INT(data);

    printer_formfeed((unsigned int)device - PRINTER_MIN);
}


/** \brief  Create button to send formfeed to the printer
 *
 * \param[in]   device  device number (4-7)
 *
 * \return  GtkButton
 */
static GtkWidget *create_formfeed_button(int device)
{
    GtkWidget *button;
    char       label[64];

    g_snprintf(label, sizeof label, "Send formfeed to %s",
               children[device - PRINTER_MIN].title);
    button = gtk_button_new_with_label(label);
    g_signal_connect(button,
                     "clicked",
                     G_CALLBACK(on_formfeed_clicked),
                     GINT_TO_POINTER(device));
    return button;
}

/** \brief  Create a widget for the settings of printer # \a device
 *
 * Creates a widget for \a device to control its resource. The widget for
 * device #7 is different/simpler.
 *
 * TODO: Refactor to use the #children array; add support for creating userport
 *       printer widgets using a special device number (8?).
 *
 * \param[in]   device  device number (4-7)
 *
 * \return  GtkGrid
 */
static GtkWidget *create_printer_widget(int device)
{
    GtkWidget *grid;
    GtkWidget *type     = NULL;
    GtkWidget *virtdev  = NULL;
    GtkWidget *iec      = NULL;
    GtkWidget *driver   = NULL;
    GtkWidget *outmode  = NULL;
    GtkWidget *outdev   = NULL;
    GtkWidget *formfeed = NULL;
    int        index    = device - PRINTER_MIN;
    const child_t *child = &children[index];

    grid = vice_gtk3_grid_new_spaced(8, 0);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);

    /* create widgets */
    if (child->has_type) {
        /* None/FS/OpenCBM */
        type = printer_emulation_type_widget_create(device);
    } else if (child->has_realdev) {
        /* device 7 only has None/OpenCBM for type, so we use a
         * custom check button */
        type = create_real_device7_check_button();
    }
    if (child->has_virtdev) {
        virtdev = create_virtual_device_check_button(device);
    }
    if (child->has_iec && machine_has_iec()) {
        iec = create_iec_check_button(device);
    }
    if (child->has_driver) {
        driver = printer_driver_widget_create(device);
    }
    if (child->has_outmode) {
        outmode = printer_output_mode_widget_create(device);
    }
    if (child->has_outdev) {
        outdev = printer_output_device_widget_create(device);
    }
    if (child->has_formfeed) {
        formfeed = create_formfeed_button(device);
    }

    /* attach widgets */
    if (type != NULL) {
        gtk_grid_attach(GTK_GRID(grid), type, 0, 1, 1, 1);
    }
    if (virtdev != NULL) {
        if (!child->has_realdev) {
            gtk_widget_set_margin_top(virtdev, 8);
        }
        gtk_grid_attach(GTK_GRID(grid), virtdev, 0, 2, 2, 1);
    }
    if (iec != NULL) {
        gtk_grid_attach(GTK_GRID(grid), iec, 0, 3, 2, 1);
    }
    if (driver != NULL) {
        gtk_grid_attach(GTK_GRID(grid), driver, 1, 1, 1, 1);
    }
    if (outmode != NULL) {
        gtk_grid_attach(GTK_GRID(grid), outmode, 2, 1, 1, 1);
    }
    if (outdev != NULL) {
        gtk_grid_attach(GTK_GRID(grid), outdev, 3, 1, 1, 1);
    }
    if (formfeed != NULL) {
        gtk_widget_set_halign(formfeed, GTK_ALIGN_END);
        gtk_widget_set_valign(formfeed, GTK_ALIGN_END);
        gtk_widget_set_vexpand(formfeed, FALSE);
        gtk_grid_attach(GTK_GRID(grid), formfeed, 2, 2, 2, 2);
    }

    gtk_widget_show_all(grid);
    return grid;
#if 0

    if (device == 4 || device == 5 || device == 6) {
        /* device 4,5,6 are 'normal' printers */
        GtkWidget *formfeed;

        type    = printer_emulation_type_widget_create(device);
        virtual = create_virtual_device_check_button(device);
        if (machine_has_iec()) {
            iec = create_iec_check_button(device);
        }

        gtk_widget_set_margin_top(virtual, 8);
        gtk_grid_attach(GTK_GRID(grid), type,    0, 1, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), virtual, 0, 2, 2, 1);
        if (iec != NULL) {
            gtk_grid_attach(GTK_GRID(grid), iec, 0, 3, 2, 1);
        }

        gtk_grid_attach(GTK_GRID(grid),
                printer_driver_widget_create(device), 1, 1, 1, 1);
        gtk_grid_attach(GTK_GRID(grid),
                printer_output_mode_widget_create(device), 2, 1, 1, 1);
        gtk_grid_attach(GTK_GRID(grid),
                printer_output_device_widget_create(device), 3, 1, 1, 1);
        formfeed = create_formfeed_button(device);
        gtk_widget_set_halign(formfeed, GTK_ALIGN_END);
        gtk_widget_set_valign(formfeed, GTK_ALIGN_END);
        gtk_widget_set_vexpand(formfeed, FALSE);
        gtk_grid_attach(GTK_GRID(grid), formfeed, 2, 2, 2, 2);

    } else if (device == 7) {
        /* device 7 is 'special' */
        GtkWidget *realdevice;

        realdevice = create_real_device7_check_button();
        gtk_grid_attach(GTK_GRID(grid), realdevice, 0, 1, 1, 1);

        switch (machine_class) {
            /* these machines have IEC */
            case VICE_MACHINE_C64:      /* fall through */
            case VICE_MACHINE_C64SC:    /* fall through */
            case VICE_MACHINE_SCPU64:   /* fall through */
            case VICE_MACHINE_C128:     /* fall through */
            case VICE_MACHINE_C64DTV:   /* fall through */
#if 0
            /* FIXME: xvic does not use the generic IEC bus code in src/iecbus/iecbus.c yet */
            case VICE_MACHINE_VIC20:    /* fall through */
#endif
            case VICE_MACHINE_PLUS4:
                virtual = create_virtual_device_check_button(device);
                iec     = create_iec_check_button(device);
                gtk_grid_attach(GTK_GRID(grid), virtual, 0, 2, 1, 1);
                gtk_grid_attach(GTK_GRID(grid), iec,     0, 3, 1, 1);
                break;

            default:
                break;
        }
    }

    gtk_widget_show_all(grid);
    return grid;
#endif
}

/** \brief  Create widget to control Printer Text Devices 1-3
 *
 * \return  GtkGrid
 */
static GtkWidget *create_printer_text_devices_widget(void)
{
    GtkWidget *grid;
    int        i;

    grid = vice_gtk3_grid_new_spaced_with_label(8, 8, "Printer text devices", 6);
    for (i = 0; i < 3; i++) {
        GtkWidget  *label;
        GtkWidget  *entry;
        gchar       title[32];

        g_snprintf(title, sizeof title, "Device %d", i + 1);

        label = gtk_label_new(title);
        gtk_widget_set_halign(label, GTK_ALIGN_START);
        entry = vice_gtk3_resource_entry_full_new_sprintf("PrinterTextDevice%d",
                                                          i + 1);
        gtk_widget_set_hexpand(entry, TRUE);

        gtk_grid_attach(GTK_GRID(grid), label, 0, i + 1, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), entry, 1, i + 1, 1, 1);
    }

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create widget to control printer settings
 *
 * \param[in]   parent  parent widget
 *
 * \return  GtkGrid
 */
GtkWidget *settings_printer_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;
    GtkWidget *text_devices;
    GtkWidget *stack;
    GtkWidget *switcher;
    GtkWidget *userport = NULL;
    int        unit;

    grid = gtk_grid_new();

    text_devices = create_printer_text_devices_widget();
    gtk_grid_attach(GTK_GRID(grid), text_devices, 0, 0, 1, 1);

    stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(stack),
                                  GTK_STACK_TRANSITION_TYPE_NONE);
    /* add indentation to make it more clear it is part of the tabbed interface */
    gtk_widget_set_margin_start(stack, 16);
    gtk_widget_set_margin_end(stack, 16);
    for (unit = PRINTER_MIN; unit <= PRINTER_MAX; unit++) {
        GtkWidget *printer;
        int        index = unit - PRINTER_MIN;

        printer = create_printer_widget(unit);
        gtk_stack_add_titled(GTK_STACK(stack),
                             printer,
                             children[index].name,
                             children[index].title);
    }
    /* determine if the userport is available */
    if (machine_has_userport()) {
        /* create userport printer widget and add to stack */
        userport = userport_printer_widget_create();
        gtk_stack_add_titled(GTK_STACK(stack),
                             userport,
                             children[4].name,
                             children[4].title);
    }

    switcher = gtk_stack_switcher_new();
    gtk_stack_switcher_set_stack(GTK_STACK_SWITCHER(switcher),
                                 GTK_STACK(stack));
    gtk_widget_set_halign(switcher, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top(switcher, 16);
    gtk_widget_set_margin_bottom(switcher, 8);

    gtk_widget_show_all(stack);
    gtk_widget_show_all(switcher);

    gtk_grid_attach(GTK_GRID(grid), switcher, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), stack,    0, 2, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}
