SDL port of VICE
================


    Contents
    --------

    1. General info
    2. Usage
    3. Building


1. General info
===============

1.1 Goal

The SDL port is meant to be an easily portable version of VICE
that is fully usable with just a joystick (with at least 2 buttons)
or a keyboard. OS/arch-specific features (such as RS-232 support)
may be omitted for portability. Everything is configurable via the UI.


1.2 Features

Some new features that are missing from (some of) the native versions:
 - Free scaling using OpenGL (from the GTK port)
 - Virtual keyboard (adapted from the GP2X port)
 - Hotkey mapping to any menu item
 - (Host) joystick event mapping to (machine) joystick, keyboard or menu item
 - No mouse required


1.3 Ports

The SDL port has been tested to work on:
 - Linux / x86, x86_64/amd64, ppc, alpha
 - Win32 / x86
 - Amiga OS 4.x / powerpc
 - Morphos / powerpc
 - AROS / x86, powerpc, x86_64/amd64
 - GP2X / arm (needs fixing)
 - BeOS / x86, powerpc
 - Openserver / x86
 - Unixware / x86
 - Solaris / x86, sparc
 - SkyOS / x86
 - QNX 6.x / x86, armle, mipsle, ppcbe, shle
 - Syllable / x86
 - Win64 / x86_64/amd64
 - ...


2. Usage
========

2.1 The menu

The menu is used with the following commands:
(default keys/joymap shown)

--------------------------------------------------
Command  | Key   | Joy  | Function
--------------------------------------------------
Activate | F12   | btn2 | Activate the menu
Up       | up    |  u   | Move cursor up
Down     | down  |  d   | Move cursor down
Left     | left  |  l   | Cancel/Page up
Right    | right |  r   | Enter menu/Page down
Select   | enter | fire | Select the item
Cancel   | <-    | btn2 | Return to previous menu
Exit     | ESC   | N/A  | Exit the menu
Map      | m     | btn3 | Map a hotkey (see 2.2)
--------------------------------------------------

The keys and joystick events can be configured via the menu.

Left/Right work as Page up/down on the file selector, otherwise
left does Cancel and right enters the selected submenu.

The joystick command Activate behaves as Cancel while in the menu.


2.2 Hotkeys

By default, the SDL port doesn't have any hotkeys defined.
Mapping a hotkey is simple:

1. Navigate the menu to an item
2. Issue the Map command (default: 'm', button 3)
3. Press the desired key(-combo) or joystick direction/button

Hotkeys can be unmapped by mapping the hotkey to an empty menu item.


2.3 Virtual keyboard

The menu commands are also used in the virtual keyboard:
(default keys/joymap shown)

--------------------------------------------------
Command  | Key   | Joy  | Function
--------------------------------------------------
Up       | up    |  u   | Move cursor up
Down     | down  |  d   | Move cursor down
Left     | left  |  l   | Move cursor left
Right    | right |  r   | Move cursor right
Select   | enter | fire | Press/release the key
Cancel   | <-    | btn2 | Press/release with shift
Exit     | ESC   | N/A  | Close the virtual kbd
Map      | m     | btn3 | Map a key/button
--------------------------------------------------

Note that pressing a key and releasing the key generate separate events.
This means that pressing Select on a key, moving to an another key and
releasing Select releases the latter key; with this, multiple keys
can be pressed down at once.

The joystick command Activate behaves as Cancel while using the virtual
keyboard.

The virtual keyboard can be moved by pressing (and holding) Select on an
empty space and moving the cursor.

The virtual keyboard can be closed by pressing the 'X' in the top left corner,
with the command Exit or with the command Cancel when the cursor is at an
empty spot.

Keys and joystick events can be mapped to the keyboard via the virtual keyboard.


2.4 Text input dialog virtual keyboard

The text input dialog also has a virtual keyboard, which can be activated
with the key F10 or joystick commands Cancel or Map.

When the virtual keyboard is active, the following commands are active:
(default keys/joymap shown)

--------------------------------------------------
Command  | Key   | Joy  | Function
--------------------------------------------------
Up       | up    |  u   | Move cursor up
Down     | down  |  d   | Move cursor down
Left     | left  |  l   | Move cursor left
Right    | right |  r   | Move cursor right
Select   | enter | fire | Press the key
Cancel   | <-    | btn2 | Press with shift
Exit     | ESC   | N/A  | Close the virtual kbd
Map      | m     | btn3 | Close the virtual kbd
--------------------------------------------------

The joystick command Activate behaves as Cancel while using the virtual
keyboard.

Simultaneous keypresses are not possible. The virtual keyboard cannot be moved.

The virtual keyboard can be closed by pressing the 'X' in the top left corner,
with the commands Exit and Map or with the command Cancel when the cursor is at an
empty spot.

To exit/cancel the dialog itself, press "esc" on the virtual keyboard.

Note that normal text input via keyboard is not possible while the virtual
keyboard is active.


2.5 Settings

The settings are saved separately into 4 files:
 - main settings (sdl-vicerc, sdl-vice.ini, "Load/Save settings")
 - fliplist (...)
 - hotkey (sdl-hotkey-MACHINE.vkm, "Load/Save hotkeys")
 - joymap (sdl-joymap-MACHINE.vjm, "Load/Save joystick map")

Remember to save the relevant settings file.


3. Building
===========

3.1 Building in *nix compile enviroments.

./configure --enable-sdlui
make
make install

You'll need the SDL libs and headers. For free scaling, the OpenGL is
also needed (libGL, opengl32.dll, ...)


3.2 Building in Visual Studio.

For MSVC building instructions see src/arch/sdl/win32-msvc/Readme.txt
