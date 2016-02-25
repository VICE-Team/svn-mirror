/*
 * archdep_dingoo.c - Miscellaneous dingoo-specific stuff.
 *
 * Written by
 *  peiselulli <peiselulli@t-online.de>
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

#include "render1x1.h"
#include "types.h"

#include <stdio.h>

#ifdef HAVE_DIRECT_H
#include <direct.h>
#endif
#ifdef HAVE_DIRENT_H
#include <dirent.h>
#endif
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#ifdef HAVE_IO_H
#include <io.h>
#endif
#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <string.h>
#include <stdlib.h>

#include "archdep.h"
#include "ioutil.h"
#include "keyboard.h"
#include "lib.h"
#include "types.h"
#include "util.h"

#include <dingoo/slcd.h>
#include <dingoo/cache.h>
#include <dingoo/keyboard.h>
#include <jz4740/cpu.h>
#include <dingoo/fsys.h>
#include <dingoo/ucos2.h>
#include <dingoo/entry.h>
#include <dingoo/jz4740.h>

#ifdef DINGOO_DEBUG
#include "../samples/zaxxon_hello_world/FontWhite.h"
#endif

#define CONTROL_BUTTON_SELECT 0x00000400
#define CONTROL_BUTTON_START 0x00000800

static char _app_path[FILENAME_MAX];
static char *_program_name;

unsigned short *g_pGameDecodeBuf = 0L;

#ifdef DINGOO_DEBUG

void PutChar(int X, int Y, unsigned char Char)
{
    unsigned short *Frame;
    int x, y;

    Frame = _lcd_get_frame();
    for (y = 0; y < CHAR_HEIGHT; y++) {
        for (x = 0; x < CHAR_WIDTH; x++) {
            Frame[(Y * CHAR_HEIGHT + y) * 320 + (X * CHAR_WIDTH + x)] = CHARS[Char][y * CHAR_WIDTH + x];
        }
    }
}

void PutString(int X, int Y, unsigned char *Str)
{
    while (*Str != 0) {
        PutChar(X, Y, *Str);
        X++;
        Str++;
        if (X > 39) {
            X = 0;
            Y++;
            if (Y > 13) {
                Y = 0;
            }
        }
    }
}

static char _buffer[256];

int trace(int line, char* file)
{
    KEY_STATUS KS;
    char buffer[256];

    memset(buffer, ' ', 80);
    snprintf(buffer, sizeof(buffer), "%s <%d>", file, line);
    strcpy(_buffer, buffer);
    PutString(1, 5, buffer);
    __dcache_writeback_all();
    _lcd_set_frame();
    _kbd_get_status(&KS);
    if (KS.status & CONTROL_BUTTON_START) {
        exit(0);
    }
    mdelay(500);
    return EXIT_SUCCESS;
}

void atexitfunc(void)
{
    KEY_STATUS KS;
    char buffer[256];

    if (strlen(_buffer) > 0) {
        PutString(0, 7, _buffer);
        __dcache_writeback_all();
        _lcd_set_frame();
        do {
            kbd_get_status(&KS);
        } while ((KS.status & CONTROL_BUTTON_SELECT) == 0);
    }
}
#endif

#define FL_UNSIGNED   1
#define FL_NEG        2
#define FL_OVERFLOW   4
#define FL_READDIGIT  8

static uintptr_t tempCore;
static uintptr_t tempLcd;
static uintptr_t tempLcdFactor;
static int dirs_amount = 0;
static int files_amount = 0;

char _app_name[] = "vice";

int archdep_init_extra(int *argc, char **argv)
{
    uintptr_t tempMemory;

    cpu_clock_get(&tempCore, &tempMemory);
    tempLcd = REG_CPM_LPCDR;
    tempLcdFactor = tempCore / tempLcd;
}

void set_overclock(int activate)
{
    uintptr_t tempCore2, tempMemory;

    if (activate) {
        if (is_lcd_active()) {
            cpu_clock_set(430000000);
            REG_CPM_LPCDR = 11;
        } else {
            cpu_clock_set(410000000);
            REG_CPM_LPCDR = 11;
        }
        cpu_clock_get(&tempCore2, &tempMemory);
    } else {
        cpu_clock_set(tempCore);
        REG_CPM_LPCDR = tempLcd;
    }
}

void archdep_shutdown_extra(void)
{
    set_overclock(0);
}

int archdep_require_vkbd(void)
{
    return 1;
}

char *archdep_default_hotkey_file_name(void)
{
    char *ret;

    ret = lib_malloc(32);
    strcpy(ret, "sdl-hotkey-");
    strcat(ret, (char *)machine_get_name());
    strcat(ret, ".vkm");
    return ret;
}

int archdep_default_logger(const char *level_string, const char *txt)
{
    return 0;
}

FILE *archdep_open_default_log_file(void)
{
    return NULL;
}

char *archdep_filename_parameter(const char *name)
{
    return lib_stralloc(name);
}

char *archdep_quote_parameter(const char *name)
{
    /* not needed(?) */
    return lib_stralloc(name);
}

