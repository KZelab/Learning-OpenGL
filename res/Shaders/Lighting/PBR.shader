// =============================================================================
// Physically Based Rendering (PBR) Shader
// =============================================================================
// PBR is a shading model that attempts to simulate how light actually behaves
// in the real world. Unlike older models (Phong, Blinn-Phong), PBR is based on
// the physics of light transport and energy conservation. It produces results
// that look consistent under all lighting conditions.
//
// The core model used here is the Cook-Torrance reflectance model, which
// treats every surface as a collection of infinitely tiny perfect mirrors
// called "microfacets". The macroscopic appearance of a surface emerges from
// the statistical distribution of these microfacet orientations.
//
// Key PBR properties:
//  - Energy Conservation: a surface cannot reflect MORE light than it receives.
//  - Metallic/Dielectric distinction: metals reflect light differently than
//    non-metals (dielectrics). Metals have coloured reflections; dielectrics do not.
//  - Microfacet theory: surface roughness controls how "spread out" specular
//    highlights are by controlling the distribution of microfacet normals.
// =============================================================================

#shader vertex
#version 330 core

// Vertex attributes from the VAO (see Vertex.h layout)
layout(location = 0) in vec3 aPosition;  // Object-space vertex position
layout(location = 1) in vec3 aNormal;    // Object-space vertex normal

// The MVP (Model-View-Projection) matrices transform coordinates between spaces:
//   Object Space -> World Space  (u_Model)
//   World Space  -> View Space   (u_View)
//   View Space   -> Clip Space   (u_Projection)
uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

// Outputs interpolated across the triangle in the rasterizer
out vec3 FragPos;  // World-space position, used for light direction math
out vec3 Normal;   // World-space normal

void main()
{
    // Transform vertex position to world space.
    // We use vec4 with w=1.0 because position is a point (translations apply).
    FragPos = vec3(u_Model * vec4(aPosition, 1.0));

    // Transform normal to world space using the Normal Matrix.
    // We can't just use u_Model directly for normals because normals are
    // directions, not points — non-uniform scaling distorts them.
    // The correct transform is the transpose of the inverse of the model matrix.
    // Note: this is expensive (per-vertex matrix inverse). For production, pass
    // this as a precomputed uniform instead.
    Normal = mat3(transpose(inverse(u_Model))) * aNormal;

    // Final clip-space position. The GPU uses this to rasterize the triangle.
    gl_Position = u_Projection * u_View * vec4(FragPos, 1.0);
}


// =============================================================================
#shader fragment
#version 330 core
// =============================================================================
// The fragment shader is where all the PBR math happens.
// For each pixel covered by a triangle, we evaluate the Reflectance Equation:
//
//   Lo(p, wo) = integral[ fr(p, wi, wo) * Li(p, wi) * dot(N, wi) ] dwi
//
// Where:
//   Lo  = outgoing radiance (what the camera sees at pixel p)
//   wo  = outgoing direction (towards camera)
//   wi  = incoming light direction
//   fr  = BRDF — how much light is reflected from wi towards wo
//   Li  = incoming radiance from direction wi
//   dot(N, wi) = Lambert's cosine law (light hits surface less at grazing angles)
//
// For a single point light we simplify the integral to a single sum over lights.
// =============================================================================

// --- Material Parameters ---
// These define the physical properties of the surface being shaded.

// Albedo: the base colour of the surface in linear space.
// For metals, this becomes the tint of specular reflections.
// For dielectrics (non-metals), this is the diffuse colour.
uniform vec3 u_Albedo;

// Metallic: 0.0 = dielectric (plastic, wood, skin), 1.0 = conductor (iron, gold).
// In real-world PBR, most materials are either fully metallic or fully dielectric.
// Values in between approximate dirty/oxidized metals or mixed materials.
uniform float u_Metallic;

// Roughness: 0.0 = perfectly smooth mirror, 1.0 = completely diffuse-looking.
// Controls the spread of the specular highlight via the microfacet distribution.
// Rough surfaces scatter light widely; smooth surfaces produce sharp reflections.
uniform float u_Roughness;

// Ambient Occlusion: a precomputed factor [0,1] representing how much ambient
// light reaches this surface point. 1.0 = fully exposed, 0.0 = fully occluded
// (e.g. a deep crevice). This approximates indirect lighting shadowing.
uniform float u_AO;

// --- Light Parameters ---
uniform vec3 u_LightPosition;   // World-space position of the point light
uniform vec3 u_LightColor;      // RGB colour of the light (linear space)
uniform float u_LightIntensity; // Scalar intensity multiplier

// Camera position in world space, needed to compute the view direction V.
uniform vec3 u_CameraPosition;

in vec3 FragPos;  // From vertex shader: world-space fragment position
in vec3 Normal;   // From vertex shader: world-space surface normal

out vec4 FragColor;

const float PI = 3.14159265359;


