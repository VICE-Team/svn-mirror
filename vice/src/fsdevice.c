/*
 * fsdevice.c - File system device.
 *
 * Written by
 *  Teemu Rantanen      (tvr@cs.hut.fi)
 *  Jarkko Sonninen     (sonninen@lut.fi)
 *  Jouko Valta         (jopi@stekt.oulu.fi)
 *  Olaf Seibert        (rhialto@mbfys.kun.nl)
 *  André Fachat        (a.fachat@physik.tu-chemnitz.de)
 *  Ettore Perazzoli    (ettore@comm2000.it)
 *  Martin Pottendorfer (Martin.Pottendorfer@aut.alcatel.at)
 *  Andreas Boose       (boose@unixserv.rz.fh-hannover.de)
 *
 * Patches by
 *  Dan Miner           (dminer@nyx10.cs.du.edu)
 *  Germano Caronni     (caronni@tik.ethz.ch)
 *  Daniel Fandrich     (dan@fch.wimsey.bc.ca)  /DF/
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

#ifdef STDC_HEADERS
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#ifdef __riscos
#include "ROlib.h"
#include "ui.h"
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <memory.h>
#endif
#include <errno.h>
#endif

#include "fsdevice.h"

#include "archdep.h"
#include "charsets.h"
#include "cmdline.h"
#include "log.h"
#include "p00.h"
#include "resources.h"
#include "utils.h"
#include "vdrive.h"

enum fsmode {
    Write, Read, Append, Directory
};

struct fs_buffer_info {
    FILE *fd;
    DIR *dp;
    enum fsmode mode;
    char dir[MAXPATHLEN];
    BYTE name[MAXPATHLEN + 5];
    int buflen;
    BYTE *bufp;
    int eof;
    int dirmpos;
    int reclen;
    int type;
} fs_info[16];

/* this should somehow go into the fs_info struct... */

static char fs_errorl[MAXPATHLEN];
static unsigned int fs_elen, fs_eptr;
static char fs_cmdbuf[MAXPATHLEN];
static unsigned int fs_cptr = 0;

static char fs_dirmask[MAXPATHLEN];

static FILE *fs_find_pc64_name(void *flp, char *name, int length, char *pname);
static void fs_test_pc64_name(void *flp, char *rname, int secondary);
static int fsdevice_compare_wildcards(char *name, char *p00name);
static void fsdevice_compare_file_name(void *flp, char *fsname2, char *fsname,
                                       int secondary);
static int fsdevice_create_file_p00(void *flp, char *name, int length,
                                    char *fsname, int secondary);
static int fsdevice_reduce_filename_p00(char *filename, int len);
static int fsdevice_eliminate_char_p00(char *filename, int pos);
static int fsdevice_evaluate_name_p00(char *name, int length, char *filename);

/* FIXME: ugly.  */
extern errortext_t floppy_error_messages;

/* ------------------------------------------------------------------------- */

static int fsdevice_convert_p00_enabled[4];
static int fsdevice_save_p00_enabled[4];
static int fsdevice_hide_cbm_files_enabled[4];
static char *fsdevice_8_dir;
static char *fsdevice_9_dir;
static char *fsdevice_10_dir;
static char *fsdevice_11_dir;

static int set_fsdevice_8_convert_p00(resource_value_t v)
{
    fsdevice_convert_p00_enabled[0] = (int) v;
    return 0;
}

static int set_fsdevice_9_convert_p00(resource_value_t v)
{
    fsdevice_convert_p00_enabled[1] = (int) v;
    return 0;
}

static int set_fsdevice_10_convert_p00(resource_value_t v)
{
    fsdevice_convert_p00_enabled[2] = (int) v;
    return 0;
}

static int set_fsdevice_11_convert_p00(resource_value_t v)
{
    fsdevice_convert_p00_enabled[3] = (int) v;
    return 0;
}

static int set_fsdevice_8_dir(resource_value_t v)
{
    const char *name = (const char *) v;

    if (fsdevice_8_dir != NULL && name != NULL
        && strcmp(name, fsdevice_8_dir) == 0)
        return 0;

    string_set(&fsdevice_8_dir, name);
    return 0;
}

static int set_fsdevice_9_dir(resource_value_t v)
{
    const char *name = (const char *) v;

    if (fsdevice_9_dir != NULL && name != NULL
        && strcmp(name, fsdevice_9_dir) == 0)
        return 0;

    string_set(&fsdevice_9_dir, name);
    return 0;
}

static int set_fsdevice_10_dir(resource_value_t v)
{
    const char *name = (const char *) v;

    if (fsdevice_10_dir != NULL && name != NULL
        && strcmp(name, fsdevice_10_dir) == 0)
        return 0;

    string_set(&fsdevice_10_dir, name);
    return 0;
}

static int set_fsdevice_11_dir(resource_value_t v)
{
    const char *name = (const char *) v;

    if (fsdevice_11_dir != NULL && name != NULL
        && strcmp(name, fsdevice_11_dir) == 0)
        return 0;

    string_set(&fsdevice_11_dir, name);
    return 0;
}

static int set_fsdevice_8_save_p00(resource_value_t v)
{
    fsdevice_save_p00_enabled[0] = (int) v;
    return 0;
}

static int set_fsdevice_9_save_p00(resource_value_t v)
{
    fsdevice_save_p00_enabled[1] = (int) v;
    return 0;
}

static int set_fsdevice_10_save_p00(resource_value_t v)
{
    fsdevice_save_p00_enabled[2] = (int) v;
    return 0;
}

