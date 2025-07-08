#shadervertex
#version 460 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec2 atexCoord;
    layout (location = 2) in vec3 aOffset;

    //uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    out vec2 v_texCoord;

    void main()
    {
        gl_Position = projection * view * vec4(aPos + aOffset, 1.0);

        v_texCoord = atexCoord;
    };

#shaderfragment
#version 460 core
    in vec2 v_texCoord;
    uniform sampler2D u_Texture;
    out vec4 FragColor;
    void main()
    {
        vec4 texColor = texture(u_Texture, v_texCoord);
        FragColor = texColor;
    };

