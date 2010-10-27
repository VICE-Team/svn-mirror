@echo off
copy ..\res.txt res.txt
copy ..\resdialogs.txt resd.txt
copy ..\resmenu.txt resm.txt
copy ..\resstrings.txt ress.txt
copy ..\..\..\translate.txt trans.txt
if Windows_NT == %OS% goto winnt
goto use16
:winnt
if defined ProgramFiles(x86) GOTO use32
:use16
genwr res.h menuid.h res.txt resd.txt resm.txt ress.txt
gentr trans.txt trans.h transtab.h
goto after
:use32
genwinres32 res.h menuid.h res.txt resd.txt resm.txt ress.txt
gentranslate32 trans.txt trans.h transtab.h
:after
copy res.h ..\res.h
copy menuid.h ..\menuid.h
copy trans.h ..\..\..\translate.h
copy transtab.h ..\..\..\translate_table.h
del res.txt resd.txt resm.txt ress.txt trans.txt res.h menuid.h trans.h transtab.h
