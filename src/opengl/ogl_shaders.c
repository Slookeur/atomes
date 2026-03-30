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
* @file ogl_shaders.c
* @short OpenGL shaders for the atomes program
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'ogl_shaders.c'
*
* Contains:
*

 - The OpenGL shaders for the atomes program

*
* Notes:
*

  LLM tools (ChatGPT, Gemini via Antigravity, Claude) were used at few occasions to prepare some sections of this file, including:
    - To write parts of the ray-tracing shaders
    - To write parts the gradient background shaders

*/

#include "global.h"

// Point shaders:

// const GLchar * point_vertex;
// const GLchar * point_colors;

//#define GLSL(src) "#version 430 core\n" #src
#define GLSL(src) "#version 150\n" #src

const GLchar * point_vertex = GLSL(
  uniform mat4 mvp;
  in vec3 vert;
  in vec3 offset;
  in float size;
  in vec4 vertColor;

  out vec4 vert_color;
  void main()
  {
    vert_color = vertColor;
    gl_PointSize = size;
    gl_Position = mvp * vec4(vert + offset, 1.0);
  }
);

const GLchar * point_color = GLSL(
  in vec4 vert_color;
  out vec4 fragment_color;
  void main()
  {
    fragment_color = vert_color;
  }
);

// Basic line shaders

// const GLchar * line_vertex;
// const GLchar * line_colors;

const GLchar * line_vertex = GLSL(
  uniform mat4 mvp;
  in vec3 vert;
  in vec4 vertColor;

  out vec4 vert_color;
  void main()
  {
    vert_color = vertColor;
    gl_Position = mvp * vec4(vert, 1.0);
  }
);

const GLchar * line_color = GLSL(
  in vec4 vert_color;
  out vec4 fragment_color;
  void main()
  {
    fragment_color = vert_color;
  }
);

const GLchar * line_stipple = GLSL(
  layout(lines) in;
  layout(line_strip, max_vertices=2) out;
  in vec4 vert_color[];
  uniform float depth;

  out float line_pos;
  out vec4 v_color;
  void main()
  {
    vec2 win_pos_0 = gl_in[0].gl_Position.xy;
    vec2 win_pos_1 = gl_in[1].gl_Position.xy;
    gl_Position = gl_in[0].gl_Position;
    line_pos = 0.0;
    v_color = vert_color[0];
    EmitVertex();
    gl_Position = gl_in[1].gl_Position;
    float psize = 3.0; // No particular reason, just seems right
    line_pos = psize * length(win_pos_1 - win_pos_0);
    line_pos *= depth;
    v_color = vert_color[1];
    EmitVertex();
    EndPrimitive();
  }
);

const GLchar * angle_vertex = GLSL(
  uniform mat4 mvp;
  in vec3 vert;
  in vec4 vertColor;

  out vec4 vert_color;
  void main()
  {
    vert_color = vertColor;
    gl_Position = vec4(vert, 1.0);
  }
);

const GLchar * angle_stipple = GLSL(
  layout(triangles) in;
  layout(line_strip, max_vertices=20) out;
  in vec4 vert_color[];

  uniform float depth;
  uniform mat4 mvp;
  uniform mat4 un_view;
  uniform mat4 text_proj;
  uniform vec4 viewp;

  out float line_pos;
  out vec4 v_color;

  const float PI = 3.14159265359;

  mat4 translate_this (in vec3 coord)
  {
    mat4 translate;
    translate[0] = vec4(1.0, 0.0, 0.0, 0.0);
    translate[1] = vec4(0.0, 1.0, 0.0, 0.0);
    translate[2] = vec4(0.0, 0.0, 1.0, 0.0);
    translate[3][0] = coord.x;
    translate[3][1] = coord.y;
    translate[3][2] = coord.z;
    translate[3][3] = 1.0;
    return translate;
  }

  vec3 project (in vec3 coord)
  {
    mat4 n_mvp = (mvp * translate_this (coord));
    vec4 res = n_mvp * vec4(vec3(0.0), 1.0);
    if (res.w != 0.0)
    {
      res.w = 1.0 / res.w;
      res.x = res.w * res.x + 1.0;
      res.y = res.w * res.y + 1.0;
      res.z = res.w * res.z + 1.0;
      return vec3 (res.x*viewp.z+viewp.x, res.y*viewp.w+viewp.y, res.z);
    }
    else
    {
      return vec3 (0.0, 0.0, -1.0);
    }
  }

  float angle2d (in vec2 at, in vec2 bt, in vec2 ct)
  {
    vec2 ab = bt - at;
    vec2 bc = bt - ct;
    float theta = dot(ab,bc) / (length(ab) * length(bc));
    if (theta < -1.0)
    {
      return acos (-2.0 - theta);
    }
    else if (theta > 1.0)
    {
      return acos (2.0 - theta);
    }
    else
    {
      return acos (theta);
    }
  }

  void main()
  {
    vec3 pa = project (gl_in[0].gl_Position.xyz);
    vec3 pb = project (gl_in[1].gl_Position.xyz);
    vec3 pc = project (gl_in[2].gl_Position.xyz);
    vec2 pd;
    pd.x = pb.x + 100.0;
    pd.y = pb.y;
    float alpha = angle2d (pa.xy, pb.xy, pd);
    float beta = angle2d (pc.xy, pb.xy, pd);
    float theta = angle2d (pa.xy, pb.xy, pc.xy);
    if (pa.y > pb.y && pc.y > pb.y)
    {
      beta = min (alpha, beta);
    }
    else if (pa.y < pb.y && pc.y < pb.y)
    {
      beta = min (-alpha, -beta);
      // or beta = - max (alpha, beta);
    }
    else
    {
      vec2 pe;
      vec2 pf;
      pe.y = max(pa.y, pc.y);
      if (pe.y == pa.y)
      {
        pe.x = pa.x;
        pf.y = pc.y;
        pf.x = pc.x;
      }
      else
      {
        pe.x = pc.x;
        pf.y = pa.y;
        pf.x = pa.x;
      }
      beta = angle2d (pe, pb.xy, pd);
      alpha = angle2d (pf, pb.xy, pd);
      if (beta + alpha < PI)
      {
        beta -= theta;
      }
    }

    float psize = 3.0; // No particular reason, just seems right
    float tan_factor = tan(theta/10.0);
    float radial_factor = cos(theta/10.0);
    float dist = min(length(pb-pa), length(pb-pc)) / 3.0;
    float x = dist * cos(beta);
    float y = dist * sin(beta);
    float tx;
    float ty;

    vec2 apos;
    vec2 bpos;
    v_color = vert_color[0];

    vec4 pos_s = mvp * gl_in[0].gl_Position;
    vec4 pos_e = mvp * gl_in[1].gl_Position;
    line_pos = 0.0;
    gl_Position = pos_s;
    EmitVertex();
    float line_save = psize * length(pos_e.xy - pos_s.xy);
    line_pos = line_save;
    line_pos *= depth;
    gl_Position = pos_e;
    EmitVertex();
    int i;
    line_pos = -1.0;
    EmitVertex();
    for (i = 0; i < 11; i++)
    {
      apos = vec2(x,y);
      gl_Position = text_proj * (vec4(pb, 1.0) + vec4(apos.x, apos.y, 0.0, 1.0));
      EmitVertex();
      if (i == 0)
      {
        line_pos = line_save;
        EmitVertex();
      }
      tx = -y;
      ty = x;
      x += tx * tan_factor;
      y += ty * tan_factor;
      x *= radial_factor;
      y *= radial_factor;
      bpos = vec2(x,y);
      line_pos += psize * length(bpos - apos) / 20.0;
    }
    EmitVertex();
    line_save = line_pos;
    line_pos = -1.0;
    EmitVertex();
    gl_Position = pos_e;
    EmitVertex();
    line_pos = line_save;
    line_pos *= depth;
    EmitVertex();
    pos_s = mvp * gl_in[2].gl_Position;
    line_pos += depth * psize * length(pos_s.xy - pos_e.xy);
    gl_Position = pos_s;
    EmitVertex();
    EndPrimitive();
  }
);

const GLchar * line_stipple_color = GLSL(
  uniform int factor;
  uniform uint pattern;
  in float line_pos;
  in vec4 v_color;

  out vec4 fragment_color;
  void main()
  {
    fragment_color = v_color;
    if (line_pos == -1.0) discard;
    uint bit = uint(round(line_pos/factor)) % 16U;
    if ((pattern & (1U<<bit)) == 0U) discard;
  }
);

const GLchar * angle_color = GLSL(
  uniform int factor;
  uniform uint pattern;
  in float line_pos;
  in vec4 v_color;

  out vec4 fragment_color;
  void main()
  {
    fragment_color = v_color;
    if (line_pos == -1.0) discard;
  }
);

// Triangle shader

