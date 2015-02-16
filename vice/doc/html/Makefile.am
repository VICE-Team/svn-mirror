@ALTERNATE_DOCDIR_TRUE@realdocdir = $(prefix)/share/doc/vice
@ALTERNATE_DOCDIR_FALSE@realdocdir = $(VICEDIR)/doc

docdir = $(realdocdir)

doc_DATA = \
	amigaos.html \
	beos.html \
	images/CommodoreInnovationPriceSoftware2008.jpg \
	images/new.gif \
	images/vice-logo.jpg \
	images/vice-logo-small.png \
	index.html \
	macosx.html \
	COPYING \
	NEWS \
	qnx.html \
	sco.html \
	solaris.html \
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
	vice_toc.html

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
