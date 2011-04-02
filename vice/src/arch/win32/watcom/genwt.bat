@echo off
copy ..\res.txt res.txt
copy ..\resdialogs.txt resd.txt
copy ..\resmenu.txt resm.txt
copy ..\resstrings.txt ress.txt
copy ..\..\..\translate.txt trans.txt
copy ..\..\..\..\doc\vice.texi vice.tex
copy ..\..\..\infocontrib.sed ic.sed
if Windows_NT == %OS% goto winnt
goto use16
:winnt
if defined ProgramFiles(x86) GOTO use32
:use16
genic vice.tex ic.h ic.sed
genwr res.h menuid.h strid.h res.txt resd.txt resm.txt ress.txt
gentr trans.txt trans.h transtab.h
goto after
:use32
geninfocontrib32 vice.tex ic.h ic.sed
genwinres32 res.h menuid.h strid.h res.txt resd.txt resm.txt ress.txt
gentranslate32 trans.txt trans.h transtab.h
:after
copy ic.h ..\..\..\infocontrib.h 
copy res.h ..\res.h
copy menuid.h ..\menuid.h
copy strid.h ..\stringid.h
copy trans.h ..\..\..\translate.h
copy transtab.h ..\..\..\translate_table.h
del ic.h vice.tex ic.sed res.txt resd.txt resm.txt ress.txt trans.txt res.h menuid.h strid.h trans.h transtab.h
