/*
 * resources.c - Resource handling for VICE.
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

/* This implements simple facilities to handle the resources and command-line
   options.  All the resources for the emulators can be stored in a single
   file, and they are separated by an `emulator identifier', i.e. the machine
   name between brackets (e.g. ``[C64]'').  All the resources are stored in the
   form ``ResourceName=ResourceValue'', and separated by newline characters.
   Leading and trailing spaces are removed from the ResourceValue unless it is
   put between quotes (").*/

#include "vice.h"

#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#ifdef __hpux
#define _INCLUDE_XOPEN_SOURCE
#define _XPG2
#include <limits.h>
#undef  _INCLUDE_XOPEN_SOURCE
#undef  _XPG2
#else
#include <limits.h>
#endif

#ifdef __MSDOS__
#include <io.h>
#include <dir.h>		/* fnmerge(), fnsplit() */
/* This is defined in `dir.h' and conflicts with our definition in
   `types.h'. */
#undef DRIVE
#endif /* __MSDOS__ */

#include "resources.h"
#include "video.h"
#include "ui.h"
#include "sid.h"
/* #include "mem.h" */
#include "utils.h"

#ifdef PET
#include "pet/pets.h"
#endif

#ifdef HAS_JOYSTICK
#include "joystick.h"
#endif

#ifdef HAVE_TRUE1541
#include "true1541.h"
#endif

#ifdef REU
#include "reu.h"
#endif

extern void video_resize(void);

/* Stringizer. */
#define _STR(x) #x
#define STR(X) _STR(X)

AppResources app_resources;

enum resource_type { RES_INTEGER, RES_STRING };

struct resource {
    const char *name;		/* resource name */
    void *ptr;			/* pointer to the resource value */
    enum resource_type type;	/* type of the resource */
    void (*change_func)(void);	/* function to call when the value is
				   changed */
};

/* This is the list of resources which can be loaded/saved.  Notice that not
   all of the resources are here. */
static struct resource resources[] = {
    { "Directory", &app_resources.directory, RES_STRING, NULL },
    { "ProjectDir", &app_resources.projectDir, RES_STRING, NULL },
#ifndef PET
    { "KernalName", &app_resources.kernalName, RES_STRING, NULL },
#endif
    /*{ "ExromName", &app_resources.exromName, RES_STRING, NULL },*/
    /*{ "ProgramName", &app_resources.programName, RES_STRING, NULL },*/
    { "RamName", &app_resources.ramName, RES_STRING, NULL },
    { "HexFlag", &app_resources.hexFlag, RES_INTEGER, NULL },
    { "VideoCache", &app_resources.videoCache, RES_INTEGER, NULL },
    { "HTMLBrowserCommand", &app_resources.htmlBrowserCommand, RES_STRING,
      NULL },
    { "SaveResourcesOnExit", &app_resources.saveResourcesOnExit, RES_INTEGER,
      NULL },
    { "NoTraps", &app_resources.noTraps, RES_INTEGER, NULL },
    { "Speed", &app_resources.speed, RES_INTEGER, NULL },
    { "RefreshRate", &app_resources.refreshRate, RES_INTEGER, NULL },

/* With the current setup, having these in the configuration file can be
   confusing. */
#if 0
    { "FloppyName", &app_resources.floppyName, RES_STRING, NULL },
    { "Floppy9Name", &app_resources.floppy9Name, RES_STRING, NULL },
    { "Floppy10Name", &app_resources.floppy10Name, RES_STRING, NULL },
#endif

/* Machine-specific resources. */
#ifndef PET
    /* The PET uses one single ROM file with editor, basic and kernal. */
    { "BasicName", &app_resources.basicName, RES_STRING, NULL },
#else
    /* PetModel must be first because the latter override some stuff set here */
    { "PetModel", &app_resources.petModel, RES_STRING,
      pet_set_model },
    { "KernalName", &app_resources.kernalName, RES_STRING,
      NULL },
    { "VideoWidth", &app_resources.videoWidth, RES_INTEGER,
      pet_set_video_width },
    { "KeyboardType", &app_resources.keyboardType, RES_STRING,
      pet_set_keyboard_type },
    { "PetRom9", &app_resources.petrom9Name, RES_STRING, NULL },
    { "PetRomA", &app_resources.petromAName, RES_STRING, NULL },
    { "PetRomB", &app_resources.petromBName, RES_STRING, NULL },
    { "PetRomBasic", &app_resources.petromBasic, RES_STRING, NULL },
    { "PetRomEditor", &app_resources.petromEditor, RES_STRING, NULL },
    { "PetRam9", &app_resources.petram9, RES_INTEGER, NULL },
    { "PetRamA", &app_resources.petramA, RES_INTEGER, NULL },
    { "PetDiag", &app_resources.petdiag, RES_INTEGER, NULL },
    { "NumpadJoystick", &app_resources.numpadJoystick, RES_INTEGER, NULL },
#endif
#ifdef VIC20
    { "MemoryExp", &app_resources.memoryExp, RES_STRING, NULL },
#endif
#ifdef CBM64
    { "EmuID", &app_resources.emuID, RES_INTEGER, NULL },
#endif
#ifdef HAVE_TRUE1541
    { "True1541", &app_resources.true1541, RES_INTEGER, true1541_ack_switch },
#ifdef CBM64
    { "True1541ParallelCable", &app_resources.true1541ParallelCable, RES_INTEGER, NULL },
#endif
    { "True1541ExtendImage", &app_resources.true1541ExtendImage,
      RES_INTEGER, NULL },
    { "True1541IdleMethod", &app_resources.true1541IdleMethod, RES_INTEGER,
      NULL },
    { "True1541SyncFactor", &app_resources.true1541SyncFactor, RES_INTEGER,
      true1541_ack_sync_factor },
    { "BasicRev", &app_resources.basicRev, RES_STRING, NULL },
    { "KernalRev", &app_resources.kernalRev, RES_STRING, NULL },
    { "Module", &app_resources.module, RES_STRING, NULL },
    { "CheckSsColl", &app_resources.checkSsColl, RES_INTEGER, NULL },
    { "CheckSbColl", &app_resources.checkSbColl, RES_INTEGER, NULL },
    { "DosName", &app_resources.dosName, RES_STRING, NULL },
    { "JoyPort", &app_resources.joyPort, RES_INTEGER, NULL },
#endif
#ifdef C128
    { "BiosName", &app_resources.biosName, RES_STRING, NULL },
#endif

/* Special expansions. */
#ifdef IEEE488
    { "IEEE488", &app_resources.ieee488, RES_INTEGER, NULL },
#endif
#ifdef REU
    { "REU", &app_resources.reu, RES_INTEGER, activate_reu },
    { "REUName", &app_resources.reuName, RES_STRING, NULL },
#if 0
    { "REUSize", &app_resources.reusize, RES_STRING, NULL },
#endif
#endif

/* OS-specific resurces. */
#ifndef __MSDOS__
    { "PrivateColormap", &app_resources.privateColormap, RES_INTEGER, NULL },
    { "Mitshm", &app_resources.mitshm, RES_INTEGER, NULL },
    { "UseXSync", &app_resources.useXSync, RES_INTEGER, NULL },
#if defined(HAS_JOYSTICK) /* && !defined(PET) */
    { "JoyDevice1", &app_resources.joyDevice1, RES_INTEGER, joyset },
    { "JoyDevice2", &app_resources.joyDevice2, RES_INTEGER, joyset },
#endif
#endif
#ifdef SOUND
    { "Sound", &app_resources.sound, RES_INTEGER, close_sound },
    { "SoundSampleRate", &app_resources.soundSampleRate, RES_INTEGER, close_sound },
    { "SoundDeviceName", &app_resources.soundDeviceName, RES_STRING, close_sound },
    { "SoundDeviceArg", &app_resources.soundDeviceArg, RES_STRING, close_sound },
    { "SoundBufferSize", &app_resources.soundBufferSize, RES_INTEGER, close_sound },
#ifndef __MSDOS__
    { "SoundSuspendTime", &app_resources.soundSuspendTime, RES_INTEGER, close_sound },
#endif /* __MSDOS__ */
    { "SoundSpeedAdjustment", &app_resources.soundSpeedAdjustment, RES_INTEGER, close_sound },
#if defined(CBM64) || defined(C128)
    { "SIDFilters", &app_resources.sidFilters, RES_INTEGER, close_sound },
    { "SIDModel", &app_resources.sidModel, RES_INTEGER, close_sound },
#endif /* CBM64 || C128 */
#endif /* SOUND */
#if defined(__MSDOS__) && !defined(PET)
    /* On the PET, the VGA mode is hardcoded. */
    { "VGAMode", &app_resources.vgaMode, RES_INTEGER, video_ack_vga_mode },
#endif
#ifndef __MSDOS__
    { "DoubleScan", &app_resources.doubleScan, RES_INTEGER, video_resize },
    { "DoubleSize", &app_resources.doubleSize, RES_INTEGER, video_resize },
#endif
    { NULL }
};

