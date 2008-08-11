#!/bin/sh
# make-bindist.sh for the QNX 6.x port
#
# written by Marco van den Heuvel <blackystardust68@yahoo.com>
#
# make-bindist.sh <strip> <vice-version> <prefix> <cross> <zip|nozip> <topsrcdir> <make-command>
#                 $1      $2             $3       $4      $5          $6          $7

STRIP=$1
VICEVERSION=$2
PREFIX=$3
CROSS=$4
ZIPKIND=$5
TOPSRCDIR=$6
MAKECOMMAND=$7

if test x"$PREFIX" != "x/opt"; then
  echo Error: installation path is not /opt
  exit 1
fi

if test x"$CROSS" = "xtrue"; then
  echo Error: \"make bindist\" for QNX 6 can only be done on QNX 6
  exit 1
fi

if [ ! -e src/x64 -o ! -e src/x64dtv -o ! -e src/x128 -o ! -e src/xvic -o ! -e src/xpet -o ! -e src/xplus4 -o ! -e src/xcbm2 -o ! -e src/c1541 -o ! -e src/petcat -o ! -e src/cartconv ]
then
  echo Error: \"make\" needs to be done first
  exit 1
fi

echo Generating QNX 6 port binary distribution.
rm -f -r VICE-$VICEVERSION
curdir=`pwd`
$MAKECOMMAND prefix=$curdir/VICE-$VICEVERSION/opt VICEDIR=$curdir/VICE-$VICEVERSION/opt/lib/vice install
$STRIP VICE-$VICEVERSION/opt/bin/x64
$STRIP VICE-$VICEVERSION/opt/bin/x64dtv
$STRIP VICE-$VICEVERSION/opt/bin/x128
$STRIP VICE-$VICEVERSION/opt/bin/xvic
$STRIP VICE-$VICEVERSION/opt/bin/xpet
$STRIP VICE-$VICEVERSION/opt/bin/xplus4
$STRIP VICE-$VICEVERSION/opt/bin/xcbm2
$STRIP VICE-$VICEVERSION/opt/bin/c1541
$STRIP VICE-$VICEVERSION/opt/bin/petcat
$STRIP VICE-$VICEVERSION/opt/bin/cartconv
rm `find VICE-$VICEVERSION -name "amiga_*.vkm"`
rm `find VICE-$VICEVERSION -name "beos_*.vkm"`
rm `find VICE-$VICEVERSION -name "dos_*.vkm"`
rm `find VICE-$VICEVERSION -name "os2*.vkm"`
rm `find VICE-$VICEVERSION -name "osx*.vkm"`
rm `find VICE-$VICEVERSION -name "win_*.vkm"`
rm `find VICE-$VICEVERSION -name "RO*.vkm"`
rm `find VICE-$VICEVERSION -name "*.vsc"`
if test x"$ZIPKIND" = "xzip"; then
  gcc $TOPSRCDIR/src/arch/unix/qnx6/getsize.c -o ./getsize
  gcc $TOPSRCDIR/src/arch/unix/qnx6/getlibs.c -o ./getlibs
  mkdir -p public/VICE/core-$VICEVERSION/x86/opt/bin
  mv VICE-$VICEVERSION/opt/bin/x* public/VICE/core-$VICEVERSION/x86/opt/bin
  mv VICE-$VICEVERSION/opt/bin/c1541 public/VICE/core-$VICEVERSION/x86/opt/bin
  mv VICE-$VICEVERSION/opt/bin/cartconv public/VICE/core-$VICEVERSION/x86/opt/bin
  mv VICE-$VICEVERSION/opt/bin/petcat public/VICE/core-$VICEVERSION/x86/opt/bin

  current_date=`date +%Y/%m/%d`

cat >manifest.01 <<_END
<?xml version="1.0"?>
<RDF:RDF xmlns:RDF="http://www.w3.org" xmlns:QPM="http://www.qnx.com">
   <RDF:Description about="http://www.qnx.com">
      <QPM:PackageManifest>
         <QPM:PackageDescription>
            <QPM:PackageType>Application</QPM:PackageType>
            <QPM:PackageName>VICE-$VICEVERSION-x86-public</QPM:PackageName>
_END

echo >manifest.02 "            <QPM:PackageSize/>"

cat >manifest.03 <<_END
            <QPM:PackageReleaseNumber>1</QPM:PackageReleaseNumber>
            <QPM:PackageReleaseDate>$current_date</QPM:PackageReleaseDate>
            <QPM:PackageRepository/>
            <QPM:FileVersion>2.0</QPM:FileVersion>
         </QPM:PackageDescription>

         <QPM:ProductDescription>
            <QPM:ProductName>VICE for x86</QPM:ProductName>
