#pragma once
#include "TestPBR.h"
#include "../Shader.h"
#include <memory>

namespace test
{
    class TestPBRSphereGrid : public TestPBR
    {
    public:
        TestPBRSphereGrid(GLFWwindow* window);
        ~TestPBRSphereGrid() override = default;

        void Render()    override;
        void RenderGUI() override;

    private:
        static constexpr int   GRID_SIZE   = 7;
        static constexpr float SPACING     = 2.5f;
        static constexpr int   NUM_LIGHTS  = 4;

        std::unique_ptr<Shader> m_GridShader;

        glm::vec3 m_LightPositions[NUM_LIGHTS];
        glm::vec3 m_LightColours[NUM_LIGHTS];   // normalized [0,1] RGB
        float     m_LightIntensity = 300.0f;
    };
}
