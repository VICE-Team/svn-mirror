# make-ub.sh
#
# build a universal binary of vice for macs
#
# written by Christian Vogelgsang <chris@vogelgsang.org>
#
# call this from vice top-level directory

echo "--- Build Mac Universal Binary Distribution ---"

# check for vice version in configure.in
if [ ! -e configure.in ]; then
  echo "please run script from inside VICE source base directory!"
  exit 1
fi
# fetch vice version
eval `grep 'VICE_VERSION_[A-Z]*=' configure.in`
if test x"$VICE_VERSION_BUILD" = "x" -o x"$VICE_VERSION_BUILD" = "x0" ; then
  VICE_VERSION=$VICE_VERSION_MAJOR"."$VICE_VERSION_MINOR
else
  VICE_VERSION=$VICE_VERSION_MAJOR"."$VICE_VERSION_MINOR"."$VICE_VERSION_BUILD
fi

# -- config --
READLINE_SRC=`pwd`/../readline-5.1
HIDUTIL_SRC=`pwd`/../HIDUtilitiesSource
VICE_SRC=`pwd`
BUILD_DIR=UB

COMMON_CFLAGS=-O3
PPC_SDK=/Developer/SDKs/MacOSX10.3.9.sdk
INTEL_SDK=/Developer/SDKs/MacOSX10.4u.sdk

# -- fetch real paths --
if [ ! -d $READLINE_SRC ]; then
	echo "ERROR missing readline source!"
	exit 1
fi
if [ ! -d $HIDUTIL_SRC ]; then
	echo "ERROR missing HIDUtil source!"
	exit 1
fi
echo "readline: $READLINE_SRC"
echo "hituil:   $HIDUTIL_SRC"

# -- setup build dir --
mkdir -p $BUILD_DIR
cd $BUILD_DIR ; BUILD_DIR=`pwd`
echo "build:    $BUILD_DIR"
if [ ! -d $BUILD_DIR ]; then
	echo "ERROR creating BUILD_DIR=$BUILD_DIR"
	exit 1
fi
BUILD_INTEL=$BUILD_DIR/intel
BUILD_PPC=$BUILD_DIR/ppc
BUILD_INTEL_TEMP=$BUILD_DIR/intel/temp
BUILD_PPC_TEMP=$BUILD_DIR/ppc/temp
mkdir -p $BUILD_INTEL/include
mkdir -p $BUILD_INTEL/lib
mkdir -p $BUILD_PPC/include
mkdir -p $BUILD_PPC/lib
mkdir -p $BUILD_INTEL_TEMP
mkdir -p $BUILD_PPC_TEMP
mkdir -p $BUILD_DIR/src

cd $BUILD_PPC_TEMP
rm -rf *

# -- readline (currently only required for powerpc on 10.3) --
if [ ! -e $BUILD_PPC/lib/libreadline.a ]; then
	echo "--- Readline (ppc) ---"
	env CC="gcc -arch ppc -isysroot $PPC_SDK" \
		  LD="gcc -arch ppc -isysroot $PPC_SDK" \
			$READLINE_SRC/configure --prefix=$BUILD_PPC --disable-shared
	make
	make install
	rm -rf *
fi

# -- HIDUtils --
HIDUTIL_SRC_FILES="HID_Config_Utilities.c HID_Error_Handler.c HID_Name_Lookup.c \
	HID_Queue_Utilities.c HID_Transaction_Utilities.c HID_Utilities.c"
if [ ! -e $BUILD_PPC/lib/libHIDUtilities.a ]; then
	cd $BUILD_PPC_TEMP
	rm -rf *
	echo "-- HIDUtilities (ppc) --"
	for src in $HIDUTIL_SRC_FILES ; do
		echo "compiling $src (ppc)"
		gcc -arch ppc -isysroot $PPC_SDK -O3 -W -c $HIDUTIL_SRC/$src 
	done
	ar cr  $BUILD_PPC/lib/libHIDUtilities.a *.o
	ranlib $BUILD_PPC/lib/libHIDUtilities.a
	cp $HIDUTIL_SRC/HID_Utilities_External.h $BUILD_PPC/include
	rm -rf *
fi
if [ ! -e $BUILD_INTEL/lib/libHIDUtilities.a ]; then
	cd $BUILD_INTEL_TEMP
	rm -rf *
	echo "-- HIDUtilities (intel) --"
	for src in $HIDUTIL_SRC_FILES ; do
		echo "compiling $src (ppc)"
		gcc -arch i386 -isysroot $INTEL_SDK -O3 -W -c $HIDUTIL_SRC/$src
	done
	ar cr  $BUILD_INTEL/lib/libHIDUtilities.a *.o
	ranlib $BUILD_INTEL/lib/libHIDUtilities.a
	cp $HIDUTIL_SRC/HID_Utilities_External.h $BUILD_INTEL/include
	rm -rf *
fi

# -- VICE --
export PATH=/usr/X11R6/bin:$PATH
if [ ! -e $BUILD_PPC/bin/x64 ]; then
	cd $BUILD_PPC_TEMP
	echo "-- VICE (ppc) --"
	set -x
	env \
		CPPFLAGS="-I$BUILD_PPC/include" \
		CFLAGS="-I$BUILD_PPC/include $COMMON_CFLAGS" \
		LDFLAGS="-L$BUILD_PPC/lib" \
		CC="gcc -arch ppc -isysroot $PPC_SDK" \
		CXX="g++ -arch ppc -isysroot $PPC_SDK" \
		LD="gcc -arch ppc -isysroot $PPC_SDK" \
		$VICE_SRC/configure --prefix=$BUILD_PPC
	set +x
	make
	make install
fi
if [ ! -e $BUILD_INTEL/bin/x64 ]; then
	cd $BUILD_INTEL_TEMP
	echo "-- VICE (intel) --"
	set -x
	env \
		CPPFLAGS="-I$BUILD_INTEL/include" \
		CFLAGS="-I$BUILD_INTEL/include $COMMON_CFLAGS" \
		LDFLAGS="-L$BUILD_INTEL/lib" \
		CC="gcc -arch i386 -isysroot $INTEL_SDK" \
		CXX="g++ -arch i386 -isysroot $INTEL_SDK" \
		LD="gcc -arch i386 -isysroot $INTEL_SDK" \
		$VICE_SRC/configure --prefix=$BUILD_INTEL
	set +x
	make
	make install
fi

# combine ppc and i386 compiles into a universal binary
if [ ! -e $BUILD_DIR/src/x64 ]; then
	echo "-- combining apps --"
	APPS="c1541 cartconv petcat x128 x64 xcbm2 xpet xplus4 xvic"
	for app in $APPS ; do
		echo "combining $app"
		lipo -create -output $BUILD_DIR/src/$app $BUILD_PPC/bin/$app $BUILD_INTEL/bin/$app	
	done
fi

# call make-bindist to bundle everything
cd $BUILD_DIR
$SHELL $VICE_SRC/src/arch/unix/macosx/make-bindist.sh $VICE_SRC strip $VICE_VERSION zip

echo "--- Ready ---"
