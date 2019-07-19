/*
 * archdep.c - Miscellaneous system-specific stuff.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

#ifdef AMIGA_SUPPORT
#ifndef __VBCC__
#define __USE_INLINE__
#endif

#include <proto/dos.h>
#include <proto/exec.h>

#ifndef AMIGA_OS4
#include <proto/socket.h>
#endif
#endif /* AMIGA_SUPPORT */

#include "vice_sdl.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <sys/stat.h>
#include <unistd.h>

#include "archdep.h"
#include "findpath.h"
#include "ioutil.h"
#include "kbd.h"
#include "keyboard.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "ui.h"
#include "util.h"

/* FIXME: includes for beos */
/* FIXME: includes for amiga */
/* FIXME: includes for os/2 */

/* These functions are defined in the files included below and
   used lower down. */
static int archdep_init_extra(int *argc, char **argv);
static void archdep_shutdown_extra(void);

#ifdef AMIGA_SUPPORT

#if defined(AMIGA_OS4)
#include <exec/execbase.h>
#ifndef __USE_BASETYPE__
  extern struct Library * SysBase;
#else
  extern struct ExecBase * SysBase;
#endif /* __USE_BASETYPE__ */
#endif

#ifndef AMIGA_OS4
struct Library *SocketBase;
#endif

#ifdef POWERSDL_AMIGA_INLINE
struct Library *PowerSDLBase = NULL;
#define SDLLIBBASE PowerSDLBase
#define SDLLIBNAME "powersdl.library"
#endif

#ifdef SDL_AMIGA_INLINE
struct Library *SDLBase = NULL;
#define SDLLIBBASE SDLBase
#define SDLLIBNAME "SDL.library"
#endif

#if defined(SDL_AMIGA_INLINE) || defined(POWERSDL_AMIGA_INLINE)
void SDL_Quit(void)
{
    SDL_RealQuit();
    CloseLibrary(SDLLIBBASE);
}
#endif

#ifdef SDL_AMIGA_INLINE
int SDL_Init(Uint32 flags)
{
    SDLLIBBASE = OpenLibrary(SDLLIBNAME, 0L);

    if (!SDLLIBBASE) {
        printf("Unable to open %s\n", SDLLIBNAME);
        archdep_vice_exit(0);
    }

    return SDL_RealInit(flags);
}
#endif

#ifdef POWERSDL_AMIGA_INLINE
int VICE_SDL_Init(Uint32 flags)
{
    SDLLIBBASE = OpenLibrary(SDLLIBNAME, 0L);

    if (!SDLLIBBASE) {
        printf("Unable to open %s\n", SDLLIBNAME);
        archdep_vice_exit(0);
    }
    return SDL_Init(flags);
}

#define SDL_REALINIT VICE_SDL_Init
#endif

#define __USE_INLINE__

#undef BYTE
#undef WORD
#include <exec/types.h>
#include <exec/nodes.h>
#include <exec/lists.h>
#include <exec/memory.h>

#include <proto/exec.h>
#include <proto/intuition.h>

#ifdef AMIGA_OS4
struct Library *ExpansionBase = NULL;
struct ExpansionIFace *IExpansion = NULL;
#endif

#ifdef HAVE_PROTO_OPENPCI_H
struct Library *OpenPciBase = NULL;
#endif

#if defined(HAVE_PROTO_OPENPCI_H) || defined(AMIGA_OS4)
int pci_lib_loaded = 1;
#endif

/* ----------------------------------------------------------------------- */

#define LIBS_ACTION_ERROR     0
#define LIBS_ACTION_WARNING   1

typedef struct amiga_libs_s {
    char *lib_name;
    void **lib_base;
    int lib_version;
    void **interface_base;
    int action;
    int **var;
} amiga_libs_t;

static amiga_libs_t amiga_libs[] = {
#ifdef AMIGA_OS4
    { "expansion.library", &ExpansionBase, 50, &IExpansion, LIBS_ACTION_WARNING, &pci_lib_loaded },
#endif
#ifdef HAVE_PROTO_OPENPCI_H
    { "openpci.library", &OpenPciBase, 0, NULL, LIBS_ACTION_WARNING, &pci_lib_loaded },
#endif
    { NULL, NULL, 0, NULL, 0, NULL }
};

