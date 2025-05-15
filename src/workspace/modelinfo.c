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
* @file modelinfo.c
* @short Functions to display model information in GtkTextBuffer
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'modelinfo.c'
*
* Contains:
*

 - Functions to display model information in GtkTextBuffer

*
* List of functions:

  void print_spg_name (gchar * str , GtkTextBuffer * buffer);
  void print_spg_setting (gchar * init, GtkTextBuffer * buffer);
  void model_info (project * this_proj, GtkTextBuffer * buffer);
  void print_this_ogl_stuff (gchar * text, GLenum name, GtkTextBuffer * buf);
  void opengl_info (project * this_proj, GtkTextBuffer * buf);

*/

#include "global.h"
#include "interface.h"

extern gchar * groups[230] ;
extern char * vect_comp[3];
extern gchar * substitute_string (gchar * init, gchar * o_motif, gchar * n_motif);
extern gchar * get_num_string (gchar * str);

/*!
  \fn void print_spg_name (gchar * str , GtkTextBuffer * buffer)

  \brief print space group name in a GtkTextBuffer

  \param str the space group string
  \param buffer the GtkTextBuffer
*/
void print_spg_name (gchar * str , GtkTextBuffer * buffer)
{
  str = substitute_string (str, "</", "y");
  str = substitute_string (str, "<", "w");
  str = substitute_string (str, ">", "x");
  if (strstr(str, "wsubx"))
  {
    gchar * sta = g_strdup_printf ("%s", str);
    sta = strtok (sta, "wsubx");
    print_info (sta, "bold", buffer);
    while (sta)
    {
      sta = strtok (NULL, "ysubx");
      if (sta)
      {
        print_info (sta, "sub_bold", buffer);
        sta = strtok (NULL, "wsubx");
        if (sta) print_info (sta, "bold", buffer);
      }
    }
  }
  else
  {
    print_info (str, "bold", buffer);
  }
  g_free (str);
}

/*!
  \fn void print_spg_setting (gchar * init, GtkTextBuffer * buffer)

  \brief print space group name in a GtkTextBuffer

  \param init the space group setting
  \param buffer the GtkTextBuffer
*/
void print_spg_setting (gchar * init, GtkTextBuffer * buffer)
{
  gchar * str = g_strdup_printf ("%s", init);
  str = substitute_string (str, "_", " ");
  str = get_num_string (str);
  str = substitute_string (str, " ", NULL);
  print_spg_name (str, buffer);
}

