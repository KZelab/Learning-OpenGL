#include "TestMaze.h"

#include "../../VertexBufferLayout.h"
#include "../../vendor/imgui/imgui.h"

#include <algorithm>
#include <cmath>
#include <random>
#include <glm/gtc/matrix_transform.hpp>

namespace test {

    // Tunable game constants
    static const float WallHeight = 2.5f;       // Walls are taller than the player
    static const float InteractDistance = 2.0f; // Max reach of the E-key ray
    static const float PickupRadius = 0.6f;     // Walk-over distance for coins/key
    static const float LevelTime[MazeMap::LevelCount] = { 90.0f, 120.0f };

    TestMaze::TestMaze(GLFWwindow* window)
        : m_window(window)
    {
        // Grounded FPS camera - the Player repositions it on level load
        m_Camera = std::make_unique<Camera>(
            window,
            glm::vec3(0.0f, 1.6f, 0.0f),  // position (placeholder until LoadLevel)
            glm::vec3(0.0f, 1.0f, 0.0f),  // up vector
            0.0f,                         // yaw
            0.0f,                         // pitch
            60.0f                         // FOV
        );

        m_Player = std::make_unique<Player>(m_Camera.get(), &m_Map);

        m_Shader = std::make_unique<Shader>("res/Shaders/Maze.shader");

        m_WallTexture  = std::make_unique<Texture>("res/Textures/maze/wall.png");
        m_FloorTexture = std::make_unique<Texture>("res/Textures/maze/floor.png");
        m_DoorTexture  = std::make_unique<Texture>("res/Textures/maze/door.png");

        BuildCubeMesh();

        glEnable(GL_DEPTH_TEST);

        LoadLevel(0, DefaultSeed(0));
    }

    // =========================================================================
    // GEOMETRY
    // =========================================================================
    /**
     * Builds a unit cube (1x1x1, centred on the origin) directly with the
     * buffer abstraction classes.
     *
     * 24 VERTICES, NOT 8:
     * Each face needs its own flat normal and its own 0..1 texture
     * coordinates, so corner positions cannot be shared between faces -
     * every face gets 4 unique vertices (6 faces x 4 = 24), indexed as
     * 2 triangles each (6 x 6 = 36 indices).
     *
     * Vertex layout (matches Maze.shader locations):
     *   location 0: position (3 floats)
     *   location 1: normal   (3 floats)
     *   location 2: texCoord (2 floats)
     */
    void TestMaze::BuildCubeMesh()
    {
        const float vertices[] = {
            // position             normal              texCoord
            // Front face (+Z)
            -0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,   0.0f, 0.0f,
             0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,   1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,   1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,   0.0f, 1.0f,
            // Back face (-Z)
             0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,   0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,   1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
             0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,   0.0f, 1.0f,
            // Left face (-X)
            -0.5f, -0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
            // Right face (+X)
             0.5f, -0.5f,  0.5f,    1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
             0.5f, -0.5f, -0.5f,    1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,    1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
             0.5f,  0.5f,  0.5f,    1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
            // Top face (+Y)
            -0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,   0.0f, 0.0f,
             0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,   1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,   0.0f, 1.0f,
            // Bottom face (-Y)
            -0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,   0.0f, 0.0f,
             0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
             0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,   1.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,   0.0f, 1.0f,
        };

        // Two CCW triangles per face: 0-1-2 and 2-3-0 (offset by face base)
        unsigned int indices[36];
        for (unsigned int face = 0; face < 6; face++)
        {
            unsigned int base = face * 4;
            unsigned int* tri = &indices[face * 6];
            tri[0] = base;     tri[1] = base + 1; tri[2] = base + 2;
            tri[3] = base + 2; tri[4] = base + 3; tri[5] = base;
        }

        m_CubeVAO = std::make_unique<VertexArray>();
        m_CubeVBO = std::make_unique<VertexBuffer>(vertices, sizeof(vertices));

        VertexBufferLayout layout;
        layout.Push<float>(3); // position
        layout.Push<float>(3); // normal
        layout.Push<float>(2); // texCoord
        m_CubeVAO->AddBuffer(*m_CubeVBO, layout);

        m_CubeIBO = std::make_unique<IndexBuffer>(indices, 36);
    }

