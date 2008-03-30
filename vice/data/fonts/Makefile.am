fontsdir = $(prefix)/lib/vice/fonts

fonts_DATA = \
	vice-cbm.bdf

EXTRA_DIST = $(fonts_DATA) cbm-directory-charset.fon 

install: install-data-am
	@echo "Preparing fontdir..." ; \
	cd $(DESTDIR)$(prefix)/lib/vice/fonts ; \
	bdftopcf -o vice-cbm.pcf vice-cbm.bdf ; \
	rm vice-cbm.bdf ; \
	mkfontdir .
