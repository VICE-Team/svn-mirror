#!/bin/sh
echo Generating AMIGAOS port binary distribution.
rm -f -r ../VICE
mkdir ../VICE
if test x"$2" != "xaros"; then
  $1 src/x64$3
  $1 src/x128$3
  $1 src/xvic$3
  $1 src/xpet$3
  $1 src/xplus4$3
  $1 src/xcbm2$3
  $1 src/c1541$3
  $1 src/petcat$3
  $1 src/cartconv$3
fi
cp src/x64$3 ../VICE/x64.exe
cp src/x128$3 ../VICE/x128.exe
cp src/xvic$3 ../VICE/xvic.exe
cp src/xpet$3 ../VICE/xpet.exe
cp src/xplus4$3 ../VICE/xplus4.exe
cp src/xcbm2$3 ../VICE/xcbm2.exe
cp src/c1541$3 ../VICE/c1541.exe
cp src/petcat$3 ../VICE/petcat.exe
cp src/cartconv$3 ../VICE/cartconv.exe
cp -a data/C128 data/C64 data/CBM-II data/DRIVES ../VICE
cp -a data/PET data/PLUS4 data/PRINTER data/VIC20 ../VICE
cp -a data/fonts ../VICE
cp -a doc/html ../VICE
cp FEEDBACK README ../VICE
cp src/arch/amigaos/README.OS4 ../VICE
cp src/arch/info-files/VICE.info ../
cp src/arch/info-files/*.exe.info ../VICE
rm `find ../VICE -name "Makefile*"`
rm `find ../VICE -name "dos_*.vkm"`
rm `find ../VICE -name "os2*.vkm"`
rm `find ../VICE -name "beos_*.vkm"`
rm `find ../VICE -name "win_*.vkm"`
rm `find ../VICE -name "x11_*.vkm"`
rm ../VICE/html/texi2html
echo AMIGAOS port binary destribution directory generated as ../VICE
