This document describes how to compile VICE using the openwatcom compiler.

- Start the openwatcom IDE and open vice.wpj and perform a make all.

- After the compiling is done you should have c1541.exe, cartconv.exe,
  petcat.exe, x128.exe, x64.exe, x64dtv.exe, x64sc.exe, xcbm2.exe,
  xpet.exe, xplus4.exe and xvic.exe in the watcom folder.

- You can optionally run the make-watcom-bindist.bat file to create a
  distribution directory, from which you can run the emulators.
