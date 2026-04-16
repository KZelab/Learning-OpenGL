#include "TestPBRSphereGrid.h"
#include "../Renderer.h"
#include "imgui.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace test
{
    TestPBRSphereGrid::TestPBRSphereGrid(GLFWwindow* window)
        : TestPBR(window)
    {
        m_Camera = std::make_unique<Camera>(
            window,
            glm::vec3(0.0f, 0.0f, 40.0f),
            glm::vec3(0.0f, 1.0f,  0.0f),
            -180.0f, 0.0f, 45.0f
        );

        m_GridShader = std::make_unique<Shader>("res/shaders/Lighting/PBRGrid.shader");

        // 4 lights at the corners (classic LearnOpenGL layout)
        m_LightPositions[0] = glm::vec3(-10.0f,  10.0f, 10.0f);
        m_LightPositions[1] = glm::vec3( 10.0f,  10.0f, 10.0f);
        m_LightPositions[2] = glm::vec3(-10.0f, -10.0f, 10.0f);
        m_LightPositions[3] = glm::vec3( 10.0f, -10.0f, 10.0f);

        // Normalized white lights; intensity applied separately at upload time
        for (int i = 0; i < NUM_LIGHTS; ++i)
            m_LightColours[i] = glm::vec3(1.0f);

        m_Albedo      = glm::vec3(0.5f, 0.5f, 0.5f);
        m_AO          = 1.0f;
        m_RotateModel = false;
    }

    void TestPBRSphereGrid::Render()
    {
        Renderer renderer;
        renderer.Clear();

        m_GridShader->Bind();

        m_GridShader->setUniformMat4f("u_View",       m_View);
        m_GridShader->setUniformMat4f("u_Projection", m_Projection);
        m_GridShader->setUniform3f("u_Albedo",        m_Albedo.r,   m_Albedo.g,   m_Albedo.b);
        m_GridShader->setUniform1f("u_AO",            m_AO);

        glm::vec3 camPos = m_Camera->getPosition();
        m_GridShader->setUniform3f("u_CameraPosition", camPos.x, camPos.y, camPos.z);

        // Upload all 4 lights — colour * intensity gives the radiance value the shader expects
        for (int i = 0; i < NUM_LIGHTS; ++i)
        {
            glm::vec3 radiance = m_LightColours[i] * m_LightIntensity;
            std::string posName   = "u_LightPositions[" + std::to_string(i) + "]";
            std::string colourName = "u_LightColours["    + std::to_string(i) + "]";
            m_GridShader->setUniform3f(posName,   m_LightPositions[i].x, m_LightPositions[i].y, m_LightPositions[i].z);
            m_GridShader->setUniform3f(colourName, radiance.r,             radiance.g,             radiance.b);
        }

        const float half = (GRID_SIZE - 1) * 0.5f * SPACING;  // 7.5

        for (int j = 0; j < GRID_SIZE; ++j)       // Y = metallic
        {
            float metallic = static_cast<float>(j) / (GRID_SIZE - 1);

            for (int i = 0; i < GRID_SIZE; ++i)   // X = roughness
            {
                float roughness = 0.05f + static_cast<float>(i) / (GRID_SIZE - 1) * 0.95f;

                glm::mat4 model = glm::translate(glm::mat4(1.0f),
                    glm::vec3(-half + i * SPACING, -half + j * SPACING, 0.0f));

                m_GridShader->setUniformMat4f("u_Model",    model);
                m_GridShader->setUniform1f("u_Metallic",   metallic);
                m_GridShader->setUniform1f("u_Roughness",  roughness);
                m_Sphere->Draw();
            }
        }
    }

    void TestPBRSphereGrid::RenderGUI()
    {
        ImGui::Text("PBR Sphere Grid (7x7)");
        ImGui::Separator();
        ImGui::Text("X  ->  Roughness  [0.05 ... 1.0]");
        ImGui::Text("Y  ->  Metallic   [0.0  ... 1.0]");
        ImGui::Separator();
        ImGui::ColorEdit3("Albedo", glm::value_ptr(m_Albedo));
        ImGui::SliderFloat("AO",    &m_AO, 0.0f, 1.0f);
        ImGui::Separator();
        ImGui::Text("Lights");
        ImGui::SliderFloat("Intensity (all)", &m_LightIntensity, 0.0f, 1000.0f);
        for (int i = 0; i < NUM_LIGHTS; ++i)
        {
            ImGui::PushID(i);
            std::string label = "Light " + std::to_string(i + 1);
            ImGui::SliderFloat3(label.c_str(), glm::value_ptr(m_LightPositions[i]), -30.0f, 30.0f);
            ImGui::ColorEdit3(("Colour##" + std::to_string(i)).c_str(), glm::value_ptr(m_LightColours[i]));
            ImGui::PopID();
        }
        ImGui::Separator();
        if (ImGui::Checkbox("Wireframe", &m_Wireframe))
            glPolygonMode(GL_FRONT_AND_BACK, m_Wireframe ? GL_LINE : GL_FILL);
    }
}
