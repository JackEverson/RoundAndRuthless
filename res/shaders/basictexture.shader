#shadervertex
#version 460 core
    layout (location = 0) in vec2 aPos;
    layout (location = 1) in vec2 aTexCoord;
    layout (location = 2) in vec3 instancePos;
    layout (location = 3) in vec2 instanceSize;
    layout (location = 4) in vec4 instanceColor;

    uniform mat4 aModel;
    uniform mat4 aProjection;
    uniform mat4 aView;

    out vec4 v_color;
    out vec2 v_texCoord;
    out float v_distance;

    void main()
    {
	vec4 scaled = aModel * vec4(aPos * instanceSize, 0, 1);
	vec4 worldPos = vec4(instancePos, 0) + scaled;

	gl_Position = aProjection * aView * worldPos;
	//gl_Position = vec4(worldPos, 0.0, 1.0);

        v_color = instanceColor;
        v_texCoord = aTexCoord;
	v_distance = worldPos.z;
    };

#shaderfragment
#version 460 core
    in vec4 v_color;
    in vec2 v_texCoord;
    in float v_distance;

    uniform sampler2D u_Texture;

    out vec4 FragColor;

    void main()
    {
	float fogFactor = exp(v_distance * 0.5);
	//float fogFactor = 0.5;
	
	vec4 textureColor = v_color * texture(u_Texture, v_texCoord);
	vec4 fogColor = vec4(0.1, 0.1, 0.1, 1.0);

	vec4 finalColor = mix(fogColor, textureColor, fogFactor);
	FragColor = vec4(finalColor.rgb, textureColor.a);
    };

