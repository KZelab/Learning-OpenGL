#shader vertex
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

out vec3 FragPos;
out vec3 Normal;

void main()
{
    FragPos = vec3(u_Model * vec4(aPosition, 1.0));
    Normal = mat3(transpose(inverse(u_Model))) * aNormal;
    gl_Position = u_Projection * u_View * vec4(FragPos, 1.0);
}

#shader fragment
#version 330 core

// Material parameters
uniform vec3  u_Albedo;
uniform float u_Metallic;
uniform float u_Roughness;
uniform float u_AO;

// 4 point lights
uniform vec3  u_LightPositions[4];
uniform vec3  u_LightColours[4];    // pre-multiplied by intensity

// Camera position
uniform vec3  u_CameraPosition;

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColour;

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a  = roughness * roughness;
    float a2 = a * a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return a2 / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    return GeometrySchlickGGX(NdotV, roughness) * GeometrySchlickGGX(NdotL, roughness);
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main()
{
    vec3 N = normalize(Normal);
    vec3 V = normalize(u_CameraPosition - FragPos);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, u_Albedo, u_Metallic);

    vec3 Lo = vec3(0.0);

    for (int i = 0; i < 4; ++i)
    {
        vec3  L         = normalize(u_LightPositions[i] - FragPos);
        vec3  H         = normalize(V + L);
        float dist      = length(u_LightPositions[i] - FragPos);
        float atten     = 1.0 / (dist * dist);
        vec3  radiance  = u_LightColours[i] * atten;

        float NDF = DistributionGGX(N, H, u_Roughness);
        float G   = GeometrySmith(N, V, L, u_Roughness);
        vec3  F   = FresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3  kS = F;
        vec3  kD = (vec3(1.0) - kS) * (1.0 - u_Metallic);

        vec3  num   = NDF * G * F;
        float denom = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3  spec  = num / denom;

        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * u_Albedo / PI + spec) * radiance * NdotL;
    }

    vec3 ambient = vec3(0.03) * u_Albedo * u_AO;
    vec3 colour   = ambient + Lo;

    // HDR tonemapping + gamma correction
    colour = colour / (colour + vec3(1.0));
    colour = pow(colour, vec3(1.0 / 2.2));

    FragColour = vec4(colour, 1.0);
}