enum arg_type { ARG_REQUIRED, ARG_NONE };

struct cmd_line_option {
    const char *id;
    const char *desc;
    void *res_ptr;
    enum resource_type res_type;
    enum arg_type arg_type;
    char *value;
    void (*change_func)(void);	/* Function to call when the value is
                                   changed.  */
};

struct cmd_line_option options[] = {
    { "-directory",	"Specify system directory",
      &app_resources.directory, RES_STRING, ARG_REQUIRED, NULL, NULL },
    { "-8",		"Specify a X64/D64 disk image file for unit 8",
      &app_resources.floppyName, RES_STRING, ARG_REQUIRED, NULL, NULL },
    { "-9",		"Specify a X64/D64 disk image file for unit 9",
      &app_resources.floppy9Name, RES_STRING, ARG_REQUIRED, NULL, NULL },
    { "-10",		"Specify a X64/D64 disk image file for unit 10",
      &app_resources.floppy10Name, RES_STRING, ARG_REQUIRED, NULL, NULL },
    { "-hex",		"Enable hex mode in monitor",
      &app_resources.hexFlag, RES_INTEGER, ARG_NONE, "1", NULL },
    { "+hex",		"Disable hex mode in monitor",
      &app_resources.hexFlag, RES_INTEGER, ARG_NONE, "0", NULL },
    { "-kernal",	"Specify Kernal ROM image name",
      &app_resources.kernalName, RES_STRING, ARG_REQUIRED, NULL, NULL },
    { "-ram",		"Specify RAM image for loading at startup",
      &app_resources.ramName, RES_STRING, ARG_REQUIRED, NULL, NULL },
    { "-traps",		"Enable serial traps",
      &app_resources.noTraps, RES_INTEGER, ARG_NONE, "0", NULL },
    { "+traps",		"Disable serial traps",
      &app_resources.noTraps, RES_INTEGER, ARG_NONE, "1", NULL },
    { "-verbose",	"Enable verbose mode",
      &app_resources.verboseFlag, RES_INTEGER, ARG_NONE, "1", NULL },
    { "-vcache",	"Enable the video cache",
      &app_resources.videoCache, RES_INTEGER, ARG_NONE, "1", NULL },
    { "+vcache",	"Disable the video cache",
      &app_resources.videoCache, RES_INTEGER, ARG_NONE, "0", NULL },
    { "-htmlbrowser",   "Specify an HTML browser for the on-line help",
      &app_resources.htmlBrowserCommand, RES_STRING, ARG_REQUIRED, NULL, NULL },
    { "-speed",		"Specify maximum emulation speed (0 for no limit)",
      &app_resources.speed, RES_INTEGER, ARG_REQUIRED, NULL, NULL },
    { "-refresh",	"Repaint each <value> frames (0 for automatic setting)",
      &app_resources.refreshRate, RES_INTEGER, ARG_REQUIRED, NULL, NULL },

#if 0				/* not supported */
    { "-load",		"Load specified program into RAM",
      &app_resources.programName, RES_STRING, ARG_REQUIRED, NULL, NULL },
    { "-start",		"Specify the start address",
      &app_resources.startAddr, RES_STRING, ARG_REQUIRED, NULL, NULL },
#endif

/* Special expansions. */
#ifdef REU
    { "-reu",		"Enable the 512RAM Expansion Unit",
      &app_resources.reu, RES_INTEGER, ARG_NONE, "1", NULL },
    { "+reu",		"Disable the 512RAM Expansion Unit",
      &app_resources.reu, RES_INTEGER, ARG_NONE, "0", NULL },
    { "-reuimage",	"Specify a RAM image file for the REU unit",
      &app_resources.reuName, RES_STRING, ARG_REQUIRED, NULL, NULL },
#endif
#ifdef IEEE488
    { "-ieee488",	"Emulate the IEEE488 optional interface",
      &app_resources.ieee488, RES_INTEGER, ARG_NONE, "1", NULL },
    { "+ieee488",	"Disable the IEEE488 optional interface",
      &app_resources.ieee488, RES_INTEGER, ARG_NONE, "0", NULL },
#endif

/* Super kludge to start programs */
#ifdef AUTOSTART
    { "-autostart",	"Autostart this floppy or disk image",
      &app_resources.autostartName, RES_STRING, ARG_REQUIRED, NULL, NULL },
#endif

/* Machine-specific options. */
#if defined(CBM64) || defined(VIC20)
    { "-basic",		"Specify the BASIC ROM image file",
      &app_resources.basicName, RES_STRING, ARG_REQUIRED, NULL, NULL },
#if 0
    { "-exrom",		"Specify the external ROM image file",
      &app_resources.exromName, RES_STRING, ARG_REQUIRED, NULL, NULL },
#endif
#endif
#ifdef CBM64
    { "-tape",		"Specify a T64 tape image",
      &app_resources.tapeName, RES_STRING, ARG_REQUIRED, NULL, NULL },
    { "-1",		"Specify a T64 tape image",
      &app_resources.tapeName, RES_STRING, ARG_REQUIRED, NULL, NULL },
    { "-emuid",		"Enable emulator identification" ,
      &app_resources.emuID, RES_INTEGER, ARG_NONE, "1", NULL },
    { "+emuid",		"Disable emulator identification",
      &app_resources.emuID, RES_INTEGER, ARG_NONE, "0", NULL },
#endif
#ifdef PET
    { "-model",		"PET model number (like '3032' or '8032')",
      &app_resources.petModel, RES_STRING, ARG_REQUIRED, NULL, pet_set_model },
    { "-videowidth",		"PET screen width (40 or 80)",
      &app_resources.videoWidth, RES_INTEGER, ARG_REQUIRED, NULL, pet_set_video_width },
    { "-keyboardtype",	"PET keyboard ('business' or 'graphics')",
      &app_resources.keyboardType, RES_STRING, ARG_REQUIRED, NULL, pet_set_keyboard_type },
    { "-petrom9",	"PET extension ROM $9*** (4k)",
      &app_resources.petrom9Name, RES_STRING, ARG_REQUIRED, NULL, NULL },
    { "-petromA",	"PET extension ROM $A*** (4k)",
      &app_resources.petromAName, RES_STRING, ARG_REQUIRED, NULL, NULL },
    { "-petromB",	"PET extension ROM $B*** (4k)",
      &app_resources.petromBName, RES_STRING, ARG_REQUIRED, NULL, NULL },
    { "-petromBasic",	"PET Basic ROM replacement $B000 (8-12k)",
      &app_resources.petromBasic, RES_STRING, ARG_REQUIRED, NULL, NULL },
    { "-petromEditor",	"PET Editor ROM replacement $e800, 2k",
      &app_resources.petromEditor, RES_STRING, ARG_REQUIRED, NULL, NULL },
    { "-petram9",	"PET 8296 RAM mapping for $9*** (4k)",
      &app_resources.petram9, RES_INTEGER, ARG_NONE, "1", NULL },
    { "+petram9",	"PET 8296 ROM mapping for $9*** (4k)",
      &app_resources.petram9, RES_INTEGER, ARG_NONE, "0", NULL },
    { "-petramA",	"PET 8296 RAM mapping for $A*** (4k)",
      &app_resources.petramA, RES_INTEGER, ARG_NONE, "1", NULL },
    { "+petramA",	"PET 8296 ROM mapping for $A*** (4k)",
      &app_resources.petramA, RES_INTEGER, ARG_NONE, "0", NULL },
    { "-petdiag",	"set PET userport diagnostic pin (PIA1 PA7)",
      &app_resources.petdiag, RES_INTEGER, ARG_NONE, "1", NULL },
    { "+petdiag",	"clear PET userport diagnostic pin (PIA1 PA7)",
      &app_resources.petdiag, RES_INTEGER, ARG_NONE, "0", NULL },
    { "-npjoystick",	"enable Numpad joystick emulation",
      &app_resources.numpadJoystick, RES_INTEGER, ARG_NONE, "1", NULL },
    { "+npjoystick",	"disable Numpad joystick emulation",
      &app_resources.numpadJoystick, RES_INTEGER, ARG_NONE, "0", NULL },
#endif
#if defined(CBM64) || defined(C128)
    { "-checksb",	"Enable sprite-background collision registers",
      &app_resources.checkSbColl, RES_INTEGER, ARG_NONE, "1", NULL },
    { "+checksb",	"Disable sprite-background collision registers",
      &app_resources.checkSbColl, RES_INTEGER, ARG_NONE, "0", NULL },
    { "-checkss",	"Enable sprite-sprite collision registers",
      &app_resources.checkSsColl, RES_INTEGER, ARG_NONE, "1", NULL },
    { "+checkss",	"Disable sprite-sprite collision registers",
      &app_resources.checkSsColl, RES_INTEGER, ARG_NONE, "0", NULL },
#ifdef PATCH_ROM
    { "-kernalrev",	"Patch the ROM to a different revision (0, 1, 67 or 100)",
      &app_resources.kernalRev, RES_STRING, ARG_REQUIRED, NULL, NULL },
#endif
#if defined(HAS_JOYSTICK) /* && !defined(PET) */
    { "-joydev1",       "Specify joystick device for port 1",
      &app_resources.joyDevice1, RES_INTEGER, ARG_REQUIRED, NULL, NULL },
    { "-joydev2",       "Specify joystick device for port 2",
      &app_resources.joyDevice2, RES_INTEGER, ARG_REQUIRED, NULL, NULL },
#else
    { "-joyport",	"Specify numpad joystick port",
      &app_resources.joyPort, RES_INTEGER, ARG_REQUIRED, NULL, NULL },
#endif
#endif
#ifdef HAVE_TRUE1541
    { "-1541",		"Enable hardware 1541 emulation",
      &app_resources.true1541, RES_INTEGER, ARG_NONE, "1", NULL },
    { "+1541",		"Disable hardware 1541 emulation",
      &app_resources.true1541, RES_INTEGER, ARG_NONE, "0", NULL },
    { "-driveidle",     "Specify a 1541 idling method (0 or 1)",
      &app_resources.true1541IdleMethod, RES_INTEGER, ARG_REQUIRED, NULL, NULL },
    { "-drivesync",	"Specify a custom 1541 sync factor",
      &app_resources.true1541SyncFactor, RES_INTEGER, ARG_REQUIRED, NULL, NULL },
    { "-paldrive",	"Use PAL 1541 sync factor",
      &app_resources.true1541SyncFactor, RES_INTEGER, ARG_NONE, STR(TRUE1541_PAL_SYNC_FACTOR), NULL },
    { "-ntscdrive",	"Use NTSC 1541 sync factor",
      &app_resources.true1541SyncFactor, RES_INTEGER, ARG_NONE, STR(TRUE1541_NTSC_SYNC_FACTOR), NULL },
    { "-dosname",	"Specify a 1541 ROM image",
      &app_resources.dosName, RES_STRING, ARG_REQUIRED, NULL, NULL },
#ifdef CBM64
    { "-parallel",	"Emulate a SpeedDOS-compatible parallel cable",
      &app_resources.true1541ParallelCable, RES_INTEGER, ARG_NONE, "1", NULL },
    { "+parallel",	"Disable the SpeedDOS-compatible parallel cable",
      &app_resources.true1541ParallelCable, RES_INTEGER, ARG_NONE, "0", NULL },
#endif
#endif /* HAVE_TRUE1541 */
#ifdef VIC20
    { "-memory",	"Specify a memory expansion unit",
      &app_resources.memoryExp, RES_STRING, ARG_REQUIRED, NULL, NULL },
#endif

/* OS-specific options. */
#ifdef __MSDOS__
#ifndef PET
    /* On the PET, the VGA mode is hardcoded. */
    { "-vgamode",	"Select VGA mode",
      &app_resources.vgaMode, RES_INTEGER, ARG_REQUIRED, NULL, NULL },
#endif
    { "-dosoundsetup",  "Run MIDAS sound setup before starting",
      &app_resources.doSoundSetup, RES_INTEGER, ARG_NONE, "1", NULL },
#else /* !__MSDOS__ */
    { "-dsize",		"Enable double size",
      &app_resources.doubleSize, RES_INTEGER, ARG_NONE, "1", NULL },
    { "+dsize",		"Disable double size",
      &app_resources.doubleSize, RES_INTEGER, ARG_NONE, "0", NULL },
    { "-install",	"Install a private color map",
      &app_resources.privateColormap, RES_INTEGER, ARG_NONE, "1", NULL },
    { "+install",	"Use the standard color map",
      &app_resources.privateColormap, RES_INTEGER, ARG_NONE, "0", NULL },
    { "-xsync",		"Enable usage of the XSync() call when blitting",
      &app_resources.useXSync, RES_INTEGER, ARG_NONE, "1", NULL },
    { "+xsync",		"Disable usage of the XSync() call when blitting",
      &app_resources.useXSync, RES_INTEGER, ARG_NONE, "0", NULL },
#ifdef MITSHM
    { "-mitshm",        "Enable MITSHM",
      &app_resources.mitshm, RES_INTEGER, ARG_NONE, "1", NULL },
    { "+mitshm",        "Disable MITSHM",
      &app_resources.mitshm, RES_INTEGER, ARG_NONE, "0", NULL },
#endif /* MITSHM */
#endif /* !__MSDOS__ */
#if !defined(__MSDOS__) || defined(PET)
    { "-dscan",		"Enable double scan",
      &app_resources.doubleScan, RES_INTEGER, ARG_NONE, "1", NULL },
    { "+dscan",		"Disable double scan",
      &app_resources.doubleScan, RES_INTEGER, ARG_NONE, "0", NULL },
#endif
#ifdef SOUND
    { "-sound",         "Enable sound",
      &app_resources.sound, RES_INTEGER, ARG_NONE, "1", NULL },
    { "+sound",         "Disable sound",
      &app_resources.sound, RES_INTEGER, ARG_NONE, "0", NULL },
    { "-soundspeedadj", "Enable automatic sound speed adjustment",
      &app_resources.soundSpeedAdjustment, RES_INTEGER, ARG_NONE, "1", NULL },
    { "+soundspeedadj", "Disable automatic sound speed adjustment",
      &app_resources.soundSpeedAdjustment, RES_INTEGER, ARG_NONE, "0", NULL },
    { "-soundrate",     "Sound sample rate",
      &app_resources.soundSampleRate, RES_INTEGER, ARG_REQUIRED, NULL, NULL },
    { "-soundbufsize",  "Sound kernal buffer size (milliseconds)",
      &app_resources.soundBufferSize, RES_INTEGER, ARG_REQUIRED, NULL, NULL },
#if defined(CBM64) || defined(C128)
    { "-sidmodel",	"Select the SID model (1: 8580, 0: 6581)",
      &app_resources.sidModel, RES_INTEGER, ARG_REQUIRED, NULL, NULL },
    { "-sidfilters",	"Emulate the SID filters",
      &app_resources.sidFilters, RES_INTEGER, ARG_NONE, "1", NULL },
    { "+sidfilters",	"Disable the SID filters",
      &app_resources.sidFilters, RES_INTEGER, ARG_NONE, "0", NULL },
#endif
#ifndef __MSDOS__
    { "-soundsuspend",  "Sound suspend time on errors (seconds)",
      &app_resources.soundSuspendTime, RES_INTEGER, ARG_REQUIRED, NULL, NULL },
#endif
    { "-sounddev",      "Sound device name",
      &app_resources.soundDeviceName, RES_STRING, ARG_REQUIRED, NULL, NULL },
    { "-soundarg",      "Sound device initialization parameter",
      &app_resources.soundDeviceArg, RES_STRING, ARG_REQUIRED, NULL, NULL },
#endif
#if X_DISPLAY_DEPTH == 0
    { "-displaydepth",	"X Display Depth (1 - 32)",
      &app_resources.displayDepth, RES_INTEGER, ARG_REQUIRED, NULL, NULL },
#endif

