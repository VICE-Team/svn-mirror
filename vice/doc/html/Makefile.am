@ALTERNATE_DOCDIR_TRUE@realdocdir = $(prefix)/share/doc/vice
@ALTERNATE_DOCDIR_FALSE@realdocdir = $(VICEDIR)/doc

docdir = $(realdocdir)

doc_DATA = \
	amigaos.html \
	android.html \
	beos.html \
	darwin.html \
	images/CIPS2008.png \
	images/new.gif \
	images/vice-logo.svg \
	fonts/Lato-Black.eot \
	fonts/Lato-BlackItalic.eot \
	fonts/Lato-BlackItalic.ttf \
	fonts/Lato-BlackItalic.woff \
	fonts/Lato-BlackItalic.woff2 \
	fonts/Lato-Black.ttf \
	fonts/Lato-Black.woff \
	fonts/Lato-Black.woff2 \
	fonts/Lato-Bold.eot \
	fonts/Lato-BoldItalic.eot \
	fonts/Lato-BoldItalic.ttf \
	fonts/Lato-BoldItalic.woff \
	fonts/Lato-BoldItalic.woff2 \
	fonts/Lato-Bold.ttf \
	fonts/Lato-Bold.woff \
	fonts/Lato-Bold.woff2 \
	fonts/Lato-Hairline.eot \
	fonts/Lato-HairlineItalic.eot \
	fonts/Lato-HairlineItalic.ttf \
	fonts/Lato-HairlineItalic.woff \
	fonts/Lato-HairlineItalic.woff2 \
	fonts/Lato-Hairline.ttf \
	fonts/Lato-Hairline.woff \
	fonts/Lato-Hairline.woff2 \
	fonts/Lato-Heavy.eot \
	fonts/Lato-HeavyItalic.eot \
	fonts/Lato-HeavyItalic.ttf \
	fonts/Lato-HeavyItalic.woff \
	fonts/Lato-HeavyItalic.woff2 \
	fonts/Lato-Heavy.ttf \
	fonts/Lato-Heavy.woff \
	fonts/Lato-Heavy.woff2 \
	fonts/Lato-Italic.eot \
	fonts/Lato-Italic.ttf \
	fonts/Lato-Italic.woff \
	fonts/Lato-Italic.woff2 \
	fonts/Lato-Light.eot \
	fonts/Lato-LightItalic.eot \
	fonts/Lato-LightItalic.ttf \
	fonts/Lato-LightItalic.woff \
	fonts/Lato-LightItalic.woff2 \
	fonts/Lato-Light.ttf \
	fonts/Lato-Light.woff \
	fonts/Lato-Light.woff2 \
	fonts/Lato-Medium.eot \
	fonts/Lato-MediumItalic.eot \
	fonts/Lato-MediumItalic.ttf \
	fonts/Lato-MediumItalic.woff \
	fonts/Lato-MediumItalic.woff2 \
	fonts/Lato-Medium.ttf \
	fonts/Lato-Medium.woff \
	fonts/Lato-Medium.woff2 \
	fonts/Lato-Regular.eot \
	fonts/Lato-Regular.ttf \
	fonts/Lato-Regular.woff \
	fonts/Lato-Regular.woff2 \
	fonts/Lato-Semibold.eot \
	fonts/Lato-SemiboldItalic.eot \
	fonts/Lato-SemiboldItalic.ttf \
	fonts/Lato-SemiboldItalic.woff \
	fonts/Lato-SemiboldItalic.woff2 \
	fonts/Lato-Semibold.ttf \
	fonts/Lato-Semibold.woff \
	fonts/Lato-Semibold.woff2 \
	fonts/Lato-Thin.eot \
	fonts/Lato-ThinItalic.eot \
	fonts/Lato-ThinItalic.ttf \
	fonts/Lato-ThinItalic.woff \
	fonts/Lato-ThinItalic.woff2 \
	fonts/Lato-Thin.ttf \
	fonts/Lato-Thin.woff \
	fonts/Lato-Thin.woff2 \
	fonts/OFL.txt \
	index.html \
	macosx.html \
	COPYING \
	NEWS \
	nsosrh.html \
	qnx.html \
	sco.html \
	sitemap.xml \
	solaris.html \
	sortix.html \
	vice.css \
	vice_1.html \
	vice_2.html \
	vice_3.html \
	vice_4.html \
	vice_5.html \
	vice_6.html \
	vice_7.html \
	vice_8.html \
	vice_9.html \
	vice_10.html \
	vice_11.html \
	vice_12.html \
	vice_13.html \
	vice_14.html \
	vice_15.html \
	vice_16.html \
	vice_17.html \
	vice_18.html \
	vice_19.html \
	vice_20.html \
	vice_21.html \
	vice_22.html \
	vice_toc.html \
	robots.txt

EXTRA_DIST = $(doc_DATA) \
	texi2html

CLEAN_STUFF = \
	vice_1.html \
	vice_2.html \
	vice_3.html \
	vice_4.html \
	vice_5.html \
	vice_6.html \
	vice_7.html \
	vice_8.html \
	vice_9.html \
	vice_10.html \
	vice_11.html \
	vice_12.html \
	vice_13.html \
	vice_14.html \
	vice_15.html \
	vice_16.html \
	vice_17.html \
	vice_18.html \
	vice_19.html \
	vice_20.html \
	vice_21.html \
	vice_22.html \
	vice_toc.html

vice_1.html vice_2.html vice_3.html vice_4.html vice_5.html vice_6.html vice_7.html vice_8.html vice_9.html vice_10.html vice_11.html vice_12.html vice_13.html vice_14.html vice_15.html vice_16.html vice_17.html vice_18.html vice_19.html vice_20.html vice_21.html vice_22.html: vice_toc.html

vice_toc.html: $(top_srcdir)/doc/vice.texi
	(cd $(builddir); \
	$(PERL) $(srcdir)/texi2html -number -split_chapter -D$(PLATFORM_DOX_FLAGS) $(top_srcdir)/doc/vice.texi)

COPYING: $(top_srcdir)/COPYING
	cp $< $@

NEWS: $(top_srcdir)/NEWS
	cp $< $@

clean:	clean-am
	rm -f $(CLEAN_STUFF)
