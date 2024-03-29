/*
 * vdrive-bam.h - Virtual disk-drive implementation.
 *                BAM specific functions.
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

#ifndef VICE_VDRIVE_BAM_H
#define VICE_VDRIVE_BAM_H

#include "types.h"

#define BAM_FIRST_TRACK         0
#define BAM_FIRST_SECTOR        1
#define BAM_FORMAT_TYPE         2
#define BAM_BIT_MAP             4       /* start of bitmap in each BAM blk */
#define BAM_BIT_MAP_1581        16
#define BAM_BIT_MAP_8050        6
#define BAM_BIT_MAP_8250        BAM_BIT_MAP_8050
#define BAM_BIT_MAP_NP          32
#define BAM_BIT_MAP_9000        16
#define BAM_VERSION_1541        165     /* position of DOS version (2A/2C)
                                           in BAM blk */
#define BAM_VERSION_1581        25
#define BAM_VERSION_8050        27
#define BAM_VERSION_8250        BAM_VERSION_8050
#define BAM_VERSION_NP          25
#define BAM_VERSION_9000        27

struct vdrive_s;

int vdrive_bam_allocate_chain(struct vdrive_s *vdrive, unsigned int t, unsigned int s);
int vdrive_bam_allocate_chain_255(struct vdrive_s *vdrive, unsigned int t, unsigned int s);
int vdrive_bam_alloc_first_free_sector(struct vdrive_s *vdrive, unsigned int *track,
                                       unsigned int *sector);
int vdrive_bam_alloc_next_free_sector(struct vdrive_s *vdrive, unsigned int *track,
                                      unsigned int *sector);
int vdrive_bam_alloc_next_free_sector_interleave(struct vdrive_s *vdrive, unsigned int *track,
                                                 unsigned int *sector, unsigned int interleave);
int vdrive_bam_allocate_sector(struct vdrive_s *vdrive, unsigned int track, unsigned int sector);
int vdrive_bam_is_sector_allocated(struct vdrive_s *vdrive, unsigned int track, unsigned int sector);

void vdrive_bam_clear_all(struct vdrive_s *vdrive);
void vdrive_bam_create_empty_bam(struct vdrive_s *vdrive, const char *name, uint8_t *id);
int unsigned vdrive_bam_free_block_count(struct vdrive_s *vdrive);
int vdrive_bam_free_sector(struct vdrive_s *vdrive, unsigned int track, unsigned int sector);
int vdrive_bam_int_get_disk_id(struct vdrive_s *vdrive, uint8_t *id);
int vdrive_bam_get_disk_id(unsigned int unit, unsigned int drive, uint8_t *id);
int vdrive_bam_set_disk_id(unsigned int unit, unsigned int drive, uint8_t *id);
int vdrive_bam_read_bam(struct vdrive_s *vdrive);

#if 0
int vdrive_bam_reread_bam(unsigned int unit, unsigned int drive);
#endif

int vdrive_bam_write_bam(struct vdrive_s *vdrive);
int vdrive_bam_isgeos(struct vdrive_s *vdrive);
void vdrive_bam_setup_bam(struct vdrive_s *vdrive);

#endif
