#   File:		README-docker-mingw32-build.md
#   Date:		Thu Aug  3 22:26:28 2023
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
  docker build --tag vice-buildcontainer:0.3 .
  
Start the build process:
  ./dock-run.sh [-i] [-su] vice-checkout-directory

The parameter shall point to the directory you've checked out vice
from sourceforge. Normally this contains two files:
"svn-instructions.txt" and the directory "vice"

if option -i is given, the container is run interactively and vice is
not built. This is useful to check different configure options, or
adding of missing Fedora packages.

if option -su is given, the user within the container runs with super
user privileges. This is useful in conjunction with -i, if you want to
check and install missing fedora packages. Ultimately missing packages
need to be added to the install commands in 'Dockerfile' .

Currently the build takes the 'shared' configure options but removes a
few as the respective mingw prebuilt packages are not (yet) available
on Fedora. These are:
  --with-unzip-bin
  --enable-ethernet
  --with-lame
  --with-vorbis
  --with-gif

The host system is --host=x86_64-w64-mingw32

If all went well, you'll find packaged binary distro (.zip) for
win32/64bit under vice-checkout-directory/docker-build

