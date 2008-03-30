/*
 * info.c - Info about the VICE project, including the GPL.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
 *
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */

/* FIXME: For now, we avoid ISO characters completely to avoid problems with
   MS-DOS.  But we should provide some automatic conversion instead.  */

const char license_text[] =
"                 GNU GENERAL PUBLIC LICENSE\n"
"                    Version 2, June 1991\n"
"\n"
" Copyright (C) 1989, 1991 Free Software Foundation, Inc. \n"
"    59 Temple Place, Suite 330, Boston, MA  02111-1307  USA\n"
"\n"
" Everyone is permitted to copy and distribute verbatim copies of\n"
" this license document, but changing it is not allowed.\n"
"\n"
"\n"
"                         Preamble\n"
"\n"
"The licenses for most software are designed to take away your\n"
"freedom to share and change it. By contrast, the GNU General\n"
"Public License is intended to guarantee your freedom to share and\n"
"change free software--to make sure the software is free for all\n"
"its users.  This General Public License applies to most of the\n"
"Free Software Foundation's software and to any other program\n"
"whose authors commit to using it.  (Some other Free Software\n"
"Foundation software is covered by the GNU Library General Public\n"
"License instead.)  You can apply it to your programs, too.\n"
"\n"
"When we speak of free software, we are referring to freedom, not\n"
"price.  Our General Public Licenses are designed to make sure\n"
"that you have the freedom to distribute copies of free software\n"
"(and charge for this service if you wish), that you receive\n"
"source code or can get it if you want it, that you can change the\n"
"software or use pieces of it in new free programs; and that you\n"
"know you can do these things.\n"
"\n"
"To protect your rights, we need to make restrictions that forbid\n"
"anyone to deny you these rights or to ask you to surrender the\n"
"rights.  These restrictions translate to certain responsibilities\n"
"for you if you distribute copies of the software, or if you\n"
"modify it.\n"
"\n"
"For example, if you distribute copies of such a program, whether\n"
"gratis or for a fee, you must give the recipients all the rights\n"
"that you have.  You must make sure that they, too, receive or can\n"
"get the source code.  And you must show them these terms so they\n"
"know their rights.\n"
"\n"
"We protect your rights with two steps: (1) copyright the\n"
"software, and (2) offer you this license which gives you legal\n"
"permission to copy, distribute and/or modify the software.\n"
"\n"
"Also, for each author's protection and ours, we want to make\n"
"certain that everyone understands that there is no warranty for\n"
"this free software.  If the software is modified by someone else\n"
"and passed on, we want its recipients to know that what they have\n"
"is not the original, so that any problems introduced by others\n"
"will not reflect on the original authors' reputations.\n"
"\n"
"Finally, any free program is threatened constantly by software\n"
"patents.  We wish to avoid the danger that redistributors of a\n"
"free program will individually obtain patent licenses, in effect\n"
"making the program proprietary.  To prevent this, we have made it\n"
"clear that any patent must be licensed for everyone's free use or\n"
"not licensed at all.\n"
"\n"
"The precise terms and conditions for copying, distribution and\n"
"modification follow.\n"
"\n"
"\n"
"GNU GENERAL PUBLIC LICENSE TERMS AND CONDITIONS FOR\n"
"COPYING, DISTRIBUTION AND MODIFICATION\n"
"\n"
"0. This License applies to any program or other work which\n"
"contains a notice placed by the copyright holder saying it may be\n"
"distributed under the terms of this General Public License.  The\n"
"\"Program\", below, refers to any such program or work, and a\n"
"\"work based on the Program\" means either the Program or any\n"
"derivative work under copyright law: that is to say, a work\n"
"containing the Program or a portion of it, either verbatim or\n"
"with modifications and/or translated into another language.\n"
"(Hereinafter, translation is included without limitation in the\n"
"term \"modification\".)  Each licensee is addressed as \"you\".\n"
"\n"
"Activities other than copying, distribution and modification are\n"
"not covered by this License; they are outside its scope.  The act\n"
"of running the Program is not restricted, and the output from the\n"
"Program is covered only if its contents constitute a work based\n"
"on the Program (independent of having been made by running the\n"
"Program).  Whether that is true depends on what the Program does.\n"
"\n"
"  1. You may copy and distribute verbatim copies of the Program's\n"
"source code as you receive it, in any medium, provided that you\n"
"conspicuously and appropriately publish on each copy an\n"
"appropriate copyright notice and disclaimer of warranty; keep\n"
"intact all the notices that refer to this License and to the\n"
"absence of any warranty; and give any other recipients of the\n"
"Program a copy of this License along with the Program.\n"
"\n"
"You may charge a fee for the physical act of transferring a copy,\n"
"and you may at your option offer warranty protection in exchange\n"
"for a fee.\n"
"\n"
"  2. You may modify your copy or copies of the Program or any\n"
"portion of it, thus forming a work based on the Program, and copy\n"
"and distribute such modifications or work under the terms of\n"
"Section 1 above, provided that you also meet all of these\n"
"conditions:\n"
"\n"
"    a) You must cause the modified files to carry prominent\n"
"    notices stating that you changed the files and the date of\n"
"    any change.\n"
"\n"
"    b) You must cause any work that you distribute or publish,\n"
"    that in whole or in part contains or is derived from the\n"
"    Program or any part thereof, to be licensed as a whole at no\n"
"    charge to all third parties under the terms of this License.\n"
"\n"
"    c) If the modified program normally reads commands\n"
"    interactively when run, you must cause it, when started\n"
"    running for such interactive use in the most ordinary way, to\n"
"    print or display an announcement including an appropriate\n"
"    copyright notice and a notice that there is no warranty (or\n"
"    else, saying that you provide a warranty) and that users may\n"
"    redistribute the program under these conditions, and telling\n"
"    the user how to view a copy of this License.  (Exception: if\n"
"    the Program itself is interactive but does not normally print\n"
"    such an announcement, your work based on the Program is not\n"
"    required to print an announcement.)\n"
"\n"
"These requirements apply to the modified work as a whole.  If\n"
"identifiable sections of that work are not derived from the\n"
"Program, and can be reasonably considered independent and\n"
"separate works in themselves, then this License, and its terms,\n"
"do not apply to those sections when you distribute them as\n"
"separate works.  But when you distribute the same sections as\n"
"part of a whole which is a work based on the Program, the\n"
"distribution of the whole must be on the terms of this License,\n"
"whose permissions for other licensees extend to the entire whole,\n"
"and thus to each and every part regardless of who wrote it.\n"
"\n"
"Thus, it is not the intent of this section to claim rights or\n"
"contest your rights to work written entirely by you; rather, the\n"
"intent is to exercise the right to control the distribution of\n"
"derivative or collective works based on the Program.\n"
"\n"
"In addition, mere aggregation of another work not based on the\n"
"Program with the Program (or with a work based on the\n"
"Program) on a volume of a storage or distribution medium\n"
"does not bring the other work under the scope of this License.\n"
"\n"
"  3. You may copy and distribute the Program (or a work based on\n"
"it, under Section 2) in object code or executable form under the\n"
"terms of Sections 1 and 2 above provided that you also do one of\n"
"the following:\n"
"\n"
"    a) Accompany it with the complete corresponding\n"
"    machine-readable source code, which must be distributed under\n"
"    the terms of Sections 1 and 2 above on a medium customarily\n"
"    used for software interchange; or,\n"
"\n"
"    b) Accompany it with a written offer, valid for at least\n"
"    three years, to give any third party, for a charge no more\n"
"    than your cost of physically performing source distribution,\n"
"    a complete machine-readable copy of the corresponding source\n"
"    code, to be distributed under the terms of Sections 1 and 2\n"
"    above on a medium customarily used for software interchange;\n"
"    or,\n"
"\n"
"    c) Accompany it with the information you received as to the\n"
"    offer to distribute corresponding source code.  (This\n"
"    alternative is allowed only for noncommercial distribution\n"
"    and only if you received the program in object code or\n"
"    executable form with such an offer, in accord with Subsection\n"
"    b above.)\n"
"\n"
"The source code for a work means the preferred form of the work\n"
"for making modifications to it.  For an executable work, complete\n"
"source code means all the source code for all modules it\n"
"contains, plus any associated interface definition files, plus\n"
"the scripts used to control compilation and installation of the\n"
"executable.  However, as a special exception, the source code\n"
"distributed need not include anything that is normally\n"
"distributed (in either source or binary form) with the major\n"
"components (compiler, kernel, and so on) of the operating system\n"
"on which the executable runs, unless that component itself\n"
"accompanies the executable.\n"
"\n"
"If distribution of executable or object code is made by offering\n"
"access to copy from a designated place, then offering equivalent\n"
"access to copy the source code from the same place counts as\n"
"distribution of the source code, even though third parties are\n"
"not compelled to copy the source along with the object code.\n"
"\n"
"  4. You may not copy, modify, sublicense, or distribute the\n"
"Program except as expressly provided under this License.  Any\n"
"attempt otherwise to copy, modify, sublicense or distribute the\n"
"Program is void, and will automatically terminate your rights\n"
"under this License.  However, parties who have received copies,\n"
"or rights, from you under this License will not have their\n"
"licenses terminated so long as such parties remain in full\n"
"compliance.\n"
"\n"
"  5. You are not required to accept this License, since you have\n"
"not signed it.  However, nothing else grants you permission to\n"
"modify or distribute the Program or its derivative works.  These\n"
"actions are prohibited by law if you do not accept this License.\n"
"Therefore, by modifying or distributing the Program (or any work\n"
"based on the Program), you indicate your acceptance of this\n"
"License to do so, and all its terms and conditions for copying,\n"
"distributing or modifying the Program or works based on it.\n"
"\n"
"  6. Each time you redistribute the Program (or any work based on\n"
"the Program), the recipient automatically receives a license from\n"
"the original licensor to copy, distribute or modify the Program\n"
"subject to these terms and conditions.  You may not impose any\n"
"further restrictions on the recipients' exercise of the rights\n"
"granted herein.  You are not responsible for enforcing compliance\n"
"by third parties to this License.\n"
"\n"
"  7. If, as a consequence of a court judgment or allegation of\n"
"patent infringement or for any other reason (not limited to\n"
"patent issues), conditions are imposed on you (whether by court\n"
"order, agreement or otherwise) that contradict the conditions of\n"
"this License, they do not excuse you from the conditions of this\n"
"License.  If you cannot distribute so as to satisfy\n"
"simultaneously your obligations under this License and any other\n"
"pertinent obligations, then as a consequence you may not\n"
"distribute the Program at all.  For example, if a patent license\n"
"would not permit royalty-free redistribution of the Program by\n"
"all those who receive copies directly or indirectly through you,\n"
"then the only way you could satisfy both it and this License\n"
"would be to refrain entirely from distribution of the Program.\n"
"\n"
"If any portion of this section is held invalid or unenforceable\n"
"under any particular circumstance, the balance of the section is\n"
"intended to apply and the section as a whole is intended to apply\n"
"in other circumstances.\n"
"\n"
"It is not the purpose of this section to induce you to infringe\n"
"any patents or other property right claims or to contest validity\n"
"of any such claims; this section has the sole purpose of\n"
"protecting the integrity of the free software distribution\n"
"system, which is implemented by public license practices.  Many\n"
"people have made generous contributions to the wide range of\n"
"software distributed through that system in reliance on\n"
"consistent application of that system; it is up to the\n"
"author/donor to decide if he or she is willing to distribute\n"
"software through any other system and a licensee cannot impose\n"
"that choice.\n"
"\n"
"This section is intended to make thoroughly clear what is\n"
"believed to be a consequence of the rest of this License.\n"
"\n"
"  8. If the distribution and/or use of the Program is restricted\n"
"in certain countries either by patents or by copyrighted\n"
"interfaces, the original copyright holder who places the Program\n"
"under this License may add an explicit geographical distribution\n"
"limitation excluding those countries, so that distribution is\n"
"permitted only in or among countries not thus excluded.  In such\n"
"case, this License incorporates the limitation as if written in\n"
"the body of this License.\n"
"\n"
"  9. The Free Software Foundation may publish revised and/or new\n"
"versions of the General Public License from time to time.  Such\n"
"new versions will be similar in spirit to the present version,\n"
"but may differ in detail to address new problems or concerns.\n"
"\n"
"Each version is given a distinguishing version number.  If the\n"
"Program specifies a version number of this License which applies\n"
"to it and \"any later version\", you have the option of following\n"
"the terms and conditions either of that version or of any later\n"
"version published by the Free Software Foundation.  If the\n"
"Program does not specify a version number of this License, you\n"
"may choose any version ever published by the Free Software\n"
"Foundation.\n"
"\n"
"  10. If you wish to incorporate parts of the Program into other\n"
"free programs whose distribution conditions are different, write\n"
"to the author to ask for permission.  For software which is\n"
"copyrighted by the Free Software Foundation, write to the Free\n"
"Software Foundation; we sometimes make exceptions for this.  Our\n"
"decision will be guided by the two goals of preserving the free\n"
"status of all derivatives of our free software and of promoting\n"
"the sharing and reuse of software generally.";


