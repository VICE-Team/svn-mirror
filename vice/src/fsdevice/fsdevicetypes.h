/*
 * fsdevicetypes.h - File system device.
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

#ifndef _FSDEVICETYPES_H
#define _FSDEVICETYPES_H

#include "vice.h"
#include "tape.h"

#include <stdio.h>

#ifdef __riscos
#include "archdep.h"
#else
#ifdef __IBMC__
#include <direct.h>
#include "snippets/dirport.h"
#else
#include <dirent.h>
#endif
#endif

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#include "types.h"

enum fsmode {
    Write, Read, Append, Directory
};

struct fs_buffer_info_s {
    FILE *fd;
    DIR  *dp;
    tape_image_t tape;
    enum fsmode mode;
    char dir[MAXPATHLEN];
    BYTE name[MAXPATHLEN + 5];
    int buflen;
    BYTE *bufp;
    int eof;
    int reclen;
    int type;
};
typedef struct fs_buffer_info_s fs_buffer_info_t;

extern fs_buffer_info_t fs_info[];

struct vdrive_s;

extern void fsdevice_error(struct vdrive_s *vdrive, int code);
extern FILE *fsdevice_find_pc64_name(struct vdrive_s *vdrive, char *name,
                                     int length, char *pname);
extern char *fsdevice_get_path(unsigned int unit);
extern int fsdevice_evaluate_name_p00(char *name, int length, char *filename);
extern void fsdevice_compare_file_name(struct vdrive_s *vdrive, char *fsname2,
                                       char *fsname, int secondary);
extern int fsdevice_create_file_p00(struct vdrive_s *vdrive, char *name,
                                    int length, char *fsname, int secondary);
extern void fsdevice_test_pc64_name(struct vdrive_s *vdrive, char *rname,
                                    int secondary);
extern int fsdevice_error_get_byte(struct vdrive_s *vdrive, BYTE *data);
extern int fsdevice_flush_write_byte(struct vdrive_s *vdrive, BYTE data);

extern char fs_dirmask[];

#endif

