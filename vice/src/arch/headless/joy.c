/** \file   joy.h
 * \brief   Headless Joystick support
 *
 * \author  David Hogan <david.q.hogan@gmail.com>
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

int joy_arch_cmdline_options_init(void)
{
	/* printf("%s\n", __func__); */

	return 0;
}

int joy_arch_init(void)
{
	/* printf("%s\n", __func__); */

	return 0;
}

int joy_arch_resources_init(void)
{
	/* printf("%s\n", __func__); */

	return 0;
}

int joy_arch_set_device(int port_idx, int new_dev)
{
	/* printf("%s\n", __func__); */

	return 0;
}

void joystick(void)
{
	/* printf("%s\n", __func__); */
}

void joystick_close(void)
{
	/* printf("%s\n", __func__); */
}