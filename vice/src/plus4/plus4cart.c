/*
 * plus4cart.c -- Plus4 cartridge handling.
 *
 * Written by
 *  Tibor Biczo <crown@axelero.hu>
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

#include "plus4cart.h"
#include "plus4mem.h"
#include "log.h"
#include "util.h"
#include "machine.h"
#include "resources.h"
#include <string.h>

int plus4cart_load_func_lo(const char *rom_name)
{
    /* Load cartridge to Function low ROM.  */
    if (*rom_name != 0) {
        if (util_file_load(rom_name,
            extromlo1, PLUS4_CART16K_SIZE, UTIL_FILE_LOAD_FILL) < 0) {
            log_error(LOG_ERR,
                      "Couldn't load function low ROM `%s'.",
                      rom_name);
            return -1;
        }
    }
    return 0;
}

int plus4cart_load_func_hi(const char *rom_name)
{
    /* Load cartridge to Function high ROM.  */
    if (*rom_name != 0) {
        if (util_file_load(rom_name,
            extromhi1, PLUS4_CART16K_SIZE, UTIL_FILE_LOAD_FILL) < 0) {
            log_error(LOG_ERR,
                      "Couldn't load function high ROM `%s'.",
                      rom_name);
            return -1;
        }
    }
    return 0;
}

int plus4cart_load_c1lo(const char *rom_name)
{
    /* Load c1 low ROM.  */
    if (*rom_name != 0) {
        if (util_file_load(rom_name,
            extromlo2, PLUS4_CART16K_SIZE, UTIL_FILE_LOAD_FILL) < 0) {
            log_error(LOG_ERR,
                      "Couldn't load cartridge 1 low ROM `%s'.",
                      rom_name);
            return -1;
        }
    }
    return 0;
}

int plus4cart_load_c1hi(const char *rom_name)
{
    /* Load c1 high ROM.  */
    if (*rom_name != 0) {
        if (util_file_load(rom_name,
            extromhi2, PLUS4_CART16K_SIZE, UTIL_FILE_LOAD_FILL) < 0) {
            log_error(LOG_ERR,
                      "Couldn't load cartridge 1 high ROM `%s'.",
                      rom_name);
            return -1;
        }
    }
    return 0;
}

int plus4cart_load_c2lo(const char *rom_name)
{
    /* Load c2 low ROM.  */
    if (*rom_name != 0) {
        if (util_file_load(rom_name,
            extromlo3, PLUS4_CART16K_SIZE, UTIL_FILE_LOAD_FILL) < 0) {
            log_error(LOG_ERR,
                      "Couldn't load cartridge 2 low ROM `%s'.",
                      rom_name);
            return -1;
        }
    }
    return 0;
}

int plus4cart_load_c2hi(const char *rom_name)
{
    /* Load c2 high ROM.  */
    if (*rom_name != 0) {
        if (util_file_load(rom_name,
            extromhi3, PLUS4_CART16K_SIZE, UTIL_FILE_LOAD_FILL) < 0) {
            log_error(LOG_ERR,
                      "Couldn't load cartridge 2 high ROM `%s'.",
                      rom_name);
            return -1;
        }
    }
    return 0;
}

void plus4cart_detach_cartridges(void)
{
    resources_set_value("c1loName", "");
    resources_set_value("c1hiName", "");
    resources_set_value("c2loName", "");
    resources_set_value("c2hiName", "");

    memset(extromlo2, 0, PLUS4_CART16K_SIZE);
    memset(extromhi2, 0, PLUS4_CART16K_SIZE);
    memset(extromlo3, 0, PLUS4_CART16K_SIZE);
    memset(extromhi3, 0, PLUS4_CART16K_SIZE);

    machine_trigger_reset(MACHINE_RESET_MODE_HARD);
}
