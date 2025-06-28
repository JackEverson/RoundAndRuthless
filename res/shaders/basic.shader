#shadervertex
#version 460 core
    layout (location = 0) in vec3 aPos;

    uniform vec3 aColor;
    uniform vec3 aLoc;

    out vec3 color;

    void main()
    {
        vec3 newPos = aPos + aLoc;
        gl_Position = vec4(newPos.x, newPos.y, newPos.z, 1.0);
        color = aColor;
    };

#shaderfragment
#version 460 core
    in vec3 color;
    out vec4 FragColor;
    void main()
    {
        FragColor = vec4(color, 1.0f);
    };

