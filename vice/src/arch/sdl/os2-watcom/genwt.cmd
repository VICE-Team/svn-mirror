rem genwt.cmd - generate files for the OS/2 SDL port on OS/2
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

@echo off
echo "Generating files for OS/2 SDL"
copy ..\..\..\translate.txt trans.txt
copy ..\..\..\..\doc\vice.texi vice.tex
copy ..\..\..\buildtools\infocontrib.sed ic.sed
copy ..\..\..\..\AUTHORS authors.txt
copy ..\..\unix\macosx\Resources\Credits.html credits.htm
copy ..\..\..\..\README readme.tx1
copy ..\..\..\..\doc\html\index.html index.htm
copy ..\..\os2\dialogs\dialogs.rc dlgs.rc
copy ..\..\..\..\man\vice.1 vice1.txt
geninfocontribos2 ..\ ..\ watcom\vice.tex watcom\ic.h watcom\ic.sed watcom\authors.txt watcom\credits.htm watcom\readme.tx1 watcom\index.htm watcom\dlgs.rc watcom\vice1.txt
gentranslateos2 trans.txt trans.h transtab.h
copy ic.h ..\..\..\infocontrib.h 
copy trans.h ..\..\..\translate.h
copy transtab.h ..\..\..\translate_table.h
copy authors.txt ..\..\..\..\AUTHORS
copy credits.htm ..\..\unix\macosx\Resources\Credits.html
copy readme.tx1 ..\..\..\..\README
copy index.htm ..\..\..\..\doc\html\index.html
copy dlgs.rc ..\..\os2\dialogs\dialogs.rc
copy vice1.txt ..\..\..\..\man\vice.1
del ic.h vice.tex ic.sed trans.txt trans.h transtab.h authors.txt credits.htm readme.tx1 index.htm dlgs.rc vice1.txt
