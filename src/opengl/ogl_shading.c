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
* @file ogl_shading.c
* @short Functions to create GLSL programs \n
         Functions to manage GLSL programs
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'ogl_shading.c'
*
* Contains:
*

 - The functions to create GLSL programs
 - The functions to manage GLSL programs

*
*
* List of functions:

  GLuint create_shader (int type, const GLchar * src);
  GLuint * alloc_shader_pointer (GLuint * pointer, int shaders);
  GLuint * glsl_add_lights (glsl_program * glsl);

  gboolean in_md_shaders (project * this_proj, int id);
  gboolean glsl_disable_cull_face (glsl_program * glsl);

  void set_light_uniform_location (GLuint * lightning, int id, int j, int k, char * string);
  void glsl_bind_points (glsl_program * glsl, object_3d * obj);
  void glsl_bind_spheres (glsl_program * glsl, object_3d * obj);
  void glsl_bind_lines (glsl_program * glsl, object_3d * obj);
  void glsl_bind_cylinders (glsl_program * glsl, object_3d * obj);
  void glsl_bind_caps (glsl_program * glsl, object_3d * obj);
  void glsl_bind_polyhedra (glsl_program * glsl, object_3d * obj);
  void glsl_bind_background (glsl_program * glsl, object_3d * obj);
  void update_string_instances (glsl_program * glsl, object_3d * obj);
  void glsl_bind_string (glsl_program * glsl, object_3d * obj);
  void re_create_all_md_shaders (glwin * view);
  void re_create_md_shaders (int nshaders, int shaders[nshaders], project * this_proj);
  void cleaning_shaders (glwin * view, int shader);
  void recreate_all_shaders (glwin * view);
  void init_default_shaders (glwin * view);
  void init_shaders (glwin * view);
  void set_lights_data (glsl_program * glsl);
  void shading_glsl_text (glsl_program * glsl);
  void render_this_shader (glsl_program * glsl, int ids);
  void draw_vertices (int id);

  glsl_program * init_shader_program (int object, int object_id,
                                      const GLchar * vertex, const GLchar * geometry, const GLchar * fragment,
                                      GLenum type_of_vertices, int narray, int nunif, gboolean lightning, object_3d * obj);

  object_3d * duplicate_object_3d (object_3d * old_obj);

*/

#include "global.h"
#include "interface.h"
#include "glview.h"
#include "ogl_shading.h"

/* Create and compile a shader */
/*!
  \fn GLuint create_shader (int type, const GLchar * src)

  \brief create an OpenGL GLSL shader

  \param type type of the shader
  \param src name of the shader
*/
GLuint create_shader (int type, const GLchar * src)
{
  GLuint shader;
  int status;

  shader = glCreateShader (type);
  glShaderSource (shader, 1, & src, NULL);
  glCompileShader (shader);

  glGetShaderiv (shader, GL_COMPILE_STATUS, & status);
  if (status == GL_FALSE)
  {
    int log_len;
    char *buffer;
    glGetShaderiv (shader, GL_INFO_LOG_LENGTH, & log_len);
    buffer = g_malloc (log_len + 1);
    glGetShaderInfoLog (shader, log_len, NULL, buffer);
    g_warning ("Compile failure in %s shader:\n%s",
               type == GL_VERTEX_SHADER ? "vertex" : "fragment",
               buffer);

    g_free (buffer);
    glDeleteShader (shader);
    return 0;
  }
  return shader;
}

/*!
  \fn GLuint * alloc_shader_pointer (GLuint * pointer, int shaders)

  \brief allocate GLuint data pointer, ensure that it is clean

  \param pointer the data pointer to allocate
  \param shaders the data size to allocate
*/
GLuint * alloc_shader_pointer (GLuint * pointer, int shaders)
{
  if (pointer != NULL)
  {
    g_free (pointer);
    pointer = NULL;
  }
  return allocgluint(shaders);
}

#define LIGHT_INFO     3
#define MATERIAL_DATA  6
#define FOG_DATA       5
#define LIGHT_DATA    10


/*!
  \fn void set_light_uniform_location (GLuint * lightning, int id, int j, int k, char * string)

  \brief set lightning uniform location

  \param lightning
  \param id shader id
  \param j light id
  \param k uniform id
  \param string uniform string
*/
void set_light_uniform_location (GLuint * lightning, int id, int j, int k, char * string)
{
  lightning[LIGHT_INFO+MATERIAL_DATA+FOG_DATA+LIGHT_DATA*j+k] = glGetUniformLocation (id, g_strdup_printf ("AllLights[%d].%s", j, string));
}

/*!
  \fn GLuint * glsl_add_lights (glsl_program * glsl)

  \brief add lightning to an OpenGL shader program

  \param glsl the target glsl
*/
GLuint * glsl_add_lights (glsl_program * glsl)
{
  int tot = MATERIAL_DATA + plot -> l_ghtning.lights * LIGHT_DATA + LIGHT_INFO + FOG_DATA;
  GLuint * lightning = allocgluint(tot);
  lightning[0]  = glGetUniformLocation (glsl -> id, "m_view");
  lightning[1]  = glGetUniformLocation (glsl -> id, "lights_on");
  lightning[2]  = glGetUniformLocation (glsl -> id, "mat.albedo");
  lightning[3]  = glGetUniformLocation (glsl -> id, "mat.metallic");
  lightning[4]  = glGetUniformLocation (glsl -> id, "mat.roughness");
  lightning[5]  = glGetUniformLocation (glsl -> id, "mat.back_light");
  lightning[6]  = glGetUniformLocation (glsl -> id, "mat.gamma");
  lightning[7]  = glGetUniformLocation (glsl -> id, "mat.alpha");
  lightning[8]  = glGetUniformLocation (glsl -> id, "fog.mode");
  lightning[9]  = glGetUniformLocation (glsl -> id, "fog.based");
  lightning[10] = glGetUniformLocation (glsl -> id, "fog.density");
  lightning[11] = glGetUniformLocation (glsl -> id, "fog.depth");
  lightning[12] = glGetUniformLocation (glsl -> id, "fog.color");
  lightning[13] = glGetUniformLocation (glsl -> id, "numLights");
  int j;
  for (j=0; j<plot -> l_ghtning.lights; j++)
  {
    set_light_uniform_location (lightning, glsl -> id, j, 0, "type");
    set_light_uniform_location (lightning, glsl -> id, j, 1, "position");
    set_light_uniform_location (lightning, glsl -> id, j, 2, "direction");
    set_light_uniform_location (lightning, glsl -> id, j, 3, "intensity");
    set_light_uniform_location (lightning, glsl -> id, j, 4, "constant");
    set_light_uniform_location (lightning, glsl -> id, j, 5, "linear");
    set_light_uniform_location (lightning, glsl -> id, j, 6, "quadratic");
    set_light_uniform_location (lightning, glsl -> id, j, 7, "cone_angle");
    set_light_uniform_location (lightning, glsl -> id, j, 8, "spot_inner");
    set_light_uniform_location (lightning, glsl -> id, j, 9, "spot_outer");
  }
  return lightning;
}