int archdep_expand_path(char **return_path, const char *orig_name)
{
    char tmp[FILENAME_MAX];
    int len;

    if (archdep_path_is_relative(orig_name)) {
        getcwd(tmp, FILENAME_MAX);
        len = strlen(tmp);
        if (tmp[len - 1] == FSDEV_DIR_SEP_CHR) {
            *return_path = util_concat(tmp, orig_name, NULL);
        } else {
            *return_path = util_concat(tmp, FSDEV_DIR_SEP_STR, orig_name, NULL);
        }
    } else {
        *return_path = lib_stralloc(orig_name);
    }
    return 0;
}

char *archdep_program_name(void)
{
    return lib_stralloc(_program_name);
}

char *make_absolute_system_path(const char *s)
{
    char *ret;
    int len;

    len = strlen(s) + 2;
    ret = lib_malloc(strlen(_app_path) + len);
    strcpy(ret, _app_path);
    strcat(ret, FSDEV_DIR_SEP_STR);
    strcat(ret, s);
    return ret;
}

char *archdep_default_resource_file_name(void)
{
    return make_absolute_system_path("vicerc-native");
}

char *archdep_default_save_resource_file_name(void)
{
    return make_absolute_system_path("vicerc-native");
}

char *archdep_make_backup_filename(const char *fname)
{
    return make_absolute_system_path("vicerc.bu");
}

char *archdep_default_fliplist_file_name(void)
{
    return make_absolute_system_path("fliplist");
}

char *archdep_default_rtc_file_name(void)
{
    return make_absolute_system_path("rtcfile");
}

int archdep_file_set_gzip(const char *name)
{
}

int archdep_path_is_relative(const char *p)
{
    if (p == NULL) {
        return 0;
    }
    if (*p != FSDEV_DIR_SEP_CHR) {
        return 0;
    }
    if (p[1] == ':') {
        return 0;
    }
    return 1;
}

int archdep_file_is_gzip(const char *name)
{
    return 0;
}

FILE *archdep_mkstemp_fd(char **filename, const char *mode)
{
    return NULL;
}

void archdep_startup_log_error(const char *format, ...)
{
}

char *archdep_tmpnam(void)
{
    static char s[FILENAME_MAX];

    strcpy(s, _app_path);
    strcat(s, FSDEV_DIR_SEP_STR);
    strcat(s, "tmp");
    return lib_stralloc(s);
}

char *archdep_default_autostart_disk_image_file_name(void)
{
    return util_concat("autostart-", machine_get_name(), ".d64", NULL);
}

char *archdep_default_sysfile_pathlist(const char *emu_id)
{
    return lib_stralloc(_app_path);
}

int archdep_stat(const char *file_name, unsigned int *len, unsigned int *isdir)
{
    struct stat statbuf;

    if (stat(file_name, &statbuf) < 0) {
        *len = 0;
        *isdir = 0;
        return -1;
    }

    *len = statbuf.st_size;
    *isdir = S_ISDIR(statbuf.st_mode);

    return 0;
}

int archdep_rename(const char *oldpath, const char *newpath)
{
    return rename(oldpath, newpath);
}

int access(const char *pathname, int mode)
{
    return 0;
}

void set_dingoo_pwd(const char *path)
{
    char *ptr;

    char _path[FILENAME_MAX];
    strcpy(_path, path);
    ptr = (char *)strrchr(_path, FSDEV_DIR_SEP_CHR);
    _program_name = lib_stralloc(ptr + 1);
    ptr[0] = 0;
    strcpy(_app_path, _path);
}

static int ioutil_compare_names(const void* a, const void* b)
{
    ioutil_name_table_t *arg1 = (ioutil_name_table_t*)a;
    ioutil_name_table_t *arg2 = (ioutil_name_table_t*)b;
    return strcmp(arg1->name, arg2->name);
}

