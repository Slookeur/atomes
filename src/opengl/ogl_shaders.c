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
    /*if(dot(gl_PointCoord-0.5,gl_PointCoord-0.5)>0.25)
    {
      discard;
    }
    else
    {*/
      fragment_color = vert_color;
    //}
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

const GLchar * axis_line_vertex = GLSL(
  uniform mat4 mvp;
  uniform vec4 vertColor;
  in vec3 vert;

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
    float back_light;
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

    float rim = mix(1.0 - roughness * mat.back_light * 0.9, 1.0, NdV);

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
      vec3 L = light.position - v_pos;
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
      alpha = surfaceColor.w * mat.alpha;
    }
    if (fog.mode > 0)
    {
      fragment_color = vec4 (Apply_fog(surfaceColor.xyz*color), alpha);
    }
    else
    {
      fragment_color = vec4 (surfaceColor.xyz*color, alpha);
    }
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

const GLchar * axis_sphere_vertex = GLSL(
  uniform mat4 mvp;
  uniform mat4 m_view;
  uniform vec4 vertColor;

  in vec3 vert;
  in vec3 vertNormal;

  out vec4 surfaceColor;
  out vec3 surfacePosition;
  out vec3 surfaceNormal;
  out vec3 surfaceToCamera;
  void main ()
  {
    surfaceColor = vertColor;
    vec4 pos = vec4(vert, 1.0);
    surfacePosition = vec3(m_view * pos);
    surfaceNormal   = mat3(m_view) * vertNormal;
    surfaceToCamera = normalize (- surfacePosition);
    gl_PointSize = 1.0;
    gl_Position = mvp * pos;
  }
);

// Cylinder

const GLchar * gs_cylinder_vertex = GLSL(
  in vec3 vert;
  in vec4 vertColor;
  out vec4 vertCol;
  void main ()
  {
    vertCol  = vertColor;
    gl_Position = vec4(vert, 1.0);
  }
);

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

// Axis cylinder

const GLchar * axis_cylinder_geom = GLSL(

  layout (lines) in;
  layout (triangle_strip, max_vertices=64) out;

  uniform mat4 mvp;
  uniform mat4 m_view;
  uniform float radius;

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
    float r1 = gl_in[0].gl_Position.w;
    float r2 = gl_in[1].gl_Position.w;
    vec3 axis = normalize(v2 - v1);
    vec3 perp_x = create_perp (axis);
    vec3 perp_y = cross (axis, perp_x);
    float step = 2.0 * pi / float(32 - 1);
    for(int i=0; i<32; i++)
    {
      float a = i * step;
      float ca = cos(a);
      float sa = sin(a);

      vec3 normal = normalize(ca*perp_x + sa*perp_y);
      vec3 p1 = v1 + r1 * normal;
      vec3 p2 = v2 + r2 * normal;

      surfaceNormal =  mat3(m_view) * normal;
      gl_Position = mvp * vec4(p1, 1.0);

      surfacePosition = vec3(m_view * vec4(p1, 1.0));
      surfaceToCamera = normalize (- surfacePosition);
      EmitVertex();

      gl_Position = mvp * vec4 (p2, 1.0);
      surfacePosition = vec3(m_view * vec4(p2, 1.0));
      surfaceToCamera = normalize (- surfacePosition);
      EmitVertex();
    }
    EndPrimitive();
  }
);

// Tetrahedra

const GLchar * polyedron_vertex = GLSL(
  in vec4 vert;
  in vec4 vertColor;

  out vec4 surfaceColor;
  void main ()
  {
    surfaceColor = vertColor;
    gl_Position = vert;
  }
);

const GLchar * polyedron_geom = GLSL(

  layout (triangles) in;
  layout (triangle_strip, max_vertices=3) out;

  uniform mat4 mvp;
  uniform mat4 m_view;

  out vec3 surfacePosition;
  out vec3 surfaceNormal;
  out vec3 surfaceToCamera;

  float pi = 3.141592653;

  vec3 get_triangle_normal (in vec3 v1, in vec3 v2, in vec3 v3)
  {
    vec3 edge_a = v3 - v1;
    vec3 edge_b = v2 - v1;
    return normalize(cross(edge_a, edge_b));
  }

  void main()
  {
    vec3 v[3];
    int i;
    for (i=0; i<3; i++)
    {
      v[i] = gl_in[i].gl_Position.xyz;
    }
    float sign = gl_in[0].gl_Position.w;
    vec3 normal = sign * get_triangle_normal(v[0], v[1], v[2]);
    surfaceNormal = mat3(m_view) * normal;
    for (i=0; i<3; i++)
    {
      gl_Position = mvp * vec4(v[i], 1.0);
      surfacePosition = vec3(m_view * vec4(v[i], 1.0));
      surfaceToCamera = normalize (- surfacePosition);
      EmitVertex();
    }
    EndPrimitive();
  }
);

const GLchar * polyedron_color = GLSL(

  int PHONG           = 1;
  int BLINN           = 2;
  int COOK_TORRANCE   = 3;
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
    float back_light;
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
  uniform vec4 vertColor;

  in vec3 surfacePosition;
  in vec3 surfaceNormal;
  in vec3 surfaceToCamera;

  out vec4 fragment_color;

  const float PI = 3.14159265359;

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

  // compute fresnel specular factor for given base specular and product
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

    float rim = mix(1.0 - roughness * mat.back_light * 0.9, 1.0, NdV);

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
      vec3 L = light.position - v_pos;
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

  void main()
  {
    // Properties
    vec3 color;
    float alpha;
    if (lights_on == 0)
    {
      color = vec3(1.0);
      alpha = vertColor.w;
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
      alpha = vertColor.w * mat.alpha;
    }
    if (fog.mode > 0)
    {
      fragment_color = vec4 (Apply_fog(vertColor.xyz*color), alpha);
    }
    else
    {
      fragment_color = vec4 (vertColor.xyz*color, alpha);
    }
  }
);

const GLchar * pick_color = GLSL(
  in vec4 vert_color;
  out uvec4 fragment_color;
  void main()
  {
    fragment_color = uvec4(vert_color);
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
    fragment_color = vert_color * vec4(1.0, 1.0, 1.0, texture (tex, text_coords).r);
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
    fragment_color = vert_color * sampled;
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
