#include <GL/glew.h>                // GLEW library for managing OpenGL extensions
#include <GLFW/glfw3.h>             // GLFW for window management and input handling
#include <iostream>                 // For console output
#include <string>                   // For string handling

#include "glm/glm.hpp"              // GLM math library for vectors and matrices
#include "glm/gtc/matrix_transform.hpp" // GLM functions for creating transformation matrices
#include "glm/gtc/type_ptr.hpp"     // GLM functions for converting matrices to arrays

// Our abstracted classes
#include "Shader.h"
#include "Mesh.h"
#include "Renderer.h"

// =================================================================
// TEXTURE LOADING HELPER FUNCTIONS
// =================================================================

// Function to create a simple procedural checkerboard texture
unsigned int CreateCheckerboardTexture() {
    // Create a simple 8x8 checkerboard pattern
    const int width = 8;
    const int height = 8;
    unsigned char textureData[width * height * 3]; // RGB format

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = (y * width + x) * 3;
            // Create checkerboard pattern
            bool isWhite = ((x + y) % 2) == 0;
            unsigned char color = isWhite ? 255 : 64; // White or dark grey

            textureData[index + 0] = color; // Red
            textureData[index + 1] = color; // Green
            textureData[index + 2] = color; // Blue
        }
    }

    // Generate and bind texture
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    return textureID;
}

