@ALTERNATE_DOCDIR_TRUE@realdocdir = $(prefix)/share/doc/vice
@ALTERNATE_DOCDIR_FALSE@realdocdir = $(VICEDIR)/doc

docdir = $(realdocdir)

MISC_DOX = \
	MacOSX-ResourceTree.txt \
	NLS-Howto.txt \
	iec-bus.txt \
	openGLsync-howto.txt \
	coding-guidelines.txt \
	CIA-README.txt

BUILDING_DOX = \
	building/GP2X-Howto.txt \
	building/MacOSX-Howto.txt \
	building/Minix-Howto.txt \
	building/MSDOS-Howto.txt \
	building/MSDOS-Ethernet-Howto.txt \
	building/OpenVMS-Howto.txt \
	building/SDL-Howto.txt \
	building/Win32-Cygwin-Howto.txt \
	building/Win32-Mingw32-Howto.txt

READMES = \
	readmes/Readme-Amiga.txt \
	readmes/Readme-BeOS.txt \
	readmes/Readme-DOS.txt \
	readmes/Readme-MacOSX.txt \
	readmes/Readme-OS2.txt \
	readmes/Readme-SDL.txt

TEXI_TOOLS = \
	fixdox.sh \
	texi2chm.sh \
	texi2guide.sh \
	t2h.pl

VICE_DOX = \
	vice.txt \
	vice.pdf \
	vice.chm \
	vice.hlp \
	vice.guide \
	vice.inf

info_TEXINFOS = vice.texi

dist_doc_DATA = $(MISC_DOX) $(VICE_DOX) $(BUILDING_DOX) $(READMES)

EXTRA_DIST = $(TEXI_TOOLS)

SUBDIRS = html

AM_MAKEINFOHTMLFLAGS = 
AM_MAKEINFOFLAGS = --no-split

$(srcdir)/vice.txt:	vice.texi
	$(MAKEINFO) -o vicetmp.txt --no-headers $(srcdir)/vice.texi
	$(srcdir)/fixdox.sh txt <vicetmp.txt >$(srcdir)/vice.txt
	rm -f vicetmp.txt

$(srcdir)/vice.pdf:	vice.texi
if BUILD_PDF
	$(TEXI2DVI) -q --clean --pdf -o $(srcdir)/vice.pdf $(srcdir)/vice.texi

PDF_DEPS=$(srcdir)/vice.pdf
else
	touch $(srcdir)/vice.pdf

PDF_DEPS=
endif

$(srcdir)/vice.guide:	vice.texi
if BUILD_AMIGAGUIDE
	$(srcdir)/texi2guide.sh $(MAKEGUIDE) $(srcdir)/vice.guide $(srcdir)/vice.texi

GUIDE_DEPS=$(srcdir)/vice.guide
else
	touch $(srcdir)/vice.guide

GUIDE_DEPS=
endif

$(srcdir)/vice.hlp:	vice.texi
if BUILD_HLP
	$(MAKERTF) --hpj=vice.hpj --output=vice.rtf $(srcdir)/vice.texi
	$(HCRTF) -o VICE.HLP -xn vice.hpj
	rm -f vice.hpj vice.rtf
	mv VICE.HLP $(srcdir)/vice.hlp

HLP_DEPS=$(srcdir)/vice.hlp
else
	touch $(srcdir)/vice.hlp

HLP_DEPS=
endif

$(srcdir)/vice.chm:	vice.texi
if BUILD_CHM
	$(srcdir)/texi2chm.sh $(HHC) $(srcdir)/vice.chm $(srcdir)/vice.texi

CHM_DEPS=$(srcdir)/vice.chm
else
	touch $(srcdir)/vice.chm

CHM_DEPS=
endif

$(srcdir)/vice.inf:	vice.texi
if BUILD_IPF
	$(TEXI2IPF) $(srcdir)/vice.texi >temp.ipf
	$(IPFC) -i temp.ipf
	mv temp.inf $(srcdir)/vice.inf
	rm -f temp.ipf

IPF_DEPS=$(srcdir)/vice.inf
else
	touch $(srcdir)/vice.inf

IPF_DEPS=
endif

INFO_DEPS = $(srcdir)/vice.txt $(srcdir)/vice.info $(PDF_DEPS) $(GUIDE_DEPS) $(HLP_DEPS) $(CHM_DEPS) $(IPF_DEPS)