_END

  ls -l -R public/VICE/core-$VICEVERSION/x86/opt >size.tmp
  manifest_size=`./getsize size.tmp`
  rm -f -r size.tmp
  echo >manifest.04 "            <QPM:ProductSize>$manifest_size</QPM:ProductSize>"

  echo >manifest.05 "            <QPM:ProductIdentifier>VICE-host_x86</QPM:ProductIdentifier>"

  cat >manifest.06 <<_END
            <QPM:ProductEmail>vice-devel@firenze.linux.it</QPM:ProductEmail>
            <QPM:VendorName>Public</QPM:VendorName>
            <QPM:VendorInstallName>public</QPM:VendorInstallName>
            <QPM:VendorURL>http://www.viceteam.org</QPM:VendorURL>
            <QPM:VendorEmbedURL/>
            <QPM:VendorEmail>vice-devel@firenze.linux.it</QPM:VendorEmail>
            <QPM:AuthorName>The VICE Team</QPM:AuthorName>
            <QPM:AuthorURL>http://www.viceteam.org</QPM:AuthorURL>
            <QPM:AuthorEmbedURL/>
            <QPM:AuthorEmail>vice-devel@firenze.linux.it</QPM:AuthorEmail>
            <QPM:ProductIconSmall>http://www.trikaliotis.net/vicekb/common/vice-logo.png</QPM:ProductIconSmall>
            <QPM:ProductIconLarge>http://www.viceteam.org/images/vice-logo.jpg</QPM:ProductIconLarge>
            <QPM:ProductDescriptionShort>The Versatile Commodore Emulator</QPM:ProductDescriptionShort>
            <QPM:ProductDescriptionLong>VICE is a program that runs on a Unix, MS-DOS, Win32, OS/2, Acorn RISC OS, QNX 6.x, AmigaOS or BeOS machine and executes programs intended for the old 8-bit computers. The current version emulates the C64, the C128, the VIC20, all the PET models (except the SuperPET 9000, which is out of line anyway), the PLUS4 and the CBM-II (aka C610).</QPM:ProductDescriptionLong>
            <QPM:ProductDescriptionURL>http://www.viceteam.org</QPM:ProductDescriptionURL>
            <QPM:ProductDescriptionEmbedURL/>
_END

echo >manifest.07 "            <QPM:InstallPath>public/VICE/core-$VICEVERSION/x86</QPM:InstallPath>"

cat >manifest.08 <<_END
         </QPM:ProductDescription>

         <QPM:ReleaseDescription>
            <QPM:ReleaseDate>$current_date</QPM:ReleaseDate>
            <QPM:ReleaseVersion>$VICEVERSION</QPM:ReleaseVersion>
            <QPM:ReleaseUrgency>High</QPM:ReleaseUrgency>
            <QPM:ReleaseStability>Stable</QPM:ReleaseStability>
            <QPM:ReleaseNoteMinor/>
            <QPM:ReleaseNoteMajor/>
            <QPM:ReleaseBuild>1</QPM:ReleaseBuild>
            <QPM:ExcludeCountries>
               <QPM:Country/>
            </QPM:ExcludeCountries>

            <QPM:ReleaseCopyright>GNU General Public License</QPM:ReleaseCopyright>
         </QPM:ReleaseDescription>

         <QPM:LicenseUrl>rep://VICE-$VICEVERSION-public.repdata/LicenseUrl/COPYING</QPM:LicenseUrl>
         <QPM:ProductKeyUrl/>
         <QPM:RelationshipDescription>
_END

cat >manifest.09 <<_END
            <QPM:RelationshipType>component</QPM:RelationshipType>
            <QPM:ComponentType>Required</QPM:ComponentType>
            <QPM:ComponentParent>VICE</QPM:ComponentParent>
_END

cat >manifest.10 <<_END
            <QPM:ComponentFilter/>
         </QPM:RelationshipDescription>

         <QPM:ContentDescription>
            <QPM:ContentTopic>Games and Diversions/Game Emulatiors</QPM:ContentTopic>
            <QPM:ContentKeyword>c64,c128,vic20,plus4,c16,cbm510,cbm610,c510,c610,cbm,commodore,emulator,pet</QPM:ContentKeyword>
_END

echo >manifest.11 "            <QPM:Processor>x86</QPM:Processor>"

cat >manifest.12 <<_END
            <QPM:TargetProcessor/>
            <QPM:TargetOS>qnx6</QPM:TargetOS>
            <QPM:HostOS>none</QPM:HostOS>
            <QPM:DisplayEnvironment>X11R6</QPM:DisplayEnvironment>
            <QPM:TargetAudience>User</QPM:TargetAudience>
_END

  ./getlibs public/VICE/core-$VICEVERSION/x86/opt/bin/x64 >manifest.13

