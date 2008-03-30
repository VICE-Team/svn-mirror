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

#ifndef _FFMPEGLIB_H
#define _FFMPEGLIB_H

#if defined (WIN32) && !defined(__GNUC__)
#undef inline
#endif

#include "ffmpeg/avformat.h"

/* avcodec fucntions */
typedef int (*avcodec_open_t) (AVCodecContext*, AVCodec*);
typedef int (*avcodec_close_t) (AVCodecContext*);
typedef AVCodec* (*avcodec_find_encoder_t) (enum CodecID);
typedef int (*avcodec_encode_audio_t) (AVCodecContext*, uint8_t*, int, const short*);
typedef int (*avcodec_encode_video_t) (AVCodecContext*, uint8_t*, int, const AVFrame*);
typedef int (*avpicture_fill_t) (AVPicture*, uint8_t*, int, int, int);
typedef int (*avpicture_get_size_t) (int, int, int);
typedef int (*img_convert_t) (AVPicture*, int, AVPicture*, int, int, int);
typedef void (*__av_freep_t) (void**);

extern avcodec_open_t               p_avcodec_open;
extern avcodec_close_t              p_avcodec_close;
extern avcodec_find_encoder_t       p_avcodec_find_encoder;
extern avcodec_encode_audio_t       p_avcodec_encode_audio;
extern avcodec_encode_video_t       p_avcodec_encode_video;
extern avpicture_fill_t             p_avpicture_fill;
extern avpicture_get_size_t         p_avpicture_get_size;
extern img_convert_t                p_img_convert;
extern __av_freep_t                 p___av_freep;

/* avformat fucntions */
typedef void (*av_register_all_t) (void);
typedef AVStream* (*av_new_stream_t) (AVFormatContext*, int);
typedef int (*av_set_parameters_t) (AVFormatContext*, AVFormatParameters*);
typedef int (*av_write_header_t) (AVFormatContext*);
typedef int (*av_write_frame_t) (AVFormatContext*, int, const uint8_t*, int);
typedef int (*av_write_trailer_t) (AVFormatContext*);
typedef int (*url_fopen_t) (ByteIOContext*, const char*, int);
typedef int (*url_fclose_t) (ByteIOContext*);
typedef void (*dump_format_t) (AVFormatContext *, int, const char*, int);
typedef AVOutputFormat* (*guess_format_t) (const char*, const char*, const char*);

extern av_register_all_t            p_av_register_all;
extern av_new_stream_t              p_av_new_stream;
extern av_set_parameters_t          p_av_set_parameters;
extern av_write_header_t            p_av_write_header;
extern av_write_frame_t             p_av_write_frame;
extern av_write_trailer_t           p_av_write_trailer;
extern url_fopen_t                  p_url_fopen;
extern url_fclose_t                 p_url_fclose;
extern dump_format_t                p_dump_format;
extern guess_format_t               p_guess_format;

extern int ffmpeglib_open(void);
extern void ffmpeglib_close(void);

#endif