/*!
  \fn void glsl_bind_points (glsl_program * glsl, object_3d * obj)

  \brief bind a 3D object point to an OpenGL shader program

  \param glsl the target glsl
  \param obj the 3D object point to bind
*/
void glsl_bind_points (glsl_program * glsl, object_3d * obj)
{
  glsl -> array_pointer[1] = glGetAttribLocation (glsl -> id, "offset");
  glsl -> array_pointer[2] = glGetAttribLocation (glsl -> id, "size");
  glsl -> array_pointer[3] = glGetAttribLocation (glsl -> id, "vertColor");

  // The points vertices
  glBindBuffer(GL_ARRAY_BUFFER, glsl -> vbo[0]);
  glBufferData(GL_ARRAY_BUFFER, obj -> vert_buffer_size * obj -> num_vertices*sizeof(GLfloat), obj -> vertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(glsl -> array_pointer[0]);
  glVertexAttribPointer(glsl -> array_pointer[0], 3, GL_FLOAT, GL_FALSE, obj -> vert_buffer_size*sizeof(GLfloat), (GLvoid*) 0);
  // The instances (pos + col)
  glBindBuffer(GL_ARRAY_BUFFER, glsl -> vbo[1]);
  glBufferData(GL_ARRAY_BUFFER, obj -> inst_buffer_size * obj -> num_instances * sizeof(GLfloat), obj -> instances, GL_STATIC_DRAW);
  glEnableVertexAttribArray (glsl -> array_pointer[1]);
  glVertexAttribPointer (glsl -> array_pointer[1], 3, GL_FLOAT, GL_FALSE, obj -> inst_buffer_size*sizeof(GLfloat), (GLvoid*) 0);
  glVertexAttribDivisor (glsl -> array_pointer[1], 1);
  glEnableVertexAttribArray (glsl -> array_pointer[2]);
  glVertexAttribPointer (glsl -> array_pointer[2], 1, GL_FLOAT, GL_FALSE, obj -> inst_buffer_size*sizeof(GLfloat), (GLvoid*) (3*sizeof(GLfloat)));
  glVertexAttribDivisor (glsl -> array_pointer[2], 1);
  glEnableVertexAttribArray (glsl -> array_pointer[3]);
  glVertexAttribPointer (glsl -> array_pointer[3], 4, GL_FLOAT, GL_FALSE, obj -> inst_buffer_size*sizeof(GLfloat), (GLvoid*) (4*sizeof(GLfloat)));
  glVertexAttribDivisor (glsl -> array_pointer[3], 1);
}

/*!
  \fn void glsl_bind_spheres (glsl_program * glsl, object_3d * obj)

  \brief bind a 3D object sphere to an OpenGL shader program

  \param glsl the target glsl
  \param obj the 3D object sphere to bind
*/
void glsl_bind_spheres (glsl_program * glsl, object_3d * obj)
{
  glsl -> array_pointer[1] = glGetAttribLocation (glsl -> id, "offset");
  glsl -> array_pointer[2] = glGetAttribLocation (glsl -> id, "radius");
  glsl -> array_pointer[3] = glGetAttribLocation (glsl -> id, "vertColor");

  // The sphere vertices
  glBindBuffer(GL_ARRAY_BUFFER, glsl -> vbo[0]);
  glBufferData(GL_ARRAY_BUFFER, obj -> vert_buffer_size * obj -> num_vertices*sizeof(GLfloat), obj -> vertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(glsl -> array_pointer[0]);
  glVertexAttribPointer(glsl -> array_pointer[0], 3, GL_FLOAT, GL_FALSE, obj -> vert_buffer_size*sizeof(GLfloat), (GLvoid*) 0);
  // The sphere indices
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glsl -> vbo[1]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj -> num_indices*sizeof(GLint), obj -> indices, GL_STATIC_DRAW);
  // The instances (pos + col)
  glBindBuffer(GL_ARRAY_BUFFER, glsl -> vbo[2]);
  glBufferData(GL_ARRAY_BUFFER, obj -> inst_buffer_size * obj -> num_instances * sizeof(GLfloat), obj -> instances, GL_STATIC_DRAW);
  glEnableVertexAttribArray (glsl -> array_pointer[1]);
  glVertexAttribPointer (glsl -> array_pointer[1], 3, GL_FLOAT, GL_FALSE, obj -> inst_buffer_size*sizeof(GLfloat), (GLvoid*) 0);
  glVertexAttribDivisor (glsl -> array_pointer[1], 1);
  glEnableVertexAttribArray (glsl -> array_pointer[2]);
  glVertexAttribPointer (glsl -> array_pointer[2], 1, GL_FLOAT, GL_FALSE, obj -> inst_buffer_size*sizeof(GLfloat), (GLvoid*) (3*sizeof(GLfloat)));
  glVertexAttribDivisor (glsl -> array_pointer[2], 1);
  glEnableVertexAttribArray (glsl -> array_pointer[3]);
  glVertexAttribPointer (glsl -> array_pointer[3], 4, GL_FLOAT, GL_FALSE, obj -> inst_buffer_size*sizeof(GLfloat), (GLvoid*) (4*sizeof(GLfloat)));
  glVertexAttribDivisor (glsl -> array_pointer[3], 1);
}

/*!
  \fn void glsl_bind_lines (glsl_program * glsl, object_3d * obj)

  \brief bind a 3D object line to an OpenGL shader program

  \param glsl the target glsl
  \param obj the 3D object line to bind
*/
void glsl_bind_lines (glsl_program * glsl, object_3d * obj)
{
  glsl -> array_pointer[1] = glGetAttribLocation (glsl -> id, "vertColor");
 // The line vertices
  glBindBuffer(GL_ARRAY_BUFFER, glsl -> vbo[0]);
  glBufferData(GL_ARRAY_BUFFER, obj -> vert_buffer_size * obj -> num_vertices*sizeof(GLfloat), obj -> vertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(glsl -> array_pointer[0]);
  glVertexAttribPointer(glsl -> array_pointer[0], 3, GL_FLOAT, GL_FALSE, obj -> vert_buffer_size *sizeof(GLfloat), (GLvoid*) 0);
  glEnableVertexAttribArray(glsl -> array_pointer[1]);
  glVertexAttribPointer(glsl -> array_pointer[1], 4, GL_FLOAT, GL_FALSE, obj -> vert_buffer_size *sizeof(GLfloat), (GLvoid*) (3*sizeof(GLfloat)));

  if (glsl -> object == MEASU)
  {
    glsl -> uniform_loc[1] = glGetUniformLocation (glsl -> id, "factor");
    glsl -> uniform_loc[2] = glGetUniformLocation (glsl -> id, "pattern");
    glsl -> uniform_loc[3] = glGetUniformLocation (glsl -> id, "text_proj");
    glsl -> uniform_loc[4] = glGetUniformLocation (glsl -> id, "un_view");
    glsl -> uniform_loc[5] = glGetUniformLocation (glsl -> id, "viewp");
    glsl -> uniform_loc[6] =  glGetUniformLocation (glsl -> id, "depth");
  }
}

/*!
  \fn void glsl_bind_cylinders (glsl_program * glsl, object_3d * obj)

  \brief bind a 3D object cylinder to an OpenGL shader program

  \param glsl the target glsl
  \param obj the 3D object, cylinder to bind
*/
void glsl_bind_cylinders (glsl_program * glsl, object_3d * obj)
{
  glsl -> array_pointer[1] = glGetAttribLocation (glsl -> id, "offset");
  glsl -> array_pointer[2] = glGetAttribLocation (glsl -> id, "height");
  glsl -> array_pointer[3] = glGetAttribLocation (glsl -> id, "radius");
  glsl -> array_pointer[4] = glGetAttribLocation (glsl -> id, "quat");
  glsl -> array_pointer[5] = glGetAttribLocation (glsl -> id, "vertColor");

  // The cylinder vertices
  glBindBuffer(GL_ARRAY_BUFFER, glsl -> vbo[0]);
  glBufferData(GL_ARRAY_BUFFER, obj -> vert_buffer_size * obj -> num_vertices*sizeof(GLfloat), obj -> vertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(glsl -> array_pointer[0]);
  glVertexAttribPointer(glsl -> array_pointer[0], 3, GL_FLOAT, GL_FALSE, obj -> vert_buffer_size*sizeof(GLfloat), (GLvoid*) 0);

  // The cylinder indices
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glsl -> vbo[1]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj -> num_indices*sizeof(GLint), obj -> indices, GL_STATIC_DRAW);

  // The instances (pos + lenght, + rad + rot + col)
  glBindBuffer(GL_ARRAY_BUFFER, glsl -> vbo[2]);
  glBufferData(GL_ARRAY_BUFFER, obj -> inst_buffer_size * obj -> num_instances * sizeof(GLfloat), obj -> instances, GL_STATIC_DRAW);
  glEnableVertexAttribArray (glsl -> array_pointer[1]);
  glVertexAttribPointer (glsl -> array_pointer[1], 3, GL_FLOAT, GL_FALSE, obj -> inst_buffer_size*sizeof(GLfloat), (GLvoid*) 0);
  glVertexAttribDivisor (glsl -> array_pointer[1], 1);
  glEnableVertexAttribArray (glsl -> array_pointer[2]);
  glVertexAttribPointer (glsl -> array_pointer[2], 1, GL_FLOAT, GL_FALSE, obj -> inst_buffer_size*sizeof(GLfloat), (GLvoid*) (3*sizeof(GLfloat)));
  glVertexAttribDivisor (glsl -> array_pointer[2], 1);
  glEnableVertexAttribArray (glsl -> array_pointer[3]);
  glVertexAttribPointer (glsl -> array_pointer[3], 1, GL_FLOAT, GL_FALSE, obj -> inst_buffer_size*sizeof(GLfloat), (GLvoid*) (4*sizeof(GLfloat)));
  glVertexAttribDivisor (glsl -> array_pointer[3], 1);
  glEnableVertexAttribArray (glsl -> array_pointer[4]);
  glVertexAttribPointer (glsl -> array_pointer[4], 4, GL_FLOAT, GL_FALSE, obj -> inst_buffer_size*sizeof(GLfloat), (GLvoid*) (5*sizeof(GLfloat)));
  glVertexAttribDivisor (glsl -> array_pointer[4], 1);
  glEnableVertexAttribArray (glsl -> array_pointer[5]);
  glVertexAttribPointer (glsl -> array_pointer[5], 4, GL_FLOAT, GL_FALSE, obj -> inst_buffer_size*sizeof(GLfloat), (GLvoid*) (9*sizeof(GLfloat)));
  glVertexAttribDivisor (glsl -> array_pointer[5], 1);
}

/*!
  \fn void glsl_bind_caps (glsl_program * glsl, object_3d * obj)

  \brief bind a 3D object cylinder cap to an OpenGL shader program

  \param glsl the target glsl
  \param obj the 3D object, cylinder cap to bind
*/
void glsl_bind_caps (glsl_program * glsl, object_3d * obj)
{
  glsl -> array_pointer[1] = glGetAttribLocation (glsl -> id, "offset");
  glsl -> array_pointer[2] = glGetAttribLocation (glsl -> id, "radius");
  glsl -> array_pointer[3] = glGetAttribLocation (glsl -> id, "quat");
  glsl -> array_pointer[4] = glGetAttribLocation (glsl -> id, "vertColor");

  // The cylinder vertices
  glBindBuffer(GL_ARRAY_BUFFER, glsl -> vbo[0]);
  glBufferData(GL_ARRAY_BUFFER, obj -> vert_buffer_size * obj -> num_vertices*sizeof(GLfloat), obj -> vertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(glsl -> array_pointer[0]);
  glVertexAttribPointer(glsl -> array_pointer[0], 3, GL_FLOAT, GL_FALSE, obj -> vert_buffer_size*sizeof(GLfloat), (GLvoid*) 0);

  // The cylinder indices
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glsl -> vbo[1]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj -> num_indices*sizeof(GLint), obj -> indices, GL_STATIC_DRAW);

  // The instances (pos + length, + rad + rot + col)
  glBindBuffer(GL_ARRAY_BUFFER, glsl -> vbo[2]);
  glBufferData(GL_ARRAY_BUFFER, obj -> inst_buffer_size * obj -> num_instances * sizeof(GLfloat), obj -> instances, GL_STATIC_DRAW);
  glEnableVertexAttribArray (glsl -> array_pointer[1]);
  glVertexAttribPointer (glsl -> array_pointer[1], 3, GL_FLOAT, GL_FALSE, obj -> inst_buffer_size*sizeof(GLfloat), (GLvoid*) 0);
  glVertexAttribDivisor (glsl -> array_pointer[1], 1);
  glEnableVertexAttribArray (glsl -> array_pointer[2]);
  glVertexAttribPointer (glsl -> array_pointer[2], 1, GL_FLOAT, GL_FALSE, obj -> inst_buffer_size*sizeof(GLfloat), (GLvoid*) (3*sizeof(GLfloat)));
  glVertexAttribDivisor (glsl -> array_pointer[2], 1);
  glEnableVertexAttribArray (glsl -> array_pointer[3]);
  glVertexAttribPointer (glsl -> array_pointer[3], 4, GL_FLOAT, GL_FALSE, obj -> inst_buffer_size*sizeof(GLfloat), (GLvoid*) (4*sizeof(GLfloat)));
  glVertexAttribDivisor (glsl -> array_pointer[3], 1);
  glEnableVertexAttribArray (glsl -> array_pointer[4]);
  glVertexAttribPointer (glsl -> array_pointer[4], 4, GL_FLOAT, GL_FALSE, obj -> inst_buffer_size*sizeof(GLfloat), (GLvoid*) (8*sizeof(GLfloat)));
  glVertexAttribDivisor (glsl -> array_pointer[4], 1);
}

/*!
  \fn void glsl_bind_polyhedra (glsl_program * glsl, object_3d * obj)

  \brief bind a 3D object polyhedra to an OpenGL shader program

  \param glsl the target glsl
  \param obj the 3D object polyhedra to bind
*/
void glsl_bind_polyhedra (glsl_program * glsl, object_3d * obj)
{
  glsl -> array_pointer[1] = glGetAttribLocation (glsl -> id, "vertNormal");
  glsl -> array_pointer[2] = glGetAttribLocation (glsl -> id, "vertColor");

  // The polyhedra vertices (triangles + normal orientation)
  glBindBuffer(GL_ARRAY_BUFFER, glsl -> vbo[0]);
  glBufferData(GL_ARRAY_BUFFER, obj -> vert_buffer_size * obj -> num_vertices*sizeof(GLfloat), obj -> vertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(glsl -> array_pointer[0]);
  glVertexAttribPointer(glsl -> array_pointer[0], 3, GL_FLOAT, GL_FALSE, obj -> vert_buffer_size*sizeof(GLfloat), (GLvoid*) 0);
  glEnableVertexAttribArray(glsl -> array_pointer[1]);
  glVertexAttribPointer(glsl -> array_pointer[1], 3, GL_FLOAT, GL_FALSE, obj -> vert_buffer_size*sizeof(GLfloat), (GLvoid*) (3*sizeof(GLfloat)));
  glEnableVertexAttribArray(glsl -> array_pointer[2]);
  glVertexAttribPointer(glsl -> array_pointer[2], 4, GL_FLOAT, GL_FALSE, obj -> vert_buffer_size*sizeof(GLfloat), (GLvoid*) (6*sizeof(GLfloat)));
}

/*!
  \fn void glsl_bind_background (glsl_program * glsl, object_3d * obj)

  \brief bind background data to an OpenGL shader program

  \param glsl the target glsl
  \param obj the 3D object to bind
*/
void glsl_bind_background (glsl_program * glsl, object_3d * obj)
{

  glsl -> uniform_loc[0] = glGetUniformLocation (glsl -> id, "first_color");
  glsl -> uniform_loc[1] = glGetUniformLocation (glsl -> id, "second_color");
  glsl -> uniform_loc[2] = glGetUniformLocation (glsl -> id, "gradient");
  glsl -> uniform_loc[3] = glGetUniformLocation (glsl -> id, "position");

  glBindBuffer(GL_ARRAY_BUFFER, glsl -> vbo[0]);
  glBufferData(GL_ARRAY_BUFFER, obj -> vert_buffer_size * obj -> num_vertices*sizeof(GLfloat), obj -> vertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(glsl -> array_pointer[0]);
  glVertexAttribPointer(glsl -> array_pointer[0], 2, GL_FLOAT, GL_FALSE, obj -> vert_buffer_size*sizeof(GLfloat), (GLvoid*) 0);
}

/*!
  \fn void update_string_instances (glsl_program * glsl, object_3d * obj)

  \brief Update OpenGL string texture instances

  \param glsl the target glsl
  \param obj
*/
void update_string_instances (glsl_program * glsl, object_3d * obj)
{
  // The instances (pos)
  glBindBuffer(GL_ARRAY_BUFFER, glsl -> vbo[1]);
  glBufferData(GL_ARRAY_BUFFER, obj -> inst_buffer_size * obj -> num_instances * sizeof(GLfloat), obj -> instances, GL_STATIC_DRAW);
  glEnableVertexAttribArray (glsl -> array_pointer[2]);

  glVertexAttribPointer (glsl -> array_pointer[2], 3, GL_FLOAT, GL_FALSE, obj -> inst_buffer_size*sizeof(GLfloat), (GLvoid*) 0);
  glVertexAttribDivisor (glsl -> array_pointer[2], 1);
  if (glsl -> object == MEASU)
  {
    glEnableVertexAttribArray (glsl -> array_pointer[3]);
    glVertexAttribPointer (glsl -> array_pointer[3], 3, GL_FLOAT, GL_FALSE, obj -> inst_buffer_size*sizeof(GLfloat), (GLvoid*) (3*sizeof(GLfloat)));
    glVertexAttribDivisor (glsl -> array_pointer[3], 1);
    glEnableVertexAttribArray (glsl -> array_pointer[4]);
    glVertexAttribPointer (glsl -> array_pointer[4], 3, GL_FLOAT, GL_FALSE, obj -> inst_buffer_size*sizeof(GLfloat), (GLvoid*) (6*sizeof(GLfloat)));
    glVertexAttribDivisor (glsl -> array_pointer[4], 1);
    if (obj -> inst_buffer_size == 12)
    {
      glEnableVertexAttribArray (glsl -> array_pointer[5]);
      glVertexAttribPointer (glsl -> array_pointer[5], 3, GL_FLOAT, GL_FALSE, obj -> inst_buffer_size*sizeof(GLfloat), (GLvoid*) (9*sizeof(GLfloat)));
      glVertexAttribDivisor (glsl -> array_pointer[5], 1);
    }
  }
}

/*!
  \fn void glsl_bind_string (glsl_program * glsl, object_3d * obj)

  \brief bind a 3D object text string to an OpenGL shader program

  \param glsl the target glsl
  \param obj the 3D object text string to bind
*/
void glsl_bind_string (glsl_program * glsl, object_3d * obj)
{
  glActiveTexture (GL_TEXTURE0);
  glBindTexture (ogl_texture, obj -> texture);
  glsl -> uniform_loc[1] = glGetUniformLocation (glsl -> id, "tex");
  glUniform1i (glsl -> uniform_loc[1], 0);
  glsl -> uniform_loc[2] = glGetUniformLocation (glsl -> id, "text_proj");
  glsl -> uniform_loc[3] = glGetUniformLocation (glsl -> id, "un_view");
  glsl -> uniform_loc[4] = glGetUniformLocation (glsl -> id, "viewp");
  glsl -> uniform_loc[5] = glGetUniformLocation (glsl -> id, "pos_shift");
  glsl -> uniform_loc[6] = glGetUniformLocation (glsl -> id, "vert_color");

  glsl -> array_pointer[1] = glGetAttribLocation (glsl -> id, "tcoord");
  glsl -> array_pointer[2] = glGetAttribLocation (glsl -> id, "offset");
  if (glsl -> object == MEASU)
  {
    glsl -> uniform_loc[7] =  glGetUniformLocation (glsl -> id, "tilted");
    glsl -> array_pointer[3] = glGetAttribLocation (glsl -> id, "at_a");
    glsl -> array_pointer[4] = glGetAttribLocation (glsl -> id, "at_b");
    if (obj -> inst_buffer_size == 12) glsl -> array_pointer[5] = glGetAttribLocation (glsl -> id, "at_c");
  }

  // The string (rendered using triangles and textures + colors)
  glBindBuffer(GL_ARRAY_BUFFER, glsl -> vbo[0]);
  glBufferData(GL_ARRAY_BUFFER, obj -> vert_buffer_size * obj -> num_vertices*sizeof(GLfloat), obj -> vertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(glsl -> array_pointer[0]);
  glVertexAttribPointer(glsl -> array_pointer[0], 2, GL_FLOAT, GL_FALSE, obj -> vert_buffer_size*sizeof(GLfloat), (GLvoid*) 0);
  glEnableVertexAttribArray(glsl -> array_pointer[1]);
  glVertexAttribPointer(glsl -> array_pointer[1], 2, GL_FLOAT, GL_FALSE, obj -> vert_buffer_size*sizeof(GLfloat), (GLvoid*) (2*sizeof(GLfloat)));
  update_string_instances (glsl, obj);
}

/*!
  \fn object_3d * duplicate_object_3d (object_3d * old_obj)

  \brief create a copy of an object_3d data structure

  \param old_obj the data structure to copy
*/
object_3d * duplicate_object_3d (object_3d * old_obj)
{
  object_3d * new_obj = g_malloc0 (sizeof*new_obj);
  new_obj -> quality = old_obj -> quality;
  // Vertices
  new_obj -> num_vertices = old_obj -> num_vertices;
  new_obj -> vert_buffer_size = old_obj -> vert_buffer_size;
  new_obj -> vertices = duplicate_float (new_obj -> num_vertices*old_obj -> vert_buffer_size, old_obj -> vertices);
  // Indices
  new_obj -> num_indices = old_obj -> num_indices;
  new_obj -> ind_buffer_size = old_obj -> ind_buffer_size;
  if (old_obj -> indices != NULL)
  {
    new_obj -> indices = duplicate_int (new_obj -> num_indices*old_obj -> ind_buffer_size, old_obj -> indices);
  }
  // Instances
  new_obj -> num_instances = old_obj -> num_instances;
  new_obj -> inst_buffer_size = old_obj -> inst_buffer_size;
  if (old_obj -> instances != NULL)
  {
    new_obj -> instances = duplicate_float (new_obj -> num_instances*old_obj -> inst_buffer_size, old_obj -> instances);
  }
  new_obj -> texture = old_obj -> texture;
  int i;
  for (i=0; i<5; i++) new_obj -> shift[i] = old_obj -> shift[i];
  return new_obj;
}

/*!
  \fn glsl_program * init_shader_program (int object, int object_id,
                                          const GLchar * vertex, const GLchar * geometry, const GLchar * fragment,
                                          GLenum type_of_vertices, int narray, int nunif, gboolean lightning, object_3d * obj)

  \brief create an OpenGL shader program

  \param object shader id (in enum shaders)
  \param object_id shader type in: GLSL_SPHERES, GLSL_POINTS, GLSL_LINES, GLSL_CYLINDERS, GLSL_CAPS, GLSL_POLYEDRA, GLSL_STRING, GLSL_BACK
  \param vertex general shader: in the shaders defined in 'ogl_shaders.c'
  \param geometry geometry shader, if any: in the shaders defined in 'ogl_shaders.c'
  \param fragment color shader, if any: in the shaders defined in 'ogl_shaders.c'
  \param type_of_vertices type of vertices in: GL_POINTS, GL_LINES, GL_TRIANGLE GL_TRIANGLE_FAN, GL_TRIANGLE_STRIP
  \param narray number of vertices to allocate
  \param nunif number of uniform location(s) to allocate
  \param lightning use lightning (1/0)
  \param obj the object 3D to bind to the shader
*/
glsl_program * init_shader_program (int object, int object_id,
                                    const GLchar * vertex, const GLchar * geometry, const GLchar * fragment,
                                    GLenum type_of_vertices, int narray, int nunif, gboolean lightning, object_3d * obj)
{
  glsl_program * glsl = g_malloc0 (sizeof * glsl);

  glsl -> id = glCreateProgram ();
  glsl -> object = object;
  glsl -> draw_type = object_id;

  glsl -> vertex_shader = create_shader (GL_VERTEX_SHADER, vertex);
  glAttachShader (glsl -> id, glsl -> vertex_shader);
  if (geometry != NULL)
  {
    glsl -> geometry_shader = create_shader (GL_GEOMETRY_SHADER, geometry);
    glAttachShader (glsl -> id, glsl -> geometry_shader);
  }
  glsl -> fragment_shader = create_shader (GL_FRAGMENT_SHADER, fragment);
  glAttachShader (glsl -> id, glsl -> fragment_shader);

  glBindFragDataLocation (glsl -> id, 0, "fragment_color");
  glLinkProgram(glsl -> id);

  glsl -> vert_type = type_of_vertices;

  glGenVertexArrays (1, & glsl -> vao);

  glBindVertexArray (glsl -> vao);

  glsl -> array_pointer = alloc_shader_pointer (glsl -> array_pointer, narray);
  glsl -> uniform_loc = alloc_shader_pointer (glsl -> uniform_loc, nunif);

  // For other than background always the MVP matrix as uniform 0
  if (object_id != GLSL_BACK) glsl -> uniform_loc[0] = glGetUniformLocation (glsl -> id, "mvp");
  // and always the vertices as array 0
  glsl -> array_pointer[0] = glGetAttribLocation (glsl -> id, "vert");

  glsl -> light_uniform = NULL;
  if (lightning) glsl -> light_uniform = glsl_add_lights (glsl);

  glsl -> obj = duplicate_object_3d (obj);

  glsl -> draw_instanced = FALSE;

  int nvbo = 1;
  if (glsl -> obj -> num_indices > 0) nvbo ++;
  if (glsl -> obj -> num_instances > 0)
  {
    nvbo ++;
    if (glsl -> obj -> num_instances > 1) glsl -> draw_instanced = TRUE;
  }

  glsl -> vbo = allocgluint (nvbo);
  glGenBuffers (nvbo, glsl -> vbo);

  switch (object_id)
  {
    case GLSL_SPHERES:
      // narray = 4
      glsl_bind_spheres (glsl, glsl -> obj);
      break;
    case GLSL_POINTS:
      // narray = 4
      glsl_bind_points (glsl, glsl -> obj);
      break;
    case GLSL_LINES:
      // narray = 3
      glsl_bind_lines (glsl, glsl -> obj);
      break;
    case GLSL_CYLINDERS:
      // narray = 6
      glsl_bind_cylinders (glsl, glsl -> obj);
      break;
    case GLSL_CAPS:
      // narray = 5
      glsl_bind_caps (glsl, glsl -> obj);
      break;
    case GLSL_POLYEDRA:
      glsl_bind_polyhedra (glsl, glsl -> obj);
      break;
    case GLSL_STRING:
      glsl_bind_string (glsl, glsl -> obj);
      break;
    case GLSL_BACK:
      glsl_bind_background (glsl, glsl -> obj);
      break;
  }
  glDetachShader (glsl -> id,glsl -> vertex_shader);
  if (geometry != NULL) glDetachShader (glsl -> id, glsl -> geometry_shader);
  glDetachShader (glsl -> id, glsl -> fragment_shader);

  return glsl;
}

/*!
  \fn gboolean in_md_shaders (project * this_proj, int id)

  \brief is this shader MD dependent ?

  \param this_proj the target project
  \param id the shader id
*/
gboolean in_md_shaders (project * this_proj, int id)
{
  if (id == ATOMS) return TRUE;
  if (id == BONDS) return TRUE;
  if (id == POLYS) return TRUE;
  if (id == RINGS) return TRUE;
  if (id == SELEC) return TRUE;
  if (id == MDBOX) return this_proj -> cell.npt;
  if (id == VOLMS) return TRUE;
  return FALSE;
}

/*!
  \fn void re_create_all_md_shaders (glwin * view)

  \brief re-initialize all MD dependent OpenGL shaders

  \param view the target glwin
*/
void re_create_all_md_shaders (glwin * view)
{
  int i, j;
  project * this_proj = get_project_by_id(view -> proj);
  for (i=0; i<NGLOBAL_SHADERS; i++)
  {
    if (in_md_shaders (this_proj, i))
    {
      for (j=0; j< this_proj -> steps; j++) view -> n_shaders[i][j] = -1;
    }
  }
}

/*!
  \fn void re_create_md_shaders (int nshaders, int shaders[nshaders], project * this_proj)

  \brief re-initialize some MD dependent OpenGL shaders

  \param nshaders the number of shader(s) to initialize
  \param shaders the list of shaders
  \param this_proj the target project
*/
void re_create_md_shaders (int nshaders, int shaders[nshaders], project * this_proj)
{
  int i, j, k;
  for (i=0; i<nshaders; i++)
  {
    k = shaders[i];
    this_proj -> modelgl -> create_shaders[k] = TRUE;
    for (j=0; j<this_proj -> steps; j++)
    {
      this_proj -> modelgl -> n_shaders[k][j] = -1;
    }
  }
}

/*!
  \fn void cleaning_shaders (glwin * view, int shader)

  \brief re-initialize an OpenGL shader

  \param view the target glwin
  \param shader the shader to initialize
*/
void cleaning_shaders (glwin * view, int shader)
{
  int i = (in_md_shaders(get_project_by_id(view -> proj), shader)) ? step : 0;
  if (view -> ogl_glsl[shader][i] != NULL)
  {
    g_free (view -> ogl_glsl[shader][i]);
    view -> ogl_glsl[shader][i] = NULL;
  }
  view -> n_shaders[shader][i] = (in_md_shaders(get_project_by_id(view -> proj), shader)) ? -1 : 0;
}

/*!
  \fn void recreate_all_shaders (glwin * view)

  \brief re-initialize all OpenGL shaders

  \param view the target glwin
*/
void recreate_all_shaders (glwin * view)
{
  int i;
  for (i=0; i<NGLOBAL_SHADERS; i++)
  {
    view -> create_shaders[i] = TRUE;
  }
}

/*!
  \fn void init_default_shaders (glwin * view)

  \brief re-initialize the default OpenGL shaders

  \param view the target glwin
*/
void init_default_shaders (glwin * view)
{
  int shaders[6] = {ATOMS, BONDS, SELEC, POLYS, RINGS, VOLMS};
  re_create_md_shaders (6, shaders, get_project_by_id (view -> proj));
  view -> create_shaders[LABEL] = TRUE;
  view -> create_shaders[PICKS] = TRUE;
  view -> create_shaders[MEASU] = TRUE;
  update (view);
  int i;
  if (is_atom_win_active(view) || (view -> mode == EDITION && (view -> anim -> last -> img -> selected[1] -> selected || view -> selection_mode == 5)))
  {
    if (view -> measure_win != NULL)
    {
      for (i=0; i<3; i++)
      {
        if (view -> measure_win -> selection_tree[i] != NULL) update_selection_tree (view, 1, i);
      }
    }
  }
}

/*!
  \fn void init_shaders (glwin * view)

  \brief initialize all the OpenGL shaders

  \param view the target glwin
*/
void init_shaders (glwin * view)
{
  int i, j;
  project * this_proj = get_project_by_id (view -> proj);
  for (i=0; i<NGLOBAL_SHADERS; i++)
  {
    view -> ogl_glsl[i] = NULL;
    if (in_md_shaders (this_proj, i))
    {
      view -> ogl_glsl[i] = g_malloc0 (this_proj -> steps*sizeof*view -> ogl_glsl[i]);
      view -> n_shaders[i] = allocint (this_proj -> steps);
      for (j=0; j<this_proj -> steps; j++)
      {
        view -> n_shaders[i][j] = -1;
      }
    }
    else
    {
      j = (i == MEASU) ? 2 : 1;
      view -> ogl_glsl[i] = g_malloc0 (j*sizeof*view -> ogl_glsl[i]);
      view -> ogl_glsl[i][0] = NULL;
      view -> n_shaders[i] = allocint (j);
    }
    view -> create_shaders[i] = TRUE;
  }
}

/*!
  \fn gboolean glsl_disable_cull_face (glsl_program * glsl)

  \brief Disable or enable cull face for OpenGL rendering

  \param glsl the target glsl
*/
gboolean glsl_disable_cull_face (glsl_program * glsl)
{
  if (glsl -> draw_type == GLSL_POINTS)
  {
    return FALSE;
  }
  else if (glsl -> draw_type == GLSL_LINES)
  {
    return FALSE;
  }
  else if (glsl -> draw_type == GLSL_LIGHT)
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}

/*!
  \fn void set_lights_data (glsl_program * glsl)

  \brief set lightning data for an OpenGL progam

  \param glsl the target glsl
*/
void set_lights_data (glsl_program * glsl)
{
  int j, k;
  vec3_t l_pos, l_dir;
  k = (glsl -> draw_type == GLSL_LIGHT) ? 0 : plot -> m_terial.param[0];
  glUniformMatrix4fv (glsl -> light_uniform[0], 1, GL_FALSE, & wingl -> model_view_matrix.m00);
  glUniform1i (glsl -> light_uniform[1], k);
  glUniform3f (glsl -> light_uniform[2], plot -> m_terial.albedo.x, plot -> m_terial.albedo.y, plot -> m_terial.albedo.z);
  for (j=0; j<5; j++) glUniform1f (glsl -> light_uniform[3+j], plot -> m_terial.param[j+1]);
  glUniform1i (glsl -> light_uniform[8], plot -> f_g.mode);
  glUniform1i (glsl -> light_uniform[9], plot -> f_g.based);
  glUniform1f (glsl -> light_uniform[10], plot -> f_g.density/plot -> p_depth);
  glUniform2f (glsl -> light_uniform[11], plot -> f_g.depth[0]*plot -> p_depth/100.0 + plot -> p_depth, plot -> f_g.depth[1]*plot -> p_depth/100.0+ plot -> p_depth);
  glUniform3f (glsl -> light_uniform[12], plot -> f_g.color.x, plot -> f_g.color.y, plot -> f_g.color.z);
  glUniform1i (glsl -> light_uniform[13], plot -> l_ghtning.lights);
  for (j=0; j<plot -> l_ghtning.lights; j++)
  {
    k = j*LIGHT_DATA + LIGHT_INFO + MATERIAL_DATA + FOG_DATA;
    glUniform1i (glsl -> light_uniform[k], plot -> l_ghtning.spot[j].type);
    if (plot -> l_ghtning.spot[j].fix == 0)
    {
      l_pos = m4_mul_pos (wingl -> model_matrix, plot -> l_ghtning.spot[j].position);
    }
    else
    {
      l_pos = m4_mul_pos (wingl -> model_view_matrix, plot -> l_ghtning.spot[j].position);
    }
    glUniform3f (glsl -> light_uniform[k+1], l_pos.x, l_pos.y, l_pos.z);
    if (plot -> l_ghtning.spot[j].fix == 0)
    {
      l_dir = m4_mul_pos (wingl -> model_matrix, plot -> l_ghtning.spot[j].direction);
    }
    else
    {
      l_dir = m4_mul_pos (wingl -> model_view_matrix, plot -> l_ghtning.spot[j].direction);
    }
    glUniform3f (glsl -> light_uniform[k+2], l_dir.x, l_dir.y, l_dir.z);
    glUniform3f (glsl -> light_uniform[k+3], plot -> l_ghtning.spot[j].intensity.x, plot -> l_ghtning.spot[j].intensity.y, plot -> l_ghtning.spot[j].intensity.z);
    glUniform1f (glsl -> light_uniform[k+4], plot -> l_ghtning.spot[j].attenuation.x);
    glUniform1f (glsl -> light_uniform[k+5], plot -> l_ghtning.spot[j].attenuation.y);
    glUniform1f (glsl -> light_uniform[k+6], plot -> l_ghtning.spot[j].attenuation.z);
    glUniform1f (glsl -> light_uniform[k+7], cos(plot -> l_ghtning.spot[j].spot_data.x*pi/180.0));
    glUniform1f (glsl -> light_uniform[k+8], cos(plot -> l_ghtning.spot[j].spot_data.y*pi/180.0));
    glUniform1f (glsl -> light_uniform[k+9], cos(plot -> l_ghtning.spot[j].spot_data.z*pi/180.0));
  }
}

uint16_t stipple_pattern[NDOTS]={ 0xAAAA, 0x1111, 0x0000, 0x55FF, 0x24FF, 0x3F3F, 0x33FF, 0x27FF};
int this_tilt;
int this_pattern;
int this_factor;

/*!
  \fn void shading_glsl_text (glsl_program * glsl)

  \brief Render text in OpenGL

  \param glsl the target glsl
*/
void shading_glsl_text (glsl_program * glsl)
{
  wingl -> label_projection_matrix = create_label_matrices ();
  glUniformMatrix4fv (glsl -> uniform_loc[2], 1, GL_FALSE, & wingl -> label_projection_matrix.m00);
  glUniformMatrix4fv (glsl -> uniform_loc[3], 1, GL_FALSE, & wingl -> un_view_matrix.m00);
  glUniform4f (glsl -> uniform_loc[4], wingl -> view_port.w, wingl -> view_port.x, wingl -> view_port.y, wingl -> view_port.z);
  glUniform4f (glsl -> uniform_loc[5], glsl -> obj -> shift[0], glsl -> obj -> shift[1],
                                       glsl -> obj -> shift[2], glsl -> obj -> shift[3]);
  glUniform4f (glsl -> uniform_loc[6], glsl -> col -> red, glsl -> col -> green,
                                       glsl -> col -> blue, glsl -> col -> alpha);
  if (glsl -> object == MEASU) glUniform1i (glsl -> uniform_loc[7], this_tilt);
}

/*!
  \fn void render_this_shader (glsl_program * glsl, int ids)

  \brief render an OpenGL shader

  \param glsl the target glsl
  \param ids
*/
void render_this_shader (glsl_program * glsl, int ids)
{
  int j, k;
  if (glsl -> object == MAXIS)
  {
    j = 0;
    if (is_atom_win_active(wingl))
    {
      k = wingl -> atom_win -> active;
      if (wingl -> atom_win -> show_axis[k])
      {
        j = wingl -> atom_win -> axis[k];
      }
    }
    wingl -> axis_proj_model_view_matrix = create_axis_matrices (j);
    glUniformMatrix4fv (glsl -> uniform_loc[0], 1, GL_FALSE, & wingl -> axis_proj_model_view_matrix.m00);
    j = (plot -> xyz -> axis == WIREFRAME) ? 1 : 3;
    if (ids > j) shading_glsl_text (glsl);
  }
  else if (glsl -> object == LABEL)
  {
    glUniformMatrix4fv (glsl -> uniform_loc[0], 1, GL_FALSE, & wingl -> proj_model_view_matrix.m00);
    shading_glsl_text (glsl);
  }
  else if (glsl -> object == BACKG)
  {
    for (j=0; j<2; j++)
    {
      glUniform4f (glsl -> uniform_loc[j], plot -> back -> gradient_color[j].red,
                                           plot -> back -> gradient_color[j].green,
                                           plot -> back -> gradient_color[j].blue,
                                           plot -> back -> gradient_color[j].alpha);
    }
    glUniform1i (glsl -> uniform_loc[2], plot -> back -> direction);
    glUniform1f (glsl -> uniform_loc[3], plot -> back -> position);
  }
  else if (glsl -> object == MEASU)
  {
    glUniformMatrix4fv (glsl -> uniform_loc[0], 1, GL_FALSE, & wingl -> proj_model_view_matrix.m00);
    if (glsl -> vert_type == GL_TRIANGLE_STRIP)
    {
      shading_glsl_text (glsl);
    }
    else
    {
      glUniform1i (glsl -> uniform_loc[1], this_factor);
      glUniform1ui (glsl -> uniform_loc[2], stipple_pattern[this_pattern]);
      if (glsl -> vert_type == GL_TRIANGLES)
      {
        wingl -> label_projection_matrix = create_label_matrices ();
        glUniformMatrix4fv (glsl -> uniform_loc[3], 1, GL_FALSE, & wingl -> label_projection_matrix.m00);
      }
      glUniformMatrix4fv (glsl -> uniform_loc[4], 1, GL_FALSE, & wingl -> un_view_matrix.m00);
      glUniform4f (glsl -> uniform_loc[5], wingl -> view_port.w, wingl -> view_port.x, wingl -> view_port.y, wingl -> view_port.z);
      if (plot -> rep == PERSPECTIVE)
      {
        glUniform1f (glsl -> uniform_loc[6], 1.0);
      }
      else
      {
        glUniform1f (glsl -> uniform_loc[6], plot -> p_depth);
      }
    }
  }
  else if (glsl -> object == LIGHT)
  {
    if (plot -> light_loc[ids])
    {
      glUniformMatrix4fv (glsl -> uniform_loc[0], 1, GL_FALSE, & wingl -> proj_model_matrix.m00);
    }
    else
    {
      glUniformMatrix4fv (glsl -> uniform_loc[0], 1, GL_FALSE, & wingl -> proj_model_view_matrix.m00);
    }
  }
  else
  {
    glUniformMatrix4fv (glsl -> uniform_loc[0], 1, GL_FALSE, & wingl -> proj_model_view_matrix.m00);
  }

  if (glsl -> line_width != 0.0) glLineWidth (glsl -> line_width);

  if (glsl -> light_uniform != NULL) set_lights_data (glsl);

  glBindVertexArray (glsl -> vao);

  if (glsl_disable_cull_face (glsl)) glDisable (GL_CULL_FACE);
  if (plot -> render == LINE)
  {
    glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth (1.0);
  }
  else if (plot -> render == PTS)
  {
    glPolygonMode (GL_FRONT_AND_BACK, GL_POINT);
  }
  else
  {
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
  }

  if (glsl -> draw_type == GLSL_SPHERES || glsl -> draw_type == GLSL_CYLINDERS || glsl -> draw_type == GLSL_CAPS)
  {
    if (glsl -> draw_instanced)
    {
      glDrawElementsInstanced (glsl -> vert_type, glsl -> obj -> num_indices, GL_UNSIGNED_INT, 0, glsl -> obj -> num_instances);
    }
    else
    {
      glDrawElements (glsl -> vert_type, glsl -> obj -> num_indices, GL_UNSIGNED_INT, 0);
    }
  }
  else if (glsl -> draw_type == GLSL_POINTS || glsl -> draw_type == GLSL_LINES || glsl -> draw_type == GLSL_STRING)
  {
    if (glsl -> draw_type == GLSL_STRING)
    {
      glEnable (ogl_texture);
      glActiveTexture (GL_TEXTURE0);
      glBindTexture (ogl_texture, glsl -> obj -> texture);
    }
    if (glsl -> draw_instanced)
    {
      j = (glsl -> draw_type == GLSL_STRING) ? 4 : 3*(glsl -> draw_type+1);
      glDrawArraysInstanced (glsl -> vert_type, 0, j, glsl -> obj -> num_instances);
    }
    else
    {
      glDrawArrays (glsl -> vert_type, 0, glsl -> obj -> num_vertices);
    }
  }
  else
  {
    if (glsl -> draw_type == GLSL_BACK) glDisable (GL_DEPTH_TEST);
    glDrawArrays (glsl -> vert_type, 0, glsl -> obj -> num_vertices);
    if (glsl -> draw_type == GLSL_BACK) glEnable (GL_DEPTH_TEST);
  }
  if (glsl_disable_cull_face (glsl)) glEnable (GL_CULL_FACE);
  glBindVertexArray (0);
}

/*!
  \fn void draw_vertices (int id)

  \brief Draw OpenGL shader program

  \param id the ID of the program to render
*/
void draw_vertices (int id)
{
  int i, j;
  glsl_program * glsl;
  if (id != MEASU)
  {
    i = (in_md_shaders(proj_gl, id)) ? step : 0;
    for (j=0; j<wingl -> n_shaders[id][i]; j++)
    {
      if (wingl -> ogl_glsl[id][i][j])
      {
        glsl = wingl -> ogl_glsl[id][i][j];
        glUseProgram (glsl -> id);
        render_this_shader (glsl, j);
      }
    }
  }
  else
  {
    for (i=0; i<2; i++)
    {
      this_pattern = plot -> mpattern[i];
      this_tilt = plot -> mtilt[i];
      this_factor = plot -> mfactor[i];
      for (j=0; j<wingl -> n_shaders[id][i]; j++)
      {
        if (wingl -> ogl_glsl[id][i][j])
        {
          glsl = wingl -> ogl_glsl[id][i][j];
          glUseProgram (glsl -> id);
          render_this_shader (glsl, j);
        }
      }
    }
  }
}
