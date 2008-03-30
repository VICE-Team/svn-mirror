/*
 * romset.c - romset file handling
 *
 * Written by
 *  Andre Fachat        (a.fachat@physik.tu-chemnitz.de)
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

#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>

#include "config.h"
#include "types.h"
#include "mem.h"
#include "resources.h"
#include "sysfile.h"
#include "log.h"
#include "utils.h"

extern int romset_load(const char *filename) {
    FILE *fp;
    int retval, line_num;
    int err = 0;

    fp = sysfile_open(filename, NULL);

    if(!fp) {
        log_warning(LOG_DEFAULT, "Could not open file '%s' for reading (%s)!",
		filename,strerror(errno));
	return -1;
    }

    log_message(LOG_DEFAULT, "Loading ROM set from file '%s'",filename);

    line_num = 0;
    do {
        retval = resources_read_item_from_file(fp);
        if (retval == -1) {
            log_error(LOG_DEFAULT,
                      "%s: Invalid resource specification at line %d.",
                      filename, line_num);
            err = 1;
        }
        line_num++;
    } while (retval != 0);
    fclose(fp);

    /* mem_load(); setting the resources is now enought */

    return err;
}

extern int romset_dump(const char *filename, const char **resource_list) {
    FILE *fp;
    const char *s;

    fp = fopen(filename, "w");
    if (fp) {
        log_message(LOG_DEFAULT, "Dumping ROM set to file '%s'",filename);
	s = *resource_list++;
	while(s) {
	    resources_write_item_to_file(fp, s);
	    s = *resource_list++;
	}
	fclose(fp);
	return 0;
    }
    log_warning(LOG_DEFAULT, "Could not open file '%s' for writing (%s)!",
		filename,strerror(errno));
    return -1;
}




typedef struct string_link_t {
  char *name;
  struct string_link_t *next;
} string_link_t;


static int num_romsets = 0;
static int array_size = 0;
static string_link_t *romsets = NULL;



#define READ_ROM_LINE \
    if ((bptr = fgets(buffer, 256, fp)) != NULL) \
    { \
        line_num++; b = buffer; \
        while ((*b == ' ') || (*b == '\t')) b++; \
    }

int romset_load_archive(const char *filename)
{
    FILE *fp;
    int line_num;
    char buffer[256];

    if ((fp = sysfile_open(filename, NULL)) == NULL)
    {
        log_warning(LOG_DEFAULT, "Could not open file '%s' for reading!", filename);
        return -1;
    }

    log_message(LOG_DEFAULT, "Loading ROM sets from file '%s'", filename);

    line_num = 0;
    while (!feof(fp))
    {
        char *b=NULL, *bptr;
        string_link_t *anchor, *item, *last;
        int length;

        READ_ROM_LINE;
        if (bptr == NULL) break;
        if ((*b == '\n') || (*b == '#')) continue;
        length = strlen(b);
        if (num_romsets >= array_size)
        {
            array_size += 4;
            if (romsets == NULL)
                romsets = (string_link_t*)xmalloc(array_size * sizeof(string_link_t));
            else
                romsets = (string_link_t*)xrealloc(romsets, array_size * sizeof(string_link_t));
        }
        anchor = romsets + num_romsets;
        anchor->name = (char*)xmalloc(length);
        strncpy(anchor->name, b, length-1); anchor->name[length-1] = '\0';
        anchor->next = NULL;

        READ_ROM_LINE
        if ((bptr == NULL) || (*b != '{'))
        {
            log_warning(LOG_DEFAULT, "Parse error at line %d", line_num);
            fclose(fp); return -1;
        }
        last = anchor;
        while (!feof(fp))
        {
            READ_ROM_LINE
            if (bptr == NULL)
            {
                log_warning(LOG_DEFAULT, "Parse error at line %d", line_num);
                fclose(fp); return -1;
            }
            if (*b == '}') break;
            length = strlen(b);
            item = (string_link_t*)xmalloc(sizeof(string_link_t));
            item->name = (char*)xmalloc(length);
            strncpy(item->name, b, length-1); item->name[length-1] = '\0';
            item->next = NULL;
            last->next = item; last = item;
        }
        num_romsets++;
    }

    fclose(fp);

    return 0;
}


int romset_dump_archive(const char *filename)
{
    FILE *fp;
    string_link_t *item;
    int i;

    if ((fp = sysfile_open(filename, NULL)) == NULL)
    {
        log_warning(LOG_DEFAULT, "Could not open file '%s' for writing!", filename);
        return -1;
    }

    log_message(LOG_DEFAULT, "Saving ROM sets to file '%s'", filename);

    for (i=0; i<num_romsets; i++)
    {
        item = romsets + i;
        fprintf(fp, "%s\n", item->name);
        fprintf(fp, "{\n");
        while (item->next != NULL)
        {
            item = item->next; fprintf(fp, "\t%s\n", item->name);
        }
        fprintf(fp, "}\n");
    }

    fclose(fp);

    return 0;
}


int romset_select(const char *romset_name)
{
    int i;
    string_link_t *item;

    item = romsets;
    for (i=0; i<num_romsets; i++, item++)
    {
        if (strcmp(romset_name, item->name) == 0)
        {
            while (item->next != NULL)
            {
                item = item->next;
                /* FIXME, this is currently just a dummy.
                   I suggest splitting up resources_read_item_from_file into <idem>
                   and resources_read_item_from_buffer. */
                printf("Set resource %s\n", item->name);
            }
            return 0;
        }
    }
    return -1;
}


void romset_clear(void)
{
    int i;
    string_link_t *item, *last;

    for (i=0; i<num_romsets; i++)
    {
        item = romsets + i;
        free(item->name); item = item->next;
        while (item != NULL)
        {
            last = item; item = item->next;
            free(last->name); free(last);
        }
    }
    if (romsets != NULL) free(romsets);

    num_romsets = 0; array_size = 0;
}


int romset_get_number(void)
{
    return num_romsets;
}


char *romset_get_item(int number)
{
    if ((number < 0) || (number >= num_romsets)) return NULL;

    return romsets[number].name;
}
