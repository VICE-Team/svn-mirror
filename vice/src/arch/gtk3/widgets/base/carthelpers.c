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
int (*carthelpers_is_enabled_func)(int type);
int (*carthelpers_enable_func)(int type);
int (*carthelpers_disable_func)(int type);


/** \brief  Placeholder function for functions accepting (int)
 *
 * Makes sure calling for example, carthelpers_flush_func() doesn't dereference
 * a NULL pointer when that was passed into carthelpers_set_functions()
 *
 * \return  -1
 */
static int null_handler(int type)
{
    debug_gtk3("warning: not implemented (NULL)\n");
    return -1;
}


/** \brief  Placeholder function for functions accepting (int, const char *)
 *
 * Makes sure calling for example, carthelpers_save_func() doesn't dereference
 * a NULL pointer when that was passed into carthelpers_set_functions()
 *
 * \return  -1
 */

static int null_handler_save(int type, const char *filename)
{
    debug_gtk3("warning: not implemented (NULL)\n");
    return -1;
}


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
 * \param[in]   is_enabled_func cartridge enabled state function
 * \param[in]   enable_func     cartridge enable function
 * \param[in]   disable_func    cartridge disable function
 */
void carthelpers_set_functions(
        int (*save_func)(int, const char *),
        int (*flush_func)(int),
        int (*is_enabled_func)(int),
        int (*enable_func)(int),
        int (*disable_func)(int))
{
    carthelpers_save_func = save_func ? save_func : null_handler_save;
    carthelpers_flush_func = flush_func ? flush_func : null_handler;
    carthelpers_is_enabled_func = is_enabled_func ? is_enabled_func : null_handler;
    carthelpers_enable_func = enable_func ? enable_func : null_handler;
    carthelpers_disable_func = disable_func ? disable_func : null_handler;
}
