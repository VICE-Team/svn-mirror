  __  __          __     _____ ____ _____
 |  \/  | __ _  __\ \   / /_ _/ ___| ____|
 | |\/| |/ _` |/ __\ \ / / | | |   |  _|
 | |  | | (_| | (__ \ V /  | | |___| |___
 |_|  |_|\__,_|\___| \_/  |___\____|_____|


IMPORTANT INFORMATION
_____________________

There are some special considerations when using VICE on MacOS X I therefor 
urge you to read through this file before using any of the software installed 
with this package!

ABOUT MACVICE
______________

The MacVICE application is not a part of the normal VICE package and its only 
purpose is to make life a bit easier for the average Mac user for further 
details on MacVICE see the end of this file.

MAC SPECIFIC ISSUES
___________________
You will need X11 installed to use this software.

The VICE emulators heavily rely on the use of a two-button mouse. Most Mac 
users do not have this type of mouse, however it is possible for X11 to 
emulate a two (or three) button mouse. If you are using Apples X11.app go to 
Preferences... under the X11 menu and select the appropriate option.

It is not possible to move the files (or the VICE directory itself) to 
another location.

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

MACVICE
_______

MacVICE is NOT an emulator and is NOT (yet) connected to the VICE project. 
MacVICE is simply a launcher for the wonderful emulators produced by the VICE 
team that allows Mac users to start there emulator sessions in a way more 
natural for them.

THE VICE TEAM SHOULD NOT BE BLAMED OR DISCREDITED IN ANY WAY FOR THIS 
SOFTWARE. NOR SHALL I (Daniel Aarno) RECEIVE ANY CREDIT FOR THE WORK DONE ON 
THE EMULATORS WHICH IS THE PRODUCT OF THE HARD WORKING VICE TEAM.

MacVICE is licensed under the Attribution assurance license, see the file 
LICENSE.txt for more information. The set of VICE emulators are licensed under 
the GPL see the file GPL.txt for further information.

Hopefully you will find this piece of software useful, any comments, 
questions or bug-reports may be sent to: macbishop@users.sf.net.

Note: Since MacVICE is a quick 'hack' it silently ignores all errors.

ABOUT THIS BINARY DISTRIBUTION
______________________________

This binary distribution of VICE for MacOS X was put together by 
Daniel Aarno - macbishop@users.sf.net. If you have any questions or comments 
regarding this binary distribution please send an e-mail to 
macbishop@users.sf.net with VICE in the subject, if you have questions about 
VICE in general see the FEEDBACK and README files.

The binaries are optimized for the MPPC750 (G3) microprocessor.

INSTALLATION
------------

Simply double-click the MacVICE.pkg icon and follow the instructions.
