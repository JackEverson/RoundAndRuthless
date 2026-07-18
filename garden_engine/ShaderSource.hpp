#pragma once

constexpr const char *TextureVertexShader =
    R"(#version 460 core
    layout (location = 0) in vec2 a_pos;
    layout (location = 1) in vec2 a_tex_coord;
    layout (location = 2) in vec3 i_pos;
    layout (location = 3) in vec2 i_size;
    layout (location = 4) in vec4 i_color;
    layout (location = 5) in mat4 i_model;
    layout (location = 9) in vec2 i_uv_scale;

    uniform mat4 u_projection;
    uniform mat4 u_view;
    uniform vec3 u_cam_pos;

    out vec4 v_color;
    out vec2 v_tex_coord;
    out vec3 v_world_pos;

    void main()
    {
      vec4 scaled = i_model * vec4(a_pos * i_size, 0, 1);
      vec4 worldpos = vec4(i_pos, 0) + scaled;

      gl_Position = u_projection * u_view * worldpos;

      v_color = i_color;
      v_tex_coord = a_tex_coord * i_uv_scale;
      v_world_pos = worldpos.xyz;
    };
  )";

constexpr const char *TextureFragmentShader =
    R"(#version 460 core
    in vec4 v_color;
    in vec2 v_tex_coord;
    in vec3 v_world_pos;

    struct PointLight{
      vec3 position;
      vec3 color;
      float radius;
    };

    const int MAX_LIGHTS = 8;
    uniform PointLight u_lights[MAX_LIGHTS];
    uniform int u_light_count = 0;
    uniform float u_ambient = 0.1;
    uniform vec4 u_fog_color = vec4(0.1, 0.1, 0.1, 1.0);


    uniform sampler2D u_texture;
    uniform float u_fog_factor = 0.15;
    uniform vec3 u_cam_pos;

    out vec4 FragColor;

    void main() {

      // per-fragment distance: interpolating this across large quads (e.g. the
      // floor) pulled far-corner fog onto surfaces right under the camera
      float v_distance = distance(v_world_pos, u_cam_pos);
      float fog_factor = exp(-v_distance * u_fog_factor);

      vec3 light_total = vec3(u_ambient);
      for (int i = 0; i < u_light_count; i++) {
          float d = distance(v_world_pos, u_lights[i].position);
          float atten = 1.0 / (1.0 + pow(d / u_lights[i].radius, 2.0));
          light_total += atten * u_lights[i].color;
      }
      vec4 texture_color = v_color * texture(u_texture, v_tex_coord) * vec4(clamp(light_total, 0.0, 1.0), 1.0);

      // cutout pixels write neither color nor DEPTH — without this, the
      // invisible parts of sprites/fences occlude whatever draws after them
      if (texture_color.a < 0.1) discard;

      vec4 final_color = mix(u_fog_color, texture_color, fog_factor);
      FragColor = vec4(final_color.rgb, texture_color.a);
    };
  )";

constexpr const char *BasicVertexShader =
    R"(#version 460 core
layout(location = 0) in vec3 aPos;

uniform vec3 aColor;
uniform vec3 aLoc;

out vec3 color;

void main() {
  vec3 newPos = aPos + aLoc;
  gl_Position = vec4(newPos.x, newPos.y, newPos.z, 1.0);
  color = aColor;
};
  )";

constexpr const char *BasicFragmentShader =
    R"(#version 460 core
in vec3 color;
out vec4 FragColor;
void main() { FragColor = vec4(color, 1.0f); };
  )";

constexpr const char *BackgroundVertexShader =
    R"(#version 460 core
      layout (location = 0) in vec2 aPos;
      layout (location = 1) in vec2 aTexCoord;

      out vec2 v_texCoord;

      void main()
      {
          v_texCoord = aTexCoord;
    gl_Position = vec4(aPos * 2, 0, 1);
      };
  )";

constexpr const char *BackgroundFragmentShader =
    R"(#version 460 core
in vec2 v_texCoord;

uniform sampler2D u_Texture;

out vec4 FragColor;

void main() { FragColor = texture(u_Texture, v_texCoord); };
  )";
