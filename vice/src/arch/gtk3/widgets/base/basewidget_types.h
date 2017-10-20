/** \file   src/arch/gtk3/widgets/base/basewidget_types.h
 * \brief   Types used for the base widgets
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
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

#ifndef VICE_BASEWIDGET_TYPES_H
#define VICE_BASEWIDGET_TYPES_H

#include "vice.h"
#include <gtk/gtk.h>


/** \brief  Entry for a combo box using an integer as ID
 */
typedef struct ui_combo_entry_int_s {
    const char *name;   /**< displayed in the combo box */
    int         id;     /**< ID for the entry in the combo box */
} ui_combo_entry_int_t;


/** \brief  Entry for a combo box using a string as ID
 */
typedef struct ui_combo_entry_str_s {
    const char *name;   /**< displayed in the combo box */
    const char *id;     /**< ID for the entry in the combo box */
} ui_combo_entry_str_t;


#endif
