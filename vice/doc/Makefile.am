@ALTERNATE_DOCDIR_TRUE@realdocdir = $(prefix)/share/doc/vice
@ALTERNATE_DOCDIR_FALSE@realdocdir = $(VICEDIR)/doc

docdir = $(realdocdir)

MISC_DOX = \
	CIA-README.txt \
	coding-guidelines.txt \
	Documentation-Howto.txt \
	Doxygen-Howto.txt \
	iec-bus.txt \
	MacOSX-ResourceTree.txt \
	NLS-Howto.txt

BUILDING_DOX = \
	building/GP2X-Howto.txt \
	building/MacOSX-Howto.txt \
	building/Minix-Howto.txt \
	building/MSDOS-Howto.txt \
	building/MSDOS-Ethernet-Howto.txt \
	building/OpenVMS-Howto.txt \
	building/SDL-Howto.txt \
	building/Win32-Cygwin-Howto.txt \
	building/Win32-Mingw32-Howto.txt \
	building/Linux-Mingw32-Howto.txt

READMES = \
	readmes/Readme-Amiga.txt \
	readmes/Readme-BeOS.txt \
	readmes/Readme-DOS.txt \
	readmes/Readme-MacOSX.txt \
	readmes/Readme-OS2.txt \
	readmes/Readme-SDL.txt

TEXI_TOOLS = \
	fixdox.sh \
	t2h.pl \
	texi2chm.sh \
	texi2guide.sh

DOC_TOOLS = \
	checkdoc.c \
	checkdoc.mak \
	checkdoc.mon \
	Doxyfile \
	mainpage.dox \
	mkdoxy.sh \
	src2doxy.sh \
	txt2doxy.sh

STYLE_TOOLS = \
	.indent.pro \
	indent-test.c \
	uncrustify.cfg
	
VICE_DOX = \
	vice.txt \
	vice.pdf \
	vice.chm \
	vice.hlp \
	vice.guide \
	vice.inf \
	vice.info
	
info_TEXINFOS = vice.texi

dist_doc_DATA = $(MISC_DOX) $(VICE_DOX) $(BUILDING_DOX) $(READMES)

EXTRA_DIST = $(DOC_TOOLS) $(TEXI_TOOLS) $(STYLE_TOOLS)

SUBDIRS = html

AM_MAKEINFOHTMLFLAGS = 
AM_MAKEINFOFLAGS = --no-split

$(builddir)/vice.txt:		$(srcdir)/vice.texi
	LANG=C $(MAKEINFO) -o vicetmp.txt --no-headers $(srcdir)/vice.texi
	$(srcdir)/fixdox.sh txt <vicetmp.txt | sed 's/ \+$$//' >$(builddir)/vice.txt
	rm -f vicetmp.txt

$(builddir)/vice.pdf:	$(srcdir)/vice.texi
if BUILD_PDF
	$(TEXI2DVI) -q --clean --pdf -o $(builddir)/vice.pdf $(srcdir)/vice.texi

PDF_DEPS=$(builddir)/vice.pdf
else
	touch $(builddir)/vice.pdf

PDF_DEPS=
endif

$(builddir)/vice.guide:	$(srcdir)/vice.texi
if BUILD_AMIGAGUIDE
	$(srcdir)/texi2guide.sh $(MAKEGUIDE) $(builddir)/vice.guide $(srcdir)/vice.texi

GUIDE_DEPS=$(builddir)/vice.guide
else
	touch $(srcdir)/vice.guide

GUIDE_DEPS=
endif

$(builddir)/vice.hlp:	$(srcdir)/vice.texi
if BUILD_HLP
	$(MAKERTF) --hpj=vice.hpj --output=vice.rtf $(srcdir)/vice.texi
	$(HCRTF) -o VICE.HLP -xn vice.hpj
	rm -f vice.hpj vice.rtf
	mv VICE.HLP $(builddir)/vice.hlp

HLP_DEPS=$(builddir)/vice.hlp
else
	touch $(builddir)/vice.hlp

HLP_DEPS=
endif

$(builddir)/vice.chm:	$(srcdir)/vice.texi
if BUILD_CHM
	$(srcdir)/texi2chm.sh $(HHC) $(builddir)/vice.chm $(srcdir)/vice.texi $(srcdir)/fixdox.sh $(srcdir)/t2h.pl

CHM_DEPS=$(builddir)/vice.chm
else
	touch $(builddir)/vice.chm

CHM_DEPS=
endif

$(builddir)/vice.inf:	$(srcdir)/vice.texi
if BUILD_IPF
	$(TEXI2IPF) $(srcdir)/vice.texi >temp.ipf
	$(IPFC) -i temp.ipf
	mv temp.inf $(builddir)/vice.inf
	rm -f temp.ipf

IPF_DEPS=$(builddir)/vice.inf
else
	touch $(builddir)/vice.inf

IPF_DEPS=
endif

$(builddir)/vice.info:	$(srcdir)/vice.texi
	$(MAKEINFO) -o $(builddir)/vice.info --no-split $(srcdir)/vice.texi

INFO_DEPS = $(builddir)/vice.txt $(builddir)/vice.info $(PDF_DEPS) $(GUIDE_DEPS) $(HLP_DEPS) $(CHM_DEPS) $(IPF_DEPS)
