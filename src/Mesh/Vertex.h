#pragma once

struct Vertex {
    float position[3];
    float normal[3];
    float colour[3];
    float texCoords[2];
	float tangent[3]; // Added tangent for normal mapping

    Vertex(float px, float py, float pz,
           float nx, float ny, float nz,
           float r,  float g,  float b,
           float u,  float v,
		float tx = 1.f, float ty = 0.f, float tz = 0.f) // Full constructor with tangent parameters (default to a simple tangent if not provided)
        : position{ px, py, pz }, normal{ nx, ny, nz }, colour{ r, g, b },
          texCoords{ u, v }, tangent{ tx, ty, tz } {
    }

    // Convenience constructor for backward compatibility (assumes normal pointing up)
    Vertex(float px, float py, float pz, float r, float g, float b, float u, float v)
        : position{ px, py, pz }, normal{ 0.0f, 0.0f, 1.0f }, colour{ r, g, b },
          texCoords{ u, v }, tangent{ 1.0f, 0.0f, 0.0f } {
    }
};
