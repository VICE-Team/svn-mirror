/*
 * ffmpeglib.h - Interface to access the ffmpeg libs.
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

#ifndef VICE_FFMPEGLIB_H
#define VICE_FFMPEGLIB_H

#include "vice.h"

#if defined (WIN32) && !defined(__GNUC__)
/* #undef inline */
#endif

#ifdef HAVE_FFMPEG_AVFORMAT_H
#include <ffmpeg/avformat.h>
#else
#include "ffmpeg/avformat.h"
#endif

/* avcodec fucntions */
typedef int (*avcodec_open_t) (AVCodecContext*, AVCodec*);
typedef int (*avcodec_close_t) (AVCodecContext*);
typedef AVCodec* (*avcodec_find_encoder_t) (enum CodecID);
typedef int (*avcodec_encode_audio_t) (AVCodecContext*, uint8_t*, int, const short*);
typedef int (*avcodec_encode_video_t) (AVCodecContext*, uint8_t*, int, const AVFrame*);
typedef int (*avpicture_fill_t) (AVPicture*, uint8_t*, int, int, int);
typedef int (*avpicture_get_size_t) (int, int, int);
typedef int (*img_convert_t) (AVPicture*, int, AVPicture*, int, int, int);
typedef void (*av_free_t) (void**);
typedef unsigned (*avcodec_version_t) (void);

/* avformat fucntions */
typedef void (*av_register_all_t) (void);
typedef AVStream* (*av_new_stream_t) (AVFormatContext*, int);
typedef int (*av_set_parameters_t) (AVFormatContext*, AVFormatParameters*);
typedef int (*av_write_header_t) (AVFormatContext*);
#if FFMPEG_VERSION_INT==0x000408
typedef int (*av_write_frame_t) (AVFormatContext*, int, const uint8_t*, int);
#else
typedef int (*av_write_frame_t) (AVFormatContext*, AVPacket*);
#endif
typedef int (*av_write_trailer_t) (AVFormatContext*);
typedef int (*url_fopen_t) (ByteIOContext*, const char*, int);
typedef int (*url_fclose_t) (ByteIOContext*);
typedef void (*dump_format_t) (AVFormatContext *, int, const char*, int);
typedef AVOutputFormat* (*guess_format_t) (const char*, const char*, const char*);

struct ffmpeglib_s {
    avcodec_open_t              p_avcodec_open;
    avcodec_close_t             p_avcodec_close;
    avcodec_find_encoder_t      p_avcodec_find_encoder;
    avcodec_encode_audio_t      p_avcodec_encode_audio;
    avcodec_encode_video_t      p_avcodec_encode_video;
    avpicture_fill_t            p_avpicture_fill;
    avpicture_get_size_t        p_avpicture_get_size;
    img_convert_t               p_img_convert;
    av_free_t                   p_av_free;

    av_register_all_t           p_av_register_all;
    av_new_stream_t             p_av_new_stream;
    av_set_parameters_t         p_av_set_parameters;
    av_write_header_t           p_av_write_header;
    av_write_frame_t            p_av_write_frame;
    av_write_trailer_t          p_av_write_trailer;
    url_fopen_t                 p_url_fopen;
    url_fclose_t                p_url_fclose;
    dump_format_t               p_dump_format;
    guess_format_t              p_guess_format;
};

typedef struct ffmpeglib_s ffmpeglib_t;


extern int ffmpeglib_open(ffmpeglib_t *lib);
extern void ffmpeglib_close(ffmpeglib_t *lib);

#endif