    { "-saveres",	"Save settings on exit",
      &app_resources.saveResourcesOnExit, RES_INTEGER, ARG_NONE, "1", NULL },
    { "+saveres",	"Don't save settings on exit",
      &app_resources.saveResourcesOnExit, RES_INTEGER, ARG_NONE, "0", NULL },
    { NULL }
};

#ifdef __MSDOS__
#define RESOURCE_FILE_NAME	"VICERC"
#else
#define RESOURCE_FILE_NAME	".vicerc"
#endif

/* -------------------------------------------------------------------------- */

/* Set sensible defaults for all the resources. */
void resources_set_defaults(void)
{
    int i;

#ifdef __MSDOS__
    /* On MS-DOS, always boot from the directory in which the binary is
       stored. */
    {
	char *dir = concat(boot_path, EMULATOR, NULL);

	resources_set_string(&app_resources.directory, dir);
	free(dir);
    }
#else
    /* Force autodetection of boot directory. */
    resources_set_string(&app_resources.directory, NULL);
#endif

    resources_set_string(&app_resources.projectDir, PROJECTDIR);
    resources_set_string(&app_resources.basicName, BASICNAME);
    resources_set_string(&app_resources.ramName, RAMNAME);
    resources_set_string(&app_resources.kernalName, KERNALNAME);
    resources_set_string(&app_resources.dosName, DOSNAME);
    resources_set_string(&app_resources.biosName, BIOSNAME);
    resources_set_string(&app_resources.exromName, EXROMNAME);
    resources_set_string(&app_resources.charName, CHARNAME);
    resources_set_string(&app_resources.module, NULL);
    resources_set_string(&app_resources.kernalRev, NULL);
    resources_set_string(&app_resources.basicRev, NULL);

#ifdef PET
    resources_set_string(&app_resources.petModel, "8032");
    app_resources.petram9 = 0;
    app_resources.petramA = 0;
    app_resources.petdiag = 0;
    app_resources.numpadJoystick = 0;
#endif
#ifdef __MSDOS__
    /* On MS-DOS, always save the REU contents in the directory in which the
       executable is. */
    {
	char *s = concat(boot_path, REUNAME, NULL);

	resources_set_string(&app_resources.reuName, s);
	free(s);
    }
#else
    resources_set_string(&app_resources.reuName, REUNAME);
#endif

    app_resources.colors = 1;
    app_resources.hexFlag = 1;
    app_resources.asmFlag = 0;
    app_resources.debugFlag = 0;
    app_resources.verboseFlag = 0;
    app_resources.haltFlag = 0;
    app_resources.noTraps = 0;
    app_resources.privateColormap = 0;

    resources_set_string(&app_resources.programName, NULL);
    resources_set_string(&app_resources.startAddr, NULL);
    resources_set_string(&app_resources.tapeName, NULL);
    resources_set_string(&app_resources.floppyName, NULL);
    resources_set_string(&app_resources.floppy9Name, NULL);
    resources_set_string(&app_resources.floppy10Name, NULL);
    resources_set_string(&app_resources.printCommand, NULL);
    resources_set_string(&app_resources.htmlBrowserCommand, "netscape %s");

#ifdef AUTOSTART
    resources_set_string(&app_resources.autostartName, NULL);
#endif

    app_resources.emuID = 1;

    app_resources.joyPort = 2;
    app_resources.joyDevice1 = 0;
    app_resources.joyDevice2 = 0;

    app_resources.checkSsColl = app_resources.checkSbColl = 1;

    app_resources.video40 = app_resources.video80 = 0;

    app_resources.speed = 100;
    app_resources.refreshRate = 0;

#ifdef MITSHM
    app_resources.mitshm = 1;
#endif

    app_resources.doubleSize = app_resources.doubleScan = 0;

#if !defined(__MSDOS__) || defined(PET)
    /*
     * I __really__ think this should be a default for all. With videocache on
     * you get a lot better first impression. When you know how to run
     * programs that are (marginally) slower with the videocache on, you also
     * know how to disable the videocache.
     *                                                     //tvr
     */
    app_resources.videoCache = 1;
#else  /* __MSDOS__ */
    app_resources.videoCache = 0;

    /* Set the default VGA mode.  These are not the optimal values, but are
       the most compatible ones. */
#if defined(CBM64)
    app_resources.vgaMode = VGA_320x200;
#elif defined(VIC20)
    app_resources.vgaMode = VGA_360x240;
#else
    app_resources.vgaMode = VGA_640x480;
#endif
#endif

#ifdef SOUND
    app_resources.sound = 0;
    app_resources.soundSampleRate = SOUND_SAMPLE_RATE;
    resources_set_string(&app_resources.soundDeviceArg, NULL);
    resources_set_string(&app_resources.soundDeviceName, NULL);
    app_resources.soundBufferSize = SOUND_SAMPLE_BUFFER_SIZE;
#if defined(CBM64) || defined(C128)
    app_resources.sidModel = 0;	/* Old SID */
    app_resources.sidFilters = 0;
#endif
#ifdef __MSDOS__
    app_resources.soundSuspendTime = 0;
    app_resources.soundSpeedAdjustment = 0;
#else
    app_resources.soundSuspendTime = 5;
    app_resources.soundSpeedAdjustment = 1;
#endif
    close_sound();
#endif

#ifdef HAVE_TRUE1541
    app_resources.true1541 = 0;
    app_resources.true1541ExtendImage = TRUE1541_EXTEND_NEVER;
    app_resources.true1541IdleMethod = TRUE1541_IDLE_TRAP_IDLE;
    app_resources.true1541SyncFactor = TRUE1541_PAL_SYNC_FACTOR;
#ifdef CBM64
    app_resources.true1541ParallelCable = 0;
#endif
#endif

#ifdef IEEE488
    app_resources.ieee488 = 0;
#endif

#ifdef REU
    app_resources.reu = 0;
#endif

#ifdef VIC20
    resources_set_string(&app_resources.memoryExp, "all");
#endif

    app_resources.displayDepth = 0;
    app_resources.useXSync = 1;

    app_resources.saveResourcesOnExit = 0;

    /* Call all the associated change_funcs() so that the values are
       acknowledged. */
    for (i = 0; resources[i].name != NULL; i++) {
	if (resources[i].change_func != NULL)
	    (*resources[i].change_func)();
    }

#ifndef __MSDOS__
    /* Try to load the global configuration file if present. */
    resources_load(LIBDIR "/" RESOURCE_FILE_NAME, EMULATOR);
#endif

#ifdef __MSDOS__
    app_resources.doSoundSetup = 0;
#endif

    UiUpdateMenus();
}

