fontsdir = $(prefix)/lib/vice/fonts

fonts_DATA = \
	cbm-directory-charset.fon \
	vice-cbm.bdf

EXTRA_DIST = $(fonts_DATA)

install: install-data-am
	@echo "Preparing fontdir..." ; \
	cd $(DESTDIR)$(prefix)/lib/vice/fonts ; \
	bdftopcf -o vice-cbm.pcf vice-cbm.bdf ; \
	rm vice-cbm.bdf ; \
	mkfontdir .
