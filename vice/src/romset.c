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
#include "romset.h"

int romset_load(const char *filename) {
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

int romset_dump(const char *filename, const char **resource_list) {
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

int romset_load_archive(const char *filename, int autostart)
{
    FILE *fp;
    int line_num;
    char buffer[256];
    string_link_t *autoset = NULL;

    if ((fp = fopen(filename, "r")) == NULL)
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
        int entry;

        READ_ROM_LINE;
        if (bptr == NULL) break;
        if ((*b == '\n') || (*b == '#')) continue;
        length = strlen(b);
        for (entry=0, item=romsets; entry<num_romsets; entry++, item++)
        {
            if (strncmp(item->name, b, length-1) == 0) break;
        }
        if (entry >= array_size)
        {
            array_size += 4;
            if (romsets == NULL)
                romsets = (string_link_t*)xmalloc(array_size * sizeof(string_link_t));
            else
                romsets = (string_link_t*)xrealloc(romsets, array_size * sizeof(string_link_t));
        }
        anchor = romsets + entry;
        if (entry < num_romsets)
        {
            item = anchor->next;
            while (item != NULL)
            {
                last = item; item = item->next;
                free(last->name); free(last);
            }
        }
        else
        {
            anchor->name = (char*)xmalloc(length);
            strncpy(anchor->name, b, length-1); anchor->name[length-1] = '\0';
        }
        anchor->next = NULL;

        if ((autostart != 0) && (autoset == NULL)) autoset = anchor;

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
        if (entry >= num_romsets) num_romsets++;
    }

    fclose(fp);

    if (autoset != 0)
        romset_select_item(autoset->name);

    return 0;
}


int romset_dump_archive(const char *filename)
{
    FILE *fp;
    string_link_t *item;
    int i;

    if ((fp = fopen(filename, "w")) == NULL)
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


int romset_save_item(const char *filename, const char *romset_name)
{
    int i;

    for (i=0; i<num_romsets; i++)
    {
        if (strcmp(romsets[i].name, romset_name) == 0)
        {
            string_link_t *item;
            FILE *fp;

            if ((fp = fopen(filename, "w")) == NULL)
            {
                log_warning(LOG_DEFAULT, "Could not open file '%s' for writing", filename);
                return -1;
            }
            item = romsets + i;
            fprintf(fp, "%s\n", item->name);
            fprintf(fp, "{\n");
            while (item->next != NULL)
            {
                item = item->next; fprintf(fp, "\t%s\n", item->name);
            }
            fprintf(fp, "}\n");
            fclose(fp);

            return 0;
        }
    }
    return -1;
}


int romset_select_item(const char *romset_name)
{
    int i;
    string_link_t *item;

    for (i=0, item=romsets; i<num_romsets; i++, item++)
    {
        if (strcmp(romset_name, item->name) == 0)
        {
            while (item->next != NULL)
            {
                char buffer[256];
                char *b, *d;

                item = item->next;
                b = buffer; d = item->name;
                while (*d != '\0')
                {
                    if (*d == '=') break;
                    else *b++ = *d++;
                }
                *b++ = '\0';
                if (*d == '=')
                {
                    resource_type_t tp;
                    resource_value_t val = (resource_value_t)0;
                    char *arg;

                    arg = b; d++;
                    while (*d != '\0')
                    {
                        if (*d != '\"') *b++ = *d;
                        d++;
                    }
                    *b++ = '\0';
                    tp = resources_query_type(buffer);
                    switch (tp)
                    {
                        case RES_INTEGER: val = (resource_value_t)atoi(arg); break;
                        case RES_STRING: val = (resource_value_t)arg; break;
                        default: b = NULL; break;
                    }
                    if (b != NULL)
                    {
                        resources_set_value(buffer, val);
                    }
                }
            }
            /*mem_load();*/
            return 0;
        }
    }
    return -1;
}


int romset_create_item(const char *romset_name, const char **resource_list)
{
    int entry;
    string_link_t *anchor, *item, *last;
    const char **res;

    for (entry=0, item=romsets; entry<num_romsets; entry++, item++)
    {
        if (strcmp(romset_name, item->name) == 0) break;
    }
    if (entry >= array_size)
    {
        array_size += 4;
        if (romsets == NULL)
            romsets = (string_link_t*)xmalloc(array_size * sizeof(string_link_t));
        else
            romsets = (string_link_t*)xrealloc(romsets, array_size * sizeof(string_link_t));
    }
    anchor = romsets + entry;
    if (entry < num_romsets)
    {
        item = anchor->next;
        while (item != NULL)
        {
            last = item; item = item->next;
            free(last->name); free(last);
        }
    }
    else
    {
        anchor->name = (char*)xmalloc(strlen(romset_name)+1);
        strcpy(anchor->name, romset_name);
    }
    anchor->next = NULL;

    last = anchor;
    res = resource_list;
    while (*res != NULL)
    {
        resource_type_t tp;
        resource_value_t val;
        int len;

        if (resources_get_value(*res, &val) == 0)
        {
            char buffer[256];

            tp = resources_query_type(*res);
            switch (tp)
            {
                case RES_INTEGER: sprintf(buffer, "%s=%d", *res, (int)val); break;
                case RES_STRING: sprintf(buffer, "%s=\"%s\"", *res, (char*)val); break;
                default: buffer[0] = '\0';
            }
            if ((len = strlen(buffer)) > 0)
            {
              item = xmalloc(sizeof(string_link_t));
              item->name = xmalloc(len+1); strcpy(item->name, buffer);
              item->next = NULL; last->next = item; last = item;
            }
        }
        res++;
    }

    if (entry >= num_romsets) num_romsets++;

    return 0;
}


int romset_delete_item(const char *romset_name)
{
    string_link_t *item;
    int i;

    for (i=0, item=romsets; i<num_romsets; i++, item++)
    {
        if (strcmp(romset_name, item->name) == 0)
        {
            string_link_t *last;

            free(item->name); item = item->next;
            while (item != NULL)
            {
                last = item; item = item->next;
                free(last->name); free(last);
            }
            while (i < num_romsets-1)
            {
                romsets[i] = romsets[i+1]; i++;
            }
            num_romsets--;

            return 0;
        }
    }
    return -1;
}


void romset_clear_archive(void)
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
    if (romsets != NULL) {free(romsets); romsets = NULL;}

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