/* -------------------------------------------------------------------------- */

/* Check for wrong resource values, adjusting them if necessary. */
int resources_fix(void)
{
    int err = 0;

#if defined(CBM64) || defined(C128)
    if (app_resources.joyPort > 2) {
	fprintf(stderr, "Invalid JoyPort value %d: forcing 2.\n",
		app_resources.joyPort);
	app_resources.joyPort = 2;
	err = 1;
    } else if (app_resources.joyPort < 1) {
	fprintf(stderr, "Invalid JoyPort value %d: forcing 1.\n",
		app_resources.joyPort);
	app_resources.joyPort = 1;
	err = 1;
    }
#endif

#ifndef __MSDOS__
    if (app_resources.speed == 0 && app_resources.refreshRate == 0) {
	fprintf(stderr, "Invalid Speed/RefreshRate values: "
		"forcing 100%% with automatic refresh rate adjustment.\n");
	app_resources.speed = 100;
	err = 1;
    }
#endif

#ifdef __MSDOS__
    if (app_resources.vgaMode >= NUM_VGA_MODES) {
	fprintf(stderr, "Invalid VGA mode %d: forcing %d\n",
		app_resources.vgaMode, VGA_640x480);
	app_resources.vgaMode = VGA_640x480;
	err = 1;
    }
#endif

#if defined(REU) && defined(IEEE488)
    if (app_resources.reu && app_resources.ieee488) {
	fprintf(stderr, "Cannot install both REU and IEEE488: disabling IEEE488.\n");
	app_resources.ieee488 = 0;
	err = 1;
    }
#endif

#if X_DISPLAY_DEPTH == 0
    if (app_resources.displayDepth < 0 ||
	app_resources.displayDepth > 32) {
	fprintf(stderr, "Invalid displaydepth %d, using Xserver default.\n",
		app_resources.displayDepth);
	app_resources.displayDepth = 0;
    }
#endif

#ifdef SOUND
    if (app_resources.soundSuspendTime < 0)
        app_resources.soundSuspendTime = 0;
#endif

    return err;
}

