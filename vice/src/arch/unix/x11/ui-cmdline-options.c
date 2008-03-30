/*
 * ui-cmdline-options.c
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  André Fachat <fachat@physik.tu-chemnitz.de>
 *  Andreas Boose <viceteam@t-online.de>
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

#include <stdio.h>

#include "cmdline.h"


static const cmdline_option_t cmdline_options[] = {
    { "-htmlbrowser", SET_RESOURCE, 1, NULL, NULL, "HTMLBrowserCommand", NULL,
      "<command>", N_("Specify an HTML browser for the on-line help") },
    { "-install", SET_RESOURCE, 0, NULL, NULL,
      "PrivateColormap", (void *)1,
      NULL, N_("Install a private colormap") },
    { "+install", SET_RESOURCE, 0, NULL, NULL,
      "PrivateColormap", (void *)0,
      NULL, N_("Use the default colormap") },
    { "-saveres", SET_RESOURCE, 0, NULL, NULL,
      "SaveResourcesOnExit", (void *)1,
      NULL, N_("Save settings (resources) on exit") },
    { "+saveres", SET_RESOURCE, 0, NULL, NULL,
      "SaveResourcesOnExit", (void *)0,
      NULL, N_("Never save settings (resources) on exit") },
#ifdef USE_XF86_EXTENSIONS
    { "-fullscreen", SET_RESOURCE, 0, NULL, NULL,
      "UseFullscreen", (void *)1,
      NULL, N_("Enable fullscreen") },
    { "+fullscreen", SET_RESOURCE, 0, NULL, NULL,
      "UseFullscreen", (void *)0,
      NULL, N_("Disable fullscreen") },
#endif
    { "-displaydepth", SET_RESOURCE, 1, NULL, NULL,
      "DisplayDepth", NULL,
      "<value>", N_("Specify X display depth (1..32)") },
    { NULL }
};

int ui_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

