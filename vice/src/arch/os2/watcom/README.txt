This document describes how to compile VICE using the openwatcom compiler.

- The target and project files need to be turned from unix line-ending format
  to dos line-ending format, you can use a program like unix2dos for that,
  only the *.tgt and vice.wpj files need to be converted.

- Start the openwatcom IDE and open vice.wpj and perform a make all.

- After the compiling is done you should have x1541.exe, cartconv.exe,
  petcat.exe, x128.exe, x64.exe, xcbm2.exe, xpet.exe, xplus4.exe and xvic.exe
  in the watcom folder.
