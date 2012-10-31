BeVICE
======

REQUIREMENTS
---------------------------
BeVICE was developed and so far tested under BeOS 5 Personal Edition,
Zeta and Haiku.

INSTALLATION
------------------------
No nice package installer so far, just expand the binary archive to 
wherever you like, enter the directory 'BeVICE-2.*' and start your
favourite emulator (x64, x128, xvic, xpet, xplus4, xcbm2, etc).

COMPILATION
-----------------------
If you have the source distribution of VICE and want to build your own
binaries, the following hints may help:
- Building under BeOS requires the Be development tools; you can get them for
  free at: http://www.bebits.com/
- Expand the source directory vice-2.*
- Start a terminal and enter the source directory
- ./configure
  Configures for building BeOS binaries. You may also be able to use the
  --enable-sdlui option to build binaries using the SDL UI instead of the
  native BeOS UI, or the --with-sdlsound option to use just the SDL sound
  driver with the native UI. (No guarantees.)
- make
  Compiles all the stuff and creates the executables in the src subdirectory.
- make bindist
  Makes a binary distribution directory, after the directory has been made
  you can run VICE from within that directory.


BUGS (only the known;)
-------------------------------------
- Sometimes the application loses the keyboard focus; no keystrokes are
  accepted inside the emu, so the emulator has to be restarted.
  (I have not observed this; it may no longer be true.)
- The sound code has changed significantly and may change more in the future.
  How many machines it does (not) work on is largely unknown at this point.
- The monitor only works if you start the emulator from a terminal; if you
  start the emu from tracker it won't let you enter the monitor.
- The mouse emulation is rather simplistic; if you enable Grab mouse events
  only mouse movements and left button clicks get sent to the emulated machine,
  making some of the Mouse type options useless.

TODO
----------
- Some user interfaces for the settings are missing; some machine
  specific setting are missing; use commandline options or
  edit the vice.ini.
- Fullsrceen mode is needed.

FEEDBACK
------------------
If you discover problems not listed above or just want to tell us your
experiences, please write a mail to the VICE team 
  mailto:vice-emu-mail@lists.sourceforge.net
