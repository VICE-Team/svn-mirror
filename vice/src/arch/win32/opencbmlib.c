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

#include "log.h"
#include "opencbmlib.h"


static HINSTANCE opencbm_dll = NULL;


/* Macro for getting function pointers from opencbm dll.  */
#define GET_PROC_ADDRESS_AND_TEST(_name_)                                      \
    opencbmlib->p_##_name_ = (_name_##_t)GetProcAddress(opencbm_dll, #_name_); \
    if (opencbmlib->p_##_name_ == NULL) {                                      \
        log_debug("GetProcAddress " #_name_ " failed!");                       \
    } else { \
        log_debug("GetProcAddress " #_name_ " success!"); \
    }

static const char *CBMAPIDECL dummy_cbm_get_driver_name(int port)
{
    return "OpenCBM";
}

static void opencbmlib_free_library(void)
{
    if (opencbm_dll != NULL) {
        if (!FreeLibrary(opencbm_dll)) {
            log_debug("FreeLibrary opencbm.dll failed!");
        }
    }

    opencbm_dll = NULL;
}

static int opencbmlib_load_library(opencbmlib_t *opencbmlib)
{
    if (opencbm_dll == NULL) {
        opencbm_dll = LoadLibrary("opencbm.dll");

        if (opencbm_dll == NULL) {
            log_debug("LoadLibrary opencbm.dll failed!" );
            return -1;
        }

        GET_PROC_ADDRESS_AND_TEST(cbm_driver_open);
        GET_PROC_ADDRESS_AND_TEST(cbm_driver_close);
#if 0
        GET_PROC_ADDRESS_AND_TEST(cbm_get_driver_name);
#endif
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

        opencbmlib->p_cbm_get_driver_name = dummy_cbm_get_driver_name;
    }

    return 0;
}

int opencbmlib_open(opencbmlib_t *opencbmlib)
{
    return opencbmlib_load_library(opencbmlib);
}

void opencbmlib_close(void)
{
    opencbmlib_free_library();
}

#endif

