#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Tests.h"
#include "../Shader.h"
#include "../Mesh/GeometryFactory.h"
#include "../utils/Camera.h"
#include <memory>
#include "glm/glm.hpp"

namespace test
{
	class TestPBR : public Tests
	{
	public:
		TestPBR(GLFWwindow* window);
		virtual ~TestPBR() = default;

		void Update(float deltaTime) override;
		void Render() override;
		void RenderGUI() override;

	protected:
		void ApplyMaterialPreset(int preset);

	protected:
		GLFWwindow* m_Window;

		std::unique_ptr<Camera> m_Camera;
		std::unique_ptr<Mesh> m_Sphere;
		std::unique_ptr<Shader> m_PBRShader;

		// PBR material properties
		glm::vec3 m_Albedo;
		float m_Metallic;
		float m_Roughness;
		float m_AO;

		// Light properties
		glm::vec3 m_LightPosition;
		glm::vec3 m_LightColor;
		float m_LightIntensity;

		// Transformations
		glm::mat4 m_Model;
		glm::mat4 m_View;
		glm::mat4 m_Projection;

		// Display options
		bool m_Wireframe = false;
		bool m_RotateModel = true;
	};
}
