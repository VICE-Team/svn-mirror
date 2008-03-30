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

#define NUM_DRIVES 2

struct fliplist_t { 
    struct fliplist_t *next, *prev;
    char *image;
    int unit;
};

static struct fliplist_t *fliplist[NUM_DRIVES] = 
	{ (struct fliplist_t *) NULL, 
	  (struct fliplist_t *) NULL };

static char *current_image = (char *) NULL;
static int current_drive;
static struct fliplist_t *iterator;

static void show_fliplist(int unit);

/* ------------------------------------------------------------------------- */
/* interface functions */

void flip_set_current(int unit, const char *filename)
{
    if (current_image != NULL)
        free(current_image);
    current_image = stralloc(filename);
    current_drive = unit;
}

char *flip_get_head(int unit) 
{
    if (fliplist[unit - 8])
	return fliplist[unit - 8]->image;
    return (char *) NULL;
}

char *flip_get_next(int unit) 
{
    if (fliplist[unit - 8])
	return fliplist[unit - 8]->next->image;
    return (char *) NULL;
}

char *flip_get_prev(int unit) 
{
    if (fliplist[unit - 8])
	return fliplist[unit - 8]->prev->image;
    return (char *) NULL;
}

char *flip_get_image(void *fl)
{
    return ((struct fliplist_t *) fl)->image;
}

int flip_get_unit(void *fl)
{
    return ((struct fliplist_t *) fl)->unit;
}

void flip_add_image (int unit)
{
    struct fliplist_t *n;
 
    if (current_image == NULL)
	return;
    if (strcmp(current_image, "") == 0)
	return;

    n = (struct fliplist_t *) xmalloc(sizeof (struct fliplist_t));
    n->image = stralloc(current_image);
    unit = n->unit = current_drive;

    log_message(LOG_DEFAULT, "Adding `%s' to fliplist[%d]", n->image, unit);
    if (fliplist[unit - 8])
    {
	n->next = fliplist[unit - 8];
	n->prev = fliplist[unit - 8]->prev;
	n->next->prev = n;
	n->prev->next = n;
	fliplist[unit - 8] = n;
    }
    else
    {
	fliplist[unit - 8] = n;
	n->next = n;
	n->prev = n;
    }
    show_fliplist(unit);
}

void flip_remove(int unit, char *image)
{
    struct fliplist_t *tmp;

    if (fliplist[unit - 8] == (struct fliplist_t *) NULL)
	return;
    if (image == (char *) NULL)
    {
	/* no image given, so remove the head */
	if ((fliplist[unit - 8] == fliplist[unit - 8]->next) &&
	    (fliplist[unit - 8] == fliplist[unit - 8]->prev))
	{
	    /* this is the last entry */
	    tmp = fliplist[unit - 8];
	    fliplist[unit - 8] = (struct fliplist_t *) NULL;
	    goto out;
	}
    
	fliplist[unit - 8]->next->prev = fliplist[unit - 8]->prev;
	fliplist[unit - 8]->prev->next = fliplist[unit - 8]->next;
	tmp = fliplist[unit - 8];
	fliplist[unit - 8] = fliplist[unit - 8]->next;
    out:
	log_message(LOG_DEFAULT, "Removing `%s' from fliplist[%d]", 
		    tmp->image, unit);
	free (tmp->image);
	free (tmp);
	show_fliplist(unit);
	return;
    }
    else
    {
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
	    log_message(LOG_DEFAULT, "Cannot remove `%s'; not found in fliplist[%d]", it->image, unit);
	    return;
	}
	
	it->next->prev = it->prev;
	it->prev->next = it->next;
	free(it->image);
	free(it);
	show_fliplist(unit);
    }
}

void flip_attach_head (int unit, int direction)
{
    if (fliplist[unit - 8] == (struct fliplist_t *)NULL)
	return;

    if (direction)
	fliplist[unit - 8] = fliplist[unit - 8]->next;
    else
	fliplist[unit - 8] = fliplist[unit - 8]->prev;

    if (file_system_attach_disk(fliplist[unit - 8]->unit, 
				fliplist[unit - 8]->image) < 0)
    {
	/* shouldn't happen, so ignore it */
	;
    }
}

void *flip_init_iterate(int unit) 
{
    void *ret = NULL;
    
    iterator = fliplist[unit - 8];
    if (iterator) {
	ret = (void *) iterator;
	iterator = iterator->next;
    }
    return ret;
}

void *flip_next_iterate(int unit)
{
    void *ret = NULL;
    
    if (iterator) {
	if (iterator != fliplist[unit - 8]) {
	    ret = (void *) iterator;
	    iterator=iterator->next;
	}
    }
    return ret;
}

/* ------------------------------------------------------------------------- */

static void show_fliplist(int unit)
{
    struct fliplist_t *it = fliplist[unit - 8];
    
    log_message(LOG_DEFAULT, "Fliplist[%d] contains:", unit);
    if (it)
    {
	do {
	    log_message(LOG_DEFAULT, "\tUnit %d %s (n: %s, p:%s)", it->unit, it->image, it->next->image, it->prev->image);
	    it = it->next;
	} while (it != fliplist[unit - 8]);
    }
    else
	log_message(LOG_DEFAULT, "\tnothing");
}



