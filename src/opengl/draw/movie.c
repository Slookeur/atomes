/* This file is part of the 'atomes' software

'atomes' is free software: you can redistribute it and/or modify it under the terms
of the GNU Affero General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

'atomes' is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with 'atomes'.
If not, see <https://www.gnu.org/licenses/>

Copyright (C) 2022-2025 by CNRS and University of Strasbourg */

/*!
* @file movie.c
* @short Functions to encode a movie / render an image from the OpenGL rendering
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'movie.c'
*
* Contains:
*

 - The functions to encode a movie / render an image from the OpenGL rendering

*
* List of functions:

  gboolean check_to_update_shaders (glwin * view, image * img_a, image * img_b, int ogl_q);
  gboolean create_movie (glwin * view, video_options * vopts, gchar * videofile);

  void convert_rgb_pixbuf_to_yuv (GdkPixbuf * pixbuf, AVFrame * picture, int w, int h);
  void fill_image (VideoStream * vs, int width, int height, glwin * view);
  void set_old_cmap (image * img, int stp, int id);
  void init_frame_buffer (int x, int y);
  void close_frame_buffer ();
  void save_movie (glwin * view, video_options * vopts);

  static void ffmpeg_encoder_set_frame_yuv_from_rgb (uint8_t * rgb, VideoStream * vs);
  static void write_video_frame (AVFormatContext * f_context, VideoStream * vs, int frame_id, glwin * view);
  static void close_stream (AVFormatContext * fc, VideoStream * vs);

  G_MODULE_EXPORT void run_save_movie (GtkNativeDialog * info, gint response_id, gpointer data);
  G_MODULE_EXPORT void run_save_movie (GtkDialog * info, gint response_id, gpointer data);

  static GLubyte * capture_opengl_image (unsigned int width, unsigned int height);

  AVCodecContext * add_codec_context (AVFormatContext * fc, const AVCodec * vc, video_options * vopts);
  static AVFrame * alloc_video_frame (AVCodecContext * cc);

  VideoStream * add_video_stream (AVFormatContext * fc, const AVCodec * vc, video_options * vopts);

*/

#include "global.h"
#include "interface.h"
#include "project.h"
#include "glwindow.h"
#include "glview.h"
#include "movie.h"

#if LIBAVCODEC_VERSION_MAJOR < 56
#  define PIXEL_FORMAT PIX_FMT_YUV420P
#else
#  define PIXEL_FORMAT AV_PIX_FMT_YUV420P
#endif

#define AVS_FRAME_ALIGN 16

#define RGB_TO_Y(pixels, loc) (0.29900 * pixels[loc] + 0.58700 * pixels[loc+1] + 0.11400 * pixels[loc+2])
#define RGB_TO_U(pixels, loc)(-0.16874 * pixels[loc] - 0.33126 * pixels[loc+1] + 0.50000 * pixels[loc+2]+128.0)
#define RGB_TO_V(pixels, loc) (0.50000 * pixels[loc] - 0.41869 * pixels[loc+1] - 0.08131 * pixels[loc+2]+128.0)

#define AVIO_FLAG_READ 1
#define AVIO_FLAG_WRITE 2
#define AVIO_FLAG_READ_WRITE (AVIO_FLAG_READ|AVIO_FLAG_WRITE)

#ifndef AV_ROUND_PASS_MINMAX
#define AV_ROUND_PASS_MINMAX 8192
#endif

#ifndef URL_WRONLY
#  define 	URL_WRONLY   1
#endif

char * codec_name[VIDEO_CODECS] = {"MPEG-1/2",
                                   "MPEG-4",
                                   "H264",
                                   "Theora",
                                   "Flash"};

char * codec_list[VIDEO_CODECS] = {"mpeg",
                                   "avi",
                                   "mkv",
                                   "ogv",
                                   "flv"};

#if LIBAVCODEC_VERSION_MAJOR > 54
int codec_id[VIDEO_CODECS] = {AV_CODEC_ID_MPEG2VIDEO,
                              AV_CODEC_ID_MPEG4,
                              AV_CODEC_ID_H264,
                              AV_CODEC_ID_THEORA,
                              AV_CODEC_ID_FLV1};
#else
int codec_id[VIDEO_CODECS] = {CODEC_ID_MPEG2VIDEO,
                              CODEC_ID_MPEG4,
                              CODEC_ID_H264,
                              CODEC_ID_THEORA,
                              CODEC_ID_FLV1};
#endif

/*#else
GdkGLDrawable * gldrawable;
GdkGLContext * glcontext;
GdkPixmap * pixmap;
GdkGLPixmap * glpixmap;
#endif*/
GdkPixbuf * pixbuf;
uint8_t * video_outbuf;
int video_outbuf_size;
int num_frames;
int frame_start;

/*!
  \fn void convert_rgb_pixbuf_to_yuv (GdkPixbuf * pixbuf, AVFrame * picture, int w, int h)

  \brief convert an RGB pixbuf to an YUV picture frame

  \param pixbuf the Gdk RGB pixbuf to convert
  \param picture the AVFrame to store the data
  \param w image width
  \param h image height
*/
void convert_rgb_pixbuf_to_yuv (GdkPixbuf * pixbuf, AVFrame * picture, int w, int h)
{
  gint x, y, location, location2;
  gint inner_x, inner_y, half_location;
  gfloat cr, cb;
  gint pixbuf_xsize, pixbuf_ysize;
  guchar * pixels;
  gint row_stride;
  gboolean x_odd, y_odd;

  pixbuf_xsize = gdk_pixbuf_get_width (pixbuf);
  pixbuf_ysize = gdk_pixbuf_get_height (pixbuf);
  pixels = gdk_pixbuf_get_pixels (pixbuf);
  row_stride = gdk_pixbuf_get_rowstride (pixbuf);
  y_odd = (pixbuf_ysize & 0x1);
  x_odd = (pixbuf_xsize & 0x1);

  /* note, the Cr and Cb info is subsampled by 2x2 */
  for (y=0; y<pixbuf_ysize-1; y+=2)
  {
    for (x=0; x<pixbuf_xsize-1; x+=2)
    {
      cb = 0.0;
      cr = 0.0;
      for (inner_y = y; inner_y < y+2; inner_y++)
      for (inner_x = x; inner_x < x+2; inner_x++)
      {
        location = inner_y*row_stride+3*inner_x;
        picture -> data[0][inner_x+inner_y*w] = RGB_TO_Y (pixels, location);
        cb += RGB_TO_U (pixels, location);
        cr += RGB_TO_V (pixels, location);
      }
      half_location = x/2 + y*w/4;
      picture -> data[1][half_location] = cb/4.0;
      picture -> data[2][half_location] = cr/4.0;
    }
    if (x_odd)
    {
      location = y*row_stride+3*x;
      location2 = (y+1)*row_stride+3*x;

      picture -> data[0][x+y*w] = RGB_TO_Y (pixels, location);
      picture -> data[0][x+1+y*w] = 0;
      picture -> data[0][x+(y+1)*w] = RGB_TO_Y (pixels, location2);
      picture -> data[0][x+1+(y+1)*w] = 0;

      half_location = x/2 + y*w/4;
      picture -> data[1][half_location] = (RGB_TO_U(pixels, location) + RGB_TO_U(pixels, location2)+256)/4.0;
      picture -> data[2][half_location] = (RGB_TO_V(pixels, location) + RGB_TO_V(pixels, location2)+256)/4.0;
    }
  }
  if (y_odd)
  {
    for (x=0; x<pixbuf_xsize-1; x+=2)
    {
      location = y*row_stride+3*x;
      location2 = y*row_stride+3*(x+1);

      picture -> data[0][x+y*w] = RGB_TO_Y(pixels, location);
      picture -> data[0][x+1+y*w] = RGB_TO_Y(pixels, location2);
      picture -> data[0][x+(y+1)*w] = 0;
      picture -> data[0][x+1+(y+1)*w] = 0;

      half_location = x/2 + y*w/4;
      picture -> data[1][half_location] = (RGB_TO_U(pixels, location)+RGB_TO_U(pixels, location2)+256)/4.0;
      picture -> data[2][half_location] = (RGB_TO_V(pixels, location)+RGB_TO_V(pixels, location2)+256)/4.0;
    }
    if (x_odd)
    {
      location = y*row_stride+3*x;

      picture -> data[0][x+y*w] = RGB_TO_Y(pixels, location);
      picture -> data[0][x+1+y*w] = 0;
      picture -> data[0][x+(y+1)*w] = 0;
      picture -> data[0][x+1+(y+1)*w] = 0;

      half_location = x/2 + y*w/4;
      picture -> data[1][half_location] = (RGB_TO_U(pixels, location)+384)/4.0;
      picture -> data[2][half_location] = (RGB_TO_V(pixels, location)+384)/4.0;
    }
  }
}

