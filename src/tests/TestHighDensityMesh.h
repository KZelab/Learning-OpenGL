#pragma once
#include "Tests.h"
#include "../Shader.h"
#include "../utils/Camera.h"

#include "../Mesh/Model.h"

#include "GL/glew.h"
#include <GLFW/glfw3.h>
#include <memory>
#include "glm/glm.hpp"

namespace test {
	class TestHighDensityMesh : public Tests { //not really worth doing in class, will just be a copy of TestPBRNormalMap but with a different model, 
        
    public:
        TestHighDensityMesh(GLFWwindow* window);

        void Update(float deltaTime) override;
        void Render() override;
        void RenderGUI() override;

    private:
        GLFWwindow* m_window;

        std::unique_ptr<Camera> m_Camera;

        // Transformations
        glm::mat4 m_ModelMatrix;
        glm::mat4 m_View;
        glm::mat4 m_Projection;

        // Model and shader
        std::unique_ptr<Model>  m_Model;
        std::unique_ptr<Shader> m_Shader;

        float m_ModelRotationSpeed;
    };
}
