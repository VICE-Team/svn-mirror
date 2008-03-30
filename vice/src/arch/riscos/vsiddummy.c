/*
 * vsiddummy.c - Dummy sidplayer stubs for non-C64 targets
 *
 * Written by
 *  Andreas Dehmel <zarquon@t-online.de>
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

struct wimp_msg_desc_s;

int vsid_ui_load_file(const char *file)
{
  return 0;
}

int vsid_ui_mouse_click(int *block)
{
  return 0;
}

int vsid_ui_key_press(int *block)
{
  return 0;
}

int vsid_ui_message_hook(struct wimp_msg_desc_s *msg)
{
  return 0;
}

void vsid_ui_display_speed(int percent)
{
}
