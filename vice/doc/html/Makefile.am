docdir = $(prefix)/lib/vice/doc

doc_DATA = \
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
         $(PERL) texi2html -number -space_itemize -space_table -foreground 00000 -background ffffff -split_chapter -acc ../vice.texi)
