/*
 * uifliplist.c - Fliplist menu management
 *
 * Written by
 *  Michael C. Martin <mcmartin@gmail.com>
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
#include "fliplist.h"

#include "uifliplist.h"

/** \brief Fill in a menu with controls for fliplist control.
 *
 * Fliplist controls are placed at the end of a menu, after a
 * separator. Any previously-existing fliplist controls within the
 * menu will be removed.
 *
 * \param menu            The menu to be edited.
 * \param unit            The drive unit (8-11) that this menu
 *                        will control.
 * \param separator_count The number of menu separators in the
 *                        part of the menu that does not involve
 *                        the fliplist.
 */
void ui_populate_fliplist_menu(GtkWidget *menu, int unit, int separator_count)
{
    const char *fliplist_string;
    GtkWidget *menu_item;
    /* TODO: Wipe out fliplist entries */
    
    /* Fliplist controls in GTK2/GNOME are next/previous and then the
     * full list of entries within it. For GTK3 we only show these if
     * the fliplist isn't empty for this drive. */
    /* TODO: Add/Remove current image to/from fliplist should really
     * be here too. */
    /* TODO: All of these image names should be shortened to the
     * suffix from the last path separator. */
    fliplist_string = fliplist_get_next(unit);
    if (fliplist_string) {
        char buf[128];
        fliplist_t fliplist_iterator;
        gtk_container_add(GTK_CONTAINER(menu), gtk_separator_menu_item_new());
        snprintf(buf, 128, _("Next: %s"), fliplist_string);
        buf[127] = 0;
        menu_item = gtk_menu_item_new_with_label(buf);
        /* TODO: Signal connect. Fliplist iteration seems a little weird. */
        gtk_container_add(GTK_CONTAINER(menu), menu_item);
        fliplist_string = fliplist_get_prev(unit);
        if (fliplist_string) {
            snprintf(buf, 128, _("Previous: %s"), fliplist_string);
            buf[127] = 0;
            menu_item = gtk_menu_item_new_with_label(buf);
            /* TODO: Signal connect. Fliplist iteration seems a little weird. */
            gtk_container_add(GTK_CONTAINER(menu), menu_item);
        }
        gtk_container_add(GTK_CONTAINER(menu), gtk_separator_menu_item_new());
        fliplist_iterator = fliplist_init_iterate(unit);
        while (fliplist_iterator) {
            menu_item = gtk_menu_item_new_with_label(fliplist_get_image(fliplist_iterator));
            /* TODO: Signal connect. Fliplist iteration seems a little weird. */
            gtk_container_add(GTK_CONTAINER(menu), menu_item);
            fliplist_iterator = fliplist_next_iterate(unit);
        }
    }
}
