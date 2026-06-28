/**
 * TestMaze.h
 *
 * A complete first-person maze game built on the test framework:
 * navigate a procedurally generated maze, collect every coin, find the
 * key, and unlock the exit door before the timer runs out - across two
 * levels of increasing size.
 *
 * KEY CONCEPTS COVERED:
 * ---------------------
 * 1. Game state machines (Playing / LevelComplete / Won / Lost)
 * 2. Rendering many objects from one mesh - a single unit cube VAO is
 *    drawn with a different model matrix (and colour/texture) per tile
 * 3. Ray-AABB intersection (slab method) for "look at + press E" interaction
 * 4. Frame-rate independent game logic (timers, pickups) via delta time
 * 5. Edge-detected key presses (act on press, not while held)
 * 6. HUD rendering with ImGui, including foreground overlays (crosshair,
 *    interaction prompts, win/lose banners)
 *
 * Collaborators:
 *   MazeGenerator - carves the maze         (procedural generation)
 *   MazeMap       - tile grid + queries     (world data)
 *   Player        - movement + collision    (move and slide)
 *   Camera        - grounded FPS camera     (shared utils class)
 */

#pragma once
#include "../Tests.h"
#include "../../Renderer.h"
#include "../../Shader.h"
#include "../../Texture.h"
#include "../../VertexArray.h"
#include "../../VertexBuffer.h"
#include "../../IndexBuffer.h"
#include "../../utils/Camera.h"
#include "MazeMap.h"
#include "Player.h"

#include <memory>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace test {
    class TestMaze : public Tests {
    public:
        TestMaze(GLFWwindow* window);

        void Update(float deltaTime) override;
        void Render() override;
        void RenderGUI() override;

    private:
        enum class GameState { Playing, LevelComplete, Won, Lost };

        void BuildCubeMesh();
        void LoadLevel(int level, unsigned int seed);
        void UpdatePlaying(float deltaTime);

        // Default seed for a level - fixed so every run of the program
        // produces the same mazes (the Regenerate button overrides this).
        static unsigned int DefaultSeed(int level) { return 1337u + (unsigned int)level * 7919u; }

        bool AllCoinsCollected() const;

        /**
         * Fires a ray from the camera along its look direction and tests it
         * against the exit door's AABB (slab method). Because the crosshair
         * sits at the exact centre of the screen, no screen-space
         * unprojection is needed - the centre of the screen IS the camera's
         * front vector.
         */
        bool RayHitsDoor(float maxDistance) const;

        // Sets per-object uniforms and issues one draw of the shared cube.
        // Pass nullptr as texture for flat-coloured objects (coins, key).
        void DrawCube(const glm::mat4& model, const glm::vec3& colour, const Texture* texture);

        GLFWwindow* m_window;
        Renderer m_Renderer;

        std::unique_ptr<Camera> m_Camera;
        std::unique_ptr<Player> m_Player;
        MazeMap m_Map;

        std::unique_ptr<Shader> m_Shader;
        std::unique_ptr<Texture> m_WallTexture;
        std::unique_ptr<Texture> m_FloorTexture;
        std::unique_ptr<Texture> m_DoorTexture;

        // One shared unit cube, reused for every object in the scene
        std::unique_ptr<VertexArray> m_CubeVAO;
        std::unique_ptr<VertexBuffer> m_CubeVBO;
        std::unique_ptr<IndexBuffer> m_CubeIBO;

        // Game state
        GameState m_State = GameState::Playing;
        int m_CurrentLevel = 0;
        unsigned int m_Seed = 0;
        float m_TimeRemaining = 0.0f;
        int m_CoinsCollected = 0;
        bool m_HasKey = false;
        bool m_DoorTargeted = false;

        // Previous-frame key states for edge detection
        bool m_EPressedLastFrame = false;
        bool m_AdvancePressedLastFrame = false;
    };
}