const char warranty_text[] =
"NO WARRANTY\n"
"~~~~~~~~~~~\n"
"\n"
"  11. BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE,\n"
"THERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT\n"
"PERMITTED BY APPLICABLE LAW.  EXCEPT WHEN OTHERWISE\n"
"STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER\n"
"PARTIES PROVIDE THE PROGRAM \"AS IS\" WITHOUT WARRANTY\n"
"OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,\n"
"BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF\n"
"MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.\n"
"THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF\n"
"THE PROGRAM IS WITH YOU.  SHOULD THE PROGRAM PROVE\n"
"DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY\n"
"SERVICING, REPAIR OR CORRECTION.\n"
"\n"
"  12. IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR\n"
"AGREED TO IN WRITING WILL ANY COPYRIGHT HOLDER, OR ANY\n"
"OTHER PARTY WHO MAY MODIFY AND/OR REDISTRIBUTE THE\n"
"PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR\n"
"DAMAGES, INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL OR\n"
"CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OR\n"
"INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED\n"
"TO LOSS OF DATA OR DATA BEING RENDERED INACCURATE OR\n"
"LOSSES SUSTAINED BY YOU OR THIRD PARTIES OR A FAILURE\n"
"OF THE PROGRAM TO OPERATE WITH ANY OTHER PROGRAMS),\n"
"EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF\n"
"THE POSSIBILITY OF SUCH DAMAGES.  ";