/* -------------------------------------------------------------------------- */

void resources_set_string(char **s, const char *value)
{
    if (value == NULL || *value == '\0') {
	if (*s != NULL)
	    free(*s);
	*s = NULL;
    } else {
	char *p;

	if (*s == NULL)
	    p = (char *)malloc(strlen(value) + 1);
	else
	    p = (char *)realloc(*s, strlen(value) + 1);

	if (p == NULL) {
	    fprintf(stderr, "Virtual memory exhausted.\n");
	    exit(-1);
	}

	strcpy(p, value);
	*s = p;
    }
}

static void set_value(void *ptr, enum arg_type type, const char *value)
{
    if (!value)
	return;

    switch (type) {
      case RES_INTEGER:
	*((int *)ptr) = atoi(value);
	break;
      case RES_STRING:
	resources_set_string((char **)ptr, value);
	break;
    }
}

int resources_set_value(const char *name, const char *value)
{
    int i;

    for (i = 0; resources[i].name != NULL; i++) {
	if (strcmp(resources[i].name, name) == 0) {
	    switch (resources[i].type) {
	      case RES_INTEGER:
		*((int *)resources[i].ptr) = atoi(value);
		break;
	      case RES_STRING:
		resources_set_string((char **)resources[i].ptr, value);
		break;
	    }
	    if (resources[i].change_func != NULL)
		(*resources[i].change_func)();
	    return 0;
	}
    }

    return -1;
}

