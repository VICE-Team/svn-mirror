@echo off
copy ..\res.txt res.txt
copy ..\resdialogs.txt resd.txt
copy ..\resmenu.txt resm.txt
copy ..\resstrings.txt ress.txt
copy ..\..\..\translate.txt trans.txt
copy ..\..\..\..\doc\vice.texi vice.tex
copy ..\..\..\infocontrib.sed ic.sed
copy ..\..\..\..\AUTHORS authors.txt
copy ..\..\unix\macosx\Resources\Credits.html credits.htm
copy ..\..\..\..\README readme.txt
copy ..\..\..\..\doc\html\index.html index.htm
copy ..\..\os2\dialogs\dialogs.rc dlgs.rc
copy ..\..\..\..\man\vice.1 vice1.txt
if Windows_NT == %OS% goto winnt
goto use16
:winnt
if defined ProgramFiles(x86) GOTO use32
:use16
genic ..\ ..\ watcom\vice.tex watcom\ic.h watcom\ic.sed watcom\authors.txt watcom\credits.htm watcom\readme.txt watcom\index.htm watcom\dlgs.rc watcom\vice1.txt
genwr res.h menuid.h strid.h res.txt resd.txt resm.txt ress.txt
gentr trans.txt trans.h transtab.h
goto after
:use32
geninfocontrib32 ..\ ..\ watcom\vice.tex watcom\ic.h watcom\ic.sed watcom\authors.txt watcom\credits.htm watcom\readme.txt watcom\index.htm watcom\dlgs.rc watcom\vice1.txt
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
copy dlgs.rc ..\..\os2\dialogs\dialogs.rc
copy vice1.txt ..\..\..\..\man\vice.1
del ic.h vice.tex ic.sed res.txt resd.txt resm.txt ress.txt trans.txt res.h menuid.h strid.h trans.h transtab.h authors.txt credits.htm readme.txt index.htm dlgs.rc vice1.txt
