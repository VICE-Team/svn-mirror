/** \file   src/arch/gtk3/widgets/base/carthelpers.c
 * \brief   Cartridge helpers functions
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

#include "vice.h"
#include <gtk/gtk.h>

#include "machine.h"
#include "resources.h"
#include "debug_gtk3.h"
#include "cartridge.h"

#include "carthelpers.h"


int (*carthelpers_save_func)(int type, const char *filename);
int (*carthelpers_flush_func)(int type);
int (*carthelpers_enabled_func)(int type);


/** \brief  Set cartridge helper functions
 *
 * This function helps to avoid the problems with VSID wrt cartridge code:
 * VSID doesn't link against any cartridge code and since the various widgets
 * in src/arch/gtk3 are linked into a single .a object which VSID also links to
 * we need a way to use cartridge functions without VSID borking during linking.
 * Passing in pointers to the cart functions in ${emu}ui.c (except vsidui.c)
 * 'solves' this problem.
 *
 * Normally \a save_func should be cartridge_save_image(), \a fush_func should
 * be cartridge_flush_image() and \a enabled_func should be
 * \a cartridge_type_enabled.
 * These are the functions used by many/all(?) cartridge widgets
 *
 * \param[in]   save_func       cartridge image save-as function
 * \param[in]   flush_func      cartridge image flush/save function
 * \param[in]   enabled_func    cartridge enabled state function
 */
void carthelpers_set_functions(
        int (*save_func)(int, const char *),
        int (*flush_func)(int),
        int (*enabled_func)(int))
{
    carthelpers_save_func = save_func;
    carthelpers_flush_func = flush_func;
    carthelpers_enabled_func = enabled_func;
}