static int set_fsdevice_11_save_p00(resource_value_t v)
{
    fsdevice_save_p00_enabled[3] = (int) v;
    return 0;
}

static int set_fsdevice_8_hide_cbm_files(resource_value_t v)
{
    if (!fsdevice_convert_p00_enabled[0])
	return -1;
    fsdevice_hide_cbm_files_enabled[0] = (int) v;
    return 0;
}

static int set_fsdevice_9_hide_cbm_files(resource_value_t v)
{
    if (!fsdevice_convert_p00_enabled[1])
	return -1;
    fsdevice_hide_cbm_files_enabled[1] = (int) v;
    return 0;
}

static int set_fsdevice_10_hide_cbm_files(resource_value_t v)
{
    if (!fsdevice_convert_p00_enabled[2])
	return -1;
    fsdevice_hide_cbm_files_enabled[2] = (int) v;
    return 0;
}

static int set_fsdevice_11_hide_cbm_files(resource_value_t v)
{
    if (!fsdevice_convert_p00_enabled[3])
	return -1;
    fsdevice_hide_cbm_files_enabled[3] = (int) v;
    return 0;
}

static resource_t resources[] = {
    { "FSDevice8ConvertP00", RES_INTEGER, (resource_value_t) 1,
      (resource_value_t *) &fsdevice_convert_p00_enabled[0],
      set_fsdevice_8_convert_p00 },
    { "FSDevice9ConvertP00", RES_INTEGER, (resource_value_t) 1,
      (resource_value_t *) &fsdevice_convert_p00_enabled[1],
      set_fsdevice_9_convert_p00 },
    { "FSDevice10ConvertP00", RES_INTEGER, (resource_value_t) 1,
      (resource_value_t *) &fsdevice_convert_p00_enabled[2],
      set_fsdevice_10_convert_p00 },
    { "FSDevice11ConvertP00", RES_INTEGER, (resource_value_t) 1,
      (resource_value_t *) &fsdevice_convert_p00_enabled[3],
      set_fsdevice_11_convert_p00 },
    { "FSDevice8Dir", RES_STRING, (resource_value_t) FSDEVICE_DEFAULT_DIR,
      (resource_value_t *) &fsdevice_8_dir, set_fsdevice_8_dir },
    { "FSDevice9Dir", RES_STRING, (resource_value_t) FSDEVICE_DEFAULT_DIR,
      (resource_value_t *) &fsdevice_9_dir, set_fsdevice_9_dir },
    { "FSDevice10Dir", RES_STRING, (resource_value_t) FSDEVICE_DEFAULT_DIR,
      (resource_value_t *) &fsdevice_10_dir, set_fsdevice_10_dir },
    { "FSDevice11Dir", RES_STRING, (resource_value_t) FSDEVICE_DEFAULT_DIR,
      (resource_value_t *) &fsdevice_11_dir, set_fsdevice_11_dir },
    { "FSDevice8SaveP00", RES_INTEGER, (resource_value_t) 1,
      (resource_value_t *) &fsdevice_save_p00_enabled[0],
      set_fsdevice_8_save_p00 },
    { "FSDevice9SaveP00", RES_INTEGER, (resource_value_t) 1,
      (resource_value_t *) &fsdevice_save_p00_enabled[1],
      set_fsdevice_9_save_p00 },
    { "FSDevice10SaveP00", RES_INTEGER, (resource_value_t) 1,
      (resource_value_t *) &fsdevice_save_p00_enabled[2],
      set_fsdevice_10_save_p00 },
    { "FSDevice11SaveP00", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &fsdevice_save_p00_enabled[3],
      set_fsdevice_11_save_p00 },
    { "FSDevice8HideCBMFiles", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &fsdevice_hide_cbm_files_enabled[0],
      set_fsdevice_8_hide_cbm_files },
    { "FSDevice9HideCBMFiles", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &fsdevice_hide_cbm_files_enabled[1],
      set_fsdevice_9_hide_cbm_files },
    { "FSDevice10HideCBMFiles", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &fsdevice_hide_cbm_files_enabled[2],
      set_fsdevice_10_hide_cbm_files },
    { "FSDevice11HideCBMFiles", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &fsdevice_hide_cbm_files_enabled[3],
      set_fsdevice_11_hide_cbm_files },
    { NULL }
};

int fsdevice_init_resources(void)
{
    return resources_register(resources);
}

/* ------------------------------------------------------------------------- */

static int cmdline_fsdirectory(const char *param, void *extra_param)
{
    int unit = (int) extra_param;
    char directory[MAXPATHLEN];

    strcpy(directory, param);
    strcat(directory, FSDEV_DIR_SEP_STR);

    switch (unit) {
      case 8:
	set_fsdevice_8_dir((resource_value_t) directory);
	break;
      case 9:
	set_fsdevice_9_dir((resource_value_t) directory);
	break;
      case 10:
	set_fsdevice_10_dir((resource_value_t) directory);
	break;
      case 11:
	set_fsdevice_11_dir((resource_value_t) directory);
	break;
      default:
	fprintf(stdout, "Invalid unit number %d.\n", unit);
    }

    return 0;
}

static cmdline_option_t cmdline_options[] = {
    { "-fs8", CALL_FUNCTION, 1, cmdline_fsdirectory, (void *) 8, NULL, NULL,
      "<name>", "Use <name> as directory for file system device #8" },
    { "-fs9", CALL_FUNCTION, 1, cmdline_fsdirectory, (void *) 9, NULL, NULL,
      "<name>", "Use <name> as directory for file system device #9" },
    { "-fs10", CALL_FUNCTION, 1, cmdline_fsdirectory, (void *) 10, NULL, NULL,
      "<name>", "Use <name> as directory for file system device #10" },
    { "-fs11", CALL_FUNCTION, 1, cmdline_fsdirectory, (void *) 11, NULL, NULL,
      "<name>", "Use <name> as directory for file system device #11" },
    { NULL }
};

