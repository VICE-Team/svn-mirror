# Creating a cross-build system for building Gtk3-Windows on Linux

**WARNING**: Work in progress, do not use these instructions yet!
             You're better off using the 'GTK3-debian-frankenvice.md' document.


## Test machine setup details

I'll be using a Debian 10.0 64-bit VM, and the target is to create a cross-build
system for 64-bit Windows. I choose Debian since that is what pokefinder uses,
it is widely used, I'm familiar with it, and our other build instructions use
Debian as well. I'll be using 'vice' as the username for both root and the
default user, passwords for both will also be 'vice', to keep thing simple.


I used the netinstall CD image to install Debian:
<https://cdimage.debian.org/debian-cd/current/amd64/iso-cd/debian-10.0.0-amd64-netinst.iso>

When the installer asks for the 'Software selection', select only 'SSH server'
and 'standard system utilities', nothing else.
We want a headless box so all this can (hopefully) be reproduced on the
pokefinder server.

When the installer asks to reboot, do so.

After reboot log in as the normal user, and do `su`.

Update any packages:

$ apt update
$ apt upgrade

Reboot if required (ie kernal update or so)


### Enable sudo

Debian 10.0 no longer updates $PATH when using `su` which means some binaries
in /sbin may not be found, such as `shutdown` and `reboot`, which is annoying,
to say the least.

So as the normal user:

$ su
$ apt install sudo

We don't have /sbin in our path, so we need to do this:
$ /sbin/adduser vice sudo

Now log out from the root shell AND the normal user shell and log back in as
the normal user for the sudoers changes to take effect.



### Installing required packages on the VM

We need quite a few packages on the VM:
(Unfortunately I forgot to document the packages I installed)

* autoconf
* automake
* build-essential
* pkg-config
* byacc
* flex
* gettext (for Atk)
* texinfo
* texlive-fonts-recommended
* mingw-w64
* mingw-w64-tools
* subversion
* git
* curl
* xa65
* python3-pip
* meson + ninja (a build system used by Gnome/Gtk, Python based)

    Do not use apt, but use pip3:

    $ sudo pip3 install meson ninja

