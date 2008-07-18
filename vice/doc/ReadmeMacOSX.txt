Mac OS X Port of VICE
====================

This document contains information relevant for the Mac OS X Port of VICE.

This official Mac OS X Port is maintained by 
  Christian Vogelgsang <chris@vogelgsang.org>


  Contents
  --------

  1. Installation
  2. Usage of Gtk+/X11 Port
  3. Usage of Cocoa Port
  4. Joystick Support
  5. Building VICE
  6. Feedback


1. Installation
---------------


1.1 Choose Port

The VICE Emulators for Macs are distributed in three versions that use
different UI front ends: native Cocoa, X11/Xaw, and, Gtk+. The first one
directly uses the well known Mac user interface. The other ones are based on
the X11 Window System that is available additionally for Mac OS X.

X11/Xaw is based on the original X11 interface of VICE and is very light but
with limited UI interface. The second one is based on the Gtk+ widget library
and has a feature rich UI with pull-down menus. Xaw is a slim port that needs
no non-system extra libraries, Gtk+ requires a large set of external libs and
thus is quite large.

The ports are named with *-cocoa-*, *-x11-*, or *-gtk-* in the distribution
archive name.

The X11/Xaw and Gtk+ are complete ports, i.e. all features available on other
systems are available and all emulators are included. The Cocoa port is still
under construction and thus considered "experimental". It does not include all
features available and some emulators are not complete.


1.2 Install X11 (Only for X11/Xaw and Gtk+ Port)

Make sure you have X11.app installed on your Mac. It is provided on your Mac
OSX install CD/DVD (at least on Mac OS X 10.4 or later) and often not
installed by default. Click on the corresponding "X11.pkg" to install it. On
newer systems there is an "Optional Installs.pkg" installer that allows to
install X11 in the "Custom Installation" section.


1.3 Install Application

The Mac OS X port is distributed as a disk image *.dmg. Just click on the
image to mount it. For later usage it is useful to copy the contents of the
disk image to your hard disk. Just create a new VICE directory in your
applications folder (/Applications) and copy all files there.

If you like, you can move the Application binaries anywhere you want.


1.4 Create Bundles for each Emulator (Optional; Only for X11/Xaw and Gtk+ Port)

The VICE distribution contains a single application bundle called VICE. If you
run this one by double clicking on it then it will ask which emulator to run.

If you want to have own bundles for each emulator then simply copy the VICE
bundle to a new one and name this after one of the emulators (e.g. x64). If
you click on the new bundle then the named emulator will be launched directly
without asking. Make sure that the bundle is named exactly like the emulator
otherwise the launcher will not work.

The separate emulator application bundles are not distributed in the release
dmg to save download time and server space.


1.5 Setup Command Line Tools

You can use the emulators and some additional tools (e.g. c1541, cardconv,
petcat) directly from the command line (in Terminal.app or in an X11 xterm).

Just add the "tools" directory of the distribution to your shell's PATH:

 > export PATH=/Applications/VICE/tools:$PATH

The command line emulators are launched with a helper script that calls the
binary embedded in the VICE.app application bundle. It is important to keep
the VICE.app bundle in the directory directly next to the "tools" directory
otherwise the command line emulators will not run. Similarly, the Cocoa port
needs the emulator App bundle in the same directory.


1.6 Network Setup

This port of VICE supports the ethernet emulation available in some emulators
(e.g. TFE emulation in x64). If you want to use the ethernet support then the
libpcap support library needs to be setup correctly in the system. The
following device nodes on your system need read and write access for your user
name:

  crw-rw----   1 root  admin   23,   0 Nov  1 16:55 /dev/bpf0
  crw-rw----   1 root  admin   23,   1 Nov  1 16:54 /dev/bpf1
  crw-rw----   1 root  admin   23,   2 Nov  1 16:54 /dev/bpf2
  crw-rw----   1 root  admin   23,   3 Nov  1 16:54 /dev/bpf3

This example allows read and write access for the "admin" group. Any user that
is allowed to manage the system can use the ethernet emulation in VICE.

The permissions can be set manually by chmod'ing the nodes. This will last
only for the current session as Mac OS X restores them on reboot. To fix this,
a system start up script can be installed that sets the correct permissions on
each system boot. The script is provided with an installation guide in the
libpcap-source distribution. Download from http://www.tcpdump.org and have a
look at libpcap/README.macosx.

Unfortunately, the WLAN adapter (usually device en1) on portable Macs does not
support libpcap packet capturing (as of 10.4 and 10.5). So TFE emulation on a
wireless network adapter may not work. Use the ethernet port instead.



