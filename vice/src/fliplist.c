/*
 * fliplist.c
 *
 * Written by
 *  Martin Pottendorfer
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
#include "log.h"
#include "utils.h"
#include "fliplist.h"
#include "attach.h"

struct fliplist_t { 
    struct fliplist_t *next, *prev;
    char *image;
    int unit;
};

static struct fliplist_t *fliplist = (struct fliplist_t *) NULL;
static char *current_image;
static int current_drive;

static void show_fliplist(void);

/* ------------------------------------------------------------------------- */
/* interface functions */

void
flip_set_current(int unit, char *filename)
{
    current_image = filename;
    current_drive = unit;
}

void
flip_add_image (void)
{
    struct fliplist_t *n;
 
    if (current_image == NULL)
	return;
    
    n = (struct fliplist_t *) xmalloc(sizeof (struct fliplist_t));
    n->image = stralloc(current_image);
    n->unit = current_drive;

    log_message(LOG_DEFAULT, "Adding `%s' to fliplist", n->image);
    if (fliplist)
    {
	n->next = fliplist;
	n->prev = fliplist->prev;
	n->next->prev = n;
	n->prev->next = n;
	fliplist = n;
    }
    else
    {
	fliplist = n;
	n->next = n;
	n->prev = n;
    }
    show_fliplist();
}

void
flip_remove(int unit, char *image)
{
    struct fliplist_t *tmp;

    if (fliplist == (struct fliplist_t *) NULL)
	return;
    if (image == (char *) NULL)
    {
	/* no image given, so remove the head */
	if ((fliplist == fliplist->next) &&
	    (fliplist == fliplist->prev))
	{
	    /* this is the last entry */
	    tmp = fliplist;
	    fliplist = (struct fliplist_t *) NULL;
	    goto out;
	}
    
	fliplist->next->prev = fliplist->prev;
	fliplist->prev->next = fliplist->next;
	tmp = fliplist;
	fliplist = fliplist->next;
    out:
	log_message(LOG_DEFAULT, "Removing `%s' from fliplist", tmp->image);
	free (tmp->image);
	free (tmp);
	show_fliplist();
    }
    else
    {
	/* do a lookup and remove it */
	/* not used so far, so I'll implement it later */
    }
}

void
flip_attach_head (int direction)
{
    if (fliplist == (struct fliplist_t *)NULL)
	return;

    if (direction)
	fliplist = fliplist->next;
    else
	fliplist = fliplist->prev;

    if (file_system_attach_disk(fliplist->unit, 
				fliplist->image) < 0)
    {
	/* shouldn't happen, so ignore it */
	;
    }
}

/* ------------------------------------------------------------------------- */

static void
show_fliplist(void)
{
    struct fliplist_t *it = fliplist;
    
    log_message(LOG_DEFAULT, "Fliplist contains:");
    if (it)
    {
	do {
	    log_message(LOG_DEFAULT, "\tUnit %d %s (n: %s, p:%s", it->unit, it->image, it->next->image, it->prev->image);
	    it = it->next;
	} while (it != fliplist);
    }
    else
	log_message(LOG_DEFAULT, "\tnothing");
}

