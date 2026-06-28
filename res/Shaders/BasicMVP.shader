#shader vertex // Specifies the vertex shader section
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec4 aColour;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 fragColour;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    fragColour = aColour; // Pass the colour to the fragment shader
}


#shader fragment // Specifies the fragment shader section
#version 330 core

in vec4 fragColour;

uniform vec4 u_Colour; // The colour passed in from the CPU-side code

out vec4 colour;

void main() {
    colour = u_Colour; // Use the provided uniform colour
}