(Perhaps more, we'll get to it when we hit a problem)


#### Fix pkg-config for Windows cross-building

Currently (2019-07-14), Debian has an issue with running pkg-config for Windows
cross-builds. Whenever you run into a message like:

    Please install dpkg-dev to use pkg-config when cross-building

Don't bother installing dpkg-dev, it's already installed and reinstalling won't
fix anything.

Edit `/usr/share/pkg-config-crosswrapper` and comment out the 'if [ "$?" != 0 ]
branch after the multiarch="...." line (line 13 on my box).





#### Optional packages

* A decent text editor, I'm using Vim:
$ apt install vim

* A text mode web browser, I use links2, had good experience with it when
  setting up Gentoo:
$ apt install links2


##### VirtualBox guest additions

Install dmks (this will also install the kernal headers needed):

$ apt install dkms

then in the VBox UI select "Devices" -> "Insert Guest Additions Cd Image ...".
On my VM the CD didn't automount, so (as root) mount the CD with `mount /dev/cdrom /media/cdrom`
Then run
$ sudo sh /media/cdrom/VBoxLinuxAdditions.run

Reboot the VM to make the guest additions work properly,


### Building Gtk and related packages

The following instructions assume using the home directory as the target for downloads and extracted tarballs. Also, unless otherwise notified, the "normal" user (ie non-root) should be used.

I'll be using the /usr/x86_64-w64-mingw32 prefix used by mingw to install all
cross-built stuff.


**TODO**: Add instructions on cross-building and installing libtool!!



#### zlib

```
$ wget https://zlib.net/zlib-1.2.11.tar.gz
$ tar -xvzf zlib-1.2.11.tar.gz
$ cd zlib-1.2.11.tar.gz
```

Stole this from <https://wiki.openttd.org/Cross-compiling_for_Windows#zlib> and
altered it for this guide.

```
$ sed -e s/"PREFIX ="/"PREFIX = x86_64-w64-mingw32-"/ -i win32/Makefile.gcc
$ make -f win32/Makefile.gcc
$ sudo BINARY_PATH=/usr/x86_64-w64-mingw32/bin \
    INCLUDE_PATH=/usr/x86_64-w64-mingw32/include \
    LIBRARY_PATH=/usr/x86_64-w64-mingw32/lib \
    make -f win32/Makefile.gcc install SHARED_MODE=1
```

#### libpng

```
$ wget https://downloads.sourceforge.net/project/libpng/libpng16/1.6.37/libpng-1.6.37.tar.gz
$ cd libpng-1.6.3
$ ./configure --prefix=/usr/x86_64-w64-mingw32 \
              --host=x86_64-w64-mingw32 \
              --enable-static \
              --enable-shared
$ make
$ sudo make install
```

[no errors whatsoever, which is nice]

**For most of the remaing libs, it might be a good idea to look at Fedora's
build scripts**


#### GLib

```
$ wget https://download.gnome.org/sources/glib/2.61/glib-2.61.1.tar.xz
$ tar -vxf glib-2.61.1.tar.xz
$ cd ~/glib-2.61.1.1/
```

I'll be following these instructions to cross-compile GLib:
<https://developer.gnome.org/glib/stable/glib-cross-compiling.html>
And I'll be using the default /usr/local prefix.
(Note that thate page uses an incorrect command line argument for meson:
 it uses --cross_file, while Meson uses --cross-file)

Since Meson/Ninja need user-created config files, I'll be creating a directory
~/config-files/ where I'll store those files, so I can package those files later,
should I wish to.


Create a ~/config-files/glib2-cross-file.txt file as mentioned on that page, and run:

```
$ meson --prefix /usr/x86_64-w64-mingw32 --cross-file ~/config-files/glib2-cross-file.txt builddir
```

After that build the library:
```
$ ninja -C builddir
```

And install it:
```
$ sudo ninja -C builddir install
```


##### Testing the Glib install

For test compiles I'll use a directory ~/test-files

So cd to ~/test-files and paste this into "test-glib-win64.c":

```C
#include <glib.h>
#include <windows.h>

int WinMain(HINSTANCE hInstance,
            HINSTANCE hPrevInstance,
            LPSTR     lpCommandLine,
            int       nShowCommand)
{
        g_print("WinMain() called!\n");
        return 0;
}
```

And run:
```
$ x86_64-w64-mingw32-gcc `x86_64-w64-mingw32-pkg-config --cflags glib-2.0` \
        test-glib-win64.c `x86_64-w64-mingw32-pkg-config --libs glib-2.0`
```

This should build an a.exe which prints "WinMain() called!"
(you'll need a Windows box for this, so scp "a.exe" to the Windows box and run
 a.exe on that.)





#### Atk (Doesn't work on Windows, so hopefully we can skip this)

Download and extract Atk 2.33.3:

```
$ wget https://download.gnome.org/sources/atk/2.33/atk-2.33.3.tar.xz
$ tar -xvf atk-2.33.3.tar.xz
$ cd atk-2-33.3
```

Copy over the Meson cross-file we used for GLib:
```
$ cp ~/config-files/glib2-cross-file.txt ~/config-files/atk-cross-file.txt
```

Add the following lines under the [binaries] header:
```
glib-genmarshal = '/usr/x86_64-w64-mingw32/bin/glib-genmarshal'
glib-mkenums = '/usr/x86_64-w64-mingw32/bin/glib-mkenums'
```

Now configure and build Atk:
(make sure to include the `-Dintrospection` to avoid trouble later!)
```
$ meson --prefix /usr/x86_64-w64-mingw32 \
        --cross-file ~/config-files/atk-cross-file.txt \
        -Dintrospection=false \
        builddir
$ ninja -C builddir
$ sudo ninja -C builddir install
```



#### Cairo

##### Dependency: pixman

Cairo requires pixman, so let's build that first:
We also need pixman:

```
$ wget https://www.cairographics.org/releases/pixman-0.38.4.tar.gz
$ tar -xvzf pixman-0.38.4.tar.gz
$ cd ~/pixman-0.38.4
```

Copy ~/config-files/glib2-cross-file.txt to ~/config-files/pixman-cross-file.txt

Now configure, build and install pixman:
```
$ meson --prefix /usr/x86_64-w64-mingw32 \
        --cross-file ~/config-files/pixman-cross-file.txt \
        builddir
$ ninja -C builddir
$ sudo ninja -C builddir install
```


#### Dependency: freetype2

Now we need to install freetype2:

```
$ wget https://download.savannah.gnu.org/releases/freetype/freetype-2.10.1.tar.gz
$ tar -xvzf freetype-2.10.1.tar.gz
$ cd freetype-2.10.1
```

Freetype uses autotools, not Meson:

```
$ ./configure --prefix=/usr/x86_64-w64-mingw32 \
              --host=x86_64-w64-mingw32 \
              --enable-static \
              --enable-shared
$ make
$ sudo make install
```

#### Dependency: fontconfig

Cairo needs fontconfig, so let's install that now. Version numbers ending in 91-99 appear to be unstable releases, so we won't use those.


##### Dependency: libxml2

Fontconfig needs either libxml2 or expat, let's install libxml2, since it was developed for Gnome.

```
$ wget ftp://xmlsoft.org/libxml2/libxml2-2.9.9.tar.gz
$ tar -xvzf libxml2-2.9.9.tar.gz
$ cd libxml2-2.9.9
```

Configure, build and install libxml2:

libxml2 fails to link with --enable-shared (which is its default), it also appears to think it can generate Python bindings, which will also fail:

```
$ ./configure --prefix=/usr/x86_64-w64-mingw32 \
              --host=x86_64-w64-mingw32 \
              --enable-static \
              --disable-shared \
              --without-python
$ make
$ sudo make install
```


##### Dependency: gperf

Seems fontconfig also needs gperf, so we'll be installing that as well, sigh.

```
$ sudo apt install gperf
```

##### Dependency: libuuid?

Building fontconfig fails with errors building tests due to not recognizing
the `uuid_t` type.

https://sourceforge.net/projects/libuuid/files/libuuid-1.0.3.tar.gz

Aargh: `#include <sys/syscall.h>`, so not portable as the author claims.


[ **We'll skip fontconfig for now** ]


#### Build and install fontconfig

```
$ wget https://www.freedesktop.org/software/fontconfig/release/fontconfig-2.13.1.tar.gz
$ tar -xvzf fontconfig-2.13.1.tar.gz
$ cd fontconfig-2.13.1
```

```
$ ./configure --prefix=/usr/x86_64-w64-mingw32 \
              --host=x86_64-w64-mingw32 \
              --enable-static \
              --enable-shared \
              --enable-iconv \
              --disable-docs \
              --enable-libxml2
$ make
$ sudo make install
```


#### Build and install Cairo

Cairo uses the release style where even minor numbers are stable and uneven are unstable, so we'll grab the latest stable release:

```
$ wget https://cairographics.org/releases/cairo-1.16.0.tar.xz
$ tar -xvf cairo-1.16.0.tar.xz
$ cd cairo-1.16.0
```

Cairo has a large selection of configure options, including options for OpenGL, OpenGLESv2 and OpenGLESv3. Neither MSYS2 nor Fedora seem to use these in their build scripts, so we'll skip these for now.

Most of the configure switches are taken from MSYS2's PKGBUILD script. I've removed fontconfig support since that caused a lot of trouble (uuid).

```
$ ./configure --prefix=/usr/x86_64-w64-mingw32 \
              --host=x86_64-w64-mingw32 \
              --enable-win32 \
              --enable-win32-font \
              --enable-png \
              --enable-static \
              --enable-shared \
              --enable-gobject \
              --disable-xlib \
              --disable-xcb \
              --enable-ft
$ make
$ sudo make install
```

[end-of-edit 2019-07-16 16:07]




### Pango


Now we need Pango


#### Dependency: fribidi

```
$ git clone https://github.com/fribidi/fribidi.git
$ cp ~/config-files/glib2-cross-file.txt ~/config-files/fribidi-cross-file.txt
$ cd fribidi
$ meson --prefix /usr/x86_64-w64-mingw32 \
        --cross-file ~/config-files/fribidi-cross-file.txt \
        -Ddocs=false \
        builddir
$ ninja -C builddir
$ sudo ninja -C builddir install
```

#### Optional dependency: harfbuzz

Seems to not be required, but is also an optional dependency of Cairo and freetype2, so perhaps it will be useful? Skipping now.


### Cross-compile Pango

```
$ wget https://github.com/GNOME/pango/archive/1.43.0.tar.gz
$ mv 1.43.0.tar.gz pango-1.43.0.tar.gz
$ tar -xvfz pango-1.43.0.tar.gz
$ cd ~/pango-1.43.0.tar.gz
```

Copy ~/config-files/atk-cross-file.txt to ~/config-files/pango-cross-file.txt

```
$ meson --prefix /usr/x86_64-w64-mingw32 \
        --cross-file ~/config-files/pango-cross-file.txt \
        -Dgir=false \
        builddir
$ ninja -C builddir
$ sudo ninja -C builddir install
```

[ end-of-edit 2019-07-16 19:39 ]



### GDK-Pixbuf

GDK-Pixbuf is the next dependency of Gtk3.



    Looking at the meson_options.txt it seems I have to decide between using
    native-windows-loaders or linking to libpng, libtiff, libjpeg and libjpeg2000.
    I've decided to not use the native-windows-loaders.


#### Dependency: libpng

Already satisfied.

#### Dependency: libjpeg

```
$ wget https://www.ijg.org/files/jpegsrc.v9c.tar.gz
$ tar -xvzf jpegsrc.v9c.tar.gz
$ cd jpeg-9c
$ ./configure --prefix=/usr/x86_64-w64-mingw32 \
              --host=x86_64-w64-mingw32 \
              --enable-static \
              --enable-shared
$ make
$ sudo make install
```


#### Dependency; libtiff

This seems to be very old, build instructions mention MSVC on Win95-2000 or
DJGPP. I'll be skipping this.


#### Configure, build and install GDK-Pixbuf

```
$ git clone https://gitlab.gnome.org/GNOME/gdk-pixbuf.git
$ cd gdk-pixbuf
```
Once again copy ~/config-files/atk-cross-file.txt to ~/config-files/gdk-pixbuf-cross-file.txt

```
$ meson --prefix /usr/x86_64-w64-mingw32 \
        --cross-file ~/config-files/gdk-pixbuf-cross-file.txt \
        -Dpng=true \
        -Dtiff=false \
        -Djpeg=true \
        -Djasper=false \
        -Dx11=false \
        -Ddocs=false \
        -Dgir=false \
        -Dman=false \
        -Dnative_windows_loaders=false \
        -Dinstalled_tests=false \
        builddir
$ ninja -C builddir
$ sudo ninja -C builddir install
```



[ end-of-edit 2019-07-16 21:13 ]


### Gtk3

First let's try building Gtk3 without any fancy OpenGL stuff. Once we get the build system to create a Cairo-based Gtk3-Windows dist, we can tweak the VM to try to get OpenGL working.

[--old-shit--]
$ PKG_CONFIG_PATH=/opt/cross/lib/pkgconfig PATH="/opt/cross/bin:$PATH" CFLAGS="-I/opt/cross/include" LDFLAGS="-L/opt/cross/lib" ./configure --prefix=/usr/cross --host=x86_64-w64-mingw32 --disable-introspection --enable-win32-backend --enable-win32-gles --disable-wayland-backend --disable-mir-backend --disable-x11-backend

This currently fails with not finding Epoxy, a lib similar to GLEW.
[--end-old-shit--]

**TODO: add proper URL for Gtk+**
```
$ wget gtk+-3.24.10.tar.gz
$ tar -xvzf gtk+-3.24.10.tar.gz
$ cd gtk+-3.24.10

$ ./configure --prefix=/usr/x86_64-w64-mingw32 \
              --host=x86_64-w64-mingw32 \
              --disable-static \
              --enable-shared \
              --disable-x11-backend \
              --enable-win32-backend \
              --disable-quartz-backend \
              --disable-broadway-backend \
              --disable-wayland-backend \
              --disable-mir-backend \
              --disable-glibtest \
              --disable-win32-gles \
              --disable-cups \
              --disable-papi \
              --disable-cloudprint \
              --enable-introspection=no \
              --enable-gtk-doc=no \
              --enable-gtk-doc-html=no \
              --enable-gtk-doc-pdf=no \
              --enable-man=no
```

Still needs Expoxy:



#### Dependency Expoy

This is a hard requirement for Gtk3, and apparently only available via git:

```
$ git clone https://github.com/anholt/libepoxy.git
$ cd ~/libepoxy
$ cp ~/config-files/atk-cross-file.txt ~/config-files/epoxy-cross-file.txt
$ meson --prefix /usr/x86_64-w64-mingw32 \
        --cross-file ~/config-files/epoxy-cross-file.txt \
        -Ddocs=false \
        -Dglx=no \
        -Degl=yes \
        -Dx11=false \
        -Dtests=false \
        builddir
$ ninja -C builddir
$ sudo ninja -C builddir install
```

Gtk3 needs EGL support, unfortunately that requires compiling libepoxy against mesa, which in turn seems to need llvm. So I give up for now :)

[-end-of-edit 2019-07-16 22:20]


##### Dependency: llvm

$ sudo apt install cmake

```
$ wget https://releases.llvm.org/8.0.0/llvm-8.0.0.src.tar.xz
$ tar -xvf llvm-8.0.0.src.tar.xz
$ cd llvm-8.0.0.src
```

Create a ~/config-files/Toolchain-mingw32.cmake file with:

(Might need more shit, no idea)

```Cmake
SET(CMAKE_SYSTEM_NAME Windows)
SET(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
SET(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
SET(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)
SET(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32 /usr/x86_64-w64-mingw32)
```

```
$ mkdir builddir
$ cd builddir
$ cmake -DCMAKE_TOOLCHAIN_FILE=~/config-files/Toolchain-mingw32.cmake \
        -DCMAKE_CXX_STANDARD=11 ..
$ make

Fucks up about std::mutex
```

-- old shit --

$ PKG_CONFIG_PATH=/opt/cross/lib/pkgconfig PATH="/opt/cross/bin:$PATH" CFLAGS="-I/opt/cross/include" LDFLAGS="-L/opt/cross/lib" meson --prefix=/opt/cross --cross-file cross_file.txt -Dgir=false -Dman=false -Dx11=false builddir -Degl=yes
$ su
$ cd builddir
$ ninja install
$ (CTRL-D)

Fails due to not having EGL, which seems to require cross-compiling Mesa.

$ wget https://mesa.freedesktop.org/archive/mesa-19.0.3.tar.xz
$ tar -xvf mesa
$ cd mesa

Needs 'Scons',
And Scons on Debian needs Python2 (that's a lot of old shit)
(Looks like current Scons supports Python 3, but would require building Scons)

$ su
$ apt-get install python-pip
$ pip2 install mako
$ (CTRL-D)


$ cd into mesa
$ scons platform=windows toolchain=crossmingw machine=x86_64 libgl-gdi

This takes a while, but we should end up with `build/windows-x86_64-debug/gallium/targets/libgl-gdi/opengl32.dll`. Which should be copied into the Windows bindist dir when doing a bindist. (Massive TODO)


$ su
$ cp -R include/\* /opt/cross/include


Still fucks up.

