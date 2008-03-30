#!/bin/sh
echo Generating WIN32 port binary distribution.
rm -f -r ../WinVICE-$2
mkdir ../WinVICE-$2
$1 src/x64.exe
$1 src/x128.exe
$1 src/xvic.exe
$1 src/xpet.exe
$1 src/xplus4.exe
$1 src/xcbm2.exe
$1 src/c1541.exe
$1 src/petcat.exe
$1 src/cartconv.exe
cp src/x64.exe src/x128.exe src/xvic.exe ../WinVICE-$2
cp src/xpet.exe src/xplus4.exe src/xcbm2.exe ../WinVICE-$2
cp src/c1541.exe src/petcat.exe src/cartconv.exe ../WinVICE-$2
cp -a data/C128 data/C64 data/CBM-II data/DRIVES ../WinVICE-$2
cp -a data/PET data/PLUS4 data/PRINTER data/VIC20 ../WinVICE-$2
cp -a data/fonts ../WinVICE-$2
cp -a doc/html ../WinVICE-$2
cp FEEDBACK README ../WinVICE-$2
cp doc/cartconv.txt ../WinVICE-$2
rm `find ../WinVICE-$2 -name "Makefile*"`
rm `find ../WinVICE-$2 -name "amiga_*.vkm"`
rm `find ../WinVICE-$2 -name "dos_*.vkm"`
rm `find ../WinVICE-$2 -name "os2*.vkm"`
rm `find ../WinVICE-$2 -name "beos_*.vkm"`
rm `find ../WinVICE-$2 -name "x11_*.vkm"`
rm ../WinVICE-$2/html/texi2html
echo WIN32 port binary destribution generated in ../WinVICE-$2
