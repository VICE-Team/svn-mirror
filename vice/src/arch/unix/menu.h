/*
 * menu.h - Menu definition for the Xaw-based user interface.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
 *  Teemu Rantanen (tvr@cs.hut.fi)
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

typedef struct _MenuEntry {
    char *string;
    UiMenuCallback callback;
    UiCallbackDataPtr callback_data;
    struct _MenuEntry *sub_menu;
} MenuEntry;

MenuEntry AttachDiskImageSubmenu[] = {
    { "Unit #8...",
      (UiMenuCallback) UiAttachDisk, (UiCallbackDataPtr) 8, NULL },
    { "Unit #9...",
      (UiMenuCallback) UiAttachDisk, (UiCallbackDataPtr) 9, NULL },
    { "Unit #10...",
      (UiMenuCallback) UiAttachDisk, (UiCallbackDataPtr) 10, NULL },
    { NULL }
};

MenuEntry DetachDiskImageSubmenu[] = {
    { "Unit #8",
      (UiMenuCallback) UiDetachDisk, (UiCallbackDataPtr) 8, NULL },
    { "Unit #9",
      (UiMenuCallback) UiDetachDisk, (UiCallbackDataPtr) 9, NULL },
    { "Unit #10",
      (UiMenuCallback) UiDetachDisk, (UiCallbackDataPtr) 10, NULL },
    { "All",
      (UiMenuCallback) UiDetachDisk, (UiCallbackDataPtr) -1, NULL },
    { NULL }
};

MenuEntry ResetSubmenu[] = {
    { "Soft",
      (UiMenuCallback) UiReset, NULL, NULL },
    { "Hard",
      (UiMenuCallback) UiPowerUpReset, NULL, NULL },
    { NULL }
};

MenuEntry LeftMenu[] = {
    { "Attach a disk image",
      NULL, NULL, AttachDiskImageSubmenu },
    { "Detach disk image",
      NULL, NULL, DetachDiskImageSubmenu },
    { "--" },
#if defined(CBM64) || defined(C128)
    { "Attach a tape image...",
      (UiMenuCallback) UiAttachTape, NULL, NULL },
    { "Detach tape image",
      (UiMenuCallback) UiDetachTape, NULL, NULL },
    { "--" },
    { "Smart-attach a file...",
      (UiMenuCallback) UiSmartAttach, NULL, NULL },
    { "--" },
#endif
    { "Change working directory...",
      (UiMenuCallback) UiChangeWorkingDir, NULL, NULL },
    { "--" },
    { "Activate the Monitor",
      (UiMenuCallback) UiActivateMonitor, NULL, NULL },
    { "Activate the XDebugger",
      (UiMenuCallback) UiActivateXDebugger, NULL, NULL },
#ifdef LED_ARRAY
    { "Activate the Virtual BUS Tester",
      (UiMenuCallback) NULL, NULL, NULL },
#endif
    { "Run C1541",
      (UiMenuCallback) UiRunC1541, NULL, NULL },
    { "--" },
    { "Browse manuals",
      (UiMenuCallback) UiBrowseManual, NULL, NULL },
    { "--" },
    { "Reset",
      NULL, NULL, ResetSubmenu },
    { "--" },
    { "Exit " EMULATOR " emulator",
      (UiMenuCallback) UiExit, NULL, NULL },
    { "--" },
    { "*Pause",
      (UiMenuCallback) UiTogglePause, NULL, NULL },
    { "--" },
    { "About...",
      (UiMenuCallback) UiInfo, NULL, NULL },
    { NULL }
};

MenuEntry SetRefreshRateSubmenu[] = {
    { "*Auto",
      (UiMenuCallback) UiSetRefreshRate, (UiCallbackDataPtr) 0, NULL },
    { "*1/1",
      (UiMenuCallback) UiSetRefreshRate, (UiCallbackDataPtr) 1, NULL },
    { "*1/2",
      (UiMenuCallback) UiSetRefreshRate, (UiCallbackDataPtr) 2, NULL },
    { "*1/3",
      (UiMenuCallback) UiSetRefreshRate, (UiCallbackDataPtr) 3, NULL },
    { "*1/4",
      (UiMenuCallback) UiSetRefreshRate, (UiCallbackDataPtr) 4, NULL },
    { "*1/5",
      (UiMenuCallback) UiSetRefreshRate, (UiCallbackDataPtr) 5, NULL },
    { "*1/6",
      (UiMenuCallback) UiSetRefreshRate, (UiCallbackDataPtr) 6, NULL },
    { "*1/7",
      (UiMenuCallback) UiSetRefreshRate, (UiCallbackDataPtr) 7, NULL },
    { "*1/8",
      (UiMenuCallback) UiSetRefreshRate, (UiCallbackDataPtr) 8, NULL },
    { "*1/9",
      (UiMenuCallback) UiSetRefreshRate, (UiCallbackDataPtr) 9, NULL },
    { "*1/10",
      (UiMenuCallback) UiSetRefreshRate, (UiCallbackDataPtr) 10, NULL },
    { "--" },
    { "*Custom...",
      (UiMenuCallback) UiSetCustomRefreshRate, NULL, NULL },
    { NULL }
};

MenuEntry SetMaximumSpeedSubmenu[] = {
    { "*100%",
      (UiMenuCallback) UiSetMaximumSpeed, (UiCallbackDataPtr) 100, NULL },
    { "*50%",
      (UiMenuCallback) UiSetMaximumSpeed, (UiCallbackDataPtr) 50, NULL },
    { "*20%",
      (UiMenuCallback) UiSetMaximumSpeed, (UiCallbackDataPtr) 20, NULL },
    { "*10%",
      (UiMenuCallback) UiSetMaximumSpeed, (UiCallbackDataPtr) 10, NULL },
    { "*No limit",
      (UiMenuCallback) UiSetMaximumSpeed, (UiCallbackDataPtr) 0, NULL },
    { "--" },
    { "*Custom...",
      (UiMenuCallback) UiSetCustomMaximumSpeed, NULL, NULL },
    { NULL }
};

#if defined (CBM64) || defined (C128)

#ifndef HAS_JOYSTICK

MenuEntry SetNumpadJoystickPortSubmenu[] = {
    { NULL }
};

#else  /* HAS_JOYSTICK */

