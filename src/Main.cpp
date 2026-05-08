#include <GL/glew.h>                // GLEW library for managing OpenGL extensions
#include <GLFW/glfw3.h>             // GLFW for window management and input handling
#include <iostream>                 // For console output
#include <string>                   // For string handling

#include "glm/glm.hpp"              // GLM math library for vectors and matrices
#include "glm/gtc/matrix_transform.hpp" // GLM functions for creating transformation matrices
#include "glm/gtc/type_ptr.hpp"     // GLM functions for converting matrices to arrays

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
    // STEP 4.5: Enable Depth Testing
    // =================================================================
    // Enable depth testing for proper 3D rendering
    glEnable(GL_DEPTH_TEST);

    // Set depth function (GL_LESS is default - closer objects pass the test)
    glDepthFunc(GL_LESS);

    // Enable backface culling for performance (optional)
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);           // Cull back faces
    glFrontFace(GL_CCW);           // Counter-clockwise vertices = front face

    // =================================================================
    // STEP 5: Define 3D Cube Vertex Data with Texture Coordinates
    // =================================================================
    // A cube has 8 vertices and 6 faces (12 triangles total).
    // Each vertex has position (X,Y,Z), colour (R,G,B), and texture coordinates (U,V).

    // Define 8 unique vertices for a cube with UV coordinates
    float vertices[] = {
        // Position        // Colour             // UV
        // X      Y     Z     R     G     B      U     V
        // Front face vertices
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,  // 0: Front-bottom-left - Red
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,  // 1: Front-bottom-right - Green
         0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,  // 2: Front-top-right - Blue
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f,  // 3: Front-top-left - Yellow

        // Back face vertices
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,  1.0f, 0.0f,  // 4: Back-bottom-left - Magenta
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,  0.0f, 0.0f,  // 5: Back-bottom-right - Cyan
         0.5f,  0.5f, -0.5f,  1.0f, 0.5f, 0.0f,  0.0f, 1.0f,  // 6: Back-top-right - Orange
        -0.5f,  0.5f, -0.5f,  0.5f, 0.5f, 0.5f,  1.0f, 1.0f   // 7: Back-top-left - Grey
    };

    // Define indices for cube faces (12 triangles = 36 indices)
    // Each face uses 2 triangles, vertices in counter-clockwise order for front-facing
    unsigned int indices[] = {
        // Front face (Z = +0.5)
        0, 1, 2,  2, 3, 0,
        // Back face (Z = -0.5)
        4, 5, 6,  6, 7, 4,
        // Left face (X = -0.5)
        7, 3, 0,  0, 4, 7,
        // Right face (X = +0.5)
        1, 5, 6,  6, 2, 1,
        // Bottom face (Y = -0.5)
        4, 0, 1,  1, 5, 4,
        // Top face (Y = +0.5)
        3, 7, 6,  6, 2, 3
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
    // stride of 8*sizeof(float) (8 floats per vertex: XYZ + RGB + UV), offset 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);             // Enable position attribute

    // Configure vertex attribute 1: Colour
    // Location 1 (matches shader), 3 components (R,G,B), GL_FLOAT type, not normalized,
    // stride of 8*sizeof(float), offset 3*sizeof(float) (skip the XYZ position data)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);             // Enable colour attribute

    // Configure vertex attribute 2: Texture Coordinates
    // Location 2 (matches shader), 2 components (U,V), GL_FLOAT type, not normalized,
    // stride of 8*sizeof(float), offset 6*sizeof(float) (skip XYZ + RGB data)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);             // Enable texture coordinate attribute

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
    // Vertex Shader: Processes each vertex with transformation matrix and texture coordinates
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

    // Fragment Shader: Samples texture and blends with vertex colours
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

    // =================================================================
    // STEP 10: Compile and Create Shader Program
    // =================================================================
    unsigned int shaderProgram = CreateShader(vertexShader, fragmentShader);

    // =================================================================
    // STEP 11: Create and Setup Textures
    // =================================================================
    // Create two different textures for demonstration
    unsigned int checkerboardTexture = CreateCheckerboardTexture();
    unsigned int gradientTexture = CreateGradientTexture();

    // Get the uniform location for our texture sampler in the shader
    int textureLocation = glGetUniformLocation(shaderProgram, "u_Texture");
    if (textureLocation == -1) {
        std::cout << "Warning: Could not find u_Texture uniform in shader!" << std::endl;
    }

    // =================================================================
    // STEP 12: Setup Transformation Matrices
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

        // Clear the screen and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // =================================================================
        // CREATE DYNAMIC TRANSFORMATIONS
        // =================================================================

        // Get current time for animations
        float currentTime = static_cast<float>(glfwGetTime());

        // Note: Individual model transformations are now handled per-cube below

        // =================================================================
        // DRAW MULTIPLE CUBES AT DIFFERENT DEPTHS!
        // =================================================================

        // 1. Use our shader program
        glUseProgram(shaderProgram);

        // 3. Bind our VAO (this tells OpenGL how to interpret our vertex data)
        glBindVertexArray(VAO);

        // Draw multiple cubes to demonstrate depth testing
        glm::vec3 cubePositions[] = {
            glm::vec3( 0.0f,  0.0f,  0.0f),   // Center cube
            glm::vec3( 2.0f,  0.0f, -1.0f),   // Right cube (further back)
            glm::vec3(-1.5f, -0.5f, -2.0f),   // Left-bottom cube (furthest back)
            glm::vec3( 0.5f,  1.5f, 0.5f),    // Right-top cube (closer)
            glm::vec3(-1.0f,  0.8f, 1.0f)     // Left-top cube (closest)
        };

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
            glUniform1i(textureLocation, 0);  // Tell shader to use texture unit 0

            // Calculate MVP matrix for this cube
            glm::mat4 cubeMvp = projection * view * cubeModel;

            // Upload the MVP matrix to the shader for this cube
            glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(cubeMvp));

            // Draw this cube using indexed rendering!
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }

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
    glDeleteTextures(1, &checkerboardTexture);  // Clean up textures
    glDeleteTextures(1, &gradientTexture);
    glDeleteProgram(shaderProgram);

    // =================================================================
    // STEP 15: Cleanup GLFW
    // =================================================================
    // Destroy the window and clean up GLFW resources
    glfwDestroyWindow(window);
    glfwTerminate();

    std::cout << "Multiple 3D textured cubes rendered successfully! Window closed." << std::endl;
    return 0;
}