const GLchar * full_vertex = GLSL(
  uniform mat4 mvp;
  uniform mat4 m_view;
  in vec3 vert;
  in vec3 vertNormal;
  in vec4 vertColor;

  out vec4 surfaceColor;
  out vec3 surfacePosition;
  out vec3 surfaceNormal;
  out vec3 surfaceToCamera;
  void main ()
  {
    surfaceColor    = vertColor;

    surfacePosition = vec3(m_view * vec4(vert, 1.0f));
    surfaceNormal   = mat3(m_view) * vertNormal;
    surfaceToCamera = normalize (- surfacePosition);
    gl_Position     = mvp * vec4(vert, 1.0f);
  }
);

// Sphere

const GLchar * sphere_vertex = GLSL(
  uniform mat4 mvp;
  uniform mat4 m_view;

  in vec3 vert;
  in vec3 offset;
  in vec4 vertColor;
  in float radius;

  out vec4 surfaceColor;
  out vec3 surfacePosition;
  out vec3 surfaceNormal;
  out vec3 surfaceToCamera;
  void main ()
  {
    surfaceColor    = vertColor;
    vec4 pos = vec4 (radius*vert + offset, 1.0);
    surfacePosition = vec3(m_view * pos);
    surfaceNormal   = mat3(m_view) * vert;
    surfaceToCamera = normalize (- surfacePosition);
    gl_PointSize = 1.0;
    gl_Position = mvp * pos;
  }
);

// Cylinder

const GLchar * cylinder_vertex = GLSL(
  uniform mat4 mvp;
  uniform mat4 m_view;
  in vec4 quat;
  in float height;
  in float radius;
  in vec3 offset;
  in vec3 vert;
  in vec4 vertColor;

  out vec4 surfaceColor;
  out vec3 surfacePosition;
  out vec3 surfaceNormal;
  out vec3 surfaceToCamera;

  vec3 rotate_this (in vec3 v, in vec4 quat)
  {
    vec3 u = vec3(quat.x, quat.y, quat.z);
    float s = quat.w;
    return 2.0 * dot(u,v) * u + (s*s - dot(u,u)) * v + 2.0 * s * cross (u,v);
  }

  void main ()
  {
    surfaceColor = vertColor;
    vec3 pos =  vec3(radius*vert.x, radius*vert.y, height*vert.z);
    vec3 norm = normalize (vec3(vert.x, vert.y, 0.0));
    if (quat.w != 0.0)
    {
      pos = rotate_this (pos, quat);
      norm = rotate_this (norm, quat);
    }
    pos += offset;
    surfacePosition = vec3(m_view * vec4(pos,1.0));
    surfaceNormal   = mat3(m_view) * norm;
    surfaceToCamera = normalize (- surfacePosition);
    gl_Position = mvp * vec4(pos,1.0);
  }
);

const GLchar * cone_vertex = GLSL(
  uniform mat4 mvp;
  uniform mat4 m_view;
  in vec4 quat;
  in float height;
  in float radius;
  in vec3 offset;
  in vec3 vert;
  in vec4 vertColor;

  out vec4 surfaceColor;
  out vec3 surfacePosition;
  out vec3 surfaceNormal;
  out vec3 surfaceToCamera;

  vec3 rotate_this (in vec3 v, in vec4 quat)
  {
    vec3 u = vec3(quat.x, quat.y, quat.z);
    float s = quat.w;
    return 2.0 * dot(u,v) * u + (s*s - dot(u,u)) * v + 2.0 * s * cross (u,v);
  }

  void main ()
  {
    surfaceColor = vertColor;
    vec3 pos =  vec3(radius*vert.x, radius*vert.y, height*vert.z);
    // The normal calculation changes / cylinders
    float B = sqrt(radius*radius + height*height);
    vec3 norm = vec3(height*vert.x/B, height*vert.y/B, radius/B);
    if (quat.w != 0.0)
    {
      pos = rotate_this (pos, quat);
      norm = rotate_this (norm, quat);
    }
    pos += offset;
    surfacePosition = vec3(m_view * vec4(pos,1.0));
    surfaceNormal   = mat3(m_view) * norm;
    surfaceToCamera = normalize (- surfacePosition);
    gl_Position = mvp * vec4(pos,1.0);
  }
);

const GLchar * cap_vertex = GLSL(
  uniform mat4 mvp;
  uniform mat4 m_view;
  in vec4 quat;
  in float radius;
  in vec3 offset;
  in vec3 vert;
  in vec4 vertColor;

  out vec4 surfaceColor;
  out vec3 surfacePosition;
  out vec3 surfaceNormal;
  out vec3 surfaceToCamera;

  vec3 rotate_this (in vec3 v, in vec4 quat)
  {
    vec3 u = vec3(quat.x, quat.y, quat.z);
    float s = quat.w;
    return 2.0 * dot(u,v) * u + (s*s - dot(u,u)) * v + 2.0 * s * cross (u,v);
  }

  void main ()
  {
    surfaceColor = vertColor;
    vec3 pos =  vec3(radius*vert.x, radius*vert.y, vert.z);
    vec3 norm = vec3(0.0, 0.0, -1.0);
    if (quat.w != 0.0)
    {
      pos = rotate_this (pos, quat);
      norm = rotate_this (norm, quat);
    }
    pos += offset;
    surfacePosition = vec3(m_view * vec4(pos,1.0));
    surfaceNormal   = mat3(m_view) * norm;
    surfaceToCamera = normalize (- surfacePosition);
    gl_Position = mvp * vec4(pos,1.0);
  }
);

const GLchar * gs_cylinder_geom = GLSL(
  layout (lines) in;
  layout (triangle_strip, max_vertices=64) out;

  uniform mat4 mvp;
  uniform mat4 m_view;
  uniform int quality;
  uniform float radius;

  in vec4 vertCol[];

  out vec4 surfaceColor;
  out vec3 surfacePosition;
  out vec3 surfaceNormal;
  out vec3 surfaceToCamera;

  float pi = 3.141592653;

  vec3 create_perp (in vec3 axis)
  {
    vec3 u = vec3(0.0, 0.0, 1.0);
    vec3 v = vec3(0.0, 1.0, 0.0);
    vec3 res =  cross(u, axis);
    if (length(res) == 0.0)
    {
      res = cross (v, axis);
    }
    return res;
  }

  void main()
  {
    vec3 v1 = gl_in[0].gl_Position.xyz;
    vec3 v2 = gl_in[1].gl_Position.xyz;
    vec3 axis = normalize(v2 - v1);
    vec3 perp_x = create_perp (axis);
    vec3 perp_y = cross (axis, perp_x);
    float step = 2.0 * pi / float(quality - 1);
    for(int i=0; i<quality; i++)
    {
      float a = i * step;
      float ca = cos(a);
      float sa = sin(a);

      vec3 normal = normalize(ca*perp_x + sa*perp_y);
      vec3 p1 = v1 + radius * normal;
      vec3 p2 = v2 + radius * normal;

      surfaceNormal =  mat3(m_view) * normal;
      gl_Position = mvp * vec4(p1, 1.0);

      surfacePosition = vec3(m_view * vec4(p1, 1.0));
      surfaceToCamera = normalize (- surfacePosition);
      surfaceColor = vertCol[0];
      EmitVertex();

      gl_Position = mvp * vec4 (p2, 1.0);
      surfacePosition = vec3(m_view * vec4(p2, 1.0));
      surfaceToCamera = normalize (- surfacePosition);
      surfaceColor = vertCol[1];
      EmitVertex();
    }
    EndPrimitive();
  }
);

const GLchar * string_vertex = GLSL(
  uniform mat4 mvp;
  uniform mat4 un_view;
  uniform mat4 text_proj;
  uniform vec4 viewp;
  uniform vec4 pos_shift;
  in vec2 vert;
  in vec2 tcoord;
  in vec3 offset;
  out float angle;

  out vec2 text_coords;
  mat4 translate_this (in vec3 coord)
  {
    mat4 translate;
    translate[0] = vec4(1.0, 0.0, 0.0, 0.0);
    translate[1] = vec4(0.0, 1.0, 0.0, 0.0);
    translate[2] = vec4(0.0, 0.0, 1.0, 0.0);
    translate[3][0] = coord.x;
    translate[3][1] = coord.y;
    translate[3][2] = coord.z;
    translate[3][3] = 1.0;

    return translate;
  }

  vec4 project (in vec3 coord)
  {
    mat4 n_mvp = ((mvp * translate_this (coord)) * un_view) * translate_this (pos_shift.xyz);
    vec4 res = n_mvp * vec4(vec3(0.0), 1.0);
    if (res.w != 0.0)
    {
      res.w = 1.0 / res.w;
      res.x = res.w * res.x + 1.0;
      res.y = res.w * res.y + 1.0;
      res.z = res.w * res.z + 1.0;
      return vec4 (res.x*viewp.z+viewp.x, res.y*viewp.w+viewp.y, pos_shift.w*res.z, 1.0);
    }
    else
    {
      return vec4 (0.0, 0.0, -1.0, 0.0);
    }
  }

  void main()
  {
    text_coords = tcoord;
    vec4 pos = project (offset) + vec4(vert, 0.0, 1.0);
    gl_Position = text_proj * pos;
  }
);

