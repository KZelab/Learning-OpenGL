#include "TestPBRNormalMap.h"
#include "../Renderer.h"
#include "../vendor/imgui/imgui.h"
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

test::TestPBRNormalMap::TestPBRNormalMap(GLFWwindow* window)
    : TestPBR(window)
{
    m_NormalMapShader = std::make_unique<Shader>("res/shaders/Lighting/PBRNormalMap.shader");

    m_3DModel = std::make_unique<Model>("res/Model/survival_guitar_backpack/scene.gltf");
    m_3DModel->setScale(glm::vec3(0.1f));
    m_3DModel->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
}

void test::TestPBRNormalMap::Update(float deltaTime)
{
    TestPBR::Update(deltaTime);  // updates camera, m_View, m_Projection

    if (m_RotateModel)
    {
        float angle = static_cast<float>(glfwGetTime()) * 20.0f;
        m_3DModel->setRotation(glm::vec3(0.0f, angle, 0.0f));
    }
    else
    {
        m_3DModel->setRotation(glm::vec3(0.0f));
    }
}

void test::TestPBRNormalMap::SetModelUniforms(Shader& shader)
{
    shader.setUniformMat4f("u_Model",      m_3DModel->getTransformMatrix());
    shader.setUniformMat4f("u_View",       m_View);
    shader.setUniformMat4f("u_Projection", m_Projection);

    shader.setUniform1f("u_Metallic",  m_Metallic);
    shader.setUniform1f("u_Roughness", m_Roughness);
    shader.setUniform1f("u_AO",        m_AO);

    shader.setUniform3f("u_LightPosition",  m_LightPosition.x,  m_LightPosition.y,  m_LightPosition.z);
    shader.setUniform3f("u_LightColor",     m_LightColor.r,     m_LightColor.g,     m_LightColor.b);
    shader.setUniform1f("u_LightIntensity", m_LightIntensity);

    glm::vec3 camPos = m_Camera->getPosition();
    shader.setUniform3f("u_CameraPosition", camPos.x, camPos.y, camPos.z);

    shader.setUniform1i("u_UseNormalMap", (int)m_UseNormalMap);
    shader.setUniform1f("u_NormalScale",  m_NormalMapScale);
}

void test::TestPBRNormalMap::Render()
{
    Renderer renderer;
    renderer.Clear();

    m_NormalMapShader->Bind();
    SetModelUniforms(*m_NormalMapShader);
    m_3DModel->Draw(*m_NormalMapShader);
}

void test::TestPBRNormalMap::RenderGUI()
{
    TestPBR::RenderGUI();
    ImGui::Separator();
    ImGui::Text("Normal Mapping");
    ImGui::Checkbox("Enable Normal Map", &m_UseNormalMap);
    ImGui::SliderFloat("Normal Strength", &m_NormalMapScale, 0.0f, 3.0f);
}
