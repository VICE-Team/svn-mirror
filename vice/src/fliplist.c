/*
 * fliplist.c
 *
 * Written by
 *  Martin Pottendorfer <Martin.Pottendorfer@aut.alcatel.at>
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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "archdep.h"
#include "attach.h"
#include "cmdline.h"
#include "fliplist.h"
#include "lib.h"
#include "log.h"
#include "resources.h"
#include "util.h"


#define NUM_DRIVES 4


struct fliplist_t {
    struct fliplist_t *next, *prev;
    char *image;
    unsigned int unit;
};

static struct fliplist_t *fliplist[NUM_DRIVES] = {
    (struct fliplist_t *)NULL,
    (struct fliplist_t *)NULL
};

static char *current_image = (char *)NULL;
static unsigned int current_drive;
static struct fliplist_t *iterator;

static const char flip_file_header[] = "# Vice fliplist file";

#define buffer_size 1024

static void show_fliplist(unsigned int unit);

static char *fliplist_file_name = NULL;


static int set_fliplist_file_name(resource_value_t v, void *param)
{
    if (util_string_set(&fliplist_file_name, (const char *)v))
        return 0;

    flip_load_list((unsigned int)-1, fliplist_file_name, 0);

    return 0;
}

static resource_t resources[] = {
    { "FliplistName", RES_STRING, NULL,
      (void *)&fliplist_file_name, set_fliplist_file_name, NULL },
    { NULL }
};

int fliplist_resources_init(void)
{
    resources[0].factory_value
        = (resource_value_t)archdep_default_fliplist_file_name();

    if (resources_register(resources) < 0)
        return -1;

    lib_free(resources[0].factory_value);

    return 0;
}

static const cmdline_option_t cmdline_options[] =
{
    { "-flipname", SET_RESOURCE, 1, NULL, NULL, "FliplistName", NULL,
      "<name>", "Specify name of the flip list file image" },
    { NULL }
};

int fliplist_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */
/* interface functions */

void flip_shutdown(void)
{
    lib_free(current_image);
}

void flip_set_current(unsigned int unit, const char *filename)
{
    if (current_image != NULL)
        lib_free(current_image);
    current_image = lib_stralloc(filename);
    current_drive = unit;
}

char *flip_get_head(unsigned int unit)
{
    if (fliplist[unit - 8])
        return fliplist[unit - 8]->image;
    return (char *) NULL;
}

char *flip_get_next(unsigned int unit)
{
    if (fliplist[unit - 8])
        return fliplist[unit - 8]->next->image;
    return (char *) NULL;
}

char *flip_get_prev(unsigned int unit)
{
    if (fliplist[unit - 8])
        return fliplist[unit - 8]->prev->image;
    return (char *) NULL;
}

char *flip_get_image(void *fl)
{
    return ((struct fliplist_t *) fl)->image;
}

unsigned int flip_get_unit(void *fl)
{
    return ((struct fliplist_t *) fl)->unit;
}

void flip_add_image(unsigned int unit)
{
    struct fliplist_t *n;

    if (current_image == NULL)
        return;
    if (strcmp(current_image, "") == 0)
        return;

    n = (struct fliplist_t *)lib_malloc(sizeof(struct fliplist_t));
    n->image = lib_stralloc(current_image);
    unit = n->unit = current_drive;

    log_message(LOG_DEFAULT, "Adding `%s' to fliplist[%d]", n->image, unit);
    if (fliplist[unit - 8]) {
        n->next = fliplist[unit - 8];
        n->prev = fliplist[unit - 8]->prev;
        n->next->prev = n;
        n->prev->next = n;
        fliplist[unit - 8] = n;
    } else {
        fliplist[unit - 8] = n;
        n->next = n;
        n->prev = n;
    }
    show_fliplist(unit);
}

void flip_remove(unsigned int unit, char *image)
{
    struct fliplist_t *tmp;

    if (fliplist[unit - 8] == (struct fliplist_t *) NULL)
        return;
    if (image == (char *) NULL) {
        /* no image given, so remove the head */
        if ((fliplist[unit - 8] == fliplist[unit - 8]->next) &&
            (fliplist[unit - 8] == fliplist[unit - 8]->prev)) {
            /* this is the last entry */
            tmp = fliplist[unit - 8];
            fliplist[unit - 8] = (struct fliplist_t *) NULL;
        } else {
            fliplist[unit - 8]->next->prev = fliplist[unit - 8]->prev;
            fliplist[unit - 8]->prev->next = fliplist[unit - 8]->next;
            tmp = fliplist[unit - 8];
            fliplist[unit - 8] = fliplist[unit - 8]->next;
        }
        log_message(LOG_DEFAULT, "Removing `%s' from fliplist[%d]",
                    tmp->image, unit);
        lib_free(tmp->image);
        lib_free(tmp);
        show_fliplist(unit);
        return;
    } else {
        /* do a lookup and remove it */
        struct fliplist_t *it = fliplist[unit - 8];

        if (strcmp(it->image, image) == 0) {
            /* it's the head */
            flip_remove(unit, NULL);
            return;
        }
        it = it->next;
        while ((strcmp(it->image, image) != 0) &&
               (it != fliplist[unit - 8]))
            it = it->next;

        if (it == fliplist[unit - 8]) {
            log_message(LOG_DEFAULT,
                        "Cannot remove `%s'; not found in fliplist[%d]",
                        it->image, unit);
            return;
        }

        it->next->prev = it->prev;
        it->prev->next = it->next;
        lib_free(it->image);
        lib_free(it);
        show_fliplist(unit);
    }
}