/*!
  \fn static void ffmpeg_encoder_set_frame_yuv_from_rgb (uint8_t * rgb, VideoStream * vs)

  \brief set an encoder YUV frame from an RGB image

  \param rgb the RGB data to convert
  \param vs the video stream to encode the data
*/
static void ffmpeg_encoder_set_frame_yuv_from_rgb (uint8_t * rgb, VideoStream * vs)
{
  const int in_linesize = 4 * vs -> cc -> width;
  vs -> sws_ctx = sws_getCachedContext (vs -> sws_ctx,
                                        vs -> cc -> width, vs -> cc -> height, AV_PIX_FMT_BGRA,
                                        vs -> cc -> width, vs -> cc -> height, PIXEL_FORMAT,
                                        0, NULL, NULL, NULL);
  sws_scale (vs -> sws_ctx, (const uint8_t * const *)&rgb,
             & in_linesize, 0, vs -> cc -> height,
             vs -> frame -> data, vs -> frame -> linesize);
}

/*!
  \fn static GLubyte * capture_opengl_image (unsigned int width, unsigned int height)

  \brief capture an OpenGL image from an OpenGL rendering
*/
static GLubyte * capture_opengl_image (unsigned int width, unsigned int height)
{
  size_t i, nvals;
  nvals = width * height * 4;
  GLubyte * pixels = g_malloc (nvals * sizeof(GLubyte));
  GLubyte * rgb = g_malloc (nvals * sizeof(GLubyte));
  glReadPixels (0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, pixels);
  // Flip data veritcally
  for (i = 0; i < height; i++)
  {
    memcpy (rgb + 4 * width * i, pixels  + 4 * width * (height - i - 1), 4 * width);
  }
  g_free (pixels);
  return rgb;
}
//#endif

/*!
  \fn void fill_image (VideoStream * vs, int width, int height, glwin * view)

  \brief render an image from an OpenGL rendering

  \param vs the video stream
  \param width image width
  \param height image height
  \param view the target glwin
*/
void fill_image (VideoStream * vs, int width, int height, glwin * view)
{
  // opengl call is here !!!
  reshape (view, width, height, FALSE);
  draw (view);
  // Might need some correction(s) here for HiDPI screens
  GLubyte * image = capture_opengl_image (width, height);
  if (vs != NULL)
  {
    //if (movie) convert_rgb_pixbuf_to_yuv (pixbuf, frame, width, height);
    ffmpeg_encoder_set_frame_yuv_from_rgb (image, vs);
  }
  else
  {
    cairo_surface_t * surf = cairo_image_surface_create_for_data ((guchar *)image, CAIRO_FORMAT_ARGB32,
                             width, height, cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, width));
    pixbuf = convert_to_pixbuf (surf);
    cairo_surface_destroy (surf);
  }
  g_free (image);
}

/*!
  \fn static void write_video_frame (AVFormatContext * f_context, VideoStream * vs, int frame_id, glwin * view)

  \brief write a video frame from an OpenGL render

  \param f_context the format context to use
  \param vs the video stream
  \param frame_id the frame id number
  \param view the target glwin
*/
static void write_video_frame (AVFormatContext * f_context, VideoStream * vs, int frame_id, glwin * view)
{
  int out_size = 0;

  fill_image (vs, vs -> cc -> width, vs -> cc -> height, view);

  AVPacket packet;
#if LIBAVCODEC_VERSION_MAJOR > 57
  out_size = av_new_packet (& packet, 0);
  if (out_size != 0)
  {
    // "Error while encoding video frame"
    g_warning ("MOVIE_ENCODING:: VIDEO_FRAME:: error:: %s", av_err2str(out_size));
  }
  // get_packet_defaults (& packet);
#else
  av_init_packet (& packet);
#endif

  if (! out_size)
  {
    packet.dts = packet.pts = AV_NOPTS_VALUE;
    packet.data = NULL;
    packet.size = 0;

    vs -> frame -> pts = frame_id + 1;
    out_size = avcodec_send_frame (vs -> cc, vs -> frame);
    if (out_size < 0)
    {
      // "Error while encoding video frame"
      g_warning ("MOVIE_ENCODING:: VIDEO_FRAME:: error:: %s", av_err2str (out_size));
    }
    else
    {
      if (avcodec_receive_packet (vs -> cc, & packet) < 0)
      {
        // "Error while encoding video frame"
         if (frame_id + 1 > frame_start) g_warning ("MOVIE_ENCODING:: VIDEO_FRAME:: warning:: packet empty, ignoring frame= %d", frame_id);
      }
      else
      {
        av_packet_rescale_ts (& packet, vs -> cc -> time_base, vs -> st -> time_base);

        packet.stream_index = vs -> st -> index;
        packet.flags |= AV_PKT_FLAG_KEY;
        out_size = av_interleaved_write_frame (f_context, & packet);
        if (out_size != 0)
        {
          // "Error while encoding video frame"
          g_warning ("MOVIE_ENCODING:: VIDEO_FRAME:: error:: %s", av_err2str(out_size));
        }
        av_packet_unref(& packet);
      }
    }
  }
}

