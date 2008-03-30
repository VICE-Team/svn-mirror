/*
 * ffmpeglib.c - Interface to access the ffmpeg libs.
 *
 * Written by
 *  Andreas Matthies <andreas.matthies@gmx.net>
 *  Andreas Boose <viceteam@t-online.de>
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

#include "gfxoutputdrv/ffmpeglib.h"


#define GET_PROC_ADDRESS_AND_TEST_AVCODEC(_name_) \
    lib->p_##_name_ = _name_;

#define GET_PROC_ADDRESS_AND_TEST_AVFORMAT(_name_) \
    lib->p_##_name_ = _name_;


int ffmpeglib_open(ffmpeglib_t *lib)
{
    GET_PROC_ADDRESS_AND_TEST_AVCODEC(avcodec_open);
    GET_PROC_ADDRESS_AND_TEST_AVCODEC(avcodec_close);
    GET_PROC_ADDRESS_AND_TEST_AVCODEC(avcodec_find_encoder);
    GET_PROC_ADDRESS_AND_TEST_AVCODEC(avcodec_encode_audio);
    GET_PROC_ADDRESS_AND_TEST_AVCODEC(avcodec_encode_video);
    GET_PROC_ADDRESS_AND_TEST_AVCODEC(avpicture_fill);
    GET_PROC_ADDRESS_AND_TEST_AVCODEC(avpicture_get_size);
    GET_PROC_ADDRESS_AND_TEST_AVCODEC(img_convert);
    GET_PROC_ADDRESS_AND_TEST_AVCODEC(__av_freep);

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

    return 0;
}

void ffmpeglib_close(ffmpeglib_t *lib)
{
}

#endif

