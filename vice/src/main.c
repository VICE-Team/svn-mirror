/*
 * main.c - VICE startup.
 *
 * Written by
 *  Vesa-Matti Puro  (vmp@lut.fi)
 *  Jarkko Sonninen  (sonninen@lut.fi)
 *  Jouko Valta      (jopi@stekt.oulu.fi)
 *  Ettore Perazzoli (ettore@comm2000.it)
 *  André Fachat     (a.fachat@physik.tu-chemnitz.de)
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

#ifdef __hpux
#ifndef _INCLUDE_POSIX_SOURCE
#define _INCLUDE_POSIX_SOURCE
#endif
#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE
#endif
#endif  /* __hpux */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

/* system info */
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

#ifdef __hpux
#define _INCLUDE_XOPEN_SOURCE
#define _XPG2
#include <limits.h>
#undef  _INCLUDE_XOPEN_SOURCE
#undef  _XPG2
#else
#include <limits.h>
#endif

#ifdef __GO32__
#include <fcntl.h>
#endif

#include "patchlevel.h"
#include "maincpu.h"
#include "serial.h"
#include "interrupt.h"
#include "sid.h"
#include "ui.h"
#include "vsync.h"
#include "video.h"
#include "kbd.h"
#include "resources.h"
#include "mon.h"
#include "autostart.h"
#include "findpath.h"
#include "machspec.h"
#include "utils.h"
#include "joystick.h"

#ifdef __MSDOS__
#include "vmidas.h"
#endif

/* ------------------------------------------------------------------------- */

const char *progname;
const char *boot_path;

static RETSIGTYPE break64(int sig);
static void exit64(void);

/* ------------------------------------------------------------------------- */

#ifdef __MSDOS__

#include <dir.h>

static char *orig_workdir;

static void restore_workdir(void)
{
    if (orig_workdir)
	chdir(orig_workdir);
}

static void preserve_workdir(void)
{
    orig_workdir = getcwd(NULL, GET_PATH_MAX);
    atexit(restore_workdir);
}

/* Warning!  This must be called *once*.  */
static void set_boot_path(const char *prg_path)
{
    char drive[MAXDRIVE], path[MAXDIR], file[MAXFILE], ext[MAXEXT];
    PATH_VAR(path);

    fnsplit(prg_path, drive, path, file, ext);
    if (*drive == '\0' || *path == '\0')
	strcpy(path, orig_workdir);
    else
	fnmerge(path, drive, path, NULL, NULL);

    /* Remove trailing `/'.  */
    {
        int len = strlen(path);

        if (len > 0 && path[len - 1] == '/')
            path[len - 1] = '\0';
    }
    boot_path = stralloc(path);
}

#else  /* __MSDOS__ */

/* Warning!  This must be called *once*.  */
static void set_boot_path(const char *prg_path)
{
    boot_path = findpath(prg_path, getenv("PATH"), X_OK);

    /* Remove the program name.  */
    *strrchr(boot_path, '/') = '\0';
}

#endif /* __MSDOS__ */

/* ------------------------------------------------------------------------- */

