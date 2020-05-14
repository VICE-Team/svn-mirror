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


#### Enable sudo

We need sudo for the frankenvice script (I think)

So, we'll install sudo (on Debian 10.3) and add the 'vice' user to the sudoers:
```sh
$ su -i
$ apt install sudo
$ adduser vice sudo
```

Now log out completely and log back in to have the 'vice' user recognized as a sudoer.

**UPDATE 2020-05-13**

When selecting an empty password, the debian installer will lock out root and automagically add the normal user to sudoers. This is actually preferred since we need sudo anyway for some scripts.


### Install required packages

Make sure you have the basic development tools:

```
$ sudo apt install autoconf automake build-essential byacc flex git subversion \
        vim xa65 alien p7zip-full texinfo gawk zip unzip yasm dos2unix \
        libglib2.0-dev-bin
```
(todo: probably a lot more)

Do **not** install any native Linux Gtk/GLib packages unless specifically told to do so, this might result in unwanted results.
The above mentioned `libglib2.0-dev-bin` is an exception, we don't want to install the windows version of it and run it via Wine.

(`make bindist` uses gawk, not awk, so make sure to install gawk)


#### Install Debian's mingw packages

Make sure you don't have apt set to automatically pull in 'suggested' packages, that will pull in wine, which we don't want. Default debian won't do that.

```sh
$ sudo apt install mingw-w64 mingw-w64-tools
```

Hint: Debian mingw packages are called 'mingw-w64-\*" while Fedora mingw packages are called 'mingw64-\*'. This will come in handy later to see which packages came from Debian and which were installed via RPM's using 'alien'.




#### Check out VICE trunk

VICE trunk contains a script and a list of packages, which should make the next step a lot easier.

Check out trunk (read-only):
```sh
$ cd
$ svn checkout https://svn.code.sf.net/p/vice-emu/code/trunk vice-trunk
```


#### Install Fedora packages

Since Debian doesn't provide any Gtk/GLib packages for cross-compiling to Windows, we'll be using Fedora packages. At the moment of writing I've been using Fedora 30 packages.


**UPDATE 2020-05-13**

The directory ~/vice-trunk/vice/build/mingw/frankenvice now contains a script to automate downloading, converting and installing Fedora mingw64 packages. It'll use the file rpm-packages.txt for its package list.
But first read the `frankenvice-install.sh` file to make sure the build environment is setup up properly:

##### Fix alien's Perl code:

Patch alien's Rpm.pm to allow zstd handling:

The file is located at `/usr/share/perl5/Alien/Package/Rpm.pm`

The patch provided by Larsks is this:
```diff
diff --git a/Alien/Package/Rpm.pm b/Alien/Package/Rpm.pm
index d53be2b..66ff3d6 100644
--- a/Alien/Package/Rpm.pm
+++ b/Alien/Package/Rpm.pm
@@ -159,10 +159,12 @@ sub unpack {
 	$this->SUPER::unpack(@_);
 	my $workdir=$this->unpacked_tree;
 	
-	# Check if we need to use lzma to uncompress the cpio archive
+	# Check if we need to uncompress the cpio archive
 	my $decomp='';
 	if ($this->do("rpm2cpio '".$this->filename."' | lzma -t -q > /dev/null 2>&1")) {
 		$decomp = 'lzma -d -q |';
+	} elsif ($this->do("rpm2cpio '".$this->filename."' | zstd -t -q > /dev/null 2>&1")) {
+		$decomp = 'zstd -d -q |';
 	}
 
```


##### Fix pkg-config for Windows cross-building

Currently (2020-03-23), Debian has an issue with running pkg-config for Windows
cross-builds. Whenever you run into a message like:

    Please install dpkg-dev to use pkg-config when cross-building

Don't bother installing dpkg-dev, it's already installed and reinstalling won't
fix anything.

Edit `/usr/share/pkg-config-crosswrapper` and comment out the 'if [ "$?" != 0 ]
branch after the multiarch="...." line (line 13 on my box).


##### Run the script to download and install Fedora mingw64 packages

```sh
$ cd
$ vice-trunk/vice/build/mingw/frankenvice/frankenvice-install.sh
```

This should download, convert and install Fedora32 mingw64 packages.



##### Glib tools

Make sure Glib related bins can be found (there are .exe's to do this, but that
requires Wine, so probably not do that)