const GLchar * full_color = GLSL(

  int PHONG           = 1;
  int BLINN           = 2;
  int COOK_BLINN      = 3;
  int COOK_BECKMANN   = 4;
  int COOK_GGX        = 5;

  struct Light {
    int type;
    vec3 position;
    vec3 direction;
    vec3 intensity;
    float constant;
    float linear;
    float quadratic;
    float cone_angle;
    float spot_inner;
    float spot_outer;
  };

  struct Material {
    vec3 albedo;
    float metallic;
    float roughness;
    float ambient_occlusion;
    float gamma;
    float alpha;
  };

  struct Fog {
    int mode;
    int based;
    float density;
    vec2 depth;
    vec3 color;
  };

  uniform Light AllLights[10];
  uniform Material mat;
  uniform Fog fog;
  uniform int lights_on;
  uniform int numLights;

  in vec4 surfaceColor;
  in vec3 surfacePosition;
  in vec3 surfaceNormal;
  in vec3 surfaceToCamera;

  out vec4 fragment_color;

  const float PI = 3.14159265359;
  const float EPS = 1e-5;

  // clamping to 0 - 1 range
  float saturate (in float value)
  {
    return clamp(value, 0.0, 1.0);
  }

  // phong (lambertian) diffuse term
  float phong_diffuse()
  {
    return (1.0 / PI);
  }

  // compute Fresnel specular factor for given base specular and product
  // product could be NdV or VdH depending on used technique
  vec3 fresnel_factor (in vec3 f0, in float product)
  {
    return mix(f0, vec3(1.0), pow(1.01 - product, 5.0));
  }

  // following functions are copies of UE4
  // for computing cook-torrance specular lighting terms

  float D_blinn(in float roughness, in float NdH)
  {
    float m = roughness * roughness;
    float m2 = m * m;
    float n = 2.0 / m2 - 2.0;
    return (n + 2.0) / (2.0 * PI) * pow(NdH, n);
  }

  float D_beckmann(in float roughness, in float NdH)
  {
    float m = roughness * roughness;
    float m2 = m * m;
    float NdH2 = NdH * NdH;
    return exp((NdH2 - 1.0) / (m2 * NdH2)) / (PI * m2 * NdH2 * NdH2);
  }

  float D_GGX(in float roughness, in float NdH)
  {
    float m = roughness * roughness;
    float m2 = m * m;
    float d = (NdH * m2 - NdH) * NdH + 1.0;
    return m2 / (PI * d * d);
  }

  float G_schlick(in float roughness, in float NdV, in float NdL)
  {
    float k = roughness * roughness * 0.5;
    float V = NdV * (1.0 - k) + k;
    float L = NdL * (1.0 - k) + k;
    return 0.25 / (V * L);
  }

  // simple phong specular calculation with normalization
  vec3 phong_specular(in vec3 V, in vec3 L, in vec3 N, in vec3 specular, in float roughness)
  {
    vec3 R = reflect(-L, N);
    float spec = max(0.0, dot(V, R));

    float k = 1.999 / (roughness * roughness);

    return min(1.0, 3.0 * 0.0398 * k) * pow(spec, min(10000.0, k)) * specular;
  }

  // simple blinn specular calculation with normalization
  vec3 blinn_specular(in float NdH, in vec3 specular, in float roughness)
  {
    float k = 1.999 / (roughness * roughness);

    return min(1.0, 3.0 * 0.0398 * k) * pow(NdH, min(10000.0, k)) * specular;
  }

  // cook-torrance specular calculation
  vec3 cooktorrance_specular (in int cook, in float NdL, in float NdV, in float NdH, in vec3 specular, in float roughness)
  {
    float D;
    if (cook == COOK_BLINN)
    {
      D = D_blinn(roughness, NdH);
    }
    else if (cook == COOK_BECKMANN)
    {
      D = D_beckmann(roughness, NdH);
    }
    else if (cook == COOK_GGX)
    {
      D = D_GGX(roughness, NdH);
    }

    float G = G_schlick(roughness, NdV, NdL);

    float rim = mix(1.0 - roughness * 0.9, 1.0, NdV);

    return (1.0 / rim) * specular * G * D;
  }

  vec3 Apply_lighting_model (in int model, in Light light, in vec3 specular)
  {
    vec3 v_pos = surfacePosition;
    // L, V, H vectors
    vec3 L;
    float A;
    float I = 1.0;
    if (light.type == 0)
    {
      // Directional light
      L = normalize (-light.direction);
      A = 1.0;
    }
    else
    {
      L = light.position - v_pos;
      float dist = length (L);
      L = normalize(L);
      A = 1.0 / (light.constant + light.linear*dist + light.quadratic*dist*dist);
      if (light.type == 2)
      {
        float theta = dot(L, normalize(light.position-light.direction));
        if(theta > light.cone_angle)
        {
          float epsilon = light.spot_inner - light.spot_outer;
          I = saturate((theta - light.spot_outer) / epsilon);
        }
        else
        {
          return vec3(0.0001);
        }
      }
    }
    vec3 V = normalize(-v_pos);
    vec3 H = normalize(L + V);
    vec3 N = surfaceNormal;

    // compute material reflectance
    float NdL = max(0.0, dot(N, L));
    float NdV = max(0.001, dot(N, V));
    float NdH = max(0.001, dot(N, H));
    float HdV = max(0.001, dot(H, V));
    float LdV = max(0.001, dot(L, V));

    // fresnel term is common for any, except phong
    // so it will be calculated inside ifdefs
    vec3 specfresnel;
    vec3 specref;
    if (model == PHONG)
    {
      // specular reflectance with PHONG
      specfresnel = fresnel_factor (specular, NdV);
      specref = phong_specular (V, L, N, specfresnel, mat.roughness);
    }
    else if (model == BLINN)
    {
      // specular reflectance with BLINN
      specfresnel = fresnel_factor (specular, HdV);
      specref = blinn_specular (NdH, specfresnel, mat.roughness);
    }
    else
    {
      // specular reflectance with COOK-TORRANCE
      specfresnel = fresnel_factor(specular, HdV);
      specref = cooktorrance_specular(model, NdL, NdV, NdH, specfresnel, mat.roughness);
    }

    specref *= vec3(NdL);

    // diffuse is common for any model
    vec3 diffref = (vec3(1.0) - specfresnel) * phong_diffuse() * NdL;

    // compute lighting
    vec3 reflected_light = vec3(0);
    vec3 diffuse_light = vec3(0);    // initial value == constant ambient light

    // point light
    vec3 light_color = light.intensity * A * I;
    reflected_light += specref * light_color;
    diffuse_light += diffref * light_color;

    // final result
    return diffuse_light * mix(mat.albedo, vec3(0.0), mat.metallic) + reflected_light;
  }

  vec3 Apply_fog (in vec3 lightColor)
  {
      //distance
    float dist = 0.0;
    float fogFactor = 0.0;

    //compute distance used in fog equations
    if (fog.based == 0)
    {
      //plane based
      dist = abs (surfacePosition.z);
    }
    else
    {
      //range based
      dist = length (surfacePosition);
    }

    if (fog.mode == 1) // linear fog
    {
      fogFactor = (fog.depth.x - dist)/(fog.depth.y - fog.depth.x);
    }
    else if (fog.mode == 2) // exponential fog
    {
      fogFactor = 1.0 / exp (dist * fog.density);
    }
    else
    {
      fogFactor = 1.0 / exp((dist * fog.density)* (dist * fog.density));
    }
    fogFactor = saturate (fogFactor);
    return mix (fog.color, lightColor, fogFactor);
  }

  float computeAO(vec3 N)
  {
    float ao = 0.5 + 0.5 * N.y;  // plage 0.0 -> 1.0
    ao = pow(ao, 1.0);           // gamma pour adoucir
    ao *= mat.ambient_occlusion;
    return saturate(ao);
  }

  void main ()
  {
    // Properties
    vec3 color;
    float alpha;
    if (lights_on == 0)
    {
      color = vec3(1.0);
      alpha = surfaceColor.w;
    }
    else
    {
     // mix between metal and non-metal material, for non-metal
     // constant base specular factor of 0.04 grey is used
      vec3 specular = mix(vec3(0.04), mat.albedo, mat.metallic);
      color = vec3(0.0);
      for(int i = 0; i < numLights; i++)
      {
        color +=  Apply_lighting_model (lights_on, AllLights[i], specular);
      }
      color = pow(color, vec3(1.0/mat.gamma));
      float ao = computeAO(surfaceNormal);
      color *= ao;
      alpha = surfaceColor.w * mat.alpha;
    }

    if (fog.mode > 0)
    {
      vec3 col = Apply_fog(surfaceColor.rgb*color);
      fragment_color = vec4 (col * alpha, alpha);
    }
    else
    {
      fragment_color = vec4 ((surfaceColor.rgb*color) * alpha, alpha);
    }
  }
);

