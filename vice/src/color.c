/*
 * color.c - Color management for displays using a palette.
 *
 * Written by
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

#include <stdlib.h>

#include "log.h"
#include "palette.h"
#include "utils.h"

struct color_rgb_s {
    unsigned int red;
    unsigned int green;
    unsigned int blue;
};
typedef struct color_rgb_s color_rgb_t;

struct owner_list_s {
    void *color_owner;
    struct owner_list_s *next;
};
typedef struct owner_list_s owner_list_t;

struct color_list_s {
    color_rgb_t color_rgb_req;
    color_rgb_t color_rgb_alloced;
    PIXEL pixel_data;
    owner_list_t *owner;
    struct color_list_s *next;
};
typedef struct color_list_s color_list_t;

static color_list_t *color_alloced;

static log_t color_log = LOG_ERR;

/*-----------------------------------------------------------------------*/

static void color_owner_create_empty(owner_list_t **owner)
{
    *owner = (owner_list_t *)xmalloc(sizeof(owner_list_t));

    (*owner)->color_owner = NULL;
    (*owner)->next = NULL;
}

static void color_owner_free(owner_list_t *owner)
{
    owner_list_t *owner_next;

    do {
        owner_next = owner->next;
        free(owner);
        owner = owner_next;
    } while (owner != NULL);
}

static void color_owner_add(owner_list_t *owner, void *c)
{
    while (owner->next != NULL)
        owner = owner->next;

    owner->color_owner = c;
    color_owner_create_empty(&owner->next);
}

static void color_owner_copy(owner_list_t *dest, owner_list_t *src)
{
    while (dest->next != NULL)
        dest = dest->next;

    while (src->next != NULL) {
        dest->color_owner = src->color_owner;
        color_owner_create_empty(&dest->next);
        dest = dest->next;
        src = src->next;
    }
}

static void color_owner_remove(owner_list_t **owner, void *c)
{
    owner_list_t *owner_list;

    if ((*owner)->next == NULL)
        return;

    if ((*owner)->color_owner == c) {
        owner_list_t *next_owner;
        next_owner = (*owner)->next;
        free(*owner);
        *owner = next_owner;
        return;
    }

    owner_list = (*owner);

    while (owner_list->next != NULL && owner_list->next->next != NULL) {
        if (owner_list->next->color_owner == c) {
            owner_list_t *next_owner;
            next_owner = owner_list->next->next;
            free(owner_list->next);
            owner_list->next = next_owner;

            log_message(color_log, "Remove nth owner.");
            break;
        }
        owner_list = owner_list->next;
    }

}

/*-----------------------------------------------------------------------*/

static void color_create_empty_entry(color_list_t **color_entry)
{
    *color_entry = (color_list_t *)xmalloc(sizeof(color_list_t));

    color_owner_create_empty(&((*color_entry)->owner));
    (*color_entry)->next = NULL;
}

static void color_free(color_list_t *list)
{
    color_list_t *list_next;

    do {
        list_next = list->next;
        color_owner_free(list->owner);
        free(list);
        list = list_next;
    } while (list != NULL);
}

static void color_palette_to_list(color_list_t *color_list, void *c,
                                  const palette_t *palette)
{
    unsigned int i;
    color_list_t *current = color_list;

    while (current->next != NULL)
        current = current->next;

    for (i = 0; i < palette->num_entries; i++) {
        current->color_rgb_req.red = palette->entries[i].red;
        current->color_rgb_req.green = palette->entries[i].green;
        current->color_rgb_req.blue = palette->entries[i].blue;
        color_owner_add(current->owner, c);
        color_create_empty_entry(&current->next);
        current = current->next;
    }
}

static void color_copy_entry(color_list_t *dest, color_list_t *src)
{
    while (dest->next != NULL)
        dest = dest->next;

    dest->color_rgb_req.red = src->color_rgb_req.red;
    dest->color_rgb_req.green = src->color_rgb_req.green;
    dest->color_rgb_req.blue = src->color_rgb_req.blue;
    color_owner_copy(dest->owner, src->owner);
    color_create_empty_entry(&dest->next);
}