int main(int argc, char **argv)
{
    ADDRESS start_addr;

    if (atexit (exit64) < 0) {
	perror ("atexit");
	return -1;
    }

#ifdef __GO32__
    /* Set the default file mode. */
    _fmode = O_BINARY;
#endif
#ifdef __MSDOS__
    /* Avoid exiting to a different directory than the one we were called
       from. */
    preserve_workdir();
#endif

    progname = strrchr(argv[0], '/');
    if (!progname)
	progname = argv[0];
    else
	progname++;

    set_boot_path(argv[0]);
    printf("boot_path = `%s'\n", boot_path);

    /* VICE boot sequence. */

    printf ("\n*** VICE Version %s ***\n", VERSION);
    printf ("Welcome to %s, the Commodore %s Emulator for the X-Window System."
	    "\n\n", progname, machdesc.machine_name);
    printf ("Copyright (c) 1993-1998\n"
	    "E. Perazzoli, T. Rantanen, A. Fachat, J. Valta, D. Sladic and "
	    "J. Sonninen.\n\n");

    /* Initialize the user interface.  UiInit() might need to handle the
       command line somehow, so we call it before parsing the options.
       (e.g. under X11, the `-display' option is handled independently). */
    if (UiInit (&argc, argv) < 0)
	exit (-1);

    /* Initialize the resources. */
    resources_set_defaults(0);

    /* Load the user's configuration file. */
    if (resources_load(NULL, machdesc.machine_name) == -1) {
	fprintf(stderr,
		"Couldn't find user's configuration file: "
		"using default settings.\n");
	/* The resource file might contain errors, and thus certain resources
	   might have been initialized anyway. */
	resources_set_defaults();
    }

    putchar('\n');

    /* Parse the command line. */
    if (parse_cmd_line(&argc, argv))
	exit(-1);

    /* Complete the GUI initialization (after loading the resources) if
       necessary. */
    if (UiInitFinish() < 0)
	exit(-1);

    putchar('\n');

#ifdef __MSDOS__
    signal(SIGINT,   SIG_IGN);
#else
    signal(SIGINT,   break64);
#endif

    signal(SIGSEGV,  break64);
    signal(SIGHUP,   break64);
    signal(SIGQUIT,  break64);
    signal(SIGILL,   break64);
    signal(SIGTERM,  break64);
    signal(SIGPIPE,  break64);

#ifdef HAS_JOYSTICK
    /* Initialize real joystick. */
    joyini();
#endif

#ifndef MITSHM
    app_resources.mitshm = 0;
#endif

#ifndef __MSDOS__
    printf ("Initializing graphics (MITSHM %s)...\n\n",
	    app_resources.mitshm ? "enabled" : "disabled");
#else
    puts ("Initializing graphics...\n");
#endif

    if (video_init() < 0)
	exit (-1);

    /* Machine-specific initialization. */
    if (machine_init() < 0) {
        fprintf(stderr, "Machine initialization failed.\n");
        exit(1);
    }

    /* Attach specified disk images. */

    if (app_resources.floppyName
	&& serial_select_file(DT_DISK | DT_1541, 8,
			      app_resources.floppyName) < 0)
	fprintf (stderr, "\nFloppy attachment on drive #8 failed.\n");
    if (app_resources.floppy9Name
	&& serial_select_file(DT_DISK | DT_1541, 9,
			      app_resources.floppy9Name) < 0)
	fprintf (stderr, "\nFloppy attachment on drive #9 failed.\n");
    if (app_resources.floppy10Name
	&& serial_select_file(DT_DISK | DT_1541, 10,
			      app_resources.floppy10Name) < 0)
	fprintf (stderr, "\nFloppy attachment on drive #10 failed.\n");

    if (app_resources.tapeName && *app_resources.tapeName)
	if (serial_select_file(DT_TAPE, 1, app_resources.tapeName) < 0) {
	    fprintf (stderr, "No Tape.\n");
	}

#ifdef SOUND
#ifdef __MSDOS__
    if (app_resources.doSoundSetup) {
	vmidas_startup();
	vmidas_config();
    }
#endif

    /* Fire up the sound emulation. */
    initialize_sound();
#endif

    putchar ('\n');

    /* Use the specified start address for booting up. */
    if (app_resources.startAddr)
	start_addr = (ADDRESS) strtol(app_resources.startAddr, NULL,
				      app_resources.hexFlag ? 16 : 10);
    else
        start_addr = 0;		/* Use normal RESET vector. */

    if (app_resources.asmFlag)
	mon(start_addr);

    vsync_init();
    kbd_init();

    maincpu_trigger_reset();

    mainloop(start_addr);
    printf("perkele!\n");
    exit(0);   /* never reached */
}

static RETSIGTYPE break64(int sig)
{
#ifdef SYS_SIGLIST_DECLARED
    fprintf(stderr, "Received signal %d (%s).\n", sig, sys_siglist[sig]);
#else
    fprintf(stderr, "Received signal %d.\n", sig);
#endif

    exit (-1);
}

static void exit64(void)
{
    /* Disable SIGINT.  This is done to prevent the user from keeping C-C
       pressed and thus breaking the cleanup process, which might be
       dangerous. */
    signal(SIGINT, SIG_IGN);

    printf("\nExiting...\n");

    machine_shutdown();
    video_free();
    close_sound();

#ifdef HAS_JOYSTICK
    joyclose();
#endif

    putchar ('\n');
}

void end64()
{
    exit (-1);
}

