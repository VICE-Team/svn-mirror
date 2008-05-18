@ALTERNATE_DOCDIR_TRUE@realdocdir = $(prefix)/share/doc/vice
@ALTERNATE_DOCDIR_FALSE@realdocdir = $(VICEDIR)/doc

docdir = $(realdocdir)

doc_DATA = \
	amigaos.html \
	beos.html \
	images/new.gif \
	images/vice-logo.jpg \
	index.html \
	macosx.html \
	plain/64doc.txt \
	plain/BUGS \
	plain/COPYING \
	plain/NEWS \
	plain/PETdoc.txt \
	plain/TODO \
	plain/Walkthrough-Howto.txt \
	plain/cbm_basic_tokens.txt \
	plain/drive_info.txt \
	plain/serial.txt \
	solaris.html \
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
	vice_toc.html

EXTRA_DIST = $(doc_DATA) \
	texi2html

vice_toc.html: $(top_srcdir)/doc/vice.texi
	(cd $(srcdir); \
         $(PERL) ./texi2html -number -split_chapter ../vice.texi)