    // =========================================================================
    // GAME STATE
    // =========================================================================
    void TestMaze::LoadLevel(int level, unsigned int seed)
    {
        m_CurrentLevel = level;
        m_Seed = seed;

        m_Map.Load(level, seed);

        m_TimeRemaining = LevelTime[level];
        m_CoinsCollected = 0;
        m_HasKey = false;
        m_DoorTargeted = false;
        m_State = GameState::Playing;

        m_Player->Respawn(m_Map.PlayerStart(), m_Map.StartYaw());
    }

    bool TestMaze::AllCoinsCollected() const
    {
        return m_CoinsCollected == (int)m_Map.Coins().size();
    }

    void TestMaze::Update(float deltaTime)
    {
        // Always run camera input: it owns the ESC pause toggle, and its
        // movement/look handlers are no-ops while detached, so this is safe
        // in every state. The Player's collision keeps us out of walls.
        m_Player->Update(deltaTime);
        m_Camera->Update(deltaTime);

        // ESC-detached means paused: timer frozen, no pickups, no interaction
        if (m_Camera->isDetached())
            return;

        // Edge-detected "advance" key (Enter) for the end-of-state screens
        bool advance = glfwGetKey(m_window, GLFW_KEY_ENTER) == GLFW_PRESS;
        bool advancePressed = advance && !m_AdvancePressedLastFrame;
        m_AdvancePressedLastFrame = advance;

        switch (m_State)
        {
        case GameState::Playing:
            UpdatePlaying(deltaTime);
            break;

        case GameState::LevelComplete:
            if (advancePressed)
                LoadLevel(m_CurrentLevel + 1, DefaultSeed(m_CurrentLevel + 1));
            break;

        case GameState::Lost:
            // Retry the SAME maze: reuse the seed the level was built with
            if (advancePressed)
                LoadLevel(m_CurrentLevel, m_Seed);
            break;

        case GameState::Won:
            if (advancePressed)
                LoadLevel(0, DefaultSeed(0));
            break;
        }
    }

    void TestMaze::UpdatePlaying(float deltaTime)
    {
        // ---------------------------------------------------------------------
        // Timer
        // ---------------------------------------------------------------------
        m_TimeRemaining -= deltaTime;
        if (m_TimeRemaining <= 0.0f)
        {
            m_TimeRemaining = 0.0f;
            m_State = GameState::Lost;
            return;
        }

        glm::vec3 playerPos = m_Camera->getPosition();

        // ---------------------------------------------------------------------
        // Pickups - walk-over collection using a horizontal distance check
        // (the pickups bob up and down, so Y is deliberately ignored)
        // ---------------------------------------------------------------------
        auto distanceXZ = [](const glm::vec3& a, const glm::vec3& b)
        {
            float dx = a.x - b.x;
            float dz = a.z - b.z;
            return std::sqrt(dx * dx + dz * dz);
        };

        for (Collectible& coin : m_Map.Coins())
        {
            if (!coin.collected && distanceXZ(playerPos, coin.position) < PickupRadius)
            {
                coin.collected = true;
                m_CoinsCollected++;
            }
        }

        Collectible& key = m_Map.Key();
        if (!key.collected && distanceXZ(playerPos, key.position) < PickupRadius)
        {
            key.collected = true;
            m_HasKey = true;
        }

        // ---------------------------------------------------------------------
        // Exit door - aim at it (crosshair ray) and press E to unlock.
        // Unlocking needs the key AND every coin, so both mechanics matter.
        // ---------------------------------------------------------------------
        m_DoorTargeted = !m_Map.IsDoorOpen() && RayHitsDoor(InteractDistance);

        bool ePressed = glfwGetKey(m_window, GLFW_KEY_E) == GLFW_PRESS;
        if (ePressed && !m_EPressedLastFrame && m_DoorTargeted && m_HasKey && AllCoinsCollected())
        {
            m_Map.OpenDoor();
            m_DoorTargeted = false;
        }
        m_EPressedLastFrame = ePressed;

        // ---------------------------------------------------------------------
        // Walking through the open doorway finishes the level
        // ---------------------------------------------------------------------
        if (m_Map.IsDoorOpen())
        {
            int col, row;
            m_Map.WorldToTile(playerPos, col, row);
            if (col == m_Map.DoorCol() && row == m_Map.DoorRow())
            {
                m_State = (m_CurrentLevel + 1 < MazeMap::LevelCount)
                    ? GameState::LevelComplete
                    : GameState::Won;
            }
        }
    }

