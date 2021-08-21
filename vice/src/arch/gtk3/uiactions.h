/** \file   uiactions.h
 * \brief   Gtk3 UI action names and descriptions
 *
 * Provides names and descriptions for Gtk3 UI actions.
 *
 * Used by menu items and custom hotkeys. There will be no Doxygen docblocks
 * for most of the defines, since they're self-explanatory. And obviously I will
 * not bitch about keeping the text within 80 columns :D
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
 */

#ifndef VICE_UIACTIONS_H
#define VICE_UIACTIONS_H

#define ACTION_SMART_ATTACH             "smart-attach"
#define ACTION_SMART_ATTACH_DESC        "Attach a medium to the emulator inspecting its type"

#define ACTION_TOGGLE_PAUSE             "toggle-pause"
#define ACTION_TOGGLE_PAUSE_DESC        "Toggle Pause"

#define ACTION_TOGGLE_MOUSE_GRAB        "toggle-mouse-grab"
#define ACTION_TOGGLE_MOUSE_GRAB_DESC   "Toggle Mouse Grab"

#define ACTION_TOGGLE_WARP_MODE         "toggle-warp-mode"
#define ACTION_TOGGLE_WARP_MODE_DESC    "Toggle Warp Mode"

#define ACTION_ADVANCE_FRAME            "advance-frame"
#define ACTION_ADVANCE_FRAME_DESC       "Advance emulation one frame"

#define ACTION_TOGGLE_FULLSCREEN        "toggle-fullscreen"
#define ACTION_TOGGLE_FULLSCREEN_DESC   "Toggle fullscreen"

#define ACTION_TOGGLE_FULLSCREEN_DECORATIONS        "toggle-fullscreen-decorations"
#define ACTION_TOGGLE_FULLSCREEN_DECORATIONS_DESC   "Show menu/status in fullscreen"

#define ACTION_TOGGLE_CONTROLPORT_SWAP          "toggle-joyport-swap"
#define ACTION_TOGGLE_CONTROLPORT_SWAP_DESC     "Swap controlport joysticks"

#define ACTION_TOGGLE_USERPORT_SWAP             "toggle-userport-swap"
#define ACTION_TOGGLE_USERPORT_SWAP_DESC        "Swap userport joysticks"



#endif
