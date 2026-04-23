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

        GLFWwindow* m_Window;
        std::unique_ptr<Camera>         m_Camera;
        std::unique_ptr<Mesh>           m_Mesh;
        std::unique_ptr<Shader>         m_ObjectShader;
        std::unique_ptr<Shader>         m_ScreenShader;

        // The multisampled FBO is the scene render target.  Instead of writing one colour
        // value per pixel, the GPU allocates N sub-pixel sample slots per pixel (N = sample
        // count).  Each sample is shaded independently so polygon edges are covered by
        // different fractions of the sample grid, producing smoother silhouettes.
        // Multisampled renderbuffers cannot be directly bound as textures in a shader, so
        // they must be resolved before the result can be read back.
        std::unique_ptr<MSAAFramebuffer> m_MSAAFBO;

        // The resolve FBO is a plain single-sample framebuffer whose colour attachment IS a
        // regular GL_TEXTURE_2D.  After the scene is drawn into m_MSAAFBO, glBlitFramebuffer
        // copies it here, averaging the N samples down to one colour per pixel.  The result
        // is an anti-aliased image stored in a texture that the screen-space quad shader can
        // sample normally in Pass 3.
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
