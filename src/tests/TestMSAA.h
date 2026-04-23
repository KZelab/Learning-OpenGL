#pragma once
#include "Tests.h"
#include "../Shader.h"
#include "../Mesh/GeometryFactory.h"
#include "../utils/Camera.h"
#include "../Framebuffer.h"
#include "../MSAAFramebuffer.h"
#include "../VertexArray.h"
#include "../VertexBuffer.h"
#include "../IndexBuffer.h"
#include <memory>
#include "GL/glew.h"
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"

namespace test
{
    class TestMSAA : public Tests
    {
    public:
        TestMSAA(GLFWwindow* window);
        ~TestMSAA();
        void Update(float deltaTime) override;
        void Render() override;
        void RenderGUI() override;

    private:
        void BuildFramebuffers();
        void RenderScene();

        GLFWwindow* m_Window;
        std::unique_ptr<Camera>         m_Camera;
        std::unique_ptr<Mesh>           m_Mesh;
        std::unique_ptr<Shader>         m_ObjectShader;
        std::unique_ptr<Shader>         m_ScreenShader;

        std::unique_ptr<MSAAFramebuffer> m_MSAAFBO;
        std::unique_ptr<Framebuffer>     m_ResolveFBO;

        std::unique_ptr<VertexArray>    m_QuadVAO;
        std::unique_ptr<VertexBuffer>   m_QuadVBO;
        std::unique_ptr<IndexBuffer>    m_QuadIBO;

        glm::mat4 m_Model;
        glm::mat4 m_View;
        glm::mat4 m_Projection;

        glm::vec3 m_ObjectColour;

        bool m_MSAAEnabled = true;
        bool m_Wireframe = false;
        bool m_FBODirty = false;
        int  m_SampleIndex = 2;   // 0:1x  1:2x  2:4x  3:8x

        int m_Width;
        int m_Height;
    };
}