int fsdevice_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

int attach_fsdevice(int device, char *var, const char *name)
{
    if (serial_attach_device(device, (char *) var, (char *) name,
			     read_fs, write_fs, open_fs, close_fs, flush_fs))
	return 1;
    fs_error(IPE_DOS_VERSION);
    return 0;
}

void fsdevice_set_directory(char *filename, int unit)
{
#if 0
    char *p;

    /* FIXME: Remove this once the select directory dialog is available.  */
    p = strrchr(filename, FSDEV_DIR_SEP_CHR);
    *(++p) = '\0';
#endif

    switch (unit) {
      case 8:
	set_fsdevice_8_dir((resource_value_t) filename);
	break;
      case 9:
	set_fsdevice_9_dir((resource_value_t) filename);
	break;
      case 10:
	set_fsdevice_10_dir((resource_value_t) filename);
	break;
      case 11:
	set_fsdevice_11_dir((resource_value_t) filename);
	break;
    }
    return;
}

static char *fsdevice_get_path(int unit)
{
    switch (unit) {
      case 8:
	return fsdevice_8_dir;
	break;
      case 9:
	return fsdevice_9_dir;
	break;
      case 10:
	return fsdevice_10_dir;
	break;
      case 11:
	return fsdevice_11_dir;
	break;
    }
    return NULL;
}

void fs_error(int code)
{
    static int last_code;
    const char *message;

    /* Only set an error once per command */
    if (code != IPE_OK && last_code != IPE_OK && last_code != IPE_DOS_VERSION)
	return;

    last_code = code;

    if (code == IPE_DOS_VERSION) {
	message = "VICE FS DRIVER V2.0";
    } else {
	errortext_t *e;
	e = &floppy_error_messages;
	while (e->nr >= 0 && e->nr != code)
	    e++;
	if (e->nr >= 0)
	    message = e->text;
	else
	    message = "UNKNOWN ERROR NUMBER";
    }

    sprintf(fs_errorl, "%02d,%s,00,00\015", code, message);

    fs_elen = strlen(fs_errorl);
    fs_eptr = 0;

    if (code && code != IPE_DOS_VERSION)
	log_message(LOG_DEFAULT, "Fsdevice: ERR = %02d, %s", code, message);
}

void flush_fs(void *flp, int secondary)
{
    DRIVE *floppy = (DRIVE *)flp;
    char *cmd, *realarg, *arg, *realarg2 = NULL, *arg2 = NULL;
    char cbmcmd[MAXPATHLEN], name1[MAXPATHLEN], name2[MAXPATHLEN];
    int er = IPE_SYNTAX;
    FILE *fd;

    if (secondary != 15 || !fs_cptr)
	return;

    /* remove trailing cr */
    while (fs_cptr && (fs_cmdbuf[fs_cptr - 1] == 13))
	fs_cptr--;
    fs_cmdbuf[fs_cptr] = 0;

    strcpy(cbmcmd, fs_cmdbuf);
    petconvstring(cbmcmd, 1);	/* CBM name to FSname */
    cmd = cbmcmd;
    while (*cmd == ' ')
	cmd++;

    arg = strchr(cbmcmd, ':');
    if (arg) {
	*arg++ = '\0';
    }
    realarg = strchr(fs_cmdbuf, ':');
    if (realarg) {
	*realarg++ = '\0';
    }

    if (!strcmp(cmd, "cd")) {
	er = IPE_OK;
	if (chdir(arg)) {
	    er = IPE_NOT_FOUND;
	    if (errno == EPERM)
		er = IPE_PERMISSION;
	}
    } else if (!strcmp(cmd, "md")) {
	er = IPE_OK;
	if (mkdir(arg, /*S_IFDIR | */ 0770)) {
	    er = IPE_INVAL;
	    if (errno == EEXIST)
		er = IPE_FILE_EXISTS;
	    if (errno == EACCES)
		er = IPE_PERMISSION;
	    if (errno == ENOENT)
		er = IPE_NOT_FOUND;
	}
    } else if (!strcmp(cmd, "rd")) {
	er = IPE_OK;
	if (rmdir(arg)) {
	    er = IPE_NOT_EMPTY;
	    if (errno == EPERM)
		er = IPE_PERMISSION;
	}
    } else if (*cmd == 's') {
	er = IPE_DELETED;
	fd = fs_find_pc64_name(flp, realarg, strlen(realarg), name1);
	    if (fd != NULL) {
		fclose(fd);
	    } else {
		if (fsdevice_hide_cbm_files_enabled[floppy->unit - 8]) {
		    fs_error(IPE_NOT_FOUND);
		    fs_cptr = 0;
		    return;
		}
		strcpy(name1, fsdevice_get_path(floppy->unit));
                strcat(name1, FSDEV_DIR_SEP_STR);
		strcat(name1, arg);
	    }
	if (unlink(name1)) {
	    er = IPE_NOT_FOUND;
	    if (errno == EPERM)
		er = IPE_PERMISSION;
	}
    } else if (*cmd == 'r') {
	if ((arg2 = strchr(arg, '='))) {
	    char name2long[MAXPATHLEN];
	    er = IPE_OK;
	    *arg2++ = 0;
	    realarg2 = strchr(realarg, '=');
	    *realarg2++ = 0;
	    fd = fs_find_pc64_name(flp, realarg2, strlen(realarg2), name2long);
	    if (fd != NULL) {
		/* Rename P00 file.  */
		int name1len;
		char *p, p00name[17], p00type, p00count[2];
		char name1p00[MAXPATHLEN], name2p00[MAXPATHLEN];
		fclose(fd);
		strcpy(name2p00, name2long);
		p = strrchr(name2long, FSDEV_EXT_SEP_CHR);
		p00type = p[1];
		*p = '\0';
		p = strrchr(name2long, FSDEV_DIR_SEP_CHR);
		strcpy(name2, ++p);
		name1len = fsdevice_evaluate_name_p00(realarg, strlen(realarg),
							name1);
		name1[name1len] = '\0';
		memset(p00name, 0, 17);
		strncpy(p00name, realarg, 16);
		fd = fopen(name2p00, "r+");
		if (fd) {
		    if ((fseek(fd, 8, SEEK_SET) != 0)
                        || (fwrite(p00name, 16, 1, fd) < 1))
			er = IPE_NOT_FOUND;
		    fclose(fd);
		} else {
		    er = IPE_NOT_FOUND;
		}
		if (er == IPE_OK && strcmp(name1, name2) != 0) {
		    int i;
		    for (i = 0; i < 100; i++) {
			memset(name1p00, 0, MAXPATHLEN);
			strcpy(name1p00, fsdevice_get_path(floppy->unit));
                        strcat(name1p00, FSDEV_DIR_SEP_STR);
			strcat(name1p00, name1);
			strcat(name1p00, FSDEV_EXT_SEP_STR);
			strncat(name1p00, &p00type, 1);
			sprintf(p00count, "%02i", i);
			strncat(name1p00, p00count, 2);
			fd = fopen(name1p00, MODE_READ);
			if (fd) {
			    fclose(fd);
			    continue;
			}
			if (rename(name2p00, name1p00) == 0)
			    break;
		    }
		}
	    } else {
		/* Rename CBM file.  */
		if (fsdevice_hide_cbm_files_enabled[floppy->unit - 8]) {
		    fs_error(IPE_NOT_FOUND);
		    fs_cptr = 0;
		    return;
		}
		strcpy(name1, fsdevice_get_path(floppy->unit));
                strcat(name1, FSDEV_DIR_SEP_STR);
		strcat(name1, arg);
		strcpy(name2, fsdevice_get_path(floppy->unit));
                strcat(name2, FSDEV_DIR_SEP_STR);
		strcat(name2, arg2);
		if (rename(name2, name1)) {
		    er = IPE_NOT_FOUND;
		    if (errno == EPERM)
			er = IPE_PERMISSION;
		}
	    }
	}
    }
    fs_error(er);
    fs_cptr = 0;
}

