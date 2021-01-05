/** \file   resourcenumericstring.c
 * \brief   Numeric string connected to a resource
 *
 * Used for resources that store their numeric value as a string.
 *
 * Supports using suffixes 'K', 'M', and 'G' for KiB, MiB and GiB respectively.
 *
 *
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
 *
 */

#include "vice.h"

#include <gtk/gtk.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdint.h>
#include <ctype.h>
#include <errno.h>

#include "vice_gtk3.h"
#include "lib.h"
#include "resources.h"

#include "resourcenumericstring.h"


/** \brief  CSS rule for the widget to show the current contents are invalid
 */
#define CSS_INVALID \
    "entry { color: red; }"

/** \brief  CSS rule for the widget to show the current contents are alid
 */
#define CSS_VALID \
    "entry { color: green; }"


/** \brief  Object holding unit (suffix, factor) pairs
 */
typedef struct numstr_unit_s {
    int         suffix; /**< suffix, in upper case */
    uint64_t    factor; /**< number to multiply numeric string with */
} numstr_unit_t;


/** \brief  List of suffixes and their factors
 */
static const numstr_unit_t units[] = {
    { 'K',  1<<10 },
    { 'M',  1<<20 },
    { 'G',  1<<30 },
#if 0
    { 'T',  1U<<40U },
#endif
    { -1,   0 }
};


/** \brief  List of valid keys for the widget
 *
 * This excludes GKD_Enter since that's used to attempt to set the entry.
 */
static const gint valid_keys[] = {
    GDK_KEY_0, GDK_KEY_1, GDK_KEY_2, GDK_KEY_3, GDK_KEY_4,
    GDK_KEY_5, GDK_KEY_6, GDK_KEY_7, GDK_KEY_8, GDK_KEY_9,
    GDK_KEY_k, GDK_KEY_K,   /* KiB */
    GDK_KEY_m, GDK_KEY_M,   /* MiB */
    GDK_KEY_g, GDK_KEY_G,   /* GiB */
    GDK_KEY_x, GDK_KEY_X,   /* 0x -> hex */
    GDK_KEY_BackSpace,
    GDK_KEY_Insert,
    GDK_KEY_Left,
    GDK_KEY_Right,
    -1
};


/** \brief  Validate input
 *
 * \param[in,out]   widget  GtkEntry instance
 * \param[in]       data    extra data (unused yet)
 *
 * \return  bool
 */
static gboolean input_is_valid(GtkWidget *widget, gpointer data)
{
    long long result;
    char *endptr;
    const char *text;
    int64_t factor = 1;

    text = gtk_entry_get_text(GTK_ENTRY(widget));
    if (*text == '\0') {
        /* special: 0 means no fixed size */
        return TRUE;
    }

    result = strtoull(text, &endptr, 0);
    if (*endptr != '\0') {
        /* possible suffix */
        int i;

        debug_gtk3("Got possible suffix '%c'", *endptr);

        if (endptr == text) {
            /* missing digits */
            return FALSE;
        }

        for (i = 0; units[i].factor > 0; i++) {
            if (toupper(*endptr) == units[i].suffix) {
                debug_gtk3("Found valid suffix '%d' -> %" PRIu64,
                        toupper(*endptr), units[i].factor);
                factor = units[i].factor;
                break;
            }
        }
        if (units[i].suffix < 0) {
            debug_gtk3("Invalid suffix.");
            return FALSE;
        }

        if (endptr[1] != '\0') {
            return FALSE;
        }
    }

    debug_gtk3("result = %lld", result * factor);

    return TRUE;

}



/** \brief  Event handler for the 'changed' event
 *
 * Get's triggered after accepting/refusing any key input. so we check the input
 * for validity here, and also add a visual hint via CSS.
 *
 * \param[in,out]   widget  widget
 * \param[in]       data    unused extra data
 */
static void on_entry_changed(GtkWidget *widget, gpointer data)
{
    GtkCssProvider *provider;

    debug_gtk3("Called");
    provider = g_object_get_data(G_OBJECT(widget), "CSSProvider");
    if (!input_is_valid(widget, NULL)) {
        vice_gtk3_css_provider_add(widget, provider);
    } else {
        vice_gtk3_css_provider_remove(widget, provider);
    }

}


/** \brief  Handler for the "focus-out" event
 *
 * \param[in]   entry   entry box
 * \param[in]   event   event object
 * \param[in]   data    unused
 *
 * \return  TRUE
 */
static gboolean on_focus_out_event(
        GtkEntry *entry,
        GdkEvent *event,
        gpointer data)
{
    const char *value;
    const char *resource;

    debug_gtk3("Triggered!");
    value = gtk_entry_get_text(GTK_ENTRY(entry));
    resource = resource_widget_get_resource_name(GTK_WIDGET(entry));

    debug_gtk3("Got '%s' with Enter for '%s;", value, resource);

    if (resources_set_string(resource, value) < 0) {
        debug_gtk3("Implement proper error popup");
    }
    return TRUE;
}



/** \brief  Handler for the "on-key-press" event
 *
 * \param[in]   entry   entry box
 * \param[in]   event   event object
 * \param[in]   data    unused
 *
 * \return  TRUE if Enter was pushed, FALSE otherwise (makes the pushed key
 *          propagate to the entry)
 */