// =============================================================================
// BRDF Component 1: Normal Distribution Function (NDF) — GGX/Trowbridge-Reitz
// =============================================================================
// The NDF answers: "what fraction of microfacets are oriented exactly halfway
// between the light direction (L) and view direction (V)?" Only microfacets
// whose normal aligns with the halfway vector H can reflect light towards V.
//
// GGX (also called Trowbridge-Reitz) is the industry standard NDF because it
// has a longer, more realistic specular tail compared to older models (Blinn-Phong).
// Rough surfaces spread energy over a wide area; smooth surfaces concentrate it.
//
// Formula:  D(N, H, alpha) = alpha^2 / (PI * ((NdotH^2 * (alpha^2 - 1) + 1)^2))
// Where alpha = roughness^2  (squaring roughness gives more perceptually linear
// control — artists expect roughness 0.5 to look "halfway" between 0 and 1).
// =============================================================================
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    // Square roughness for perceptually linear remapping (Disney/Epic convention)
    float a = roughness * roughness;
    float a2 = a * a;

    // NdotH: how well the microfacet normal (H) aligns with the surface normal (N).
    // Clamped to 0 to avoid negative values at grazing/back-facing angles.
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    // The denominator drives the shape of the specular lobe.
    // When roughness -> 0, the denominator shrinks and the spike becomes very sharp.
    // When roughness -> 1, the denominator flattens, spreading energy over the hemisphere.
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}


// =============================================================================
// BRDF Component 2: Geometry Function — Schlick-GGX (single direction)
// =============================================================================
// Even microfacets oriented correctly can be blocked:
//   - Shadowing: incoming light blocked by neighbouring microfacets.
//   - Masking:   outgoing light blocked before it reaches the camera.
//
// This function computes the probability that a microfacet is visible from a
// given direction (either V or L). It returns a value in [0, 1].
//
// The k parameter differs between direct and IBL lighting (here we use the
// direct-light remapping: k = (roughness + 1)^2 / 8).
// =============================================================================
float GeometrySchlickGGX(float NdotV, float roughness)
{
    // k remapping for direct lighting (Schlick-Beckmann approximation).
    // Adding 1 before squaring prevents overly dark results at roughness = 0.
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    // Schlick approximation: ratio of light NOT blocked by microfacets.
    // At NdotV = 1 (surface facing directly towards view): returns 1 (no blocking).
    // At NdotV = 0 (grazing angle): returns a low value (lots of blocking).
    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

// =============================================================================
// Smith's Geometry Method — combines masking AND shadowing
// =============================================================================
// Smith's method applies the single-direction geometry function twice:
//   - Once for the view direction (masking — can camera see the microfacet?)
//   - Once for the light direction (shadowing — does light reach the microfacet?)
//
// Multiplying them gives the probability that a microfacet is BOTH lit AND visible.
// This is the statistically correct way to combine the two effects.
// =============================================================================
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);  // View angle cosine
    float NdotL = max(dot(N, L), 0.0);  // Light angle cosine

    // ggx2 = masking (view direction), ggx1 = shadowing (light direction)
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}


// =============================================================================
// BRDF Component 3: Fresnel Equation — Schlick Approximation
// =============================================================================
// The Fresnel effect describes how the amount of reflected vs. refracted light
// changes based on the viewing angle. At grazing angles (surface nearly edge-on),
// almost ALL light is reflected — this is why a pond looks mirror-like from afar
// but transparent when you look straight down into it.
//
// F0 is the "base reflectivity" — the reflectance at normal incidence (looking
// straight at the surface). It varies by material:
//   - Dielectrics: typically ~0.04 (4% specular reflectance, 96% refracted/diffuse)
//   - Metals: high and often coloured (e.g. gold ~[1.0, 0.86, 0.57])
//
// The Schlick approximation avoids the expensive exact Fresnel computation:
//   F(cosTheta, F0) = F0 + (1 - F0) * (1 - cosTheta)^5
// =============================================================================
vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    // cosTheta = dot(H, V): how head-on the view is relative to the halfway vector.
    // As cosTheta -> 0 (grazing), pow(1 - cosTheta, 5) -> 1, so F -> vec3(1).
    // clamp prevents NaN from floating-point error slightly outside [0,1].
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}