int write_fs(void *flp, BYTE data, int secondary)
{
    if (secondary == 15) {
	if (fs_cptr < MAXPATHLEN - 1) {		/* keep place for nullbyte */
	    fs_cmdbuf[fs_cptr++] = data;
	    return SERIAL_OK;
	} else {
	    fs_error(IPE_LONG_LINE);
	    return SERIAL_ERROR;
	}
    }
    if (fs_info[secondary].mode != Write && fs_info[secondary].mode != Append)
	return FLOPPY_ERROR;

    if (fs_info[secondary].fd) {
	fputc(data, fs_info[secondary].fd);
	return FLOPPY_COMMAND_OK;
    };

    return FLOPPY_ERROR;
}


int read_fs(void *flp, BYTE * data, int secondary)
{
    DRIVE *floppy = (DRIVE *)flp;
    int i, l, f;
    unsigned short blocks;
    struct dirent *dirp;	/* defined in /usr/include/sys/dirent.h */
#ifdef __riscos
    int objType;
    int catInfo[4];
#else
    struct stat statbuf;
#endif
    struct fs_buffer_info *info = &fs_info[secondary];
    char rname[256];

    if (secondary == 15) {
	if (!fs_elen)
	    fs_error(IPE_OK);
	if (fs_eptr < fs_elen) {
	    *data = fs_errorl[fs_eptr++];
	    return SERIAL_OK;
	} else {
	    fs_error(IPE_OK);
	    *data = 0xc7;
	    return SERIAL_EOF;
	}
    }
    switch (info->mode) {
      case Write:
      case Append:
	  return FLOPPY_ERROR;

      case Read:
	  if (info->fd) {
	      i = fgetc(info->fd);
	      if (ferror(info->fd))
		  return FLOPPY_ERROR;
	      if (feof(info->fd)) {
		  *data = 0xc7;
		  return SERIAL_EOF;
	      }
	      *data = i;
	      return SERIAL_OK;
	  }
	  break;

      case Directory:
	  if (info->dp) {
	      if (info->buflen <= 0) {
		  char buf[MAXPATHLEN];

		  info->bufp = info->name;

		  if (info->eof) {
		      *data = 0xc7;
		      return SERIAL_EOF;
		  }
		  /*
		   * Find the next directory entry and return it as a CBM
		   * directory line.
		   */

		  /* first test if dirmask is needed - maybe this should be
		     replaced by some regex functions... */
		  f = 1;
		  do {
		      char *p;
		      dirp = readdir(info->dp);
		      if (!dirp)
			  break;
		      fs_info[secondary].type = FT_PRG;
		      strcpy(rname, dirp->d_name);
		      if (fsdevice_convert_p00_enabled[(floppy->unit) - 8])
			  fs_test_pc64_name(flp, rname, secondary);
			  if (strcmp(rname, dirp->d_name) == 0
			  && fsdevice_hide_cbm_files_enabled[floppy->unit - 8])
			      continue;
		      if (!*fs_dirmask)
			  break;
		      l = strlen(fs_dirmask);
		      for (p = rname, i = 0; *p && fs_dirmask[i] && i < l; i++) {
			  if (fs_dirmask[i] == '?') {
			      p++;
			  } else if (fs_dirmask[i] == '*') {
			      if (!fs_dirmask[i + 1]) {
				  f = 0;
				  break;
			      }	/* end mask */
			      while (*p && (*p != fs_dirmask[i + 1]))
				  p++;
			  } else {
			      if (*p != fs_dirmask[i])
				  break;
			      p++;
			  }
			  if ((!*p) && (!fs_dirmask[i + 1])) {
			      f = 0;
			      break;
			  }
		      }
		  } while (f);

		  if (dirp != NULL) {
		      BYTE *p = info->name;
		      char *tp;

		      strcpy(buf, info->dir);
		      strcat(buf, FSDEV_DIR_SEP_STR);
		      tp = buf + strlen(buf);
		      strcat(buf, dirp->d_name);

		      /* Line link, Length and spaces */

		      p += 2;	/* skip link addr, fill in later */
#ifdef __riscos
                     if ((objType = ReadCatalogueInfo(buf, catInfo)) != 0)
                         blocks = (unsigned short) ((catInfo[2] + 253) / 254);
#else
                     if (stat(buf, &statbuf) >= 0)
                         blocks = (unsigned short) ((statbuf.st_size + 253) / 254);
#endif
                     else
                         blocks = 0;   /* this file can't be opened */

		      SET_LO_HI(p, blocks);

		      if (blocks < 10)
			  *p++ = ' ';
		      if (blocks < 100)
			  *p++ = ' ';
		      if (blocks < 1000)
			  *p++ = ' ';

		      *p++ = ' ';


		      /*
		       * Filename
		       */

		      *p++ = '"';

		      if (strcmp(rname, dirp->d_name)) {
			  for (i = 0; rname[i] && (*p = rname[i]); ++i, ++p);
		      } else {
			  for (i = 0; tp[i] /*i < dirp->d_namlen */ &&
			       (*p = p_topetcii(tp[i] /*dirp->d_name[i] */ )); ++i, ++p);
		      }

		      *p++ = '"';
		      for (; i < 17; i++)
			  *p++ = ' ';

#ifdef __riscos
                     if ((objType & 2) != 0)
#else
                     if (S_ISDIR(statbuf.st_mode))
#endif
                     {
			  *p++ = 'D';
			  *p++ = 'I';
			  *p++ = 'R';
		      } else {
			  switch(fs_info[secondary].type) {
			    case FT_DEL:
			      *p++ = 'D';
			      *p++ = 'E';
			      *p++ = 'L';
			      break;
			    case FT_SEQ:
			      *p++ = 'S';
			      *p++ = 'E';
			      *p++ = 'Q';
			      break;
			    case FT_PRG:
			      *p++ = 'P';
			      *p++ = 'R';
			      *p++ = 'G';
			      break;
			    case FT_USR:
			      *p++ = 'U';
			      *p++ = 'S';
			      *p++ = 'R';
			      break;
			    case FT_REL:
			      *p++ = 'R';
			      *p++ = 'E';
			      *p++ = 'L';
			      break;
			  }
		      }

		      *p = '\0';	/* to allow strlen */

		      /* some (really very) old programs rely on the directory
		         entry to be 32 Bytes in total (incl. nullbyte) */
		      l = strlen((char *) (info->name + 4)) + 4;
		      while (l < 31) {
			  *p++ = ' ';
			  l++;
		      }

		      *p++ = '\0';

		      info->dirmpos += p - info->name;
		      *info->name = info->dirmpos & 0xff;
		      *(info->name + 1) = (info->dirmpos >> 8) & 0xff;

		      info->buflen = (int) (p - info->name);

		  } else {

		      /* EOF => End file */

		      memset(info->name, 0, 2);
		      info->buflen = 2;
		      info->eof++;
		  }
	      }			/* info->buflen */
	      *data = *info->bufp++;
	      info->buflen--;
	      return SERIAL_OK;

	  }			/* info->dp */
	  break;
    }

    return FLOPPY_ERROR;
}