/* ===========================================================================
 * Perfect-impostor shaders (plot -> ray_tracing).
 *
 * Principle: each primitive (sphere, cylinder, cone, cap) is rendered as a
 * camera-aligned billboard quad (4 vertices, GL_TRIANGLE_STRIP).
 * The fragment shader performs an exact analytic ray intersection
 * and additionally writes gl_FragDepth with the depth of the actual hit point,
 * giving pixel-perfect silhouettes AND correct depth-buffer occlusion between
 * overlapping atoms / bonds.
 *
 * Billboard proxy geometry (draw_billboard_quad, see d_atoms.c):
 *   vertices[0] = (-1,-1, 0)   GL_TRIANGLE_STRIP layout:
 *   vertices[1] = (-1,+1, 0)      tri 0: verts 0-1-2
 *   vertices[2] = (+1,-1, 0)      tri 1: verts 1-2-3
 *   vertices[3] = (+1,+1, 0)
 *   indices: 0 1 2 3
 *
 * Instance buffer layouts are IDENTICAL to the non-impostor counterparts
 * (ATOM_BUFF_SIZE, CYLI_BUFF_SIZE, CAPS_BUFF_SIZE) so no changes are needed
 * to the buffer-filling code in d_*.c.
 * ===========================================================================*/

/* --------------------------------------------------------------------------
 * Sphere impostor – vertex shader
 * Build a camera-aligned billboard quad that tightly covers the projected
 * sphere footprint.  All work is done in view space; m_proj is used for
 * the final clip-space position.
 * --------------------------------------------------------------------------*/
const GLchar * sphere_vertex_ray = GLSL(
  uniform mat4 m_view;
  uniform mat4 m_proj;
  uniform int view_is_ortho;

  in vec3 vert;       /* billboard corner: (-1,-1,0)…(+1,+1,0)          */
  in vec3 offset;     /* sphere center, world space (from instance data) */
  in float radius;    /* sphere radius  (from instance data)             */
  in vec4 vertColor;  /* RGBA color    (from instance data)             */

  out vec4 surfaceColor;
  out vec3 surfacePosition;
  out vec3 imp_a;             /* sphere center, view space */
  out vec3 imp_b;             /* unused for sphere – set to 0                */
  out float imp_r;            /* sphere radius (world-space units)           */
  flat out int   form_type;   /* 0 = sphere                               */
  flat out float clip_r_a;    /* sphere clip radius at imp_a endpoint (0 for non-cylinders) */
  flat out float clip_r_b;    /* sphere clip radius at imp_b endpoint (0 for non-cylinders) */

  void main ()
  {
    surfaceColor = vertColor;

    /* Sphere centre in view space */
    vec3 center_vs = vec3(m_view * vec4(offset, 1.0));

    /* View-space radius with scale correction */
    float r_vs = radius * length(mat3(m_view) * vec3(1.0, 0.0, 0.0));

    /* Billboard quad shifted forward to be in front of the sphere focal plane,
       and inflated by 2.0x to cover perspective distortion at screen edges. */
    // vec3 billboard_vs = center_vs + (2.0 * r_vs) * vec3(vert.x, vert.y, 1.0);

    float inflation = (view_is_ortho == 0) ? 1.05 : 2.0;
    vec3 billboard_vs = center_vs + (inflation * r_vs) * vec3(vert.x, vert.y, 1.0);

    surfacePosition = billboard_vs;

    imp_a      = center_vs;
    imp_b      = vec3(0.0);
    imp_r      = r_vs;
    form_type  = 0;
    clip_r_a   = 0.0;   /* spheres have no clipping */
    clip_r_b   = 0.0;
    gl_Position = m_proj * vec4(billboard_vs, 1.0);
    gl_Position.z = max(gl_Position.z, -gl_Position.w);
  }
);

/* --------------------------------------------------------------------------
 * Cylinder impostor – vertex shader
 * Build a tight axis-aligned bounding-box billboard from the two cylinder
 * endpoints (reconstructed from instance data).
 * --------------------------------------------------------------------------*/
const GLchar * cylinder_vertex_ray = GLSL(
  uniform mat4 m_view;
  uniform mat4 m_proj;
  uniform int  view_is_ortho;

  in vec4  quat;       /* rotation quaternion {w,x,y,z}                   */
  in float height;     /* full cylinder length (from instance data)       */
  in float radius;     /* cylinder radius                                 */
  in vec3 offset;      /* cylinder midpoint, world space                  */
  in vec3 vert;        /* billboard corner: (-1,-1,0)…(+1,+1,0)           */
  in vec4 vertColor;
  /* Sphere clip radii at the two cylinder endpoints (CYLI_BUFF_SIZE+2 layout).
     r_sphere_a = sphere radius at the atom-side endpoint (imp_a / local z=+0.5h).
     r_sphere_b = sphere radius at the midpoint-side endpoint (0.0 for half-bonds). */
  in float r_sphere_a;
  in float r_sphere_b;

  out vec4 surfaceColor;
  out vec3 surfacePosition;
  out vec3 imp_a;             /* endpoint p1, view space */
  out vec3 imp_b;             /* endpoint p2, view space */
  out float imp_r;            /* view-space radius       */
  flat out int   form_type;   /* 1 = cylinder            */
  flat out float clip_r_a;    /* passed through to fragment for endpoint clipping */
  flat out float clip_r_b;

  vec3 rotate_this (in vec3 v, in vec4 q)
  {
    vec3 u = vec3(q.x, q.y, q.z);
    float s = q.w;
    return 2.0 * dot(u, v) * u + (s*s - dot(u, u)) * v + 2.0 * s * cross(u, v);
  }

  void main ()
  {
    surfaceColor = vertColor;

    /* Reconstruct world-space endpoints from instance data */
    vec3 local_p1 = vec3(0.0, 0.0,  0.5 * height);
    vec3 local_p2 = vec3(0.0, 0.0, -0.5 * height);
    if (quat.w != 0.0)
    {
      local_p1 = rotate_this(local_p1, quat);
      local_p2 = rotate_this(local_p2, quat);
    }
    vec3 p1_vs = vec3(m_view * vec4(offset + local_p1, 1.0));
    vec3 p2_vs = vec3(m_view * vec4(offset + local_p2, 1.0));

    /* View-space radius with scale correction */
    float r_vs = radius * length(mat3(m_view) * vec3(1.0, 0.0, 0.0));

    /* Dual-endpoint perspective-aware inflation */
    float inflation = (view_is_ortho == 0) ? 1.05 : 2.0;
    float r_vs_bb = inflation * r_vs;

    /* View-space AABB of both endpoint discs */
    float xmin = min(p1_vs.x, p2_vs.x) - r_vs_bb;
    float xmax = max(p1_vs.x, p2_vs.x) + r_vs_bb;
    float ymin = min(p1_vs.y, p2_vs.y) - r_vs_bb;
    float ymax = max(p1_vs.y, p2_vs.y) + r_vs_bb;
    float z_bb = max(p1_vs.z, p2_vs.z) + r_vs_bb;

    float bx = mix(xmin, xmax, (vert.x + 1.0) * 0.5);
    float by = mix(ymin, ymax, (vert.y + 1.0) * 0.5);
    vec3 billboard_vs = vec3(bx, by, z_bb);

    surfacePosition = billboard_vs;

    imp_a     = p1_vs;
    imp_b     = p2_vs;
    imp_r     = r_vs;
    form_type = 1;
    /* Pass sphere clip radii to the fragment shader.
       clip_r_a is scaled to view space the same way as imp_r.  */
    float r_scale = length(mat3(m_view) * vec3(1.0, 0.0, 0.0));
    clip_r_a  = r_sphere_a * r_scale;
    clip_r_b  = r_sphere_b * r_scale;

    gl_Position = m_proj * vec4(billboard_vs, 1.0);
    gl_Position.z = max(gl_Position.z, -gl_Position.w);
  }
);

/* --------------------------------------------------------------------------
 * Cone impostor – vertex shader
 * Same as cylinder impostor but accounts for the asymmetric radii:
 * ra = 0 at p1 (apex, local z = +0.5·height), rb = radius at p2 (base).
 * --------------------------------------------------------------------------*/
