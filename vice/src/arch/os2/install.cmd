/*           Install the Vice/2-Folder 
*/

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */

 call RxFuncAdd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
 call SysLoadFuncs

'cls'
say ''
Say 'Installation of Desktop-Objects for Vice/2'
Say '------------------------------------------'
Say ''
Say 'VICE is the Versatile Commodore Emulator'
Say ' (c) by the VICE_Team'
Say ''
say 'This program and all files are released under the GPL.'
Say 'For Information see ''COPYING''.'
say ''

/* Get curent directory */
origdir = directory()
curdir  = origdir

say 'Current directory is' curdir
say ''

/* ----- VICE/2 Folder ----- */

say 'Creating a Vice/2 folder on the desktop...'
setupstring="OBJECTID=<VICE2>;" ||,
            "ICONFILE="   || curdir ||"\icons\folder1.ico;" ||,
            "ICONNFILE=1,"|| curdir ||"\icons\folder2.ico"
if SysCreateObject("WPFolder","Vice/2","<WP_DESKTOP>",setupstring,"update")<>1 then
do
    say '!!! Can''t create the folder. Exiting...'
    say ''
    exit
end

/* ----- VICE/2 Program Objects ----- */

say 'Creating Program objects...'

CALL createProgram "C=64",        'x64.exe',     '*.d64,*.d64.gz,*.d64.zip,*.g64,*.g64.gz,*.g64.zip,*.p64,*.p64.gz,*.x64,*.x64.gz,*.x64.zip,*.t64,*.t64.gz,*.d64.zip,*.tap,*.tap.gz'
CALL createProgram "C=64SC",      'x64sc.exe',   '*.d64,*.d64.gz,*.d64.zip,*.g64,*.g64.gz,*.g64.zip,*.p64,*.p64.gz,*.x64,*.x64.gz,*.x64.zip,*.t64,*.t64.gz,*.d64.zip,*.tap,*.tap.gz'
CALL createProgram "C=64DTV",     'x64dtv.exe',  '*.d64,*.d64.gz,*.d64.zip,*.g64,*.g64.gz,*.g64.zip,*.p64,*.p64.gz,*.x64,*.x64.gz,*.x64.zip,*.t64,*.t64.gz,*.d64.zip'
CALL createProgram "C=SCPU64",    'xscpu64.exe',   '*.d64,*.d64.gz,*.d64.zip,*.g64,*.g64.gz,*.g64.zip,*.p64,*.p64.gz,*.x64,*.x64.gz,*.x64.zip,*.t64,*.t64.gz,*.d64.zip,*.tap,*.tap.gz'
CALL createProgram "C=128",       'x128.exe',    '*.d71,*.d71.gz,*.d81,*.d81.gz'
CALL createProgram "VIC 20",      'xvic.exe',    '*.d64,*.d64.gz,*.d64.zip,*.g64,*.g64.gz,*.g64.zip,*.p64,*.p64.gz,*.x64,*.x64.gz,*.x64.zip,*.t64,*.t64.gz,*.d64.zip,*.tap,*.tap.gz'
CALL createProgram "PET",         'xpet.exe',    '*.d80,*.d80.gz,*.d80.zip,*.d82,*.d82.gz,*.d82.zip'
CALL createProgram "Plus4",       'xplus4.exe',  '*.d64,*.d64.gz,*.d64.zip,*.g64,*.g64.gz,*.g64.zip,*.p64,*.p64.gz,*.x64,*.x64.gz,*.x64.zip,*.t64,*.t64.gz,*.d64.zip,*.tap,*.tap.gz'
CALL createProgram "CBM-6x0/7x0", 'xcbm2.exe',   '*.d80,*.d80.gz,*.d80.zip,*.d82,*.d82.gz,*.d82.zip'
CALL createProgram "CBM-5x0",     'xcbm5x0.exe', '*.d80,*.d80.gz,*.d80.zip,*.d82,*.d82.gz,*.d82.zip'
CALL createProgramIcon "C=1541", 'x1541.exe', 'c1541.ico', '', ''
CALL createProgramIcon "reSID Player", 'vsid.exe', 'vsid.ico', '-vsid %*', '*.sid'

setupstring='EXENAME=e.exe;PARAMETERS='curdir'\vice2.log'
if SysCreateObject("WPProgram","log-File","<VICE2>",setupstring,"update")<>1 then
do
    say '!!! Can''t create a program object for log-file.'
    say ''
end

setupstring='EXENAME=e.exe;PARAMETERS='curdir'\vice2.cfg'
if SysCreateObject("WPProgram","cfg-file","<VICE2>",setupstring,"update")<>1 then
do
    say '!!! Can''t create a program object for cfg-file.'
    say ''
end
/*CALL createShadow "<VICE2>", 'vice.log'*/

