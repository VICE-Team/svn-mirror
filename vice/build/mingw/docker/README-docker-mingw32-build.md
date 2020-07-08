#   File:		README-docker-mingw32-build.md
#   Date:		Wed Jul  8 23:24:04 2020
#   Author:		pottendo (pottendo)

A simple setup to for a docker-supported host to build vice cross for
win32/64 bit using the mingw64 toolchain and fedora cross package. 
It has been tested on Ubuntu 20.04

Note: currently **this produces vice-binaries which don't work**, as the
gtk+ mingw32 libs shipped by fedora aren't compatible with the current
vice gtk3 backends. Today msys2 libraries are known to work.
(you therefore say, this is useless - well, you're correct! Let's
hope future fedora or vice releases fix that issue!)

Usage:
Follow instructions to install docker on your host here:
  https://docs.docker.com/engine/install

Note the following commands/scripts assume you've installed a docker
group and added your user to this group (no sudo prefix is required
then)

Setup your container:
  docker build --tag vice-buildcontainer:0.2 .
  
Start the build process:
  ./doch-run.sh <vice-checkout-directory>

The parameter shall point to the directory you've checked out vice
from sourceforge. Normally this contains two files:
"svn-instructions.txt" and the directory "vice"

If all went well, you'll find packaged binary distro (.zip) for
win32/64bit under <vice-checkout-directory>/docker-build

TODO: fix permissions to not use 'root' for docker-build directory
