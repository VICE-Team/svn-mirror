ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³                                                                         ³
³                    Texinfo to OS/2 *.IPF Converter                      ³
³                    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ                      ³
³                                                                         ³
³                             Version 1.00                                ³
³                                                                         ³
³       Author: Andreas Kaiser                                            ³
³               Danziger Str. 4                                           ³
³               D-70597 Stuttgart 70                                      ³
³               Germany                                                   ³
³                                                                         ³
³               Fidonet:  2:246/8506.9                                    ³
³               Internet: ak@ananke.stgt.sub.org                          ³
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

Legal notices:
--------------

     BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO
WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW. 
EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR
OTHER PARTIES PROVIDE THE PROGRAM "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. 
THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE PROGRAM IS
WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF
ALL NECESSARY SERVICING, REPAIR OR CORRECTION.

     IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN
WRITING WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY MODIFY
AND/OR REDISTRIBUTE THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU
FOR DAMAGES, INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL OR
CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OR INABILITY TO USE THE
PROGRAM (INCLUDING BUT NOT LIMITED TO LOSS OF DATA OR DATA BEING
RENDERED INACCURATE OR LOSSES SUSTAINED BY YOU OR THIRD PARTIES OR A
FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER PROGRAMS), EVEN IF
SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH
DAMAGES.

Usage:
------
	texi2ipf [options] Texinfo-source-file IPF-source-file
or
	texi2ipf [options] Texinfo-source-file online-doc-file  
where                          
	IPF-source-file = *.IPF
	online-doc-file = *.INF

	Options:
		-Dflag	set Texinfo flag
		-Uflag	set Texinfo flag
		-n	use @node to split into panels, not @chapter
		-?	display usage

If the extension of the output filename is .INF, texi2ipf automatically
invokes the IPFC program, which compiles the temporary .IPF file to
.INF. The IPFC program is a component of the OS/2 toolkit. Any other
extension specifies an IPF source file. If you run IPFC separately, use
its option /W1 to avoid warnings.

The general rule is: "garbage in garbage out". If you document contains
bugs such as unterminated controls ("@end xxx" missing) you may get any
kind of errors, including core dumps.

Texi2ipf normally uses both @node and @chapter/@section/... Texinfo
controls to separate the text into IPF panels. Most Texinfo document
are formatted with @node immediately preceding the corresponding
@chapter control. The @node information is used to resolve
cross-references, the @chapter controls define individual panels and
their toc levels. You can use -n if a document should be interpreted
using @node controls only.

Text is converted corrsponding to the printed document whenever
possible. Tabulations, table spacings and indents are imprecise and
sometimes implicitly defined by the IPF format. The program isn't
perfect, so some documents may not appear correctly formatted,
especially if the text format changes significantly within a line of
text. Enumerations alway start with an arabic 1, no matter what the
control specifies.
