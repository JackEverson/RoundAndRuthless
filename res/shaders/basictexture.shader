#shadervertex
#version 460 core
    layout (location = 0) in vec2 aPos;
    layout (location = 1) in vec2 atexCoord;
    layout (location = 2) in vec2 instancePos;
    layout (location = 3) in vec2 instanceSize;

    //uniform mat4 aModel;
    uniform mat4 aProjection;
    uniform mat4 aView;
    uniform vec4 aColor;

    out vec4 v_color;
    out vec2 v_texCoord;

    void main()
    {
	vec2 scaled = aPos * instanceSize;
	vec2 worldPos = instancePos + scaled;

	gl_Position = aProjection * aView * vec4(worldPos, 0.0, 1.0);
	//gl_Position = vec4(worldPos, 0.0, 1.0);

        v_color = aColor;
        v_texCoord = atexCoord;
    };

#shaderfragment
#version 460 core
    in vec4 v_color;
    in vec2 v_texCoord;

    uniform sampler2D u_Texture;

    out vec4 FragColor;

    void main()
    {
        FragColor = v_color * texture(u_Texture, v_texCoord);
    };

