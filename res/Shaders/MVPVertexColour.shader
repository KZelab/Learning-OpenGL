#shader vertex // Specifies the vertex shader section
#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColour;
layout(location = 3) in vec2 aTexCoord;


out vec3 ourColour;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPosition, 1.0);
    ourColour = aColour;
}


#shader fragment // Specifies the fragment shader section
#version 330 core // Using GLSL version 3.30, compatible with OpenGL 3.3
in vec3 ourColour;
out vec4 FragColour;

void main()
{
    FragColour = vec4(ourColour, 1.0);
}