    // =========================================================================
    // RAY-AABB INTERSECTION (SLAB METHOD)
    // =========================================================================
    /**
     * Treats the AABB as the intersection of three "slabs" (one per axis).
     * For each axis we find the parametric interval [t1, t2] in which the ray
     * is inside that axis' slab; the ray hits the box only if all three
     * intervals overlap. Clamping tmin to 0 ignores hits behind the camera,
     * and starting tmax at maxDistance enforces the interaction reach.
     */
    bool TestMaze::RayHitsDoor(float maxDistance) const
    {
        glm::vec3 origin = m_Camera->getPosition();
        glm::vec3 direction = m_Camera->getFront();

        float half = m_Map.TileSize() * 0.5f;
        glm::vec3 doorCenter = m_Map.DoorPosition() + glm::vec3(0.0f, WallHeight * 0.5f, 0.0f);
        glm::vec3 boxMin = doorCenter - glm::vec3(half, WallHeight * 0.5f, half);
        glm::vec3 boxMax = doorCenter + glm::vec3(half, WallHeight * 0.5f, half);

        float tMin = 0.0f;
        float tMax = maxDistance;

        for (int axis = 0; axis < 3; axis++)
        {
            if (std::abs(direction[axis]) < 1e-8f)
            {
                // Ray is parallel to this slab: it must already be inside it
                if (origin[axis] < boxMin[axis] || origin[axis] > boxMax[axis])
                    return false;
            }
            else
            {
                float invD = 1.0f / direction[axis];
                float t1 = (boxMin[axis] - origin[axis]) * invD;
                float t2 = (boxMax[axis] - origin[axis]) * invD;
                if (t1 > t2) std::swap(t1, t2);

                tMin = std::max(tMin, t1);
                tMax = std::min(tMax, t2);

                if (tMin > tMax)
                    return false; // The slab intervals no longer overlap
            }
        }
        return true;
    }

    // =========================================================================
    // RENDERING
    // =========================================================================
    void TestMaze::DrawCube(const glm::mat4& model, const glm::vec3& colour, const Texture* texture)
    {
        m_Shader->Bind();
        m_Shader->setUniformMat4f("u_Model", model);
        m_Shader->setUniform3f("u_ObjectColour", colour.r, colour.g, colour.b);

        if (texture)
        {
            texture->Bind(0);
            m_Shader->setUniform1i("u_Texture", 0);
            m_Shader->setUniform1i("u_UseTexture", 1);
        }
        else
        {
            m_Shader->setUniform1i("u_UseTexture", 0);
        }

        m_Renderer.Draw(*m_CubeVAO, *m_CubeIBO, *m_Shader);
    }