const char contrib_text[] =
"VICE derives from X64, the first Commodore 64 emulator for the X
Window System.  Here is an informal list of the people who were
mostly involved in the development of X64 and VICE:

The VICE core team:

  Ettore Perazzoli (ettore@comm2000.it) made the 6510, VIC-II,
  VIC-I and CRTC emulations, part of the hardware-level 1541
  emulation, speed optimizations, bug fixes, the event-driven
  cycle-exact engine, the Xt/Xaw/Xfwf-based GUI for X11, a general
  code reorganization, the new resource handling, most of the
  documentation and the MS-DOS port (well, somebody had to do it).

  Teemu Rantanen (tvr@cs.hut.fi) implemented the SID emulation
  and the trap-based disk drive and serial bus implementation;
  added support for multiple display depths under X11.

  Andre' Fachat (fachat@physik.tu-chemnitz.de) wrote the PET
  emulators, the CIA and VIA emulation, the IEEE488 interface,
  implemented the IEC serial bus in `xvic' and made tons of bug
  fixes.

  Daniel Sladic (sladic@eecg.toronto.edu) started the work on
  hardware-level 1541 emulation and wrote the new monitor
  introduced with VICE 0.15.

  Andreas Boose (boose@unixserv.rz.fh-hannover.de) gave lots of
  information and bug reports about the VIC-II, the 6510 and the
  CIAs; moreover, he wrote several test-routines that were used to
  improve the emulation.  Also contributed some important patches,
  fixed several problems in the 1541 emulation, implemented the
  SpeedDOS parallel cable emulation, and added cartridge support.

Former team members:

  Jouko Valta (jopi@stekt.oulu.fi) wrote `petcat' and `c1541',
  `T64' handling, user service and maintenance (most of the work
  in x64 0.3.x was made by him); retired from the project in July
  96, after VICE 0.10.0.

  Jarkko Sonninen (sonninen@lut.fi) was the founder of the
  project, wrote the old version of the 6502 emulation and the
  XDebugger, and retired from the project after x64 0.2.1.

External contributors:

  Michael Schwendt (sidplay@geocities.com) helped with the SID
  (audio) chip emulation, bringing important suggestions and bug
  reports, as well as the wave tables and filter emulation from
  his SIDplay emulator.

  Christian Bauer (bauec002@goofy.zdv.uni-mainz.de) wrote the
  very interesting \"VIC article\" from which we got invaluable
  information about the VIC-II chip: without this, the VIC-II
  implementation would have not been possible.

  Wolfgang Lorenz wrote an excellent 6510 test suite that helped
  us to debug the CPU emulation.

  Giuliano Procida (gpp10@cam.ac.uk) is the maintainer of the
  VICE `deb' package for the Debian distribution, and also
  helped proofreading the documentation.

  Marko M\"akel\"a (msmakela@nic.funet.fi) wrote lots of CPU
  documentation.

  Martin Pottendorfer (Martin.Pottendorfer@aut.alcatel.at)
  implemented the \"Contents\" button in the file selector and
  provided us with several suggestions and constant testing of
  constantly broken snapshots.

  Chris Sharp (sharpc@hurlsey.ibm.com) wrote the AIX sound
  driver.

  Krister Walfridsson (cato@df.lth.se) implemented joystick and
  sound support for NetBSD.

  Dag Lem (resid@nimrod.no) implemented an alternative cycle-exact
  SID emulator (reSID), and contributed a bugfix in our SID
  emulation.

  Peter Andrew Felvegi aka Petschy (petschy@derceto.sch.bme.hu)
  fixed a couple of bugs in the fast serial emulation.

  Olaf Seibert (rhialto@polder.ubc.kun.nl) contributed some PET,
  and disk drive patches.

  Daniel Fandrich (dan@fch.wimsey.bc.ca) contributed some disk
  drive patches.

  Heiko Selber (selber@fhi-berlin.mpg.de) contributed some VIC20
  I/O patches.

  Steven Tieu (stieu@physics.ubc.ca) added initial support for
  16/24 bpp X11 displays.

  Alexander Lehmann (alex@mathematik.th-darmstadt.de) added
  complete support for all the VIC20 memory configurations for
  the old VICE 0.12.

  Lionel Ulmer implemented joystick support for Linux and a first
  try of a SID emulation for SGI machines.

  Bernhard Kuhn (kuhn@eikon.e-technik.tu-muenchen.de)
  made some joystick improvements for Linux.

  Gerhard Wesp (gwesp@cosy.sbg.ac.at) contributed the
  `extract' command in `c1541'.

  Ricardo Ferreira (storm@esoterica.pt) contributed the
  `unlynx' and `system' commands in `c1541'.

  Tomi Ollila (Tomi.Ollila@tfi.net) donated `findpath.c'.

  Richard Hable (hab@fim.uni-linz.ac.at) contributed the initial
  version of the REU Emulation.

  Vesa-Matti Puro (vmp@lut.fi) wrote the very first 6502 CPU
  emulator in x64 0.1.0. That was the beginning of the story...

  Dan Miner (dminer@nyx10.cs.du.edu) contributed some patches to
  the fast disk drive emulation.

  Frank Prindle (Frank.Prindle@lambada.oit.unc.edu) contributed
  some patches.

  Peter Weighill (stuce@csv.warwick.ac.uk) gave many ideas and
  contributed the ROM patcher.

  Dominique Strigl (strigl@sxb.bsf.alcatel.fr), Craig Jackson
  (Craig.Jackson@lambada.oit.unc.edu) and Lasse Jyrkinen
  ((Lasse.Jyrkinen@uz.kuleuven.ac.be) contributed miscellaneous
  patches in the old X64 times.

  Per Olofsson (MagerValp@Goth.Org) digitalized the C64 colors
  used in the default palette.

  Paul David Doherty (h0142kdd@rz.hu-berlin.de) wrote `zip2disk',
  on which the Zipcode support in `c1541' is based.

  Robert H. Forsman Jr., Brian Totty (totty@cs.uiuc.edu) and
  Robert W. McMullen (rwmcm@orion.ae.utexas.edu) provided the
  widget set for implementing the Xaw GUI.

  Shawn Hargreaves (shawn@talula.demon.co.uk) wrote Allegro, the
  graphics and audio library used in the MS-DOS version.

(We hope we have not forgotten anybody; if you think we have,
please tell us.)

Thanks also to everyone else for sending suggestions, ideas, bug
reports, questions and requests.  In particular, a warm thank
goes to the following people:

  Lutz Sammer
  Ralph Mason (RALPHM@msmail02.liffe.com)
  George Caswell (timbuktu@the-eye.res.wpi.edu)
  Per Olofsson (MagerValp@goth.org)
  Jasper Phillips (philljas@newt.cs.orst.edu)
  Luca Forcucci (frk@geocities.com)
  Asger Alstrup (alstrup@diku.dk)
  Bernhard Schwall (uzs484@uni-bonn.de)
  Salvatore Valente (svalente@mit.edu)
  Arthur Hagen (art@broomstick.com)
  Douglas Carmichael (dcarmich@mcs.com)
  Ferenc Veres (lion@c64.rulez.org)

Last but not least, a very special thank to Andreas Arens, Lutz
Sammer, Edgar Tornig, Christian Bauer, Wolfgang Lorenz, Miha
Peternel and Per H^akan Sundell for writing cool emulators to
compete with.  :-)";

/*
Local Variables:
fill-column: 66
End:
*/