// Function to create a simple gradient texture
unsigned int CreateGradientTexture() {
    // Create a simple 16x16 gradient pattern
    const int width = 16;
    const int height = 16;
    unsigned char textureData[width * height * 3]; // RGB format

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = (y * width + x) * 3;

            // Create horizontal gradient from red to blue
            unsigned char red = (unsigned char)((255 * x) / (width - 1));
            unsigned char blue = (unsigned char)((255 * y) / (height - 1));
            unsigned char green = 128; // Constant green

            textureData[index + 0] = red;   // Red
            textureData[index + 1] = green; // Green
            textureData[index + 2] = blue;  // Blue
        }
    }

    // Generate and bind texture
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);

    // Set texture parameters for smooth filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return textureID;
}

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
    // Create a 960x540 window with the title "Learning OpenGL - Abstraction"
    GLFWwindow* window = glfwCreateWindow(960, 540, "Learning OpenGL - Abstraction", NULL, NULL);
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
    // STEP 5: Create Renderer and Setup 3D State
    // =================================================================
    // Create our abstracted renderer object
    Renderer renderer;

    // Configure 3D rendering state through renderer
    renderer.EnableDepthTesting();
    renderer.EnableBackfaceCulling();
    renderer.SetClearColour(0.2f, 0.3f, 0.8f, 1.0f); // Nice blue background

    // =================================================================
    // STEP 6: Create Mesh Object
    // =================================================================
    // Use our abstracted Mesh class to create a textured cube
    Mesh* cubeMesh = Mesh::CreateCube();

    // =================================================================
    // STEP 7: Create Shader Object
    // =================================================================
    // Define shader sources as strings
    std::string vertexShader =
        "#version 330 core\n"                          // GLSL version
        "layout (location = 0) in vec3 aPos;\n"        // Input: vertex position
        "layout (location = 1) in vec3 aColour;\n"     // Input: vertex colour
        "layout (location = 2) in vec2 aTexCoord;\n"   // Input: texture coordinates
        "\n"
        "uniform mat4 u_MVP;\n"                        // Model-View-Projection matrix uniform
        "\n"
        "out vec3 vertexColour;\n"                     // Output: colour to fragment shader
        "out vec2 texCoord;\n"                         // Output: texture coordinates to fragment shader
        "\n"
        "void main()\n"
        "{\n"
        "   // Transform vertex position using MVP matrix\n"
        "   gl_Position = u_MVP * vec4(aPos, 1.0);\n"  // Apply transformations
        "   vertexColour = aColour;\n"                 // Pass colour to fragment shader
        "   texCoord = aTexCoord;\n"                   // Pass texture coordinates to fragment shader
        "}\n";

    std::string fragmentShader =
        "#version 330 core\n"                          // GLSL version
        "in vec3 vertexColour;\n"                      // Input: interpolated colour from vertex shader
        "in vec2 texCoord;\n"                          // Input: interpolated texture coordinates
        "\n"
        "uniform sampler2D u_Texture;\n"               // Texture sampler uniform
        "out vec4 FragColor;\n"                        // Output: final pixel colour
        "\n"
        "void main()\n"
        "{\n"
        "   // Sample texture colour at interpolated UV coordinates\n"
        "   vec4 texColor = texture(u_Texture, texCoord);\n"
        "   // Blend texture colour with vertex colour (multiply blend)\n"
        "   FragColor = texColor * vec4(vertexColour, 1.0f);\n"
        "}\n";

    // Create our abstracted Shader object
    Shader shader(vertexShader, fragmentShader);

    // =================================================================
    // STEP 8: Create and Setup Textures
    // =================================================================
    // Create two different textures for demonstration
    unsigned int checkerboardTexture = CreateCheckerboardTexture();
    unsigned int gradientTexture = CreateGradientTexture();

    // =================================================================
    // STEP 9: Setup Transformation Matrices
    // =================================================================
    // Create projection matrix (perspective camera)
    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f),    // Field of view (45 degrees)
        960.0f / 540.0f,        // Aspect ratio (width/height)
        0.1f,                   // Near clipping plane
        100.0f                  // Far clipping plane
    );

    // Create view matrix (camera looking at origin from distance)
    glm::mat4 view = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 3.0f),   // Camera position
        glm::vec3(0.0f, 0.0f, 0.0f),   // Look at point (origin)
        glm::vec3(0.0f, 1.0f, 0.0f)    // Up vector
    );

    // =================================================================
    // STEP 10: Main Render Loop with Abstracted Objects
    // =================================================================
    // Keep the window open and responsive until the user closes it
    while (!glfwWindowShouldClose(window)) {

        // Clear the screen using our abstracted renderer
        renderer.Clear();

        // Get current time for animations
        float currentTime = static_cast<float>(glfwGetTime());

        // Bind our shader for all cubes
        shader.Bind();

        // Bind our mesh for all cubes
        cubeMesh->Bind();

        // Define cube positions for multiple cubes
        glm::vec3 cubePositions[] = {
            glm::vec3( 0.0f,  0.0f,  0.0f),   // Center cube
            glm::vec3( 2.0f,  0.0f, -1.0f),   // Right cube (further back)
            glm::vec3(-1.5f, -0.5f, -2.0f),   // Left-bottom cube (furthest back)
            glm::vec3( 0.5f,  1.5f, 0.5f),    // Right-top cube (closer)
            glm::vec3(-1.0f,  0.8f, 1.0f)     // Left-top cube (closest)
        };

        // Draw multiple cubes with different textures and transformations
        for (unsigned int i = 0; i < 5; i++) {
            // Create individual model matrix for each cube
            glm::mat4 cubeModel = glm::mat4(1.0f);
            cubeModel = glm::translate(cubeModel, cubePositions[i]);

            // Add rotation (different rotation for each cube)
            float angle = currentTime * (1.0f + i * 0.3f);  // Different speeds
            cubeModel = glm::rotate(cubeModel, angle, glm::vec3(1.0f + i * 0.2f, 0.3f + i * 0.1f, 0.5f));

            // Apply any additional user transformations to first cube only
            if (i == 0) {
                if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                    cubeModel = glm::translate(cubeModel, glm::vec3(0.0f, 0.001f, 0.0f));
                }
                if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                    cubeModel = glm::translate(cubeModel, glm::vec3(0.0f, -0.001f, 0.0f));
                }
                if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                    cubeModel = glm::translate(cubeModel, glm::vec3(-0.001f, 0.0f, 0.0f));
                }
                if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                    cubeModel = glm::translate(cubeModel, glm::vec3(0.001f, 0.0f, 0.0f));
                }
                if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
                    cubeModel = glm::scale(cubeModel, glm::vec3(1.001f, 1.001f, 1.001f));
                }
                if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
                    cubeModel = glm::scale(cubeModel, glm::vec3(0.999f, 0.999f, 0.999f));
                }
            }

            // Bind different textures for different cubes
            if (i % 2 == 0) {
                glBindTexture(GL_TEXTURE_2D, checkerboardTexture);  // Even cubes use checkerboard
            } else {
                glBindTexture(GL_TEXTURE_2D, gradientTexture);      // Odd cubes use gradient
            }
            shader.setUniform1i("u_Texture", 0);  // Tell shader to use texture unit 0

            // Calculate MVP matrix for this cube
            glm::mat4 cubeMvp = projection * view * cubeModel;

            // Upload the MVP matrix to the shader using our abstracted method
            shader.setUniformMat4f("u_MVP", cubeMvp);

            // Draw this cube using our abstracted renderer!
            glDrawElements(GL_TRIANGLES, cubeMesh->GetIndexCount(), GL_UNSIGNED_INT, 0);
        }

        // Swap the front and back buffers (double buffering)
        glfwSwapBuffers(window);

        // Check for window events (keyboard, mouse, window resize, etc.)
        glfwPollEvents();
    }

    // =================================================================
    // STEP 11: Cleanup Resources
    // =================================================================
    // Clean up our abstracted objects (destructors handle OpenGL cleanup)
    delete cubeMesh;

    // Clean up textures
    glDeleteTextures(1, &checkerboardTexture);
    glDeleteTextures(1, &gradientTexture);

    // =================================================================
    // STEP 12: Cleanup GLFW
    // =================================================================
    // Destroy the window and clean up GLFW resources
    glfwDestroyWindow(window);
    glfwTerminate();

    std::cout << "Abstracted 3D textured cubes rendered successfully! Window closed." << std::endl;
    return 0;
}