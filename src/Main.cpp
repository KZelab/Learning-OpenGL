// Player Physics Example with GLFW
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Player/Player.h"
#include "Player/InputHandler.h"
#include "Shader.h"
#include "Voxel/Chunk.h"
#include "Voxel/ChunkMeshBuilder.h"
#include "TextureAtlas.h"
#include "Renderer.h"
#include "Model.h"
#include "OBJLoader.h"

#include <iostream>

void glfw_error_callback(int error, const char* description)
{
    std::cerr << "GLFW Error (" << error << "): " << description << std::endl;
}

int main()
{
    glfwSetErrorCallback(glfw_error_callback);
    // GLFW initialisation
    if (!glfwInit())
    {
        std::cout << "Failed to initialise GLFW" << std::endl;
        return -1;
    }

    // Window hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);




    // Create window
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Voxel Engine", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Set GLFW callbacks
    // GLEW initialisation
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Enable backface culling for performance
   // glEnable(GL_CULL_FACE);
   // glCullFace(GL_BACK);

    // Load texture atlas
    TextureAtlas atlas;
    if (!atlas.LoadAtlas("res/Textures/kenney_voxel_atlas.png", 16))
    {
        std::cerr << "ERROR: Failed to load texture atlas!" << std::endl;
        glfwTerminate();
        return -1;
    }

    atlas.RegisterTexture("air", 0);  // Air
    atlas.RegisterTexture("greystone", 1);  // Greystone
    atlas.RegisterTexture("dirt", 2);  // Dirt
    atlas.RegisterTexture("grass_top", 3);  // GrassTop
    atlas.RegisterTexture("dirt_grass", 4);  // DirtGrass
    atlas.RegisterTexture("sand", 5);  // Sand
    atlas.RegisterTexture("water", 6);  // Water
    atlas.RegisterTexture("trunk_top", 7);  // TrunkTop
    atlas.RegisterTexture("trunk_side", 8);  // TrunkSide
    atlas.RegisterTexture("leaves", 9);  // Leaves
    atlas.RegisterTexture("glass", 10);  // Glass
    atlas.RegisterTexture("stone", 11);  // Stone
    atlas.RegisterTexture("wood", 12);  // Wood
    atlas.RegisterTexture("rock", 13);  // Rock
    atlas.RegisterTexture("gravel_stone", 14);  // GravelStone
    atlas.RegisterTexture("brick_red", 15);  // BrickRed
    atlas.RegisterTexture("snow", 16);  // Snow
    atlas.RegisterTexture("ice", 17);  // Ice

    // Create a test chunk
    Chunk chunk(0, 0);
    chunk.FillLayers(0, 60, VoxelType::STONE);
    chunk.FillLayers(61, 69, VoxelType::DIRT);
    chunk.FillLayer(70, VoxelType::GRASS);

    chunk.UpdateEmptyStatus();
    // Build mesh
    ChunkMeshBuilder builder(&chunk);
    builder.BuildMesh();
    builder.ApplyMeshToChunk();

    std::cout << "Mesh built: " << builder.GetVertexCount() << " vertices, "
              << builder.GetIndexCount() << " indices, "
              << builder.GetTriangleCount() << " triangles" << std::endl;

    // Create player at spawn position (above grass layer at Y=70, spawn at Y=72)
    Player player(glm::vec3(8.0f, 72.0f, 8.0f));

    // Create input handler and set up callbacks
    InputHandler inputHandler(&player, &chunk);
    inputHandler.SetupCallbacks(window);

    // Load player model
    player.LoadModel("res/Model/Creative_Character_free.obj");

    // Load shaders
    Renderer renderer;
    Shader voxelShader("res/shaders/voxel.shader");
    Shader modelShader("res/shaders/model.shader");

    std::cout << "\nControls:" << std::endl;
    std::cout << "WASD - Move horizontally" << std::endl;
    std::cout << "Space - Jump" << std::endl;
    std::cout << "Ctrl - Sprint" << std::endl;
    std::cout << "V - Toggle camera (First/Third Person)" << std::endl;
    std::cout << "Mouse - Look around" << std::endl;
    std::cout << "Scroll - Zoom" << std::endl;
    std::cout << "ESC - Exit" << std::endl;

    // Timing
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        // Calculate delta time
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Process input
        inputHandler.ProcessInput(window, deltaTime);

        // Update player physics and collision
        player.Update(deltaTime, chunk);

        // Clear buffers
        renderer.Clear();


        // Get window size for aspect ratio
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
   

        float aspectRatio = static_cast<float>(width) / static_cast<float>(height);


        glViewport(0, 0, width, height);

        // Calculate view-projection matrix from player's camera
        glm::mat4 projection = player.GetCamera().GetProjectionMatrix(aspectRatio);
        glm::mat4 view = player.GetCamera().GetViewMatrix();
        glm::mat4 viewProjection = projection * view;
        glm::vec3 cameraPos = player.GetCamera().GetPosition();

        // Render chunk with voxel shader
        voxelShader.Bind();
        atlas.Bind(0);

        voxelShader.setUniform1i("u_Texture", 0);
        voxelShader.setUniformMat4f("u_ViewProjection", viewProjection);
        voxelShader.setUniformMat4f("u_Model", glm::mat4(1.0f));
        voxelShader.setUniform3f("u_LightDir", 0.5f, 1.0f, 0.3f); // Sun direction

        // Debug: verify texture binding once
        static bool debugPrinted = false;
        if (!debugPrinted)
        {
            GLint currentTexture;
            glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTexture);
            std::cout << "DEBUG: Current bound texture ID: " << currentTexture << std::endl;
            std::cout << "DEBUG: Atlas texture ID: " << (atlas.GetTexture() ? atlas.GetTexture()->getRendererID() : 0) << std::endl;

            player.GetCamera().debugPrint();

            debugPrinted = true;
        }


        if (chunk.HasMesh())
        {
            renderer.Draw(*chunk.GetVAO(), *chunk.GetIBO());
        }

        // Render player model (only in third-person mode)
        if (player.GetCameraMode() == Player::CameraMode::ThirdPerson &&
            player.GetModel() && player.GetModel()->IsUploadedToGPU())
        {
            modelShader.Bind();

            // Set uniforms for model shader
            glm::mat4 modelMatrix = player.GetModel()->GetModelMatrix();

            modelShader.setUniformMat4f("u_Model", modelMatrix);
            modelShader.setUniformMat4f("u_ViewProjection", viewProjection);
            modelShader.setUniform3f("u_LightDir", 0.5f, 1.0f, 0.3f);  // Sun direction
            modelShader.setUniform3f("u_ViewPos", cameraPos.x, cameraPos.y, cameraPos.z);

            // Bind texture if available, otherwise use solid color
            if (player.GetModel()->GetTexture())
            {
                player.GetModel()->GetTexture()->Bind(0);
                modelShader.setUniform1i("u_Texture", 0);
                modelShader.setUniform1i("u_UseTexture", 1);
            }
            else
            {
                modelShader.setUniform1i("u_UseTexture", 0);
                modelShader.setUniform3f("u_Color", 0.8f, 0.7f, 0.6f);  // Warm character color fallback
            }

            renderer.Draw(*player.GetModel()->GetVAO(), *player.GetModel()->GetIBO());
        }

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // Cleanup
    glfwTerminate();
    return 0;
}

