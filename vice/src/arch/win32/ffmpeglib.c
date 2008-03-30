/*
 * ffmpeglib.c - Interface to access the ffmpeg libs.
 *
 * Written by
 *  Andreas Matthies <andreas.matthies@gmx.net>
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

#include <windows.h>

#include "gfxoutputdrv/ffmpeglib.h"
#include "log.h"

static HINSTANCE avcodec_dll = NULL;
static HINSTANCE avformat_dll = NULL;


static void ffmpeglib_free_library(void)
{
    if (avcodec_dll) {
        if (!FreeLibrary(avcodec_dll)) {
            log_debug("FreeLibrary avcodec.dll failed!");
        }
    }
    avcodec_dll = NULL;

    p_avcodec_open = NULL;
    p_avcodec_close = NULL;
    p_avcodec_find_encoder = NULL;
    p_avcodec_encode_audio = NULL;
    p_avcodec_encode_video = NULL;
    p_avpicture_fill = NULL;
    p_avpicture_get_size = NULL;
    p_img_convert = NULL;
    p___av_freep = NULL;

    if (avformat_dll) {
        if (!FreeLibrary(avformat_dll)) {
            log_debug("FreeLibrary avformat.dll failed!");
        }
    }
    avformat_dll = NULL;

    p_av_register_all = NULL;
    p_av_new_stream = NULL;
    p_av_set_parameters = NULL;
    p_av_write_header = NULL;
    p_av_write_frame = NULL;
    p_av_write_trailer = NULL;
    p_url_fopen = NULL;
    p_url_fclose = NULL;
    p_dump_format = NULL;
    p_guess_format = NULL;

}

/* macro for getting functionpointers from avcodec */
#define GET_PROC_ADDRESS_AND_TEST_AVCODEC( _name_ ) \
    p_##_name_ = (_name_##_t) GetProcAddress(avcodec_dll, #_name_ ); \
    if (!p_##_name_ ) { \
        log_debug("GetProcAddress " #_name_ " failed!"); \
        ffmpeglib_free_library(); \
        return -1; \
    } 

/* macro for getting functionpointers from avformat */
#define GET_PROC_ADDRESS_AND_TEST_AVFORMAT( _name_ ) \
    p_##_name_ = (_name_##_t) GetProcAddress(avformat_dll, #_name_ ); \
    if (!p_##_name_ ) { \
        log_debug("GetProcAddress " #_name_ " failed!"); \
        ffmpeglib_free_library(); \
        return -1; \
    } 


static int ffmpeglib_load_library(void)
{
    if (!avcodec_dll) {
        avcodec_dll = LoadLibrary("avcodec.dll");

        if (!avcodec_dll) {
            log_debug("LoadLibrary avcodec.dll failed!" );
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
        GET_PROC_ADDRESS_AND_TEST_AVCODEC(__av_freep);
    }

    if (!avformat_dll) {
        avformat_dll = LoadLibrary("avformat.dll");

        if (!avformat_dll) {
            log_debug("LoadLibrary avformat.dll failed!" );
            return -1;
        }

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

    return 0;
}


int ffmpeglib_open(void)
{
    return ffmpeglib_load_library();
}


void ffmpeglib_close(void)
{
    ffmpeglib_free_library();
}


#endif /* #ifdef HAVE_FFMPEG */
