/*
 * palette.c - Palette handling.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
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

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "utils.h"
#include "types.h"
#include "palette.h"
#include "sysfile.h"

palette_t *palette_create(int num_entries, const char *entry_names[])
{
    palette_t *p = xmalloc(sizeof(palette_t));
    int i;

    p->num_entries = num_entries;
    p->entries = xmalloc(sizeof(palette_entry_t) * num_entries);
    memset(p->entries, 0, sizeof(palette_entry_t) * num_entries);

    if (entry_names != NULL)
        for (i = 0; i < num_entries; i++)
            p->entries[i].name = stralloc(entry_names[i]);

    return p;
}

void palette_free(palette_t *p)
{
    int i;

    if (p == NULL)
        return;

    for (i = 0; i < p->num_entries; i++)
        if (p->entries[i].name != NULL)
	    free(p->entries[i].name);

    free(p->entries);
    free(p);
}

int palette_set_entry(palette_t *p, int number,
                      BYTE red, BYTE green, BYTE blue, BYTE dither)
{
    if (p == NULL || number >= p->num_entries)
        return -1;

    p->entries[number].red = red;
    p->entries[number].green = green;
    p->entries[number].blue = blue;
    p->entries[number].dither = dither;

    return 0;
}

int palette_copy(palette_t *dest, const palette_t *src)
{
    int i;

    if (dest->num_entries != src->num_entries)
        return -1;

    for (i = 0; i < src->num_entries; i++) {
        dest->entries[i].red = src->entries[i].red;
        dest->entries[i].green = src->entries[i].green;
        dest->entries[i].blue = src->entries[i].blue;
        dest->entries[i].dither = src->entries[i].dither;
    }

    return 0;
}

static char *next_nonspace(const char *p)
{
    while (*p != '\0' && isspace((int) *p))
        p++;

    return (char *) p;
}

int palette_load(const char *file_name, palette_t *palette_return)
{
    char buf[1024];
    int line_num, entry_num;
    palette_t *tmp;
    char *complete_path;
    FILE *f;


    f = sysfile_open(file_name, &complete_path);
    if (f == NULL) {
        /* Try to add the extension.  */
        char *tmp = concat(file_name, PALETTE_FILE_EXTENSION, NULL);

        f = sysfile_open(tmp, &complete_path);
        if (f == NULL)
            return -1;
    }

    printf("Loading palette `%s'.\n", complete_path);
    free(complete_path);

    tmp = palette_create(palette_return->num_entries, NULL);
    line_num = entry_num = 0;
    while (get_line(buf, 1024, f) >= 0) {
        line_num++;
        if (*buf != '#') {
            int i;
            BYTE values[4];
            const char *p1 = next_nonspace(buf);

            for (i = 0; i < 4; i++) {
                long result;
                const char *p2;

                if (i == 0 && *p1 == '\0') /* empty line */
                    break;
                if (string_to_long(p1, &p2, 16, &result) < 0) {
                    fprintf(stderr, "%s, %d: number expected.\n",
                            file_name, line_num);
                    fclose(f);
                    return -1;
                }
                if (result < 0
                    || (i == 3 && result > 0xf)
                    || result > 0xff
                    || result < 0) {
                    fprintf(stderr, "%s, %d: invalid value.\n",
                            file_name, line_num);
                    fclose(f);
                    return -1;
                }
                values[i] = result;
                p1 = p2;
            }
            if (i > 0) {
                p1 = next_nonspace(p1);
                if (*p1 != '\0') {
                    fprintf(stderr, "%s, %d: garbage at end of line.\n",
                            file_name, line_num);
                    fclose(f);
                    return -1;
                }
                if (entry_num >= palette_return->num_entries) {
                    fprintf(stderr, "%s: too many entries.\n", file_name);
                    fclose(f);
                    return -1;
                }
                palette_set_entry(tmp, entry_num,
                                  values[0], values[1], values[2], values[3]);
                entry_num++;
            }
        }
    }

    fclose(f);

    if (entry_num < palette_return->num_entries) {
        fprintf(stderr, "%s: too few entries.\n", file_name);
        return -1;
    }

    palette_copy(palette_return, tmp);
    palette_free(tmp);

    return 0;
}

int palette_save(const char *file_name, const palette_t *palette)
{
    int i;
    FILE *f = fopen(file_name, "w");

    if (f == NULL)
        return -1;

    fprintf(f, "#\n# VICE Palette file\n#\n");
    fprintf(f, "# Syntax:\n# Red Green Blue Dither\n#\n\n");

    for (i = 0; i < palette->num_entries; i++)
        fprintf(f, "# %s\n%02X %02X %02X %01X\n\n",
                palette->entries[i].name,
                palette->entries[i].red,
                palette->entries[i].green,
                palette->entries[i].blue,
                palette->entries[i].dither);

    return fclose(f);
}
