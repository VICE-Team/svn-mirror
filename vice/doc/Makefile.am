SUBDIRS = building html readmes

@ALTERNATE_DOCDIR_TRUE@realdocdir = $(prefix)/share/doc/vice
@ALTERNATE_DOCDIR_FALSE@realdocdir = $(VICEDIR)/doc

docdir = $(realdocdir)

MISC_DOX = \
	CIA-README.txt \
	coding-guidelines.txt \
	Documentation-Howto.txt \
	Doxygen-Howto.txt \
	ffmpeg-support.txt \
	hardware-sids.txt \
	iec-bus.txt \
	SDL-support.txt \
	Release-Howto.txt

TEXI_TOOLS = \
	fixdox.sh \
	t2h.pl \
	texi2guide.sh

DOC_TOOLS = \
	checkdoc.c \
	checkdoc.mak \
	Doxyfile \
	mainpage.dox \
	mkdoxy.sh \
	src2doxy.sh \
	txt2doxy.sh

STYLE_TOOLS = \
	.indent.pro \
	indent-test.c \
	uncrustify.cfg

# VICE_DOX is conditionally extended with vice.pdf, vice.hlp etc
VICE_DOX = \
	vice.txt \
	vice.info

dist_doc_DATA = $(MISC_DOX) $(VICE_DOX)

EXTRA_DIST = $(DOC_TOOLS) $(TEXI_TOOLS) $(STYLE_TOOLS)

if UNIX_MACOSX_COMPILE
# Use brew supplied texi2dvi over buggy Apple version
export PATH := /usr/local/opt/texinfo/bin:$(PATH)
endif

.vice-texi-charset-ok:	$(srcdir)/vice.texi
	if [ "`file --mime-encoding $<`" != "$<: iso-8859-1" ]; then \
		echo "ERROR: vice.texi contains content that is not valid iso-8859-1" >&2; \
		false; \
	fi
	touch .vice-texi-charset-ok

$(builddir)/vice.txt:	.vice-texi-charset-ok
	touch vicetmp.txt
	LANG=C $(MAKEINFO) -D$(PLATFORM_DOX_FLAGS) -o vicetmp.txt --no-validate --no-headers $(srcdir)/vice.texi
	$(srcdir)/fixdox.sh txt <vicetmp.txt | LANG=C sed 's/ \+$$//' >$(builddir)/vice.txt
	rm -f vicetmp.txt

$(builddir)/vice.info:	.vice-texi-charset-ok
	touch $(builddir)/vice.info
	$(MAKEINFO) -D$(PLATFORM_DOX_FLAGS) -o $(builddir)/vice.info --no-validate --no-split $(srcdir)/vice.texi

if BUILD_PDF
VICE_DOX += vice.pdf
$(builddir)/vice.pdf:	.vice-texi-charset-ok
	LANG=C sed 's/@heading NO WARRANTY/@center NO WARRANTY/g' <$(srcdir)/vice.texi >vicepdf.texi
	LANG=C $(TEXI2DVI) -I $(srcdir) -q --clean --pdf "--texinfo=@set $(PLATFORM_DOX_FLAGS) " -o $(builddir)/vice.pdf vicepdf.texi
	rm -f vicepdf.texi
endif

if BUILD_AMIGAGUIDE
VICE_DOX += vice.guide
$(builddir)/vice.guide:	.vice-texi-charset-ok
	cp $(srcdir)/vice.texi tmp.texi
	$(srcdir)/fixdox.sh guide
	$(srcdir)/texi2guide.sh $(MAKEGUIDE) $(builddir)/vice.guide vicetmp.texi $(host_cpu) $(host_os)
	rm -f tmp.texi vicetmp.texi
endif

if BUILD_IPF
VICE_DOX += vice.inf
$(builddir)/vice.inf:	.vice-texi-charset-ok
	cp $(srcdir)/vice.texi tmp.texi
	$(srcdir)/fixdox.sh ipf
	$(TEXI2IPF) $(srcdir)/vicetmp.texi >temp.ipf
	$(IPFC) -i temp.ipf
	mv temp.inf $(builddir)/vice.inf
	rm -f temp.ipf vicetmp.texi
endif

# This doesn't appear to delete vice.pdf, vice.chm, vice.guide etc added
# in this Makefile with VICE_DOX += FOO, perhaps just some ordinary `rm -f`
# would do better? -- Compyx
CLEANFILES = .vice-texi-charset-ok $(VICE_DOX)