/* -------------------------------------------------------------------------- */

static int find_match(const char *opt)
{
    int i, match;
    int exact_match;
    int opt_len = strlen(opt);

    for (i = 0, match = -1, exact_match = 0; options[i].id != NULL; i++) {
	if (strncmp(options[i].id, opt, opt_len) == 0) {
	    if (options[i].id[opt_len] == '\0') {
		exact_match = 1;
		match = i;
	    } else if (match == -1) {
		match = i;
	    } else if (!exact_match) {
		/* Ambiguous option */
		match = -2;
		break;
	    }
	}
    }
    return match;
}

int parse_cmd_line(int *argc, char **argv)
{
    int i, err;
    int have_standalone = 0;

    for (i = 1, err = 0; i < *argc && !err; i++) {
	if (strcmp(argv[i], "-help") == 0
	    || strcmp(argv[i], "-h") == 0) {
	    describe_cmd_line_options();
	    err = 1;
	} else {
	    /* Check for a "standalone" argument (no leading `-' or `+').  If
               we have it, it goes into `autostartName'.  We can have only one
               of them on the command line.  */
	    if (!have_standalone && i < *argc
		&& *argv[i] != '-' && *argv[i] != '+') {
		set_value(&app_resources.autostartName, RES_STRING, argv[i]);
		have_standalone = 1;
	    } else {
		int match = find_match(argv[i]);

		switch (match) {
		  case -1:
		    fprintf(stderr, "\nUnknown option: `%s'\n", argv[i]);
		    err = 1;
		    break;
		  case -2:
		    fprintf(stderr, "\nAmbiguous option: `%s'\n", argv[i]);
		    err = 1;
		    break;
		  default:
		    switch (options[match].arg_type) {
		      case ARG_NONE:
			set_value(options[match].res_ptr,
				  options[match].res_type,
				  options[match].value);
			if(options[match].change_func)
			    options[match].change_func();
			break;
		      case ARG_REQUIRED:
			if (i > *argc - 2) {
			    fprintf(stderr,
				    "\nOption `%s' needs a parameter\n",
				    argv[i]);
			    err = 1;
			} else {
			    set_value(options[match].res_ptr,
				      options[match].res_type,
				      argv[i + 1]);
			    if(options[match].change_func)
				options[match].change_func();
			    i++;
			}
			break;
		    }
		}
	    }
	}
    }

    if (!err) {
	resources_fix();
    } else {
	fprintf(stderr, "Use `%s -help' for a list of the available options.\n",
		progname);
    }

    return err;
}

