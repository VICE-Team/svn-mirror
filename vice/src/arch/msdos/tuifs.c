/*
 * tuifs.c - A simple text-based file selector.
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

#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <dirent.h>
#include <fnmatch.h>
#include <keys.h>
#include <string.h>
#include <pc.h>
#include <ctype.h>

#include "tui.h"
#include "utils.h"
#include "tuiview.h"

/* ------------------------------------------------------------------------- */

enum file_type { FT_NORMAL, FT_DIR };

struct file_item {
    char name[0x100];
    enum file_type type;
};

struct file_list {
    int num_items;
    int num_used_items;
    struct file_item *items;
};

static struct file_list *file_list_create(void)
{
    struct file_list *new_list;

    new_list = (struct file_list *)malloc(sizeof(struct file_list));
    new_list->num_items = new_list->num_used_items = 0;
    new_list->items = NULL;

    return new_list;
}

static void file_list_clear(struct file_list *fl)
{
    if (fl->items != NULL)
	free(fl->items);

    fl->items = NULL;
    fl->num_used_items = fl->num_items = 0;
}

static void file_list_free(struct file_list *fl)
{
    if (fl != NULL) {
	file_list_clear(fl);
	free(fl);
    }
}

static void file_list_add_item(struct file_list *fl, const char *name,
			       enum file_type type)
{
    if (fl->num_items == fl->num_used_items) {
	fl->num_items += 100;
	if (fl->items != NULL)
	    fl->items = (struct file_item *)realloc(fl->items,
						    fl->num_items
						    * sizeof(*fl->items));
	else
	    fl->items = (struct file_item *)malloc(fl->num_items
						   * sizeof(*fl->items));
	if (fl->items == NULL) {
	    fprintf(stderr, "Virtual memory exhausted.\n");
	    exit(-1);
	}
    }

    strcpy(fl->items[fl->num_used_items].name, name);
    fl->items[fl->num_used_items].type = type;
    fl->num_used_items++;
}

static int file_list_sort_func(const void *e1, const void *e2)
{
    struct file_item *f1 = (struct file_item *)e1;
    struct file_item *f2 = (struct file_item *)e2;

    /* Directories always come first. */
    if (f1->type != f2->type) {
	if (f1->type == FT_DIR)
	    return -1;
	if (f2->type == FT_DIR)
	    return +1;
    }
    return strcmp(f1->name, f2->name);
}

static void file_list_sort(struct file_list *fl)
{
    qsort(fl->items, fl->num_used_items, sizeof(struct file_item),
	  file_list_sort_func);
}

static struct file_list *file_list_read(const char *path, const char *pattern)
{
    struct dirent *d;
    struct file_list *fl = file_list_create();
    char *wd = get_current_dir();
    DIR *ds;

    if (chdir(path) == -1) {
	free(wd);
	return NULL;
    }

    ds = opendir(".");
    if (ds == NULL) {
	chdir(wd);
	free(wd);
	return NULL;
    }

    /* Skip ".". */
    readdir(ds);

    while((d = readdir(ds)) != NULL) {
	struct stat s;
	int type;

	if (stat(d->d_name, &s) != -1) {
	    type = S_ISDIR(s.st_mode) ? FT_DIR : FT_NORMAL;
	    if (pattern == NULL || fnmatch(pattern, d->d_name, 0) == 0
		|| type == FT_DIR)
		file_list_add_item(fl, d->d_name, type);
	}
    }

    file_list_sort(fl);
    chdir(wd);
    free(wd);
    closedir(ds);

    return fl;
}

static int file_list_find(const struct file_list *fl, const char *str, int len)
{
    int i;

    for (i = 0; i < fl->num_used_items; i++)
	if (strncmp(fl->items[i].name, str, len) == 0)
	    return i;
    return -1;
}

/* ------------------------------------------------------------------------- */

static void file_selector_display_path(const char *path)
{
    tui_set_attr(FIRST_LINE_FORE, FIRST_LINE_BACK, 0);
    textcolor(FIRST_LINE_FORE);
    textbackground(FIRST_LINE_BACK);
    tui_display(0, tui_num_lines() - 1, 80, path);
}

static void file_selector_display_item(struct file_list *fl, int num,
				       int first_item_num, int x, int y,
				       int width, int height, int num_cols)
{
    y += (num - first_item_num) % height;
    x += ((num - first_item_num) / height) * width;

    if (num >= fl->num_used_items) {
	tui_hline(x, y, ' ', width);
    } else {
	if (fl->items[num].type == FT_DIR)
	    tui_display(x, y, width, " %s/ ", fl->items[num].name);
	else
	    tui_display(x, y, width, " %s ", fl->items[num].name);
    }
}

static void file_selector_update(struct file_list *fl,
				 int first_item_num, int x, int y,
				 int width, int height, int num_cols)
{
    int i;

    for (i = 0; i < num_cols * height; i++)
	file_selector_display_item(fl, first_item_num + i, first_item_num,
				   x, y, width, height, num_cols);
}

/* ------------------------------------------------------------------------- */

/* Display a file selector with title `title' and containing the files that
   match `pattern'.  `read_contents_func', if not NULL, is a function that
   returns the contents of the file (as a malloced ASCII string) passed as
   the parameter; then if the user can press `space' on a file name, contents
   are extracted with this function and displayed.  Return the name of the
   selected file, or NULL if the user pressed ESC to leave with no
   selection.  */
