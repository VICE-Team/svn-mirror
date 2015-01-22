#!/bin/sh

# see if we are in the top of the tree
if [ ! -f configure.proto ]; then
  cd ../..
  if [ ! -f configure.proto ]; then
    echo "please run this script from the base of the VICE directory"
    echo "or from the appropriate build directory"
    exit 1
  fi
fi

curdir=`pwd`

CC="/bin/sh $curdir/src/arch/beos/ppc-cc.sh" CXX="/bin/sh $curdir/src/arch/beos/ppc-cc.sh" RANLIB=true STRIP=true AR="/bin/sh $curdir/src/arch/beos/ppc-ar.sh" CFLAGS="-inclpaths nosys" LDFLAGS="-L/boot/develop/lib/ppc /boot/develop/lib/ppc/glue-noinit.a /boot/develop/lib/ppc/init_term_dyn.o /boot/develop/lib/ppc/start_dyn.o -lbe -lroot" ./configure --host=ppc-beos --enable-sdl --enable-native-tools
make
make bindist
