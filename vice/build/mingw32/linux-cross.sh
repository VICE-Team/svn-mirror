#!/bin/sh
#
# This is just an example how to configure VICE for cross compiling.
# 
# The script has to be called from the top level directory:
# ./build/mingw32/linux-cross.sh
#
( cd . \
&& echo aclocal; aclocal \
&& echo automake; automake --gnu Makefile \
&& echo autoconf; autoconf )
echo configure
CFLAGS="-Wall -mcpu=pentium -O5 -fomit-frame-pointer" CXXFLAGS="-Wall -mcpu=pentium -O5 -fomit-frame-pointer -fno-exceptions" CXX="/opt/mingw32/bin/i386-mingw32-c++" CC="/opt/mingw32/bin/i386-mingw32-gcc" AR="/opt/mingw32/i386-mingw32/bin/ar" RANLIB="/opt/mingw32/i386-mingw32/bin/ranlib" WINDRES="/opt/mingw32/bin/i386-mingw32-windres" ./configure --enable-ffmpeg --host=i386-mingw32 $*