/*!
  \fn static AVFrame * alloc_video_frame (AVCodecContext * cc)

  \brief allocate a video frame using a codec context

  \param cc the codec context
*/
static AVFrame * alloc_video_frame (AVCodecContext * cc)
{
  AVFrame * frame;
  frame = av_frame_alloc ();
  if (! frame)
  {
    return NULL;
  }
  frame -> format = cc -> pix_fmt;
  frame -> width = cc -> width;
  frame -> height = cc -> height;
  if (av_frame_get_buffer (frame, 32) < 0)
  {
    return NULL;
  }
  return frame;
}

/*!
  \fn AVCodecContext * add_codec_context (AVFormatContext * fc, const AVCodec * vc, video_options * vopts)

  \brief create a video codec context

  \param fc the format context
  \param vc the codec
  \param vopts the video encoding options
*/
AVCodecContext * add_codec_context (AVFormatContext * fc, const AVCodec * vc, video_options * vopts)
{
  AVCodecContext * cc;
  if (! (vc = avcodec_find_encoder (codec_id[vopts -> codec])))
  {
    // Codec not found
    g_warning ("MOVIE_ENCODING:: Could not find codec:: %s", codec_name[vopts -> codec]);
    return NULL;
  }
  if (! (cc = avcodec_alloc_context3(vc)))
  {
    g_warning ("MOVIE_ENCODING:: Could not allocate encoding context");
    return NULL;
  }
  //g_debug ("Codec_id= %d", cc -> codec_id);
  cc -> codec_id = codec_id[vopts -> codec];
  //g_debug ("Codec_id= %d", cc -> codec_id);
  /* put sample parameters */
  cc -> bit_rate_tolerance = vopts -> bitrate*1000;
  cc -> bit_rate = vopts -> bitrate*1000;

  /* resolution must be a multiple of two */
  cc -> width = vopts -> video_res[0];
  cc -> height = vopts -> video_res[1];

  cc -> time_base = (AVRational){1, vopts -> framesec};
  cc -> framerate = (AVRational){vopts -> framesec, 1};

  if (vopts -> codec != 1 && vopts -> codec != 4) cc -> max_b_frames = 1;

  cc -> gop_size = vopts -> extraframes; /* emit one intra frame every n frames */
  cc -> pix_fmt = PIXEL_FORMAT;

  if (vopts -> codec == 2) av_opt_set (cc -> priv_data, "preset", "slow", 0);
  /*
   Some container formats (like MP4) require global headers to be present
   Mark the encoder so that it behaves accordingly.
  */
#if LIBAVCODEC_VERSION_MAJOR > 57
  if (fc -> oformat -> flags & AVFMT_GLOBALHEADER) cc -> flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
#else
  if (fc -> oformat -> flags & AVFMT_GLOBALHEADER) cc -> flags |= CODEC_FLAG_GLOBAL_HEADER;
#endif
  return cc;
}

/*!
  \fn VideoStream * add_video_stream (AVFormatContext * fc, const AVCodec * vc, video_options * vopts)

  \brief create video stream and the associated data buffers

  \param fc the format context
  \param vc the codec
  \param vopts the video encoding options
*/
VideoStream * add_video_stream (AVFormatContext * fc, const AVCodec * vc, video_options * vopts)
{
  VideoStream * stream = g_malloc0 (sizeof*stream);
  stream -> cc = add_codec_context (fc, vc, vopts);
  if (stream -> cc == NULL) return NULL;

#if LIBAVCODEC_VERSION_MAJOR > 53
  stream -> st = avformat_new_stream (fc, vc);
#else
  st = av_new_stream (fc, vc);
#endif
  if (! stream -> st)
  {
    g_warning ("MOVIE_ENCODING:: Could not allocate video stream");
    return NULL;
  }
  stream -> st -> time_base = stream -> cc -> time_base;
  stream -> frame = alloc_video_frame (stream -> cc);
  if (stream -> frame == NULL)
  {
    g_warning ("MOVIE_ENCODING:: Could not allocate raw frame buffer");
    return NULL;
  }
  return stream;
}

/*!
  \fn static void close_stream (AVFormatContext * fc, VideoStream * vs)

  \brief close the video stream and free associated data buffers

  \param fc the format context to free
  \param vs the video stream to close
*/
static void close_stream (AVFormatContext * fc, VideoStream * vs)
{
  avcodec_free_context (& vs -> cc);
  av_frame_free (& vs -> frame);
  sws_freeContext (vs -> sws_ctx);
  avformat_free_context (fc);
}

int * old_cmap[2];

/*!
  \fn void set_old_cmap (image * img, int stp, int id)

  \brief preserve color map information

  \param img the target image
  \param stp MD step
  \param id color map id (0 = atom(s), 1 = polyhedra)
*/
void set_old_cmap (image * img, int stp, int id)
{
  old_cmap[id][stp] = img -> color_map[id];
}

