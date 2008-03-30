/*
 * palette.c - Palette handling.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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
#include "log.h"
#include "palette.h"
#include "sysfile.h"
#include "types.h"
#include "utils.h"

static log_t palette_log = LOG_ERR;

palette_t *palette_create(unsigned int num_entries, const char *entry_names[])
{
    palette_t *p;
    unsigned int i;

    p = (palette_t *)xmalloc(sizeof(palette_t));

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
    unsigned int i;

    if (p == NULL)
        return;

    for (i = 0; i < p->num_entries; i++)
        if (p->entries[i].name != NULL)
            free(p->entries[i].name);

    free(p->entries);
    free(p);
}

static int palette_set_entry(palette_t *p, unsigned int number,
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

static int palette_copy(palette_t *dest, const palette_t *src)
{
    unsigned int i;

    if (dest->num_entries != src->num_entries) {
        log_error(palette_log,
                  "Number of entries of src and dest palette do not match.");
        return -1;
    }

    for (i = 0; i < src->num_entries; i++)
        palette_set_entry(dest, i, src->entries[i].red, src->entries[i].green,
                          src->entries[i].blue, src->entries[i].dither);

    return 0;
}

static char *next_nonspace(const char *p)
{
    while (*p != '\0' && isspace((int)*p))
        p++;

    return (char *)p;
}

int palette_load(const char *file_name, palette_t *palette_return)
{
    char buf[1024];
    unsigned int line_num, entry_num;
    int line_len, err = -1;
    palette_t *tmp_palette;
    char *complete_path;
    FILE *f;

    f = sysfile_open(file_name, &complete_path, MODE_READ_TEXT);
    if (f == NULL) {
        /* Try to add the extension.  */
        char *tmp = stralloc(file_name);

        util_add_extension(&tmp, "vpl");
        f = sysfile_open(tmp, &complete_path, MODE_READ_TEXT);
        free(tmp);
        if (f == NULL)
            return -1;
    }

    log_message(palette_log, "Loading palette `%s'.", complete_path);
    free(complete_path);

    tmp_palette = palette_create(palette_return->num_entries, NULL);

    line_num = entry_num = 0;
    line_len = util_get_line(buf, 1024, f);

    if (line_len < 0) {
        log_error(palette_log, "Could not read from palette file.");
        goto return_err;
    }

    while (line_len >= 0) {
        line_num++;
        if (line_len > 0 && *buf != '#') {
            int i;
            BYTE values[4];
            const char *p1 = next_nonspace(buf);

            for (i = 0; i < 4; i++) {
                long result;
                const char *p2;

                if (i == 0 && *p1 == '\0') /* empty line */
                    break;
                if (util_string_to_long(p1, &p2, 16, &result) < 0) {
                    log_error(palette_log, "%s, %d: number expected.",
                              file_name, line_num);
                    goto return_err;
                }
                if (result < 0
                    || (i == 3 && result > 0xf)
                    || result > 0xff
                    || result < 0) {
                    log_error(palette_log, "%s, %d: invalid value.",
                              file_name, line_num);
                    goto return_err;
                }
                values[i] = (BYTE)result;
                p1 = p2;
            }

            if (i > 0) {
                p1 = next_nonspace(p1);
                if (*p1 != '\0') {
                    log_error(palette_log,
                              "%s, %d: garbage at end of line.",
                              file_name, line_num);
                    goto return_err;
                }
                if (entry_num >= palette_return->num_entries) {
                    log_error(palette_log,
                              "%s: too many entries.", file_name);
                    goto return_err;
                }
                if (palette_set_entry(tmp_palette, entry_num,
                    values[0], values[1], values[2], values[3]) < 0) {
                    log_error(palette_log, "Failed to set palette entry.");
                    goto return_err;
                }
                entry_num++;
            }
        }
        line_len = util_get_line(buf, 1024, f);
    }

    if (entry_num < palette_return->num_entries) {
        log_error(palette_log, "%s: too few entries.", file_name);
        goto return_err;
    }

    if (palette_copy(palette_return, tmp_palette) < 0) {
        log_error(palette_log, "Failed to copy palette.");
        goto return_err;
    }

    err = 0;

return_err:
    fclose(f);
    palette_free(tmp_palette);

    return err;
}

int palette_save(const char *file_name, const palette_t *palette)
{
    unsigned int i;
    FILE *f;

    f = fopen(file_name, MODE_WRITE);

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

void palette_init(void)
{
    palette_log = log_open("Palette");
}

