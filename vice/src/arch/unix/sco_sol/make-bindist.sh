#!/bin/sh
# make-bindist.sh for the OPENSERVER, UNIXWARE & SOLARIS ports
#
# written by Marco van den Heuvel <blackystardust68@yahoo.com>
#
# make-bindist.sh <strip> <vice-version> <prefix> <cross> <zip|nozip> <x64sc-included> <system> <gui> <infodir> <mandir> <topsrcdir> <make-command>
#                 $1      $2             $3       $4      $5          $6               $7       $8    $9        $10      $11

STRIP=$1
VICEVERSION=$2
PREFIX=$3
CROSS=$4
ZIPKIND=$5
X64SC=$6
SYSTEM=$7
GUI=$8
INFODIR=$9

shift
MANDIR=$9

shift
TOPSRCDIR=$9

shift
MAKECOMMAND=$9

setnormalmake()
{
  makefound="none"
  OLD_IFS=$IFS
  IFS=":"

  TOTALPATH="/usr/ccs/bin:$PATH"

  for i in $TOTALPATH
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
  checkmake
fi

if test x"$CROSS" = "xtrue"; then
  echo Error: make bindist for $PLATFORM can only be done on $PLATFORM
  exit 1
fi

if test x"$X64SC" = "xyes"; then
  SCFILE="x64sc"
else
  SCFILE=""
fi

EMULATORS="x64 x64dtv $SCFILE x128 xcbm2 xpet xplus4 xvic"
CONSOLE_TOOLS="c1541 cartconv petcat"
EXECUTABLES="$EMULATORS $CONSOLE_TOOLS"

for i in $EXECUTABLES
do
  if [ ! -e src/$i ]
  then
    echo Error: \"make\" needs to be done first
    exit 1
  fi
done

echo Generating $PLATFORM port binary distribution.
rm -f -r VICE-$VICEVERSION
curdir=`pwd`
$MAKECOMMAND -e prefix=$curdir/VICE-$VICEVERSION/usr/local VICEDIR=$curdir/VICE-$VICEVERSION/usr/local/lib/vice install
for i in $EXECUTABLES
do
  $STRIP VICE-$VICEVERSION/usr/local/bin/$i
done
mkdir -p VICE-$VICEVERSION/$MANDIR/man1
if test x"$ZIPKIND" = "xzip"; then
  rm -f -r /var/spool/pkg/UMCVICE
  gcc $TOPSRCDIR/src/arch/unix/sco_sol/convertprototype.c -o ./convertprototype

  currentdir=`pwd`

  cd VICE-$VICEVERSION/usr/local
  find . -print | pkgproto >prototype.tmp
  echo >prototype "i pkginfo=./pkginfo"
  $currentdir/convertprototype prototype.tmp >>prototype

  if test x"$SYSTEM" = "xsol"; then
    arch_cpu_name=`uname -m`
    arch_cpu=unknown

    file bin/x64 >file.tmp
    cpu_is_64bit=`sed -n -e "s/.*\(64-bit\).*/\1/p" file.tmp`
    rm -f file.tmp

    if test x"$arch_cpu_name" = "xi86pc"; then
      if test x"$cpu_is_64bit" = "x64-bit"; then
        arch_cpu=amd64
      else
        arch_cpu=x86
      fi
    fi

    if test x"$arch_cpu_name" = "xs390"; then
      arch_cpu=s390
    fi

    if test x"$arch_cpu_name" = "xarm"; then
      arch_cpu=arm
    fi

    if test x"$arch_cpu" = "xunknown"; then
      if test x"$cpu_is_64bit" = "x64-bit"; then
        arch_cpu=sparc64
      else
        arch_cpu=sparc
      fi
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
NAME="VICE-$GUI"
ARCH="$arch_cpu"
VERSION="$VICEVERSION"
CATEGORY="emulator"
VENDOR="The VICE Team"
EMAIL="vice-devel@firenze.linux.it"
PSTAMP="Marco van den Heuvel"
BASEDIR="/usr/local"
CLASSES="none"
_END

  packagename=vice-$GUI-$VICEVERSION-$arch_version-$arch_cpu-local

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