void flip_attach_head (unsigned int unit, int direction)
{
    if (fliplist[unit - 8] == (struct fliplist_t *)NULL)
        return;

    if (direction)
        fliplist[unit - 8] = fliplist[unit - 8]->next;
    else
        fliplist[unit - 8] = fliplist[unit - 8]->prev;

    if (file_system_attach_disk(fliplist[unit - 8]->unit,
                                fliplist[unit - 8]->image) < 0) {
        /* shouldn't happen, so ignore it */
        ;
    }
}

void *flip_init_iterate(unsigned int unit)
{
    void *ret = NULL;

    iterator = fliplist[unit - 8];
    if (iterator) {
        ret = (void *)iterator;
        iterator = iterator->next;
    }
    return ret;
}

void *flip_next_iterate(unsigned int unit)
{
    void *ret = NULL;

    if (iterator) {
        if (iterator != fliplist[unit - 8]) {
            ret = (void *)iterator;
            iterator=iterator->next;
        }
    }
    return ret;
}

void flip_clear_list(unsigned int unit)
{
    struct fliplist_t *flip = fliplist[unit - 8];

    if (flip != NULL) {
        do {
            struct fliplist_t *tmp = flip->next;

            lib_free(flip->image);
            lib_free(flip);
            flip = tmp;
        }
        while (flip != fliplist[unit - 8]);

        fliplist[unit - 8] = NULL;
    }
}

int flip_save_list(unsigned int unit, const char *filename)
{
    int all_units = 0;
    struct fliplist_t *flip;
    FILE *fp = NULL;

    if (unit == (unsigned int)-1) {
        all_units = 1;
        unit = 8;
    }

    do {
        flip = fliplist[unit - 8];

        if (flip != (struct fliplist_t *)NULL) {
            if (!fp) {
                if ((fp = fopen(filename, MODE_WRITE)) == NULL)
                    return -1;
                fprintf(fp, "%s\n\n", flip_file_header);
            }

            fprintf(fp, "UNIT %d\n", unit);
            do {
                fprintf(fp, "%s\n", flip->image);
                flip = flip->next;
            }
            while (flip != fliplist[unit - 8]);

        }
        unit++;
    } while (all_units && ((unit - 8) < NUM_DRIVES));

    if (fp)
        fclose(fp);
    return 0;
}

int flip_load_list(unsigned int unit, const char *filename, int autoattach)
{
    FILE *fp;
    char buffer[buffer_size];
    int all_units = 0, i;

    if (filename == NULL || (fp = fopen(filename, MODE_READ)) == NULL)
        return -1;

    buffer[0] = '\0';
    fgets(buffer, buffer_size, fp);

    if (strncmp(buffer, flip_file_header, strlen(flip_file_header)) != 0) {
        log_message(LOG_DEFAULT, "File %s is not a fliplist file", filename);
        fclose(fp);
        return -1;
    }
    if (unit == (unsigned int)-1) {
        all_units = 1;
        for (i = 0; i < NUM_DRIVES; i++)
            flip_clear_list(i+8);
    }
    else
        flip_clear_list(unit);

    while (!feof(fp)) {
        char *b;

        buffer[0] = '\0';
        fgets(buffer, buffer_size, fp);

        if (all_units && strncmp("UNIT ", buffer, 5) == 0) {
            long unit_long;

            util_string_to_long(buffer + 5, NULL, 10, &unit_long);

            unit = (unsigned int)unit_long;
            continue;
        }

        /* remove trailing whitespace (linefeeds etc) */
        b = buffer + strlen(buffer);
        while ((b > buffer) && (isspace((unsigned int)(b[-1]))))
            b--;

        if (b > buffer) {
            struct fliplist_t *tmp;

            *b = '\0';

            if (unit == (unsigned int)-1) {
                log_message(LOG_DEFAULT, "Fliplist has inconsistent view for unit, assuming 8.\n");
                unit = 8;
            }

            tmp = (struct fliplist_t*)lib_malloc(sizeof(struct fliplist_t));
            tmp->image = lib_stralloc(buffer);
            tmp->unit = unit;

            if (fliplist[unit - 8] == NULL) {
                fliplist[unit - 8] = tmp;
                tmp->prev = tmp;
                tmp->next = tmp;
            } else {
                tmp->next = fliplist[unit - 8];
                tmp->prev = fliplist[unit - 8]->prev;
                tmp->next->prev = tmp;
                tmp->prev->next = tmp;
                fliplist[unit - 8] = tmp;
            }
        }
    }

    current_drive = unit;

    fclose(fp);

    if (all_units) {
        for (i = 0; i < NUM_DRIVES; i++)
            show_fliplist(i + 8);
    } else
        show_fliplist(unit);


    if (autoattach)
        flip_attach_head(unit, 1);

    return 0;
}

/* ------------------------------------------------------------------------- */

static void show_fliplist(unsigned int unit)
{
    struct fliplist_t *it = fliplist[unit - 8];

    log_message(LOG_DEFAULT, "Fliplist[%d] contains:", unit);

    if (it) {
        do {
            log_message(LOG_DEFAULT,
                        "\tUnit %d %s (n: %s, p:%s)", it->unit, it->image,
                        it->next->image, it->prev->image);
            it = it->next;
        } while (it != fliplist[unit - 8]);
    } else
        log_message(LOG_DEFAULT, "\tnothing");
}

