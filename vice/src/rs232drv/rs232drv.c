/*
 * rs232drv.c - Common RS232 driver handling.
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

#include "archdep.h"
#include "cmdline.h"
#include "lib.h"
#include "resources.h"
#include "rs232.h"
#include "rs232drv.h"
#ifdef HAS_TRANSLATION
#include "translate.h"
#endif
#include "types.h"
#include "util.h"


#ifdef HAVE_RS232

#define NUM_DEVICES 4

/*static*/ char *devfile[NUM_DEVICES] = { NULL, NULL, NULL, NULL };

static int set_devfile(resource_value_t v, void *param)
{
    util_string_set(&devfile[(int)param], (const char *)v);
    return 0;
}

/* ------------------------------------------------------------------------- */

static const resource_t resources[] = {
    { "RsDevice1", RES_STRING, (resource_value_t)ARCHDEP_RS232_DEV1,
      (void *)&devfile[0], set_devfile, (void *)0 },
    { "RsDevice2", RES_STRING, (resource_value_t)ARCHDEP_RS232_DEV2,
      (void *)&devfile[1], set_devfile, (void *)1 },
    { "RsDevice3", RES_STRING, (resource_value_t)ARCHDEP_RS232_DEV3,
      (void *)&devfile[2], set_devfile, (void *)2 },
    { "RsDevice4", RES_STRING, (resource_value_t)ARCHDEP_RS232_DEV4,
      (void *)&devfile[3], set_devfile, (void *)3 },
    { NULL }
};

int rs232drv_resources_init(void)
{
    if (resources_register(resources) < 0)
        return -1;

    return rs232_resources_init();
}

void rs232drv_resources_shutdown(void)
{
    lib_free(devfile[0]);
    lib_free(devfile[1]);
    lib_free(devfile[2]);
    lib_free(devfile[3]);

    rs232_resources_shutdown();
}

#ifdef HAS_TRANSLATION
static const cmdline_option_trans_t cmdline_options[] = {
    { "-rsdev1", SET_RESOURCE, 1, NULL, NULL, "RsDevice1", NULL,
      IDCLS_P_NAME, IDCLS_SPECIFY_RS232_1_NAME },
    { "-rsdev2", SET_RESOURCE, 1, NULL, NULL, "RsDevice2", NULL,
      IDCLS_P_NAME, IDCLS_SPECIFY_RS232_2_NAME },
    { "-rsdev3", SET_RESOURCE, 1, NULL, NULL, "RsDevice3", NULL,
      IDCLS_P_NAME, IDCLS_SPECIFY_RS232_3_NAME },
    { "-rsdev4", SET_RESOURCE, 1, NULL, NULL, "RsDevice4", NULL,
      IDCLS_P_NAME, IDCLS_SPECIFY_RS232_4_NAME },
    { NULL }
};
#else
static const cmdline_option_t cmdline_options[] = {
    { "-rsdev1", SET_RESOURCE, 1, NULL, NULL, "RsDevice1", NULL,
      "<name>", N_("Specify name of first RS232 device") },
    { "-rsdev2", SET_RESOURCE, 1, NULL, NULL, "RsDevice2", NULL,
      "<name>", N_("Specify name of second RS232 device") },
    { "-rsdev3", SET_RESOURCE, 1, NULL, NULL, "RsDevice3", NULL,
      "<name>", N_("Specify name of third RS232 device") },
    { "-rsdev4", SET_RESOURCE, 1, NULL, NULL, "RsDevice4", NULL,
      "<name>", N_("Specify name of fourth RS232 device") },
    { NULL }
};
#endif

int rs232drv_cmdline_options_init(void)
{
#ifdef HAS_TRANSLATION
    if (cmdline_register_options_trans(cmdline_options) < 0)
#else
    if (cmdline_register_options(cmdline_options) < 0)
#endif
        return -1;

    return rs232_cmdline_options_init();
}

void rs232drv_init(void)
{
    rs232_init();
}

void rs232drv_reset(void)
{
    rs232_reset();
}

int rs232drv_open(int device)
{
    return rs232_open(device);
}

void rs232drv_close(int fd)
{
    rs232_close(fd);
}

int rs232drv_putc(int fd, BYTE b)
{
    return rs232_putc(fd, b);
}

int rs232drv_getc(int fd, BYTE *b)
{
    return rs232_getc(fd, b);
}

#else

void rs232drv_init(void)
{

}

void rs232drv_reset(void)
{

}

int rs232drv_open(int device)
{
    return -1;
}

void rs232drv_close(int fd)
{
}

int rs232drv_putc(int fd, BYTE b)
{
    return -1;
}

int rs232drv_getc(int fd, BYTE *b)
{
    return -1;
}

int rs232drv_resources_init(void)
{
    return 0;
}

void rs232drv_resources_shutdown(void)
{
}

int rs232drv_cmdline_options_init(void)
{
    return 0;
}

#endif

