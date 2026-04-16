#include "TestHighDensityMesh.h"
#include "../Renderer.h"
#include <imgui.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace test {

    TestHighDensityMesh::TestHighDensityMesh(GLFWwindow* window)
        : m_window(window),
        m_ModelRotationSpeed(0.5f)
    {
        glEnable(GL_DEPTH_TEST);

        m_Camera = std::make_unique<Camera>(
            window,
            glm::vec3(0.0f, 0.0f, 3.0f),  // position
            glm::vec3(0.0f, 1.0f, 0.0f),  // world up
            -90.0f,                         // yaw
            0.0f,                           // pitch
            45.0f                           // fov
        );

        m_Model  = std::make_unique<Model>("res/Model/Creative_Character_free.obj");
        m_Shader = std::make_unique<Shader>("res/Shaders/Mesh.shader");

		InitDefaultScene();
    }

    void TestHighDensityMesh::Update(float deltaTime) {
        m_Camera->processInput(deltaTime);
        m_Camera->Update(deltaTime);

        static float rotation = 0.0f;
        rotation += m_ModelRotationSpeed * deltaTime;
        m_ModelMatrix = glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0, 1, 0));

        m_View       = m_Camera->getViewMatrix();
        m_Projection = glm::perspective(glm::radians(m_Camera->getFOV()), 800.0f / 600.0f, 0.1f, 1000.0f);
    }

    void TestHighDensityMesh::Render() {
        Renderer renderer;
        renderer.Clear();

        m_DefaultScene->Render(m_View, m_Projection);

        m_Shader->Bind();

        // MVP
        m_Shader->setUniformMat4f("u_Model",      m_ModelMatrix);
        m_Shader->setUniformMat4f("u_View",       m_View);
        m_Shader->setUniformMat4f("u_Projection", m_Projection);

        // Lighting
        glm::vec3 camPos = m_Camera->getPosition();
        m_Shader->setUniform3f("u_CameraPos",    camPos.x, camPos.y, camPos.z);
        m_Shader->setUniform3f("u_LightPos",     5.0f, 10.0f, 5.0f);
        m_Shader->setUniform3f("u_LightColor",   1.0f,  1.0f, 1.0f);
        m_Shader->setUniform1f("u_AmbientStrength",  0.15f);
        m_Shader->setUniform1f("u_SpecularStrength",  0.5f);
        m_Shader->setUniform1f("u_Shininess",        32.0f);
        m_Shader->setUniform1i("u_UseDiffuseTexture",  1);
        m_Shader->setUniform1i("u_UseSpecularTexture", 0);

        m_Model->Draw(*m_Shader);
    }

    void TestHighDensityMesh::RenderGUI() {
        m_Camera->cameraGUI();
    }

}
