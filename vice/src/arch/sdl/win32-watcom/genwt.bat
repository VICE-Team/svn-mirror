@echo off
copy ..\..\..\translate.txt trans.txt
copy ..\..\..\..\doc\vice.texi vice.tex
copy ..\..\..\infocontrib.sed ic.sed
if Windows_NT == %OS% goto winnt
goto use16
:winnt
if defined ProgramFiles(x86) GOTO use32
:use16
genic vice.tex ic.h ic.sed
gentr trans.txt trans.h transtab.h
goto after
:use32
geninfocontrib32 vice.tex ic.h ic.sed
gentranslate32 trans.txt trans.h transtab.h
:after
copy ic.h ..\..\..\infocontrib.h
copy trans.h ..\..\..\translate.h
copy transtab.h ..\..\..\translate_table.h
del ic.h ic.sed vice.tex trans.txt trans.h transtab.h