int open_fs(void *flp, const char *name, int length, int secondary)
{
    DRIVE *floppy = (DRIVE *)flp;
    FILE *fd;
    DIR *dp;
    BYTE *p, *linkp;
    char fsname[MAXPATHLEN], fsname2[MAXPATHLEN], rname[MAXPATHLEN];
    char *mask, *comma;
    int status = 0, i, reallength, readmode, rl;

    if (fs_info[secondary].fd)
	return FLOPPY_ERROR;

    memcpy(fsname2, name, length);
    fsname2[length] = 0;

    if (secondary == 15) {
	for (i = 0; i < length; i++)
	    status = write_fs(flp, name[i], 15);
	return status;
    }

    if (secondary == 1)
	readmode = FAM_WRITE;
    else
	readmode = FAM_READ;

    rl = 0;

    if (floppy_parse_name(fsname2, length, fsname, &reallength, &readmode,
			  &fs_info[secondary].type, &rl) != SERIAL_OK)
	return SERIAL_ERROR;

    if (fs_info[secondary].type == FT_DEL)
	fs_info[secondary].type = (secondary < 2) ? FT_PRG : FT_SEQ;

    fsname[reallength] = 0;
    strncpy(rname, fsname, reallength);

    petconvstring(fsname, 1);	/* CBM name to FSname */

    switch (readmode) {
      case FAM_WRITE:
	fs_info[secondary].mode = Write;
	break;
      case FAM_READ:
	fs_info[secondary].mode = Read;
	break;
      case FAM_APPEND:
	fs_info[secondary].mode = Append;
	break;
    }

    if (*name == '$') {	/* Directory read */
	if ((secondary != 0) || (fs_info[secondary].mode != Read)) {
	    fs_error(IPE_NOT_WRITE);
	    return FLOPPY_ERROR;
	}
	/* Test on wildcards.  */
	if (!(mask = strrchr(fsname, '/')))
	    mask = fsname;
	if (strchr(mask, '*') || strchr(mask, '?')) {
	    if (*mask == '/') {
		strcpy(fs_dirmask, mask + 1);
		*mask++ = 0;
	    } else {
		strcpy(fs_dirmask, mask);
		strcpy(fsname, fsdevice_get_path(floppy->unit));
	    }
	} else {
	    *fs_dirmask = 0;
	    if (!*fsname)
		strcpy(fsname, fsdevice_get_path(floppy->unit));
	}
	/* trying to open */
	if (!(dp = opendir((char *) fsname))) {
	    for (p = (BYTE *) fsname; *p; p++)
		if (isupper((int) *p))
		    *p = tolower((int) *p);
	    if (!(dp = opendir((char *) fsname))) {
		fs_error(IPE_NOT_FOUND);
		return FLOPPY_ERROR;
	    }
	}
	strcpy(fs_info[secondary].dir, fsname);

	/*
	 * Start Address, Line Link and Line number 0
	 */

	p = fs_info[secondary].name;

	*p++ = 1;
	*p++ = 4;

	linkp = p;
	p += 2;

	*p++ = 0;
	*p++ = 0;

	*p++ = (BYTE) 0x12;	/* Reverse on */

	*p++ = '"';
	strcpy((char *) p, fs_info[secondary].dir);	/* Dir name */
	petconvstring((char *) p, 0);	/* ASCII name to PETSCII */
	i = 0;
	while (*p) {
	    ++p;
	    i++;
	}
	while (i < 16) {
	    *p++ = ' ';
	    i++;
	}
	*p++ = '"';
	while (i < 22) {
	    *p++ = ' ';
	    i++;
	}
	*p++ = 0;

	i = 0x0401 + p - linkp;
	*linkp = i & 0xff;
	*(linkp + 1) = (i >> 8) & 0xff;

	fs_info[secondary].buflen = p - fs_info[secondary].name;
	fs_info[secondary].bufp = fs_info[secondary].name;
	fs_info[secondary].mode = Directory;
	fs_info[secondary].dp = dp;
	fs_info[secondary].eof = 0;
	fs_info[secondary].dirmpos = i;		/* start address of next line */

    } else {			/* Normal file, not directory ("$") */

	/* Override access mode if secondary address is 0 or 1.  */
	if (secondary == 0)
	    fs_info[secondary].mode = Read;
	if (secondary == 1)
	    fs_info[secondary].mode = Write;

	/* Remove comma.  */
	if (fsname[0] == ',') {
	    fsname[1] = '\0';
	} else {
	    comma = strchr(fsname, ',');
	    if (comma != NULL)
		*comma = '\0';
	}
        strcpy(fsname2, fsname);
        strcpy(fsname, fsdevice_get_path(floppy->unit));
        strcat(fsname, FSDEV_DIR_SEP_STR);
        strcat(fsname, fsname2);

	/* Test on wildcards.  */
	if (strchr(fsname2, '*') || strchr(fsname2, '?')) {
	    if (fs_info[secondary].mode == Write
				|| fs_info[secondary].mode == Append) {
		fs_error(IPE_BAD_NAME);
		return FLOPPY_ERROR;
	    } else {
		fsdevice_compare_file_name(flp, fsname2, fsname, secondary);
	    }
	}

	/* Open file for write mode access.  */
	if (fs_info[secondary].mode == Write) {
	    fd = fopen(fsname, MODE_READ);
	    if (fd != NULL) {
		fclose(fd);
		fs_error(IPE_FILE_EXISTS);
		return FLOPPY_ERROR;
	    }
	    if (fsdevice_convert_p00_enabled[(floppy->unit) - 8]) {
		fd = fs_find_pc64_name(flp, rname, reallength, fsname2);
		if (fd != NULL) {
		    fclose(fd);
		    fs_error(IPE_FILE_EXISTS);
		    return FLOPPY_ERROR;
		}
	    }
	    if (fsdevice_save_p00_enabled[(floppy->unit) - 8]) {
		if (fsdevice_create_file_p00(flp, rname, reallength, fsname,
							secondary) > 0) {
		    fs_error(IPE_FILE_EXISTS);
		    return FLOPPY_ERROR;
		} else {
		    fd = fopen(fsname, "a+");
		    fs_info[secondary].fd = fd;
		    fs_error(IPE_OK);
		    return FLOPPY_COMMAND_OK;
		}
	    } else {
		fd = fopen(fsname, MODE_WRITE);
		fs_info[secondary].fd = fd;
		fs_error(IPE_OK);
		return FLOPPY_COMMAND_OK;
	    }
	}

	/* Open file for append mode access.  */
	if (fs_info[secondary].mode == Append) {
	    fd = fopen(fsname, MODE_READ);
	    if (!fd) {
		if (!fsdevice_convert_p00_enabled[(floppy->unit) - 8]) {
		    fs_error(IPE_NOT_FOUND);
		    return FLOPPY_ERROR;
		}
		fd = fs_find_pc64_name(flp, rname, reallength, fsname2);
		if (!fd) {
		    fs_error(IPE_NOT_FOUND);
		    return FLOPPY_ERROR;
		}
		fclose(fd);
		fd = fopen(fsname2, "a+");
		if (!fd) {
		    fs_error(IPE_NOT_FOUND);
		    return FLOPPY_ERROR;
		}
		fs_info[secondary].fd = fd;
		fs_error(IPE_OK);
		return FLOPPY_COMMAND_OK;
	    } else {
		fclose(fd);
		fd = fopen(fsname, "a+");
		if (!fd) {
		    fs_error(IPE_NOT_FOUND);
		    return FLOPPY_ERROR;
		}
		fs_info[secondary].fd = fd;
		fs_error(IPE_OK);
		return FLOPPY_COMMAND_OK;
	    }
	}

	/* Open file for read mode access.  */
 	fd = fopen(fsname, MODE_READ);
	if (!fd) {
	    if (!fsdevice_convert_p00_enabled[(floppy->unit) - 8]) {
		fs_error(IPE_NOT_FOUND);
		return FLOPPY_ERROR;
	    }
	    fd = fs_find_pc64_name(flp, rname, reallength, fsname2);
	    if (!fd) {
		fs_error(IPE_NOT_FOUND);
		return FLOPPY_ERROR;
	    }
	    fs_info[secondary].fd = fd;
	    fs_error(IPE_OK);
	    return FLOPPY_COMMAND_OK;
	} else {
	    if (fsdevice_hide_cbm_files_enabled[floppy->unit - 8]) {
		fclose(fd);
		fs_error(IPE_NOT_FOUND);
		return FLOPPY_ERROR;
	    }
	    fs_info[secondary].fd = fd;
	    fs_error(IPE_OK);
	    return FLOPPY_COMMAND_OK;
	}
    }
#ifdef __riscos
    ui_set_drive_leds(floppy->unit - 8, 1);
#endif
    fs_error(IPE_OK);
    return FLOPPY_COMMAND_OK;
}