int load_libs(void)
{
    int i = 0;

    while (amiga_libs[i].lib_name) {
        amiga_libs[i].lib_base[0] = OpenLibrary(amiga_libs[i].lib_name, amiga_libs[i].lib_version);
#ifdef AMIGA_OS4
        if (amiga_libs[i].lib_base[0]) {
            amiga_libs[i].interface_base[0] = GetInterface(amiga_libs[i].lib_base[0], "main", 1, NULL);
            if (amiga_libs[i].interface_base[0] == NULL) {
                CloseLibrary(amiga_libs[i].lib_base[0]);
                amiga_libs[i].lib_base[0] = NULL;
            }
        }
#endif
        if (!amiga_libs[i].lib_base[0]) {
            if (amiga_libs[i].action == LIBS_ACTION_ERROR) {
                return -1;
            } else {
                amiga_libs[i].var[0] = 0;
            }
        }
        i++;
    }
    return 0;
}

void close_libs(void)
{
    int i = 0;

    while (amiga_libs[i].lib_name) {
#ifdef AMIGA_OS4
        if (amiga_libs[i].interface_base) {
            DropInterface((struct Interface *)amiga_libs[i].interface_base[0]);
        }
#endif
        if (amiga_libs[i].lib_base) {
            CloseLibrary(amiga_libs[i].lib_base[0]);
        }
        i++;
    }
}

#endif /* AMIGA_SUPPORT */

#ifdef BEOS_COMPILE

/* This check is needed for haiku, since it always returns 1 on
   SupportsWindowMode() */
int CheckForHaiku(void)
{
    struct utsname name;

    uname(&name);
    if (!strncasecmp(name.sysname, "Haiku", 5)) {
        return -1;
    }
    return 0;
}
#endif

#include "kbd.h"
#include "log.h"

#ifndef SDL_REALINIT
#define SDL_REALINIT SDL_Init
#endif

/*
 * XXX: this will get fixed once the code in this file is moved into
 *      src/arch/shared
 */
#include "../shared/archdep_atexit.h"
#include "../shared/archdep_create_user_config_dir.h"
#include "../shared/archdep_user_config_path.h"

/******************************************************************************/
static char *argv0 = NULL;
#if defined(ARCHDEP_OS_BEOS)
static char *orig_workdir = NULL;
#endif
#if defined(ARCHDEP_OS_AMIGA)    
static int run_from_wb = 0;
#endif

/* called from archdep.c:archdep_init */
static int archdep_init_extra(int *argc, char **argv)
{
#if defined(ARCHDEP_OS_WINDOWS)
    _fmode = O_BINARY;

    _setmode(_fileno(stdin), O_BINARY);
    _setmode(_fileno(stdout), O_BINARY);
#endif
    
#if defined(ARCHDEP_OS_WINDOWS) || defined(ARCHDEP_OS_BEOS) || defined(ARCHDEP_OS_OS2)
    argv0 = lib_strdup(argv[0]);
#endif
#if defined(ARCHDEP_OS_BEOS)
    orig_workdir = getcwd(NULL, PATH_MAX);
#endif
    
#if defined(ARCHDEP_OS_AMIGA)    
    if (*argc == 0) { /* run from WB */
        run_from_wb = 1;
    } else { /* run from CLI */
        run_from_wb = 0;
    }
    load_libs();
#endif    
    return 0;
}

/* called from archdep.c:archdep_shutdown */
static void archdep_shutdown_extra(void)
{
    if (argv0) {
        lib_free(argv0);
    }
#if defined(ARCHDEP_OS_AMIGA)    
    lib_free(boot_path);
    close_libs();
#endif    
}

/******************************************************************************/

int archdep_init(int *argc, char **argv)
{
    archdep_program_path_set_argv0(argv[0]);

    archdep_create_user_config_dir();

    if (SDL_REALINIT(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
        fprintf(stderr, "SDL error: %s\n", SDL_GetError());
        return 1;
    }

    /*
     * Call SDL_Quit() via atexit() to avoid segfaults on exit.
     * See: https://wiki.libsdl.org/SDL_Quit
     * I'm not sure this actually registers SDL_Quit() as the last atexit()
     * call, but it appears to work at least (BW)
     */
    if (archdep_vice_atexit(SDL_Quit) != 0) {
        log_error(LOG_ERR,
                "failed to register SDL_Quit() with archdep_vice_atexit().");
        archdep_vice_exit(1);
    }

    return archdep_init_extra(argc, argv);
}


void archdep_shutdown(void)
{
    /* free memory used by the exec path */
    archdep_program_path_free();
    /* free memory used by the exec name */
    archdep_program_name_free();
    /* free memory used by the boot path */
    archdep_boot_path_free();
    /* free memory used by the home path */
    archdep_home_path_free();
    /* free memory used by the config files path */
    archdep_user_config_path_free();
    /* free memory used by the sysfile pathlist */
    archdep_default_sysfile_pathlist_free();

#ifdef HAVE_NETWORK
    archdep_network_shutdown();
#endif
    archdep_shutdown_extra();
    archdep_extra_title_text_free();
}
