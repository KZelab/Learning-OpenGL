#include "MazeMap.h"
#include "MazeGenerator.h"

#include <cmath>

namespace test
{
    // Height above the floor at which coins and the key float
    static const float PickupHeight = 0.9f;

    void MazeMap::Load(int level, unsigned int seed)
    {
        // Per-level difficulty: level 2 is a bigger maze with more coins
        const int sizes[LevelCount] = { 11, 15 };
        const int coins[LevelCount] = { 5, 8 };

        if (level < 0) level = 0;
        if (level >= LevelCount) level = LevelCount - 1;

        m_Grid = MazeGenerator::Generate(sizes[level], sizes[level], seed, coins[level]);

        // Reset parsed state from any previous level
        m_Coins.clear();
        m_Key = Collectible{};
        m_DoorOpen = false;

        // =====================================================================
        // PARSE THE GRID INTO GAMEPLAY ENTITIES
        // =====================================================================
        for (int row = 0; row < Rows(); row++)
        {
            for (int col = 0; col < Cols(); col++)
            {
                glm::vec3 center = TileCenter(col, row);

                switch (At(col, row))
                {
                case 'S':
                    m_PlayerStart = center;
                    break;
                case 'C':
                    m_Coins.push_back({ center + glm::vec3(0.0f, PickupHeight, 0.0f), false });
                    break;
                case 'K':
                    m_Key = { center + glm::vec3(0.0f, PickupHeight, 0.0f), false };
                    break;
                case 'D':
                    m_DoorPosition = center;
                    m_DoorCol = col;
                    m_DoorRow = row;
                    break;
                }
            }
        }

        // Face the player down the open corridor at the start.
        // Camera yaw convention: 0 degrees looks along +X, 90 along +Z.
        int startCol, startRow;
        WorldToTile(m_PlayerStart, startCol, startRow);
        m_StartYaw = !IsWall(startCol + 1, startRow) ? 0.0f : 90.0f;
    }

    bool MazeMap::IsWall(int col, int row) const
    {
        // Everything beyond the grid is solid - you can never leave the world
        if (col < 0 || col >= Cols() || row < 0 || row >= Rows())
            return true;

        char tile = At(col, row);

        if (tile == '#')
            return true;

        // The exit door blocks movement until it has been unlocked
        if (tile == 'D')
            return !m_DoorOpen;

        return false;
    }

    glm::vec3 MazeMap::TileCenter(int col, int row) const
    {
        return glm::vec3(col * m_TileSize, 0.0f, row * m_TileSize);
    }

    void MazeMap::WorldToTile(const glm::vec3& world, int& col, int& row) const
    {
        // Tile centres sit at integer multiples of tileSize, so a tile spans
        // +/- half a tile around its centre - hence the rounding.
        col = (int)std::floor(world.x / m_TileSize + 0.5f);
        row = (int)std::floor(world.z / m_TileSize + 0.5f);
    }
}