int close_fs(void *flp, int secondary)
{
#ifdef __riscos
    DRIVE *floppy = (DRIVE*)flp;

    ui_set_drive_leds(floppy->unit - 8, 0);
#endif

    if (secondary == 15) {
	fs_error(IPE_OK);
	return FLOPPY_COMMAND_OK;
    }
    switch (fs_info[secondary].mode) {
      case Write:
      case Read:
      case Append:
	  if (!fs_info[secondary].fd)
	      return FLOPPY_ERROR;

	  fclose(fs_info[secondary].fd);
	  fs_info[secondary].fd = NULL;
	  break;

      case Directory:
	  if (!fs_info[secondary].dp)
	      return FLOPPY_ERROR;

	  closedir(fs_info[secondary].dp);
	  fs_info[secondary].dp = NULL;
	  break;
    }

    return FLOPPY_COMMAND_OK;
}

void fs_test_pc64_name(void *flp, char *rname, int secondary)
{
    DRIVE *floppy = (DRIVE *)flp;
    char p00id[8];
    char p00name[17];
    char pathname[MAXPATHLEN];
    FILE *fd;
    int tmptype;

    tmptype = p00_check_name(rname);
    if (tmptype >= 0) {
        strcpy(pathname, fsdevice_get_path(floppy->unit));
        strcat(pathname, FSDEV_DIR_SEP_STR);
        strcat(pathname, rname);
        fd = fopen(pathname, MODE_READ);
        if (!fd)
            return;

	fread((char *) p00id, 8, 1, fd);
	if (ferror(fd)) {
	    fclose(fd);
	    return;
	}
	p00id[7] = '\0';
	if (!strncmp(p00id, "C64File", 7)) {
	    fread((char *) p00name, 16, 1, fd);
	    if (ferror(fd)) {
		fclose(fd);
		return;
	    }
	    fs_info[secondary].type = tmptype;
	    p00name[16] = '\0';
	    strcpy(rname, p00name);
	    fclose(fd);
	    return;
	}
	fclose(fd);
    }
}