/*!
  \fn void model_info (project * this_proj, GtkTextBuffer * buffer)

  \brief display model information in GtkTexBuffer

  \param this_proj the target project
  \param buffer the GtkTextBuffer
*/
void model_info (project * this_proj, GtkTextBuffer * buffer)
{
  int i, j, k;
  double v;
  gchar * str;
  if (g_strcmp0(this_proj -> projfile, "(null)") == 0) this_proj -> projfile = NULL;
  if (this_proj -> projfile != NULL)
  {
    print_info ("\n\tProject file: ", "italic", buffer);
    print_info (this_proj -> projfile, NULL, buffer);
  }

  if (this_proj -> tfile > -1 && this_proj -> coordfile != NULL)
  {
    print_info ("\n\tCoordinates file: ", "italic", buffer);
    print_info (this_proj -> coordfile, NULL, buffer);
    print_info ("\n\tFile type:   ", "italic", buffer);
    print_info (coord_files[this_proj -> tfile], NULL, buffer);
    this_proj -> tfile = 0;
  }
  print_info ("\n\nModel properties\n\n\n", "heading", buffer);
  if (this_proj -> steps > 1)
  {
    print_info ("\tNumber of MD steps = ", "italic", buffer);
    str = g_strdup_printf ("%d\n", this_proj -> steps);
    print_info (str, "bold", buffer);
    g_free (str);
  }
  print_info ("\tNumber of atoms    = ", "italic", buffer);
  str = g_strdup_printf ("%d\n", this_proj -> natomes);
  print_info (str, "bold", buffer);
  g_free (str);
  print_info ("\tNumber of species  = ", "italic", buffer);
  str = g_strdup_printf ("%d\n\n", this_proj -> nspec);
  print_info (str, "bold", buffer);
  g_free (str);
  if (this_proj -> cell.volume > 0.0)
  {
    gchar * latv[2][3]={{"a", "b", "c"},
                        {"α", "β", "γ"}};
    if (this_proj -> cell.npt)
    {
      print_info ("\tAv. lattice parameters:\n\n", NULL, buffer);
    }
    else
    {
      print_info ("\tLattice parameters:\n\n", NULL, buffer);
    }
    for (i=0; i<2; i++)
    {
      print_info ("\t", NULL, buffer);
      for (j=0; j<3; j++)
      {
        str = g_strdup_printf ("\t %s=", latv[i][j]);
        print_info (str, "bold_italic", buffer);
        g_free (str);
        if (this_proj -> cell.npt)
        {
          v = 0.0;
          for (k=0; k<this_proj -> steps; k++) v += this_proj -> cell.box[k].param[i][j];
          v /= this_proj -> steps;
        }
        else
        {
          v = this_proj -> cell.box[0].param[i][j];
        }
        str = g_strdup_printf ("%10.5lf", v);
        print_info (str, "bold_green", buffer);
        g_free (str);
      }
      print_info ("\n", NULL, buffer);
    }
    print_info ("\n", NULL, buffer);
    if (this_proj -> cell.npt)
    {
      print_info ("\tAv. lattice vectors:\n\n\t\t", NULL, buffer);
    }
    else
    {
      print_info ("\tLattice vectors:\n\n\t\t", NULL, buffer);
    }
    for (i=0; i<3; i++)
    {
      str = g_strdup_printf ("       %s  ", vect_comp[i]);
      print_info (str, NULL, buffer);
      g_free (str);
    }
    print_info ("\n", NULL, buffer);
    for (i=0; i<3; i++)
    {
      str = g_strdup_printf ("\t\t%s", latv[0][i]);
      print_info (str, NULL, buffer);
      g_free (str);
      for (j=0; j<3; j++)
      {
        if (this_proj -> cell.npt)
        {
          v = 0.0;
          for (k=0; k<this_proj -> steps; k++) v += this_proj -> cell.box[k].vect[i][j];
          v /= this_proj -> steps;
        }
        else
        {
          v = this_proj -> cell.box[0].vect[i][j];
        }
        str = g_strdup_printf ("%10.5lf", v);
        print_info (str, "bold_orange", buffer);
        g_free (str);
      }
      print_info ("\n", NULL, buffer);
    }
    if (this_proj -> cell.sp_group)
    {
      print_info ("\n", NULL, buffer);
      print_info ("\tCrystal information:\n\n", "italic", buffer);
      str = g_strdup_printf ("%d", this_proj -> cell.sp_group -> id);
      print_info ("\t\tSpace group N°:       ","italic", buffer);
      print_info (str, "bold", buffer);
      g_free (str);
      print_info ("\n", NULL, buffer);
      print_info ("\t\tSpace group:          ","italic", buffer);
      print_spg_name (g_strdup_printf ("%s", groups[this_proj -> cell.sp_group -> id-1]), buffer);
      print_info ("\n", NULL, buffer);
      print_info ("\t\tSpace group setting:  ","italic", buffer);
      print_spg_setting (this_proj -> cell.sp_group -> setting, buffer);
      print_info ("\n", NULL, buffer);
      print_info ("\t\tHermman-Mauguin:      ","italic", buffer);
      print_spg_setting (this_proj -> cell.sp_group -> hms, buffer);
      print_info ("\n", NULL, buffer);
      print_info ("\t\tBravais lattice:      ","italic", buffer);
      print_info (this_proj -> cell.sp_group -> bravais, "bold", buffer);
      print_info ("\n", NULL, buffer);
    }

    print_info ("\n", NULL, buffer);
    if (this_proj -> cell.npt)
    {
      print_info ("\tAv. volume         = ", "italic", buffer);
    }
    else
    {
      print_info ("\tVolume             = ", "italic", buffer);
    }
    str = g_strdup_printf ("%14.6lf", this_proj -> cell.volume);
    print_info (str, "bold", buffer);
    g_free (str);
    print_info ("  Å", NULL, buffer);
    print_info ("3", "sup", buffer);
    if (this_proj -> cell.npt)
    {
      print_info ("\n\tAv. density        = ", "italic", buffer);
    }
    else
    {
      print_info ("\n\tDensity            = ", "italic", buffer);
    }
    str = g_strdup_printf ("%14.6lf", this_proj -> cell.density);
    print_info (str, "bold", buffer);
    g_free (str);
    print_info ("  g / cm", NULL, buffer);
    print_info ("3", "sup", buffer);
    if (this_proj -> cell.npt)
    {
      print_info ("\n\tAv. number density = ", "italic", buffer);
    }
    else
    {
      print_info ("\n\tNumber density     = ", "italic", buffer);
    }
    str = g_strdup_printf ("%14.6lf", this_proj -> natomes / this_proj -> cell.volume);
    print_info (str, "bold", buffer);
    g_free (str);
    print_info ("  Atoms / Å", NULL, buffer);
    print_info ("3", "sup", buffer);
  }
  print_info ("\n\n\tEmpirical formula:\t", "italic", buffer);
  for (i=0; i<this_proj -> nspec; i++)
  {
    print_info (this_proj -> chemistry -> label[i], "bold", buffer);
    if (this_proj -> chemistry -> formula[i] != 1)
    {
      str = g_strdup_printf ("%d", this_proj -> chemistry -> formula[i]);
      print_info (str, "sub", buffer);
      g_free (str);
    }
    print_info (" ", NULL, buffer);
  }
  if (this_proj -> cell.volume != 0.0)
  {
    print_info ("\n\n\tAtom\t Number    Fraction\tNumber density (atoms / Å", "italic", buffer);
    print_info ("3", "sup", buffer);
    print_info (")\n", "italic", buffer);
  }
  else
  {
    print_info ("\n\n\tAtom\t Number    Fraction", "italic", buffer);
  }

  for (i=0; i<this_proj -> nspec; i++)
  {
    str = g_strdup_printf ("\n\t%s\t%7d\t%11.6lf",
                           this_proj -> chemistry -> label[i], this_proj -> chemistry -> nsps[i],
                           (double)this_proj -> chemistry -> nsps[i]/this_proj -> natomes);
    print_info (str, textcolor(i), buffer);
    g_free (str);
    if (this_proj -> cell.volume != 0.0)
    {
      str = g_strdup_printf ("\t%11.6lf", this_proj -> chemistry -> nsps[i]/this_proj -> cell.volume);
      print_info (str, textcolor(i), buffer);
      g_free (str);
    }
  }
}

