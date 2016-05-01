#!/bin/sh

# see if we are in the top of the tree
if [ ! -f configure.proto ]; then
  cd ../..
  if [ ! -f configure.proto ]; then
    echo "please run this script from the base of the VICE directory"
    exit 1
  fi
fi

curdir=`pwd`

build_i386=no
build_m68k=no
build_hppa=no
build_sparc=no

if test x"$1" = "xi386" -o x"$2" = "xi386" -o x"$3" = "xi386" -o x"$4" = "xi386"; then
  build_i386=yes
fi

if test x"$1" = "xm68k" -o x"$2" = "xm68k" -o x"$3" = "xm68k" -o x"$4" = "xm68k"; then
  build_m68k=yes
fi

if test x"$1" = "xhppa" -o x"$2" = "xhppa" -o x"$3" = "xhppa" -o x"$4" = "xhppa"; then
  build_hppa=yes
fi

if test x"$1" = "xsparc" -o x"$2" = "xsparc" -o x"$3" = "xsparc" -o x"$4" = "xsparc"; then
  build_sparc=yes
fi

if test x"$build_i386" = "xno" -a x"$build_m68k" = "xno" -a x"$build_hppa" = "xno" -a x"$build_sparc" = "xno"; then
  echo "At least one platform needs to be selected for the build"
  echo "Usage: build.sh [i386] [m68k] [hppa] [sparc]"
  exit 1
fi

makedone=no
single_build=no

mkdir bins

if test x"$build_i386" = "xyes"; then
  CFLAGS="-arch i386" ./configure -v --host=i386-next-nextstep --prefix=/usr/local -disable-nls --without-resid --with-xaw3d --enable-native-tools ac_cv_c_bigendian=no
  make
  makedone=yes
  if [ ! -e src/x64 -o ! -e src/xscpu64 -o ! -e src/x64dtv -o ! -e src/x128 -o ! -e src/xvic -o ! -e src/xpet -o ! -e src/xplus4 -o ! -e src/xcbm2 -o ! -e src/xcbm5x0 -o ! -e src/c1541 -o ! -e src/petcat -o ! -e src/cartconv ]
  then
    echo Error: One \(or more\) i386 binaries missing
    exit 1
  fi
  if test x"$build_m68k" = "xyes" -o x"$build_hppa" = "xyes" -o x"$build_sparc" = "xyes"; then
    for i in x128 x64 xscpu64 x64dtv xcbm2 xcbm5x0 xpet xplus4 xvic c1541 cartconv petcat
    do
      strip src/$i
      mv src/$i bins/$i.i386
    done
  else
    single_build=yes
  fi
fi

if test x"$build_m68k" = "xyes"; then
  if test x"$makedone" = "xyes"; then
    make clean
  fi
  CFLAGS="-arch m68k" ./configure -v --host=m68k-next-nextstep --prefix=/usr/local -disable-nls --without-resid --with-xaw3d --enable-native-tools ac_cv_c_bigendian=yes
  make
  makedone=yes
  if [ ! -e src/x64 -o ! -e src/xscpu64 -o ! -e src/x64dtv -o ! -e src/x128 -o ! -e src/xvic -o ! -e src/xpet -o ! -e src/xplus4 -o ! -e src/xcbm2 -o ! -e src/xcbm5x0 -o ! -e src/c1541 -o ! -e src/petcat -o ! -e src/cartconv ]
  then
    echo Error: One \(or more\) m68k binaries missing
    exit 1
  fi
  if test x"$build_i386" = "xyes" -o x"$build_hppa" = "xyes" -o x"$build_sparc" = "xyes"; then
    for i in x128 x64 xscpu64 x64dtv xcbm2 xcbm5x0 xpet xplus4 xvic c1541 cartconv petcat
    do
      strip src/$i
      mv src/$i bins/$i.m68k
    done
  else
    single_build=yes
  fi
fi

if test x"$build_hppa" = "xyes"; then
  if test x"$makedone" = "xyes"; then
    make clean
  fi
  CFLAGS="-arch hppa" ./configure -v --host=hppa-next-nextstep --prefix=/usr/local -disable-nls --without-resid --with-xaw3d --enable-native-tools ac_cv_c_bigendian=yes
  make
  makedone=yes
  if [ ! -e src/x64 -o ! -e src/xscpu64 -o ! -e src/x64dtv -o ! -e src/x128 -o ! -e src/xvic -o ! -e src/xpet -o ! -e src/xplus4 -o ! -e src/xcbm2 -o ! -e src/xcbm5x0 -o ! -e src/c1541 -o ! -e src/petcat -o ! -e src/cartconv ]
  then
    echo Error: One \(or more\) hppa binaries missing
    exit 1
  fi
  if test x"$build_i386" = "xyes" -o x"$build_m68k" = "xyes" -o x"$build_sparc" = "xyes"; then
    for i in x128 x64 xscpu64 x64dtv xcbm2 xcbm5x0 xpet xplus4 xvic c1541 cartconv petcat
    do
      strip src/$i
      mv src/$i bins/$i.hppa
    done
  else
    single_build=yes
  fi
fi

if test x"$build_sparc" = "xyes"; then
  if test x"$makedone" = "xyes"; then
    make clean
  fi
  CFLAGS="-arch sparc" ./configure -v --host=sparc-next-nextstep --prefix=/usr/local -disable-nls --without-resid --with-xaw3d --enable-native-tools ac_cv_c_bigendian=yes
  make
  if [ ! -e src/x64 -o ! -e src/xscpu64 -o ! -e src/x64dtv -o ! -e src/x128 -o ! -e src/xvic -o ! -e src/xpet -o ! -e src/xplus4 -o ! -e src/xcbm2 -o ! -e src/xcbm5x0 -o ! -e src/c1541 -o ! -e src/petcat -o ! -e src/cartconv ]
  then
    echo Error: One \(or more\) sparc binaries missing
    exit 1
  fi
  if test x"$build_i386" = "xyes" -o x"$build_m68k" = "xyes" -o x"$build_hppa" = "xyes"; then
    for i in x128 x64 xscpu64 x64dtv xcbm2 xcbm5x0 xpet xplus4 xvic c1541 cartconv petcat
    do
      strip src/$i
      mv src/$i bins/$i.sparc
    done
  else
    single_build=yes
  fi
fi

if test x"$single_build" = "xno"; then

  for i in x128 x64 xscpu64 x64dtv xcbm2 xcbm5x0 xpet xplus4 xvic c1541 cartconv petcat
  do
    binaries=""

    if test x"$build_i386" = "xyes"; then
        binaries="bins/$i.i386"
    fi

    if test x"$build_m68k" = "xyes"; then
        binaries="$binaries bins/$i.m68k"
    fi

    if test x"$build_hppa" = "xyes"; then
        binaries="$binaries bins/$i.hppa"
    fi

    if test x"$build_sparc" = "xyes"; then
        binaries="$binaries bins/$i.sparc"
    fi

    lipo $binaries -output src/$i -create

  done
fi
echo "now do a 'gnumake -e prefix=$curdir/VICE-$VICEVERSION/usr/local  VICEDIR=$curdir/VICE-$VICEVERSION/usr/local/lib/vice install'"
echo "and then a 'make bindistzip' to build the package."