/*!
  \fn gboolean check_to_update_shaders (glwin * view, image * img_a, image * img_b, int ogl_q)

  \brief test if it is required to update the OpenGL shaders, and which one(s)

  \param view the target glwin
  \param img_a the previous image
  \param img_b the next image
  \param ogl_q OpenGL quality
*/
gboolean check_to_update_shaders (glwin * view, image * img_a, image * img_b, int ogl_q)
{
  gboolean shaders = FALSE;
  int i, j, k;
  int stp = img_b -> step;

  if (ogl_q == 0 && img_a -> quality != img_b -> quality)
  {
    view -> create_shaders[MDBOX] = TRUE;
    view -> create_shaders[MAXIS] = TRUE;
    view -> n_shaders[ATOMS][stp] = -1;
    view -> create_shaders[ATOMS] = TRUE;
    view -> n_shaders[BONDS][stp] = -1;
    view -> create_shaders[BONDS] = TRUE;
    view -> n_shaders[POLYS][stp] = -1;
    view -> create_shaders[POLYS] = TRUE;
    view -> n_shaders[RINGS][stp] = -1;
    view -> create_shaders[RINGS] = TRUE;
    view -> n_shaders[VOLMS][stp] = -1;
    view -> create_shaders[VOLMS] = TRUE;
    view -> n_shaders[SELEC][stp] = -1;
    view -> create_shaders[SELEC] = TRUE;
    view -> create_shaders[LABEL] = TRUE;
    view -> create_shaders[MEASU] = TRUE;
    for (i=0; i<2; i++) set_old_cmap (img_b, stp, i);
    return TRUE;
  }

  for (i=0; i<3; i++)
  {
    if (img_a -> abc -> extra_cell[i] != img_b -> abc -> extra_cell[i])
    {
      view -> create_shaders[MDBOX] = TRUE;
      view -> create_shaders[MAXIS] = TRUE;
      view -> n_shaders[ATOMS][stp] = -1;
      view -> create_shaders[ATOMS] = TRUE;
      view -> n_shaders[BONDS][stp] = -1;
      view -> create_shaders[BONDS] = TRUE;
      view -> n_shaders[POLYS][stp] = -1;
      view -> create_shaders[POLYS] = TRUE;
      view -> n_shaders[RINGS][stp] = -1;
      view -> create_shaders[RINGS] = TRUE;
      view -> n_shaders[VOLMS][stp] = -1;
      view -> create_shaders[VOLMS] = TRUE;
      view -> n_shaders[SELEC][stp] = -1;
      view -> create_shaders[SELEC] = TRUE;
      view -> create_shaders[LABEL] = TRUE;
      view -> create_shaders[MEASU] = TRUE;
      for (i=0; i<2; i++) set_old_cmap (img_b, stp, i);
      return TRUE;
    }
  }

  project * tmp_proj = get_project_by_id(view -> proj);
  coord_info * this_coord = tmp_proj -> coord;
  for (i=0; i<9; i++)
  {
    for (j=0; j<this_coord -> totcoord[i]; j++)
    {
      if (img_a -> show_coord[i][j] != img_b -> show_coord[i][j])
      {
        view -> n_shaders[ATOMS][stp] = -1;
        view -> create_shaders[ATOMS] = TRUE;
        view -> n_shaders[BONDS][stp] = -1;
        view -> create_shaders[BONDS] = TRUE;
        view -> n_shaders[SELEC][stp] = -1;
        view -> create_shaders[SELEC] = TRUE;
        view -> create_shaders[LABEL] = TRUE;
        view -> create_shaders[MEASU] = TRUE;
      }
    }
    if (i < 2)
    {
      for (j=0; j<this_coord -> totcoord[i]; j++)
      {
        if (img_a -> show_poly[i][j] != img_b -> show_poly[i][j])
        {
          view -> n_shaders[POLYS][stp] = -1;
          view -> create_shaders[POLYS] = TRUE;
        }
      }
    }
    else if (i > 3)
    {
      for (j=0; j<this_coord -> totcoord[i]; j++)
      {
        if (img_a -> show_poly[i][j] != img_b -> show_poly[i][j])
        {
          view -> n_shaders[RINGS][stp] = -1;
          view -> create_shaders[RINGS] = TRUE;
        }
      }
    }
  }
  for (j=0; j<this_coord -> totcoord[9]; j++)
  {
    if (img_a -> show_coord[9][j] != img_b -> show_coord[9][j])
    {
      view -> n_shaders[ATOMS][stp] = -1;
      view -> create_shaders[ATOMS] = TRUE;
      view -> n_shaders[BONDS][stp] = -1;
      view -> create_shaders[BONDS] = TRUE;
      view -> create_shaders[LABEL] = TRUE;
      view -> create_shaders[MEASU] = TRUE;
    }
  }

  if (img_a -> draw_clones != img_b -> draw_clones)
  {
    view -> n_shaders[ATOMS][stp] = -1;
    view -> create_shaders[ATOMS] = TRUE;
    view -> n_shaders[BONDS][stp] = -1;
    view -> create_shaders[BONDS] = TRUE;
    view -> n_shaders[POLYS][stp] = -1;
    view -> create_shaders[POLYS] = TRUE;
    view -> n_shaders[RINGS][stp] = -1;
    view -> create_shaders[RINGS] = TRUE;
    view -> n_shaders[SELEC][stp] = -1;
    view -> create_shaders[SELEC] = TRUE;
    view -> create_shaders[LABEL] = TRUE;
    shaders = TRUE;
  }

  if (img_a -> color_map[0] != img_b -> color_map[0] || img_b -> color_map[0] != old_cmap[0][stp])
  {
    view -> n_shaders[ATOMS][stp] = -1;
    view -> create_shaders[ATOMS] = TRUE;
    view -> n_shaders[BONDS][stp] = -1;
    view -> create_shaders[BONDS] = TRUE;
    view -> create_shaders[LABEL] = TRUE;
    set_old_cmap (img_b, stp, 0);
    shaders = TRUE;
  }

  if (img_a -> color_map[1] != img_b -> color_map[1] || img_b -> color_map[1] != old_cmap[1][stp])
  {
    view -> n_shaders[POLYS][stp] = -1;
    view -> create_shaders[POLYS] = TRUE;
    set_old_cmap (img_b, stp, 1);
    shaders = TRUE;
  }

  if (img_a -> step != img_b -> step)
  {
    if (view -> n_shaders[ATOMS][stp] < 0) view -> create_shaders[ATOMS] = TRUE;
    if (view -> n_shaders[BONDS][stp] < 0) view -> create_shaders[BONDS] = TRUE;
    if (view -> n_shaders[POLYS][stp] < 0) view -> create_shaders[POLYS] = TRUE;
    if (view -> n_shaders[RINGS][stp] < 0) view -> create_shaders[RINGS] = TRUE;
    if (view -> n_shaders[VOLMS][stp] < 0) view -> create_shaders[VOLMS] = TRUE;
    if (view -> n_shaders[SELEC][stp] < 0) view -> create_shaders[SELEC] = TRUE;
    view -> create_shaders[LABEL] = TRUE;
    view -> create_shaders[MEASU] = TRUE;
    shaders = TRUE;
  }

  gboolean do_volms = FALSE;
  for (i=0; i<FILLED_STYLES; i++)
  {
    if (img_a -> show_vol[i] != img_b -> show_vol[i]) do_volms = TRUE;
    if (img_a -> vol_col[i].red != img_b -> vol_col[i].red) do_volms = TRUE;
    if (img_a -> vol_col[i].green != img_b -> vol_col[i].green) do_volms = TRUE;
    if (img_a -> vol_col[i].blue != img_b -> vol_col[i].blue) do_volms = TRUE;
    for (j=0; j<2; j++)
    {
      if (img_a -> fm_show_vol[j][i] == NULL && img_b -> fm_show_vol[j][i] != NULL)
      {
        do_volms = TRUE;
      }
      else if (img_a -> fm_show_vol[j][i] != NULL && img_b -> fm_show_vol[j][i] == NULL)
      {
        do_volms = TRUE;
      }
      else if (img_a -> fm_show_vol[j][i] != NULL && img_b -> fm_show_vol[j][i] != NULL)
      {
        for (k=0; k<this_coord -> totcoord[j+2]; k++)
        {
          if (img_a -> fm_show_vol[j][i][k] != img_b -> fm_show_vol[j][i][k]) do_volms = TRUE;
          if (img_a -> fm_vol_col[j][i][k].red != img_b -> fm_vol_col[j][i][k].red) do_volms = TRUE;
          if (img_a -> fm_vol_col[j][i][k].green != img_b -> fm_vol_col[j][i][k].green) do_volms = TRUE;
          if (img_a -> fm_vol_col[j][i][k].blue != img_b -> fm_vol_col[j][i][k].blue) do_volms = TRUE;
        }
      }
    }
  }
  if (do_volms)
  {
    view -> create_shaders[VOLMS] = shaders = TRUE;
    view -> n_shaders[VOLMS][stp] = -1;
  }

  if (img_a -> abc -> box != img_b -> abc -> box) view -> create_shaders[MDBOX] = shaders = TRUE;
  if (img_a -> abc -> rad != img_b -> abc -> rad) view -> create_shaders[MDBOX] = shaders = TRUE;
  if (img_a -> abc -> line != img_b -> abc -> line) view -> create_shaders[MDBOX] = shaders = TRUE;
  if (img_a -> abc -> color.red != img_b -> abc -> color.red) view -> create_shaders[MDBOX] = shaders = TRUE;
  if (img_a -> abc -> color.green != img_b -> abc -> color.green) view -> create_shaders[MDBOX] = shaders = TRUE;
  if (img_a -> abc -> color.blue != img_b -> abc -> color.blue) view -> create_shaders[MDBOX] = shaders = TRUE;

  if (img_a -> xyz -> axis != img_b -> xyz -> axis) view -> create_shaders[MAXIS] = shaders = TRUE;
  if (img_a -> xyz -> rad != img_b -> xyz -> rad) view -> create_shaders[MAXIS] = shaders = TRUE;
  if (img_a -> xyz -> line != img_b -> xyz -> line) view -> create_shaders[MAXIS] = shaders = TRUE;
  if (img_a -> xyz -> length != img_b -> xyz -> length) view -> create_shaders[MAXIS] = shaders = TRUE;
  if (img_a -> xyz -> t_pos != img_b -> xyz -> t_pos) view -> create_shaders[MAXIS] = shaders = TRUE;
  if (img_a -> xyz -> color == NULL && img_b -> xyz -> color != NULL)
  {
    view -> create_shaders[MAXIS] = shaders = TRUE;
  }
  else if (img_a -> xyz -> color != NULL && img_b -> xyz -> color == NULL)
  {
    view -> create_shaders[MAXIS] = shaders = TRUE;
  }
  else if (img_a -> xyz -> color != NULL && img_b -> xyz -> color != NULL)
  {
    for (i=0; i<3; i++)
    {
      if (img_a -> xyz -> color[i].red != img_b -> xyz -> color[i].red) view -> create_shaders[MAXIS] = shaders = TRUE;
      if (img_a -> xyz -> color[i].green != img_b -> xyz -> color[i].green) view -> create_shaders[MAXIS] = shaders = TRUE;
      if (img_a -> xyz -> color[i].blue != img_b -> xyz -> color[i].blue) view -> create_shaders[MAXIS] = shaders = TRUE;
    }
  }
  if (img_a -> xyz -> labels != img_b -> xyz -> labels) view -> create_shaders[MAXIS] = shaders = TRUE;
  for (i=0; i<3; i++)
  {
    if (g_strcmp0 (img_a -> xyz -> title[i], img_b -> xyz -> title[i]) != 0) view -> create_shaders[MAXIS] = shaders = TRUE;
    if (img_a -> xyz -> c_pos[i] != img_b -> xyz -> c_pos[i]) view -> create_shaders[MAXIS] = shaders = TRUE;
  }
  if (g_strcmp0 (img_a -> labels[2].font, img_b -> labels[2].font) != 0) view -> create_shaders[MAXIS] = shaders = TRUE;
  if (img_a -> labels[2].position != img_b -> labels[2].position) view -> create_shaders[MAXIS] = shaders = TRUE;
  if (img_a -> labels[2].scale != img_b -> labels[2].scale) view -> create_shaders[MAXIS] = shaders = TRUE;
  if (img_a -> labels[2].render != img_b -> labels[2].render) view -> create_shaders[MAXIS] = shaders = TRUE;
  if (img_a -> labels[2].color == NULL && img_b -> labels[2].color != NULL)
  {
    view -> create_shaders[MAXIS] = shaders = TRUE;
  }
  else if (img_a -> labels[2].color != NULL && img_b -> labels[2].color == NULL)
  {
    view -> create_shaders[MAXIS] = shaders = TRUE;
  }
  else if (img_a -> labels[2].color != NULL && img_b -> labels[2].color != NULL)
  {
    for (i=0; i<3; i++)
    {
      if (img_a -> labels[2].color[i].red != img_b -> labels[2].color[i].red) view -> create_shaders[MAXIS] = shaders = TRUE;
      if (img_a -> labels[2].color[i].green != img_b -> labels[2].color[i].green) view -> create_shaders[MAXIS] = shaders = TRUE;
      if (img_a -> labels[2].color[i].blue != img_b -> labels[2].color[i].blue) view -> create_shaders[MAXIS] = shaders = TRUE;
    }
  }

  if (img_a -> cloned_poly != img_b -> cloned_poly)
  {
    view -> n_shaders[POLYS][stp] = -1;
    view -> create_shaders[POLYS] = TRUE;
    view -> n_shaders[RINGS][stp] = -1;
    view -> create_shaders[RINGS] = TRUE;
    shaders = TRUE;
  }

  gboolean dorings = FALSE;
  for (i=0; i<5; i++)
  {
    if (view -> ring_max[i])
    {
      if (img_a -> i_rings[i] && img_b -> i_rings[i])
      {
        if (img_a -> i_rings[i][0][0] != img_b -> i_rings[i][0][0])
        {
          view -> n_shaders[RINGS][stp] = -1;
          view -> create_shaders[RINGS] = TRUE;
          dorings = shaders = TRUE;
          break;
        }
        else
        {
          for (j=0; j<img_a -> i_rings[i][0][0]; j++)
          {
            if ((img_a -> i_rings[i][j+1][0] != img_b -> i_rings[i][j+1][0]) || (img_a -> i_rings[i][j+1][1] != img_b -> i_rings[i][j+1][1]))
            {
              view -> n_shaders[RINGS][stp] = -1;
              view -> create_shaders[RINGS] = TRUE;
              dorings = shaders = TRUE;
              break;
            }
          }
        }
      }
      else if (img_b -> i_rings[i])
      {
        dorings = TRUE;
      }
      if (dorings) break;
    }
  }

  for (i=0; i<2; i++)
  {
    for (j=0; j<tmp_proj -> nspec; j++)
    {
      if (img_a -> show_label[i][j] != img_b -> show_label[i][j])
      {
        view -> create_shaders[LABEL] = shaders = TRUE;
      }
      if (img_a -> show_atom[i][j] != img_b -> show_atom[i][j])
      {
        view -> n_shaders[ATOMS][stp] = -1;
        view -> create_shaders[ATOMS] = TRUE;
        view -> n_shaders[BONDS][stp] = -1;
        view -> create_shaders[BONDS] = TRUE;
        view -> n_shaders[SELEC][stp] = -1;
        view -> create_shaders[SELEC] = TRUE;
        view -> create_shaders[LABEL] = TRUE;
        view -> create_shaders[MEASU] = TRUE;
        shaders = TRUE;
        break;
      }
    }
    for (j=0; j<tmp_proj -> natomes; j++)
    {
      if (img_a -> at_data[j].show[i] != img_b -> at_data[j].show[i] || img_a -> at_data[j].style != img_b -> at_data[j].style)
      {
        view -> n_shaders[ATOMS][stp] = -1;
        view -> create_shaders[ATOMS] = TRUE;
        view -> n_shaders[BONDS][stp] = -1;
        view -> create_shaders[BONDS] = TRUE;
        view -> n_shaders[SELEC][stp] = -1;
        view -> create_shaders[SELEC] = TRUE;
        view -> create_shaders[LABEL] = TRUE;
        view -> create_shaders[MEASU] = TRUE;
        shaders = TRUE;
        break;
      }
      if (img_a -> at_data[j].label[i] != img_b -> at_data[j].label[i])
      {
        view -> create_shaders[LABEL] = shaders = TRUE;
      }
      if (img_a -> at_data[j].pick[0] != img_b -> at_data[j].pick[0])
      {
        view -> n_shaders[SELEC][stp] = -1;
        view -> create_shaders[SELEC] = shaders = TRUE;
      }
      if (img_a -> at_data[j].pick[1] != img_b -> at_data[j].pick[1])
      {
        view -> n_shaders[SELEC][stp] = -1;
        view -> create_shaders[SELEC] = shaders = TRUE;
      }
    }
  }

  for (i=0; i<2*tmp_proj -> nspec; i++)
  {
    if ((img_a -> sphererad[i] != img_b -> sphererad[i])
     || (img_a -> pointrad[i] != img_b -> pointrad[i])
     || (img_a -> atomicrad[i] != img_b -> atomicrad[i]))
    {
      view -> n_shaders[ATOMS][stp] = -1;
      view -> create_shaders[ATOMS] = TRUE;
      view -> n_shaders[SELEC][stp] = -1;
      view -> create_shaders[SELEC] = TRUE;
      view -> create_shaders[LABEL] = TRUE;
      shaders = TRUE;
    }
    for (j=0; j<2*tmp_proj -> nspec; j++)
    {
      if ((img_a -> bondrad[i][j] != img_b -> bondrad[i][j])
      || (img_a -> linerad[i][j] != img_b -> linerad[i][j]))
      {
        view -> n_shaders[BONDS][stp] = -1;
        view -> create_shaders[BONDS] = TRUE;
        view -> n_shaders[SELEC][stp] = -1;
        view -> create_shaders[SELEC] = TRUE;
        view -> create_shaders[LABEL] = TRUE;
        shaders = TRUE;
      }
    }
  }

  for (i=0; i<2; i++)
  {
    if (img_a -> acl_format[i] != img_b -> acl_format[i]) view -> create_shaders[LABEL] = shaders = TRUE;
    if (g_strcmp0 (img_a -> labels[i].font, img_b -> labels[i].font) != 0) view -> create_shaders[LABEL] = shaders = TRUE;
    if (img_a -> labels[i].position != img_b -> labels[i].position) view -> create_shaders[LABEL] = shaders = TRUE;
    if (img_a -> labels[i].scale != img_b -> labels[i].scale) view -> create_shaders[LABEL] = shaders = TRUE;
    if (img_a -> labels[i].render != img_b -> labels[i].render) view -> create_shaders[LABEL] = shaders = TRUE;
    for (j=0; j<3; j++)
    {
      if (img_a -> labels[i].shift[j] != img_b -> labels[i].shift[j]) view -> create_shaders[LABEL] = shaders = TRUE;
    }
    if (img_a -> labels[i].color == NULL && img_b -> labels[i].color != NULL)
    {
      view -> create_shaders[LABEL] = shaders = TRUE;
    }
    else if (img_a -> labels[i].color != NULL && img_b -> labels[i].color == NULL)
    {
      view -> create_shaders[LABEL] = shaders = TRUE;
    }
    else if (img_a -> labels[i].color != NULL && img_b -> labels[i].color != NULL)
    {
      for (j=0; j<tmp_proj -> nspec; j++)
      {
        if (img_a -> labels[i].color[j].red != img_b -> labels[i].color[j].red) view -> create_shaders[LABEL] = shaders = TRUE;
        if (img_a -> labels[i].color[j].green != img_b -> labels[i].color[j].green) view -> create_shaders[LABEL] = shaders = TRUE;
        if (img_a -> labels[i].color[j].blue != img_b -> labels[i].color[j].blue) view -> create_shaders[LABEL] = shaders = TRUE;
      }
    }

    if (img_a -> radall[i] != img_b -> radall[i])
    {
      view -> n_shaders[ATOMS][stp] = -1;
      view -> create_shaders[ATOMS] = TRUE;
      view -> n_shaders[BONDS][stp] = -1;
      view -> create_shaders[BONDS] = TRUE;
      view -> n_shaders[SELEC][stp] = -1;
      view -> create_shaders[SELEC] = TRUE;
      view -> create_shaders[LABEL] = TRUE;
      shaders = TRUE;
    }
  }

  if (img_a -> render != img_b -> render)
  {
    view -> n_shaders[ATOMS][stp] = -1;
    view -> create_shaders[ATOMS] = TRUE;
    view -> n_shaders[BONDS][stp] = -1;
    view -> create_shaders[BONDS] = TRUE;
    view -> n_shaders[SELEC][stp] = -1;
    view -> create_shaders[SELEC] = TRUE;
    shaders = TRUE;
  }
  if (img_a -> style != img_b -> style)
  {
    view -> n_shaders[ATOMS][stp] = -1;
    view -> create_shaders[ATOMS] = TRUE;
    view -> n_shaders[BONDS][stp] = -1;
    view -> create_shaders[BONDS] = TRUE;
    view -> n_shaders[SELEC][stp] = -1;
    view -> create_shaders[SELEC] = TRUE;
    view -> create_shaders[LABEL] = TRUE;
    shaders = TRUE;
  }

  if (img_a -> m_is_pressed != img_b -> m_is_pressed) view -> create_shaders[MEASU] = shaders = TRUE;
  for (i=0; i<2; i++)
  {
    if (img_a -> mtilt[i] != img_b -> mtilt[i]) view -> create_shaders[MEASU] = shaders = TRUE;
    if (img_a -> mpattern[i] != img_b -> mpattern[i]) view -> create_shaders[MEASU] = shaders = TRUE;
    if (img_a -> mfactor[i] != img_b -> mfactor[i]) view -> create_shaders[MEASU] = shaders = TRUE;
    if (img_a -> mwidth[i] != img_b -> mwidth[i]) view -> create_shaders[MEASU] = shaders = TRUE;
    if (g_strcmp0 (img_a -> labels[3+i].font, img_b -> labels[3+i].font) != 0) view -> create_shaders[MEASU] = shaders = TRUE;
    if (img_a -> labels[3+i].position != img_b -> labels[3+i].position) view -> create_shaders[MEASU] = shaders = TRUE;
    if (img_a -> labels[3+i].scale != img_b -> labels[3+i].scale) view -> create_shaders[MEASU] = shaders = TRUE;
    if (img_a -> labels[3+i].render != img_b -> labels[3+i].render) view -> create_shaders[MEASU] = shaders = TRUE;
    if (img_a -> labels[3+i].color[0].red != img_b -> labels[3+i].color[0].red) view -> create_shaders[MEASU] = shaders = TRUE;
    if (img_a -> labels[3+i].color[0].green != img_b -> labels[3+i].color[0].green) view -> create_shaders[MEASU] = shaders = TRUE;
    if (img_a -> labels[3+i].color[0].blue != img_b -> labels[3+i].color[0].blue) view -> create_shaders[MEASU] = shaders = TRUE;
    for (j=0; j<3; j++)
    {
      if (img_a -> labels[3+i].shift[j] != img_b -> labels[3+i].shift[j]) view -> create_shaders[MEASU] = shaders = TRUE;
    }
  }

  return shaders;
}

