# Creating a cross-build system for building Gtk3-Windows on Linux

**WARNING**: Work in progress, do not use these instructions yet!


## Test machine setup details

I'll be using a Debian 9.8 64-bit VM, and the target is to create a cross-build
system for 64-bit Windows. I choose Debian since that is what pokefinder uses,
it is widely used, I'm familiar with it, and our other build instructions use
Debian as well.

I used the netinstall CD image to install Debian:
<https://cdimage.debian.org/debian-cd/current/amd64/iso-cd/debian-9.8.0-amd64-netinst.iso>

When the installer runs dselect(?), select only 'base system' and 'ssh server'.
We want a headless box so all this can (hopefully) be reproduced on the pokefinder
server.


### Installing required packages on the VM

We need quite a few packages on the VM:
(Unfortunately I forgot to document the packages I installed)

* autoconf
* automake
* build-essential
* pkg-config
* byacc
* flex
* texinfo
* texlive-\* (?) (for pdf)
* mingw-w64
* mingw-x64-tools
* meson (a build system used by GLib, Python based)

    Do not use apt, but use pip (as root):

    $ apt-get install python3-pip
    $ pip3 install meson ninja

(lots more, I forgot)

* A decent text editor, I'm using Vim


* gettext (required by Atk, not sure if need to build a Windows version)
* curl


#### Optional packages

links, a text-based web browser to download source packages, but wget will
probably do fine.

Virtualbox Guest Additions

Install dmks, then in the VBox UI select "Devices" -> "Insert Guest Additions Cd Image ...".
On my VM the CD didn't automount, so (as root) mount the CD with `mount /dev/cdrom /media/cdrom`
Then (as root), run `sh /media/cdrom/VBoxLinuxAdditions.run`, this will install
the guest additions kernel modules (takes a while), so we can use "shared folders" and shared/bidirection clipboards. The VM's framebuffer will also become a bit larger.

Reboot the VM to make the guest additions work properly.


### Building Gtk and related packages

I decided to use /opt/cross as the target of the cross-compiled packages. No
idea if that's clever, we'll have to see. But at least it keeps all the Windows
stuff removed from the Linux stuff, and makes for easier packaging later, if
required.

The following instructions assume using the home directory as the target for downloads and extracted tarballs. Also, unless otherwise notified, the "normal" user (ie non-root) should be used.


#### GLib

Download and extract <https://download.gnome.org/sources/glib/2.60/glib-2.60.0.tar.xz>

I'll be following these instructions to cross-compile GLib:
<https://developer.gnome.org/glib/stable/glib-cross-compiling.html>

So, create a cross_build.txt file in the glib dirm and then run:
$ meson --prefix=/opt/cross --cross-file cross_file.txt builddir

After that, cd to builddir/ and use ninja to install (as root):
$ ninja install

This will install the Glib-2.0 files into /opt/cross/usr/local/


##### Testing the Glib instal

[[TODO: add test source file]]


$ PKG_CONFIG_PATH=/opt/cross/lib/pkgconfig x86_64-w64-mingw32-gcc `x86_64-w64-mingw32-pkg-config --libs --cflags glib-2.0` -lmingw32 test-glib-win32.c -lglib-2.0 -L/opt/cross/lib/glib-2.0

This should build an a.exe which prints "WinMain called".

#### Atk

Download and extract <https://download.gnome.org/sources/atk/2.32/atk-2.32.0.tar.xz>

$ cd atk-2-32
$ cp ~/glib-2.58.2/cross_file.txt ,

Shit, Atk depends on xgettext

$ meson --prefix=/opt/cross --cross-file cross_file.txt
$ cd builddir
(as root)
$ ninja install


Running configure in gtk3-3.24.7 does no longer complaing about missing GLib
or Atk.


#### Cairo


First we need to install libpng

$ wget https://downloads.sourceforge.net/project/libpng/libpng16/1.6.36/libpng-1.6.36.tar.gz
$ cd libpng-1.6.36
$ PKG_CONFIG_PATH=/opt/cross/lib/pkgconfig CPPFLAGS="-I/opt/cross/include" LDFLAGS="-L/opt/cross/bin" ./configure --prefix=/opt/cross --host=x86_64-w64-mingw32

$ make
(libtools spits out a lot of errors, we'll see if that has consequences)

(as root)
$ make install

We also need pixman:

$ wget https://www.cairographics.org/releases/pixman-0.38.2.tar.gz

unpack and cd 
$ PKG_CONFIG_PATH=/opt/cross/lib/pkgconfig CPPFLAGS="-I/opt/cross/include" LDFLAGS="-L/opt/cross/bin" ./configure --prefix=/opt/cross --host=x86_64-w64-mingw32
$ make


we need libfreetype2:
https://download.savannah.gnu.org/releases/freetype/freetype-2.10.0.tar.gz


Let's try Cairo now:

$ PKG_CONFIG_PATH=/opt/cross/lib/pkgconfig CPPFLAGS="-I/opt/cross/include" LDFLAGS="-L/opt/cross/lib" ./configure --prefix=/opt/cross --host=x86_64-w64-mingw32


Works.



Now we need Pango
$ wget https://github.com/GNOME/pango/archive/1.43.0.tar.gz
$ mv 1.43.0.tar.gz pango-1.43.0.tar.gz
$ tar -xvfz pango-1.43.0.tar.gz
$ cd ~/pango-1.43.0.tar.gz

Crap! Another Meson build thing.



Pango needs PATH=/opt/cross/bin:$PATH to find a few glib bins, but it also seems
to need gobject-introspection.

$ wget https://github.com/GNOME/gobject-introspection/archive/1.60.1.tar.gz
$ mv 1.60.1.tar.gz gobject-introspection-1.60.1.tar.gz
$ tar -xvzf gobject-introspection-1.60.1.tar.gz

(gobject-introspection needs bison, so (as root) apt-get install bison)


As with the other Meson-based projecs, copy ~/glib-2.0/cross_file.txt
And watch it fail with a very mysterious message


