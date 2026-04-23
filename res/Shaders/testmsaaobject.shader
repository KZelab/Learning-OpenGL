#shader vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

out vec3 v_Normal;

void main()
{
    gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);
    v_Normal    = mat3(transpose(inverse(u_Model))) * a_Normal;
}

#shader fragment
#version 330 core

in vec3 v_Normal;

uniform vec3 u_Colour;

out vec4 FragColour;

void main()
{
    vec3  norm     = normalize(v_Normal);
    vec3  lightDir = normalize(vec3(1.0, 1.0, 1.0));
    float diff     = max(dot(norm, lightDir), 0.0);
    vec3  result   = u_Colour * (0.25 + 0.75 * diff);
    FragColour     = vec4(result, 1.0);
}