static gboolean on_key_press_event(
        GtkEntry *entry,
        GdkEvent *event,
        gpointer data)
{
    GdkEventKey *keyev = (GdkEventKey *)event;

    if (keyev->type == GDK_KEY_PRESS) {
        gint keyval = keyev->keyval;
        debug_gtk3("Key Press event, keyval = %d", keyval);

        if (keyev->keyval == GDK_KEY_Return) {
            /*
             * We handled Enter/Return for Gtk3/GLib, whether or not the
             * resource actually gets updated is another issue.
             */
            const char *value;
            const char *resource;

            value = gtk_entry_get_text(GTK_ENTRY(entry));
            resource = resource_widget_get_resource_name(GTK_WIDGET(entry));

            debug_gtk3("Got '%s' with Enter for '%s;", value, resource);

            if (resources_set_string(resource, value) < 0) {
                debug_gtk3("Implement proper error popup");
            }
            return TRUE;

        } else {

            int i;
            for (i = 0; valid_keys[i] >= 0; i++) {
                debug_gtk3("Comparing %d against %d", keyval, valid_keys[i]);
                if (valid_keys[i] == keyval) {
                    debug_gtk3("Valid keyval %d", keyval);
                    return FALSE;
                }
            }
            return TRUE;
        }
    }
    return FALSE;
}



/** \brief  Handler for the 'destroy' event of the widget
 *
 * Cleans up resource allocated by the widget.
 *
 * \param[in,out]   widget  widget
 * \param[in]       data    extra event data (unused)
 */
static void on_destroy(GtkWidget *widget, gpointer data)
{
    resource_widget_free_resource_name(widget);
    resource_widget_free_string(widget, "ResourceOrig");
}


/** \brief  Create numeric string entry box for \a resource
 *
 * Sets the following properties on the widget:
 *  - ResourceName      resource name
 *  - ResourceOrig      value of resource during construction
 *  - ResourceMin       lower limit (default = NULL, no limit)
 *  - ResourceMax       upper limit (default = NULL, no limit)
 *
 * \param[in]   resource    resource name
 *
 * \return  GtkEntry
 */
GtkWidget *vice_gtk3_resource_numeric_string_new(const char *resource)
{
    GtkWidget *entry;
    const char *current = NULL;
    char *orig;
    GtkCssProvider *provider;

    entry = gtk_entry_new();
    resource_widget_set_resource_name(entry, resource);

    /* get current resource value */
    if (resources_get_string(resource, &current) < 0) {
        debug_gtk3("Failed to get resource '%s' value.", resource);
        current = NULL;
    }
    /* this assumes a correct value */
    gtk_entry_set_text(GTK_ENTRY(entry), current);

    /* store orignal value for 'reset' method */
    orig = lib_strdup(current != NULL ? current : "");
    g_object_set_data(G_OBJECT(entry), "ResourceOrig", orig);

    /* set limits */
    g_object_set_data(G_OBJECT(entry), "ResouceMinLSB", GUINT_TO_POINTER(0));
    g_object_set_data(G_OBJECT(entry), "ResouceMinMSB", GUINT_TO_POINTER(0));
    g_object_set_data(G_OBJECT(entry), "ResouceMaxLSB", GUINT_TO_POINTER(-1));
    g_object_set_data(G_OBJECT(entry), "ResouceMaxMSB", GUINT_TO_POINTER(-1));

    /* set preferrence to upper case (doesn't work for shit) */
    gtk_entry_set_input_hints(GTK_ENTRY(entry), GTK_INPUT_HINT_UPPERCASE_CHARS);

    /* add CSS provider to visually (in)validate input */
    provider = vice_gtk3_css_provider_new(CSS_INVALID);
    g_object_set_data(G_OBJECT(entry), "CSSProvider", provider);

    g_signal_connect_unlocked(entry, "destroy", G_CALLBACK(on_destroy), NULL);
    g_signal_connect(entry, "changed", G_CALLBACK(on_entry_changed), NULL);
    g_signal_connect(entry, "key-press-event", G_CALLBACK(on_key_press_event), NULL);
    g_signal_connect(entry, "focus-out-event",
            G_CALLBACK(on_focus_out_event), NULL);

    gtk_widget_show_all(entry);
    return entry;
}


/** \brief  Set limits on the widget's valid values
 *
 * These limits are by default set to 0 to UINT64_MAX
 *
 * \param[in,out]   widget  resource numeric string widget
 * \param[in]       min     minimum value
 * \param[in]       max     maximum value
 *
 */
void vice_gtk3_resource_numeric_string_set_limits(GtkWidget *widget,
                                                  uint64_t min,
                                                  uint64_t max)
{
    uint32_t min_lsb;
    uint32_t min_msb;
    uint32_t max_lsb;
    uint32_t max_msb;

    min_lsb = min & G_MAXUINT32;
    min_msb = min >> 32U;
    max_lsb = min & G_MAXUINT32;
    max_msb = min >> 32U;

    g_object_set_data(G_OBJECT(widget), "ResouceMinLSB", GUINT_TO_POINTER(min_lsb));
    g_object_set_data(G_OBJECT(widget), "ResouceMinMSB", GUINT_TO_POINTER(min_msb));
    g_object_set_data(G_OBJECT(widget), "ResouceMaxLSB", GUINT_TO_POINTER(max_lsb));
    g_object_set_data(G_OBJECT(widget), "ResouceMaxMSB", GUINT_TO_POINTER(max_msb));
}
