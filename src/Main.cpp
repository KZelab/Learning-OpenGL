#include <GL/glew.h>                // GLEW library for managing OpenGL extensions
#include <GLFW/glfw3.h>             // GLFW for window management and input handling
#include <iostream>                 // For console output
#include <string>                   // For string handling

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
    // STEP 5: Define Triangle Vertex Data
    // =================================================================
    // A triangle requires 3 vertices. Each vertex has an X, Y, and Z coordinate.
    // In OpenGL, coordinates range from -1.0 to 1.0 for the visible area.
    // This is called "Normalized Device Coordinates" (NDC).
    float vertices[] = {
        // X      Y      Z
        -0.5f, -0.5f,  0.0f,  // Bottom left vertex
         0.5f, -0.5f,  0.0f,  // Bottom right vertex
         0.0f,  0.5f,  0.0f   // Top center vertex
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

    // Configure the vertex attribute (position)
    // Location 0, 3 components (X,Y,Z), GL_FLOAT type, not normalized,
    // stride of 3*sizeof(float), offset 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);             // Enable the vertex attribute

    // =================================================================
    // STEP 8: Define Shader Sources
    // =================================================================
    // Vertex Shader: Processes each vertex, determining final position
    std::string vertexShader =
        "#version 330 core\n"                          // GLSL version
        "layout (location = 0) in vec3 aPos;\n"        // Input: vertex position
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"  // Output position
        "}\n";

    // Fragment Shader: Determines the color of each pixel
    std::string fragmentShader =
        "#version 330 core\n"                          // GLSL version
        "out vec4 FragColor;\n"                        // Output: pixel color
        "void main()\n"
        "{\n"
        "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"  // Orange color (R,G,B,A)
        "}\n";

    // =================================================================
    // STEP 9: Compile and Create Shader Program
    // =================================================================
    unsigned int shaderProgram = CreateShader(vertexShader, fragmentShader);

    // =================================================================
    // STEP 10: Set Clear Color
    // =================================================================
    // Set the color that will be used when clearing the screen
    // RGBA values: (Red, Green, Blue, Alpha) - each from 0.0 to 1.0
    glClearColor(0.2f, 0.3f, 0.8f, 1.0f); // Nice blue background

    // =================================================================
    // STEP 11: Main Render Loop
    // =================================================================
    // Keep the window open and responsive until the user closes it
    while (!glfwWindowShouldClose(window)) {

        // Clear the screen with our chosen clear color
        glClear(GL_COLOR_BUFFER_BIT);

        // =================================================================
        // DRAW THE TRIANGLE!
        // =================================================================

        // 1. Use our shader program
        glUseProgram(shaderProgram);

        // 2. Bind our VAO (this tells OpenGL how to interpret our vertex data)
        glBindVertexArray(VAO);

        // 3. Draw the triangle!
        // glDrawArrays draws primitives using currently bound VAO
        // GL_TRIANGLES: draw triangles using every 3 vertices
        // 0: start at vertex 0
        // 3: use 3 vertices total (one triangle)
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Swap the front and back buffers (double buffering)
        // This prevents flickering by drawing to a hidden buffer first
        glfwSwapBuffers(window);

        // Check for window events (keyboard, mouse, window resize, etc.)
        glfwPollEvents();
    }

    // =================================================================
    // STEP 12: Cleanup OpenGL Resources
    // =================================================================
    // Clean up our OpenGL objects
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    // =================================================================
    // STEP 13: Cleanup GLFW
    // =================================================================
    // Destroy the window and clean up GLFW resources
    glfwDestroyWindow(window);
    glfwTerminate();

    std::cout << "Triangle rendered successfully! Window closed." << std::endl;
    return 0;
}