*Looks like this is no longer required, managed to build just fine without this*

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
$ cd /usr/x86_64-w64-mingw32/share/glib-2.0/schemas
$ sudo /usr/lib/x86_64-linux-gnu/glib-2.0/glib-compile-schemas .
```



#### Try compiling VICE

```sh
$ cd ~/vice-trunk/vice
$ ./autogen.sh
$ cd ..
$ mkdir gtk3-win64-build
$ cd gtk3-win64-build
$ ../vice/configure --enable-native-gtk3ui --host=x86_64-w64-mingw32 --enable-arch=no
```

If this works, we're on the right track, so we'll run make next:
```sh
$ make 2>&1 | tee build.log
$ make bindist
$ make bindistzip
```

I get a few warnings still:
```
cp: cannot stat '/usr/lib/gcc/x86_64-w64-mingw32/8.3-win32/../../../../x86_64-w64-mingw32/bin/libgcc_s_*.dll': No such file or directory
cp: cannot stat '/usr/lib/gcc/x86_64-w64-mingw32/8.3-win32/../../../../x86_64-w64-mingw32/bin/liblzma-5.dll': No such file or directory
cp: failed to get attributes of 'share/icons/hicolor': No such file or directory
```

And while running on Windows 10, I get warnings about libtiff-5.dll and libwinpthread-1.dll not being found.
libwinpthread-1.dll is in /usr/x86_64-w64-mingw32/lib/, apparently installed with Debian's mingw64-dev or so.


EXPERIMENTAL SHIT FOLLOWING:

Create symlink for the libwinpthread-1.dll:

```sh
$ cd /usr/x86_64-w64-mingw32/bin
$ sudo ln -s ../lib/libwinpthread-1.dll libwinphread-1.dll
```
doesn't work :(


**Done for now, someone else can figure this crap out.**


**OLD (2020-05-14), most if not all the following can be ignored**

These are the packages I had to use:

mingw64-adwaita-icon-theme
mingw64-atk
mingw64-bzip2
mingw64-cairo
mingw64-expat
mingw64-fontconfig
mingw64-freeglut
mingw64-freetype
mingw64-fribidi
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
$ cd ~/rpm
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

```sh
$ cd ~
$ svn checkout https://svn.code.sf.net/p/vice-emu/code/trunk vice-trunk
```

See if configure can find everything. Especially important is that it can find
the openGL stuff, since that's what we're doing this horror for:



Finally: scp the GTK3VICE-win64-rxxxxx.zip to a Windows box and see if it works.

#### Generate PDF

texinfo isn't enough to generate the vice.pdf, due to some enconding changes in vice.texi we now have to install 'texlive-fonts-recommended', which pulls in 100+ packages, including Ruby and a bunch of X11 stuff, non of which we need.



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




## WIP: Convert .deb for direct use with FrankenVice

**See the end of this document: 'Using vice-rpm-to-deb.sh' for an easier way**

To avoid the hassle of updating paths and pkgconfig files for each package, we can update the .deb generated by `alien` for direct use with `dpkg` by doing the copying and sed magic in the .deb itself. This will avoid possible errors and there shouldn't be a '/usr/x86\_64-w64-mingw32/sys-root' directory.

### Example, doing it manually

**Note**: Debian and Fedora use slightly different filenames to indicate version, revision and architecture of a package, so a future script should handle this properly to avoid problems.


Let's take the 'mingw64-cairo' package as an example. This assumes .rpm packages in '~/rpm' and alien-converted .deb packages in ~/deb.

Create a temporary working dir for the conversion (as normal user):
```sh
mkdir ~/temp
```

Convert .rpm to .deb:
```sh
$ su
$ cd /home/vice/deb
$ alien --to-deb ../rpm/mingw64-cairo-1.14.10-5.fc30.noarch.rpm
$ exit
```

Unpack the .deb generated with alien:
(alien bumps the release number, so we went from -5 to -6, using --bump=0 will
 fix this)
```sh
$ cd ~/temp
# Use RAW to extract the entire archive, including the metadata (./DEBIAN dir):
$ dpkg-deb -R ~/deb/mingw64-cairo_1.14.10-6_all.deb mingw64-cairo_1.14.10-6_all
```

Move the mingw files to their Debian location:
```sh
$ cd mingw64-cairo_1.14.10-6_all
# Make sure to not use '/usr' here, but plain 'usr'
$ mv usr/x86_64-w64-mingw32/sys-root/mingw/* usr/x86_64-w64-mingw32
```

Update any pkg-config files that may be there:
```sh
$ cd usr/x86_64-w64-mingw32/lib/pkgconfig
$ for pc in *.pc; do sed -i 's@/sys-root/mingw@@' "$pc"; done
```

**Note**: some packages have symlinks in them for .pc files (example: mingw64-libpng), so we need to handle that. Worst case Ontario: sed does its magic on a file twice, probably.


Create the new .deb with the updated files:
```sh
$ cd ~/temp
# The --build command needs the ./DEBIAN dir, so make sure you used -R to
# extract the alien-generate .deb
$ dpkg-deb -b mingw64-cairo_1.14.10-6_all
```

This should create a file 'mingw64-cairo\_1.14.10-6\_all.deb, which can be installed with `dpgk`:
```sh
$ su
$ dpkg -i mingw64-cairo_1.14.10-6_all.deb
```

**TODO**

* Figure out how to handle symlinks properly
* Perhaps update the ./DEBIAN/control file to make clear we (VICE) created the package
* Turn this into a script and put it in trunk
* alien seem to bump version (ie foo\_1.0-42 to foo\_1.0-43), we need to avoid this to retain some sanity.


### Using vice-rpm-to-deb.sh

In the build/mingw/frankenvice directory in trunk is a script `vice-rpm-to-deb.sh` which can convert Fedora RPMs to Debian DEBs for use with FrankenVice.

What it basically does is this:

* call `alien` to convert rpm to deb
* use dpkg-deb to extract deb into /tmp
* move files and dirs from Fedora's "/usr/x86\_64-w64-mingw32/sys-root/mingw/" dir to Debian's "/usr/x86\_64-w64-mingw32/"
* Patch any pkg-config '\*.pc' files to point to the correct Debian mingw dirs
* use dpkg-deb to recreate a deb package from the patched files
* copy deb to current dir

What it doesn't do is fix symlinks that use absolute paths, I haven't seen any packages that use that, so perhaps we're safe, but I doubt that,

Another nasty thing is that it requires root to run, due to `alien` somehow requiring root access. The script also currently requires bash, due to my limited shell scripting skills.


#### Patch Alien to work with current RPM's using zstd

Patch alien's Rpm.pm to allow zstd handling:

The file is located at `/usr/share/perl5/Alien/Package/Rpm.pm`

The patch provided by Larsks is this:
```diff
diff --git a/Alien/Package/Rpm.pm b/Alien/Package/Rpm.pm
index d53be2b..66ff3d6 100644
--- a/Alien/Package/Rpm.pm
+++ b/Alien/Package/Rpm.pm
@@ -159,10 +159,12 @@ sub unpack {
 	$this->SUPER::unpack(@_);
 	my $workdir=$this->unpacked_tree;
 	
-	# Check if we need to use lzma to uncompress the cpio archive
+	# Check if we need to uncompress the cpio archive
 	my $decomp='';
 	if ($this->do("rpm2cpio '".$this->filename."' | lzma -t -q > /dev/null 2>&1")) {
 		$decomp = 'lzma -d -q |';
+	} elsif ($this->do("rpm2cpio '".$this->filename."' | zstd -t -q > /dev/null 2>&1")) {
+		$decomp = 'zstd -d -q |';
 	}
 
```

I already updated bits of the Perl code before writing this, so I manually added some of the patch. So good luck applying this one.

But after this you should be able to use Gtk3-3.24.x RPMs (which will require installing a shitload of other .fc32 packages)


#### Example use

(This again assumes a normal user called 'vice', and ~/deb and ~/rpm dirs)

Let's try to install FLAC.

Download the RPM:
```sh
$ cd ~/rpm
$ wget http://download-ib01.fedoraproject.org/pub/fedora/linux/releases/30/Everything/x86_64/os/Packages/m/mingw64-flac-1.3.2-6.fc30.noarch.rpm
```

Use the script to convert to deb:
```sh
$ cd ~/temp
$ su
$ /home/vice/vice-trunk/vice/build/mingw/frankenvice/vice-rpm-to-deb.sh ../rpm/mingw64-flac-1.3.2-6.fc30.noarch.rpm
```

This should result in:
```
Working on RPM file mingw64-flac-1.3.2-6.fc30.noarch.rpm
  name    : mingw64-flac
  version : 1.3.2
  release : 6
  deb file: mingw64-flac_1.3.2-6_all.deb
Running alien on ../rpm/mingw64-flac-1.3.2-6.fc30.noarch.rpm ... OK
Extracting alien-generated files ... OK.
Moving mingw files into proper Debian paths.
Patching pkg-config files:
  patching /tmp/mingw64-flac_1.3.2-6_all/usr/x86_64-w64-mingw32/lib/pkgconfig/flac.pc
  patching /tmp/mingw64-flac_1.3.2-6_all/usr/x86_64-w64-mingw32/lib/pkgconfig/flac++.pc
Generating .deb package.
dpkg-deb: building package 'mingw64-flac' in '/tmp/mingw64-flac_1.3.2-6_all.deb'.
Copying to current dir.
```

Check if the package is there:
```sh
$ ls
mingw64-flac_1.3.2-6_all.deb    mingw64-xz-libs_5.2.3-5_all.deb
mingw64-gtk3_3.22.30-2_all.deb
```

Now install the package:
```sh
# optional: make a copy:
$ cp mingw64-flac_1.3.2-6_all.deb ../deb
# install:
$ dpkg -i mingw64-flac_1.3.2-6_all.deb
```
(Ctrl+D)


Run VICE's ./configure:

```sh
$ cd ~/vice-trunk/vice
$ ./configure --enable-native-gtk3ui --enable-debug-gtk3ui --enable-debug --host=x86_64-w64-mingw32
$ grep -i 'flac' config.log
```

If the grep output has '-lFLAC' and '#define HAVE\_LIBFLAC 1', flac will work.

