#pragma once

constexpr const char *TextureVertexShader =
    R"(#version 460 core
    layout (location = 0) in vec2 a_pos;
    layout (location = 1) in vec2 a_tex_coord;
    layout (location = 2) in vec3 i_pos;
    layout (location = 3) in vec2 i_size;
    layout (location = 4) in vec4 i_color;
    layout (location = 5) in mat4 i_model;

    uniform mat4 u_projection;
    uniform mat4 u_view;
    uniform vec3 u_campos;

    out vec4 v_color;
    out vec2 v_texCoord;
    out float v_distance;

    void main()
    {
      vec4 scaled = i_model * vec4(a_pos * i_size, 0, 1);
      vec4 worldpos = vec4(i_pos, 0) + scaled;

      gl_Position = u_projection * u_view * worldpos;

      v_color = i_color;
      v_texCoord = a_tex_coord;
	    v_distance = distance(worldpos.xyz, u_campos);  
    };
  )";

constexpr const char *TextureFragmentShader =
    R"(#version 460 core
    in vec4 v_color;
    in vec2 v_texCoord;
    in float v_distance;

    uniform sampler2D u_Texture;
    uniform float u_fogfactor = 0.15;

    out vec4 FragColor;

    void main() {

      float fogFactor = exp(-v_distance * u_fogfactor);

      vec4 textureColor = v_color * texture(u_Texture, v_texCoord);
      vec4 fogColor = vec4(0.1, 0.1, 0.1, 1.0);

      vec4 finalColor = mix(fogColor, textureColor, fogFactor);
      FragColor = vec4(finalColor.rgb, textureColor.a);
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