cat >manifest.14 <<_END
         </QPM:ContentDescription>

         <QPM:ProductInstallationDependencies>
            <QPM:ProductRequirements>This package requires the following libraries to operate correctly : libintl, libSDL, libpng, libX11.</QPM:ProductRequirements>
         </QPM:ProductInstallationDependencies>
_END

cat >manifest.15 <<_END
         <QPM:QNXLicenseDescription/>
      </QPM:PackageManifest>

      <QPM:FileManifest>
         <QPM:Listing>
            <QPM:Dir name="">
               <QPM:Dir name="opt">
                  <QPM:Dir name="bin">
                     <QPM:File>c1541</QPM:File>
                     <QPM:File>petcat</QPM:File>
                     <QPM:File>cartconv</QPM:File>
                     <QPM:File>x128</QPM:File>
                     <QPM:File>x64</QPM:File>
                     <QPM:File>x64dtv</QPM:File>
                     <QPM:File>xcbm2</QPM:File>
                     <QPM:File>xpet</QPM:File>
                     <QPM:File>xplus4</QPM:File>
                     <QPM:File>xvic</QPM:File>
                  </QPM:Dir>
               </QPM:Dir>
            </QPM:Dir>
         </QPM:Listing>
      </QPM:FileManifest>
   </RDF:Description>
</RDF:RDF>
_END

  cat >public/VICE/core-$VICEVERSION/x86/MANIFEST manifest.01 manifest.02 \
      manifest.03 manifest.04 manifest.05 manifest.06 manifest.07 manifest.08 \
      manifest.09 manifest.10 manifest.11 manifest.12 manifest.13 manifest.14 \
      manifest.15

  tar cf VICE-x86.tar public
  gzip VICE-x86.tar
  mv VICE-x86.tar.gz VICE-$VICEVERSION-x86-public.qpk

  ls -l -R VICE-$VICEVERSION-x86-public.qpk >size.tmp
  manifest_size=`./getsize size.tmp` 
  rm -f -r size.tmp

  echo >manifest.04 "            <QPM:PackageSize>$manifest_size</QPM:PackageSize>"

  cat >VICE-$VICEVERSION-x86-public.qpm manifest.01 manifest.02 manifest.03 manifest.04 \
      manifest.05 manifest.06 manifest.07 manifest.08 manifest.09 manifest.10 \
      manifest.11 manifest.12 manifest.13 manifest.14 manifest.15

  rm -f -r public

  mkdir -p public/VICE/core-$VICEVERSION
  mv VICE-$VICEVERSION/opt public/VICE/core-$VICEVERSION

  mkdir -p VICE-$VICEVERSION-public.repdata/LicenseUrl
  cp public/VICE/core-$VICEVERSION/opt/lib/vice/doc/COPYING VICE-$VICEVERSION-public.repdata/LicenseUrl

  echo >manifest.02 "            <QPM:PackageSize/>"

  ls -l -R public/VICE/core-$VICEVERSION/opt >size.tmp
  manifest_size=`./getsize size.tmp`
  rm -f -r size.tmp
  echo >manifest.04 "            <QPM:ProductSize>$manifest_size</QPM:ProductSize>"

  echo >manifest.05 "            <QPM:ProductIdentifier>VICE</QPM:ProductIdentifier>"

  echo >manifest.07 "            <QPM:InstallPath>public/VICE/core-$VICEVERSION</QPM:InstallPath>"

  cat >manifest.09 <<_END
            <QPM:RelationshipType>core</QPM:RelationshipType>
            <QPM:ComponentType>Required</QPM:ComponentType>
            <QPM:ComponentParent/>