/*!
  \fn void print_this_ogl_stuff (gchar * text, GLenum name, GtkTextBuffer * buf)

  \brief print OpenGL stuff in a GtkTextBuffer

  \param text Message
  \param name OpenGL id to retreive
  \param buf the GtkTextBuffer
*/
void print_this_ogl_stuff (gchar * text, GLenum name, GtkTextBuffer * buf)
{
  int i;
  gchar * str;
  print_info (text, NULL, buf);
  glGetIntegerv (name, & i);
  str = g_strdup_printf ("%d\n", i);
  print_info (str, "bold_blue", buf);
  g_free (str);
}

/*!
  \fn void opengl_info (project * this_proj, GtkTextBuffer * buf)

  \brief display OpenGL information in a GtkTextBuffer

  \param this_proj the target project
  \param buf the GtkTextBuffer
*/
void opengl_info (project * this_proj, GtkTextBuffer * buf)
{
  int i, j, k, l;
  i = j = k = l = -1;

  gtk_gl_area_make_current ((GtkGLArea *)this_proj -> modelgl -> plot);
  gchar * str;
  print_info ("\n\nOpenGL driver information\n\n", "heading", buf);
  print_info ("\tOpenGL Version  \t\t\t: ", NULL, buf);
  str = g_strdup_printf ("%s\n", (const char*)glGetString(GL_VERSION));
  print_info (str, "bold_blue", buf);
  g_free (str);
  print_info ("\tOpenGL Driver\t\t\t\t: ", NULL, buf);
  str = g_strdup_printf ("%s\n", (const char*)glGetString(GL_VENDOR));
  print_info (str, "bold_blue", buf);
  g_free (str);
  print_info ("\tOpenGL Hardware \t\t\t: ", NULL, buf);
  str = g_strdup_printf ("%s\n", (const char*)glGetString(GL_RENDERER));
  print_info (str, "bold_blue", buf);
  g_free (str);

  print_info ("\tOpenGL Shading Version \t\t\t: ", NULL, buf);
  str = g_strdup_printf ("%s\n\n", (const char*)glGetString (GL_SHADING_LANGUAGE_VERSION));
  print_info (str, "bold_blue", buf);
  g_free (str);

  print_info ("\tOpenGL Window information:\n\n", NULL, buf);
  print_info ("\t\tColor Bits (R,G,B,A)\t\t: ", NULL, buf);
  glGetIntegerv (GL_RED_BITS, & i);
  glGetIntegerv (GL_GREEN_BITS, & j);
  glGetIntegerv (GL_BLUE_BITS, & k);
  glGetIntegerv (GL_ALPHA_BITS, & l);
  str = g_strdup_printf ("%d, %d, %d, %d\n", i, j, k, l);
  print_info (str, "bold_blue", buf);
  g_free (str);
  print_this_ogl_stuff ("\t\tDepth Bits \t\t\t: ", GL_DEPTH_BITS, buf);
  print_this_ogl_stuff ("\t\tStencil Bits \t\t\t: ", GL_STENCIL_BITS, buf);
  print_this_ogl_stuff ("\t\tMax. Lights Allowed \t\t: ", GL_MAX_LIGHTS, buf);
  print_this_ogl_stuff ("\t\tMax. Texture Size \t\t: ", GL_MAX_TEXTURE_SIZE, buf);
  print_this_ogl_stuff ("\t\tMax. Clipping Planes \t\t: ", GL_MAX_CLIP_PLANES, buf);
  print_this_ogl_stuff ("\t\tMax. Modelview Matrix Stacks \t: ", GL_MAX_MODELVIEW_STACK_DEPTH, buf);
  print_this_ogl_stuff ("\t\tMax. Projection Matrix Stacks \t: ", GL_MAX_PROJECTION_STACK_DEPTH, buf);
  print_this_ogl_stuff ("\t\tMax. Attribute Stacks \t\t: ", GL_MAX_ATTRIB_STACK_DEPTH, buf);
  print_this_ogl_stuff ("\t\tMax. Texture Stacks \t\t: ", GL_MAX_TEXTURE_STACK_DEPTH, buf);

  print_info ("\n\tGeometry Shader information:\n\n", NULL, buf);
  print_this_ogl_stuff ("\t\tMax. output vertices \t\t: ", GL_MAX_GEOMETRY_OUTPUT_VERTICES, buf);
  print_this_ogl_stuff ("\t\tMax. output components \t\t: ", GL_MAX_GEOMETRY_OUTPUT_COMPONENTS, buf);
  print_this_ogl_stuff ("\t\tMax. total output components \t: ", GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS, buf);

  print_info ("\n\tTotal number of OpenGL Extensions\t: ", NULL, buf);
  glGetIntegerv (GL_NUM_EXTENSIONS, & i);
  str = g_strdup_printf ("%d\n", i);
  print_info (str, "bold_blue", buf);
  g_free (str);
  print_info ("\tExtensions list:\n", NULL, buf);
  for (j=0; j<i; j++)
  {
    str = g_strdup_printf ("\t\t N°%d\t:\t%s\n", j+1, (const char*)glGetStringi(GL_EXTENSIONS,j));
    print_info (str, NULL, buf);
    g_free (str);
  }
}
