SUBDIRS = p64 liblame libx264 libffmpeg ffmpeg

ffmpeglibs: $(FFMPEG_DIR)

.PHONY: libp64 libffmpeg

libp64:
	(cd p64; $(MAKE))

libffmpeg:
	(cd libffmpeg; $(MAKE))

ffmpeglibs-install:
	(cd libffmpeg; $(MAKE) install-libs)