2. Usage of Gtk+ and X11/Xaw Ports
----------------------------------

2.1 Run VICE Bundle

Simply run the emulator suite by clicking on the VICE.app icon. A chooser
dialog will be opened to select the emulator you want to launch. If you want
to launch an emulator directly then see section 1.4 for creating separate
bundles for each emulator.

On start up the bundle will first open a terminal window (xterm) for the VICE
log messages and then the screen windows for the virtual displays.

The VICE bundles support a simple style of drag and drop: You can launch the
emulator by dragging a disk image (*.d64 *.d71 *.d81) or a program file (*.prg
*.p00) on the bundle icon. This will smart-attach the given binary and launch
the emulator. If the emulator is already running then drag and drop does not
work.

Also, an emulator bundle will register the typical file types (see above) and
then you can simply click on a disk image directly to launch it in VICE.


2.2 Run from Command Line

First add the "tools" directory to your PATH (see section 1.5) in your
favorite shell. Now you can run all emulators directly by calling the launcher
script in "tools" (e.g. x64, x128...). Any arguments passed to the launcher
are directly forwarded to the emulator binary.

For the Gtk+ and X11/Xaw ports on start up the launcher ensures that the X11
environment is available and redirects all console output directly to the
console. An additional xterm window for log messages is not opened.


2.3 Use the X11 UI

The X11 UI is very light and only presents the virtual display in a window
without any decoration. All controls and options of VICE are kept in two
pull-down menus that are available after pressing the left or right mouse
button inside the emulation screen. 

If you do not have a second mouse button on your Mac then you can emulate the
right mouse by pressing ALT + left mouse button. Make sure to enable this
option in the Preferences of X11.app:

  "Emulate three button mouse: On"

Some menu entries have keyboard short cuts. The commands use the Control key
on other ports but the Command (Apple) key on Macs. This will only work if you
disable X11's own short cut mapping in the Preferences of X11.app:

  "Enable keyboard shortcuts under X11: Off" 

Have a look at the official VICE manuals for an in-depth description of the
available commands and options.


2.4 Use the Gtk+ UI

The Gtk+ UI is much more user-friendly. An emulator window with a pull-down
menu is opened and all controls and options are available there by clicking
with the left mouse button.

The menu short cuts work as annotated in the menus: Press the Control key plus
the assigned short cut key. Gtk+ does not use the Apple/Command key so the
remapping as proposed in 2.3 for the X11 UI is not required.

Have a look at the official VICE manuals for an in-depth description of the
available commands and options.


2.5 Keyboard Mapping in Gtk+/X11

The keyboard mapping on X11 (and on your Mac) is done as follows:
 
 1. Map raw key code of your keyboard to X11 key code with Xmodmap
 2. VICE maps the X11 key code to the virtual cbm key with a keymap

The X11 key code mapping (1.) on Macs is performed similar to the mapping of
native Mac OS X applications. In fact X11.app installs a key map similar to
the Mac OS X one. It even keeps track of system key map changes (by clicking
on the flag icon in your menu bar) and sets up a new map in running X11. Make
sure to enable this option in the Preferences of X11.app:

  "Use the system keyboard layout: On"

The VICE mapping (2.) is defined in *.vkm files and selected by directly
choosing a file or by selecting a predefined one. Predefined symbolic and
positional mappings are available.

The 'symbolic mapping' assigns the pressed Mac key to the same labeled key on
the virtual cbm keyboard. So pressing F1 on the Mac presses F1 on the emulated
machine. It also does automatic shifting/unshifting, e.g. pressing F2 on the
Mac results in a virtual shift press and a virtual key F1 press (i.e. F2 is
shift+F1 on the emulated machine).

