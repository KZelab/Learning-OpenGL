#include "TestMSAA.h"
#include "../Renderer.h"
#include "../VertexBufferLayout.h"
#include "imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

static const int s_SampleOptions[] = { 1, 2, 4, 8 };

test::TestMSAA::TestMSAA(GLFWwindow* window)
    : m_Window(window),
    m_ObjectColour(0.2f, 0.6f, 0.9f)
{
    glfwGetFramebufferSize(m_Window, &m_Width, &m_Height);

    m_Camera = std::make_unique<Camera>(
        window,
        glm::vec3(0.0f, 0.0f, 5.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        -90.0f,
        0.0f,
        45.0f
    );

    m_ObjectShader = std::make_unique<Shader>("res/shaders/testmsaaobject.shader");
    m_ScreenShader = std::make_unique<Shader>("res/shaders/testmsaascreen.shader");

    // Low subdivision count makes polygon edge aliasing clearly visible
    m_Mesh = GeometryFactory::CreateSphere(8, 8);

    float positions[] = {
        -1.0f, -1.0f,   0.0f, 0.0f,
         1.0f, -1.0f,   1.0f, 0.0f,
         1.0f,  1.0f,   1.0f, 1.0f,
        -1.0f,  1.0f,   0.0f, 1.0f
    };
    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

    m_QuadVAO = std::make_unique<VertexArray>();
    m_QuadVBO = std::make_unique<VertexBuffer>(positions, 4 * 4 * sizeof(float));

    VertexBufferLayout layout;
    layout.Push<float>(2); // position
    layout.Push<float>(2); // texcoord
    m_QuadVAO->AddBuffer(*m_QuadVBO, layout);

    m_QuadIBO = std::make_unique<IndexBuffer>(indices, 6);

    m_QuadVAO->unBind();
    m_QuadVBO->Unbind();
    m_QuadIBO->Unbind();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    BuildFramebuffers();
}

test::TestMSAA::~TestMSAA()
{
}

// -----------------------------------------------------------------------------
// Framebuffer management
// -----------------------------------------------------------------------------

void test::TestMSAA::BuildFramebuffers()
{
    int samples = m_MSAAEnabled ? s_SampleOptions[m_SampleIndex] : 1;

    m_MSAAFBO = std::make_unique<MSAAFramebuffer>(m_Width, m_Height, samples);
    m_ResolveFBO = std::make_unique<Framebuffer>(m_Width, m_Height);
}

// -----------------------------------------------------------------------------
// Per-frame logic
// -----------------------------------------------------------------------------

void test::TestMSAA::Update(float deltaTime)
{
    m_Camera->processInput(deltaTime);
    m_Camera->Update(deltaTime);

    m_Model = glm::mat4(1.0f);
    float angle = static_cast<float>(glfwGetTime()) * 20.0f;
    m_Model = glm::rotate(m_Model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));

    m_View = m_Camera->getViewMatrix();
    m_Projection = glm::perspective(
        glm::radians(m_Camera->getFOV()),
        static_cast<float>(m_Width) / static_cast<float>(m_Height),
        0.1f, 1000.0f
    );
}

void test::TestMSAA::RenderScene()
{
    m_ObjectShader->Bind();
    m_ObjectShader->setUniformMat4f("u_Model", m_Model);
    m_ObjectShader->setUniformMat4f("u_View", m_View);
    m_ObjectShader->setUniformMat4f("u_Projection", m_Projection);
    m_ObjectShader->setUniform3f("u_Colour",
        m_ObjectColour.r, m_ObjectColour.g, m_ObjectColour.b);

    m_Mesh->setPosition(glm::vec3(0.0f));
    m_Mesh->Draw();
}

void test::TestMSAA::Render()
{
    if (m_FBODirty)
    {
        BuildFramebuffers();
        m_FBODirty = false;
    }

    Renderer renderer;

    // ------------------------------------------------------------------
    // Pass 1: render scene into the multisampled FBO
    // ------------------------------------------------------------------
    m_MSAAFBO->Bind();
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    renderer.Clear();
    glEnable(GL_DEPTH_TEST);

    RenderScene();

    // ------------------------------------------------------------------
    // Pass 2: resolve multisampled buffer into the single-sample FBO
    // ------------------------------------------------------------------
    m_MSAAFBO->Resolve(*m_ResolveFBO);

    // ------------------------------------------------------------------
    // Pass 3: draw the resolved texture to the default framebuffer
    // ------------------------------------------------------------------
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_Width, m_Height);
    glDisable(GL_DEPTH_TEST);

    renderer.Clear();

    m_ScreenShader->Bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_ResolveFBO->GetColorTexture());
    m_ScreenShader->setUniform1i("u_ScreenTexture", 0);

    renderer.Draw(*m_QuadVAO, *m_QuadIBO, *m_ScreenShader);

    glEnable(GL_DEPTH_TEST);
}

// -----------------------------------------------------------------------------
// GUI
// -----------------------------------------------------------------------------

void test::TestMSAA::RenderGUI()
{
    ImGui::Text("MSAA Settings");
    ImGui::Separator();

    bool changed = false;

    if (ImGui::Checkbox("Enable MSAA", &m_MSAAEnabled))
        changed = true;

    ImGui::BeginDisabled(!m_MSAAEnabled);
    {
        const char* sampleLabels[] = { "1x", "2x", "4x", "8x" };
        if (ImGui::Combo("Sample Count", &m_SampleIndex, sampleLabels, IM_ARRAYSIZE(sampleLabels)))
            changed = true;

        int activeSamples = m_MSAAEnabled ? s_SampleOptions[m_SampleIndex] : 1;
        ImGui::Text("Active samples: %d", activeSamples);
    }
    ImGui::EndDisabled();

    if (changed)
        m_FBODirty = true;

    ImGui::Separator();
    ImGui::Text("Object");
    ImGui::ColorEdit3("Colour", glm::value_ptr(m_ObjectColour));

    if (ImGui::Checkbox("Wireframe", &m_Wireframe))
        glPolygonMode(GL_FRONT_AND_BACK, m_Wireframe ? GL_LINE : GL_FILL);

    ImGui::Separator();
    ImGui::TextDisabled("Toggle MSAA or reduce sample count to");
    ImGui::TextDisabled("observe aliasing on polygon edges.");
}
