#shadervertex
#version 460 core
    layout (location = 0) in vec2 aPos;
    layout (location = 1) in vec2 aTexCoord;


    out vec2 v_texCoord;

    void main()
    {
        v_texCoord = aTexCoord;
	gl_Position = vec4(aPos * 2, 0, 1);
    };

#shaderfragment
#version 460 core

    in vec2 v_texCoord;

    uniform sampler2D u_Texture;

    out vec4 FragColor;

    void main()
    {
        FragColor = texture(u_Texture, v_texCoord);
    };