extern GtkWidget * encoding_pb;

/*
typedef struct{
  int framesec;      // frame(s) per second
  int extraframes;   // extra frame(s) per second
  int codec;         // video codec
  int oglquality;    // OpenGL quality
  int bitrate;       // bitrate
  int video_res[2];  // video resolution (x, y)
} video_options;
*/

/*!
  \fn gboolean create_movie (glwin * view, video_options * vopts, gchar * videofile)

  \brief render a movie from the saved animation parameters

  \param view the target glwin
  \param vopts the video encoding options
  \param videofile video file name
*/
gboolean create_movie (glwin * view, video_options * vopts, gchar * videofile)
{
  int q;
  const AVOutputFormat * output_format = NULL;
  AVFormatContext * format_context = NULL;
  VideoStream * video_stream = NULL;
  const AVCodec * video_codec = NULL;

  int error;

#ifdef DEBUG
  g_debug ("VIDEO ENCODING:: frames per seconds:: %d", vopts -> framesec);
  g_debug ("VIDEO ENCODING:: extra frames every:: %d frame(s)", vopts -> extraframes);
  g_debug ("VIDEO ENCODING:: bitrate:: %d", vopts -> bitrate);
  g_debug ("VIDEO ENCODING:: video_x = %d , video_y = %d", vopts -> video_res[0], vopts -> video_res[1]);
  g_debug ("VIDEO ENCODING:: codec:: %d, name= %s, ext= %s", vopts -> codec, codec_name[vopts -> codec], codec_list[vopts -> codec]);
#endif // DEBUG

  num_frames = view -> anim -> frames;
#if LIBAVCODEC_VERSION_MAJOR < 57
  av_register_all ();
  avcodec_register_all ();
#endif

  if (! (format_context = avformat_alloc_context()))
  {
    g_warning ("MOVIE_ENCODING:: Could not allocate AV format context");
    return FALSE;
  }

  // Guess the desired container format based on file extension
  if (! (format_context -> oformat = av_guess_format (NULL, videofile, NULL)))
  {
    g_warning ("MOVIE_ENCODING:: Could not deduce container format: please change file name");
    return FALSE;
  }

  output_format = format_context -> oformat;

  video_stream = add_video_stream (format_context, video_codec, vopts);
  if (video_stream == NULL)
  {
    g_warning ("MOVIE_ENCODING:: Could not create video stream");
    return FALSE;
  }

  /* open the codec */
  if ((error = avcodec_open2 (video_stream -> cc, video_codec, NULL)) < 0)
  {
    // Can not open codec
    g_warning ("MOVIE_ENCODING:: could not open codec, error= %s", av_err2str(error));
    return FALSE;
  }

  avcodec_parameters_from_context (video_stream -> st -> codecpar, video_stream -> cc);
#if LIBAVCODEC_VERSION_MAJOR > 52
  av_dump_format (format_context, 0, videofile, 1);
#else
  dump_format (av_format_context, 0, videofile, 1);
#endif

#if LIBAVCODEC_VERSION_MAJOR > 52
  if (avio_open (& format_context -> pb, videofile, AVIO_FLAG_WRITE) < 0)
#else
  if (url_fopen (& av_format_context -> pb, videofile, URL_WRONLY) < 0)
#endif
  {
  // error impossible to open output file
    g_warning ("MOVIE_ENCODING:: Impossible to open the video file '%s'", videofile);
    return FALSE;
  }

#if LIBAVCODEC_VERSION_MAJOR > 52
  if (avformat_write_header (format_context, NULL) < 0)
#else
  if (av_set_parameters (av_format_context, NULL) < 0)
#endif
  {
    g_warning ("MOVIE_ENCODING:: Impossible to write the AV format header");
    return FALSE;
  }

  view -> anim -> last = view -> anim -> first;
  if (vopts -> oglquality != 0)
  {
    q = view -> anim -> last -> img -> quality;
    view -> anim -> last -> img -> quality = vopts -> oglquality;
  }

  int frame_id;
  frame_start = 0;
  if (vopts -> codec == 0)
  {
    frame_start = 1;
    write_video_frame (format_context, video_stream, 0, view);
  }
  else if (vopts -> codec == 2)
  {
    frame_start = 24;
    for (frame_id = 0; frame_id < frame_start; frame_id ++)
    {
      write_video_frame (format_context, video_stream, frame_id, view);
    }
  }
  re_create_all_md_shaders (view);
  recreate_all_shaders (view);
  for (frame_id=0; frame_id<2; frame_id++)
  {
    old_cmap[frame_id] = allocint(get_project_by_id(view -> proj) -> steps);
    set_old_cmap (view -> anim -> last -> img, 0, frame_id);
  }
  double fraction;
#ifdef GTK4
  // GMainContext * context = g_main_loop_get_context (Event_loop[dialog_id]);
#endif // GTK4
  for (frame_id = frame_start; frame_id < num_frames+frame_start; frame_id ++)
  {
    //g_debug ("Rendering frame: %d, id= %d", frame_id-frame_start, view -> anim -> last -> img -> id);
    if (vopts -> oglquality != 0)
    {
      view -> anim -> last -> img -> quality = vopts -> oglquality;
    }
    write_video_frame (format_context, video_stream, frame_id, view);
    if (frame_id-frame_start > 0 && frame_id-frame_start - 10*((frame_id-frame_start)/10) == 0)
    {
      fraction = (double)(frame_id-frame_start+1)/num_frames;
      gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(encoding_pb), fraction);
#ifdef GTK3
      while (gtk_events_pending()) gtk_main_iteration();
#else
      // while (g_main_context_pending (context)) g_main_context_iteration (context, TRUE);
#endif
    }
    if (frame_id-frame_start < num_frames-1)
    {
      check_to_update_shaders (view,
                               view -> anim -> last -> img,
                               view -> anim -> last -> next -> img,
                               vopts -> oglquality);
      view -> anim -> last = view -> anim -> last -> next;
    }
  }
  if (vopts -> oglquality != 0)
  {
    view -> anim -> last -> img -> quality = q;
  }

  av_write_trailer (format_context);

  if (!(output_format -> flags & AVFMT_NOFILE))
  {
    /* close the output file */
#if LIBAVCODEC_VERSION_MAJOR > 52
    avio_closep (& format_context -> pb);
#else
    url_fclose (av_format_context -> pb);
#endif
  }

  close_stream (format_context, video_stream);

  return TRUE;
}

