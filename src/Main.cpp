#include <GL/glew.h>                // GLEW library for managing OpenGL extensions
#include <GLFW/glfw3.h>             // GLFW for window management and input handling
#include <iostream>                 // For console output
#include <string>                   // For string handling

#include "glm/glm.hpp"              // GLM math library for vectors and matrices
#include "glm/gtc/matrix_transform.hpp" // GLM functions for creating transformation matrices
#include "glm/gtc/type_ptr.hpp"     // GLM functions for converting matrices to arrays

// =================================================================
// SHADER COMPILATION HELPER FUNCTIONS
// =================================================================

// Function to compile a shader from source code
unsigned int CompileShader(unsigned int type, const std::string& source) {
    // Create a shader object of the specified type
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();

    // Attach the source code to the shader and compile it
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    // Check for compilation errors
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);

        std::cout << "Failed to compile "
                  << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
                  << " shader!" << std::endl;
        std::cout << message << std::endl;

        glDeleteShader(id);
        return 0;
    }

    return id;
}

// Function to create a complete shader program from vertex and fragment shader sources
unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    // Create a shader program object
    unsigned int program = glCreateProgram();

    // Compile both shaders
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    // Attach shaders to the program and link them
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    // Clean up individual shader objects (they're now part of the program)
    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
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
    // STEP 5: Define Square Vertex Data with Indexed Rendering
    // =================================================================
    // Instead of duplicating vertices for a square, we define 4 unique vertices
    // and use indices to specify which vertices form triangles.
    // This is more efficient for complex geometry.

    // Define 4 unique vertices for a square
    float vertices[] = {
        // Position        // Colour
        // X      Y     Z     R     G     B
        -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // 0: Bottom-left - Red
         0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // 1: Bottom-right - Green
         0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  // 2: Top-right - Blue
        -0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 0.0f   // 3: Top-left - Yellow
    };

    // Define indices for two triangles that make up the square
    // Each triangle uses 3 indices pointing to vertices in the array above
    unsigned int indices[] = {
        0, 1, 2,   // First triangle: bottom-left, bottom-right, top-right
        2, 3, 0    // Second triangle: top-right, top-left, bottom-left
    };

    // =================================================================
    // STEP 6: Create Vertex Buffer Object (VBO)
    // =================================================================
    // A VBO stores vertex data in GPU memory for fast access during rendering.
    unsigned int VBO;
    glGenBuffers(1, &VBO);                    // Generate a buffer ID
    glBindBuffer(GL_ARRAY_BUFFER, VBO);       // Bind as the active vertex buffer

    // Upload our vertex data to the GPU
    // GL_STATIC_DRAW means we set the data once and draw it many times
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // =================================================================
    // STEP 7: Create Vertex Array Object (VAO)
    // =================================================================
    // A VAO stores the configuration of vertex attribute pointers.
    // It remembers how to interpret the data in our VBO.
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);               // Generate a VAO ID
    glBindVertexArray(VAO);                   // Bind as the active VAO

    // Configure vertex attribute 0: Position
    // Location 0 (matches shader), 3 components (X,Y,Z), GL_FLOAT type, not normalized,
    // stride of 6*sizeof(float) (6 floats per vertex: XYZ + RGB), offset 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);             // Enable position attribute

    // Configure vertex attribute 1: Colour
    // Location 1 (matches shader), 3 components (R,G,B), GL_FLOAT type, not normalized,
    // stride of 6*sizeof(float), offset 3*sizeof(float) (skip the XYZ position data)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);             // Enable colour attribute

    // =================================================================
    // STEP 8: Create Element Buffer Object (EBO)
    // =================================================================
    // An EBO stores indices that tell OpenGL which vertices to use for each triangle.
    // This allows us to reuse vertices and avoid duplication.
    unsigned int EBO;
    glGenBuffers(1, &EBO);                    // Generate an EBO ID
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // Bind as element buffer

    // Upload index data to GPU
    // GL_STATIC_DRAW because we set indices once and use them many times
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // =================================================================
    // STEP 9: Define Shader Sources
    // =================================================================
    // Vertex Shader: Processes each vertex with transformation matrix
    std::string vertexShader =
        "#version 330 core\n"                          // GLSL version
        "layout (location = 0) in vec3 aPos;\n"        // Input: vertex position
        "layout (location = 1) in vec3 aColour;\n"     // Input: vertex colour
        "\n"
        "uniform mat4 u_MVP;\n"                        // Model-View-Projection matrix uniform
        "\n"
        "out vec3 vertexColour;\n"                     // Output: colour to fragment shader
        "\n"
        "void main()\n"
        "{\n"
        "   // Transform vertex position using MVP matrix\n"
        "   gl_Position = u_MVP * vec4(aPos, 1.0);\n"  // Apply transformations
        "   vertexColour = aColour;\n"                 // Pass colour to fragment shader
        "}\n";

    // Fragment Shader: Uses interpolated colours from vertices
    std::string fragmentShader =
        "#version 330 core\n"                          // GLSL version
        "in vec3 vertexColour;\n"                      // Input: interpolated colour from vertex shader
        "out vec4 FragColor;\n"                        // Output: final pixel colour
        "\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(vertexColour, 1.0f);\n"   // Use interpolated colour with full alpha
        "}\n";

    // =================================================================
    // STEP 10: Compile and Create Shader Program
    // =================================================================
    unsigned int shaderProgram = CreateShader(vertexShader, fragmentShader);

    // =================================================================
    // STEP 11: Setup Transformation Matrices
    // =================================================================
    // Get the uniform location for our MVP matrix in the shader
    int mvpLocation = glGetUniformLocation(shaderProgram, "u_MVP");
    if (mvpLocation == -1) {
        std::cout << "Warning: Could not find u_MVP uniform in shader!" << std::endl;
    }

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
    // STEP 12: Set Clear Color
    // =================================================================
    // Set the color that will be used when clearing the screen
    // RGBA values: (Red, Green, Blue, Alpha) - each from 0.0 to 1.0
    glClearColor(0.2f, 0.3f, 0.8f, 1.0f); // Nice blue background

    // =================================================================
    // STEP 13: Main Render Loop with Transformations
    // =================================================================
    // Keep the window open and responsive until the user closes it
    while (!glfwWindowShouldClose(window)) {

        // Clear the screen with our chosen clear color
        glClear(GL_COLOR_BUFFER_BIT);

        // =================================================================
        // CREATE DYNAMIC TRANSFORMATIONS
        // =================================================================

        // Get current time for animations
        float currentTime = static_cast<float>(glfwGetTime());

        // Create model matrix with time-based rotation
        glm::mat4 model = glm::mat4(1.0f);                      // Start with identity matrix
        model = glm::rotate(model, currentTime, glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate around Z-axis

        // Handle keyboard input for additional transformations
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            model = glm::translate(model, glm::vec3(0.0f, 0.001f, 0.0f));  // Move up
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            model = glm::translate(model, glm::vec3(0.0f, -0.001f, 0.0f)); // Move down
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            model = glm::translate(model, glm::vec3(-0.001f, 0.0f, 0.0f)); // Move left
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            model = glm::translate(model, glm::vec3(0.001f, 0.0f, 0.0f));  // Move right
        }
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            model = glm::scale(model, glm::vec3(1.001f, 1.001f, 1.0f));    // Scale up
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
            model = glm::scale(model, glm::vec3(0.999f, 0.999f, 1.0f));    // Scale down
        }

        // Combine all transformation matrices: Projection * View * Model
        glm::mat4 mvp = projection * view * model;

        // =================================================================
        // DRAW THE TRANSFORMED SQUARE!
        // =================================================================

        // 1. Use our shader program
        glUseProgram(shaderProgram);

        // 2. Upload the MVP matrix to the shader
        glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));

        // 3. Bind our VAO (this tells OpenGL how to interpret our vertex data)
        glBindVertexArray(VAO);

        // 4. Draw using indexed rendering!
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Swap the front and back buffers (double buffering)
        glfwSwapBuffers(window);

        // Check for window events (keyboard, mouse, window resize, etc.)
        glfwPollEvents();
    }

    // =================================================================
    // STEP 14: Cleanup OpenGL Resources
    // =================================================================
    // Clean up our OpenGL objects
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);              // Clean up Element Buffer Object
    glDeleteProgram(shaderProgram);

    // =================================================================
    // STEP 15: Cleanup GLFW
    // =================================================================
    // Destroy the window and clean up GLFW resources
    glfwDestroyWindow(window);
    glfwTerminate();

    std::cout << "Animated square with transformations rendered successfully! Window closed." << std::endl;
    return 0;
}
