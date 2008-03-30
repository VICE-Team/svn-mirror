/*
 * opencbmlib.c - Interface to access the opencbm library.
 *
 * Written by
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

#ifdef HAVE_OPENCBM

#include "opencbmlib.h"


#define GET_PROC_ADDRESS_AND_TEST(_name_) \
    opencbmlib->p_##_name_ = _name_;


int opencbmlib_open(opencbmlib_t *opencbmlib)
{
    GET_PROC_ADDRESS_AND_TEST(cbm_driver_open);
    GET_PROC_ADDRESS_AND_TEST(cbm_driver_close);
    GET_PROC_ADDRESS_AND_TEST(cbm_get_driver_name);
    GET_PROC_ADDRESS_AND_TEST(cbm_listen);
    GET_PROC_ADDRESS_AND_TEST(cbm_talk);
    GET_PROC_ADDRESS_AND_TEST(cbm_open);
    GET_PROC_ADDRESS_AND_TEST(cbm_close);
    GET_PROC_ADDRESS_AND_TEST(cbm_raw_read);
    GET_PROC_ADDRESS_AND_TEST(cbm_raw_write);
    GET_PROC_ADDRESS_AND_TEST(cbm_unlisten);
    GET_PROC_ADDRESS_AND_TEST(cbm_untalk);
    GET_PROC_ADDRESS_AND_TEST(cbm_get_eoi);
    GET_PROC_ADDRESS_AND_TEST(cbm_reset);

    return 0;
}

void opencbmlib_close(void)
{
}

#endif