static void color_copy_list(color_list_t *dest, color_list_t *src)
{
    while (src->next != NULL) {
        color_copy_entry(dest, src);
        src = src->next;
    }
}

static void color_compare_list(color_list_t *base, color_list_t *comp,
                               color_list_t *differ, color_list_t *equal)
{
    color_list_t *cbase;

    while (comp->next != NULL) {
        cbase = base;
        while (cbase->next != NULL) {
            if (comp->color_rgb_req.red == cbase->color_rgb_req.red
                && comp->color_rgb_req.green == cbase->color_rgb_req.green
                && comp->color_rgb_req.blue == cbase->color_rgb_req.blue) {
                color_copy_entry(equal, comp);
                break;
            }
            cbase = cbase->next;
        }
        if (cbase->next == NULL) {
            color_copy_entry(differ, comp);
        }
        comp = comp->next;
    }
}

static void color_remove_owner_from_list(color_list_t *list, void *c)
{
    while (list->next != NULL) {
        color_owner_remove(&list->owner, c);
        list = list->next;
    }
}

static void color_add_owner_from_other_list(color_list_t *dest,
                                            color_list_t *src)
{
    color_list_t *cdest;

    while (src->next != NULL) {
        cdest = dest;
        while (cdest->next != NULL) {
            if (src->color_rgb_req.red == cdest->color_rgb_req.red
                && src->color_rgb_req.green == cdest->color_rgb_req.green
                && src->color_rgb_req.blue == cdest->color_rgb_req.blue) {
                color_owner_add(dest->owner, src->owner->color_owner);
                break;
            }
            cdest = cdest->next;
        }
        src = src->next;
    }
}

static void color_copy_list_with_owner(color_list_t *dest, color_list_t *src)
{
    while (src->next != NULL) {
        if (src->owner->next != NULL)
            color_copy_entry(dest, src);
        src = src->next;
    }
}

/*-----------------------------------------------------------------------*/

static void color_print_list(color_list_t *list)
{
    log_message(color_log, "List start:");
    while (list->next != NULL) {
        owner_list_t *owner_list = list->owner;
        log_message(color_log, "R %03i G %03i B %03i PIXEL %02x.",
                    list->color_rgb_req.red,
                    list->color_rgb_req.green,
                    list->color_rgb_req.blue,
                    list->pixel_data);
        while (owner_list->next != NULL) {
            log_message(color_log, "Owner: %p.", owner_list->color_owner);
            owner_list = owner_list->next;
        }
        list = list->next;
    }
    log_message(color_log, "List ends.");
}

/*-----------------------------------------------------------------------*/

void color_init(void)
{
    color_log = log_open("Color");

    color_create_empty_entry(&color_alloced);
}

int color_alloc_colors(void *c, const palette_t *palette,
                       PIXEL pixel_return[])
{
    color_list_t *color_new, *color_to_alloc, *color_no_alloc,
                 *color_alloced_owner;

    /* Convert the palette to a color list.  */
    color_create_empty_entry(&color_new);
    color_palette_to_list(color_new, c, palette);

    /* This splits `color_new' into two separate lists.  */
    color_create_empty_entry(&color_to_alloc);
    color_create_empty_entry(&color_no_alloc);
    color_compare_list(color_alloced, color_new, color_to_alloc,
                       color_no_alloc);

    /* Allocate only colors we do not have.  */
    /*color_alloc_new_colors(color_to_alloc)*/

    /* Remove the current owner from allocated colors list.  */
    color_remove_owner_from_list(color_alloced, c);

    /* Add the owner to remaining colors if necessary.  */
    color_add_owner_from_other_list(color_alloced, color_no_alloc);

    /* Add the newly allocated colors to the allocated colors list.  */
    color_copy_list(color_alloced, color_to_alloc);

    /* Copy valid colors (with owner) to new list.  */
    color_create_empty_entry(&color_alloced_owner);
    color_copy_list_with_owner(color_alloced_owner, color_alloced);

    /* Throw away old list and temp lists.  */
    color_free(color_alloced);
    color_free(color_new);
    color_free(color_to_alloc);
    color_free(color_no_alloc);

    /* The new list.  */
    color_alloced = color_alloced_owner;

    return 0;
}