const GLchar * cone_vertex_ray = GLSL(
  uniform mat4 m_view;
  uniform mat4 m_proj;
  uniform int  view_is_ortho;

  in vec4 quat;
  in float height;
  in float radius;   /* base radius; apex radius is 0 */
  in vec3 offset;
  in vec3 vert;
  in vec4 vertColor;

  out vec4 surfaceColor;
  out vec3 surfacePosition;
  out vec3 imp_a;             /* apex, view space  */
  out vec3 imp_b;             /* base center, view space */
  out float imp_r;            /* base radius, view space */
  flat out int   form_type;   /* 3 = cone          */
  flat out float clip_r_a;    /* 0.0 – cones are arrowheads, no sphere junction */
  flat out float clip_r_b;

  vec3 rotate_this (in vec3 v, in vec4 q)
  {
    vec3 u = vec3(q.x, q.y, q.z);
    float s = q.w;
    return 2.0 * dot(u, v) * u + (s*s - dot(u, u)) * v + 2.0 * s * cross(u, v);
  }

  void main ()
  {
    surfaceColor = vertColor;

    /* p1 = apex (ra=0, local z=+0.5h), p2 = base (rb=radius, local z=-0.5h) */
    vec3 local_apex = vec3(0.0, 0.0,  0.5 * height);
    vec3 local_base = vec3(0.0, 0.0, -0.5 * height);
    if (quat.w != 0.0)
    {
      local_apex = rotate_this(local_apex, quat);
      local_base = rotate_this(local_base, quat);
    }
    vec3 p_apex_vs = vec3(m_view * vec4(offset + local_apex, 1.0));
    vec3 p_base_vs = vec3(m_view * vec4(offset + local_base, 1.0));

    float r_vs = radius * length(mat3(m_view) * vec3(1.0, 0.0, 0.0));

    float inflation = (view_is_ortho == 0) ? 1.05 : 2.0;
    float r_vs_bb = inflation * r_vs;

    /* Asymmetric AABB: apex contributes radius 0, base contributes r_vs_bb */
    float xmin = min(p_apex_vs.x, p_base_vs.x - r_vs_bb);
    float xmax = max(p_apex_vs.x, p_base_vs.x + r_vs_bb);
    float ymin = min(p_apex_vs.y, p_base_vs.y - r_vs_bb);
    float ymax = max(p_apex_vs.y, p_base_vs.y + r_vs_bb);
    /* z: front face = closest of apex/base z + base radius (conservative bound). */
    float z_bb = max(p_apex_vs.z, p_base_vs.z) + r_vs_bb;

    // float bx = (vert.x > 0.0) ? xmax : xmin;
    // float by = (vert.y > 0.0) ? ymax : ymin;
    float bx = mix(xmin, xmax, (vert.x + 1.0) * 0.5);
    float by = mix(ymin, ymax, (vert.y + 1.0) * 0.5);
    vec3 billboard_vs = vec3(bx, by, z_bb);

    surfacePosition = billboard_vs;

    /* imp_a = apex, imp_b = base centre (matches intersect_cone convention) */
    imp_a     = p_apex_vs;
    imp_b     = p_base_vs;
    imp_r     = r_vs;
    form_type = 3;
    clip_r_a  = 0.0;
    clip_r_b  = 0.0;

    gl_Position = m_proj * vec4(billboard_vs, 1.0);
    gl_Position.z = max(gl_Position.z, -gl_Position.w);
  }
);

/* --------------------------------------------------------------------------
 * Cap impostor – vertex shader
 * A cap is a flat disk; its impostor is a square billboard centered on the
 * disk center with half-side = radius.
 * --------------------------------------------------------------------------*/
const GLchar * cap_vertex_ray = GLSL(
  uniform mat4 m_view;
  uniform mat4 m_proj;
  uniform int  view_is_ortho;

  in vec4 quat;
  in float radius;
  in vec3 offset;   /* cap center, world space  */
  in vec3 vert;     /* billboard corner         */
  in vec4 vertColor;

  out vec4 surfaceColor;
  out vec3 surfacePosition;
  out vec3 imp_a;             /* cap center, view space  */
  out vec3 imp_b;             /* cap normal, view space  */
  out float imp_r;            /* cap radius              */
  flat out int   form_type;   /* 2 = cap                 */
  flat out float clip_r_a;    /* 0.0 – caps are at midpoints, no sphere junction */
  flat out float clip_r_b;

  vec3 rotate_this (in vec3 v, in vec4 q)
  {
    vec3 u = vec3(q.x, q.y, q.z);
    float s = q.w;
    return 2.0 * dot(u, v) * u + (s*s - dot(u, u)) * v + 2.0 * s * cross(u, v);
  }

  void main ()
  {
    surfaceColor = vertColor;
    vec3 center_vs = vec3(m_view * vec4(offset, 1.0));

    /* Normal of the disk in view space */
    vec3 norm_local = vec3(0.0, 0.0, -1.0);
    if (quat.w != 0.0) norm_local = rotate_this(norm_local, quat);
    vec3 norm_vs = normalize (mat3(m_view) * norm_local);

    float r_vs = radius * length(mat3(m_view) * vec3(1.0, 0.0, 0.0));
    float inflation = (view_is_ortho == 0) ? 1.05 : 2.0;

    /* Camera-aligned square billboard, inflated for robustness. */
    vec3 billboard_vs = center_vs + (inflation * r_vs) * vec3(vert.x, vert.y, 1.0);

    surfacePosition = billboard_vs;

    imp_a     = center_vs;
    imp_b     = norm_vs;
    imp_r     = r_vs;
    form_type = 2;
    clip_r_a  = 0.0;
    clip_r_b  = 0.0;

    gl_Position = m_proj * vec4(billboard_vs, 1.0);
    gl_Position.z = max(gl_Position.z, -gl_Position.w);
  }
);

/* --------------------------------------------------------------------------
 * Perfect-impostor fragment shader.
 * --------------------------------------------------------------------------*/
