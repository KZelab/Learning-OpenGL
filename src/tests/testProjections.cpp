#include "testProjections.h"
#include "../Renderer.h"
#include "../vendor/imgui/imgui.h"     // Dear ImGui library for GUI elements
#include <GL/glew.h>     
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"              // GLM for mathematical operations and matrix manipulation
#include "glm/gtc/matrix_transform.hpp" // For GLM transformations, like translating matrices
namespace test
{
    TestProjections::TestProjections() 
    {

        useOrthographic = true;


        m_Shader = std::make_unique<Shader>(R"(res/Shaders/MVPVertexColour.shader)");


        m_Cube = GeometryFactory::CreateCube();

        // Enable depth testing
        glEnable(GL_DEPTH_TEST);

        // Unbind objects to prevent accidental modifications or conflicts
        m_Shader->Unbind(); // Unbind Shader Program to ensure clean state
	}
	void TestProjections::Update(float deltaTime)
	{

	}

	void TestProjections::Render()
	{
        Renderer renderer;
        renderer.ClearColour_White();

        renderer.Clear();

        m_Shader->Bind();


        // Model matrix: rotating cube
        glm::mat4 model = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
        m_Shader->setUniformMat4f("model", model);
        
        // View matrix: camera at (0, 0, 3)
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
        m_Shader->setUniformMat4f("view", view);
       

        glm::mat4 projection;
        if (useOrthographic) {
            std::cout << "Orthographic mode \n";
            projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
        }
        else {
            std::cout << "perspective mode \n";
            projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        }
        m_Shader->setUniformMat4f("projection", projection); 

        m_Cube->Draw();


	}

	void TestProjections::RenderGUI()
	{
        ImGui::Text("Currently using: %s mode", useOrthographic ? "Orthographic" : "Perspective");

        if(!useOrthographic)
	        if (ImGui::Button("Orthographic mode"))
	        {
	            useOrthographic = true;
	        }
        if(useOrthographic)
	        if (ImGui::Button("Perspective mode"))
	        {
	            useOrthographic = false;
	        }        
	}



}
