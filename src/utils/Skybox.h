#pragma once
#include <GL/glew.h>
#include <memory>
#include <glm/glm.hpp>
#include "Shader.h"

// Procedural skybox using a gradient sky function — no cube map texture needed.
// Uses raw positions-only VAO (not the Mesh system) to avoid the 56-byte Vertex overhead.
class Skybox
{
public:
    Skybox();
    ~Skybox();

    Skybox(const Skybox&)            = delete;
    Skybox& operator=(const Skybox&) = delete;

    // Strips translation from view, sets depth to LEQUAL/DEPTH_MASK false, draws, restores state.
    void Draw(const glm::mat4& view, const glm::mat4& projection);

private:
    GLuint m_VAO = 0;
    GLuint m_VBO = 0;
    std::unique_ptr<Shader> m_Shader;
};