const GLchar * full_color_ray = GLSL(

  int PHONG           = 1;
  int BLINN           = 2;
  int COOK_BLINN      = 3;
  int COOK_BECKMANN   = 4;
  int COOK_GGX        = 5;

  struct Light {
    int type;
    vec3 position;
    vec3 direction;
    vec3 intensity;
    float constant;
    float linear;
    float quadratic;
    float cone_angle;
    float spot_inner;
    float spot_outer;
  };

  struct Material {
    vec3 albedo;
    float metallic;
    float roughness;
    float ambient_occlusion;
    float gamma;
    float alpha;
  };

  struct Fog {
    int mode;
    int based;
    float density;
    vec2 depth;
    vec3 color;
  };

  struct Hit {
    vec3 pos;
    vec3 normal;
  };

  uniform Light AllLights[10];
  uniform Material mat;
  uniform Fog fog;
  uniform int lights_on;
  uniform int numLights;
  uniform int view_is_ortho;
  uniform mat4 m_proj;

  in vec4  surfaceColor;
  in vec3  surfacePosition;

  // Standardized Raytracing parameters
  in vec3  imp_a;
  in vec3  imp_b;
  in float imp_r;
  flat in int   form_type;
  /* Sphere clip radii at the two cylinder endpoints (passed from cylinder_vertex_ray).
     Non-zero only for cylinders (form_type==1).  When a cylinder hit lies inside the
     sphere at either endpoint, the fragment is discarded — the sphere impostor handles
     that region.  This eliminates z-fighting at sphere/cylinder junctions without any
     depth bias, which would break 3D depth perception.                              */
  flat in float clip_r_a;
  flat in float clip_r_b;

  out vec4 fragment_color;

  const float PI = 3.14159265359;
  const float EPS = 1e-5;

  // clamping to 0 - 1 range
  float saturate (in float value)
  {
    return clamp(value, 0.0, 1.0);
  }

  // phong (lambertian) diffuse term
  float phong_diffuse()
  {
    return (1.0 / PI);
  }

  // compute Fresnel specular factor for given base specular and product
  // product could be NdV or VdH depending on used technique
  vec3 fresnel_factor (in vec3 f0, in float product)
  {
    return mix(f0, vec3(1.0), pow(1.01 - product, 5.0));
  }

  // following functions are copies of UE4
  // for computing Cook-Torrance specular lighting terms

  float D_blinn(in float roughness, in float NdH)
  {
    float m = roughness * roughness;
    float m2 = m * m;
    float n = 2.0 / m2 - 2.0;
    return (n + 2.0) / (2.0 * PI) * pow(NdH, n);
  }

  float D_beckmann(in float roughness, in float NdH)
  {
    float m = roughness * roughness;
    float m2 = m * m;
    float NdH2 = NdH * NdH;
    return exp((NdH2 - 1.0) / (m2 * NdH2)) / (PI * m2 * NdH2 * NdH2);
  }

  float D_GGX(in float roughness, in float NdH)
  {
    float m = roughness * roughness;
    float m2 = m * m;
    float d = (NdH * m2 - NdH) * NdH + 1.0;
    return m2 / (PI * d * d);
  }

  float G_schlick(in float roughness, in float NdV, in float NdL)
  {
    float k = roughness * roughness * 0.5;
    float V = NdV * (1.0 - k) + k;
    float L = NdL * (1.0 - k) + k;
    return 0.25 / (V * L);
  }

  // simple Phong specular calculation with normalization
  vec3 phong_specular(in vec3 V, in vec3 L, in vec3 N, in vec3 specular, in float roughness)
  {
    vec3 R = reflect(-L, N);
    float spec = max(0.0, dot(V, R));

    float k = 1.999 / (roughness * roughness);

    return min(1.0, 3.0 * 0.0398 * k) * pow(spec, min(10000.0, k)) * specular;
  }

  // simple Blinn specular calculation with normalization
  vec3 blinn_specular(in float NdH, in vec3 specular, in float roughness)
  {
    float k = 1.999 / (roughness * roughness);

    return min(1.0, 3.0 * 0.0398 * k) * pow(NdH, min(10000.0, k)) * specular;
  }

  // Cook-Torrance specular calculation
  vec3 cooktorrance_specular (in int cook, in float NdL, in float NdV, in float NdH, in vec3 specular, in float roughness)
  {
    float D;
    if (cook == COOK_BLINN)
    {
      D = D_blinn(roughness, NdH);
    }
    else if (cook == COOK_BECKMANN)
    {
      D = D_beckmann(roughness, NdH);
    }
    else if (cook == COOK_GGX)
    {
      D = D_GGX(roughness, NdH);
    }

    float G = G_schlick(roughness, NdV, NdL);

    float rim = mix(1.0 - roughness * 0.9, 1.0, NdV);

    return (1.0 / rim) * specular * G * D;
  }

  vec3 Apply_lighting_model (in int model, in Light light, in vec3 specular, in vec3 v_pos, in vec3 N)
  {
    // L, V, H vectors
    vec3 L;
    float A;
    float I = 1.0;
    if (light.type == 0)
    {
      // Directional light
      L = normalize (-light.direction);
      A = 1.0;
    }
    else
    {
      L = light.position - v_pos;
      float dist = length (L);
      L = normalize(L);
      A = 1.0 / (light.constant + light.linear*dist + light.quadratic*dist*dist);
      if (light.type == 2)
      {
        float theta = dot(L, normalize(light.position-light.direction));
        if(theta > light.cone_angle)
        {
          float epsilon = light.spot_inner - light.spot_outer;
          I = saturate((theta - light.spot_outer) / epsilon);
        }
        else
        {
          return vec3(0.0001);
        }
      }
    }
    vec3 V = normalize(-v_pos);
    vec3 H = normalize(L + V);

    // compute material reflectance
    float NdL = max(0.0, dot(N, L));
    float NdV = max(0.001, dot(N, V));
    float NdH = max(0.001, dot(N, H));
    float HdV = max(0.001, dot(H, V));
    float LdV = max(0.001, dot(L, V));

    // Fresnel term is common for any, except Phong
    // so it will be calculated inside ifdefs
    vec3 specfresnel;
    vec3 specref;
    if (model == PHONG)
    {
      // specular reflectance with Phong
      specfresnel = fresnel_factor (specular, NdV);
      specref = phong_specular (V, L, N, specfresnel, mat.roughness);
    }
    else if (model == BLINN)
    {
      // specular reflectance with Blinn
      specfresnel = fresnel_factor (specular, HdV);
      specref = blinn_specular (NdH, specfresnel, mat.roughness);
    }
    else
    {
      // specular reflectance with Cook-Torrance
      specfresnel = fresnel_factor(specular, HdV);
      specref = cooktorrance_specular (model, NdL, NdV, NdH, specfresnel, mat.roughness);
    }

    specref *= vec3(NdL);

    // diffuse is common for any model
    vec3 diffref = (vec3(1.0) - specfresnel) * phong_diffuse() * NdL;

    // compute lighting
    vec3 reflected_light = vec3(0);
    vec3 diffuse_light = vec3(0);    // initial value == constant ambient light

    // point light
    vec3 light_color = light.intensity * A * I;
    reflected_light += specref * light_color;
    diffuse_light += diffref * light_color;

    // final result
    return diffuse_light * mix(mat.albedo, vec3(0.0), mat.metallic) + reflected_light;
  }

  vec3 Apply_fog (in vec3 lightColor, in vec3 v_pos, in vec3 normal)
  {
     // distance
    float dist = 0.0;
    float fogFactor = 0.0;

    // compute distance used in fog equations
    if (fog.based == 0)
    {
      // plane based
      dist = abs (v_pos.z);
    }
    else
    {
      // range based
      dist = length (v_pos);
    }

    if (fog.mode == 1) // linear fog
    {
      fogFactor = (fog.depth.x - dist)/(fog.depth.y - fog.depth.x);
    }
    else if (fog.mode == 2) // exponential fog
    {
      fogFactor = 1.0 / exp (dist * fog.density);
    }
    else
    {
      fogFactor = 1.0 / exp((dist * fog.density)* (dist * fog.density));
    }
    fogFactor = saturate (fogFactor);

    //float viewAngle = dot(normalize(-v_pos), normalize(normal));
    // fogFactor *= mix(0.7, 1.0, viewAngle);

    return mix (fog.color, lightColor, fogFactor);
  }

  bool intersect_sphere(vec3 ro, vec3 rd, vec3 c, float r, out Hit hit)
  {
    vec3 oc = ro - c;
    vec3 cr = cross(rd, oc);
    float d2 = dot(cr, cr);

    float r2 = r*r;
    if (d2 > r2) return false;

    float tca = -dot(oc, rd);
    float thc = sqrt(r2 - d2);

    float t = tca - thc;
    if (t < EPS) t = tca + thc;
    if (t < EPS) return false;

    hit.pos = ro + rd*t;
    hit.normal = normalize(hit.pos - c);
    return true;
  }

  // Numerically stable cylinder intersection via cross products.
  bool intersect_cylinder(vec3 ro, vec3 rd, vec3 pa, vec3 pb, float ra,  out Hit hit)
  {
    vec3 ba   = pb - pa;
    vec3 oc   = ro - pa;

    float baba = dot(ba, ba);
    float bard = dot(ba, rd);
    float baoc = dot(ba, oc);

    vec3 vb = cross(rd, ba);
    vec3 va = cross(oc, ba);

    float k2 = dot(vb, vb);
    if (k2 < 1e-10) return false; // Parallel to axis off-center

    float k1 = dot(va, vb);
    float k0 = dot(va, va) - ra * ra * baba;

    float h = k1 * k1 - k2 * k0;
    if (h < 0.0) return false;

    h = sqrt(h);
    float t = (-k1 - h) / k2;
    if (t < EPS) t = (-k1 + h) / k2;
    if (t < EPS) return false;

    float y = baoc + t * bard;
    if (y > -EPS && y < baba + EPS)
    {
      hit.pos  = ro + t * rd;
      hit.normal = normalize((hit.pos - pa) * baba - ba * y);
      return true;
    }
    return false;
  }

  bool intersect_cone(vec3 ro, vec3 rd, vec3 pa, vec3 pb, float ra, out Hit hit)
  {
    vec3 ba = pb - pa;
    vec3 oa = ro - pa;

    float m0 = dot(ba,ba);
    float m1 = dot(oa,ba);
    float m2 = dot(rd,ba);

    if (m0 < 1e-6) return false;

    float k = (ra*ra) / m0;
    float m = dot(rd,ba)/m0; // ~ cos angle of ray with axis
    float n = dot(oa,ba)/m0;
    float a = dot(rd,rd) - m2*m2*(1.0+k)/m0;
    float b = dot(rd,oa) - m2*m1*(1.0+k)/m0;
    float c = dot(oa,oa) - m1*m1*(1.0+k)/m0;

    float h = b*b - a*c;
    if (h < 0.0) return false;

    h = sqrt(h);

    float t1 = (-b - h)/a;
    float t2 = (-b + h)/a;
    float t = 1e20;

    if (t1 > EPS) t = t1;
    if (t2 > EPS && t2 < t) t = t2;

    if (t == 1e20) return false;

    float y = m1 + t*m2;

    if (y > -EPS && y < m0 + EPS)
    {
      hit.pos = ro + t * rd;
      hit.normal = normalize(m0*(hit.pos-pa) - ba*(1.0+k)*y);
      return true;
    }
    return false;
  }

  bool intersect_cap(vec3 ro, vec3 rd, vec3 center, vec3 normal, float radius, out Hit hit)
  {
    float denom = dot(normal, rd);
    if (abs(denom) < 1e-6) return false;

    float t = dot(center - ro, normal) / denom;
    if (t < EPS) return false;

    vec3 p = ro + t * rd;
    vec3 v = p - center;
    if (dot(v, v) > radius * radius) return false;

    hit.pos = p;
    hit.normal = normal;
    // Orient normal towards ray
    if (dot(hit.normal, rd) > 0.0) hit.normal = -hit.normal;

    return true;
  }

  bool intersect_scene(vec3 ro, vec3 rd, out Hit hit)
  {
    if (form_type == 0) // Sphere
    {
      return intersect_sphere(ro, rd, imp_a, imp_r, hit);
    }
    else if (form_type == 1) // Cylinder
    {
      return intersect_cylinder(ro, rd, imp_a, imp_b, imp_r, hit);
    }
    else if (form_type == 2) // Cap
    {
      return intersect_cap(ro, rd, imp_a, imp_b, imp_r, hit);
    }
    else if (form_type == 3) // Cone
    {
      return intersect_cone(ro, rd, imp_a, imp_b, imp_r, hit);
    }
    return false;
  }

  float computeAO(vec3 pos, vec3 N)
  {
    const int AO_SAMPLES = 16;
    const float EPS_AO = 0.001;

    float maxDist = imp_r * 8.0;
    float occlusion = 0.0;
    vec3 tangent = normalize(abs(N.x) > 0.5 ? cross(N, vec3(0,1,0)) : cross(N, vec3(1,0,0)));
    vec3 bitangent = cross(N, tangent);

    for(int i = 0; i < AO_SAMPLES; i++)
    {
      float u = float(i)/float(AO_SAMPLES);
      float v = fract(sin(float(i)*91.345) * 47453.545);

      float phi = 2.0 * PI * v;
      float cosTheta = sqrt(1.0 - u);
      float sinTheta = sqrt(u);
      vec3 dir = vec3(cos(phi) * sinTheta,  sin(phi) * sinTheta, cosTheta);

      // passage dans l’espace du tangent
      dir = normalize(dir.x*tangent + dir.y*bitangent + dir.z*N);

      Hit tmp;
      if(intersect_scene(pos + N * EPS_AO, dir, tmp))
      {
        float dist = length(tmp.pos - pos);
        // float weight = 1.0 - saturate(dist / maxDist);
        float weight = exp(-dist * 2.0 / maxDist);
        occlusion += weight;
      }
    }

    float ao = 1.0 - (occlusion / float(AO_SAMPLES));
    ao = pow(ao, mat.gamma); // softening
    // Safety clamp between 0.0 and 1.0
    return saturate(ao);
  }

  float ray_computeAO(vec3 N)
  {
    // Up orientation (Y is the up vector)
    float ao = 0.5 + 0.5 * N.y;

    // Softening
    ao = pow(ao, mat.gamma);

    // Safety clamp between 0.0 and 1.0
    return saturate(ao);
  }

  void main ()
  {
    /* Ray from camera origin through billboard fragment (view space) */
    vec3 surfaceToCamera = normalize(-surfacePosition);
    vec3 ray_origin = surfacePosition;
    vec3 ray_dir;
    if (view_is_ortho == 0)
    {
      ray_dir = vec3(0.0, 0.0, -1.0); // Parallel rays (Orthographic)
    }
    else
    {
      ray_dir = normalize(surfacePosition);   // Perspective
    }
    Hit the_hit;
    bool hit = false;

    hit = intersect_scene (ray_origin, ray_dir, the_hit);

    if (! hit) discard;
    if (dot(the_hit.normal, surfaceToCamera) < 0.0) the_hit.normal = -the_hit.normal;

    /* Sphere-entry clipping for cylinders (form_type == 1):
       discard cylinder pixels that lie inside the sphere at each endpoint,
       so the sphere impostor has exclusive ownership of those pixels.
       This eliminates z-fighting at sphere/cylinder junctions analytically,
       with no depth bias that would break 3D depth perception. */
    const float CLIP_MARGIN = 1.00f; // Optional extra margin
    if (clip_r_a > 0.0)
    {
      float r_clip = clip_r_a * CLIP_MARGIN;
      if (dot(the_hit.pos - imp_a, the_hit.pos - imp_a) < r_clip * r_clip) discard;
    }
    if (clip_r_b > 0.0)
    {
      float r_clip = clip_r_b * CLIP_MARGIN;
      if (dot(the_hit.pos - imp_b, the_hit.pos - imp_b) < r_clip * r_clip) discard;
    }

    vec4 hit_clip = m_proj * vec4(the_hit.pos, 1.0);
    if (hit_clip.w <= 0.0) discard;
    gl_FragDepth = clamp((hit_clip.z / hit_clip.w + 1.0) * 0.5, 0.0, 1.0);

    /* Lighting */
    vec3 color;
    float alpha;
    if (lights_on == 0)
    {
      color = vec3(1.0);
      alpha = surfaceColor.w;
    }
    else
    {
      // mix between metal and non-metal material, for non-metal
     // constant base specular factor of 0.04 grey is used
      vec3 specular = mix(vec3(0.04), mat.albedo, mat.metallic);
      color = vec3(0.0);
      vec3 diffuse = vec3(0.0);
      int i;
      for(i = 0; i < numLights; i++)
      {
        diffuse += Apply_lighting_model (lights_on, AllLights[i], specular, the_hit.pos, the_hit.normal);
      }
      float ao = computeAO (the_hit.pos, the_hit.normal) * ray_computeAO (the_hit.normal);

      diffuse *= (ao * mat.ambient_occlusion);
      vec3 lit_color = pow(diffuse,vec3(1.0/mat.gamma));
      color = surfaceColor.rgb*lit_color;
      alpha = surfaceColor.w*mat.alpha;
    }

    if (fog.mode > 0)
    {
      vec3 col = Apply_fog(color, the_hit.pos, the_hit.normal);
      fragment_color = vec4 (col * alpha, alpha);
    }
    else
    {
      fragment_color = vec4 (color * alpha, alpha);
    }
  }
);

