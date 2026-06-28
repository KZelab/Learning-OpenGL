/**
 * MazeMap.h
 *
 * The gameplay view of a generated maze: owns the tile grid and answers
 * the questions the rest of the game needs to ask about it.
 *
 * KEY CONCEPTS COVERED:
 * ---------------------
 * 1. Tile grids - representing a 3D world as a 2D array of tiles
 * 2. World <-> tile coordinate mapping
 * 3. Separation of concerns - generation (MazeGenerator), data/queries
 *    (MazeMap), and behaviour (Player, TestMaze) live in separate classes
 *
 * WORLD MAPPING:
 * Tile (col,row) is centred at world (col * tileSize, 0, row * tileSize)
 * and extends +/- tileSize/2 on X and Z. Columns run along +X, rows along +Z.
 */

#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>

namespace test
{
    /**
     * A pickup in the maze (coin or key).
     * Position is the world-space centre it bobs/spins around.
     */
    struct Collectible
    {
        glm::vec3 position{ 0.0f };
        bool collected = false;
    };

    class MazeMap
    {
    public:
        static const int LevelCount = 2;

        /**
         * Generates and parses a level.
         *
         * @param level - 0-based level index (sets maze size and coin count)
         * @param seed  - Passed to MazeGenerator; same seed = same maze
         */
        void Load(int level, unsigned int seed);

        // ---------------------------------------------------------------------
        // Tile queries
        // ---------------------------------------------------------------------

        /**
         * Is this tile solid for movement?
         * - Out-of-bounds counts as wall (the world ends at the grid edge)
         * - The exit door counts as wall until it has been opened
         */
        bool IsWall(int col, int row) const;

        int Cols() const { return m_Grid.empty() ? 0 : (int)m_Grid[0].size(); }
        int Rows() const { return (int)m_Grid.size(); }
        float TileSize() const { return m_TileSize; }
        char At(int col, int row) const { return m_Grid[row][col]; }
        const std::vector<std::string>& Grid() const { return m_Grid; }

        // World-space centre of a tile (y = 0, the floor plane)
        glm::vec3 TileCenter(int col, int row) const;

        // Which tile contains this world position?
        void WorldToTile(const glm::vec3& world, int& col, int& row) const;

        // ---------------------------------------------------------------------
        // Parsed entities
        // ---------------------------------------------------------------------
        const glm::vec3& PlayerStart() const { return m_PlayerStart; }
        float StartYaw() const { return m_StartYaw; }

        std::vector<Collectible>& Coins() { return m_Coins; }
        const std::vector<Collectible>& Coins() const { return m_Coins; }
        Collectible& Key() { return m_Key; }
        const Collectible& Key() const { return m_Key; }

        const glm::vec3& DoorPosition() const { return m_DoorPosition; }
        int DoorCol() const { return m_DoorCol; }
        int DoorRow() const { return m_DoorRow; }

        bool IsDoorOpen() const { return m_DoorOpen; }
        void OpenDoor() { m_DoorOpen = true; }

    private:
        std::vector<std::string> m_Grid;
        float m_TileSize = 2.0f;

        glm::vec3 m_PlayerStart{ 0.0f };
        float m_StartYaw = 0.0f;

        std::vector<Collectible> m_Coins;
        Collectible m_Key;

        glm::vec3 m_DoorPosition{ 0.0f };
        int m_DoorCol = 0, m_DoorRow = 0;
        bool m_DoorOpen = false;
    };
}
