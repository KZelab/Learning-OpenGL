#shader vertex
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 3) in vec2 aTexCoords;
layout(location = 4) in vec3 aTangent;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

out vec3 FragPos;
out vec2 v_TexCoords;
out mat3 v_TBN;

void main()
{
    FragPos = vec3(u_Model * vec4(aPosition, 1.0));

    mat3 nm = mat3(transpose(inverse(u_Model)));
    vec3 N  = normalize(nm * aNormal);
    vec3 T  = normalize(nm * aTangent);
    T       = normalize(T - dot(T, N) * N);  // Gram-Schmidt re-orthogonalise
    vec3 B  = cross(N, T);
    v_TBN   = mat3(T, B, N);

    v_TexCoords = aTexCoords;

    gl_Position = u_Projection * u_View * vec4(FragPos, 1.0);
}

#shader fragment
#version 330 core

// Textures auto-bound by ModelMesh::Draw()
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;

// Normal map controls
uniform bool  u_UseNormalMap;
uniform float u_NormalScale;

// Material parameters (GUI-controlled)
uniform float u_Metallic;
uniform float u_Roughness;
uniform float u_AO;

// Light parameters
uniform vec3  u_LightPosition;
uniform vec3  u_LightColor;
uniform float u_LightIntensity;

// Camera position
uniform vec3 u_CameraPosition;

in vec3 FragPos;
in vec2 v_TexCoords;
in mat3 v_TBN;

out vec4 FragColor;

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness * roughness;
    float a2     = a * a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float denom  = (NdotH2 * (a2 - 1.0) + 1.0);
    denom        = PI * denom * denom;
    return a2 / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r     = (roughness + 1.0);
    float k     = (r * r) / 8.0;
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
    // Resolve surface normal
    vec3 N;
    if (u_UseNormalMap)
    {
        vec3 ts = texture(texture_normal1, v_TexCoords).rgb * 2.0 - 1.0;
        ts.xy  *= u_NormalScale;
        N = normalize(v_TBN * normalize(ts));
    }
    else
    {
        N = normalize(v_TBN[2]);  // geometric normal (3rd column of TBN)
    }

    // Sample albedo from diffuse texture
    vec3 albedo = texture(texture_diffuse1, v_TexCoords).rgb;

    vec3 V = normalize(u_CameraPosition - FragPos);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, u_Metallic);

    // Per-light radiance
    vec3  L          = normalize(u_LightPosition - FragPos);
    vec3  H          = normalize(V + L);
    float distance   = length(u_LightPosition - FragPos);
    float attenuation = 1.0 / (distance * distance);
    vec3  radiance   = u_LightColor * u_LightIntensity * attenuation;

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, u_Roughness);
    float G   = GeometrySmith(N, V, L, u_Roughness);
    vec3  F   = FresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3  kS        = F;
    vec3  kD        = (vec3(1.0) - kS) * (1.0 - u_Metallic);
    vec3  numerator = NDF * G * F;
    float denom     = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3  specular  = numerator / denom;

    float NdotL = max(dot(N, L), 0.0);
    vec3  Lo    = (kD * albedo / PI + specular) * radiance * NdotL;

    vec3 ambient = vec3(0.03) * albedo * u_AO;
    vec3 color   = ambient + Lo;

    // HDR tonemapping + gamma correction
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    FragColor = vec4(color, 1.0);
}