// =============================================================================
// Main Fragment Shader — Assembling the Full Cook-Torrance BRDF
// =============================================================================
void main()
{
    // Normalize interpolated vectors (rasterizer interpolation can de-normalize them)
    vec3 N = normalize(Normal);                          // Surface normal
    vec3 V = normalize(u_CameraPosition - FragPos);     // Direction to camera

    // -------------------------------------------------------------------------
    // F0: Base Reflectivity at Normal Incidence
    // -------------------------------------------------------------------------
    // For dielectrics (non-metals), F0 is achromatic and low (~0.04).
    // For metals, F0 is the albedo colour itself (metals have coloured reflections
    // because different wavelengths are absorbed differently at the surface).
    // mix() linearly interpolates: metallic=0 uses 0.04, metallic=1 uses albedo.
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, u_Albedo, u_Metallic);

    // -------------------------------------------------------------------------
    // Per-Light Radiance Calculation
    // -------------------------------------------------------------------------
    vec3 L = normalize(u_LightPosition - FragPos);  // Direction to light
    vec3 H = normalize(V + L);                      // Halfway vector between V and L
                                                    // H is the microfacet normal that
                                                    // would reflect L exactly towards V

    // Inverse-square law: light intensity falls off with distance^2.
    // This is physically accurate for point lights in 3D space.
    float distance    = length(u_LightPosition - FragPos);
    float attenuation = 1.0 / (distance * distance);

    // Radiance = the total light energy arriving from this light source.
    // Scaled by intensity and attenuated by distance.
    vec3 radiance = u_LightColor * u_LightIntensity * attenuation;

    // -------------------------------------------------------------------------
    // Cook-Torrance BRDF Evaluation
    // -------------------------------------------------------------------------
    // The specular BRDF is: fr_spec = (D * G * F) / (4 * dot(N,V) * dot(N,L))
    //
    //   D = NDF:     how many microfacets are aligned to reflect L -> V
    //   G = Geometry: probability microfacets are not shadowed/masked
    //   F = Fresnel:  ratio of reflected to refracted light
    //   Denominator:  normalisation factor derived from the microfacet model
    float NDF = DistributionGGX(N, H, u_Roughness);
    float G   = GeometrySmith(N, V, L, u_Roughness);
    vec3  F   = FresnelSchlick(max(dot(H, V), 0.0), F0);

    // -------------------------------------------------------------------------
    // Energy Conservation: splitting specular and diffuse contributions
    // -------------------------------------------------------------------------
    // kS (specular fraction) = Fresnel term F.
    // By energy conservation, the diffuse fraction kD = 1 - kS.
    // The remaining light that isn't specularly reflected is either:
    //   a) Absorbed (metals): metallic surfaces have no diffuse component because
    //      free electrons absorb the refracted light immediately.
    //   b) Scattered internally then re-emitted (dielectrics): this is diffuse.
    // So we zero out kD for metals by multiplying by (1 - metallic).
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - u_Metallic;  // Metals absorb all refracted light, leaving no diffuse

    // -------------------------------------------------------------------------
    // Cook-Torrance Specular Term
    // -------------------------------------------------------------------------
    vec3  numerator   = NDF * G * F;
    // The 0.0001 epsilon prevents division by zero at perfectly head-on angles
    // where either dot(N,V) or dot(N,L) could be exactly 0.
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3  specular    = numerator / denominator;

    // -------------------------------------------------------------------------
    // Lambertian Diffuse Term
    // -------------------------------------------------------------------------
    // The Lambertian diffuse model assumes refracted light scatters equally in
    // all directions regardless of view angle (view-independent).
    // Dividing albedo by PI normalises the BRDF so total reflected energy <= 1,
    // satisfying energy conservation (the diffuse lobe integrates to albedo over
    // the hemisphere, and integrating 1/PI over a hemisphere = 1).
    float NdotL = max(dot(N, L), 0.0);
    vec3  Lo    = (kD * u_Albedo / PI + specular) * radiance * NdotL;
    //                                                           ^^^^^
    //                                    Lambert's cosine law: light hitting at
    //                                    a steep angle delivers less energy per
    //                                    unit surface area.

    // -------------------------------------------------------------------------
    // Ambient Lighting (Simplified Indirect Lighting)
    // -------------------------------------------------------------------------
    // Real scenes have indirect lighting from bounced light (GI). Here we
    // approximate it with a small constant. AO darkens occluded areas so
    // crevices/corners don't receive the same ambient light as open surfaces.
    vec3 ambient = vec3(0.03) * u_Albedo * u_AO;

    vec3 color = ambient + Lo;

    // -------------------------------------------------------------------------
    // HDR Tonemapping — Reinhard Operator
    // -------------------------------------------------------------------------
    // PBR lighting is computed in linear HDR (values can exceed 1.0 for bright
    // lights). Before displaying on an LDR monitor we must "tonemap" — compress
    // the HDR range into [0, 1] while preserving perceived brightness/contrast.
    //
    // The Reinhard operator: x / (x + 1) is the simplest approach.
    //   - Dark values (x << 1): x / 1 ≈ x  (nearly linear, preserves shadows)
    //   - Bright values (x >> 1): x / x = 1  (caps at 1, prevents clipping)
    // More sophisticated operators (ACES, Uncharted 2) offer richer contrast,
    // but Reinhard is a good baseline.
    color = color / (color + vec3(1.0));

    // -------------------------------------------------------------------------
    // Gamma Correction
    // -------------------------------------------------------------------------
    // Monitors display in gamma-encoded space (sRGB, gamma ~2.2), meaning they
    // apply a power curve that brightens values. All our PBR math was done in
    // linear light space. To counteract the monitor's brightening, we apply
    // the INVERSE gamma: pow(x, 1/2.2) before outputting.
    // Without this, the image would appear too dark and washed out.
    // (Note: if the framebuffer is sRGB-aware, the driver does this automatically.)
    color = pow(color, vec3(1.0 / 2.2));

    FragColor = vec4(color, 1.0);
}