static GLuint fbo;
static GLuint rbo_color;
static GLuint rbo_depth;
/*!
  \fn void init_frame_buffer (int x, int y)

  \brief init a frame buffer

  \param x x size - image width
  \param y y size - image height
*/
void init_frame_buffer (int x, int y)
{
  glGenFramebuffers (1, & fbo);
  glBindFramebuffer (GL_FRAMEBUFFER, fbo);

  /* Color renderbuffer. */
  glGenRenderbuffers (1, & rbo_color);
  glBindRenderbuffer (GL_RENDERBUFFER, rbo_color);

  /* Storage */
  // glRenderbufferStorage (GL_RENDERBUFFER, GL_RGB32F, x, y);
  // glRenderbufferStorage (GL_RENDERBUFFER, GL_RGBA4, x, y);
  glRenderbufferStorage (GL_RENDERBUFFER, GL_RGB, x, y);
  glFramebufferRenderbuffer (GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo_color);
  /* Depth renderbuffer. */
  glGenRenderbuffers (1, & rbo_depth);
  glBindRenderbuffer (GL_RENDERBUFFER, rbo_depth);
  glRenderbufferStorage (GL_RENDERBUFFER, GL_DEPTH_COMPONENT, x, y);
  glFramebufferRenderbuffer (GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_depth);

  glReadBuffer (GL_COLOR_ATTACHMENT0);
}

