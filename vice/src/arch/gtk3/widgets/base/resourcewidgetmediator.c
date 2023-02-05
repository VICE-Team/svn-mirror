/** \file   resourcewidgetmeditator.c
 * \brief   Resource widget mediator
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Object and methods to mediate between resource widgets and their resources.
 *
 * Provides a way to keep extra state around and present a unified API to
 * make resource-bound widgets interact with their resource. The object
 * 'attaches' itself to a GtkWidget via g_object_set_data() and sets up a
 * signal handler to destroy itself on widget destruction.
 *
 * The methods suffixed with '_w' are shortcuts for calling mediator_for_widget()
 * followed by calling the method without the suffix on the mediator. For example:
 * \code{.c}
 *  // get meditator and then call the method:
 *  mediator_t *mediator = mediator_for_widget(widget);
 *  const char *resource = mediator_get_name(mediator);
 *
 *  // using the _w method:
 *  const char *resource = mediator_get_name_w(widget);
 * \endcode
 */

/*
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

#include "debug_gtk3.h"
#include "log.h"
#include "resources.h"

#include "resourcewidgetmediator.h"


/** \brief  Key of the mediator object in the GObject GQuark data store
 */
#define MEDIATOR_GOBJECT_KEY    "ResourceMediator"


/** \brief  Free resource mediator object
 *
 * Free resources used by \a mediator and its members.
 * Normally this function doesn't need to be called, it's registered to run
 * on widget destruction in mediator_new().
 *
 * \param[in]   mediator   mediator object
 */
static void mediator_free(mediator_t *mediator)
{
    g_value_unset(&(mediator->initial));
    g_value_unset(&(mediator->current));
    g_free(mediator->name);
    g_free(mediator);
}

/** \brief  Handler for the 'destroy' event of the mediator's widget
 *
 * This handler is connected in mediator_new() and triggered on destruction of
 * \a widget, in which case it cleans up the \a mediator object and its members.
 *
 * \param[in]   widget  widget of \a mediator
 * \param[in]   mediator   resource widget mediator object
 */
static void on_widget_destroy(GtkWidget *widget, gpointer mediator)
{
    mediator_free((mediator_t*)mediator);
}


/** \brief  Create and attach resource widget mediator
 *
 * Create new resource mediator object for resource \a name and attach it to
 * \a widget. Registers a 'destroy' signal handler on \a widget that frees
 * the mediator and its members.
 *
 * \param[in]   widget  resource widget
 * \param[in]   name    resource name
 * \param[in]   type    resource type (`G_TYPE_BOOLEAN`, `G_TYPE_INT` or
 *                      `G_TYPE_STRING`)
 *
 * \return  new resource mediator
 */
mediator_t *mediator_new(GtkWidget *widget, const char *name, GType type)
{
    mediator_t *mediator;
    int         ival = 0;
    const char *sval = NULL;

    /* We need to use g_malloc0() here to make sure the GValue members are
     * initialized to 0, since we can't use `v = G_VALUE_INIT;` (the macro
     * expands to a struct assignment).
     */
    mediator = g_malloc0(sizeof *mediator);
    g_object_set_data(G_OBJECT(widget), MEDIATOR_GOBJECT_KEY, (gpointer)mediator);

    mediator->widget  = widget;
    mediator->name    = g_strdup(name);
    mediator->type    = type;
    mediator->handler = 0;
    mediator->destroy = g_signal_connect_unlocked(G_OBJECT(widget),
                                               "destroy",
                                               G_CALLBACK(on_widget_destroy),
                                               mediator);
    /* initialize GValues to their GType */
    g_value_init(&(mediator->initial), type);
    g_value_init(&(mediator->current), type);

    /* get initial resource value */
    switch (type) {
        case G_TYPE_BOOLEAN:
            if (resources_get_int(name, &ival) < 0) {
                goto report_error;
            }
            g_value_set_boolean(&(mediator->initial), ival ? TRUE : FALSE);
            g_value_set_boolean(&(mediator->current), ival ? TRUE : FALSE);
            break;

        case G_TYPE_INT:
            if (resources_get_int(name, &ival) < 0) {
                goto report_error;
            }
            g_value_set_int(&(mediator->initial), ival);
            g_value_set_int(&(mediator->current), ival);
            break;

        case G_TYPE_STRING:
            if (resources_get_string(name, &sval) < 0) {
                goto report_error;
            }
            g_value_set_string(&(mediator->initial), sval);
            g_value_set_string(&(mediator->current), sval);
            break;

        default:
            debug_gtk3("resource %s: unhandled type %s.",
                       name, g_type_name(type));
            break;
    }

    return mediator;

report_error:
    log_error(LOG_ERR,
              "%s(): resource %s: failed to get value.",
              __func__, name);
    return mediator;
}


/** \brief  Get widget of the mediator
 *
 * \param[in]   mediator    resource mediator
 *
 * \return  GtkWidget
 */
GtkWidget *mediator_get_widget(mediator_t *mediator)
{
    return mediator->widget;
}


