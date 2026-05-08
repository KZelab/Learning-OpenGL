#include <GL/glew.h>                // GLEW library for managing OpenGL extensions
#include <GLFW/glfw3.h>             // GLFW for window management and input handling
#include <iostream>                 // For console output

int main() {
    // =================================================================
    // STEP 1: Initialize GLFW
    // =================================================================
    // GLFW is a library that handles window creation, input, and OpenGL context management.
    // It abstracts away platform-specific window creation code.
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // =================================================================
    // STEP 2: Configure OpenGL Context
    // =================================================================
    // Tell GLFW what version of OpenGL we want to use.
    // We're requesting OpenGL 3.3 Core Profile, which gives us modern OpenGL
    // without the deprecated legacy functions.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Major version 3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // Minor version 3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Core profile (modern OpenGL)

    // =================================================================
    // STEP 3: Create Window and OpenGL Context
    // =================================================================
    // Create a 960x540 window with the title "Learning OpenGL - Window Setup"
    GLFWwindow* window = glfwCreateWindow(960, 540, "Learning OpenGL - Window Setup", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Make the window's OpenGL context current on the calling thread
    glfwMakeContextCurrent(window);

    // =================================================================
    // STEP 4: Initialize GLEW
    // =================================================================
    // GLEW (GL Extension Wrangler) loads OpenGL function pointers.
    // Modern OpenGL functions are loaded at runtime, not linked at compile time.
    glewExperimental = GL_TRUE; // Enable modern OpenGL functionality
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Print OpenGL version to verify everything is working
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Graphics Card: " << glGetString(GL_RENDERER) << std::endl;

    // Enable V-Sync (synchronize with monitor refresh rate)
    glfwSwapInterval(1);

    // =================================================================
    // STEP 5: Set Clear Color
    // =================================================================
    // Set the color that will be used when clearing the screen
    // RGBA values: (Red, Green, Blue, Alpha) - each from 0.0 to 1.0
    glClearColor(0.2f, 0.3f, 0.8f, 1.0f); // Nice blue background

    // =================================================================
    // STEP 6: Main Render Loop
    // =================================================================
    // Keep the window open and responsive until the user closes it
    while (!glfwWindowShouldClose(window)) {

        // Clear the screen with our chosen clear color
        glClear(GL_COLOR_BUFFER_BIT);

        // At this point, we would draw our 3D objects...
        // But for now, we just have a colored background!

        // Swap the front and back buffers (double buffering)
        // This prevents flickering by drawing to a hidden buffer first
        glfwSwapBuffers(window);

        // Check for window events (keyboard, mouse, window resize, etc.)
        glfwPollEvents();
    }

    // =================================================================
    // STEP 7: Cleanup
    // =================================================================
    // Destroy the window and clean up GLFW resources
    glfwDestroyWindow(window);
    glfwTerminate();

    std::cout << "Window closed successfully!" << std::endl;
    return 0;
}