static void create_dir_string(char *path_string)
{
    int path_size;
    getcwd(path_string, FILENAME_MAX);
    path_size = strlen(path_string);
    if (path_string[path_size - 1] == FSDEV_DIR_SEP_CHR) {
        strcat(path_string, "*");
    } else {
        strcat(path_string, FSDEV_DIR_SEP_STR "*");
    }
}

static void ioutil_count_dir_items(const char *path, int *dir_count, int *files_count)
{
    int ret;
    fsys_file_info_t fData;

    *dir_count = 1; /* with ".." */
    *files_count = 0;
    char path_string[FILENAME_MAX];

    create_dir_string(path_string);
    ret = fsys_findfirst(path_string, FSYS_FIND_DIR, &fData);
    if (ret == 0) {
        do {
            ++(*dir_count);
        } while (fsys_findnext(&fData) == 0);
        fsys_findclose(&fData);
    }
    ret = fsys_findfirst(path_string, FSYS_FIND_FILE, &fData);
    if (ret == 0) {
        do {
            ++(*files_count);
        } while (fsys_findnext(&fData) == 0);
        fsys_findclose(&fData);
    }
}

static void ioutil_filldir(const char *path, ioutil_name_table_t *dirs, ioutil_name_table_t *files)
{
    fsys_file_info_t fData;
    int dir_count = 0;
    int file_count = 0;
    int ret;
    char *filename;
    char path_string[FILENAME_MAX];

    create_dir_string(path_string);
    dirs[dir_count].name = lib_stralloc("..");
    ++dir_count;

    ret = fsys_findfirst(path_string, FSYS_FIND_DIR, &fData);
    if (ret == 0) {
        do {
            dirs[dir_count].name = lib_stralloc(fData.name);
            ++dir_count;
        } while (fsys_findnext(&fData) == 0);
        fsys_findclose(&fData);
    }

    ret = fsys_findfirst(path_string, FSYS_FIND_FILE, &fData);
    if (ret == 0) {
        do {
            files[file_count].name = lib_stralloc(fData.name);
            ++file_count;
        } while (fsys_findnext(&fData) == 0);
        fsys_findclose(&fData);
    }
}

ioutil_dir_t *ioutil_opendir(const char *path)
{
    int dir_count, files_count;
    ioutil_dir_t *ioutil_dir;

    ioutil_count_dir_items(path, &dir_count, &files_count);
    if (dir_count + files_count < 0) {
        return NULL;
    }

    ioutil_dir = lib_malloc(sizeof(ioutil_dir_t));

    ioutil_dir->dirs = lib_malloc(sizeof(ioutil_name_table_t) * dir_count);
    ioutil_dir->files = lib_malloc(sizeof(ioutil_name_table_t) * files_count);

    ioutil_filldir(path, ioutil_dir->dirs, ioutil_dir->files);
    qsort(ioutil_dir->dirs, dir_count, sizeof(ioutil_name_table_t), ioutil_compare_names);
    qsort(ioutil_dir->files, files_count, sizeof(ioutil_name_table_t), ioutil_compare_names);

    ioutil_dir->dir_amount = dir_count;
    ioutil_dir->file_amount = files_count;
    ioutil_dir->counter = 0;

    return ioutil_dir;
}

void dingoo_reboot(void)
{
    REG_WDT_TCSR = WDT_TCSR_PRESCALE4 | WDT_TCSR_EXT_EN;
    REG_WDT_TCNT = 0;
    REG_WDT_TDR = JZ_EXTAL / 1000;   /* reset after 4ms */
    REG_TCU_TSCR = TCU_TSSR_WDTSC;   /* enable wdt clock */
    REG_WDT_TCER = WDT_TCER_TCEN;    /* wdt start */
    while (1) {
    }
}

char *archdep_get_runtime_os(void)
{
    /* TODO: add runtime os detection code */
    return "Unknown OS";
}

char *archdep_get_runtime_cpu(void)
{
    /* TODO: add runtime cpu detection code */
    return "Unknown CPU";
}

/* returns host keyboard mapping. used to initialize the keyboard map when
   starting with a black (default) config, so an educated guess works good
   enough most of the time :)

   FIXME: add more languages/actual detection
*/
int kbd_arch_get_host_mapping(void)
{
    return KBD_MAPPING_US;
}
