/**
 * MazeGenerator.h
 *
 * Procedural maze generation using the RECURSIVE BACKTRACKER algorithm
 * (a randomized depth-first search).
 *
 * KEY CONCEPTS COVERED:
 * ---------------------
 * 1. Depth-First Search with an explicit stack (no recursion limit)
 * 2. Seeded pseudo-random numbers (std::mt19937) for reproducible output
 * 3. "Perfect" mazes - every cell reachable, exactly one path between
 *    any two cells, no loops
 * 4. Distance fields - tracking how far each cell is from the start,
 *    used to place the key, exit door, and coins meaningfully
 *
 * GRID REPRESENTATION:
 * The maze is a grid of characters, one per tile:
 *   '#' wall      '.' floor       'S' player start
 *   'C' coin      'K' key         'D' exit door (in the outer border wall)
 *
 * The algorithm works on ODD-SIZED grids where "cells" live at odd
 * (col,row) coordinates and the tiles between them are the walls that
 * get knocked through:
 *
 *   # # # # #        # # # # #
 *   # c # c #        # . . . #     c = cell (always carved)
 *   # # # # #   ->   # # # . #     walls between cells are removed
 *   # c # c #        # . . . #     as the DFS visits neighbours
 *   # # # # #        # # # # #
 */

#pragma once
#include <string>
#include <vector>

namespace test
{
    class MazeGenerator
    {
    public:
        /**
         * Generates a maze grid.
         *
         * @param cols      - Grid width in tiles (forced up to an odd number)
         * @param rows      - Grid height in tiles (forced up to an odd number)
         * @param seed      - RNG seed; the same seed always produces the same maze
         * @param coinCount - How many 'C' tiles to scatter at dead-ends
         *
         * Entity placement (after carving):
         * - 'S' at cell (1,1), where the carve started
         * - 'D' replaces the border wall next to the reachable cell that is
         *       FARTHEST from the start along the maze paths
         * - 'K' at the farthest dead-end (so the key is a journey of its own)
         * - 'C' coins at randomly chosen remaining dead-ends
         */
        static std::vector<std::string> Generate(int cols, int rows,
                                                 unsigned int seed,
                                                 int coinCount);
    };
}
