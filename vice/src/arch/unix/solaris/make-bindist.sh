#!/bin/sh
# make-bindist.sh for the SOLARIS ports
#
# written by Marco van den Heuvel <blackystardust68@yahoo.com>
#
# make-bindist.sh <strip> <vice-version> <prefix> <cross> <zip|nozip> <topsrcdir>
#                 $1      $2             $3       $4      $5          $6

if test x"$3" != "x/usr/local"; then
  echo Error: installation path is not /usr/local
  exit 1
fi

if test x"$4" = "xtrue"; then
  echo Error: make bindist for SOLARIS can only be done on SOLARIS
  exit 1
fi

if [ ! -e /usr/local/bin/x64 -o ! -e /usr/local/bin/x128 -o ! -e /usr/local/bin/xvic -o ! -e /usr/local/bin/xpet -o ! -e /usr/local/bin/xplus4 -o ! -e /usr/local/bin/xcbm2 -o ! -e /usr/local/bin/c1541 -o ! -e /usr/local/bin/petcat -o ! -e /usr/local/bin/cartconv ]
then
  echo Error: \"make install\" needs to be done first
  exit 1
fi

echo Generating SOLARIS port binary distribution.
rm -f -r VICE-$2
mkdir VICE-$2
mkdir -p VICE-$2/usr/local
mkdir -p VICE-$2/usr/local/lib/locale/de/LC_MESSAGES
mv /usr/local/lib/locale/de/LC_MESSAGES/vice.* VICE-$2/usr/local/lib/locale/de/LC_MESSAGES
mkdir -p VICE-$2/usr/local/lib/locale/fr/LC_MESSAGES
mv /usr/local/lib/locale/fr/LC_MESSAGES/vice.* VICE-$2/usr/local/lib/locale/fr/LC_MESSAGES
mkdir -p VICE-$2/usr/local/lib/locale/it/LC_MESSAGES
mv /usr/local/lib/locale/it/LC_MESSAGES/vice.* VICE-$2/usr/local/lib/locale/it/LC_MESSAGES
mkdir -p VICE-$2/usr/local/lib/locale/sv/LC_MESSAGES
mv /usr/local/lib/locale/sv/LC_MESSAGES/vice.* VICE-$2/usr/local/lib/locale/sv/LC_MESSAGES
mkdir -p VICE-$2/usr/local/lib/locale/pl/LC_MESSAGES
mv /usr/local/lib/locale/pl/LC_MESSAGES/vice.* VICE-$2/usr/local/lib/locale/pl/LC_MESSAGES
mkdir -p VICE-$2/usr/local/lib/locale/nl/LC_MESSAGES
mv /usr/local/lib/locale/nl/LC_MESSAGES/vice.* VICE-$2/usr/local/lib/locale/nl/LC_MESSAGES
mkdir -p VICE-$2/usr/local/lib/locale/hu/LC_MESSAGES
mv /usr/local/lib/locale/hu/LC_MESSAGES/vice.* VICE-$2/usr/local/lib/locale/hu/LC_MESSAGES
mkdir VICE-$2/usr/local/bin
mv /usr/local/bin/vsid VICE-$2/usr/local/bin
mv /usr/local/bin/x64 VICE-$2/usr/local/bin
$1 VICE-$2/usr/local/bin/x64
mv /usr/local/bin/x128 VICE-$2/usr/local/bin
$1 VICE-$2/usr/local/bin/x128
mv /usr/local/bin/xvic VICE-$2/usr/local/bin
$1 VICE-$2/usr/local/bin/xvic
mv /usr/local/bin/xpet VICE-$2/usr/local/bin
$1 VICE-$2/usr/local/bin/xpet
mv /usr/local/bin/xplus4 VICE-$2/usr/local/bin
$1 VICE-$2/usr/local/bin/xplus4
mv /usr/local/bin/xcbm2 VICE-$2/usr/local/bin
$1 VICE-$2/usr/local/bin/xcbm2
mv /usr/local/bin/c1541 VICE-$2/usr/local/bin
$1 VICE-$2/usr/local/bin/c1541
mv /usr/local/bin/petcat VICE-$2/usr/local/bin
$1 VICE-$2/usr/local/bin/petcat
mv /usr/local/bin/cartconv VICE-$2/usr/local/bin
$1 VICE-$2/usr/local/bin/cartconv
mv /usr/local/lib/vice VICE-$2/usr/local/lib
rm `find VICE-$2 -name "amiga_*.vkm"`
rm `find VICE-$2 -name "beos_*.vkm"`
rm `find VICE-$2 -name "dos_*.vkm"`
rm `find VICE-$2 -name "os2*.vkm"`
rm `find VICE-$2 -name "win_*.vkm"`
rm `find VICE-$2 -name "RO*.vkm"`
mkdir -p VICE-$2/usr/local/man/man1
mv /usr/local/man/man1/c1541.1 VICE-$2/usr/local/man/man1
mv /usr/local/man/man1/petcat.1 VICE-$2/usr/local/man/man1
mv /usr/local/man/man1/vice.1 VICE-$2/usr/local/man/man1
mkdir VICE-$2/usr/local/info
mv /usr/local/info/vice.info* VICE-$2/usr/local/info
if test x"$5" = "xzip"; then
  rm -f -r /var/spool/pkg/UMCVICE
  gcc $6/src/arch/unix/solaris/convertprototype.c -o ./convertprototype

  currentdir=`pwd`

  cd VICE-$2/usr/local
  find . -print | pkgproto >prototype.tmp
  echo >prototype "i pkginfo=./pkginfo"
  $currentdir/convertprototype prototype.tmp >>prototype

  arch_cpu=`uname -m`
  if test x"$arch_cpu" = "xi86pc"; then
    arch_cpu=x86
  else
    arch_cpu=sparc
  fi

  arch_version=`uname -r`

  if test x"$arch_version" = "x5.6"; then
    arch_version=sol26
  fi

  if test x"$arch_version" = "x5.7"; then
    arch_version=sol7
  fi

  if test x"$arch_version" = "x5.8"; then
    arch_version=sol8
  fi

  if test x"$arch_version" = "x5.9"; then
    arch_version=sol9
  fi

  if test x"$arch_version" = "x5.10"; then
    arch_version=sol10
  fi


  cat >pkginfo <<_END
PKG="UMCVICE"
NAME="VICE"
ARCH="$arch_cpu"
VERSION="$2"
CATEGORY="emulator"
VENDOR="The VICE Team"
EMAIL="vice-devel@firenze.linux.it"
PSTAMP="Marco van den Heuvel"
BASEDIR="/usr/local"
CLASSES="none"
_END

  packagename=vice-$2-$arch_version-$arch_cpu-local

  echo >$currentdir/input.txt all

  pkgmk -r `pwd`
  rm -f -r prototype.tmp
  cd /var/spool/pkg
  pkgtrans -s `pwd` /tmp/$packagename <$currentdir/input.txt
  gzip /tmp/$packagename
  cd $currentdir
  mv /tmp/$packagename.gz ./
  rm -f -r VICE-$2 convertprototype $currentdir/input.txt

  echo SOLARIS port binary package generated as $packagename.gz
else
  echo SOLARIS port binary distribution directory generated as VICE-$2
fi
