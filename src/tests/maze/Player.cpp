#include "Player.h"

#include <cmath>

namespace test
{
    Player::Player(Camera* camera, MazeMap* map) :
        m_Camera(camera),
        m_Map(map)
    {
        // Grounded mode keeps the camera at eye height and makes WASD move
        // on the XZ plane regardless of where the player is looking.
        m_Camera->setGrounded(true, m_EyeHeight);
    }

    void Player::Update(float deltaTime)
    {
        /**
         * MOVE AND SLIDE:
         * The camera applies this frame's WASD movement first; we then take
         * that desired position and re-apply it from the previous position
         * one axis at a time, resolving collisions after each axis:
         *
         *   1. Move on X only -> push out of any wall on X
         *   2. Move on Z only -> push out of any wall on Z
         *
         * Resolving the axes separately is what produces sliding: walking
         * diagonally into a wall cancels the blocked component of the motion
         * but keeps the parallel one.
         */
        glm::vec3 previous = m_Camera->getPosition();

        m_Camera->processInput(deltaTime);
        glm::vec3 desired = m_Camera->getPosition();

        glm::vec3 resolved = previous;

        resolved.x = desired.x;
        ResolveAxis(resolved, 0);

        resolved.z = desired.z;
        ResolveAxis(resolved, 2);

        resolved.y = desired.y; // Grounded camera already pins this to eye height

        m_Camera->setPosition(resolved);
    }

    void Player::Respawn(const glm::vec3& start, float yaw)
    {
        m_Camera->setPosition(glm::vec3(start.x, m_EyeHeight, start.z));
        m_Camera->setYawPitch(yaw, 0.0f);
    }

    void Player::ResolveAxis(glm::vec3& position, int axis) const
    {
        const float wallHalf = m_Map->TileSize() * 0.5f;

        // Nudge used when pushing out of a wall so floating-point error
        // can't leave us still microscopically overlapping next frame.
        const float epsilon = 0.001f;

        // BROAD PHASE: the player box is far smaller than a tile, so it can
        // only ever touch tiles in the 3x3 block around the one it stands in.
        int playerCol, playerRow;
        m_Map->WorldToTile(position, playerCol, playerRow);

        for (int row = playerRow - 1; row <= playerRow + 1; row++)
        {
            for (int col = playerCol - 1; col <= playerCol + 1; col++)
            {
                if (!m_Map->IsWall(col, row))
                    continue;

                glm::vec3 wallCenter = m_Map->TileCenter(col, row);

                /**
                 * AABB OVERLAP TEST (per axis):
                 * Two axis-aligned boxes overlap on an axis when the distance
                 * between their centres is less than the sum of their
                 * half-sizes. They collide only if this holds on BOTH axes.
                 */
                float overlapX = (m_HalfExtent + wallHalf) - std::abs(position.x - wallCenter.x);
                float overlapZ = (m_HalfExtent + wallHalf) - std::abs(position.z - wallCenter.z);

                if (overlapX <= 0.0f || overlapZ <= 0.0f)
                    continue;

                // Push the player back out along ONLY the axis being resolved,
                // to whichever face of the wall they came from.
                if (position[axis] > wallCenter[axis])
                    position[axis] = wallCenter[axis] + wallHalf + m_HalfExtent + epsilon;
                else
                    position[axis] = wallCenter[axis] - wallHalf - m_HalfExtent - epsilon;
            }
        }
    }
}
