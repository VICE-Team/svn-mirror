\#!/bin/sh
# make-bindist.sh for the OPENSERVER, UNIXWARE & SOLARIS ports
#
# written by Marco van den Heuvel <blackystardust68@yahoo.com>
#
# make-bindist.sh <strip> <vice-version> <prefix> <cross> <zip|nozip> <system> <infodir> <mandir> <topsrcdir> <make-command>
#                 $1      $2             $3       $4      $5          $6       $7        $8       $9          $10

STRIP=$1
VICEVERSION=$2
PREFIX=$3
CROSS=$4
ZIPKIND=$5
SYSTEM=$6
INFODIR=$7
MANDIR=$8
TOPSRCDIR=$9

shift
MAKECOMMAND=$9

setnormalmake()
{
  makefound="none"
  OLD_IFS=$IFS
  IFS=":"

  for i in /usr/ccs/bin:$PATH
  do
    if [ -e $i/make ]; then
      GNUMAKE=`$i/make --version`
      case "$GNUMAKE" in
        GNU*)
          ;;
        *)
          if test x"$makefound" = "xnone"; then
            makefound="$i/make"
          fi
          ;;
      esac
    fi
  done
  if test x"$makefound" = "xnone"; then
    echo no suitable make found for bindist
    exit 1
  else
    MAKECOMMAND=$makefound
  fi
  IFS=$OLD_IFS
}

checkmake()
{
  GNUMAKE=`$MAKECOMMAND --version`
  case "$GNUMAKE" in
  GNU*)
     setnormalmake
     ;;
  esac
}

if test x"$PREFIX" != "x/usr/local"; then
  echo Error: installation path is not /usr/local
  exit 1
fi

if test x"$SYSTEM" = "xsco7"; then
  PLATFORM="UNIXWARE 7.x"
  checkmake
fi

if test x"$SYSTEM" = "xsco6"; then
  PLATFORM="OPENSERVER 6.x"
  checkmake
fi

if test x"$SYSTEM" = "xsco5"; then
  PLATFORM="OPENSERVER 5.x"
  checkmake
fi

if test x"$SYSTEM" = "xsol"; then
  PLATFORM="SOLARIS"
fi

if test x"$CROSS" = "xtrue"; then
  echo Error: make bindist for $PLATFORM can only be done on $PLATFORM
  exit 1
fi

if [ ! -e src/x64 -o ! -e src/x128 -o ! -e src/xvic -o ! -e src/xpet -o ! -e src/xplus4 -o ! -e src/xcbm2 -o ! -e src/c1541 -o ! -e src/petcat -o ! -e src/cartconv ]
then
  echo Error: \"make\" needs to be done first
  exit 1
fi

echo Generating $PLATFORM port binary distribution.
rm -f -r VICE-$VICEVERSION
curdir=`pwd`
$MAKECOMMAND -e prefix=$curdir/VICE-$VICEVERSION/usr/local VICEDIR=$curdir/VICE-$VICEVERSION/usr/local/lib/vice install
$STRIP VICE-$VICEVERSION/usr/local/bin/x64
$STRIP VICE-$VICEVERSION/usr/local/bin/x128
$STRIP VICE-$VICEVERSION/usr/local/bin/xvic
$STRIP VICE-$VICEVERSION/usr/local/bin/xpet
$STRIP VICE-$VICEVERSION/usr/local/bin/xplus4
$STRIP VICE-$VICEVERSION/usr/local/bin/xcbm2
$STRIP VICE-$VICEVERSION/usr/local/bin/c1541
$STRIP VICE-$VICEVERSION/usr/local/bin/petcat
$STRIP VICE-$VICEVERSION/usr/local/bin/cartconv
rm `find VICE-$VICEVERSION -name "amiga_*.vkm"`
rm `find VICE-$VICEVERSION -name "beos_*.vkm"`
rm `find VICE-$VICEVERSION -name "dos_*.vkm"`
rm `find VICE-$VICEVERSION -name "os2*.vkm"`
rm `find VICE-$VICEVERSION -name "osx*.vkm"`
rm `find VICE-$VICEVERSION -name "win_*.vkm"`
rm `find VICE-$VICEVERSION -name "RO*.vkm"`
rm `find VICE-$VICEVERSION -name "*.vsc"`
mkdir -p VICE-$VICEVERSION$MANDIR/man1
if test x"$ZIPKIND" = "xzip"; then
  rm -f -r /var/spool/pkg/UMCVICE
  gcc $TOPSRCDIR/src/arch/unix/sco_sol/convertprototype.c -o ./convertprototype

  currentdir=`pwd`

  cd VICE-$VICEVERSION/usr/local
  find . -print | pkgproto >prototype.tmp
  echo >prototype "i pkginfo=./pkginfo"
  $currentdir/convertprototype prototype.tmp >>prototype

  if test x"$SYSTEM" = "xsol"; then
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

    if test x"$arch_version" = "x5.11"; then
      arch_version=sol11
    fi
  else
    arch_cpu=x86

    if test x"$SYSTEM" = "xsco5"; then
      arch_version=osr5
    fi

    if test x"$SYSTEM" = "xsco6"; then
      arch_version=osr6
    fi

    if test x"$SYSTEM" = "xsco7"; then
      arch_version=uw7
    fi
  fi

  cat >pkginfo <<_END
PKG="UMCVICE"
NAME="VICE"
ARCH="$arch_cpu"
VERSION="$VICEVERSION"
CATEGORY="emulator"
VENDOR="The VICE Team"
EMAIL="vice-devel@firenze.linux.it"
PSTAMP="Marco van den Heuvel"
BASEDIR="/usr/local"
CLASSES="none"
_END

  packagename=vice-$VICEVERSION-$arch_version-$arch_cpu-local

  echo >$currentdir/input.txt all

  pkgmk -r `pwd`
  rm -f -r prototype.tmp
  cd /var/spool/pkg
  pkgtrans -s `pwd` /tmp/$packagename <$currentdir/input.txt
  gzip /tmp/$packagename
  cd $currentdir
  mv /tmp/$packagename.gz ./
  rm -f -r VICE-$VICEVERSION convertprototype $currentdir/input.txt

  echo $PLATFORM port binary package generated as $packagename.gz
else
  echo $PLATFORM port binary distribution directory generated as VICE-$VICEVERSION
fi
