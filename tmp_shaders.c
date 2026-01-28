/* This file is part of the 'atomes' software
* Adapted temporary shaders for raytracing
*/

#include "global.h"

// #define GLSL(src) "#version 430 core\n" #src
#define GLSL(src) "#version 150\n" #src

const GLchar * sphere_vertex_ray = GLSL(
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
  
  // Standardized Raytracing Interface
  out vec3 imp_a;      // Sphere Center
  out vec3 imp_b;      // Unused for sphere
  out float imp_r;     // Radius
  flat out int form_type; // 0: Sphere

  void main ()
  {
    surfaceColor    = vertColor;
    vec4 pos = vec4 (radius*vert + offset, 1.0);
    surfacePosition = vec3(m_view * pos);
    surfaceNormal   = mat3(m_view) * vert;
    surfaceToCamera = normalize (- surfacePosition);
    
    // Output standard vars
    imp_a = vec3(m_view * vec4(offset, 1.0));
    // Apply View scale to radius
    float scale = length(mat3(m_view) * vec3(1.0, 0.0, 0.0));
    imp_r = radius * scale;
    form_type = 0;
    
    gl_PointSize = 1.0;
    gl_Position = mvp * pos;
  }
);

const GLchar * cylinder_vertex_ray = GLSL(
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
  
  // Standardized Raytracing Interface
  out vec3 imp_a;      // Cylinder Start
  out vec3 imp_b;      // Cylinder End
  out float imp_r;     // Radius
  flat out int form_type; // 1: Cylinder

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
    
    // Raytracing data
    vec3 p1 = vec3(0.0, 0.0, 0.0);
    vec3 p2 = vec3(0.0, 0.0, height);
    if (quat.w != 0.0)
    {
      p1 = rotate_this (p1, quat);
      p2 = rotate_this (p2, quat);
    }
    p1 += offset;
    p2 += offset;
    
    imp_a = vec3(m_view * vec4(p1, 1.0));
    imp_b = vec3(m_view * vec4(p2, 1.0));
    // Apply View scale to radius
    float scale = length(mat3(m_view) * vec3(1.0, 0.0, 0.0));
    imp_r = radius * scale;
    form_type = 1;
    
    gl_Position = mvp * vec4(pos,1.0);
  }
);

const GLchar * cone_vertex_ray = GLSL(
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
  
  // Standardized Raytracing Interface
  out vec3 imp_a;      // Cone Apex (Tip)
  out vec3 imp_b;      // Cone Base Center
  out float imp_r;     // Base Radius
  flat out int form_type; // 4: Cone

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
    
    float B = sqrt(radius*radius + height*height);
    vec3 norm = vec3(height*vert.x/B, height*vert.y/B, radius/B); // From existing shader

    if (quat.w != 0.0)
    {
      pos = rotate_this (pos, quat);
      norm = rotate_this (norm, quat);
    }
    pos += offset;
    surfacePosition = vec3(m_view * vec4(pos,1.0));
    surfaceNormal   = mat3(m_view) * norm;
    surfaceToCamera = normalize (- surfacePosition);
    
    vec3 p_base = vec3(0.0, 0.0, 0.0);
    vec3 p_apex = vec3(0.0, 0.0, height);
    
    if (quat.w != 0.0)
    {
      p_base = rotate_this (p_base, quat);
      p_apex = rotate_this (p_apex, quat);
    }
    p_base += offset;
    p_apex += offset;
    
    imp_a = vec3(m_view * vec4(p_apex, 1.0)); // Apex
    imp_b = vec3(m_view * vec4(p_base, 1.0)); // Base Center
    // Apply View scale to radius
    float scale = length(mat3(m_view) * vec3(1.0, 0.0, 0.0));
    imp_r = radius * scale;
    form_type = 4;
    
    gl_Position = mvp * vec4(pos,1.0);
  }
);

