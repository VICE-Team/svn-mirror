/** \file   archdep_default_hotkey_file_name.c
 * \brief   Get the path to the default hotkeys file
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 *
 * TODO:    Once we start supporting custom hotkeys in the Gtk3 port, this
 *          code needs to be updated for Windows, since Gtk3-Win binaries live
 *          in bin/, so we need to strip off 'bin/'.
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
 */

#include "vice.h"
#include "archdep_defs.h"

#include "archdep.h"
#include "archdep_boot_path.h"
#include "archdep_home_path.h"
#include "archdep_join_paths.h"
#include "archdep_user_config_path.h"
#include "kbd.h"
#include "machine.h"

#include "archdep_default_hotkey_file_name.h"


/** \brief  Get path to default hotkeys file
 *
 * \return  Full path to hotkeys file
 *
 * \note    Free after use with lib_free()
 */
char *archdep_default_hotkey_file_name(void)
{
#ifdef ARCHDEP_OS_WINDOWS
    return archdep_join_paths(archdep_boot_path(),
                              KBD_PORT_PREFIX "-hotkey-",
                              machine_get_name(),
                              ".vkm",
                              NULL);
#else
    return archdep_join_paths(archdep_user_config_path(),
                              KBD_PORT_PREFIX "-hotkey-",
                              machine_get_name(),
                              ".vkm",
                              NULL);
#endif
}
