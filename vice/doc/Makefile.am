@ALTERNATE_DOCDIR_TRUE@realdocdir = $(prefix)/share/doc/vice
@ALTERNATE_DOCDIR_FALSE@realdocdir = $(VICEDIR)/doc

docdir = $(realdocdir)

doc_DATA = \
	MacOSX-Build.txt \
	MacOSX-ResourceTree.txt \
	Minix-Howto.txt \
	MSDOS-Ethernet-Howto.txt \
	MSDOS-Howto.txt \
	NLS-Howto.txt \
	Readme.beos \
	Readme.dos \
	ReadmeMacOSX.txt \
	ReadmeSDL.txt \
	Win32-Cygwin-HOWTO.txt \
	Win32-Howto.txt \
	OpenVMS-Howto.txt \
	iec-bus.txt \
	openGLsync-howto.txt \
	GP2X-Howto.txt \
	coding-guidelines.txt \
	CIA-README.txt

TEXI_TOOLS = \
	texi2chm.sh \
	texi2guide.sh \
	t2h.pl

VICE_DOX = \
	vice.txt \
	vice.pdf \
	vice.chm \
	vice.hlp \
	vice.guide

info_TEXINFOS = vice.texi

EXTRA_DIST = $(doc_DATA) $(VICE_DOX) $(TEXI_TOOLS)

SUBDIRS = html

AM_MAKEINFOHTMLFLAGS = 
AM_MAKEINFOFLAGS = --no-split

$(srcdir)/vice.txt:	vice.texi
	$(MAKEINFO) -o $(srcdir)/vice.txt --no-headers $(srcdir)/vice.texi

if BUILD_PDF
$(srcdir)/vice.pdf:	vice.texi
	$(TEXI2DVI) -q --clean --pdf -o $(srcdir)/vice.pdf $(srcdir)/vice.texi

PDF_DEPS=$(srcdir)/vice.pdf
else
PDF_DEPS=
endif

if BUILD_AMIGAGUIDE
$(srcdir)/vice.guide:	vice.texi
	$(srcdir)/texi2guide.sh $(MAKEGUIDE) $(srcdir)/vice.guide $(srcdir)/vice.texi

GUIDE_DEPS=$(srcdir)/vice.guide
else
GUIDE_DEPS=
endif

if BUILD_HLP
$(srcdir)/vice.hlp:	vice.texi
	$(MAKERTF) --hpj=vice.hpj --output=vice.rtf $(srcdir)/vice.texi
	$(HCRTF) -o VICE.HLP -xn vice.hpj
	rm -f vice.hpj vice.rtf
	mv VICE.HLP $(srcdir)/vice.hlp

HLP_DEPS=$(srcdir)/vice.hlp
else
HLP_DEPS=
endif

if BUILD_CHM
$(srcdir)/vice.chm:	vice.texi
	$(srcdir)/texi2chm.sh $(HHC) $(srcdir)/vice.chm $(srcdir)/vice.texi

CHM_DEPS=$(srcdir)/vice.chm
else
CHM_DEPS=
endif

INFO_DEPS = $(srcdir)/vice.txt $(srcdir)/vice.info $(PDF_DEPS) $(GUIDE_DEPS) $(HLP_DEPS) $(CHM_DEPS)
