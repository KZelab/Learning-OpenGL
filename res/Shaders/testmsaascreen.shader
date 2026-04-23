#shader vertex
#version 330 core

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec2 a_TexCoord;

out vec2 v_TexCoord;

void main()
{
    gl_Position = vec4(a_Position, 0.0, 1.0);
    v_TexCoord  = a_TexCoord;
}

#shader fragment
#version 330 core

in vec2 v_TexCoord;

uniform sampler2D u_ScreenTexture;

out vec4 FragColour;

void main()
{
    FragColour = texture(u_ScreenTexture, v_TexCoord);
}
