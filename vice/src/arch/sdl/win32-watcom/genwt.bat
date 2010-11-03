@echo off
copy ..\..\..\translate.txt trans.txt
if Windows_NT == %OS% goto winnt
goto use16
:winnt
if defined ProgramFiles(x86) GOTO use32
:use16
gentr trans.txt trans.h transtab.h
goto after
:use32
gentranslate32 trans.txt trans.h transtab.h
:after
copy trans.h ..\..\..\translate.h
copy transtab.h ..\..\..\translate_table.h
del trans.txt trans.h transtab.h
