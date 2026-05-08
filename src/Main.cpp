#include <GL/glew.h>                // GLEW library for managing OpenGL extensions
#include <GLFW/glfw3.h>             // GLFW for window management and input handling
#include <iostream>                 // For console output
#include <string>

#include "glm/glm.hpp"              // GLM for mathematical operations and matrix manipulation
#include "glm/gtc/matrix_transform.hpp" // For GLM transformations, like translating matrices

#include "VertexBufferLayout.h"     // Custom layout wrapper for vertex attributes
#include "Renderer.h"               // Custom renderer wrapper
#include "Shader.h"                 // Custom shader wrapper
#include "tests/Tests.h"
#include "tests/testClearColour.h"

#include "vendor/imgui/imgui.h"     // Dear ImGui library for GUI elements
#include "vendor/imgui/imgui_impl_glfw.h" // ImGui GLFW backend
#include "vendor/imgui/imgui_impl_opengl3.h" // ImGui OpenGL backend



#define IMGUI_IMPL_OPENGL_LOADER_GLEW // ImGui macro to specify GLEW as the OpenGL loader

int main() {
    // Initialize GLFW to create a window and manage the OpenGL context
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW \n";
        return -1;
    }

    // Set GLFW window hints for OpenGL version and profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Major version 3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // Minor version 3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Core profile

    // Create a windowed mode window and OpenGL context
    GLFWwindow* window = glfwCreateWindow(960, 540, "GLFW Window", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW to manage OpenGL function pointers and extensions
    glewExperimental = GL_TRUE; // Enable modern OpenGL functionality
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }
    std::cout << "GLEW VERSION:" << glGetString(GL_VERSION) << std::endl;

    glfwSwapInterval(1); // Enable V-Sync for smoother rendering


    // Scoped block for managing OpenGL resources
    //Resource Management (RAII): Using curly braces { ... } creates a block scope in C++. 
    // When you define variables within this scope, they automatically go out of scope (and are destroyed) when execution leaves that block. 
    // This helps with automatic resource management for objects like VertexArray, VertexBuffer, IndexBuffer, 
    // and others that might be handling OpenGL resources (like buffers or shaders).
    // By structuring your code this way, you ensure that certain objects are destructed at the right time and their resources are released without needing manual cleanup code.



        Renderer renderer; // Renderer object for handling draw calls

        // Setup Dear ImGui context for GUI rendering
        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(window, true); // Initialize ImGui for OpenGL + GLFW
        ImGui::StyleColorsDark(); // Set default dark theme

        if (!ImGui_ImplOpenGL3_Init()) {
            std::cerr << "ImGui OpenGL3 initialization failed" << std::endl;
            return -1;
        }
        test::Tests* currentTest = nullptr;
        test::TestMenu* TestMenu = new test::TestMenu(currentTest);

        currentTest = TestMenu;

        // Register tests in the specified order for branch 10
        TestMenu->RegisterTest<test::testClearColour>("Test Clear Colour");


        float lastTimeFrame = 0.0f;
        float deltaTime = 0.0f;


	// Main rendering loop
        while (!glfwWindowShouldClose(window)) {

            float currentFrameTime = static_cast<float>(glfwGetTime());

            deltaTime = currentFrameTime - lastTimeFrame;

            lastTimeFrame = currentFrameTime;

            renderer.Clear(); // Clear the screen to prepare for a new frame
            //renderer.ClearColour_White();

            // Start ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            if (currentTest)
            {
                currentTest->Update(deltaTime);
                currentTest->Render();
                ImGui::Begin("Test control panel");
                if (currentTest != TestMenu && ImGui::Button("<-"))
                {
                    delete currentTest;
                    currentTest = TestMenu;
                }
                    currentTest->RenderGUI();
                ImGui::End();
            }
            ImGui::Render(); // Render ImGui frame
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); // Draw ImGui to the screen
        
            // Swap front and back buffers
            glfwSwapBuffers(window);
            // Poll events (keyboard, mouse, etc.)
            glfwPollEvents();
        }

        delete currentTest;
        if(currentTest != TestMenu)
            delete TestMenu;

    // Shutdown ImGui and GLFW
    ImGui_ImplGlfw_Shutdown();
    // Cleanup resources and destroy window
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