MenuEntry SetJoystickDevice1Submenu[] = {
    { "*None",
      (UiMenuCallback) UiSetJoystickDevice1, (UiCallbackDataPtr) 0, NULL },
    { "*Analog Joystick 0",
      (UiMenuCallback) UiSetJoystickDevice1, (UiCallbackDataPtr) 1, NULL },
    { "*Analog Joystick 1",
      (UiMenuCallback) UiSetJoystickDevice1, (UiCallbackDataPtr) 2, NULL },
#ifdef HAS_DIGITAL_JOYSTICK
    { "*Digital Joystick 0",
      (UiMenuCallback) UiSetJoystickDevice1, (UiCallbackDataPtr) 3, NULL },
    { "*Digital Joystick 1",
      (UiMenuCallback) UiSetJoystickDevice1, (UiCallbackDataPtr) 4, NULL },
#endif
    { "*Numpad",
      (UiMenuCallback) UiSetJoystickDevice1, (UiCallbackDataPtr) 5, NULL },
    { NULL }
};

MenuEntry SetJoystickDevice2Submenu[] = {
    { "*None",
      (UiMenuCallback) UiSetJoystickDevice2, (UiCallbackDataPtr) 0, NULL },
    { "*Analog Joystick 0",
      (UiMenuCallback) UiSetJoystickDevice2, (UiCallbackDataPtr) 1, NULL },
    { "*Analog Joystick 1",
      (UiMenuCallback) UiSetJoystickDevice2, (UiCallbackDataPtr) 2, NULL },
#ifdef HAS_DIGITAL_JOYSTICK
    { "*Digital Joystick 0",
      (UiMenuCallback) UiSetJoystickDevice2, (UiCallbackDataPtr) 3, NULL },
    { "*Digital Joystick 1",
      (UiMenuCallback) UiSetJoystickDevice2, (UiCallbackDataPtr) 4, NULL },
#endif
    { "*Numpad",
      (UiMenuCallback) UiSetJoystickDevice2, (UiCallbackDataPtr) 5, NULL },
    { NULL }
};

#endif /* HAS_JOYSTICK */

#endif /* CBM64 || C128 */

#ifdef HAVE_TRUE1541

MenuEntry Change1541SyncFactorSubmenu[] = {
    { "*PAL", (UiMenuCallback) UiSet1541SyncFactor,
      (UiCallbackDataPtr) TRUE1541_PAL_SYNC_FACTOR, NULL },
    { "*NTSC", (UiMenuCallback) UiSet1541SyncFactor,
      (UiCallbackDataPtr) TRUE1541_NTSC_SYNC_FACTOR, NULL },
    { "*Custom...", (UiMenuCallback) UiSetCustom1541SyncFactor,
      NULL, NULL },
    { NULL }
};