/** \brief  Get the mediator for a widget
 *
 * \return  mediator or `NULL` when no mediator is attached
 */
mediator_t *mediator_for_widget(GtkWidget *widget)
{
    return g_object_get_data(G_OBJECT(widget), MEDIATOR_GOBJECT_KEY);
}


/** \brief  Get resource name of a mediator
 *
 * Get the resource name of \a mediator.
 *
 * \param[in]   mediator
 *
 * \return  resource name
 */
const char *mediator_get_name(mediator_t *mediator)
{
    return mediator->name;
}


/** \brief  Get resource name of a mediator for a widget
 *
 * Get the resource name of the mediator for \a widget. If no mediator is
 * attached `NULL` is returned
 *
 * \return  resource name
 */
const char *mediator_get_name_w(GtkWidget *widget)
{
    mediator_t *mediator = mediator_for_widget(widget);
    if (mediator != NULL) {
        return mediator->name;
    }
    return NULL;
}


/** \brief  Set ID of the signal handler of the widget that updates the resource
 *
 * Set ID of the signal handler of the widget that is used to update the bound
 * resource on widget state changes.
 *
 * \param[in]   mediator    resource mediator
 * \param[in]   handler     ID of the signal handler
 */
void mediator_set_handler(mediator_t *mediator, gulong handler)
{
    mediator->handler = handler;
}


/** \brief  Block signal handler registered on the widget for resource updates
 *
 * Block the signal handler of the widget that is used to update the bound
 * resource on widget state changes.
 *
 * \param[in]   mediator
 *
 * \note    Requires a handler ID having been set via mediator_set_handler().
 */
void mediator_handler_block(mediator_t *mediator)
{
    if (mediator->handler > 0) {
        g_signal_handler_block(mediator->widget, mediator->handler);
    }
}


/** \brief  Unblock signal handler registered on the widget for resource updates
 *
 * Unlock the signal handler of the widget that is used to update the bound
 * resource on widget state changes.
 *
 * \param[in]   mediator
 *
 * \note    Requires a handler ID having been set via mediator_set_handler().
 */
void mediator_handler_unblock(mediator_t *mediator)
{
    if (mediator->handler > 0) {
        g_signal_handler_unblock(mediator->widget, mediator->handler);
    }
}


/******************************************************************************
 *        Boolean resource methods (`gboolean` in Gtk, `int` in VICE)         *
 *****************************************************************************/

/** \brief  Get boolean resource value through mediator
 *
 * \param[in]   mediator    resource mediator
 *
 * \return  resource value
 */
gboolean mediator_get_resource_boolean(mediator_t *mediator)
{
    int      i = 0;
    gboolean b;

    resources_get_int(mediator->name, &i);
    b = i ? TRUE : FALSE;
    /* also update `current` to be sure */
    mediator_set_current_boolean(mediator, b);
    return b;
}


/** \brief  Get boolean resource value on mediator instanciation
 *
 * \param[in]   mediator    resource mediator
 *
 * \return  resource value
 */
gboolean mediator_get_initial_boolean(mediator_t *mediator)
{
    return g_value_get_boolean(&(mediator->initial));
}


/** \brief  Get last valid value of boolean resource
 *
 * The `current` member is updated on succesfully setting the resource, it can
 * be used to revert a widget to its previous state should setting the resource
 * fail.
 *
 * \param[in]   mediator    resource mediator
 *
 * \return  last valid resource value
 */
gboolean mediator_get_current_boolean(mediator_t *mediator)
{
    return g_value_get_boolean(&(mediator->current));
}


/** \brief  Get boolean factory value for resource
 *
 * \param[in]   mediator    resource mediator
 *
 * \return  resource factory value
 */
gboolean mediator_get_factory_boolean(mediator_t *mediator)
{
    int factory = 0;

    resources_get_default_value(mediator->name, &factory);
    return factory ? TRUE : FALSE;
}


/** \brief  Set current boolean resource value in the mediator
 *
 * Calling this will update the last valid value of the resource in the
 * \a mediator, which can be used to revert the widget to its previous state
 * when setting the resource fails.
 * When using update() there's no need to call this, that function calls this
 * function on success.
 *
 * \param[in]   mediator    resource mediator
 * \param[in]   value       resource value
 */
void mediator_set_current_boolean(mediator_t *mediator, gboolean value)
{
    g_value_set_boolean(&(mediator->current), value);
}


/** \brief  Set callback function to be called on succesful resource updates
 *
 * When a resource is succesfully updated \a callback will be called with
 * the widget and the new resource value as its arguments.
 *
 * \param[in]   mediator    resource mediator
 * \param[in]   callback    user-defined callback
 */
void mediator_set_callback_boolean(mediator_t *mediator,
                                   void (*callback)(GtkWidget*, gboolean))
{
    mediator->callback.b = callback;
}


/** \brief  Set callback function to be called on succesful resource updates
 *
 * When a resource is succesfully updated \a callback will be called with
 * the \a widget and the new resource value as its arguments.
 *
 * \param[in]   widget      resource widget
 * \param[in]   callback    user-defined callback
 */
