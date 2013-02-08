#!/bin/sh
#
# build a minix 3.x binary package
#
# written by Marco van den Heuvel <blackystardust68@yahoo.com>
#
# this script needs to be run from the top of the vice tree

echo "Generating Minix-3.x binary archive"

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
curdirbase=`basename $curdir`
rm -f SDL.build

cd ..

binpackage $curdirbase .

files=""

bzip2 -d $curdirbase.tar.bz2
for i in `tar -tf $curdirbase.tar`
do
  checkfile="${i:0:10}"
  if test x"$checkfile" != "x/usr/local"; then
    if test x"$i" != "x.minixpackage"; then
      files="$files --delete $i"
    fi
  fi
done
tar $files -f $curdirbase.tar
bzip2 $curdirbase.tar

echo Minix-3.x package generated as ../$curdirbase.tar.bz2