MenuEntry Change1541IdleMethodSubmenu[] = {
    { "*Skip cycles", (UiMenuCallback) UiSet1541IdleMethod,
      (UiCallbackDataPtr) TRUE1541_IDLE_SKIP_CYCLES, NULL },
    { "*Trap idle", (UiMenuCallback) UiSet1541IdleMethod,
      (UiCallbackDataPtr) TRUE1541_IDLE_TRAP_IDLE, NULL },
    { NULL }
};

#endif

#ifdef SOUND

MenuEntry ChangeSoundSampleRateSubmenu[] = {
    { "*8000Hz", (UiMenuCallback) UiSetSoundSampleRate,
      (UiCallbackDataPtr) 8000, NULL },
    { "*11025Hz", (UiMenuCallback) UiSetSoundSampleRate,
      (UiCallbackDataPtr) 11025, NULL },
    { "*22050Hz", (UiMenuCallback) UiSetSoundSampleRate,
      (UiCallbackDataPtr) 22050, NULL },
    { "*44100Hz", (UiMenuCallback) UiSetSoundSampleRate,
      (UiCallbackDataPtr) 44100, NULL },
    { "*48000Hz", (UiMenuCallback) UiSetSoundSampleRate,
      (UiCallbackDataPtr) 48000, NULL },
    { NULL }
};
MenuEntry ChangeSoundBufferSizeSubmenu[] = {
    { "*1.00 sec", (UiMenuCallback) UiSetSoundBufferSize,
      (UiCallbackDataPtr) 1000, NULL },
    { "*0.75 sec", (UiMenuCallback) UiSetSoundBufferSize,
      (UiCallbackDataPtr) 750, NULL },
    { "*0.50 sec", (UiMenuCallback) UiSetSoundBufferSize,
      (UiCallbackDataPtr) 500, NULL },
    { "*0.35 sec", (UiMenuCallback) UiSetSoundBufferSize,
      (UiCallbackDataPtr) 350, NULL },
    { "*0.30 sec", (UiMenuCallback) UiSetSoundBufferSize,
      (UiCallbackDataPtr) 300, NULL },
    { "*0.25 sec", (UiMenuCallback) UiSetSoundBufferSize,
      (UiCallbackDataPtr) 250, NULL },
    { "*0.20 sec", (UiMenuCallback) UiSetSoundBufferSize,
      (UiCallbackDataPtr) 200, NULL },
    { "*0.15 sec", (UiMenuCallback) UiSetSoundBufferSize,
      (UiCallbackDataPtr) 150, NULL },
    { "*0.10 sec", (UiMenuCallback) UiSetSoundBufferSize,
      (UiCallbackDataPtr) 100, NULL },
    { NULL }
};
MenuEntry ChangeSoundSuspendTimeSubmenu[] = {
    { "*Keep going", (UiMenuCallback) UiSetSoundSuspendTime,
      (UiCallbackDataPtr) 0, NULL },
    { "*1 sec suspend", (UiMenuCallback) UiSetSoundSuspendTime,
      (UiCallbackDataPtr) 1, NULL },
    { "*2 sec suspend", (UiMenuCallback) UiSetSoundSuspendTime,
      (UiCallbackDataPtr) 2, NULL },
    { "*5 sec suspend", (UiMenuCallback) UiSetSoundSuspendTime,
      (UiCallbackDataPtr) 5, NULL },
    { "*10 sec suspend", (UiMenuCallback) UiSetSoundSuspendTime,
      (UiCallbackDataPtr) 10, NULL },
    { NULL }
};

#if defined(CBM64) || defined(C128)
MenuEntry SidModelSubmenu[] = {
    { "*6581 (old)",
      (UiMenuCallback) UiSetSidModel, (UiCallbackDataPtr) 0, NULL },
    { "*8580 (new)",
      (UiMenuCallback) UiSetSidModel, (UiCallbackDataPtr) 1, NULL },
    { NULL }
};
#endif

MenuEntry SoundSubmenu[] = {
    { "*Enable sound playback",
      (UiMenuCallback) UiToggleSound, NULL, NULL },
    { "*Enable adaptive playback",
      (UiMenuCallback) UiToggleSoundSpeedAdjustment, NULL, NULL },
    { "*Sample rate",
      NULL, NULL, ChangeSoundSampleRateSubmenu },
    { "*Buffer size",
      NULL, NULL, ChangeSoundBufferSizeSubmenu },
    { "*Suspend time",
      NULL, NULL, ChangeSoundSuspendTimeSubmenu },
#if defined(CBM64) || defined(C128)
    { "--" },
    { "*Emulate SID filters",
      (UiMenuCallback) UiToggleSidFilters, NULL, NULL },
    { "SID Model",
      NULL, NULL, SidModelSubmenu },
#endif
    { NULL },
};

