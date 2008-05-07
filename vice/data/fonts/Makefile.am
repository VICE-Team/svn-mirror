fontsdir = $(VICEDIR)/fonts

fonts_DATA = \
	vice-cbm.bdf \
	CBM.ttf

EXTRA_DIST = $(fonts_DATA) cbm-directory-charset.fon 

install: install-data-am
@HAVE_FC_CACHE_TRUE@	@echo "Preparing fontdir, please wait..." ; \
@HAVE_FC_CACHE_TRUE@	if [ ! `fc-cache --list > /dev/null 2>&1` ] ; then \
@HAVE_FC_CACHE_TRUE@	  if test "x${HOME}" != "x" ; then \
@HAVE_FC_CACHE_TRUE@	    mkdir ${HOME}/.fonts > /dev/null 2>&1 ; \
@HAVE_FC_CACHE_TRUE@	    cp $(fontsdir)/CBM.ttf ${HOME}/.fonts ; \
@HAVE_FC_CACHE_TRUE@	    fc-cache ; \
@HAVE_FC_CACHE_TRUE@	  else \
@HAVE_FC_CACHE_TRUE@	    echo '$$HOME not set... giving up.' ; \
@HAVE_FC_CACHE_TRUE@	  fi \
@HAVE_FC_CACHE_TRUE@	else \
@HAVE_FC_CACHE_TRUE@	  echo 'fc-cache not available... giving up.' ; \
@HAVE_FC_CACHE_TRUE@	fi
@HAVE_FC_CACHE_FALSE@	@echo "Preparing fontdir..." ; \
@HAVE_FC_CACHE_FALSE@	cd $(DESTDIR)$(fontsdir) ; \
@HAVE_FC_CACHE_FALSE@	bdftopcf -o vice-cbm.pcf vice-cbm.bdf ; \
@HAVE_FC_CACHE_FALSE@	rm vice-cbm.bdf ; \
@HAVE_FC_CACHE_FALSE@	mkfontdir .
