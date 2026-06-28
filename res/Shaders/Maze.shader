#shader vertex // Specifies the vertex shader section
#version 330 core // Using GLSL version 3.30, compatible with OpenGL 3.3


layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

void main()
{
    // Transform vertex position to world space so the fragment shader has
    // the correct world position for lighting calculations.
    FragPos = vec3(u_Model * vec4(aPosition, 1.0));

    // Correctly transform the normal to world space:
    // Normals are directions, not positions, so we apply the transpose of the
    // inverse of the model matrix. This keeps normals accurate even when
    // non-uniform scaling is applied (our walls are scaled cubes!).
    Normal = mat3(transpose(inverse(u_Model))) * aNormal;

    // Texture coordinates pass straight through to the fragment shader
    TexCoord = aTexCoord;

    // Transform to clip space with the Model-View-Projection chain
    gl_Position = u_Projection * u_View * vec4(FragPos, 1.0);
};


#shader fragment // Specifies the fragment shader section
#version 330 core // Using GLSL version 3.30, compatible with OpenGL 3.3


struct Light
{
    vec3 Position; // World space
    vec3 Colour;
};

uniform Light u_Light;
uniform vec3 u_CameraPosition; // Camera in world space

// Phong lighting parameters
uniform float u_AmbientIntensity;
uniform float u_DiffuseIntensity;
uniform float u_SpecularIntensity;
uniform float u_Shininess;

// Surface appearance:
// - u_UseTexture == 1: sample u_Texture and tint it with u_ObjectColour
// - u_UseTexture == 0: use u_ObjectColour as a flat material colour
// This lets one shader draw both textured geometry (walls, floor, door)
// and untextured pickups (coins, key) without switching programs.
uniform sampler2D u_Texture;
uniform int u_UseTexture;
uniform vec3 u_ObjectColour;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

void main()
{
    // ================================
    // Ambient Lighting
    // ================================
    // I_a = k_a * I_L
    vec3 ambient = u_AmbientIntensity * u_Light.Colour;

    // ================================
    // Diffuse Lighting (Lambertian Reflection)
    // ================================
    // I_d = k_d * I_L * max(0, N . L)
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(u_Light.Position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = u_DiffuseIntensity * diff * u_Light.Colour;

    // ================================
    // Specular Lighting (Phong Reflection Model)
    // ================================
    // I_s = k_s * I_L * max(0, R . V)^s
    vec3 viewDir = normalize(u_CameraPosition - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Shininess);
    vec3 specular = u_SpecularIntensity * spec * u_Light.Colour;

    // ================================
    // Final Composition
    // ================================
    // The base (albedo) colour comes from the texture or the flat material
    // colour. Ambient and diffuse light are modulated by the base colour;
    // the specular highlight keeps the light's colour (a white shine).
    vec3 baseColour = (u_UseTexture == 1)
        ? texture(u_Texture, TexCoord).rgb * u_ObjectColour
        : u_ObjectColour;

    vec3 result = (ambient + diffuse) * baseColour + specular;
    FragColor = vec4(result, 1.0);
}
