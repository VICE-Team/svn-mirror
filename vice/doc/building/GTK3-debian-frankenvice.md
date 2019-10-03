# FrankenVICE

## Creating a Gtk3 Window cross compiler on Debian with Fedora packages

All this assumes a 64-bit box and also a 64-bit target when compiling.


### Prepare Debian box

#### Setting up a VM

Use Netinstall ISO.

I use English (simplified) as the language and Europe/Amsterdam as the region.
Keymap is also English (simplified).

Hostname: frankenvice

Username/pwd, root password: all 'vice'. (No hacker expects us to be this stupid)

When asked about packages (dselect), select only 'SSH server' and 'standard system utilities', we'll be installing anything we need manually.

Now click 'continue' until done (GRUB on MBR etc)

**REBOOT**




Make sure you have the basic development tools:

```sh
$ su
$ apt install autoconf autotools build-essential byacc flex git subversion \
        vim xa65 alien p7zip-full zip texinfo gawk* zip unzip
```
(todo: probably a lot more
 Seems glib-compile-schemas and glib-compile-resources live in Debian's
 libglib-2.0)

Do **not** install any native Linux Gtk/GLib packages unless specifically told to do so, this might result in unwanted results.

*) Make bindist uses gawk vs awk, not sure why.


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
mingw64-gtk3
mingw64-gtk-update-icon-cache
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
mingw64-termcap
mingw64-win-iconv
mingw64-winpthreads
mingw64-zlib

You can download the packages in one go like this: copy the above list into a textfile `files.txt` (make sure it has no empty line at the end or it will download ALL packages), then:
```sh
$ cd `~/rpm`
$ cat files.txt | while read f; do wget -r -l1 --no-parent --no-directories -A "${f}*.rpm" -R "*-static-*" -R "*-tools-*" https://download-ib01.fedoraproject.org/pub/fedora-secondary/releases/30/Everything/i386/os/Packages/m/; done;
```
Check the downloaded files, a few will get downloaded which you do not need - delete them before proceeding. At the time of writing those would be:
```sh
$ rm mingw64-atkmm-2.24.2-7.fc30.noarch.rpm mingw64-cairomm-1.12.0-7.fc30.noarch.rpm mingw64-pangomm-2.40.1-6.fc30.noarch.rpm mingw64-pcre2-10.32-2.fc30.noarch.rpm
```

I used `~/rpm` and `~/deb` to store these files, so converting to .deb would go like this:
```sh
$ cd ~
$ su
$ for f in rpm/*.rpm; do alien --to-deb $f; done
$ mv *.deb deb/
```

Check rpm/ and deb/ dirs to check if all got converted okayish.
If okay, install:
```sh
$ cd ~
$ su
$ for f in deb/*.deb; do dpkg -i $f; done
```


### Now what?

Two ways to go about this: properly use env vars etc to make configure and make recognize the files from the Fedora packages, or brute-force this.

I ran into a lot of trouble trying to do the 'proper' thing, so I brute-forced my way to a bindist.

### Brute force

#### Copy files from Fedora paths to Debian paths

Copy all files in `/usr/x86_64-w64-mingw32/sys-root/mingw/{bin,etc,include,lib,share} to `/usr/x86-64-w64-wingw32/`.
```sh
$ cp -R /usr/x86_64-w64-mingw32/sys-root/mingw/* /usr/x86_64-w64-mingw32/
```
#### pkg-config files

Copy and alter pkg-config files:


```sh
# update pc files to point to Debian paths (also create backup in case we fuck up)
$ su
$ cd /usr/x86_64-w64-mingw32/lib/pkgconfig
$ for pc in *.pc; do \
    echo "$pc"; \
    cp "$pc" "$pc.bak"; \
    sed -i 's@/sys-root/mingw@@' "$pc"; \
  done
```

#### Glib tools

Make sure Glib related bins can be found (there are .exe's to do this, but that
requires Wine, so probably not do that)
```sh
$ su
$ ln -s /usr/lib/x86_64-linux-gnu/glib-2.0/glib-compile-resources \
    /usr/bin/glib-compile-resources
$ ln -s /usr/lib/x86_64-linux-gnu/glib-2.0/glib-compile-schemas \
    /usr/bin/glib-compile-schemas
$ ln -s /usr/x86_64-w64-mingw32/bin/glib-genmarshal \
    /usr/bin/glib-genmarshal
$ ln -s /usr/x86_64-w64-mingw32/bin/glib-mkenums \
    /usr/bin/glib-mkenums
```

##### Compile GLib-2.0 schemas

(I assume normally the package manager will do this whenever some schema gets
 updated. Right now I had to do it to at least have a 'gschemas.compiled' file
 for make bindist(zip) to copy.)

```sh
$ su
$ cd /usr/x86_64-w64-mingw32/share/glib-2.0/schemas
$ glib-compile-schemas .
```



##### Optional: make sure configure cannot find the Fedora stuff installed via alien/dpkg

I would advise against this, unless making sure our build system uses the Debian
stuff and not the 'alien' Fedora stuff. Keeping the Fedora stuff around *should*
allow for easier updating of packages, and we will need to update packages on the
cross-compiler to keep up with upstream. The Fedora 30 gtk+-3.0 package is only
3.22.30, my Linux boxes and even msys2 are at least at 3.24.

```sh
$ su
$ mv /usr/x86_64-w64-mingw32/sys-root /usr/x86_64-w64-mingw32/sys-root-bak
```


#### Test VICE

Check out trunk, I'll assume ~/vice-trunk/vice as the location.

See if configure can find everything. Especially important is that it can find
the openGL stuff, since that's what we're doing this horror for:

```sh
$ cd ~/vice-trunk/vice
$ ./autogen.sh
$ cd ..
$ mkdir gtk3-win64-build
$ cd gtk3-win64-build
$ ../vice/configure --enable-native-gtk3ui --host=x86_64-w64-mingw32
```

If this works, we're on the right track, so we'll run make next:
```sh
$ make 2>&1 | tee build.log
```


Finally: scp the GTK3VICE-win64-rxxxxx.zip to a Windows box and see if it works.




### Installing extra packages

For example: libflac for FLAC support.

Download <http://download-ib01.fedoraproject.org/pub/fedora/linux/releases/30/Everything/x86_64/os/Packages/m/mingw64-flac-1.3.2-6.fc30.noarch.rpm> and move it into ~/rpm.

```sh
$ cd ~
$ su
$ cd deb
$ alien --to-deb ../rpm/mingw64-flac-1.3.2-6.fc30.noarch.rpm
$ dpkg -i minwg64-flac_1.3.2-7_all.deb
# sys-root exists again, for now at least
$ cd /usr/x86_64-w64-mingw32
$ cp -R sys-root/mingw/* /usr/x86_64-w64-mingw32
```

Now update the pkg-config files:
```sh
$ cd /usr/x86_64-w64-mingw32/lib/pkgconfig
$ sed -i 's@/sys-root/mingw@@' flac*.pc
```

Now configure will recognize FLAC. (Did the same for Ogg and Theora).


To clean up you can probably do:
```sh
$ rm -rfd /usr/x86_64-w64-mingw32/sys-root
```

Ofcourse any fuckery with sys-root renaming or deleting will screw with dpkg, so
unless you really need the space, leave it alone.