#ifdef __MSDOS__
static void wait_user(FILE *f)
{
    if (isatty(fileno(f)) && isatty(fileno(stdin))) {
	int old_mode = setmode(fileno(f), O_TEXT);

	fprintf(f, "Press RETURN to go on...");
	fflush(f);
	while (getchar() != '\n')
	    ;
	setmode(fileno(f), old_mode);
    }
}
#endif

void describe_cmd_line_options(void)
{
#ifdef __MSDOS__
    int num_lines = num_text_lines();
#endif
    int line_count;
    int i;

    fprintf(stderr, "\nAvailable options are:\n\n");
    line_count = 0;

    for (i = 0; options[i].id != NULL; i++) {
	char buf[256];

	sprintf(buf, "%s %s", options[i].id,
		options[i].arg_type == ARG_NONE ? "" : "<value>");
	fprintf(stderr, "  %-20s %s\n", buf, options[i].desc);
	line_count++;
#ifdef __MSDOS__
	if (line_count >= num_lines - 1) {
	    wait_user(stderr);
	    line_count = 0;
	}
#endif
    }
#ifdef __MSDOS__
    if (line_count >= num_lines - 3)
	wait_user(stderr);
#endif
}

/* -------------------------------------------------------------------------- */

/* Input one line from the file descriptor `f'. */
static int get_line(char *buf, int bufsize, FILE *f)
{
    char *r;
    int len;

    r = fgets(buf, bufsize, f);
    if (r == NULL)
	return -1;

    len = strlen(buf);

    if (len > 0) {
	char *p;

	/* Remove trailing newline character. */
	if (*(buf + len - 1) == '\n')
	    len--;

	/* Remove useless spaces. */
	while (*(buf + len - 1) == ' ')
	    len--;
	for (p = buf; *p == ' '; p++, len--)
	    ;
	memmove(buf, p, len + 1);
	*(buf + len) = '\0';
    }

    return len;
}

/* Check whether `buf' is an emulator identifier (i.e. "\[.*\]") */
static int check_emu_id(const char *buf, const char *emu_id)
{
    int emu_id_len, buf_len;

    buf_len = strlen(buf);
    if (*buf != '[' || *(buf + buf_len - 1) != ']')
	return 0;

    if (emu_id == NULL)
	return 1;

    emu_id_len = strlen(emu_id);
    if (emu_id_len != buf_len - 2)
	return 0;

    if (strncmp(buf + 1, emu_id, emu_id_len) == 0)
	return 1;
    else
	return 0;
}

/* Return a malloced string containing the name of the default user-specific
   resource file.  Warning: assumes `boot_path' does not change (it should be
   always so).  */
static const char *default_resource_file(void)
{
    static char *fname = NULL;

    if (fname == NULL) {
#ifdef __MSDOS__
	/* On MS-DOS, always boot from the directory in which the binary is
	   stored. */
	fname = concat(boot_path, RESOURCE_FILE_NAME, NULL);
#else
	fname = concat(getenv("HOME"), "/", RESOURCE_FILE_NAME, NULL);
#endif
    }

    return (const char *)fname;
}

/* -------------------------------------------------------------------------- */

/* Read one resource line from the file descriptor `f'.  Return 1 on success,
   -1 on failure, 0 on EOF. */
