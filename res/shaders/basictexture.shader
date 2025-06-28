#shadervertex
#version 460 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec2 atexCoord;

    uniform vec3 aColor;
    uniform vec3 aLoc;

    out vec3 v_color;
    out vec2 v_texCoord;

    void main()
    {
        vec3 newPos = aPos + aLoc;
        gl_Position = vec4(newPos.x, newPos.y, newPos.z, 1.0);
        v_color = aColor;
        v_texCoord = atexCoord;
    };

#shaderfragment
#version 460 core
    in vec3 v_color;
    in vec2 v_texCoord;
    uniform sampler2D u_Texture;
    out vec4 FragColor;
    void main()
    {
        vec4 texColor = texture(u_Texture, v_texCoord);
        // FragColor = vec4(v_color, 1.0f);
        FragColor = texColor;
    };

