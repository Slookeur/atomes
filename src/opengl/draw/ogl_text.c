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
* @file ogl_text.c
* @short Functions to prepare OpenGL rendering for text
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'ogl_text.c'
*
* Contains:
*

 - The functions to prepare OpenGL rendering for text

*
* List of functions:

  int * paint_bitmap (vec4_t color, GLfloat a, int cw, int ch, unsigned char * buff);

  void render_string (int glsl, int id, screen_string * this_string);
  void debug_string (screen_string  * this_string);
  void render_all_strings (int glsl, int id);
  void add_string (char * text, int id, ColRGBA col, vec3_t pos, float lshift[3], atom * at, atom * bt, atom * ct);
  void prepare_string (char * text, int id, ColRGBA col, vec3_t pos, float lshift[3], atom * at, atom * bt, atom * ct);

  static void normalize_text_size (GLenum texture,  int * width, int * height);

  screen_string * was_not_rendered_already (char * word, screen_string * list);

  ColRGBA * opposite_color (ColRGBA col);

  object_3d * create_string_texture (int cwidth, int cheight, int * pixels);
  object_3d * gl_pango_render_layout (PangoLayout * layout, GLenum texture, int id, screen_string * this_string);

*/

#include "global.h"
#include "glview.h"

#define PANGO_TEXT_SIZE 500
#define OUTLINE_WIDTH 3

#ifndef GL_CLAMP_TO_EDGE
#  define GL_CLAMP_TO_EDGE 0x812F
#endif

#ifndef GL_TEXTURE_WRAP_R
#  define GL_TEXTURE_WRAP_R 0x8072
#endif

extern int measures_drawing;
extern int type_of_measure;
extern void update_string_instances (glsl_program * glsl, object_3d * obj);

GLuint textures_id[2];

const int OUTLINE_BRUSH[2*OUTLINE_WIDTH+1][2*OUTLINE_WIDTH+1]
= {{ 10, 30,  45,  50,  45,  30,  10 },
   { 30, 65,  85,  100,  85, 65,  30 },
   { 45, 85,  200, 256, 200, 85,  45 },
   { 50, 100, 256, 256, 256, 100, 50 },
   { 45, 85,  200, 256, 200, 85,  45 },
   { 30, 65,  85,  100,  85, 65,  30 },
   { 10, 30,  45,  50,  45,  30,  10 }};

/*!
  \fn static void normalize_text_size (GLenum texture,  int * width, int * height)

  \brief normalize the text size

  \param texture the texture type
  \param width image width
  \param height image height
*/
static void normalize_text_size (GLenum texture,  int * width, int * height)
{
  // if the texture target is GL_TEXTURE_2D, that means that
  // the texture_rectangle OpenGL extension is unsupported and we must
  // use only square, power-of-two textures.
  if (texture == GL_TEXTURE_2D)
  {
    int x = max (* width, * height);
    // find next power of two
    int n;
    for (n = 1; n < x; n = n << 1);
    // the texture must be square, and its size must be a power of two.
    * width = * height = n;
  }
}

/*!
  \fn int * paint_bitmap (vec4_t color, GLfloat a, int cw, int ch, unsigned char * buff)

  \brief paint bitmap data using color

  \param color the color
  \param a opacity
  \param cw width
  \param ch height
  \param buff the data to paint
*/
int * paint_bitmap (vec4_t color, GLfloat a, int cw, int ch, unsigned char * buff)
{
  int i;
  guint8 * row, * row_end;
  guint32 rgb;
  guint32 * p;
  int * bitmap;

#if ! defined(GL_VERSION_1_2) && G_BYTE_ORDER == G_LITTLE_ENDIAN
  rgb = ((guint32) (color.x* 255.0))         |
        (((guint32) (color.y * 255.0)) << 8) |
        (((guint32) (color.z * 255.0)) << 16);
#else
  rgb = (((guint32) (color.x * 255.0)) << 24) |
        (((guint32) (color.y * 255.0)) << 16) |
        (((guint32) (color.z * 255.0)) << 8);
#endif
  bitmap = allocint (cw * ch * 4);
  p = (guint32 *) bitmap;
  row = buff + cw*ch;          /* past-the-end */
  row_end = buff;              /* beginning */
  if (a == 1.0)
  {
    while (row != row_end)
    {
       row -= cw;
       for (i = 0; i < cw; i++)
       {
#if ! defined(GL_VERSION_1_2) && G_BYTE_ORDER == G_LITTLE_ENDIAN
         * p++ = rgb | (((guint32) row[i]) << 24);
#else
         * p++ = rgb | ((guint32) row[i]);
#endif
       }
    }
  }
  else
  {
    while (row != row_end)
    {
      row -= cw;
      for (i = 0; i < cw; i++)
      {
#if ! defined(GL_VERSION_1_2) && G_BYTE_ORDER == G_LITTLE_ENDIAN
         * p++ = rgb | (((guint32) (a * row[i])) << 24);
#else
         * p++ = rgb | ((guint32) (a * row[i]));
#endif
      }
    }
  }

  return bitmap;
}