char *tui_file_selector(const char *title, const char *pattern,
			char *(*read_contents_func)(const char*))
{
    static char *return_path = NULL;
    static char *orig_path = NULL;
    struct file_list *fl;
    int curr_item, first_item, need_update;
    int x, y, width, height, num_cols, num_lines, field_width;
    int num_files;
    char str[0x100];
    int str_len = 0;
    tui_area_t backing_store = NULL;

    if (orig_path == NULL)
	free(orig_path);
    orig_path = get_current_dir();

    if (return_path == NULL)
	free(return_path);
    return_path = get_current_dir();

    fl = file_list_read(".", pattern);
    if (fl == NULL)
	return NULL;

    first_item = curr_item = 0;

    num_cols = 4;
    field_width = 18;
    num_lines = 17;
    height = num_lines + 2;
    width = field_width * num_cols + 4;
    num_files = num_cols * num_lines;

    x = CENTER_X(width);
    y = CENTER_Y(height);

    tui_display_window(x, y, width, height, MENU_FORE, MENU_BACK, title,
		       &backing_store);
    file_selector_display_path(return_path);
    need_update = 1;

    while (1) {
	int key;

	tui_set_attr(MENU_FORE, MENU_BACK, 0);
	if (need_update) {
	    file_selector_update(fl, first_item, x + 2, y + 1,
				 field_width, num_lines, num_cols);
	    need_update = 0;
	}
	tui_set_attr(MENU_FORE, MENU_HIGHLIGHT, 0);
	file_selector_display_item(fl, curr_item, first_item, x + 2, y + 1,
				   field_width, num_lines, num_cols);
	key = getkey();
	tui_set_attr(MENU_FORE, MENU_BACK, 0);
	file_selector_display_item(fl, curr_item, first_item, x + 2, y + 1,
				   field_width, num_lines, num_cols);

	switch (key) {
	  case K_Escape:
	    chdir(orig_path);
	    tui_area_put(backing_store, x, y);
	    tui_area_free(backing_store);
	    return NULL;
	  case K_Left:
	    str_len = 0;
	    if (curr_item - num_lines >= 0) {
		curr_item -= num_lines;
		if (curr_item < first_item) {
		    if (first_item >= num_lines) {
			first_item -= num_lines;
			need_update = 1;
		    } else
			curr_item += num_lines;
		}
	    }
	    break;
	  case K_Up:
	    str_len = 0;
	    if (curr_item > 0) {
		curr_item--;
		if (curr_item < first_item) {
		    first_item = curr_item;
		    need_update = 1;
		}
	    }
	    break;
	  case K_Right:
	    str_len = 0;
	    if (curr_item + num_lines < fl->num_used_items) {
		curr_item += num_lines;
		if (curr_item - first_item >= num_files) {
		    first_item += num_lines;
		    need_update = 1;
		}
	    }
	    break;
	  case K_Down:
	    str_len = 0;
	    if (curr_item < fl->num_used_items - 1) {
		curr_item++;
		if (curr_item == first_item + num_files) {
		    first_item++;
		    need_update = 1;
		}
	    }
	    break;
	  case K_PageDown:
	    str_len = 0;
	    if (curr_item + num_files < fl->num_used_items) {
		curr_item += num_files;
		first_item += num_files;
	    }
	    need_update = 1;
	    break;
	  case K_PageUp:
	    str_len = 0;
	    if (curr_item - num_files >= 0) {
		curr_item -= num_files;
		first_item -= num_files;
		if (first_item < 0)
		    first_item = 0;
		need_update = 1;
	    }
	    break;
	  case K_Home:
	    str_len = 0;
	    curr_item = 0;
	    if (first_item != 0) {
		first_item = 0;
		need_update = 1;
	    }
	    break;
	  case K_End:
	    str_len = 0;
	    curr_item = fl->num_used_items - 1;
	    first_item = curr_item - num_files + 1;
	    if (first_item < 0)
		first_item = 0;
	    need_update = 1;
	    break;
	  case K_Return:
	    str_len = 0;
	    if (fl->items[curr_item].type == FT_DIR) {
		if (chdir(fl->items[curr_item].name) != -1) {
		    file_list_free(fl);
		    fl = file_list_read(".", pattern);
		    first_item = curr_item = 0;
		    /* tui_display_window(x, y, width, height, MENU_FORE,
		       MENU_BACK, title, NULL); */
		    free(return_path);
		    return_path = get_current_dir();
		    file_selector_display_path(return_path);
		    need_update = 1;
		}
	    } else {
		char *p = concat(return_path, "/", fl->items[curr_item].name,
				 NULL);

		free(return_path);
		return_path = p;
		tui_area_put(backing_store, x, y);
		tui_area_free(backing_store);
		return return_path;
	    }
	    break;
	  case K_BackSpace:
	    if (str_len > 1) {
		int n;
		str_len--;
		n = file_list_find(fl, str, str_len);
		if (n >= 0) {
		    curr_item = n;
		    if (curr_item < first_item) {
			first_item = curr_item;
			need_update = 1;
		    } else if (first_item + num_files <= curr_item) {
			first_item = curr_item - num_files + 1;
			need_update = 1;
		    }
		}
	    } else {
		str_len = 0;
		curr_item = 0;
		if (first_item != 0) {
		    first_item = 0;
		    need_update = 1;
		}
	    }
	    break;
	  case ' ':
	    {
		char *contents = read_contents_func(fl->items[curr_item].name);

		if (contents != NULL)
		    tui_view_text(40, 20, fl->items[curr_item].name, contents);
		break;
	    }
	  default:
	    if (isprint(key) && str_len < 0x100) {
		int n;
		str[str_len] = key;
		n = file_list_find(fl, str, str_len + 1);
		if (n < 0) {
		    beep();
		} else {
		    str_len++;
		    curr_item = n;
		    if (curr_item < first_item) {
			first_item = curr_item;
			need_update = 1;
		    } else if (first_item + num_files <= curr_item) {
			first_item = curr_item - num_files + 1;
			need_update = 1;
		    }
		}
	    }
	    break;
	}
    }
}

