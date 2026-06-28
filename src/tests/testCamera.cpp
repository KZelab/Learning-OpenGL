#include "testCamera.h"
#include "../Renderer.h"
#include "../vendor/imgui/imgui.h"
#include <GL/glew.h>
#include "GLFW/glfw3.h"
#include "glm/gtc/matrix_transform.hpp"

namespace test {
    TestCamera::TestCamera(GLFWwindow* window)
        : m_window(window)
    {
        // Create cube using GeometryFactory
        m_Cube = GeometryFactory::CreateCube();
		m_Cube->setPosition(glm::vec3(0.5f, 0.0f, 0.0f));

        // Create camera with initial position and orientation
        m_Camera = std::make_unique<Camera>(
            window,
            glm::vec3(0.0f, 0.0f, 3.0f),  // position
            glm::vec3(0.0f, 1.0f, 0.0f),  // up vector
            -180.0f,                        // yaw
            0.0f,                          // pitch
            45.0f                          // FOV
        );

        m_Shader = std::make_unique<Shader>(R"(res/Shaders/MVPVertexColour.shader)");

        // Enable depth testing
        glEnable(GL_DEPTH_TEST);
		InitDefaultScene();
    }

    void TestCamera::Update(float deltaTime) {
        m_Camera->processInput(deltaTime);
        m_Camera->Update(deltaTime);
    }

    void TestCamera::Render() {
        Renderer renderer;
        renderer.ClearColour_White();
        renderer.Clear();

        glm::mat4 model = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
        glm::mat4 projection = glm::perspective(
            glm::radians(m_Camera->getFOV()),
            800.0f / 600.0f,
            0.1f,
            100.0f
        );


        m_DefaultScene->Render(m_Camera->getViewMatrix(), projection);



        m_Shader->Bind();

        // Set the model matrix (rotating cube)
        
        m_Shader->setUniformMat4f("model", model);

        // Set the view matrix from camera
        m_Shader->setUniformMat4f("view", m_Camera->getViewMatrix());

        // Set the projection matrix
 
        m_Shader->setUniformMat4f("projection", projection);

        m_Cube->Draw();
    }

    void TestCamera::RenderGUI() {
        glm::vec3 pos = m_Camera->getPosition();
        ImGui::Text("Camera Position: (%.1f, %.1f, %.1f)", pos.x, pos.y, pos.z);
        ImGui::Text("Detached: %s", m_Camera->isDetached() ? "Yes" : "No");
        ImGui::Text("FOV: %.1f", m_Camera->getFOV());
    }
}