/* ----- VICE/2 Documentation ----- */

say 'Creating objects for documentation...'

setupstring='EXENAME=view.exe;PARAMETERS=vice.inf;STARTUPDIR='curdir'\doc'
if SysCreateObject("WPProgram","Vice Manual","<VICE2>",setupstring,"update")<>1 then
do
    say '!!! Can''t create a program object for the Vice Manual'
    say ''
end

curdir = directory(origdir'\doc')

CALL createShadow "<VICE2>", 'Vice_for_OS2.txt'

if SysCreateObject("WPFolder","Documentation","<VICE2>","OBJECTID=<VICE2_DOC>","update")<>1 then
do
    say '!!! Can''t create the subfolder. Exiting...'
    say ''
    exit
end

setupstring='URL=file://'curdir'\html\vice_toc.html'
if SysCreateObject("WPUrl","HTML Documentation","<VICE2_DOC>",setupstring,"update")<>1 then
do
    say '!!! Can''t create a html object for HTML Documatation'
    say ''
end

if SysCreateObject("WPFolder","Read me!","<VICE2_DOC>","OBJECTID=<VICE2_README>","update")<>1 then
do
    say '!!! Can''t create the subfolder. Exiting...'
    say ''
    exit
end

curdir = directory(origdir'\doc\readme')

CALL createShadow "<VICE2_README>", 'AUTHORS'
CALL createShadow "<VICE2_README>", 'BUGS'
CALL createShadow "<VICE2_README>", 'COPYING'
CALL createShadow "<VICE2_README>", 'FEEDBACK'
CALL createShadow "<VICE2_README>", 'NEWS'
CALL createShadow "<VICE2_README>", 'README'
CALL createShadow "<VICE2_README>", 'TODO'

if SysCreateObject("WPFolder","Additional Info","<VICE2_DOC>","OBJECTID=<VICE2_TEXT>","update")<>1 then
do
    say '!!! Can''t create the subfolder. Exiting...'
    say ''
    exit
end

curdir = directory(origdir'\doc\txt')

/*
 CALL createShadow "<VICE2_TEXT>", 'Evaluation'
 */

say 'Creating a Vice/2 subfolder ''Online''...'
if SysCreateObject("WPFolder","Vice Online","<VICE2_DOC>","OBJECTID=<VICE2_ONLINE>","update")<>1 then
do
    say '!!! Can''t create the subfolder. Exiting...'
    say ''
    exit
end

if SysCreateObject("WPUrl","Vice Homepage","<VICE2_ONLINE>","URL=http://vice-emu.sourceforge.net/","update")<>1 then
do
    say '!!! Can''t create a html object for HTML Documatation'
end

if SysCreateObject("WPUrl","Vice/2 Homepage","<VICE2_ONLINE>","URL=http://www.d4m.de/vice2/","update")<>1 then
do
    say '!!! Can''t create a html object for HTML Documatation'
end

curdir = directory(origdir)

/* Open the Vice/2 folder */
rc=SysSetObjectData("<VICE2>","OPEN=DEFAULT")

say ''
say 'Installation succesful.'
say ''
say 'Remark: It is not necessary to reboot. The installation script'
say 'does nothing else than creating desctop object. Uninstallation'
say 'can be done by removing the Vice2-Folder easily.'
say ''

exit

createProgram:
    /* STARTUPDIR needed for Drag&Drop */
    setupstring='EXENAME='curdir'\'ARG(2)';STARTUPDIR='curdir';ASSOCFILTER='ARG(3)
    /*    say 'Creating an object for 'ARG(2)'...'*/
    if SysCreateObject("WPProgram",ARG(1),"<VICE2>",setupstring,"update")<>1 then
    do
        say '!!! Can''t create a program object for 'ARG(1)
        say ''
    end
    RETURN
    
createProgramIcon:
    /* STARTUPDIR needed for Drag&Drop */
    setupstring='EXENAME='curdir'\'ARG(2)';ICONFILE='curdir'\icons\'ARG(3)';PARAMETERS='ARG(4)';STARTUPDIR='curdir';ASSOCFILTER='ARG(5)
    /*    say 'Creating an object for 'ARG(2)'...'*/
    if SysCreateObject("WPProgram",ARG(1),"<VICE2>",setupstring,"update")<>1 then
    do
        say '!!! Can''t create a program object for 'ARG(1)
        say ''
    end
    RETURN
    
createShadow:
/*    say 'Creating shadow for 'ARG(2)'...'*/
    setupstring='SHADOWID='curdir'\'ARG(2)
    if SysCreateObject("WPShadow",ARG(2),ARG(1),setupstring,"update")<>1 then
    do
        say '!!! Can''t create the shadow for 'curdir'\'ARG(2)
        say ''
    end
    RETURN
