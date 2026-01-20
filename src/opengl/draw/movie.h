/* This file is part of the 'atomes' software

'atomes' is free software: you can redistribute it and/or modify it under the terms
of the GNU Affero General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

'atomes' is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with 'atomes'.
If not, see <https://www.gnu.org/licenses/>

Copyright (C) 2022-2026 by CNRS and University of Strasbourg */

/*!
* @file movie.h
* @short Data structure declarations for movie encoding \n
         Function declarations for movie encoding
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This header file: 'movie.h'
*
* Contains:

 - Data structure declarations for movie encoding
 - Function declarations for movie encoding

*/

#ifndef MOVIE_H_
#define MOVIE_H_

#define VIDEO_CODECS 5
#define IMAGE_FORMATS 4

#include <libavutil/avassert.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#if LIBAVCODEC_VERSION_MAJOR > 54
#include <libavutil/imgutils.h>
#include <libavutil/timestamp.h>
#endif
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libswscale/swscale.h>

// a wrapper around a single output AVStream
typedef struct VideoStream VideoStream;
struct VideoStream
{
    AVStream * st;
    AVCodecContext * cc;
    AVFrame * frame;
    struct SwsContext * sws_ctx;
};

typedef struct video_options video_options;
struct video_options
{
  int proj;
  int framesec;
  int extraframes;
  int codec;
  int oglquality;
  int bitrate;
  int * video_res;
};

extern void render_image (glwin * view, video_options * iopts);
extern void save_movie (glwin * view, video_options * vopts);
#endif
