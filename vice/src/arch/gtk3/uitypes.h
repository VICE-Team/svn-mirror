/** \file   uitypes.h
 * \brief   Gtk3 UI menu item types
 *
 * These types are shared between uimenu.{c,h} and uimachinemenu.{c,h}.
 *
 * Due to someone splitting uimenu.c into uimenu.c and uimachinemenu.c we now
 * have to deal with a lot of cross-dependencies between those files. So fixing
 * that is definitely a TODO.
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

#ifndef VICE_UITYPES_H
#define VICE_UITYPES_H

/** \brief  Menu item types
 *
 * The submenu types needs special handling, no more callbacks to create the
 * menu so we won't have to deal with the `ui_update_checkmarks` crap of Gtk2.
 *
 * The 'layer' between VICE and Gtk3 should be as thin as possible, so no
 * UI_CREATE_TOGGLE_BUTTON() stuff.
 */
typedef enum ui_menu_item_type_e {
    UI_MENU_TYPE_GUARD = -1,        /**< list terminator */
    UI_MENU_TYPE_ITEM_ACTION,       /**< standard list item: activate dialog */
    UI_MENU_TYPE_ITEM_CHECK,        /**< menu item with checkmark */
    UI_MENU_TYPE_ITEM_RADIO_INT,    /**< menu item with radio button, int value */
    UI_MENU_TYPE_ITEM_RADIO_STR,    /**< menu item with radio button, string value */
    UI_MENU_TYPE_SUBMENU,           /**< submenu */
    UI_MENU_TYPE_SEPARATOR          /**< items separator */
} ui_menu_item_type_t;



/** \brief  Menu item object
 *
 * Contains information on a menu item
 */
typedef struct ui_menu_item_s {
    /** \brief  Menu item label
     *
     * The label displayed in the menu. Do not add any accelerator description
     * here, that is set and updated dynamically.
     */
    char *label;

    /** \brief  Menu item type
     *
     * \see ui_menu_item_type_t
     */
    ui_menu_item_type_t type;

    /** \brief  UI action name
     *
     * UI action name as defined in uiactions.h.
     *
     * The action name is used for the hotkeys to be able to set and alter the
     * hotkey assigned to the menu item.
     *
     * \note    Must be unique or NULL.
     * \note    Do NOT use the same action name for similar actions in different
     *          emulators, thanks to the run-time checking of the machine this
     *          will lead to the hotkeys code updating the first matching action
     *          it finds and ignoring the other actions with the same name.
     */
    char *action_name;

    /** \brief  Menu item callback function
     *
     * The return value determines whether or not the keypress was 'consumed'
     * by the UI.
     *
     * Normally you'd return TRUE here.
     *
     * The \a widget argument is the menu item and the \a user_data argument is
     * the data member for anything except check buttons; check buttons get the
     * resource name as their \a user_data argument.
     *
     * If `NULL`, there is no callback (for separators and placeholders for
     * not-yet-implemented items).
     */
    gboolean (*callback)(GtkWidget *widget, gpointer user_data);

    /** \brief  Resource name
     *
     * Resource name for check buttons and radio buttons.
     *
     * For check buttons the resource is read and interpreted as boolean to
     * set the check button state.
     * For radio buttons the resource is read and its value compared against the
     * data member to set the radio button state. For string types strcmp(3) is
     * used, meaning the data member must match the case of the expected resource
     * value. (radio groups really shouldn't use strings as IDs anyway)
     */
    char *resource;

    /** \brief  Callback data
     *
     * The callback data is used when triggering the callback.
     *
     *  - UI_MENU_TYPE_ITEM_ACTION: 'variant', optional
     *  - UI_MENU_TYPE_SUBMENU: array of submenu items
     *  - UI_MENU_TYPE_ITEM_CHECK: ignored
     *  - UI_MENU_TYPE_ITEM_RADIO_INT: integer value
     *  - UI_MENU_TYPE_ITEM_RADIO_STRING: string value
     *  - UI_MENU_TYPE_SEPARATOR: ignored
     */
    void *data;

    /** \brief  Gdk accelerator keysym
     *
     * Gdk keysym value, set by the hotkeys code.
     *
     * \see     /usr/include/gtk-3.0/gdk/gdkkeysyms.h
     */
    guint keysym;

    /** \brief  Gdk modifier mask
     *
     * Bitmask containing Gdk modifier masks, set by the hotkeys code.
     */
    GdkModifierType modifier;

    /** \brief  Hold VICE mainlock
     *
     * Determines whether the callback should be called while holding the VICE
     * mainlock.
     */
    bool unlocked;

} ui_menu_item_t;


/** \brief  Menu references for the hotkeys interface
 *
 * \note    Each menu can have submenus.
 */
typedef struct ui_menu_ref_s {
    char *          name;   /**< name of the top level menu, for debugging */
    ui_menu_item_t *items;  /**< items of the menu, can be recursive */
} ui_menu_ref_t;


/** \brief  Terminator of a menu items list
 */
#define UI_MENU_TERMINATOR { NULL, UI_MENU_TYPE_GUARD, NULL, NULL, NULL, NULL, 0, 0 }


/** \brief  Menu items separator
 */
#define UI_MENU_SEPARATOR { "---", UI_MENU_TYPE_SEPARATOR, NULL, NULL, NULL, NULL, 0, 0 }


/** \brief  Platform-dependent accelerator key defines
 */
#ifdef MACOSX_SUPPORT
  /* Mac Command key (Windows key on PC keyboards) */
  #define VICE_MOD_MASK GDK_META_MASK
#else
  /* Alt key (Option key on Mac keyboards) */
  #define VICE_MOD_MASK GDK_MOD1_MASK
#endif


/** \brief  Iterator for vice menu items
 *
 * Used to iterate over all VICE menu items, as opposed to Gtk menu items.
 *
 * \see ui_vice_menu_iter_init()
 * \see ui_vice_menu_iter_next()
 */
typedef struct ui_vice_menu_iter_s {
    size_t          menu_index; /**< index in menu_references[] */
    ui_menu_item_t *menu_item;  /**< current item in menu */
} ui_vice_menu_iter_t;


#endif