_END

  echo >manifest.11 "            <QPM:Processor/>"

  cat >manifest.15 <<_END
         <QPM:ProductInstallationProcedure>
            <QPM:Script>
               <QPM:ScriptName>/usr/photon/bin/launchmenu_notify</QPM:ScriptName>
               <QPM:ScriptTiming>Post</QPM:ScriptTiming>
               <QPM:ScriptType>Use</QPM:ScriptType>
            </QPM:Script>

            <QPM:Script>
               <QPM:ScriptName>/usr/photon/bin/launchmenu_notify</QPM:ScriptName>
               <QPM:ScriptTiming>Post</QPM:ScriptTiming>
               <QPM:ScriptType>Unuse</QPM:ScriptType>
            </QPM:Script>
         </QPM:ProductInstallationProcedure>

         <QPM:QNXLicenseDescription/>
      </QPM:PackageManifest>

      <QPM:FileManifest>
         <QPM:PkgInclude>
            <QPM:Item>\$(PROCESSOR)/MANIFEST</QPM:Item>
         </QPM:PkgInclude>

         <QPM:Listing>
            <QPM:Dir name="">
               <QPM:Dir name="opt">
                  <QPM:Union link="../\$(PROCESSOR)/opt/bin">bin</QPM:Union>
                  <QPM:Dir name="bin">
                     <QPM:File>vsid</QPM:File>
                  </QPM:Dir>

                  <QPM:Dir name="info">
                     <QPM:File>vice.info</QPM:File>
                     <QPM:File>vice.info-1</QPM:File>
                     <QPM:File>vice.info-2</QPM:File>
                     <QPM:File>vice.info-3</QPM:File>
                     <QPM:File>vice.info-4</QPM:File>
                     <QPM:File>vice.info-5</QPM:File>
                     <QPM:File>vice.info-6</QPM:File>
                  </QPM:Dir>

                  <QPM:Dir name="lib">
                     <QPM:Dir name="locale">
                        <QPM:Dir name="de">
                           <QPM:Dir name="LC_MESSAGES">
                              <QPM:File>vice.mo</QPM:File>
                           </QPM:Dir>
                        </QPM:Dir>

                        <QPM:Dir name="fr">
                           <QPM:Dir name="LC_MESSAGES">
                              <QPM:File>vice.mo</QPM:File>
                           </QPM:Dir>
                        </QPM:Dir>

                        <QPM:Dir name="hu">
                           <QPM:Dir name="LC_MESSAGES">
                              <QPM:File>vice.mo</QPM:File>
                           </QPM:Dir>
                        </QPM:Dir>

                        <QPM:Dir name="it">
                           <QPM:Dir name="LC_MESSAGES">
                              <QPM:File>vice.mo</QPM:File>
                           </QPM:Dir>
                        </QPM:Dir>

                        <QPM:Dir name="nl">
                           <QPM:Dir name="LC_MESSAGES">
                              <QPM:File>vice.mo</QPM:File>
                           </QPM:Dir>
                        </QPM:Dir>

                        <QPM:Dir name="pl">
                           <QPM:Dir name="LC_MESSAGES">
                              <QPM:File>vice.mo</QPM:File>
                           </QPM:Dir>
                        </QPM:Dir>

                        <QPM:Dir name="sv">
                           <QPM:Dir name="LC_MESSAGES">
                              <QPM:File>vice.mo</QPM:File>
                           </QPM:Dir>
                        </QPM:Dir>
                     </QPM:Dir>

                     <QPM:Dir name="vice">
                        <QPM:Dir name="C128">
                           <QPM:File>basic64</QPM:File>
                           <QPM:File>basichi</QPM:File>
                           <QPM:File>basiclo</QPM:File>
                           <QPM:File>c64hq.vpl</QPM:File>
                           <QPM:File>c64s.vpl</QPM:File>
                           <QPM:File>ccs64.vpl</QPM:File>
                           <QPM:File>chargde</QPM:File>
                           <QPM:File>chargen</QPM:File>
                           <QPM:File>chargfr</QPM:File>
                           <QPM:File>chargse</QPM:File>
                           <QPM:File>default.vpl</QPM:File>
                           <QPM:File>default.vrs</QPM:File>
                           <QPM:File>frodo.vpl</QPM:File>
                           <QPM:File>godot.vpl</QPM:File>
                           <QPM:File>kernal</QPM:File>
                           <QPM:File>kernal64</QPM:File>
                           <QPM:File>kernalde</QPM:File>
                           <QPM:File>kernalfi</QPM:File>
                           <QPM:File>kernalfr</QPM:File>
                           <QPM:File>kernalit</QPM:File>
                           <QPM:File>kernalno</QPM:File>
                           <QPM:File>kernalse</QPM:File>
                           <QPM:File>pc64.vpl</QPM:File>
                           <QPM:File>vdc_deft.vpl</QPM:File>
                           <QPM:File>vice.vpl</QPM:File>
                           <QPM:File>x11_pos.vkm</QPM:File>
                           <QPM:File>x11_sym.vkm</QPM:File>
                        </QPM:Dir>

                        <QPM:Dir name="C64">
                           <QPM:File>basic</QPM:File>
                           <QPM:File>c64hq.vpl</QPM:File>
                           <QPM:File>c64mem.sym</QPM:File>
                           <QPM:File>c64s.vpl</QPM:File>
                           <QPM:File>ccs64.vpl</QPM:File>
                           <QPM:File>chargen</QPM:File>
                           <QPM:File>default.vpl</QPM:File>
                           <QPM:File>default.vrs</QPM:File>
                           <QPM:File>frodo.vpl</QPM:File>
                           <QPM:File>godot.vpl</QPM:File>
                           <QPM:File>kernal</QPM:File>
                           <QPM:File>pc64.vpl</QPM:File>
                           <QPM:File>vice.vpl</QPM:File>
                           <QPM:File>x11_pos.vkm</QPM:File>
                           <QPM:File>x11_sym.vkm</QPM:File>
                        </QPM:Dir>

                        <QPM:Dir name="C64DTV">
                           <QPM:File>basic</QPM:File>
                           <QPM:File>c64hq.vpl</QPM:File>
                           <QPM:File>c64mem.sym</QPM:File>
                           <QPM:File>c64s.vpl</QPM:File>
                           <QPM:File>ccs64.vpl</QPM:File>
                           <QPM:File>chargen</QPM:File>
                           <QPM:File>default.vpl</QPM:File>
                           <QPM:File>default.vrs</QPM:File>
                           <QPM:File>frodo.vpl</QPM:File>
                           <QPM:File>godot.vpl</QPM:File>
                           <QPM:File>kernal</QPM:File>
                           <QPM:File>pc64.vpl</QPM:File>
                           <QPM:File>vice.vpl</QPM:File>
                           <QPM:File>x11_pos.vkm</QPM:File>
                           <QPM:File>x11_sym.vkm</QPM:File>
                        </QPM:Dir>

                        <QPM:Dir name="CBM-II">
                           <QPM:File>amber.vpl</QPM:File>
                           <QPM:File>basic.128</QPM:File>
                           <QPM:File>basic.256</QPM:File>
                           <QPM:File>basic.500</QPM:File>
                           <QPM:File>c64hq.vpl</QPM:File>
                           <QPM:File>c64s.vpl</QPM:File>
                           <QPM:File>ccs64.vpl</QPM:File>
                           <QPM:File>chargen.500</QPM:File>
                           <QPM:File>chargen.600</QPM:File>
                           <QPM:File>chargen.700</QPM:File>
                           <QPM:File>default.vpl</QPM:File>
                           <QPM:File>frodo.vpl</QPM:File>
                           <QPM:File>godot.vpl</QPM:File>
                           <QPM:File>green.vpl</QPM:File>
                           <QPM:File>kernal</QPM:File>
                           <QPM:File>kernal.500</QPM:File>
                           <QPM:File>pc64.vpl</QPM:File>
                           <QPM:File>rom128h.vrs</QPM:File>
                           <QPM:File>rom128l.vrs</QPM:File>
                           <QPM:File>rom256h.vrs</QPM:File>
                           <QPM:File>rom256l.vrs</QPM:File>
                           <QPM:File>rom500.vrs</QPM:File>
                           <QPM:File>white.vpl</QPM:File>
                           <QPM:File>x11_buks.vkm</QPM:File>
                        </QPM:Dir>

                        <QPM:Dir name="DRIVES">
                           <QPM:File>d1541II</QPM:File>
                           <QPM:File>d1571cr</QPM:File>
                           <QPM:File>dos1001</QPM:File>
                           <QPM:File>dos1541</QPM:File>
                           <QPM:File>dos1551</QPM:File>
                           <QPM:File>dos1570</QPM:File>
                           <QPM:File>dos1571</QPM:File>
                           <QPM:File>dos1581</QPM:File>
                           <QPM:File>dos2031</QPM:File>
                           <QPM:File>dos2040</QPM:File>
                           <QPM:File>dos3040</QPM:File>
                           <QPM:File>dos4040</QPM:File>
                        </QPM:Dir>

                        <QPM:Dir name="PET">
                           <QPM:File>amber.vpl</QPM:File>
                           <QPM:File>basic1</QPM:File>
                           <QPM:File>basic2</QPM:File>
                           <QPM:File>basic4</QPM:File>
                           <QPM:File>chargen</QPM:File>
                           <QPM:File>chargen.de</QPM:File>
                           <QPM:File>edit1g</QPM:File>
                           <QPM:File>edit2b</QPM:File>
                           <QPM:File>edit2g</QPM:File>
                           <QPM:File>edit4b40</QPM:File>
                           <QPM:File>edit4b80</QPM:File>
                           <QPM:File>edit4g40</QPM:File>
                           <QPM:File>green.vpl</QPM:File>
                           <QPM:File>kernal1</QPM:File>
                           <QPM:File>kernal2</QPM:File>
                           <QPM:File>kernal4</QPM:File>
                           <QPM:File>rom1g.vrs</QPM:File>
                           <QPM:File>rom2b.vrs</QPM:File>
                           <QPM:File>rom2g.vrs</QPM:File>
                           <QPM:File>rom4b40.vrs</QPM:File>
                           <QPM:File>rom4b80.vrs</QPM:File>
                           <QPM:File>rom4g40.vrs</QPM:File>
                           <QPM:File>white.vpl</QPM:File>
                           <QPM:File>x11_bdep.vkm</QPM:File>
                           <QPM:File>x11_bdes.vkm</QPM:File>
                           <QPM:File>x11_bgrs.vkm</QPM:File>
                           <QPM:File>x11_buks.vkm</QPM:File>
                        </QPM:Dir>

                        <QPM:Dir name="PLUS4">
                           <QPM:File>3plus1hi</QPM:File>
                           <QPM:File>3plus1lo</QPM:File>
                           <QPM:File>basic</QPM:File>
                           <QPM:File>default.vpl</QPM:File>
                           <QPM:File>default.vrs</QPM:File>
                           <QPM:File>kernal</QPM:File>
                           <QPM:File>vice.vpl</QPM:File>
                           <QPM:File>x11_pos.vkm</QPM:File>
                           <QPM:File>x11_sym.vkm</QPM:File>
                        </QPM:Dir>

                        <QPM:Dir name="PRINTER">
                           <QPM:File>cbm1526</QPM:File>
                           <QPM:File>mps801</QPM:File>
                           <QPM:File>mps803</QPM:File>
                           <QPM:File>mps803.vpl</QPM:File>
                           <QPM:File>nl10-cbm</QPM:File>
                        </QPM:Dir>

                        <QPM:Dir name="VIC20">
                           <QPM:File>basic</QPM:File>
                           <QPM:File>chargen</QPM:File>
                           <QPM:File>default.vpl</QPM:File>
                           <QPM:File>default.vrs</QPM:File>
                           <QPM:File>kernal</QPM:File>
                           <QPM:File>x11_pos.vkm</QPM:File>
                           <QPM:File>x11_sym.vkm</QPM:File>
                        </QPM:Dir>

                        <QPM:Dir name="doc">
                           <QPM:File>64doc.txt</QPM:File>
                           <QPM:File>BUGS</QPM:File>
                           <QPM:File>COPYING</QPM:File>
                           <QPM:File>MSDOS-Ethernet-Howto.txt</QPM:File>
                           <QPM:File>MSDOS-Howto.txt</QPM:File>
                           <QPM:File>MinixVmd-Howto.txt</QPM:File>
                           <QPM:File>NEWS</QPM:File>
                           <QPM:File>NLS-Howto.txt</QPM:File>
                           <QPM:File>PETdoc.txt</QPM:File>
                           <QPM:File>Readme.beos</QPM:File>
                           <QPM:File>Readme.dos</QPM:File>
                           <QPM:File>ReadmeMacOSX.txt</QPM:File>
                           <QPM:File>TODO</QPM:File>
                           <QPM:File>Walkthrough-Howto.txt</QPM:File>
                           <QPM:File>Win32-Howto.txt</QPM:File>
                           <QPM:File>cbm_basic_tokens.txt</QPM:File>
                           <QPM:File>drive_info.txt</QPM:File>
                           <QPM:File>iec-bus.txt</QPM:File>
                           <QPM:File>index.html</QPM:File>
                           <QPM:File>mon.txt</QPM:File>
                           <QPM:File>new.gif</QPM:File>
                           <QPM:File>serial.txt</QPM:File>
                           <QPM:File>vice-logo.jpg</QPM:File>
                           <QPM:File>vice_1.html</QPM:File>
                           <QPM:File>vice_10.html</QPM:File>
                           <QPM:File>vice_11.html</QPM:File>
                           <QPM:File>vice_12.html</QPM:File>
                           <QPM:File>vice_13.html</QPM:File>
                           <QPM:File>vice_14.html</QPM:File>
                           <QPM:File>vice_15.html</QPM:File>
                           <QPM:File>vice_16.html</QPM:File>
                           <QPM:File>vice_2.html</QPM:File>
                           <QPM:File>vice_3.html</QPM:File>
                           <QPM:File>vice_4.html</QPM:File>
                           <QPM:File>vice_5.html</QPM:File>
                           <QPM:File>vice_6.html</QPM:File>
                           <QPM:File>vice_7.html</QPM:File>
                           <QPM:File>vice_8.html</QPM:File>
                           <QPM:File>vice_9.html</QPM:File>
                           <QPM:File>vice_toc.html</QPM:File>
                        </QPM:Dir>

                        <QPM:Dir name="fonts">
                           <QPM:File>fonts.dir</QPM:File>
                           <QPM:File>vice-cbm.pcf</QPM:File>
                        </QPM:Dir>
                     </QPM:Dir>
                  </QPM:Dir>

                  <QPM:Dir name="man">
                     <QPM:Dir name="man1">
                        <QPM:File>c1541.1</QPM:File>
                        <QPM:File>petcat.1</QPM:File>
                        <QPM:File>vice.1</QPM:File>
                     </QPM:Dir>
                  </QPM:Dir>
               </QPM:Dir>

               <QPM:Dir name="usr">
                  <QPM:Union link="../opt/info">info</QPM:Union>
                  <QPM:Union link="../opt/bin">bin</QPM:Union>
                  <QPM:Union link="../\$(PROCESSOR)/opt/bin">bin</QPM:Union>
                  <QPM:Dir name="lib">
                     <QPM:Dir name="locale">
                        <QPM:Dir name="de">
                           <QPM:Union link="../../../../opt/lib/locale/de/LC_MESSAGES">LC_MESSAGES</QPM:Union>
                        </QPM:Dir>

                        <QPM:Dir name="fr">
                           <QPM:Union link="../../../../opt/lib/locale/fr/LC_MESSAGES">LC_MESSAGES</QPM:Union>
                        </QPM:Dir>

                        <QPM:Dir name="hu">
                           <QPM:Union link="../../../../opt/lib/locale/hu/LC_MESSAGES">LC_MESSAGES</QPM:Union>
                        </QPM:Dir>

                        <QPM:Dir name="it">
                           <QPM:Union link="../../../../opt/lib/locale/it/LC_MESSAGES">LC_MESSAGES</QPM:Union>
                        </QPM:Dir>

                        <QPM:Dir name="nl">
                           <QPM:Union link="../../../../opt/lib/locale/nl/LC_MESSAGES">LC_MESSAGES</QPM:Union>
                        </QPM:Dir>

                        <QPM:Dir name="pl">
                           <QPM:Union link="../../../../opt/lib/locale/pl/LC_MESSAGES">LC_MESSAGES</QPM:Union>
                        </QPM:Dir>

                        <QPM:Dir name="sv">
                           <QPM:Union link="../../../../opt/lib/locale/sv/LC_MESSAGES">LC_MESSAGES</QPM:Union>
                        </QPM:Dir>
                     </QPM:Dir>

                     <QPM:Dir name="vice">
                        <QPM:Union link="../../../opt/lib/vice/fonts">fonts</QPM:Union>
                        <QPM:Union link="../../../opt/lib/vice/doc">doc</QPM:Union>
                        <QPM:Union link="../../../opt/lib/vice/VIC20">VIC20</QPM:Union>
                        <QPM:Union link="../../../opt/lib/vice/PRINTER">PRINTER</QPM:Union>
                        <QPM:Union link="../../../opt/lib/vice/PLUS4">PLUS4</QPM:Union>
                        <QPM:Union link="../../../opt/lib/vice/PET">PET</QPM:Union>
                        <QPM:Union link="../../../opt/lib/vice/DRIVES">DRIVES</QPM:Union>
                        <QPM:Union link="../../../opt/lib/vice/CBM-II">CBM-II</QPM:Union>
                        <QPM:Union link="../../../opt/lib/vice/C64">C64</QPM:Union>
                        <QPM:Union link="../../../opt/lib/vice/C64DTV">C64DTV</QPM:Union>
                        <QPM:Union link="../../../opt/lib/vice/C128">C128</QPM:Union>
                     </QPM:Dir>
                  </QPM:Dir>

                  <QPM:Dir name="man">
                     <QPM:Union link="../../opt/man/man1">man1</QPM:Union>
                  </QPM:Dir>
               </QPM:Dir>
            </QPM:Dir>
         </QPM:Listing>
      </QPM:FileManifest>

      <QPM:Launch name="x64">
         <QPM:String name="Topic" value="Applications/Emulators"/>
         <QPM:String name="Command" value="/opt/bin/x64"/>
         <QPM:String name="Icon" value="/usr/share/icons/topics/chameleon.gif"/>
      </QPM:Launch>

      <QPM:Launch name="x64 (no sound)">
         <QPM:String name="Topic" value="Applications/Emulators"/>
         <QPM:String name="Command" value="/opt/bin/x64 -sounddev dummy"/>
         <QPM:String name="Icon" value="/usr/share/icons/topics/chameleon.gif"/>
      </QPM:Launch>

      <QPM:Launch name="x64dtv">
         <QPM:String name="Topic" value="Applications/Emulators"/>
         <QPM:String name="Command" value="/opt/bin/x64dtv"/>
         <QPM:String name="Icon" value="/usr/share/icons/topics/chameleon.gif"/>
      </QPM:Launch>

      <QPM:Launch name="x64dtv (no sound)">
         <QPM:String name="Topic" value="Applications/Emulators"/>
         <QPM:String name="Command" value="/opt/bin/x64dtv -sounddev dummy"/>
         <QPM:String name="Icon" value="/usr/share/icons/topics/chameleon.gif"/>
      </QPM:Launch>

      <QPM:Launch name="x128">
         <QPM:String name="Topic" value="Applications/Emulators"/>
         <QPM:String name="Command" value="/opt/bin/x128"/>
         <QPM:String name="Icon" value="/usr/share/icons/topics/chameleon.gif"/>
      </QPM:Launch>

      <QPM:Launch name="x128 (no sound)">
         <QPM:String name="Topic" value="Applications/Emulators"/>
         <QPM:String name="Command" value="/opt/bin/x128 -sounddev dummy"/>
         <QPM:String name="Icon" value="/usr/share/icons/topics/chameleon.gif"/>
      </QPM:Launch>

      <QPM:Launch name="xvic">
         <QPM:String name="Topic" value="Applications/Emulators"/>
         <QPM:String name="Command" value="/opt/bin/xvic"/>
         <QPM:String name="Icon" value="/usr/share/icons/topics/chameleon.gif"/>
      </QPM:Launch>

      <QPM:Launch name="xvic (no sound)">
         <QPM:String name="Topic" value="Applications/Emulators"/>
         <QPM:String name="Command" value="/opt/bin/xvic -sounddev dummy"/>
         <QPM:String name="Icon" value="/usr/share/icons/topics/chameleon.gif"/>
      </QPM:Launch>

      <QPM:Launch name="xpet">
         <QPM:String name="Topic" value="Applications/Emulators"/>
         <QPM:String name="Command" value="/opt/bin/xpet"/>
         <QPM:String name="Icon" value="/usr/share/icons/topics/chameleon.gif"/>
      </QPM:Launch>

      <QPM:Launch name="xpet (no sound)">
         <QPM:String name="Topic" value="Applications/Emulators"/>
         <QPM:String name="Command" value="/opt/bin/xpet -sounddev dummy"/>
         <QPM:String name="Icon" value="/usr/share/icons/topics/chameleon.gif"/>
      </QPM:Launch>

      <QPM:Launch name="xplus4">
         <QPM:String name="Topic" value="Applications/Emulators"/>
         <QPM:String name="Command" value="/opt/bin/xplus4"/>
         <QPM:String name="Icon" value="/usr/share/icons/topics/chameleon.gif"/>
      </QPM:Launch>

      <QPM:Launch name="xplus4 (no sound)">
         <QPM:String name="Topic" value="Applications/Emulators"/>
         <QPM:String name="Command" value="/opt/bin/xplus4 -sounddev dummy"/>
         <QPM:String name="Icon" value="/usr/share/icons/topics/chameleon.gif"/>
      </QPM:Launch>

      <QPM:Launch name="xcbm2">
         <QPM:String name="Topic" value="Applications/Emulators"/>
         <QPM:String name="Command" value="/opt/bin/xcbm2"/>
         <QPM:String name="Icon" value="/usr/share/icons/topics/chameleon.gif"/>
      </QPM:Launch>

      <QPM:Launch name="xcbm2 (no sound)">
         <QPM:String name="Topic" value="Applications/Emulators"/>
         <QPM:String name="Command" value="/opt/bin/xcbm2 -sounddev dummy"/>
         <QPM:String name="Icon" value="/usr/share/icons/topics/chameleon.gif"/>
      </QPM:Launch>
   </RDF:Description>