FILE *fs_find_pc64_name(void *flp, char *name, int length, char *pname)
{
    DRIVE *floppy = (DRIVE *)flp;
    struct dirent *dirp;
    char *p;
    DIR *dp;
    char p00id[8], p00name[17], p00dummy[2];
    FILE *fd;

    name[length] = '\0';

    dp = opendir(fsdevice_get_path(floppy->unit));
    do {
	dirp = readdir(dp);
	if (dirp != NULL) {
	    strcpy(pname, fsdevice_get_path(floppy->unit));
            strcat(pname, FSDEV_DIR_SEP_STR);
	    strcat(pname, dirp->d_name);
	    p = pname;
	    if (p00_check_name(p) >= 0) {
		fd = fopen(p, MODE_READ);
		if (!fd)
		    continue;
		fread((char *) p00id, 8, 1, fd);
		if (ferror(fd)) {
		    fclose(fd);
		    continue;
		}
		p00id[7] = '\0';
		if (!strncmp(p00id, "C64File", 7)) {
		    fread((char *) p00name, 16, 1, fd);
		    if (ferror(fd)) {
			fclose(fd);
			continue;
		    }
		    p00name[16] = '\0';
		    if (fsdevice_compare_wildcards(name, p00name) > 0) {
			fread((char *) p00dummy, 2, 1, fd);
			if (ferror(fd)) {
			    fclose(fd);
			    continue;
			}
			return fd;
		    }
		}
		fclose(fd);
	    }
	}
    }
    while (dirp != NULL);
    closedir(dp);
    return NULL;
}