    void TestMaze::Render()
    {
        m_Renderer.ClearColour_Black();
        m_Renderer.Clear();

        // Use the real framebuffer size so the projection is never stretched
        int fbWidth, fbHeight;
        glfwGetFramebufferSize(m_window, &fbWidth, &fbHeight);
        float aspect = fbHeight > 0 ? (float)fbWidth / (float)fbHeight : 16.0f / 9.0f;

        glm::mat4 projection = glm::perspective(glm::radians(m_Camera->getFOV()), aspect, 0.1f, 100.0f);
        glm::mat4 view = m_Camera->getViewMatrix();

        // Frame-constant uniforms, set once before the per-object draws
        glm::vec3 mazeCenter = m_Map.TileCenter(m_Map.Cols() / 2, m_Map.Rows() / 2);
        glm::vec3 cameraPos = m_Camera->getPosition();

        m_Shader->Bind();
        m_Shader->setUniformMat4f("u_View", view);
        m_Shader->setUniformMat4f("u_Projection", projection);
        m_Shader->setUniform3f("u_Light.Position", mazeCenter.x, 8.0f, mazeCenter.z);
        m_Shader->setUniform3f("u_Light.Colour", 1.0f, 1.0f, 1.0f);
        m_Shader->setUniform3f("u_CameraPosition", cameraPos.x, cameraPos.y, cameraPos.z);
        m_Shader->setUniform1f("u_AmbientIntensity", 0.35f);
        m_Shader->setUniform1f("u_DiffuseIntensity", 0.8f);
        m_Shader->setUniform1f("u_SpecularIntensity", 0.2f);
        m_Shader->setUniform1f("u_Shininess", 32.0f);

        float time = (float)glfwGetTime();
        float tile = m_Map.TileSize();

        // ---------------------------------------------------------------------
        // Tiles: walls get a full-height textured block, everything else
        // gets a thin floor slab (its top face sits exactly at y = 0)
        // ---------------------------------------------------------------------
        for (int row = 0; row < m_Map.Rows(); row++)
        {
            for (int col = 0; col < m_Map.Cols(); col++)
            {
                glm::vec3 center = m_Map.TileCenter(col, row);
                char tileChar = m_Map.At(col, row);

                if (tileChar == '#')
                {
                    glm::mat4 model = glm::translate(glm::mat4(1.0f), center + glm::vec3(0.0f, WallHeight * 0.5f, 0.0f));
                    model = glm::scale(model, glm::vec3(tile, WallHeight, tile));
                    DrawCube(model, glm::vec3(1.0f), m_WallTexture.get());
                }
                else
                {
                    glm::mat4 model = glm::translate(glm::mat4(1.0f), center + glm::vec3(0.0f, -0.05f, 0.0f));
                    model = glm::scale(model, glm::vec3(tile, 0.1f, tile));
                    DrawCube(model, glm::vec3(1.0f), m_FloorTexture.get());

                    if (tileChar == 'D' && !m_Map.IsDoorOpen())
                    {
                        // The locked door fills its tile like a wall; once
                        // opened it simply isn't drawn - an open doorway
                        glm::mat4 doorModel = glm::translate(glm::mat4(1.0f), center + glm::vec3(0.0f, WallHeight * 0.5f, 0.0f));
                        doorModel = glm::scale(doorModel, glm::vec3(tile, WallHeight, tile));
                        DrawCube(doorModel, glm::vec3(0.85f, 0.6f, 0.4f), m_DoorTexture.get());
                    }
                }
            }
        }

        // ---------------------------------------------------------------------
        // Pickups - bobbing, spinning, flat-coloured
        // ---------------------------------------------------------------------
        const std::vector<Collectible>& coins = m_Map.Coins();
        for (size_t i = 0; i < coins.size(); i++)
        {
            if (coins[i].collected) continue;

            // Per-coin phase offset so they don't all bob in unison
            float phase = (float)i * 0.7f;
            glm::vec3 bob(0.0f, 0.1f * std::sin(time * 3.0f + phase), 0.0f);

            glm::mat4 model = glm::translate(glm::mat4(1.0f), coins[i].position + bob);
            model = glm::rotate(model, time * 2.0f + phase, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::scale(model, glm::vec3(0.3f));
            DrawCube(model, glm::vec3(1.0f, 0.85f, 0.1f), nullptr); // gold
        }

        const Collectible& key = m_Map.Key();
        if (!key.collected)
        {
            glm::vec3 bob(0.0f, 0.1f * std::sin(time * 3.0f), 0.0f);
            glm::mat4 model = glm::translate(glm::mat4(1.0f), key.position + bob);
            model = glm::rotate(model, time * 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::scale(model, glm::vec3(0.15f, 0.15f, 0.5f)); // elongated = key-ish
            DrawCube(model, glm::vec3(0.3f, 0.9f, 1.0f), nullptr);    // cyan
        }
    }

    // =========================================================================
    // HUD
    // =========================================================================
    void TestMaze::RenderGUI()
    {
        // --- Panel widgets (inside Main.cpp's "Test control panel" window) ---
        ImGui::Text("Level: %d / %d", m_CurrentLevel + 1, MazeMap::LevelCount);
        ImGui::Text("Time:  %.0f s", m_TimeRemaining);
        ImGui::Text("Coins: %d / %d", m_CoinsCollected, (int)m_Map.Coins().size());
        ImGui::Text("Key:   %s", m_HasKey ? "collected" : "not found");

        ImGui::Separator();
        ImGui::Text("Seed: %u", m_Seed);
        if (ImGui::Button("Regenerate maze"))
        {
            // std::random_device gives a non-deterministic seed, so this
            // builds a brand new maze for the current level
            LoadLevel(m_CurrentLevel, std::random_device{}());
        }

        ImGui::Separator();
        ImGui::TextWrapped("WASD: move | Mouse: look | E: open door | Enter: continue | ESC: pause + cursor");
        m_Camera->cameraGUI();

        // --- Foreground overlays (drawn over the 3D view, outside windows) ---
        ImDrawList* drawList = ImGui::GetForegroundDrawList();
        ImGuiIO& io = ImGui::GetIO();
        ImVec2 center(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);

        // Centred-text helper: ImGui only measures text at the default size,
        // so big text is measured via the font directly
        auto drawCenteredText = [&](float fontSize, float yOffset, ImU32 colour, const char* text)
        {
            ImFont* font = ImGui::GetFont();
            ImVec2 size = font->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, text);
            drawList->AddText(font, fontSize,
                ImVec2(center.x - size.x * 0.5f, center.y + yOffset - size.y * 0.5f),
                colour, text);
        };

        if (m_State == GameState::Playing && !m_Camera->isDetached())
        {
            // Crosshair: two short lines crossing at the exact screen centre
            const float arm = 10.0f;
            ImU32 white = IM_COL32(255, 255, 255, 200);
            drawList->AddLine(ImVec2(center.x - arm, center.y), ImVec2(center.x + arm, center.y), white, 2.0f);
            drawList->AddLine(ImVec2(center.x, center.y - arm), ImVec2(center.x, center.y + arm), white, 2.0f);

            if (m_DoorTargeted)
            {
                const char* prompt;
                if (!m_HasKey)
                    prompt = "The door is locked - find the key";
                else if (!AllCoinsCollected())
                    prompt = "The door won't budge - collect every coin first";
                else
                    prompt = "Press E to unlock the door";

                drawCenteredText(24.0f, 50.0f, IM_COL32(255, 230, 120, 255), prompt);
            }
        }

        switch (m_State)
        {
        case GameState::LevelComplete:
            drawCenteredText(48.0f, -30.0f, IM_COL32(120, 255, 120, 255), "LEVEL COMPLETE!");
            drawCenteredText(24.0f, 30.0f, IM_COL32(255, 255, 255, 255), "Press ENTER for the next maze");
            break;
        case GameState::Won:
            drawCenteredText(48.0f, -30.0f, IM_COL32(120, 255, 120, 255), "YOU ESCAPED!");
            drawCenteredText(24.0f, 30.0f, IM_COL32(255, 255, 255, 255), "Press ENTER to play again");
            break;
        case GameState::Lost:
            drawCenteredText(48.0f, -30.0f, IM_COL32(255, 100, 100, 255), "OUT OF TIME...");
            drawCenteredText(24.0f, 30.0f, IM_COL32(255, 255, 255, 255), "Press ENTER to retry this maze");
            break;
        case GameState::Playing:
            break;
        }
    }
}