gboolean render_format;

/*!
  \fn object_3d * create_string_texture (int cwidth, int cheight, int * pixels)

  \brief OpenGL 3D string object rendering

  \param cwidth width
  \param cheight height
  \param pixels the data to render
*/
object_3d * create_string_texture (int cwidth, int cheight, int * pixels)
{
  int i, j, n;
  int di, dj;
  int fi, fj, fn;
  double x, y;
  object_3d * new_string;

  new_string = g_malloc0 (sizeof*new_string);
  if (! new_string) return NULL;

  int csize = cwidth * cheight;

  int * rawbitmap;
  rawbitmap = allocint(csize);
  if (! rawbitmap) return NULL;

  n = 0;
  for (j = 0; j < cheight; j++)
  {
    for (i = 0; i < cwidth; i++)
    {
      x = (GLubyte)(pixels [n])/255.0;
      y = pow(x, 0.75);
      rawbitmap[n] = (int)(255.0 * y);
      n++;
    }
  }

  int * neighborhood;
  neighborhood = allocint (csize);
  if (! neighborhood) return NULL;

  for (i = 0; i < cheight; i++)
  {
    for (j = 0; j < cwidth; j++)
    {
      n = j + i * cwidth;
      for (di = -OUTLINE_WIDTH; di <= OUTLINE_WIDTH; di++)
      {
        for (dj = -OUTLINE_WIDTH; dj <= OUTLINE_WIDTH; dj++)
        {
          fi = i + di;
          fj = j + dj;
          if (fi >= 0 && fi < cheight && fj >= 0 && fj < cwidth)
          {
            fn = fj + fi * cwidth;
            neighborhood[fn] = max (neighborhood[fn],
                                    rawbitmap[n]
                                    * OUTLINE_BRUSH[OUTLINE_WIDTH + di]
                                    [OUTLINE_WIDTH + dj]);
          }
        }
      }
    }
  }

  // Two channels, for the outline and the glyph
  GLubyte * channels[2];
  for (i=0; i<2; i++)
  {
    channels[i] = g_malloc (csize*sizeof*channels[i]);
    if (! channels[i]) return NULL;
  }

  for (n = 0; n < csize; n++)
  {
    channels[1][n] = (GLubyte)rawbitmap[n];
    i = (neighborhood[n] >> 8) + rawbitmap[n];
    if (i > 255)
    {
      i = 255;
    }
    channels[0][n] = (GLubyte)i;
  }

  g_free (neighborhood);

  glPixelStorei (GL_UNPACK_ALIGNMENT, 1);

  for (i=0; i<2; i++)
  {
    glGenTextures (1, & textures_id[i]);
    if (! textures_id[i]) return NULL;
    glBindTexture (ogl_texture, textures_id[i]);
    glTexImage2D (ogl_texture,
                  0,
                  GL_RED,
                  cwidth,
                  cheight,
                  0,
                  GL_RED,
                  GL_UNSIGNED_BYTE,
                  channels[i]);

    glTexParameteri (ogl_texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri (ogl_texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri (ogl_texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri (ogl_texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri (ogl_texture, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture (ogl_texture, 0);
  }
  return new_string;
}

/*!
  \fn object_3d * gl_pango_render_layout (PangoLayout * layout, GLenum texture, int id, screen_string * this_string)

  \brief OpenGL 3D pango layout object rendering

  \param layout the Pango layout
  \param texture the OpenGL texture type
  \param id the label id
  \param this_string the screen string
*/
object_3d * gl_pango_render_layout (PangoLayout * layout, GLenum texture, int id, screen_string * this_string)
{
  FT_Bitmap bitmap;
  PangoRectangle prect;
  object_3d * new_string;

  int * pixels;
  int cwidth, cheight;
  int csize;

  pango_layout_get_extents (layout, NULL, & prect);
  if (prect.width == 0 || prect.height == 0) return NULL;
  cheight = bitmap.rows = PANGO_PIXELS (prect.height) + 2*OUTLINE_WIDTH;
  cwidth = bitmap.width = PANGO_PIXELS (prect.width) + 2*OUTLINE_WIDTH;

  normalize_text_size (ogl_texture, & cwidth, & cheight);

  bitmap.pitch = cwidth;
  csize = cheight*cwidth;
  bitmap.buffer = g_malloc (csize);
  memset (bitmap.buffer, 0, csize);

  bitmap.num_grays = 256;
  bitmap.pixel_mode = ft_pixel_mode_grays;
  pango_ft2_render_layout (& bitmap, layout, PANGO_PIXELS (-prect.x)+OUTLINE_WIDTH, OUTLINE_WIDTH);
  pixels = paint_bitmap (vec4(1.0,0.0,0.0,0.0), 1.0, cwidth, cheight, bitmap.buffer);
  if (! plot -> labels[id].render)
  {
    new_string = g_malloc0 (sizeof*new_string);
    new_string -> texture = -1;
    glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
    glGenTextures (1, & new_string -> texture);
    if (! new_string -> texture) return NULL;
    glBindTexture (ogl_texture,  new_string -> texture);
    glTexImage2D (ogl_texture,
                  0,
                  GL_RGBA,
                  cwidth,
                  cheight,
                  0,
                  GL_RGBA,
                  GL_UNSIGNED_BYTE,
                  pixels);

    glTexParameteri (ogl_texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri (ogl_texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri (ogl_texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri (ogl_texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri (ogl_texture, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture (ogl_texture, 0);
  }
  else
  {
    new_string = create_string_texture (cwidth, cheight, pixels);
  }

  g_free (pixels);

  new_string -> num_vertices = 4;
  new_string -> vert_buffer_size = CHAR_BUFF_SIZE;
  new_string -> vertices = allocfloat (new_string -> num_vertices*CHAR_BUFF_SIZE);
  float x = (float) cwidth;
  float y = (float) cheight;
  float twidth = (ogl_texture == GL_TEXTURE_2D) ? 1.0 : x;
  float theight = (ogl_texture == GL_TEXTURE_2D) ? 1.0 : y;
  // 0
  new_string -> vertices[0] = -x;
  new_string -> vertices[1] =  y;
  new_string -> vertices[2] = 0.0;
  new_string -> vertices[3] = theight;
  // 1
  new_string -> vertices[4] = -x;
  new_string -> vertices[5] = -y;
  new_string -> vertices[6] = 0.0;
  new_string -> vertices[7] = 0.0;
  // 2
  new_string -> vertices[8] = x;
  new_string -> vertices[9] = y;
  new_string -> vertices[10] = twidth;
  new_string -> vertices[11] = theight;
  // 3
  new_string -> vertices[12] =  x;
  new_string -> vertices[13] = -y;
  new_string -> vertices[14] = twidth;
  new_string -> vertices[15] = 0.0;

  new_string -> num_instances = this_string -> num_instances;
  int i = (this_string -> type == 3) ? 1 : (this_string -> type == 4) ? 3 : 4;
  new_string -> inst_buffer_size = 3*i;
  new_string -> instances = duplicate_float (this_string -> num_instances*3*i, this_string -> instances);

  return new_string;
}

/*!
  \fn ColRGBA * opposite_color (ColRGBA col)

  \brief compute the opposite color

  \param col input color
*/
ColRGBA * opposite_color (ColRGBA col)
{
  ColRGBA * ocol = g_malloc0 (sizeof*ocol);
  ocol -> red   = (1.0-col.red)/2.5;
  ocol -> green = (1.0-col.green)/2.5;
  ocol -> blue  = (1.0-col.blue)/2.5;
  ocol -> alpha = 1.0;
  return ocol;
}

/*!
  \fn void render_string (int glsl, int id, screen_string * this_string)

  \brief render a screen string

  \param glsl the shader id
  \param id the label id
  \param this_string the screen string to render
*/
void render_string (int glsl, int id, screen_string * this_string)
{
  int j, k, l;
  double font_size;
  // Pango elements for labels
  PangoContext * pcontext;
  PangoLayout * playout;
  PangoFontDescription * pfont;
  object_3d * string_to_render = NULL;
  int arr_size = (this_string -> type == 4) ? 5 : (this_string -> type == 5) ? 6 : this_string -> type;
  int shid = (id == 4) ? 1 : 0;
  //pcontext = pango_ft2_get_context (PANGO_TEXT_SIZE, PANGO_TEXT_SIZE);
  pcontext = pango_font_map_create_context (pango_ft2_font_map_new ());
  playout = pango_layout_new (pcontext);
  pango_layout_set_alignment (playout, PANGO_ALIGN_CENTER);
  pfont = pango_font_description_from_string (plot -> labels[id].font);
  j = pango_font_description_get_size (pfont);
  font_size = j / PANGO_SCALE;

  //g_debug ("Zoom = %f, gnear= %f, p_moy= %f, p_depth= %f", plot -> zoom, plot -> gnear, wingl -> p_moy, plot -> p_depth);
  if (plot -> labels[id].scale) font_size *= ((ZOOM/plot -> zoom)*(plot -> gnear/6.0)*(wingl -> p_moy/plot -> p_depth));
  if (in_movie_encoding)
  {
    l = (wingl -> pixels[0] > wingl -> pixels[1]) ? 1 : 0;
    font_size *= ((float)wingl -> pixels[l]/(float)tmp_pixels[l]);
  }
  pango_font_description_set_absolute_size (pfont, font_size*PANGO_SCALE);
  pango_layout_set_font_description (playout, pfont);
  pango_layout_set_text (playout, this_string -> word, strlen(this_string -> word));
  string_to_render = gl_pango_render_layout (playout, ogl_texture, id, this_string);
  if (string_to_render == NULL)
  {
    g_warning ("TEXT_RENDER:: For some reason it is impossible to render");
    g_warning ("TEXT_RENDER:: this string: '%s', using this font: %s", this_string -> word, plot -> labels[id].font);
  }
  else
  {
    for (k=0; k<3; k++) string_to_render -> shift[k] = this_string -> shift[k];
    string_to_render -> shift[3] = (float) plot -> labels[id].position;
    string_to_render -> quality = this_string -> type;
    j = this_string -> id;
    j *= (plot -> labels[id].render + 1);
    if (id == 1 && plot -> labels[0].list != NULL)
    {
      j += (plot -> labels[0].render + 1) * (plot -> labels[0].list -> last -> id + 1);
    }
    if (id == 2)
    {
      j += (plot -> xyz -> axis == WIREFRAME) ? 2 : 4;
    }
    else if (id == 3 || id == 4)
    {
      j += measures_drawing;
    }
    if (! plot -> labels[id].render)
    {
      if (ogl_texture == GL_TEXTURE_RECTANGLE_ARB)
      {
        wingl -> ogl_glsl[glsl][shid][j] = init_shader_program (glsl, GLSL_STRING, (this_string -> type == 3) ? string_vertex : (this_string -> type == 4) ? angstrom_vertex : degree_vertex,
                                                                NULL, string_color, GL_TRIANGLE_STRIP, arr_size, (this_string -> type == 3) ? 7 : 8, FALSE, string_to_render);
      }
      else
      {
        wingl -> ogl_glsl[glsl][shid][j] = init_shader_program (glsl, GLSL_STRING, (this_string -> type == 3) ? string_vertex : (this_string -> type == 4) ? angstrom_vertex : degree_vertex,
                                                                NULL, string_color_2d, GL_TRIANGLE_STRIP, arr_size, (this_string -> type == 3) ? 7 : 8, FALSE, string_to_render);
      }
      wingl -> ogl_glsl[glsl][shid][j] -> col = duplicate_color(1, & (this_string -> col));
    }
    else
    {
      if (ogl_texture == GL_TEXTURE_RECTANGLE_ARB)
      {
        string_to_render -> texture = textures_id[0];
        wingl -> ogl_glsl[glsl][shid][j] = init_shader_program (glsl, GLSL_STRING, (this_string -> type == 3) ? string_vertex : (this_string -> type == 4) ? angstrom_vertex : degree_vertex,
                                                                NULL, string_color, GL_TRIANGLE_STRIP, arr_size, (this_string -> type == 3) ? 7 : 8, FALSE, string_to_render);
        wingl -> ogl_glsl[glsl][shid][j] -> col = opposite_color(this_string -> col);
        string_to_render -> texture = textures_id[1];
        wingl -> ogl_glsl[glsl][shid][j+1] = init_shader_program (glsl, GLSL_STRING, (this_string -> type == 3) ? string_vertex : (this_string -> type == 4) ? angstrom_vertex : degree_vertex,
                                                                  NULL, string_color, GL_TRIANGLE_STRIP, arr_size, (this_string -> type == 3) ? 7 : 8, FALSE, string_to_render);
        wingl -> ogl_glsl[glsl][shid][j+1] -> col = duplicate_color(1, & (this_string -> col));
      }
      else
      {
        string_to_render -> texture = textures_id[0];
        wingl -> ogl_glsl[glsl][shid][j] = init_shader_program (glsl, GLSL_STRING, (this_string -> type == 3) ? string_vertex : (this_string -> type == 4) ? angstrom_vertex : degree_vertex,
                                                                NULL, string_color_2d, GL_TRIANGLE_STRIP, arr_size, (this_string -> type == 3) ? 7 : 8, FALSE, string_to_render);
        wingl -> ogl_glsl[glsl][shid][j] -> col = opposite_color(this_string -> col);
        string_to_render -> texture = textures_id[1];
        wingl -> ogl_glsl[glsl][shid][j+1] = init_shader_program (glsl, GLSL_STRING, (this_string -> type == 3) ? string_vertex : (this_string -> type == 4) ? angstrom_vertex : degree_vertex,
                                                                  NULL, string_color_2d, GL_TRIANGLE_STRIP, arr_size, (this_string -> type == 3) ? 7 : 8, FALSE, string_to_render);
        wingl -> ogl_glsl[glsl][shid][j+1] -> col = duplicate_color(1, & (this_string -> col));
      }
    }
    g_free (string_to_render);
  }
  pango_font_description_free (pfont);
  g_object_unref (G_OBJECT(pcontext));
  g_object_unref (G_OBJECT(playout));
}

/*!
  \fn void debug_string (screen_string  * this_string)

  \brief debug screen string data

  \param this_string
*/
void debug_string (screen_string  * this_string)
{
  g_debug ("STRING:: id= %d, text:: %s", this_string -> id, this_string -> word);
  g_debug ("STRING:: color:: r= %f, g= %f, b= %f, a= %f",
           this_string -> col.red, this_string -> col.green, this_string -> col.blue, this_string -> col.alpha);
  int i, j;
  j = 0;
  for (i=0; i<this_string -> num_instances; i++, j+=3)
  {
    g_debug ("STRING:: %d-th :: pos[%d][x]= %f, pos[%d][y]= %f, pos[%d][z]= %f, theta[%d]= %f",
             i, i, this_string -> instances[j], i, this_string -> instances[j+1], i, this_string -> instances[j+2], i,  this_string -> instances[j+2]);
  }
  g_debug ("STRING:: shift:: x= %f, y= %f, z= %f", this_string -> shift[0], this_string -> shift[1], this_string -> shift[2]);
  g_debug ("STRING:: show :: %f",  this_string -> shift[3]);
}

/*!
  \fn void render_all_strings (int glsl, int id)

  \brief render all string to be rendered for a label list

  \param glsl shader id
  \param id label id
*/
void render_all_strings (int glsl, int id)
{
  if (plot -> labels[id].list != NULL)
  {
    screen_string  * this_string = plot -> labels[id].list -> last;
    while (this_string != NULL)
    {
      //if (glsl == MEASU) debug_string (this_string);
      render_string (glsl, id, this_string);
      this_string = this_string -> prev;
    }
  }
}

/*!
  \fn screen_string * was_not_rendered_already (char * word, screen_string * list)

  \brief check if a string was not already rendered and the corresponding screen string created

  \param word the string to render
  \param list the screen string list
*/
screen_string * was_not_rendered_already (char * word, screen_string * list)
{
  if (list != NULL)
  {
    screen_string * tmp_string = list -> last;
    while (tmp_string != NULL)
    {
      if (g_strcmp0 (tmp_string -> word, word) == 0)
      {
        return tmp_string;
      }
      tmp_string = tmp_string -> prev;
    }
    tmp_string = NULL;
    g_free (tmp_string);
  }
  return NULL;
}

/*!
  \fn void add_string_instance (screen_string * string, vec3_t pos, atom * at, atom * bt, atom * ct)

  \brief add an instance to a screen string

  \param string the screen string to increase
  \param pos the position
  \param at the 1st atom, if any (bond or angle measure string)
  \param bt the 2nd atom, if any (bond or angle measure string)
  \param ct the 3rd atom, if any (angle measure string)
*/
void add_string_instance (screen_string * string, vec3_t pos, atom * at, atom * bt, atom * ct)
{
  int i, j;
  i = 0;
  j = (string -> type == 3) ? 1 : (type_of_measure == 6) ? 3 : 4;
  if (string -> num_instances > 0)
  {
    float * instances = duplicate_float (3*j*string -> num_instances, string -> instances);
    g_free (string -> instances);
    string -> instances = allocfloat (3*j*(string -> num_instances+1));
    for (i=0; i<3*j*string -> num_instances; i++)
    {
      string -> instances[i] = instances[i];
    }
  }
  else
  {
    string -> instances = allocfloat (3*j);
  }
  string -> num_instances ++;
  string -> instances[i] = pos.x;
  string -> instances[i+1] = pos.y;
  string -> instances[i+2] = pos.z;
  if (j > 1)
  {
    string -> instances[i+3] = at -> x;
    string -> instances[i+4] = at -> y;
    string -> instances[i+5] = at -> z;
    string -> instances[i+6] = bt -> x;
    string -> instances[i+7] = bt -> y;
    string -> instances[i+8] = bt -> z;
    if (j == 4)
    {
      string -> instances[i+9] = ct -> x;
      string -> instances[i+10] = ct -> y;
      string -> instances[i+11] = ct -> z;
    }
  }
}

/*!
  \fn void add_string (char * text, int id, ColRGBA col, vec3_t pos, float lshift[3], atom * at, atom * bt, atom * ct)

  \brief Add a screen string to the list of screen string to render

  \param text the text to render
  \param id the label id
  \param col the color
  \param pos the position
  \param lshift label position shift on x, y and z, if any
  \param at the 1st atom, if any (bond or angle measure string)
  \param bt the 2nd atom, if any (bond or angle measure string)
  \param ct the 3rd atom, if any (angle measure string)
*/
void add_string (char * text, int id, ColRGBA col, vec3_t pos, float lshift[3], atom * at, atom * bt, atom * ct)
{
  if (plot -> labels[id].list == NULL)
  {
    plot -> labels[id].list = g_malloc0 (sizeof*plot -> labels[id].list);
    plot -> labels[id].list -> last = plot -> labels[id].list;
  }
  else
  {
    screen_string * s_tring = g_malloc0 (sizeof*s_tring);
    s_tring -> prev = plot -> labels[id].list -> last;
    s_tring -> id = plot -> labels[id].list -> last -> id + 1;
    plot -> labels[id].list -> last = s_tring;
  }
  plot -> labels[id].list -> last -> word = g_strdup_printf ("%s", text);
  plot -> labels[id].list -> last -> col = col;
  plot -> labels[id].list -> last -> type = (id < 3) ? 3 : (type_of_measure == 6) ? 4 : 5;
  int i;
  for (i=0; i<3; i++) plot -> labels[id].list -> last -> shift[i] = lshift[i];
  add_string_instance (plot -> labels[id].list -> last, pos, at, bt, ct);
}

/*!
  \fn void prepare_string (char * text, int id, ColRGBA col, vec3_t pos, float lshift[3], atom * at, atom * bt, atom * ct)

  \brief prepare a screen string to be rendered

  \param text the text to render
  \param id the label id
  \param col the color
  \param pos the position
  \param lshift label position shift on x, y and z, if any
  \param at the 1st atom, if any (bond or angle measure string)
  \param bt the 2nd atom, if any (bond or angle measure string)
  \param ct the 3rd atom, if any (angle measure string)
*/
void prepare_string (char * text, int id, ColRGBA col, vec3_t pos, float lshift[3], atom * at, atom * bt, atom * ct)
{
  screen_string * this_string = was_not_rendered_already (text, plot -> labels[id].list);
  if (this_string == NULL)
  {
    add_string (text, id, col, pos, lshift, at, bt, ct);
  }
  else
  {
    add_string_instance (this_string, pos, at, bt, ct);
  }
}
