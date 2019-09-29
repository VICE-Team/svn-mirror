# FrankenVICE

## Creating a Gtk3 Window cross compiler on Debian with Fedora packages

All this assumes a 64-bit box and also a 64-bit target when compiling.


### Prepare Debian box

Make sure you have a Debian box with basic development tools installed:

```sh
$ su
$ apt install autoconf autotools build-essential byacc flex git subversion \
        vim xa65 alien
```
(todo: probably a lot more)

Do **not** install any native Linux Gtk/GLib packages unless specifically told to do so, this might result in unwanted results.



#### Install Debian's mingw packages

```sh
$ su
$ apt install mingw-w64 mingw-w64-tools
```

Hint: Debian mingw packages are called 'mingw-w64-\*" while Fedora mingw packages are called 'mingw64-\*'. This will come in handy later to see which packages came from Debian and which were installed via RPM's using 'alien'.


#### Install Fedora packages

Since Debian doesn't provide any Gtk/GLib packages for cross-compiling to Windows, we'll be using Fedora packages. At the moment of writing I've been using Fedora 30 packages.

These are the packages I had to use:

mingw64-adwaita-icon-theme
mingw64-atk
mingw64-bzip2
mingw64-cairo
mingw64-expat
mingw64-fontconfig
mingw64-freeglut
mingw64-freetype
mingw64-gdk-pixbuf
mingw64-gettext
mingw64-giflib
mingw64-glew
mingw64-glib2
mingw64-gtk-update-icon-cache
mingw64-gtk3
mingw64-harfbuzz
mingw64-icu
mingw64-jasper
mingw64-libcroco
mingw64-libepoxy
mingw64-libffi
mingw64-libjpeg-turbo
mingw64-libpng
mingw64-librsvg2
mingw64-libxml2
mingw64-pango
mingw64-pcre
mingw64-pixman
mingw64-sdl2
mingw64-termcap
mingw64-win-iconv
mingw64-winpthreads
mingw64-zlib


I used `~/rpm` and `~/deb` to store these files, so converting to .deb would go like this:
```sh
$ cd ~
$ su
$ for f in rpm/*.rpm; do alien --to-deb $f; done
$ mv *.deb deb/
$ Ctrl+D
```

Check rpm/ and deb/ dirs to check if all got converted okayish.
If okay, install:
```sh
$ su
$ for f in deb/*.deb; do dpkg -i $f; done
```


### Now what?

Two ways to go about this: properly use env vars etc to make configure and make recognize the files from the Fedora packages, or brute-force this.

I ran into a lot of trouble trying to do the 'proper' thing, so I brute-forced my way to a bindist.

#### Brute force

Copy all files in `/usr/x86_64-w64-mingw32/sys-root/mingw/{bin,etc,include,lib,share} to `/usr/x86-64-w64-wingw32/`.

This should cause `./configure --enable-native-gtk3ui` to pass, but compiling/linking will break due to certain glib tools.

(Don't install native glib2-tools, that'll pull in about 500 packages, including X11)

