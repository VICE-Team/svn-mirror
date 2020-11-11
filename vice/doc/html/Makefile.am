# These files are always required due to `make dist` needing them.

STATIC_FILES = \
	amigaos.html \
	android.html \
	beos.html \
	darwin.html \
	favicon.ico \
	index.html \
	latofonts.css \
	macosx.html \
	nsosrh.html \
	qnx.html \
	robots.txt \
	sco.html \
	sitemap.xml \
	solaris.html \
	sortix.html \
	unix.html \
	vice.css \
	windows.html

FONTS_FILES = \
	fonts/Lato-Bold.eot \
	fonts/Lato-Bold.ttf \
	fonts/Lato-Bold.woff \
	fonts/Lato-Bold.woff2 \
	fonts/Lato-BoldItalic.eot \
	fonts/Lato-BoldItalic.ttf \
	fonts/Lato-BoldItalic.woff \
	fonts/Lato-BoldItalic.woff2 \
	fonts/Lato-Italic.eot \
	fonts/Lato-Italic.ttf \
	fonts/Lato-Italic.woff \
	fonts/Lato-Italic.woff2 \
	fonts/Lato-Regular.eot \
	fonts/Lato-Regular.ttf \
	fonts/Lato-Regular.woff \
	fonts/Lato-Regular.woff2 \
	fonts/OFL.txt

IMAGES_FILES = \
	images/CIPS2008.png \
	images/new.gif \
	images/vice-logo.svg

EXTRA_DIST = \
	checklinks.sh \
	texi2html \
	$(STATIC_FILES) \
	$(FONTS_FILES) \
	$(IMAGES_FILES)


htmldir = $(VICE_DOCDIR)/html
fontsdir = $(htmldir)/fonts
imagesdir = $(htmldir)/images


if ENABLE_HTML_DOCS

GEN_HTML := \
	vice_foot.html \
	vice_toc.html \
	$(patsubst %,vice_%.html,$(shell seq 23))
	
TEXT_FILES = \
	COPYING \
	NEWS


HTML_FILES = \
	$(TEXT_FILES) \
	$(STATIC_FILES)

if HAVE_PERL
HTML_FILES += \
	$(GEN_HTML)
endif


COPIED_FILES = $(if $(subst x.x,,x$(srcdir)x),$(STATIC_FILES) $(FONTS_FILES) $(IMAGES_FILES))

CLEANFILES = \
	vice_*.html \
	$(TEXT_FILES) \
	$(COPIED_FILES)



if ENABLE_HTML_DOCS
html_DATA = $(HTML_FILES)
fonts_DATA = $(FONTS_FILES)
images_DATA = $(IMAGES_FILES)
else
noinst_DATA = \
	$(HTML_FILES) \
	$(FONTS_FILES) \
	$(IMAGES_FILES)
endif

# It's awkward to make the following multi-output rule work properly
# in parallel but it's the only intensive rule anyway so just
# serialize the whole Makefile. See the Automake manual for details.
# https://www.gnu.org/software/automake/manual/automake.html#Multiple-Outputs
.NOTPARALLEL:

if HAVE_PERL
$(GEN_HTML): $(top_srcdir)/doc/vice.texi $(srcdir)/texi2html
	@echo "generating html documentation..."
	$(PERL) "$(srcdir)"/texi2html -number -split_chapter -D$(PLATFORM_DOX_FLAGS) "$<"
endif

$(TEXT_FILES): %: $(top_srcdir)/%
	cp "$<" "$@"

fonts images:
	mkdir -p "$@"

$(COPIED_FILES): %: $(srcdir)/% | fonts images
	cp "$<" "$@"

endif
