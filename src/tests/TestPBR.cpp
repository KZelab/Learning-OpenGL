#include "TestPBR.h"
#include "../Renderer.h"
#include "../vendor/imgui/imgui.h"
#include <glm/gtc/type_ptr.hpp>

test::TestPBR::TestPBR(GLFWwindow* window)
	: m_Window(window),
	  m_Albedo(0.5f, 0.0f, 0.0f),
	  m_Metallic(0.5f),
	  m_Roughness(0.5f),
	  m_AO(1.0f),
	  m_LightPosition(10.0f, 10.0f, 10.0f),
	  m_LightColor(1.0f, 1.0f, 1.0f),
	  m_LightIntensity(300.0f)
{
	m_Camera = std::make_unique<Camera>(
		window,
		glm::vec3(0.0f, 0.0f, 5.0f),  // position
		glm::vec3(0.0f, 1.0f, 0.0f),  // up vector
		-90.0f,                        // yaw
		0.0f,                          // pitch
		45.0f                          // FOV
	);

	m_PBRShader = std::make_unique<Shader>("res/Shaders/Lighting/PBR.shader");

	m_Sphere = GeometryFactory::CreateSphere(32, 32);

	glEnable(GL_DEPTH_TEST);
}

void test::TestPBR::Update(float deltaTime)
{
	m_Camera->processInput(deltaTime);
	m_Camera->Update(deltaTime);

	m_Model = glm::mat4(1.0f);
	if (m_RotateModel)
	{
		float angle = static_cast<float>(glfwGetTime()) * 20.0f;
		m_Model = glm::rotate(m_Model, glm::radians(angle), glm::vec3(0, 1, 0));
	}

	m_View = m_Camera->getViewMatrix();
	m_Projection = glm::perspective(glm::radians(m_Camera->getFOV()), 800.0f / 600.0f, 0.1f, 1000.0f);
}

void test::TestPBR::Render()
{
	Renderer renderer;
	renderer.Clear();

	m_PBRShader->Bind();

	// Transform uniforms
	m_PBRShader->setUniformMat4f("u_Model", m_Model);
	m_PBRShader->setUniformMat4f("u_View", m_View);
	m_PBRShader->setUniformMat4f("u_Projection", m_Projection);

	// Material uniforms
	m_PBRShader->setUniform3f("u_Albedo", m_Albedo.r, m_Albedo.g, m_Albedo.b);
	m_PBRShader->setUniform1f("u_Metallic", m_Metallic);
	m_PBRShader->setUniform1f("u_Roughness", m_Roughness);
	m_PBRShader->setUniform1f("u_AO", m_AO);

	// Light uniforms
	m_PBRShader->setUniform3f("u_LightPosition", m_LightPosition.x, m_LightPosition.y, m_LightPosition.z);
	m_PBRShader->setUniform3f("u_LightColor", m_LightColor.r, m_LightColor.g, m_LightColor.b);
	m_PBRShader->setUniform1f("u_LightIntensity", m_LightIntensity);

	// Camera position for specular calculation
	glm::vec3 camPos = m_Camera->getPosition();
	m_PBRShader->setUniform3f("u_CameraPosition", camPos.x, camPos.y, camPos.z);

	m_Sphere->setPosition(glm::vec3(0, 0, 0));
	m_Sphere->Draw();
}

void test::TestPBR::RenderGUI()
{
	ImGui::Text("PBR Material Properties");
	ImGui::ColorEdit3("Albedo", glm::value_ptr(m_Albedo));
	ImGui::SliderFloat("Metallic", &m_Metallic, 0.0f, 1.0f);
	ImGui::SliderFloat("Roughness", &m_Roughness, 0.0f, 1.0f);
	ImGui::SliderFloat("Ambient Occlusion", &m_AO, 0.0f, 1.0f);

	ImGui::Separator();
	ImGui::Text("Material Presets");
	if (ImGui::Button("Gold")) { ApplyMaterialPreset(0); }
	ImGui::SameLine();
	if (ImGui::Button("Silver")) { ApplyMaterialPreset(1); }
	ImGui::SameLine();
	if (ImGui::Button("Copper")) { ApplyMaterialPreset(2); }
	if (ImGui::Button("Plastic")) { ApplyMaterialPreset(3); }
	ImGui::SameLine();
	if (ImGui::Button("Rubber")) { ApplyMaterialPreset(4); }

	ImGui::Separator();
	ImGui::Text("Light Properties");
	ImGui::SliderFloat3("Light Position", glm::value_ptr(m_LightPosition), -30.0f, 30.0f);
	ImGui::ColorEdit3("Light Color", glm::value_ptr(m_LightColor));
	ImGui::SliderFloat("Light Intensity", &m_LightIntensity, 0.0f, 1000.0f);

	ImGui::Separator();
	ImGui::Text("Display Options");
	if (ImGui::Checkbox("Wireframe Mode", &m_Wireframe)) {
		glPolygonMode(GL_FRONT_AND_BACK, m_Wireframe ? GL_LINE : GL_FILL);
	}
	ImGui::Checkbox("Rotate Model", &m_RotateModel);
}

void test::TestPBR::ApplyMaterialPreset(int preset)
{
	switch (preset)
	{
	case 0: // Gold
		m_Albedo = glm::vec3(1.0f, 0.765f, 0.336f);
		m_Metallic = 1.0f;
		m_Roughness = 0.3f;
		break;
	case 1: // Silver
		m_Albedo = glm::vec3(0.972f, 0.960f, 0.915f);
		m_Metallic = 1.0f;
		m_Roughness = 0.2f;
		break;
	case 2: // Copper
		m_Albedo = glm::vec3(0.955f, 0.637f, 0.538f);
		m_Metallic = 1.0f;
		m_Roughness = 0.35f;
		break;
	case 3: // Plastic
		m_Albedo = glm::vec3(0.8f, 0.1f, 0.1f);
		m_Metallic = 0.0f;
		m_Roughness = 0.4f;
		break;
	case 4: // Rubber
		m_Albedo = glm::vec3(0.1f, 0.1f, 0.1f);
		m_Metallic = 0.0f;
		m_Roughness = 0.9f;
		break;
	}
}