const GLchar * cap_vertex_ray = GLSL(
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
  
  // Standardized Raytracing Interface
  out vec3 imp_a;      // Cap Center
  out vec3 imp_b;      // Cap Normal
  out float imp_r;     // Radius
  flat out int form_type; // 2: Cap

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
    
    // Raytracing data
    vec3 c = vec3(0.0, 0.0, vert.z);
    vec3 n = vec3(0.0, 0.0, -1.0);
    if (quat.w != 0.0)
    {
      c = rotate_this (c, quat);
      n = rotate_this (n, quat);
    }
    c += offset;
    
    imp_a = vec3(m_view * vec4(c, 1.0));
    imp_b = mat3(m_view) * n;
    // Apply View scale to radius
    float scale = length(mat3(m_view) * vec3(1.0, 0.0, 0.0));
    imp_r = radius * scale;
    form_type = 2;

    gl_Position = mvp * vec4(pos,1.0);
  }
);

const GLchar * full_vertex_ray = GLSL(
  uniform mat4 mvp;
  uniform mat4 m_view;
  in vec3 vert;
  in vec3 vertNormal;
  in vec4 vertColor;

  out vec4 surfaceColor;
  out vec3 surfacePosition;
  out vec3 surfaceNormal;
  out vec3 surfaceToCamera;
  
  // Standardized Raytracing Interface
  out vec3 imp_a;
  out vec3 imp_b;
  out float imp_r;
  flat out int form_type; // 3: Triangle/Mesh

  void main ()
  {
    surfaceColor    = vertColor;

    surfacePosition = vec3(m_view * vec4(vert, 1.0f));
    surfaceNormal   = mat3(m_view) * vertNormal;
    surfaceToCamera = normalize (- surfacePosition);
    
    // Standard interface unused outputs
    imp_a = vec3(0.0);
    imp_b = vec3(0.0);
    imp_r = 0.0;
    form_type = 3;

    gl_Position     = mvp * vec4(vert, 1.0f);
  }
);

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
  
  // Standardized Raytracing Interface
  in vec3 imp_a;
  in vec3 imp_b;
  in float imp_r;
  flat in int form_type;

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
    // vec3 N = surfaceNormal; // Using argument now

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

  vec3 Apply_fog (in vec3 lightColor, in vec3 v_pos)
  {
      //distance
    float dist = 0.0;
    float fogFactor = 0.0;

    //compute distance used in fog equations
    if (fog.based == 0)
    {
      //plane based
      dist = abs (v_pos.z);
    }
    else
    {
      //range based
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
    return mix (fog.color, lightColor, fogFactor);
  }

  bool intersect_sphere(vec3 ro, vec3 rd, vec3 center, float radius, out vec3 hitPos, out vec3 hitNorm)
  {
    vec3 m = ro - center;
    float b = dot(m, rd);
    float c = dot(m, m) - radius * radius;

    if (c > 0.0 && b > 0.0) return false;
    
    float discr = b*b - c;
    if (discr < 0.0) return false;
    
    float t = -b - sqrt(discr);
    if (t < 0.0) t = 0.0;
    
    hitPos = ro + t * rd;
    hitNorm = normalize(hitPos - center);
    return true;
  }

  bool intersect_cylinder(vec3 ro, vec3 rd, vec3 pa, vec3 pb, float ra, out vec3 hitPos, out vec3 hitNorm)
  {
      vec3 ba = pb - pa;
      vec3 oc = ro - pa;
      
      float baba = dot(ba,ba);
      float bard = dot(ba,rd);
      float baoc = dot(ba,oc);
      
      // Use cross product stability
      vec3 va = cross(oc, ba);
      vec3 vb = cross(rd, ba);
      
      float k2 = dot(vb, vb); // baba - bard*bard
      float k1 = dot(va, vb); // baba*dot(oc,rd) - baoc*bard
      float k0 = dot(va, va) - ra*ra*baba; // baba*dot(oc,oc) - baoc*baoc - ra*ra*baba
      
      if (k2 == 0.0) return false; // Parallel to axis off-center
      
      float h = k1*k1 - k2*k0;
      if(h < 0.0) return false;
      
      h = sqrt(h);
      float t = (-k1 - h)/k2;
      
      // body
      float y = baoc + t*bard;
      if(y > 0.0 && y < baba)
      {
          hitPos = ro + t * rd;
          // Normal: Outward from axis
          hitNorm = normalize((hitPos - pa) * baba - ba * y);
          return true;
      }
      return false;
  }

  bool intersect_cap(vec3 ro, vec3 rd, vec3 center, vec3 normal, float radius, out vec3 hitPos, out vec3 hitNorm)
  {
      float denom = dot(normal, rd);
      if (abs(denom) > 1e-6)
      {
          float t = dot(center - ro, normal) / denom;
          if (t >= 0.0)
          {
              vec3 p = ro + t * rd;
              vec3 v = p - center;
              if (dot(v, v) <= radius * radius)
              {
                  hitPos = p;
                  hitNorm = normal; 
                  // Orient normal towards ray
                  if (dot(hitNorm, rd) > 0.0) hitNorm = -hitNorm;
                  return true;
              }
          }
      }
      return false;
  }

  bool intersect_cone(vec3 ro, vec3 rd, vec3 pa, vec3 pb, float ra, out vec3 hitPos, out vec3 hitNorm)
  {
      vec3 ba = pb - pa;
      vec3 oa = ro - pa;
      vec3 ob = ro - pb;
      
      float m0 = dot(ba,ba);
      float m1 = dot(oa,ba);
      float m2 = dot(rd,ba);
      float m3 = dot(rd,oa);
      float m4 = dot(oa,oa);
      float m5 = dot(ob,ob); // Not used?
      
      if (m0 < 1e-6) return false;
      
      float hyp_sq = m0 + ra*ra;
      float k = (ra*ra) / m0; 
      
      // vec3  oc = oa;
      float m = dot(rd,ba)/m0; 
      float n = dot(oa,ba)/m0; 
      
      float a = dot(rd,rd) - m2*m2*(1.0+k)/m0;
      float b = dot(rd,oa) - m2*m1*(1.0+k)/m0;
      float c = dot(oa,oa) - m1*m1*(1.0+k)/m0;
      
      float h = b*b - a*c;
      if (h < 0.0) return false;
      
      h = sqrt(h);
      float t = (-b - h)/a;
      
      float y = m1 + t*m2;
      
      if (y > 0.0 && y < m0)
      {
           hitPos = ro + t * rd;
           hitNorm = normalize(m0*(hitPos-pa) - ba*(1.0+k)*y);
           return true; 
      }
      
      return false;
  }

  void main ()
  {
    // Raytracing
    vec3 ray_dir = normalize(surfacePosition); // View space ray direction (from 0,0,0)
    vec3 ray_origin = vec3(0.0);
    
    vec3 hitPos;
    vec3 hitNorm;
    bool hit = false;
    
    if (form_type == 0) // Sphere
    {
        hit = intersect_sphere(ray_origin, ray_dir, imp_a, imp_r, hitPos, hitNorm);
    }
    else if (form_type == 1) // Cylinder
    {
        hit = intersect_cylinder(ray_origin, ray_dir, imp_a, imp_b, imp_r, hitPos, hitNorm);
    }
    else if (form_type == 2) // Cap
    {
        hit = intersect_cap(ray_origin, ray_dir, imp_a, imp_b, imp_r, hitPos, hitNorm);
    }
    else if (form_type == 3) // Triangle/Mesh
    {
        hit = true;
        hitPos = surfacePosition;
        if (length(surfaceNormal) > 0.0)
        {
          hitNorm = normalize(surfaceNormal);
        }
        else
        {
          hitNorm = vec3(0,0,1);
        }
    }
    else if (form_type == 4) // Cone
    {
        hit = intersect_cone(ray_origin, ray_dir, imp_a, imp_b, imp_r, hitPos, hitNorm);
    }
    
    if (!hit) discard;

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
        color +=  Apply_lighting_model (lights_on, AllLights[i], specular, hitPos, hitNorm);
      }
      color = pow(color, vec3(1.0/mat.gamma));
      alpha = surfaceColor.w * mat.alpha;
    }
    
    vec3 final_color = surfaceColor.xyz * color;
    
    if (fog.mode > 0)
    {
      fragment_color = vec4 (Apply_fog(final_color, hitPos), alpha);
    }
    else
    {
      fragment_color = vec4 (final_color, alpha);
    }
  }
);
