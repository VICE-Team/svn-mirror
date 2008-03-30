#!/bin/sh
if test x"$3" != "x/opt"; then
  echo Error: installation path is not /opt
  exit 1
fi

if test x"$4" != "xtrue"; then
  echo Error: make bindist for QNX can only be done on QNX
  exit 1
fi

echo Generating QNX port binary distribution.
rm -f -r ../VICE-$2
mkdir ../VICE-$2
mkdir ../VICE-$2/opt
mkdir -p ../VICE-$2/opt/lib/locale/de/LC_MESSAGES
mv /opt/lib/locale/de/LC_MESSAGES/vice.* ../VICE-$2/opt/lib/locale/de/LC_MESSAGES
mkdir -p ../VICE-$2/opt/lib/locale/fr/LC_MESSAGES
mv /opt/lib/locale/fr/LC_MESSAGES/vice.* ../VICE-$2/opt/lib/locale/fr/LC_MESSAGES
mkdir -p ../VICE-$2/opt/lib/locale/it/LC_MESSAGES
mv /opt/lib/locale/it/LC_MESSAGES/vice.* ../VICE-$2/opt/lib/locale/it/LC_MESSAGES
mkdir -p ../VICE-$2/opt/lib/locale/sv/LC_MESSAGES
mv /opt/lib/locale/sv/LC_MESSAGES/vice.* ../VICE-$2/opt/lib/locale/sv/LC_MESSAGES
mkdir -p ../VICE-$2/opt/lib/locale/pl/LC_MESSAGES
mv /opt/lib/locale/pl/LC_MESSAGES/vice.* ../VICE-$2/opt/lib/locale/pl/LC_MESSAGES
mkdir -p ../VICE-$2/opt/lib/locale/nl/LC_MESSAGES
mv /opt/lib/locale/nl/LC_MESSAGES/vice.* ../VICE-$2/opt/lib/locale/nl/LC_MESSAGES
mkdir ../VICE-$2/opt/bin
mv /opt/bin/vsid ../VICE-$2/opt/bin
mv /opt/bin/x64 ../VICE-$2/opt/bin
$1 ../VICE-$2/opt/bin/x64
mv /opt/bin/x128 ../VICE-$2/opt/bin
$1 ../VICE-$2/opt/bin/x128
mv /opt/bin/xvic ../VICE-$2/opt/bin
$1 ../VICE-$2/opt/bin/xvic
mv /opt/bin/xpet ../VICE-$2/opt/bin
$1 ../VICE-$2/opt/bin/xpet
mv /opt/bin/xplus4 ../VICE-$2/opt/bin
$1 ../VICE-$2/opt/bin/xplus4
mv /opt/bin/xcbm2 ../VICE-$2/opt/bin
$1 ../VICE-$2/opt/bin/xcbm2
mv /opt/bin/c1541 ../VICE-$2/opt/bin
$1 ../VICE-$2/opt/bin/c1541
mv /opt/bin/petcat ../VICE-$2/opt/bin
$1 ../VICE-$2/opt/bin/petcat
mv /opt/bin/cartconv ../VICE-$2/opt/bin
$1 ../VICE-$2/opt/bin/cartconv
mv /opt/lib/vice ../VICE-$2/opt/lib
rm `find ../VICE-$2 -name "amiga_*.vkm"`
rm `find ../VICE-$2 -name "beos_*.vkm"`
rm `find ../VICE-$2 -name "dos_*.vkm"`
rm `find ../VICE-$2 -name "os2*.vkm"`
rm `find ../VICE-$2 -name "win_*.vkm"`
mkdir -p ../VICE-$2/opt/man/man1
mv /opt/man/man1/c1541.1 ../VICE-$2/opt/man/man1
mv /opt/man/man1/petcat.1 ../VICE-$2/opt/man/man1
mv /opt/man/man1/vice.1 ../VICE-$2/opt/man/man1
mkdir ../VICE-$2/opt/info
mv /opt/info/vice.info* ../VICE-$2/opt/info
cd ..
echo QNX port binary distribution directory generated as ../VICE-$2
echo Now running the packager to make the package
packager VICE-$2