/*!
  \fn void close_frame_buffer ()

  \brief close the frame buffer
*/
void close_frame_buffer ()
{
  glDeleteFramebuffers (1, &fbo);
  glDeleteRenderbuffers (1, &rbo_color);
  glDeleteRenderbuffers (1, &rbo_depth);
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void run_save_movie (GtkNativeDialog * info, gint response_id, gpointer data)

  \brief saving a movie - running the dialog

  \param info the GtkNativeDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_save_movie (GtkNativeDialog * info, gint response_id, gpointer data)
{
  GtkFileChooser * chooser = GTK_FILE_CHOOSER((GtkFileChooserNative *)info);
#else
/*!
  \fn G_MODULE_EXPORT void run_save_movie (GtkDialog * info, gint response_id, gpointer data)

  \brief saving a movie - running the dialog

  \param info the GtkDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_save_movie (GtkDialog * info, gint response_id, gpointer data)
{
  GtkFileChooser * chooser = GTK_FILE_CHOOSER((GtkWidget *)info);
#endif
  if (response_id == GTK_RESPONSE_ACCEPT)
  {
    gchar * videofile = file_chooser_get_file_name (chooser);
#ifdef GTK4
    destroy_this_native_dialog (info);
#else
    destroy_this_dialog (info);
#endif
    video_options * vopts = (video_options *)data;
    glwin * view = get_project_by_id (vopts -> proj) -> modelgl;
    int x = view -> pixels[0];
    int y = view -> pixels[1];
    int i;
    for (i=0; i<2; i++) tmp_pixels[i] = view -> pixels[i];
    view -> pixels[0] = vopts -> video_res[0];
    view -> pixels[1] = vopts -> video_res[1];
    init_frame_buffer (vopts -> video_res[0], vopts -> video_res[1]);
    init_opengl ();
    re_create_all_md_shaders (view);
    recreate_all_shaders (view);
    in_movie_encoding = TRUE;
    gboolean res = create_movie (view, vopts, videofile);
    if (! res)
    {
      show_warning ("An error occurred when encoding movie\nyou might want to try again\nsorry for the trouble", view -> win);
    }
    close_frame_buffer ();
    in_movie_encoding = FALSE;
    re_create_all_md_shaders (view);
    recreate_all_shaders (view);
    reshape (view, x, y, TRUE);
    update (view);
  }
  else
  {
#ifdef GTK4
    destroy_this_native_dialog (info);
#else
    destroy_this_dialog (info);
#endif
  }
}

/*!
  \fn void save_movie (glwin * view, video_options * vopts)

  \brief saving a movie - prepare the dialog

  \param view the target glwin
  \param vopts the video encoding options
*/
void save_movie (glwin * view, video_options * vopts)
{
  GtkFileFilter * filter;
  gchar * str;
#ifdef GTK4
  GtkFileChooserNative * info;
#else
  GtkWidget * info;
#endif
  info = create_file_chooser ("Render Movie",
                              GTK_WINDOW(view -> win),
                              GTK_FILE_CHOOSER_ACTION_SAVE,
                              "Save");
  GtkFileChooser * chooser = GTK_FILE_CHOOSER(info);
#ifdef GTK3
  gtk_file_chooser_set_do_overwrite_confirmation (chooser, TRUE);
#endif
  file_chooser_set_current_folder (chooser);
  str = g_strdup_printf ("%s.%s", prepare_for_title(get_project_by_id(view -> proj) -> name), codec_list[vopts -> codec]);
  gtk_file_chooser_set_current_name (chooser, str);
  g_free (str);

  filter = gtk_file_filter_new ();
  str = g_strdup_printf ("%s file (*.%s)", codec_name[vopts -> codec], codec_list[vopts -> codec]);
  gtk_file_filter_set_name (GTK_FILE_FILTER(filter), str);
  g_free (str);
  str = g_strdup_printf ("*.%s",  codec_list[vopts -> codec]);
  gtk_file_filter_add_pattern (GTK_FILE_FILTER(filter), str);
  g_free (str);
  gtk_file_chooser_add_filter (chooser, filter);
#ifdef GTK4
  run_this_gtk_native_dialog ((GtkNativeDialog *)info, G_CALLBACK(run_save_movie), vopts);
#else
  run_this_gtk_dialog (info, G_CALLBACK(run_save_movie), vopts);
#endif
}
