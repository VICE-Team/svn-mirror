#!/bin/sh
if test x"$3" != "x/usr/local"; then
  echo Error: installation path is not /usr/local
  exit 1
fi

if test x"$4" != "xtrue"; then
  echo Error: make bindist for SOLARIS can only be done on SOLARIS
  exit 1
fi

echo Generating SOLARIS port binary distribution.
rm -f -r ../VICE-$2
mkdir ../VICE-$2
mkdir -p ../VICE-$2/usr/local
mkdir -p ../VICE-$2/usr/local/lib/locale/de/LC_MESSAGES
mv /usr/local/lib/locale/de/LC_MESSAGES/vice.* ../VICE-$2/usr/local/lib/locale/de/LC_MESSAGES
mkdir -p ../VICE-$2/usr/local/lib/locale/fr/LC_MESSAGES
mv /usr/local/lib/locale/fr/LC_MESSAGES/vice.* ../VICE-$2/usr/local/lib/locale/fr/LC_MESSAGES
mkdir -p ../VICE-$2/usr/local/lib/locale/it/LC_MESSAGES
mv /usr/local/lib/locale/it/LC_MESSAGES/vice.* ../VICE-$2/usr/local/lib/locale/it/LC_MESSAGES
mkdir -p ../VICE-$2/usr/local/lib/locale/sv/LC_MESSAGES
mv /usr/local/lib/locale/sv/LC_MESSAGES/vice.* ../VICE-$2/usr/local/lib/locale/sv/LC_MESSAGES
mkdir -p ../VICE-$2/usr/local/lib/locale/pl/LC_MESSAGES
mv /usr/local/lib/locale/pl/LC_MESSAGES/vice.* ../VICE-$2/usr/local/lib/locale/pl/LC_MESSAGES
mkdir -p ../VICE-$2/usr/local/lib/locale/nl/LC_MESSAGES
mv /usr/local/lib/locale/nl/LC_MESSAGES/vice.* ../VICE-$2/usr/local/lib/locale/nl/LC_MESSAGES
mkdir ../VICE-$2/usr/local/bin
mv /usr/local/bin/vsid ../VICE-$2/usr/local/bin
mv /usr/local/bin/x64 ../VICE-$2/usr/local/bin
$1 ../VICE-$2/usr/local/bin/x64
mv /usr/local/bin/x128 ../VICE-$2/usr/local/bin
$1 ../VICE-$2/usr/local/bin/x128
mv /usr/local/bin/xvic ../VICE-$2/usr/local/bin
$1 ../VICE-$2/usr/local/bin/xvic
mv /usr/local/bin/xpet ../VICE-$2/usr/local/bin
$1 ../VICE-$2/usr/local/bin/xpet
mv /usr/local/bin/xplus4 ../VICE-$2/usr/local/bin
$1 ../VICE-$2/usr/local/bin/xplus4
mv /usr/local/bin/xcbm2 ../VICE-$2/usr/local/bin
$1 ../VICE-$2/usr/local/bin/xcbm2
mv /usr/local/bin/c1541 ../VICE-$2/usr/local/bin
$1 ../VICE-$2/usr/local/bin/c1541
mv /usr/local/bin/petcat ../VICE-$2/usr/local/bin
$1 ../VICE-$2/usr/local/bin/petcat
mv /usr/local/bin/cartconv ../VICE-$2/usr/local/bin
$1 ../VICE-$2/usr/local/bin/cartconv
mv /usr/local/lib/vice ../VICE-$2/usr/local/lib
rm `find ../VICE-$2 -name "beos_*.vkm"`
rm `find ../VICE-$2 -name "dos_*.vkm"`
rm `find ../VICE-$2 -name "os2*.vkm"`
rm `find ../VICE-$2 -name "win_*.vkm"`
mkdir -p ../VICE-$2/usr/local/man/man1
mv /usr/local/man/man1/c1541.1 ../VICE-$2/usr/local/man/man1
mv /usr/local/man/man1/petcat.1 ../VICE-$2/usr/local/man/man1
mv /usr/local/man/man1/vice.1 ../VICE-$2/usr/local/man/man1
mkdir ../VICE-$2/usr/local/info
mv /usr/local/info/vice.info* ../VICE-$2/usr/local/info
vicedir=`pwd`
echo SOLARIS port binary distribution directory generated as ../VICE-$2
echo Now running the packager to make the package
cd ../VICE-$2/usr/local
$vicedir/src/arch/unix/solaris/make_package.pl
