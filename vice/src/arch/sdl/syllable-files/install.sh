#!/bin/sh

current_dir=`pwd`

echo "Installing VICE as an Applaction collection."
if test x"$current_dir" != "x/usr/VICE"; then
  rm -f -r /usr/VICE
  mkdir /usr/VICE
  mkdir /usr/VICE/bin
  mkdir /usr/VICE/lib
  cp -r bin/* /usr/VICE/bin
  cp -r lib/* /usr/VICE/lib
fi
rm -f -r /Applications/VICE
mkdir /Applications/VICE
cp x*.sh /Applications/VICE
cp uninstall.sh /Applications/VICE
if [ -e /usr/VICE/bin/x64sc ]
then
  rm -f /Applications/VICE/x64sh.sh
fi
echo "VICE has been installed and can be started from the desktop Applications item."
echo "The installation directory can now be deleted if needed."
