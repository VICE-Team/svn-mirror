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

mv src/plus4/Makefile.in src/plus4/Makefile.in.orig
sed s/"-rm -f libplus4.a"/"-rm -f -r libplus4.a;cp plus4mem.o ..\/;mv plus4mem.c plus4mem2.c;echo >plus4mem.c;$\(CC\) -c -o plus4mem.o plus4mem.c;mv plus4mem2.c plus4mem.c"/ <src/plus4/Makefile.in.orig >src/plus4/Makefile.in
mv src/Makefile.in src/Makefile.in.orig
sed s/"@XPLUS4_WINRES@"/"plus4mem.o"/ <src/Makefile.in.orig >src/Makefile.in
./configure -v --prefix=/usr/local cross_compiling=yes ac_cv_c_bigendian=no
make
