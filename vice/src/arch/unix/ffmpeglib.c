/*
 * ffmpeglib.c - Interface to access the ffmpeg libs.
 *
 * Written by
 *  Andreas Matthies <andreas.matthies@gmx.net>
 *  Christian Vogelgsang <chris@vogelgsang.org>
 * 
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */

#include "vice.h"

#ifdef HAVE_FFMPEG 

#include <dlfcn.h>

#include "gfxoutputdrv/ffmpeglib.h"
#include "log.h"

#ifdef MACOSX_SUPPORT
/* assume MacPorts paths here */
#define AVCODEC_SO_NAME     "/opt/local/lib/libavcodec.51.dylib"
#define AVFORMAT_SO_NAME    "/opt/local/lib/libavformat.52.dylib"
#define AVUTIL_SO_NAME      "/opt/local/lib/libavutil.49.dylib"
#else
#define AVCODEC_SO_NAME     "libavcodec.so.51"
#define AVFORMAT_SO_NAME    "libavformat.so.52"
#define AVUTIL_SO_NAME      "libavutil.so.49"
#endif

static void *avcodec_so = NULL;
static void *avformat_so = NULL;
static void *avutil_so = NULL;

static void ffmpeglib_free_library(ffmpeglib_t *lib)
{
    if (avcodec_so) {
        if (!dlclose(avcodec_so)) {
            log_debug("closing library " AVCODEC_SO_NAME " failed!");
        }
    }
    avcodec_so = NULL;

    lib->p_avcodec_open = NULL;
    lib->p_avcodec_close = NULL;
    lib->p_avcodec_find_encoder = NULL;
    lib->p_avcodec_encode_audio = NULL;
    lib->p_avcodec_encode_video = NULL;
    lib->p_avpicture_fill = NULL;
    lib->p_avpicture_get_size = NULL;
    lib->p_img_convert = NULL;

    if (avformat_so) {
        if (!dlclose(avformat_so)) {
            log_debug("closing library " AVFORMAT_SO_NAME " failed!");
        }
    }
    avformat_so = NULL;

    lib->p_av_init_packet = NULL;
    lib->p_av_register_all = NULL;
    lib->p_av_new_stream = NULL;
    lib->p_av_set_parameters = NULL;
    lib->p_av_write_header = NULL;
    lib->p_av_write_frame = NULL;
    lib->p_av_write_trailer = NULL;
    lib->p_url_fopen = NULL;
    lib->p_url_fclose = NULL;
    lib->p_dump_format = NULL;
    lib->p_guess_format = NULL;

    if (avutil_so) {
        if (!dlclose(avutil_so)) {
            log_debug("closing library " AVUTIL_SO_NAME " failed!");
        }
    }
    avutil_so = NULL;

    lib->p_av_free = NULL;
}

/* macro for getting functionpointers from avcodec */
#define GET_PROC_ADDRESS_AND_TEST_AVCODEC( _name_ ) \
    lib->p_##_name_ = (_name_##_t) dlsym(avcodec_so, #_name_ ); \
    if (!lib->p_##_name_ ) { \
        log_debug("symbol " #_name_ " failed!"); \
        ffmpeglib_free_library(lib); \
        return -1; \
    } 

/* macro for getting functionpointers from avformat */
#define GET_PROC_ADDRESS_AND_TEST_AVFORMAT( _name_ ) \
    lib->p_##_name_ = (_name_##_t) dlsym(avformat_so, #_name_ ); \
    if (!lib->p_##_name_ ) { \
        log_debug("symbol " #_name_ " failed!"); \
        ffmpeglib_free_library(lib); \
        return -1; \
    } 

/* macro for getting functionpointers from avutil */
#define GET_PROC_ADDRESS_AND_TEST_AVUTIL( _name_ ) \
    lib->p_##_name_ = (_name_##_t) dlsym(avutil_so, #_name_ ); \
    if (!lib->p_##_name_ ) { \
        log_debug("symbol " #_name_ " failed!"); \
        ffmpeglib_free_library(lib); \
        return -1; \
    } 

static int ffmpeglib_load_library(ffmpeglib_t *lib)
{
    avcodec_version_t avcodec_version;

    if (!avcodec_so) {
        avcodec_so = dlopen(AVCODEC_SO_NAME, RTLD_LAZY | RTLD_LOCAL);

        if (!avcodec_so) {
            log_debug("opening library " AVCODEC_SO_NAME " failed!");
            return -1;
        }

        GET_PROC_ADDRESS_AND_TEST_AVCODEC(avcodec_open);
        GET_PROC_ADDRESS_AND_TEST_AVCODEC(avcodec_close);
        GET_PROC_ADDRESS_AND_TEST_AVCODEC(avcodec_find_encoder);
        GET_PROC_ADDRESS_AND_TEST_AVCODEC(avcodec_encode_audio);
        GET_PROC_ADDRESS_AND_TEST_AVCODEC(avcodec_encode_video);
        GET_PROC_ADDRESS_AND_TEST_AVCODEC(avpicture_fill);
        GET_PROC_ADDRESS_AND_TEST_AVCODEC(avpicture_get_size);
        GET_PROC_ADDRESS_AND_TEST_AVCODEC(img_convert);
    }

    if (!avformat_so) {
        avformat_so = dlopen(AVFORMAT_SO_NAME, RTLD_LAZY | RTLD_LOCAL);

        if (!avformat_so) {
            log_debug("opening library " AVFORMAT_SO_NAME " failed!");
            return -1;
        }

        GET_PROC_ADDRESS_AND_TEST_AVFORMAT(av_init_packet);
        GET_PROC_ADDRESS_AND_TEST_AVFORMAT(av_register_all);
        GET_PROC_ADDRESS_AND_TEST_AVFORMAT(av_new_stream);
        GET_PROC_ADDRESS_AND_TEST_AVFORMAT(av_set_parameters);
        GET_PROC_ADDRESS_AND_TEST_AVFORMAT(av_write_header);
        GET_PROC_ADDRESS_AND_TEST_AVFORMAT(av_write_frame);
        GET_PROC_ADDRESS_AND_TEST_AVFORMAT(av_write_trailer);
        GET_PROC_ADDRESS_AND_TEST_AVFORMAT(url_fopen);
        GET_PROC_ADDRESS_AND_TEST_AVFORMAT(url_fclose);
        GET_PROC_ADDRESS_AND_TEST_AVFORMAT(dump_format);
        GET_PROC_ADDRESS_AND_TEST_AVFORMAT(guess_format);
    }

    if (!avutil_so) {
        avutil_so = dlopen(AVUTIL_SO_NAME, RTLD_LAZY | RTLD_LOCAL);

        if (!avformat_so) {
            log_debug("opening library " AVUTIL_SO_NAME " failed!");
            return -1;
        }

        GET_PROC_ADDRESS_AND_TEST_AVUTIL(av_free);
    }
    avcodec_version = (avcodec_version_t)dlsym(avcodec_so, "avcodec_version");

    if (avcodec_version() != LIBAVCODEC_VERSION_INT) {
        log_debug("FFMPEG version mismatch!");
        return -1;
    }
    return 0;
}


int ffmpeglib_open(ffmpeglib_t *lib)
{
    return ffmpeglib_load_library(lib);
}


void ffmpeglib_close(ffmpeglib_t *lib)
{
    ffmpeglib_free_library(lib);
}


#endif /* #ifdef HAVE_FFMPEG */
