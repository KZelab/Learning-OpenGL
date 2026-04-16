#shader vertex
#version 330 core

layout(location = 0) in vec3 aPosition;

uniform mat4 u_View;
uniform mat4 u_Projection;

out vec3 v_TexCoords;

void main()
{
    v_TexCoords = aPosition;
    // Set w == z so depth maps to 1.0 (maximum) after perspective divide
    gl_Position = (u_Projection * u_View * vec4(aPosition, 1.0)).xyww;
}

#shader fragment
#version 330 core

in  vec3 v_TexCoords;
out vec4 FragColor;

vec3 SkyGradient(vec3 dir)
{
    vec3 d       = normalize(dir);
    float t      = clamp(d.y * 0.5 + 0.5, 0.0, 1.0);
    vec3 zenith  = vec3(0.08, 0.20, 0.55);   // deep blue
    vec3 horizon = vec3(0.55, 0.72, 0.90);   // pale blue-white
    vec3 ground  = vec3(0.18, 0.15, 0.12);   // dark brownish

    if (d.y < 0.0)
        return mix(horizon, ground, clamp(-d.y * 4.0, 0.0, 1.0));
    return mix(horizon, zenith, t);
}

void main()
{
    vec3 color = SkyGradient(v_TexCoords);
    FragColor  = vec4(color, 1.0);
}
