#!/bin/sh

# see if we are in the top of the tree
if [ ! -f configure.in ]; then
  cd ../..
  if [ ! -f configure.in ]; then
    echo "please run this script from the base of the VICE directory"
    echo "or from the appropriate build directory"
    exit 1
  fi
fi

curdir=`pwd`

CC=mwccppc CXX=mwccppc RANLIB=true STRIP=true AR="/bin/sh $curdir/src/arch/beos/ppc-ar.sh" CFLAGS="-inclpaths nosys"LDFLAGS="-L/boot/develop/lib/ppc /boot/develop/lib/ppc/glue-noinit.a /boot/develop/lib/ppc/init_term_dyn.o /boot/develop/lib/ppc/start_dyn.o -lbe -lroot" ./configure --host=ppc-beos
make
make bindist