/* ===========================================================================
 * End of perfect-impostor shaders
 * ===========================================================================*/

const GLchar * angstrom_vertex = GLSL(
  uniform mat4 mvp;
  uniform mat4 un_view;
  uniform mat4 text_proj;
  uniform vec4 viewp;
  uniform vec4 pos_shift;
  uniform int tilted;

  in vec2 vert;
  in vec2 tcoord;
  in vec3 offset;
  in vec3 at_a;
  in vec3 at_b;

  out vec2 text_coords;

  float angle2d (in vec2 at, in vec2 bt, in vec2 ct)
  {
    vec2 ab = bt - at;
    vec2 bc = bt - ct;
    float theta = dot(ab,bc) / (length(ab) * length(bc));
    if (theta < -1.0)
    {
      return acos (-2.0 - theta);
    }
    else if (theta > 1.0)
    {
      return acos (2.0 - theta);
    }
    else
    {
      return acos (theta);
    }
  }

  mat4 rotate_this_z (in float theta)
  {
    return mat4 ( vec4( cos(theta),  sin(theta),  0.0, 0.0),
                  vec4(-sin(theta),  cos(theta),  0.0, 0.0),
                  vec4(        0.0,         0.0,  1.0, 0.0),
                  vec4(        0.0,         0.0,  0.0, 1.0));
  }

  mat4 translate_this (in vec3 coord)
  {
    mat4 translate;
    translate[0] = vec4(1.0, 0.0, 0.0, 0.0);
    translate[1] = vec4(0.0, 1.0, 0.0, 0.0);
    translate[2] = vec4(0.0, 0.0, 1.0, 0.0);
    translate[3][0] = coord.x;
    translate[3][1] = coord.y;
    translate[3][2] = coord.z;
    translate[3][3] = 1.0;

    return translate;
  }

  vec4 project (in vec3 coord, vec4 shift)
  {
    mat4 n_mvp = mvp * translate_this (coord) * un_view;
    vec4 res = n_mvp * vec4(vec3(0.0), 1.0);
    res = translate_this (vec3(shift.x/viewp.z, shift.y/viewp.w, shift.z)) * res;
    if (res.w != 0.0)
    {
      res.w = 1.0 / res.w;
      res.x = res.w * res.x + 1.0;
      res.y = res.w * res.y + 1.0;
      res.z = res.w * res.z + 1.0;
      return vec4 (res.x*viewp.z+viewp.x, res.y*viewp.w+viewp.y, shift.w*res.z, 1.0);
    }
    else
    {
      return vec4 (0.0, 0.0, -1.0, 0.0);
    }
  }

  void main()
  {
    text_coords = tcoord;
    float rot_angle = 0.0;
    vec4 shift = pos_shift;
    if (tilted > 0)
    {
      vec3 pos_a = project (at_a, vec4(0.0)).xyz;
      vec3 pos_b = project (at_b, vec4(0.0)).xyz;
      vec2 pa;
      vec2 pb;
      vec2 pc;
      pa.y = max(pos_a.y, pos_b.y);
      if (pa.y == pos_a.y)
      {
        pa.x = pos_a.x;
        pb.x = pos_b.x;
        pb.y = pos_b.y;
      }
      else
      {
        pa.x = pos_b.x;
        pb.x = pos_a.x;
        pb.y = pos_a.y;
      }
      pc.x = pa.x;
      pc.y = pb.y;
      rot_angle = - angle2d (pa, pb, pc);
      if (pa.x < pb.x)
      {
        rot_angle = -rot_angle;
        shift.x = -shift.x;
      }
    }
    vec4 pos;
    if (rot_angle != 0.0)
    {
      pos = project(offset, shift) + vec4(vert, 0.0, 1.0) * rotate_this_z(rot_angle);
    }
    else
    {
      pos = project(offset, shift) + vec4(vert, 0.0, 1.0);
    }
    gl_Position =  text_proj * pos;
  }
);