static int read_resource_item(FILE *f)
{
    char buf[1024];
    char *arg_ptr;
    int line_len, resname_len, arg_len;

    line_len = get_line(buf, 1024, f);

    if (line_len < 0)
	return 0;

    /* Ignore empty lines. */
    if (*buf == '\0')
	return 1;

    if (check_emu_id(buf, NULL)) {
	/* End of emulator-specific section. */
	return 0;
    }

    arg_ptr = strchr(buf, '=');
    if (!arg_ptr)
	return -1;

    resname_len = arg_ptr - buf;
    arg_ptr++;
    arg_len = strlen(arg_ptr);

    /* If the value is between quotes, remove them. */
    if (*arg_ptr == '"' && *(arg_ptr + arg_len - 1) == '"') {
	*(arg_ptr + arg_len - 1) = '\0';
	arg_ptr++;
    }

    *(buf + resname_len) = '\0';

    if (resources_set_value(buf, arg_ptr) >= 0)
	return 1;
    else {
	fprintf(stderr, "Unknown resource `%s'\n", buf);
	return -1;
    }
}

/* Load the resources from file `fname'.  If `fname' is NULL, load them from
   the default resource file. */
int resources_load(const char *fname, const char *emu_string)
{
    FILE *f;
    int retval;
    int line_num;
    int err = 0;

    if (fname == NULL)
	fname = default_resource_file();

    /*printf("Reading configuration file `%s'.\n", fname);*/

#ifdef __MSDOS__
    f = fopen(fname, "rt");
#else
    f = fopen(fname, "r");
#endif

    if (f == NULL) {
	/*perror(fname);*/
	return -1;
    }

    /* Find the start of the configuration section for this emulator. */
    for (line_num = 1; ; line_num++) {
	char buf[1024];

	if (get_line(buf, 1024, f) < 0) {
	    fclose(f);
	    return -1;
	}

	if (check_emu_id(buf, emu_string)) {
	    line_num++;
	    break;
	}
    }

    do {
	retval = read_resource_item(f);
	if (retval == -1) {
	    fprintf(stderr, "%s: Invalid resource specification at line %d.\n",
		    fname, line_num);
	    err = 1;
	}
	line_num++;
    } while (retval != 0);

    fclose(f);

    resources_fix();
    UiUpdateMenus();

    return err ? -1 : 0;
}

/* -------------------------------------------------------------------------- */

#ifndef __MSDOS__

/* Return a malloced string with the name of the backup file corresponding to
   `fname'.  */
static char *make_backup_file_name(const char *fname)
{
    return concat(fname, "~", NULL);
}

#else  /* __MSDOS__ */

/* Return a malloced string with the name of the backup file corresponding to
   `fname'.  */
static char *make_backup_file_name(const char *fname)
{
    static char backup_name[MAXPATH];
    char drive[MAXDRIVE];
    char dir[MAXDIR];
    char name[MAXFILE];
    char ext[MAXEXT];

    fnsplit(fname, drive, dir, name, ext);
    fnmerge(backup_name, drive, dir, name, "BAK");

    return stralloc(backup_name);
}

#endif /* __MSDOS__ */


/* Write the resource specification for resource number `num' to file
   descriptor `f'. */
static void write_resource_item(FILE *f, int num)
{
    fprintf(f, "%s=", resources[num].name);
    switch (resources[num].type) {
      case RES_INTEGER:
	fprintf(f, "%d\n", *((int *)(resources[num].ptr)));
	break;
      case RES_STRING:
	if (*((char **)resources[num].ptr) != NULL)
	    fprintf(f, "\"%s\"", *((char **)resources[num].ptr));
	fputc('\n', f);
	break;
    }
}

/* Save all the resources into file `fname' for the emulator identified by
   `emu_string'.  If `fname' is NULL, save them in the default resource file.
   Writing the resources does not destroy the resources for the other
   emulators. */
int resources_save(const char *fname, const char *emu_string)
{
    char *backup_name;
    FILE *in_file, *out_file;
    int have_old;
    int i;

    if (fname == NULL)
	fname = default_resource_file();

    /* Make a backup copy of the existing configuration file. */
    backup_name = make_backup_file_name(fname);
    if (rename(fname, backup_name) == 0)
	have_old = 1;
    else
	have_old = 0;

    printf("Writing configuration file `%s'.\n", fname);

#ifdef __MSDOS__
    out_file = fopen(fname, "wt");
#else
    out_file = fopen(fname, "w");
#endif

    if (!out_file) {
	perror(fname);
	free (backup_name);
	return -1;
    }

    if (have_old) {
#ifdef __MSDOS__
	in_file = fopen(backup_name, "rt");
#else
	in_file = fopen(backup_name, "r");
#endif
	if (!in_file) {
	    fclose(out_file);
	    perror(backup_name);
	    free(backup_name);
	    return -1;
	}

	/* Copy the configuration for the other emulators. */
	while (1) {
	    char buf[1024];

	    if (get_line(buf, 1024, in_file) < 0)
		break;

	    if (check_emu_id(buf, emu_string))
		break;

	    fprintf(out_file, "%s\n", buf);
	}
    } else
        in_file = NULL;

    /* Write our current configuration. */
    fprintf(out_file,"[%s]\n", emu_string);
    for (i = 0; resources[i].name != NULL; i++)
	write_resource_item(out_file, i);

    if (have_old) {
	char buf[1024];

	/* Skip the old configuration for this emulator. */
	while (1) {
	    if (get_line(buf, 1024, in_file) < 0)
		break;

	    if (check_emu_id(buf, NULL)) {
		fprintf(out_file, "%s\n", buf);
		break;
	    }
	}

	if (!feof(in_file)) {
	    /* Copy the configuration for the other emulators. */
	    while (get_line(buf, 1024, in_file) >= 0)
		fprintf(out_file, "%s\n", buf);
	}
    }

    if (in_file)
	fclose(in_file);

    fclose(out_file);
    free(backup_name);
    return 0;
}