static int fsdevice_compare_wildcards(char *name, char *p00name)
{
    int i, len;

    len = strlen(name);
    if (len == 0)
	return 0;

    for (i = 0; i < len; i++) {
	if (name[i] == '*')
	    return 1;
	if (name[i] != '?' && name[i] != p00name[i])
	    return 0;
    }
    return 1;
}

static void fsdevice_compare_file_name(void *flp, char *fsname2, char *fsname,
                                       int secondary)
{
    DRIVE *floppy = (DRIVE *)flp;
    struct dirent *dirp;
    DIR *dp;
    char rname[MAXPATHLEN];

    dp = opendir(fsdevice_get_path(floppy->unit));
    do {
	dirp = readdir(dp);
	if (dirp != NULL) {
	    if (fsdevice_compare_wildcards(fsname2, dirp->d_name) > 0) {
		strcpy(rname, dirp->d_name);
		fs_test_pc64_name(flp, rname, secondary);
		if (strcmp(rname, dirp->d_name) == 0) {
		    strcpy(fsname, fsdevice_get_path(floppy->unit));
		    strcat(fsname, FSDEV_DIR_SEP_STR);
		    strcat(fsname, dirp->d_name);
		    closedir(dp);
		    return;
		}
	    }
	}
    }
    while (dirp != NULL);
    closedir(dp);
    return;
}

static int fsdevice_create_file_p00(void *flp, char *name, int length,
                                     char *fsname, int secondary)
{
    DRIVE *floppy = (DRIVE *)flp;
    char filename[17], realname[16];
    int i, len;
    FILE *fd;

    if (length > 16)
	length = 16;
    memset(realname, 0, 16);
    strncpy(realname, name, length);

    len = fsdevice_evaluate_name_p00(name, length, filename);

    strcpy(fsname, fsdevice_get_path(floppy->unit));
    strcat(fsname, FSDEV_DIR_SEP_STR);
    strncat(fsname, filename, len);
    switch (fs_info[secondary].type) {
      case FT_DEL:
	strcat(fsname, FSDEV_EXT_SEP_STR ".D");
	break;
      case FT_SEQ:
	strcat(fsname, FSDEV_EXT_SEP_STR ".S");
	break;
      case FT_PRG:
	strcat(fsname, FSDEV_EXT_SEP_STR ".P");
	break;
      case FT_USR:
	strcat(fsname, FSDEV_EXT_SEP_STR ".U");
	break;
      case FT_REL:
	strcat(fsname, FSDEV_EXT_SEP_STR ".R");
	break;
    }
    strcat(fsname, "00");

    for (i = 1; i < 100; i++) {
	fd = fopen(fsname, MODE_READ);
	if (!fd)
	    break;
	fclose(fd);
	sprintf(&fsname[strlen(fsname) - 2], "%02i", i);
    }

    if (i >= 100)
	return 1;

    fd = fopen(fsname, MODE_WRITE);
    if (!fd)
	return 1;

    if (fwrite("C64File", 8, 1, fd) < 1) {
	fclose(fd);
	return 1;
    }
    if (fwrite(realname, 16, 1, fd) < 1) {
	fclose(fd);
	return 1;
    }
    if (fwrite("\0\0", 2, 1, fd) < 1) {
	fclose(fd);
	return 1;
    }
    fclose(fd);
    return 0;
}

static int fsdevice_reduce_filename_p00(char *filename, int len)
{
    int i, j;

    for (i = len - 1; i >= 0; i--) {
	if (filename[i] == '_')
	    if (fsdevice_eliminate_char_p00(filename, i) <= 8)
		return 8;
	}

    for (i = 0; i < len; i++) {
	if (strchr("AEIOU", filename[i]) != NULL)
	    break;
    }

    for (j = len - 1; j >= i; j--) {
	if (strchr("AEIOU", filename[j]) != NULL)
	    if (fsdevice_eliminate_char_p00(filename, j) <= 8)
		return 8;
    }

    for (i = len - 1; i >= 0; i--) {
	if (isalpha((int) filename[i]))
	    if (fsdevice_eliminate_char_p00(filename, i) <= 8)
		return 8;
    }

    for (i = len - 1; i >= 0; i--)
	if (fsdevice_eliminate_char_p00(filename, i) <= 8)
	    return 8;

    return 1;
}

static int fsdevice_eliminate_char_p00(char *filename, int pos)
{
    memcpy(&filename[pos], &filename[pos+1], 16 - pos);
    return strlen(filename);
}

static int fsdevice_evaluate_name_p00(char *name, int length, char *filename)
{
    int i, j;

    memset(filename, 0, 17);

    for (i = 0, j = 0; i < length; i++) {
	switch (name[i]) {
	  case ' ':
	  case '-':
	    filename[j++] = '_';
	    break;
	  default:
	    if (islower((int) name[i])) {
		filename[j++] = toupper(name[i]);
		break;
	    }
	    if (isalnum((int) name[i])) {
		filename[j++] = name[i];
		break;
	    }
	}
    }
    if (j == 0) {
	strcpy(filename, "_");
	j++;
    }
    return ((j > 8) ? fsdevice_reduce_filename_p00(filename, j) : j);
}
