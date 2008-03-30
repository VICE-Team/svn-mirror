MacOS X Port of VICE
====================

Official MacOS X Port maintained by 
  Christian Vogelgsang <chris@vogelgsang.org>

Readme based on initial info provided by
  Daniel Aarno

ABOUT MAC PORT
--------------

The Mac OS X port is distributed as a disk image *.dmg. Just click on the
image to mount it and launch an emulator by clicking on its icon. Some
tools (c1541, petcat, cartconv) are provided as command line applications
and thus you will need Terminal to launch them.

To install VICE, you can copy the application bundles of the emulators to 
your Applications folder or any other directory you like.

MAC SPECIFIC ISSUES
-------------------

Make sure to have X11 installed on your Mac. It is provided on your Mac OSX
install CD/DVD (at least on Mac OS X 10.4 or later) and often not installed
by default. Click on the corresponding X11.pkg to install it.

The VICE emulators heavily rely on the use of a two-button mouse. Most Mac 
users do not have this type of mouse, however it is possible for X11 to 
emulate a two (or three) button mouse. If you are using Apples X11.app go to 
Preferences... under the X11 menu and select the appropriate option.

The VICE emulators do not have a "normal" menu, instead the (two) menus are
activated by the left and right mouse-buttons. Important menu options 
(for x64) would be (LB = Left mouse-button, RB = You guess):

* LB->Attach Disk Image->Unit #8
     Attach a disk image
* RB->Keyboard Settings->Keyboard mapping type->Positional Mapping (US)
     Make the keyboard act like a c64 keyboard so you can write all the 
     characters as you would on a regular c64 keyboard, this option might be 
     especially important on non US keyboards.
* RB->Drive settings->Enable true drive emulation
     Disable this option to speed up load time (less compatible)

COMPILATION
-----------

If you wan to compile VICE yourself then you first need to install
Apple's XCode Development kit. The following libraries are also required
for full feature support:

- readline library (only for Mac OS X < 10.4)
    http://tiswww.tis.case.edu/~chet/readline/rltop.html

- HID Utilities (Apple Dev Public Source)
    http://developer.apple.com/samplecode/HID_Utilities_Source/index.html
 
Compile and install the libraries as static libs and
make sure to include the header and lib paths of both libraries in your
CPPFLAGS, CFLAGS, CXXFLAGS and LDFLAGS before calling "configure" of VICE.

Build VICE now with:

 > configure
 > make
 
You can bundle the compiled emulators into a distribution directory with:

 > make bindist
 
A *.dmg is created with:

 > make bindistzip
 
UNIVERSAL BINARIES
------------------

On default the emulator will always build in the binary format of your machine, 
i.e. on a PowerPC Mac you will get a ppc binary and on an Intel Mac you get a
i386 binary. If you want to create a universal binary then use the
make-ub.sh script available in the build/macosx directory. It will perform
all necessary steps to compile for both architectures and finally creates
a universal binary distribution bundle.

First make sure to have a current XCode version with the universal (Mac OS X
10.4u) and the old (Mac OS X 10.3) SDKs installed (look at /Developer/SDKs).

Provide the source archives of readline and HIDUtilities (see above) in the
directory where the vice-x.xx source directory lies. The directory structure
looks like:

  +-- vice-x.xx
  +-- readline-5.1
  +-- HIDUtilitiesSource

Now chdir into the vice-x.xx source directory and call:

 > build/macosx/make-ub.sh
 
If everything went well then the newly created directory UB contains the
distribution DMG.

FEEDBACK
--------

If you discover problems not listed above or just want to tell us your
experiences, please write a mail to the VICE team 

  mailto:vice-devel@firenze.linux.it

or directly to Mac OS X porter

  mailto:chris@vogelgsang.org
