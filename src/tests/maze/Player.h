/**
 * Player.h
 *
 * First-person player: wraps the Camera in grounded mode and stops it
 * walking through maze walls.
 *
 * KEY CONCEPTS COVERED:
 * ---------------------
 * 1. AABB (Axis-Aligned Bounding Box) overlap testing
 * 2. AXIS-SEPARATED collision resolution ("move and slide"):
 *    the frame's movement is applied one axis at a time, and each axis is
 *    resolved independently. If you walk diagonally into a wall, the blocked
 *    axis is cancelled but the free axis still moves - so you slide along
 *    the wall instead of sticking to it.
 * 3. Broad-phase optimisation - only the 3x3 tiles around the player are
 *    tested, not the whole grid.
 */

#pragma once
#include <glm/glm.hpp>

#include "../../utils/Camera.h"
#include "MazeMap.h"

namespace test
{
    class Player
    {
    public:
        Player(Camera* camera, MazeMap* map);

        /**
         * Runs camera input for this frame, then corrects the resulting
         * position so the player's bounding box never overlaps a wall tile.
         */
        void Update(float deltaTime);

        // Teleport to a level's start tile, facing along its corridor
        void Respawn(const glm::vec3& start, float yaw);

        float EyeHeight() const { return m_EyeHeight; }

    private:
        /**
         * Resolves one axis of movement.
         *
         * @param position - Player position with the axis already moved;
         *                   pushed back out of any overlapping wall in place
         * @param axis     - 0 = X, 2 = Z (glm::vec3 component index)
         */
        void ResolveAxis(glm::vec3& position, int axis) const;

        Camera* m_Camera;
        MazeMap* m_Map;

        // Half-size of the player's AABB in the XZ plane. The player is a
        // 0.6 x 0.6 box - small enough to fit corridors, big enough that
        // walls never clip through the near plane of the camera.
        float m_HalfExtent = 0.3f;

        float m_EyeHeight = 1.6f;
    };
}
