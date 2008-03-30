fontsdir = $(VICEDIR)/fonts

fonts_DATA = \
	CBM.ttf

EXTRA_DIST = $(fonts_DATA) cbm-directory-charset.fon 

install: install-data-am
	@echo "Preparing fontdir, please wait..." ; \
	if [ ! `fc-cache --list > /dev/null 2>&1` ] ; then \
	  if test "x${HOME}" != "x" ; then \
	    mkdir ${HOME}/.fonts > /dev/null 2>&1 ; \
	    cp $(fontsdir)/CBM.ttf ${HOME}/.fonts ; \
	    fc-cache ; \
	  else \
	    echo '$$HOME not set... giving up.' ; \
	  fi \
	else \
	  echo 'fc-cache not available... giving up.' ; \
	fi
