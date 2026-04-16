#pragma once
#include "TestPBR.h"
#include "../Mesh/Model.h"
#include <memory>

namespace test
{
    class TestPBRNormalMap : public TestPBR
    {
    public:
        TestPBRNormalMap(GLFWwindow* window);
        ~TestPBRNormalMap() override = default;

        void Update(float deltaTime) override;
        void Render()    override;
        void RenderGUI() override;

    protected:
        void SetModelUniforms(Shader& shader);

        std::unique_ptr<Model>  m_3DModel;
        std::unique_ptr<Shader> m_NormalMapShader;
        bool  m_UseNormalMap   = true;
        float m_NormalMapScale = 1.0f;
    };
}
