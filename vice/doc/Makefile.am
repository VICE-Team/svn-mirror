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
	cartconv.txt \
	iec-bus.txt \
	mon.txt \
	openGLsync-howto.txt \
	GP2X-Howto.txt \
	coding-guidelines.txt

info_TEXINFOS = vice.texi

EXTRA_DIST = $(doc_DATA)

SUBDIRS = html

AM_MAKEINFOHTMLFLAGS = 
AM_MAKEINFOFLAGS = --no-split

$(srcdir)/vice.txt:	vice.texi
	$(MAKEINFO) -o $(srcdir)/vice.txt --no-headers vice.texi

if BUILD_PDF
$(srcdir)/vice.pdf:	vice.texi
	$(TEXI2DVI) -q --clean --pdf -o $(srcdir)/vice.pdf vice.texi

INFO_DEPS = $(srcdir)/vice.txt $(srcdir)/vice.info $(srcdir)/vice.pdf
else
INFO_DEPS = $(srcdir)/vice.txt $(srcdir)/vice.info
endif