Some virtual keys are not available on modern keyboards. In this case, unused
mac keys are remapped. In the standard positional map x11_sym.vkm:

  Virtual Key               Mac Key
  -----------               -------
   RESTORE                   Page Up (Fn+Cursor Up)
   CONTROL                   Tab
   COMMODORE                 Left Control
   RUN/STOP                  Escape
   CLR/HOME                  Home (Fn+Cursor Left)
   INST/DEL                  Insert / Delete Backspace
				 
   Pound Sign                Backslash (\) Bar (|)              
   Left Arrow                Underscore (_)               
   Up Arrow                  Tilde (~) Backtick (`) Grave (^)

The 'positional mappings' try to map the physical location of a key on the Mac
keyboard to a virtual key at a similar position. E.g. your top and leftmost
key will be the top and leftmost key on your virtual machine. Unfortunately,
this mapping does not work always as expected: It relies heavily on the
national layout of your keymap. Furthermore, the physical layout of Mac and
VICE machine do not match exactly, so positional mapping will always be an
approximation. This can be even more complicate if you use different key
mappings on your Mac.

If you are using a portable Mac (MacBook, MacBook Pro, Powerbook) then make
sure to make the function keys available for VICE/X11: In the Mac OS X System
Preferences in section "Keyboard & Mouse" enable the option "Use the F1-F12
keys to control software features". The hardware controls are then accessed by
pressing F1-F12 + Fn key.



3. Usage of Cocoa Port
----------------------

Note: The Cocoa port is still "experimental", i.e. it is not full featured and
some things may be missing. This holds for the documentation as well ;)

3.1 Features

 * native Mac OS X user interface with modeless dialogs
 * any-size/fullscreen OpenGL accelerated rendering
 * multi-threaded emulation core


3.2 Current State

 * x64 90% complete, runs with UI and most menus and dialogs.
 
 * All other emus: binaries are available, no menus and dialogs.
   Use CMD+Q to quit such an emulator and drag&drop to attach disk images.
   Some keyboard mappings already work, others not.


3.3 Known Issues

 * The Resource Inspector lacks correct input cells. All values have to
   be entered as strings.
 * Some Menu entries are not greyed out but still do nothing.


3.4 Key Mapping

The key mapping show the Mac key on a US extended mapping and the right one is
the emulated Commodore key. Special keys are marked with <Symbol>.

  § (Paragraph)                 <LEFT ARROW>
  <Tab>                         <CONTROL>
  ` (Backtick), Left Alt/Ctrl   <CBM>
  <ESC>                         <RUN/STOP>
  <Backspace>                   <DEL>
  <F9>, <PgDown>                <POUND>
  <F10>, <PgUp>                 <UP ARROW>
  <F11>, <Home>                 <CLR/HOME>
  <F12>                         <RESTORE>


4. Joystick Support
-------------------

The Mac ports has support for USB joysticks. It requires a generic USB HID
joystick device which is supported by most joystick models. Every button of
the joystick is mapped to the single virtual fire button in the emulator. The
directions are derived from the X and Y axis of the device. Analog directional
ranges are converted to digital values automatically. Calibration of analog
devices is currently not supported.

Make sure to connect your joystick(s) before starting up VICE. Although USB
are plug-and-playable the current implementation requires the joystick state
to be fixed while VICE runs. So detach the joystick only after quitting the
emulator.

The Mac joysticks are always enumerated as "Analog Joystick" in VICE. The
order of devices is determined automatically in Mac OS X and cannot be
changed.

Joystick support was tested with the USB version of the Competition Pro but
should work with other models as well.


5. Building VICE
----------------

This section is only relevant if you want to compile an own distribution of
VICE from the source code.


5.1 XCode

If you wan to compile VICE yourself then you first need to install Apple's
XCode Development kit. It is available on your Mac OS X Installation DVD or
can be downloaded for free (after registration) from the Apple Developer
Network (connect.apple.com). Make sure to install the "Universal SDK 10.4"
from the "Cross Development" section if you want to create a universal binary
of VICE.

XCode itself is not required but the command line tools (compiler, linker...)
and other development files (SDKs).


5.2 Required Tools

The following tools are required or optional for a successful VICE build:

 - Platypus  (Optional)
   
     http://www.sveinbjorn.org/platypus
   
   This script bundler is used to embed the launcher script of VICE into
   an application bundle. Additionally, Platypus supports simple drag and
   drop on application start up.

   If Platypus is not available then a launcher replacement is installed. This
   script does not support drag and drop and quits immediately afert start up
   (i.e. the VICE application icon disappears after the launch)!


5.3 Required Library Sources

VICE for Macs relies on a number of external libraries. You have to get their
source code as they are missing on a default Mac system. Some libs are
optional, i.e. if they are available then some extra features of VICE are
available.

- readline library  (required only for Mac OS X < 10.4)

    http://tiswww.tis.case.edu/~chet/readline/rltop.html

- HID Utilities  (optional) (Joystick support)

    http://developer.apple.com/samplecode/HID_Utilities_Source/index.html
 
- Gtk+ Libraries  (required only for Gtk+ port)

	gettext-0.16.tar.gz      http://www.gnu.org/software/gettext/
	pkg-config-0.21.tar.gz   http://pkgconfig.freedesktop.org/wiki/
	jpegsrc.v6b.tar.gz       http://www.ijg.org/files/
	libpng-1.0.20.tar.bz2    http://www.libpng.org/pub/png/libpng.html
	tiff-3.8.2.tar.gz        http://www.libtiff.org/
	freetype-2.1.10.tar.bz2  http://www.freetype.org/
	libxml2-2.6.27.tar.gz    http://xmlsoft.org/
	fontconfig-2.4.1.tar.gz  http://www.fontconfig.org/wiki/
	glib-2.12.4.tar.bz2      http://www.gtk.org/
	cairo-1.2.6.tar.gz       http://cairographics.org/
	pango-1.14.7.tar.bz2     http://www.gtk.org/
	atk-1.9.0.tar.bz2        http://www.gtk.org/
	gtk+-2.10.6.tar.gz       http://www.gtk.org/

- Network Libraries  (optional) (Ethernet Emulation)

    libpcap-0.9.5.tar.gz        http://www.tcpdump.org/
    libnet0_1.0.2a.orig.tar.gz  http://ftp.debian.org/debian/pool/main/libn/libnet0/libnet0_1.0.2a.orig.tar.gz

- FFMPEG  (optional) (Video and Sound Capture)

    lame-3.97.tar.gz            http://lame.sourceforge.net/
    svn tree at                 svn checkout svn://svn.mplayerhq.hu/ffmpeg/trunk ffmpeg-svn


5.4 Building Required Libraries

First, build all external libraries. The VICE source distribution contains
build scripts to automatically perform this step.

Copy all library source archives into a single directory and call a build
script found in "build/macosx/" of the VICE source tree. The scripts usually
take two arguments: target directory and compile architecture (i386 or ppc).

The convenience script "build-allext.sh" builds all external libs given in
section 3.3 for both architectures: intel (i386) and power pc (ppc). This is
required for a universal binary build of VICE. The script takes only a single
argument: the library installation directory.

Example:

 > mkdir extlib-src
 > mkdir extlib
 > cp <all lib source archives> extlib-src/
 > cd extlib-src
 > sh <VICE srcdir>/build/macosx/build-allext.sh ../extlib

If all went well then the libraries are compiled and installed in the "extlib"
directory. The libraries are now ready for the VICE build.


5.5 Building VICE

The build process of VICE contains the following steps: Call "configure" in
the source tree to determine the required "make" system setup and find
available libs. Then compile the source tree to get binaries of all emulators
and tools.

A release is started by creating a distribution directory. Then the emulator
binaries are embedded in the VICE application bundle. Furthermore, the tools,
external libraries and documentation is added and finally, a release disk
image (dmg) is created.

If you want to create a universal binary of VICE then the "configure" and
"make" stages are repeated two times: for i386 and ppc. The resulting binaries
are then united into single universal binaries. This is also done for all
external libraries.

All the necessary steps for a successfull build are integrated in the build
script "build/macosx/build-vice-dist.sh". It has the following usage and must
be run from the VICE source base directory:

Usage: 

  build/macosx/build-vice-dist.sh <arch> <ui-type> <dist-type> <extlib-dir> <build-dir>

   arch        Build architecture       ub,i386,ppc
   ui-type     User Interface Type      x11,gtk
   dist-type   Type of Distribution     dmg,dir
   extlib-dir  External Libraries
   build-dir   Where VICE is built

arch: Choose the target architecture. Make sure the external libs are compiled
similar! (ub=universal binary, i386=Intel Mac, ppc=PowerPC Mac)

ui-type: Select the User Interface: either X11 (x11) or Gtk+ (gtk) based. Gtk+
requires the external Gtk+ libraries.

dist-type: Either create a disk image with the everthing embedded inside (dmg)
or create a directory with all files (dir) for direct access.

ext-lib: Where are the external libraries installed? Use same path as in
section 3.4.

build-dir: The newly created directory where all compilation takes place. The
created distribution is found inside this directory and all intermediate
files.

Example:

  > cd [VICE src dir]
  > sh build/macosx/build-vice-dist.sh ub x11 dmg ../extlib TEMP

This creates the TEMP directory in the VICE source tree and builds a universal
binary VICE distribution with X11 interface. The result is a *.dmg release
disk image found in the "TEMP/x11/ub/" directory.


6. Feedback
-----------

If you discover problems not listed above or just want to tell us your
experiences, please write a mail to the VICE team 

  mailto:vice-devel@firenze.linux.it

or directly to your fellow Mac OS X porter

  mailto:chris@vogelgsang.org


Have fun,
-Chris
