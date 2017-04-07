@echo off

rem genwt.bat - Generate files for windows
rem
rem Written by
rem  Marco van den Heuvel <blackystardust68@yahoo.com>
rem
rem This file is part of VICE, the Versatile Commodore Emulator.
rem See README for copyright notice.
rem
rem  This program is free software; you can redistribute it and/or modify
rem  it under the terms of the GNU General Public License as published by
rem  the Free Software Foundation; either version 2 of the License, or
rem  (at your option) any later version.
rem
rem  This program is distributed in the hope that it will be useful,
rem  but WITHOUT ANY WARRANTY; without even the implied warranty of
rem  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
rem  GNU General Public License for more details.
rem
rem  You should have received a copy of the GNU General Public License
rem  along with this program; if not, write to the Free Software
rem  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
rem  02111-1307  USA.

copy ..\res.txt res.txt
copy ..\resdialogs.txt resd.txt
copy ..\resmenu.txt resm.txt
copy ..\resstrings.txt ress.txt
copy ..\..\..\translate.txt trans.txt
copy ..\..\..\..\doc\vice.texi vice.tex
copy ..\..\..\buildtools\infocontrib.sed ic.sed
copy ..\..\..\..\AUTHORS authors.txt
copy ..\..\unix\macosx\Resources\Credits.html credits.htm
copy ..\..\..\..\README readme.txt
copy ..\..\..\..\doc\html\index.html index.htm
copy ..\..\..\..\man\vice.1 vice1.txt
if Windows_NT == %OS% goto winnt
goto use16
:winnt
if defined ProgramFiles(x86) GOTO use32
:use16
genic ..\ ..\ watcom\vice.tex watcom\ic.h watcom\ic.sed watcom\authors.txt watcom\credits.htm watcom\readme.txt watcom\index.htm watcom\vice1.txt
genwr res.h menuid.h strid.h res.txt resd.txt resm.txt ress.txt
gentr trans.txt trans.h transtab.h
goto after
:use32
geninfocontrib32 ..\ ..\ watcom\vice.tex watcom\ic.h watcom\ic.sed watcom\authors.txt watcom\credits.htm watcom\readme.txt watcom\index.htm watcom\vice1.txt
genwinres32 res.h menuid.h strid.h res.txt resd.txt resm.txt ress.txt
gentranslate32 trans.txt trans.h transtab.h
:after
copy ic.h ..\..\..\infocontrib.h 
copy res.h ..\res.h
copy menuid.h ..\menuid.h
copy strid.h ..\stringid.h
copy trans.h ..\..\..\translate.h
copy transtab.h ..\..\..\translate_table.h
copy authors.txt ..\..\..\..\AUTHORS
copy credits.htm ..\..\unix\macosx\Resources\Credits.html
copy readme.txt ..\..\..\..\README
copy index.htm ..\..\..\..\doc\html\index.html
copy vice1.txt ..\..\..\..\man\vice.1
del ic.h vice.tex ic.sed res.txt resd.txt resm.txt ress.txt trans.txt res.h menuid.h strid.h trans.h transtab.h authors.txt credits.htm readme.txt index.htm vice1.txt
