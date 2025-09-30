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
* @file ogl_shading.h
* @short Variable declarations related to GLSL programs \n
         Data structure declarations related to GLSL programs
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This header file: 'ogl_shading.h'
*
* Contains:

 - Variable declarations related to GLSL programs
 - Data structure declarations related to GLSL programs

*/

#ifndef OGL_SHADING_H_
#define OGL_SHADING_H_

/*! \enum glsl_styles */
enum glsl_styles {
  GLSL_POINTS    = 0, /*!< 0 */
  GLSL_SPHERES   = 1, /*!< 1 */
  GLSL_LINES     = 2, /*!< 2 */
  GLSL_CYLINDERS = 3, /*!< 3 */
  GLSL_CAPS      = 4, /*!< 4 */
  GLSL_POLYEDRA  = 5, /*!< 5 */
  GLSL_STRING    = 6, /*!< 6 */
  GLSL_LIGHT     = 7, /*!< 7 */
  GLSL_BACK      = 8  /*!< 8 */
};

#define POLY_BUFF_SIZE 10  // p(x,y,z), n(x,y,z), color (r,g,b,a)
#define LINE_BUFF_SIZE  7  // p(x,y,z), color (r,g,b,a)
#define CYLI_BUFF_SIZE 13  // p(x,y,z), length, rad, quat(w,x,y,z), color (r,g,b,a)
#define CAPS_BUFF_SIZE 12  // p(x,y,z), rad, quat(w,x,y,z), color (r,g,b,a)
#define ATOM_BUFF_SIZE  8  // p(x,y,z), rad, color (r,g,b,a)
#define CHAR_BUFF_SIZE  4  // p(x,y), t(x,y)

// Points
extern const GLchar * point_vertex;
extern const GLchar * point_color;

// Basic lines
extern const GLchar * line_vertex;
extern const GLchar * line_color;
extern const GLchar * line_stipple;
extern const GLchar * line_stipple_color;
extern const GLchar * angle_vertex;
extern const GLchar * angle_stipple;
extern const GLchar * angle_color;

extern const GLchar * angstrom_vertex;
extern const GLchar * degree_vertex;

// Triangles: Multiple lights + Materials + Transparency shaders
extern const GLchar * full_vertex;
extern const GLchar * full_color;

// Sphere
extern const GLchar * sphere_vertex;

// Cylinder and caps
extern const GLchar * cylinder_vertex;
extern const GLchar * cone_vertex;
extern const GLchar * cap_vertex;

extern const GLchar * axis_sphere_vertex;
extern const GLchar * axis_cylinder_geom;
extern const GLchar * axis_line_vertex;

// Cylinder in geometry shader
extern const GLchar * gs_cylinder_vertex;
extern const GLchar * gs_cylinder_geom;

extern const GLchar * polyedron_vertex;
extern const GLchar * polyedron_geom;
extern const GLchar * polyedron_color;

extern const GLchar * axis_cylinder_geom;

extern const GLchar * pick_color;

extern const GLchar * string_vertex;
extern const GLchar * string_color;
extern const GLchar * string_color_2d;

extern const GLchar * background_vertex;
extern const GLchar * background_linear;
extern const GLchar * background_circular;

/*! \typedef object_3d */
typedef struct object_3d object_3d;
struct object_3d
{
  int quality;             /*!< OpenGL rendering quality */
  int num_vertices;        /*!< Number of vertices to render, if any */
  int vert_buffer_size;    /*!< Buffer size for the vertices, if any  */
  float * vertices;        /*!< Vertices to render, if any  */
  int num_indices;         /*!< Number of indices, if any  */
  int ind_buffer_size;     /*!< Buffer size for the indices, if any  */
  int * indices;           /*!< Indices to render, if any  */
  int num_instances;       /*!< Number of instances, if any  */
  int inst_buffer_size;    /*!< Buffer size for the instances, if any  */
  float * instances;       /*!< Instances to render, if any  */
  GLuint texture;          /*!< Texture ID, if any */
  float shift[4];          /*!< 0 to 2, texture position shift, if any: \n
                             (0 = x_shift, 1 = y_shift, 2 = z_shift), \n
                              3 visibility (0 = normal, 1 = always) */
};

/*! \typedef glsl_program */
typedef struct glsl_program glsl_program;
struct glsl_program
{
  int id;                  /*!< The GLSL program ID */
  int object;              /*!< The number of 3D object(s) to render */
  GLuint vertex_shader;    /*!< The vertex shader ID */
  GLuint geometry_shader;  /*!< The geometry shader ID */
  GLuint fragment_shader;  /*!< The fragment shader ID */
  GLenum vert_type;        /*!< The type of vertex */
  int draw_type;           /*!< In \enum glsl_styles */
  gboolean draw_instanced; /*!< 0 = single instance, 1 = multiple instances */
  GLuint vao;              /*!< Vertex object array ID */
  GLuint * vbo;            /*!< Binding buffer(s) */
  GLuint * array_pointer;  /*!< Vertex pointer(s) */
  GLuint * uniform_loc;    /*!< Uniform location pointer(s) */
  GLuint * light_uniform;  /*!< Light(s) uniform */
  object_3d * obj;         /*!< The 3D object(s) to render */
  float line_width;        /*!< Wireframe line width */
  ColRGBA * col;           /*!< String color */
};

#endif