#endif /* SOUND */

#if defined(CBM64) || defined(C128)

MenuEntry SpecialFeaturesSubmenu[] = {
    { "*Emulation identification",
      (UiMenuCallback) UiToggleEmuID, NULL, NULL },
#ifdef IEEE488
    { "*IEEE488 interface emulation",
      (UiMenuCallback) UiToggleIEEE488, NULL, NULL },
#endif
#ifdef REU
#if REUSIZE == 256
    { "*256K RAM Expansion Unit",
      (UiMenuCallback) UiToggleREU, NULL, NULL },
#else
    { "*512K RAM Expansion Unit",
      (UiMenuCallback) UiToggleREU, NULL, NULL },
#endif
#endif
    { NULL }
};

MenuEntry JoystickSubmenu[] = {
#ifndef HAS_JOYSTICK
    { "*Numpad joystick in port 1",
      (UiMenuCallback) UiSetNumpadJoystickPort, (UiCallbackDataPtr) 1, NULL },
    { "*Numpad joystick in port 2",
      (UiMenuCallback) UiSetNumpadJoystickPort, (UiCallbackDataPtr) 2, NULL },
#else
    { "Joystick device in port 1",
      NULL, NULL, SetJoystickDevice1Submenu },
    { "Joystick device in port 2",
      NULL, NULL, SetJoystickDevice2Submenu },
#endif /* HAS_JOYSTICK */
    { "--" },
    { "Swap joystick ports",
      (UiMenuCallback) UiSwapJoystickPorts, NULL, NULL },
    { NULL }
};

#endif /* CBM64 || C128 */

#ifdef HAVE_TRUE1541
MenuEntry True1541Submenu[] = {
    { "*Enable true 1541 emulation",
      (UiMenuCallback) UiToggleTrue1541, NULL, NULL },
    { "True 1541 sync factor",
      NULL, NULL, Change1541SyncFactorSubmenu },
    { "True 1541 idle method",
      NULL, NULL, Change1541IdleMethodSubmenu },
    { NULL }
};
#endif

MenuEntry VideoSubmenu[] = {
    { "*Video cache",
      (UiMenuCallback) UiToggleVideoCache, NULL, NULL },
    { "*Double size",
      (UiMenuCallback) UiToggleDoubleSize, NULL, NULL },
    { "*Double scan",
      (UiMenuCallback) UiToggleDoubleScan, NULL, NULL },
    { "*Use XSync()",
      (UiMenuCallback) UiToggleUseXSync, NULL, NULL },
#if defined(C128) || defined(CBM64)
    { "--" },
    { "*Sprite-sprite collisions",
      (UiMenuCallback) UiToggleSpriteToSpriteCollisions, NULL, NULL },
    { "*Sprite-background collisions",
      (UiMenuCallback) UiToggleSpriteToBackgroundCollisions, NULL, NULL },
#endif
    { NULL }
};

#ifdef PET
MenuEntry PetUserportSubmenu[] = {
    { "*PET Userport Diagnostic Pin",
      (UiMenuCallback) UiTogglePetDiag, NULL, NULL },
    { NULL }
};
#endif

MenuEntry RightMenu[] = {
    { "Refresh rate",
      NULL, NULL, SetRefreshRateSubmenu },
    { "Maximum speed",
      NULL, NULL, SetMaximumSpeedSubmenu },
#if defined (C128) || defined (CBM64)
    { "--" },
    { "Joystick settings",
      NULL, NULL, JoystickSubmenu },
#endif /* C128 || CBM64 */
    { "Video settings",
      NULL, NULL, VideoSubmenu },
#ifdef SOUND
    { "Sound settings",
      NULL, NULL, SoundSubmenu },
#endif
#ifdef HAVE_TRUE1541
    { "1541 settings",
      NULL, NULL, True1541Submenu },
#endif
#if defined(CBM64) || defined(C128)
    { "I/O Extensions at $DF00",
      NULL, NULL, SpecialFeaturesSubmenu },
#endif
#if defined(PET)
    { "Userport Settings",
      NULL, NULL, PetUserportSubmenu },
#endif    
    { "--" },
    { "Save settings",
      (UiMenuCallback) UiSaveResources, NULL, NULL },
    { "Load settings",
      (UiMenuCallback) UiLoadResources, NULL, NULL },
    { "Restore default settings",
      (UiMenuCallback) UiSetDefaultResources, NULL, NULL },
    { "*Save settings on exit",
      (UiMenuCallback) UiToggleSaveResourcesOnExit, NULL, NULL },
    { NULL }
};

