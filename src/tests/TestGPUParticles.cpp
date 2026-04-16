#include "TestGPUParticles.h"
#include <cstring>

namespace test
{
    // Particle struct matching the GLSL layout (std430)
    struct GPUParticle
    {
        glm::vec2 pos;
        glm::vec2 vel;
        glm::vec4 color;
        float life;
        float maxLife;
        float size;
        float _pad;
    };

    TestGPUParticles::TestGPUParticles(GLFWwindow* /*window*/)
        : m_SSBO(0)
        , m_VAO(0)
        , m_EmitterPos(480.0f, 300.0f)
        , m_Gravity(-200.0f)
        , m_EmissionRate(10000000)
        , m_SpeedMin(50.0f)
        , m_SpeedMax(200.0f)
        , m_LifeMin(1.0f)
        , m_LifeMax(3.0f)
        , m_SizeMin(2.0f)
        , m_SizeMax(8.0f)
        , m_MaxParticles(10000000)
        , m_ColourStart(1.0f, 0.6f, 0.1f, 1.0f)
        , m_ColourEnd(1.0f, 0.0f, 0.0f, 0.0f)
        , m_Time(0.0f)
        , m_QueryBack(0)
        , m_ComputeTimeMs(0.0f)
        , m_RenderTimeMs(0.0f)
        , m_FrameHistoryIdx(0)
    {
        memset(m_FrameTimeHistory, 0, sizeof(m_FrameTimeHistory));
        memset(m_ComputeTimeHistory, 0, sizeof(m_ComputeTimeHistory));
        memset(m_RenderTimeHistory, 0, sizeof(m_RenderTimeHistory));

        // Create GL timer queries (double-buffered: write to front, read from back)
        GlCall(glGenQueries(2, m_QueryCompute));
        GlCall(glGenQueries(2, m_QueryRender));
        // Issue dummy queries so the first readback doesn't stall
        for (int i = 0; i < 2; i++)
        {
            GlCall(glBeginQuery(GL_TIME_ELAPSED, m_QueryCompute[i]));
            GlCall(glEndQuery(GL_TIME_ELAPSED));
            GlCall(glBeginQuery(GL_TIME_ELAPSED, m_QueryRender[i]));
            GlCall(glEndQuery(GL_TIME_ELAPSED));
        }

        // Initialize all particles as dead
        std::vector<GPUParticle> particles(MAX_PARTICLES);
        for (auto& p : particles)
        {
            p.pos = glm::vec2(0.0f);
            p.vel = glm::vec2(0.0f);
            p.color = glm::vec4(0.0f);
            p.life = 0.0f;
            p.maxLife = 1.0f;
            p.size = 0.0f;
            p._pad = 0.0f;
        }

        // Create SSBO
        GlCall(glGenBuffers(1, &m_SSBO));
        GlCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO));
        GlCall(glBufferData(GL_SHADER_STORAGE_BUFFER,
            MAX_PARTICLES * sizeof(GPUParticle),
            particles.data(), GL_DYNAMIC_DRAW));
        GlCall(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_SSBO));
        GlCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0));

        // Create empty VAO for vertex-pulling render
        GlCall(glGenVertexArrays(1, &m_VAO));

        // Load shaders
        m_ComputeShader = std::make_unique<ComputeShader>(R"(res/Shaders/GPUParticleCompute.glsl)");
        m_RenderShader = std::make_unique<Shader>(R"(res/Shaders/GPUParticleRender.shader)");

        // Orthographic projection matching the window
        m_Proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);
        m_View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f));

        //InitDefaultScene();
    }

    TestGPUParticles::~TestGPUParticles()
    {
        GlCall(glDeleteQueries(2, m_QueryCompute));
        GlCall(glDeleteQueries(2, m_QueryRender));
        GlCall(glDeleteBuffers(1, &m_SSBO));
        GlCall(glDeleteVertexArrays(1, &m_VAO));
    }

    void TestGPUParticles::Update(float deltaTime)
    {
        if (deltaTime > 0.1f) deltaTime = 0.1f;
        m_Time += deltaTime;

        // Read back timer results from the PREVIOUS frame (no stall — it's 1 frame old)
        int back = m_QueryBack;
        GLuint64 computeNs = 0, renderNs = 0;
        GlCall(glGetQueryObjectui64v(m_QueryCompute[back], GL_QUERY_RESULT, &computeNs));
        GlCall(glGetQueryObjectui64v(m_QueryRender[back], GL_QUERY_RESULT, &renderNs));
        m_ComputeTimeMs = static_cast<float>(computeNs) / 1000000.0f;
        m_RenderTimeMs = static_cast<float>(renderNs) / 1000000.0f;

        // Record history
        m_FrameTimeHistory[m_FrameHistoryIdx] = deltaTime * 1000.0f;
        m_ComputeTimeHistory[m_FrameHistoryIdx] = m_ComputeTimeMs;
        m_RenderTimeHistory[m_FrameHistoryIdx] = m_RenderTimeMs;
        m_FrameHistoryIdx = (m_FrameHistoryIdx + 1) % FRAME_HISTORY_SIZE;

        // Swap query buffers: now write to the one we just read
        int front = 1 - back;
        m_QueryBack = front;

        // Bind SSBO
        GlCall(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_SSBO));

        // Time the compute dispatch
        GlCall(glBeginQuery(GL_TIME_ELAPSED, m_QueryCompute[front]));

        m_ComputeShader->Bind();
        m_ComputeShader->setUniform1f("u_DeltaTime", deltaTime);
        m_ComputeShader->setUniform1f("u_Gravity", m_Gravity);
        m_ComputeShader->setUniform2f("u_EmitterPos", m_EmitterPos.x, m_EmitterPos.y);
        m_ComputeShader->setUniform1i("u_MaxParticles", m_MaxParticles);
        m_ComputeShader->setUniform1f("u_Time", m_Time);
        m_ComputeShader->setUniform1f("u_EmissionRate", m_EmissionRate);
        m_ComputeShader->setUniform4f("u_ColorStart", m_ColourStart.r, m_ColourStart.g, m_ColourStart.b, m_ColourStart.a);
        m_ComputeShader->setUniform4f("u_ColorEnd", m_ColourEnd.r, m_ColourEnd.g, m_ColourEnd.b, m_ColourEnd.a);
        m_ComputeShader->setUniform1f("u_SpeedMin", m_SpeedMin);
        m_ComputeShader->setUniform1f("u_SpeedMax", m_SpeedMax);
        m_ComputeShader->setUniform1f("u_LifeMin", m_LifeMin);
        m_ComputeShader->setUniform1f("u_LifeMax", m_LifeMax);
        m_ComputeShader->setUniform1f("u_SizeMin", m_SizeMin);
        m_ComputeShader->setUniform1f("u_SizeMax", m_SizeMax);

        unsigned int numGroups = (m_MaxParticles + WORK_GROUP_SIZE - 1) / WORK_GROUP_SIZE;
        m_ComputeShader->Dispatch(numGroups, 1, 1);

        GlCall(glEndQuery(GL_TIME_ELAPSED));

        // Memory barrier to ensure compute writes are visible to vertex shader
        GlCall(glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT));

        m_ComputeShader->Unbind();
    }

    void TestGPUParticles::Render()
    {
       // m_DefaultScene->Render(m_View, m_Proj);

        int front = 1 - m_QueryBack;
        GlCall(glBeginQuery(GL_TIME_ELAPSED, m_QueryRender[front]));

        GlCall(glEnable(GL_BLEND));
        GlCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE));
        GlCall(glEnable(GL_PROGRAM_POINT_SIZE));

        // Bind SSBO for vertex pulling
        GlCall(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_SSBO));

        glm::mat4 mvp = m_Proj * m_View;
        m_RenderShader->Bind();
        m_RenderShader->setUniformMat4f("u_MVP", mvp);

        GlCall(glBindVertexArray(m_VAO));
        GlCall(glDrawArrays(GL_POINTS, 0, m_MaxParticles));
        GlCall(glBindVertexArray(0));

        m_RenderShader->Unbind();

        GlCall(glDisable(GL_PROGRAM_POINT_SIZE));
        GlCall(glDisable(GL_BLEND));

        GlCall(glEndQuery(GL_TIME_ELAPSED));
    }

    void TestGPUParticles::RenderGUI()
    {
        // --- Performance Section ---
        float fps = ImGui::GetIO().Framerate;
        float frameMs = 1000.0f / fps;

        ImGui::Text("%.1f FPS (%.3f ms/frame)", fps, frameMs);

        if (ImGui::CollapsingHeader("Performance", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // GPU timing breakdown
            ImGui::Text("GPU Compute:  %.3f ms", m_ComputeTimeMs);
            ImGui::Text("GPU Render:   %.3f ms", m_RenderTimeMs);
            ImGui::Text("GPU Total:    %.3f ms", m_ComputeTimeMs + m_RenderTimeMs);

            ImGui::Separator();

            // Frame time graph
            float maxFrameTime = 0.0f;
            for (int i = 0; i < FRAME_HISTORY_SIZE; i++)
                if (m_FrameTimeHistory[i] > maxFrameTime) maxFrameTime = m_FrameTimeHistory[i];
            if (maxFrameTime < 1.0f) maxFrameTime = 1.0f;

            char overlay[64];
            snprintf(overlay, sizeof(overlay), "Frame: %.2f ms", m_FrameTimeHistory[(m_FrameHistoryIdx - 1 + FRAME_HISTORY_SIZE) % FRAME_HISTORY_SIZE]);
            ImGui::PlotLines("Frame Time", m_FrameTimeHistory, FRAME_HISTORY_SIZE, m_FrameHistoryIdx, overlay, 0.0f, maxFrameTime * 1.2f, ImVec2(0, 60));

            // Compute vs Render stacked-ish view
            snprintf(overlay, sizeof(overlay), "Compute: %.3f ms", m_ComputeTimeMs);
            ImGui::PlotHistogram("Compute", m_ComputeTimeHistory, FRAME_HISTORY_SIZE, m_FrameHistoryIdx, overlay, 0.0f, maxFrameTime * 0.5f, ImVec2(0, 40));

            snprintf(overlay, sizeof(overlay), "Render: %.3f ms", m_RenderTimeMs);
            ImGui::PlotHistogram("Render", m_RenderTimeHistory, FRAME_HISTORY_SIZE, m_FrameHistoryIdx, overlay, 0.0f, maxFrameTime * 0.5f, ImVec2(0, 40));

            ImGui::Separator();

            // Memory usage
            float ssboMB = static_cast<float>(m_MaxParticles * sizeof(GPUParticle)) / (1024.0f * 1024.0f);
            float ssboMaxMB = static_cast<float>(MAX_PARTICLES * sizeof(GPUParticle)) / (1024.0f * 1024.0f);
            ImGui::Text("SSBO Memory:  %.1f MB / %.1f MB allocated", ssboMB, ssboMaxMB);
            ImGui::ProgressBar(ssboMB / ssboMaxMB, ImVec2(-1, 0), "");

            // Work group info
            unsigned int numGroups = (m_MaxParticles + WORK_GROUP_SIZE - 1) / WORK_GROUP_SIZE;
            ImGui::Text("Work Groups:  %u  (local size: %u)", numGroups, WORK_GROUP_SIZE);
            ImGui::Text("GPU Threads:  %u", numGroups * WORK_GROUP_SIZE);
        }

        ImGui::Separator();

        // --- Emitter Settings ---
        ImGui::Text("Particles: %d", m_MaxParticles);

        if (ImGui::CollapsingHeader("Emitter Settings", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::SliderFloat2("Emitter Pos", &m_EmitterPos.x, 0.0f, 960.0f);
            ImGui::SliderInt("Max Particles", &m_MaxParticles, 1000, static_cast<int>(MAX_PARTICLES));
            ImGui::SliderFloat("Emission Rate", &m_EmissionRate, 100.0f, 100000.0f);
            ImGui::SliderFloat("Gravity", &m_Gravity, -500.0f, 500.0f);
        }

        if (ImGui::CollapsingHeader("Particle Properties"))
        {
            ImGui::SliderFloat("Speed Min", &m_SpeedMin, 0.0f, 500.0f);
            ImGui::SliderFloat("Speed Max", &m_SpeedMax, 0.0f, 500.0f);
            ImGui::SliderFloat("Life Min", &m_LifeMin, 0.1f, 10.0f);
            ImGui::SliderFloat("Life Max", &m_LifeMax, 0.1f, 10.0f);
            ImGui::SliderFloat("Size Min", &m_SizeMin, 1.0f, 30.0f);
            ImGui::SliderFloat("Size Max", &m_SizeMax, 1.0f, 30.0f);
        }

        if (ImGui::CollapsingHeader("Colours"))
        {
            ImGui::ColorEdit4("Colour Start", &m_ColourStart.x);
            ImGui::ColorEdit4("Colour End", &m_ColourEnd.x);
        }
    }
}