const GLchar * degree_vertex = GLSL(
  uniform mat4 mvp;
  uniform mat4 un_view;
  uniform mat4 text_proj;
  uniform vec4 viewp;
  uniform vec4 pos_shift;
  uniform int tilted;

  in vec2 vert;
  in vec2 tcoord;
  in vec3 offset;
  in vec3 at_a;
  in vec3 at_b;
  in vec3 at_c;

  const float PI = 3.14159265359;

  out vec2 text_coords;

  float angle2d (in vec2 at, in vec2 bt, in vec2 ct)
  {
    vec2 ab = bt - at;
    vec2 bc = bt - ct;
    float theta = dot(ab,bc) / (length(ab) * length(bc));
    if (theta < -1.0)
    {
      return acos (-2.0 - theta);
    }
    else if (theta > 1.0)
    {
      return acos (2.0 - theta);
    }
    else
    {
      return acos (theta);
    }
  }

  mat4 rotate_this_z (in float theta)
  {
    return mat4 ( vec4( cos(theta),  sin(theta),  0.0, 0.0),
                  vec4(-sin(theta),  cos(theta),  0.0, 0.0),
                  vec4(        0.0,         0.0,  1.0, 0.0),
                  vec4(        0.0,         0.0,  0.0, 1.0));
  }

  mat4 translate_this (in vec3 coord)
  {
    mat4 translate;
    translate[0] = vec4(1.0, 0.0, 0.0, 0.0);
    translate[1] = vec4(0.0, 1.0, 0.0, 0.0);
    translate[2] = vec4(0.0, 0.0, 1.0, 0.0);
    translate[3][0] = coord.x;
    translate[3][1] = coord.y;
    translate[3][2] = coord.z;
    translate[3][3] = 1.0;

    return translate;
  }

  vec4 project (in vec3 coord, vec4 shift)
  {
    mat4 n_mvp = mvp * translate_this (coord) * un_view;
    vec4 res = n_mvp * vec4(vec3(0.0), 1.0);
    res = translate_this (vec3(shift.x/viewp.z, shift.y/viewp.w, shift.z)) * res;
    if (res.w != 0.0)
    {
      res.w = 1.0 / res.w;
      res.x = res.w * res.x + 1.0;
      res.y = res.w * res.y + 1.0;
      res.z = res.w * res.z + 1.0;
      return vec4 (res.x*viewp.z+viewp.x, res.y*viewp.w+viewp.y, shift.w*res.z, 1.0);
    }
    else
    {
      return vec4 (0.0, 0.0, -1.0, 0.0);
    }
  }

  void main()
  {
    text_coords = tcoord;
    float rot_angle = 0.0;
    vec4 shift = pos_shift;
    vec3 pos_a = project (at_a, vec4(0.0)).xyz;
    vec3 pos_b = project (at_b, vec4(0.0)).xyz;
    vec3 pos_c = project (at_c, vec4(0.0)).xyz;
    vec2 pa = pos_a.xy;
    vec2 pb = pos_b.xy;
    vec2 pc = pos_c.xy;
    vec2 pd;
    float theta = angle2d (pa, pb, pc);
    pd.x = pb.x + 100.0;
    pd.y = pb.y;
    float alpha;
    float beta;
    float gamma;
    vec2 sign;
    sign.x = 1.0;
    sign.y = 1.0;

    beta = angle2d (pa, pb, pd);
    alpha = angle2d (pc, pb, pd);
    if (pa.y > pb.y && pc.y > pb.y)
    {
      gamma = min (alpha, beta);
    }
    else if (pa.y < pb.y && pc.y < pb.y)
    {
      gamma = min (-alpha, -beta);
    }
    else
    {
      vec2 pe;
      vec2 pf;
      pe.y = max(pa.y, pc.y);
      if (pe.y == pa.y)
      {
        pe.x = pa.x;
        pf.y = pc.y;
        pf.x = pc.x;
      }
      else
      {
        pe.x = pc.x;
        pf.y = pa.y;
        pf.x = pa.x;
      }
      beta = angle2d (pe, pb, pd);
      gamma = beta;
      alpha = angle2d (pf, pb, pd);
      if (beta + alpha < PI)
      {
        gamma -= theta;
      }
    }
    rot_angle = PI/2.0 - gamma - theta/2.0;
    vec3 a = at_b + ((at_a - at_b)/3.0 + (at_c - at_b)/3.0)/2.0;
    vec4 b = project (a, shift);
    float dist = min(length(pb-pa), length(pb-pc)) / 3.0;
    float x = pb.x + (shift.x+dist) * sin(rot_angle);
    float y = pb.y + (shift.y+dist) * cos(rot_angle);
    if (pa.y < pb.y && pc.y < pb.y)
    {
      gamma = - max (alpha, beta);
    }
    else if (pa.y > pb.y || pc.y > pb.y)
    {
      gamma += PI;
    }
    rot_angle = PI/2.0 - gamma - theta/2.0;
    vec3 c = vec3(x, y, b.z);
    vec4 pos = vec4(c, 1.0);
    if (tilted > 0)
    {
      rot_angle += PI;
      pos += vec4(vert, 0.0, 1.0) * rotate_this_z(rot_angle);
    }
    else
    {
      pos += vec4(vert, 0.0, 1.0);
    }
    gl_Position =  text_proj * pos;
  }
);

const GLchar * string_color = GLSL(
  uniform sampler2DRect tex;
  uniform vec4 vert_color;
  uniform vec4 viewp;
  uniform int tilted;
  in vec2 text_coords;

  out vec4 fragment_color;
  void main()
  {
    vec2 coords = text_coords;
    vec4 color = vert_color * vec4(1.0, 1.0, 1.0, texture (tex, text_coords).r);
    fragment_color = vec4(color.rgb * color.a, color.a);
  }
);

const GLchar * string_color_2d = GLSL(
  uniform sampler2DRect tex;
  uniform vec4 vert_color;
  in vec2 text_coords;

  out vec4 fragment_color;
  void main()
  {
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture (tex, text_coords).r);
    vec4 color = vert_color * sampled;
    fragment_color = vec4(color.rgb * color.a, color.a);
  }
);

const GLchar * background_vertex = GLSL(
  in vec2 vert;

  out vec2 fragment_coord;
  void main ()
  {
     fragment_coord = 0.5*vert + 0.5;
     gl_Position = vec4(vert, 0.0, 1.0);
  }
);

const GLchar * background_linear = GLSL(
  in vec2 fragment_coord;
  uniform vec4 first_color;
  uniform vec4 second_color;
  uniform int gradient;
  uniform float position;

  out vec4 fragment_color;
  void main ()
  {
    float horizontal = 1.0 - fragment_coord.x;
    float vertical = fragment_coord.y;
    float factor;
    switch (gradient)
    {
      case 1:
        // Linear right to left
        factor = clamp ((horizontal - position) * 2.0 + 0.5, 0.0, 1.0);
        break;
      case 2:
        // Linear bottom right to top left
         factor = clamp ((0.5*(horizontal + vertical) - position) * 2.0 + 0.5, 0.0, 1.0);
        break;
      case 3:
        // Linear bottom left to top right
        factor = clamp ((0.5*(fragment_coord.x + vertical) - position) * 2.0 + 0.5, 0.0, 1.0);
        break;
      default:
        // Linear top to bottom
        factor = clamp ((vertical - position) * 2.0 + 0.5, 0.0, 1.0);
        break;
    }
    fragment_color = mix(first_color, second_color, factor);
  }
);

const GLchar * background_circular = GLSL(
  in vec2 fragment_coord;
  uniform vec4 first_color;
  uniform vec4 second_color;
  uniform int gradient;
  uniform float position;

  out vec4 fragment_color;
  void main ()
  {
    float dist;
    float factor;
    vec2 center;
    switch (gradient)
    {
      case 0:
        // Circular right to left
        center = vec2 (0.0, 0.5); // left side centered vertically
        dist = distance (fragment_coord, center) + position - 0.5;
        // sqrt((1.0 - 0.0)^2 + (1.0 - 0.5)^2) ~ 1.118
        factor = clamp (dist / 1.118, 0.0, 1.0);
        break;
      case 1:
        // Circular left to right
        center = vec2 (1.0, 0.5); // right side centered vertically
        dist = distance (fragment_coord, center)  + position - 0.5;
        factor = clamp (dist / 1.118, 0.0, 1.0);
        break;
      case 2:
        // Top to bottom
        center = vec2 (0.5, 1.0); // right side centered vertically
        dist = distance (fragment_coord, center)  + position - 0.5;
        factor = clamp (dist / 1.118, 0.0, 1.0);
        break;
      case 3:
        // Bottom to top
        center = vec2 (0.5, 0.0); // right side centered vertically
        dist = distance (fragment_coord, center)  + position - 0.5;
        factor = clamp (dist / 1.118, 0.0, 1.0);
        break;
      case 4:
        // Circular bottom right to top left
        center = vec2 (1.0, 0.0); // bottom right corner
        dist = distance (fragment_coord, center)  + position - 0.5;
        factor = clamp (dist / 1.4142, 0.0, 1.0);
        break;
      case 5:
        // Circular bottom left to top right
        center = vec2 (0.0, 0.0); // bottom right corner
        dist = distance (fragment_coord, center)  + position - 0.5;
        factor = clamp (dist / 1.4142, 0.0, 1.0);
        break;
      case 6:
        // Circular top right to bottom left
        center = vec2 (1.0, 1.0); // top right corner
        dist = distance (fragment_coord, center)  + position - 0.5;
        factor = clamp (dist / 1.4142, 0.0, 1.0);
        break;
      case 7:
        // Circular top left to bottom right
        center = vec2 (0.0, 1.0); // top left corner
        dist = distance (fragment_coord, center)  + position - 0.5;
        factor = clamp (dist / 1.4142, 0.0, 1.0);
        break;
      default:
        center = vec2 (0.5, 0.5); // from the center
        dist = distance (fragment_coord, center)  + position - 0.5;
        factor = clamp (dist / 0.707, 0.0, 1.0);
        break;
    }
    fragment_color = mix (first_color, second_color, factor);
  }
);
