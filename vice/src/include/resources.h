/*
 * resources.h - Resource handling for VICE.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
 *  Jarkko Sonninen (sonninen@lut.fi)
 *  Jouko Valta (jopi@stekt.oulu.fi)
 *  Teemu Rantanen (tvr@cs.hut.fi)
 *  Andre' Fachat (fachat@physik.tu-chemnitz.de)
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

#ifndef _RESOURCES_H
#define _RESOURCES_H

/* FIXME: some of these are unused. */

typedef struct _AppResources {
    char *directory;
    char *projectDir;
    char *gamepath;

    char *basicName;
    char *kernalName;
    char *charName;

    char *basicRev;
    char *kernalRev;

    char *exromName;
    char *module;
    char *reuName;
    char *reusize;

    char *biosName;		/* CP/M BIOS ROM */

    char *ramName;
    char *programName;
    char *reloc;
    char *startAddr;		/* PC */

    int colors;
    int asmFlag;
    int hexFlag;
    int debugFlag;
    int traceFlag;
    int verboseFlag;
    int haltFlag;
    int noTraps;

    char *tapeName;

    char *dosName;
    char *floppyName;
    char *floppy9Name;
    char *floppy10Name;

    char *autostartName;

    char *printCommand;
    char *PrinterLang;
    char *Locale;

    char *memoryExp;

    int joyPort;

    int joyDevice1;
    int joyDevice2;  /* added 03-25-97 BK */

    int checkSsColl;
    int checkSbColl;
    int speed;

    int videoWidth;	/* PET video mode (05feb98 AF) */
    char *keyboardType;	/* PET keyboard mode (05feb98 AF) */
    char *petModel;	/* PET model (05feb98 AF) */

    char *petrom9Name;	/* PET extension ROMs */
    char *petromAName;
    char *petromBName;
    char *petromBasic;
    char *petromEditor;

    int petram9;	/* PET 8296 RAM mapping for $9*** */
    int petramA;	/* PET 8296 RAM mapping for $A*** */
    int petdiag;	/* PET diagnostic pin (PIA1 PB7) */

    int video80;
    int video40;

    int refreshRate;

    int mitshm;

    int videoCache;
    int videoCache80;
    int doubleSize;
    int doubleScan;

    int sound;
    int soundSampleRate;

    int emuID;

    int privateColormap;

    int true1541;

    char *soundDeviceName;
    char *soundDeviceArg;
    int soundBufferSize;
    int soundSuspendTime;
    int soundSpeedAdjustment;
    int doSoundSetup;		/* MS-DOS only */

    int sidModel;		/* 0: old SID, 1: new sid */
    int sidFilters;

    int true1541SyncFactor;
    int vgaMode;

    int ieee488;
    int reu;
    int mouse;

    int true1541IdleMethod;

    int displayDepth;

    int saveResourcesOnExit;

    char *htmlBrowserCommand;

    int useXSync;
} AppResources;

extern AppResources app_resources;

extern void resources_set_defaults();
extern int resources_load(const char *fname, const char *emu_string);
extern int resources_save(const char *fname, const char *emu_string);

extern int parse_cmd_line(int *argc, char **argv);
extern void describe_cmd_line_options(void);

extern void resources_set_string(char **s, const char *value);
extern int resources_set_value(const char *name, const char *value);

#endif