</RDF:RDF>
_END

  cat >public/VICE/core-$VICEVERSION/MANIFEST manifest.01 manifest.02 manifest.03 \
      manifest.04 manifest.05 manifest.06 manifest.07 manifest.08 manifest.09 \
      manifest.10 manifest.11 manifest.12 manifest.14 manifest.15

  tar cf VICE-core.tar public
  gzip VICE-core.tar
  mv VICE-core.tar.gz VICE-$VICEVERSION-public.qpk

  ls -l -R VICE-$VICEVERSION-public.qpk >size.tmp
  manifest_size=`./getsize size.tmp` 
  rm -f -r size.tmp

  echo >manifest.04 "            <QPM:PackageSize>$manifest_size</QPM:PackageSize>"

  cat >VICE-$VICEVERSION-public.qpm manifest.01 manifest.02 manifest.03 manifest.04 \
      manifest.05 manifest.06 manifest.07 manifest.08 manifest.09 manifest.10 \
      manifest.11 manifest.12 manifest.14 manifest.15

  rm -f -r public

  tar cf VICE-x86.tar VICE-$VICEVERSION-public.qp? VICE-$VICEVERSION-public.repdata VICE-$VICEVERSION-x86-public.qp?
  gzip VICE-x86.tar
  mv VICE-x86.tar.gz VICE-$VICEVERSION-x86-public.qpr

  rm -f -r *.qpk *.qpm *.repdata manifest.* VICE-$VICEVERSION

  echo QNX 6 port binary package part generated as VICE-$VICEVERSION-x86-public.qpr
else
  echo QNX 6 port binary distribution directory generated as VICE-$VICEVERSION
fi