void mediator_set_callback_boolean_w(GtkWidget *widget,
                                     void (*callback)(GtkWidget*, gboolean))
{
    mediator_t *mediator = mediator_for_widget(widget);
    if (mediator != NULL) {
        mediator->callback.b = callback;
    }
}


/** \brief  Set resource, update internal mediator and trigger user callback
 *
 * Set resource and upon success update the last valid resource value in
 * \a mediator and trigger the user-registered callback, if present.
 *
 * \param[in]   mediator    resource mediator
 * \param[in]   value       new value for resource
 *
 * \return  `TRUE` if the resource succesfully updated
 */
gboolean mediator_update_boolean(mediator_t *mediator, gboolean value)
{
    if (resources_set_int(mediator->name, value) == 0) {
        mediator_set_current_boolean(mediator, value);
        if (mediator->callback.b != NULL) {
            mediator->callback.b(mediator->widget, value);
        }
        return TRUE;
    }
    return FALSE;
}


/** \brief  Shortcut for mediator_update_boolean() using a widget
 *
 * Calls mediator_update_boolean() on the mediator obtained from \a widget.
 *
 * \param[in]   widget  resource widget
 * \param[in]   value   resource value
 *
 * \return  `TRUE` if the resource succesfully updated
 */
gboolean mediator_update_boolean_w(GtkWidget *widget, gboolean value)
{
    mediator_t *mediator = mediator_for_widget(widget);
    if (mediator != NULL) {
        return mediator_update_boolean(mediator_for_widget(widget), value);
    }
    return FALSE;
}


/******************************************************************************
 *          Integer resource methods (`int` in Gtk, `int  in VICE)            *
 *****************************************************************************/

/** \brief  Get integer resource value through mediator
 *
 * \param[in]   mediator    resource mediator
 *
 * \return  resource value
 */
int mediator_get_resource_int(mediator_t *mediator)
{
    int i = 0;

    resources_get_int(mediator->name, &i);
    return i;

}


/** \brief  Get integer resource value on mediator instanciation
 *
 * \param[in]   mediator    resource mediator
 *
 * \return  resource's value when mediator_new() was called
 */
int mediator_get_initial_int(mediator_t *mediator)
{
    return g_value_get_int(&(mediator->initial));
}


/** \brief  Get integer resource factory value
 *
 * \param[in]   mediator    resource mediator
 *
 * \return  factory value
 */
int mediator_get_factory_int(mediator_t *mediator)
{
    int factory = 0;

    resources_get_default_value(mediator->name, &factory);
    return factory;
}


/** \brief  Get last valid value of integer resource
 *
 * The `current` member is updated on succesfully setting the resource, it can
 * be used to revert a widget to its previous state should setting the resource
 * fail.
 *
 * \param[in]   mediator    resource mediator
 *
 * \return  last valid resource value
 */
int mediator_get_current_int(mediator_t *mediator)
{
    return g_value_get_int(&(mediator->current));
}


/** \brief  Set current integer resource value in the mediator
 *
 * Calling this will update the last valid value of the resource in the
 * \a mediator, which can be used to revert the widget to its previous state
 * when setting the resource fails.
 * When using update() there's no need to call this, that function calls this
 * function on success.
 *
 * \param[in]   mediator    resource mediator
 * \param[in]   value       resource value
 */
void mediator_set_current_int(mediator_t *mediator, int value)
{
    g_value_set_int(&(mediator->current), value);
}


/** \brief  Set callback function to be called on succesful resource updates
 *
 * When a resource is succesfully updated \a callback will be called with
 * the widget and the new resource value as its arguments.
 *
 * \param[in]   mediator    resource mediator
 * \param[in]   callback    user-defined callback
 */
void mediator_set_callback_int(mediator_t *mediator,
                               void (*callback)(GtkWidget*, int))
{
    mediator->callback.i = callback;
}


/** \brief  Set resource, update internal mediator and trigger user callback
 *
 * Set resource and upon success update the last valid resource value in
 * \a mediator and trigger the user-registered callback, if present.
 *
 * \param[in]   mediator    resource mediator
 * \param[in]   value       new value for resource
 *
 * \return  `TRUE` if the resource succesfully updated
 */
gboolean mediator_update_int(mediator_t *mediator, int value)
{
    if (resources_set_int(mediator->name, value) == 0) {
        mediator_set_current_int(mediator, value);
        if (mediator->callback.i != NULL) {
            mediator->callback.i(mediator->widget, value);
        }
        return TRUE;
    }
    return FALSE;
}


/** \brief  Shortcut for mediator_update_int() using a widget
 *
 * Calls mediator_update_int() on the mediator obtained from \a widget.
 *
 * \param[in]   widget  resource widget
 * \param[in]   value   resource value
 *
 * \return  `TRUE` if the resource succesfully updated
 */
gboolean mediator_update_int_w(GtkWidget *widget, int value)
{
    mediator_t *mediator = mediator_for_widget(widget);
    if (mediator != NULL) {
        return mediator_update_int(mediator_for_widget(widget), value);
    }
    return FALSE;
}
