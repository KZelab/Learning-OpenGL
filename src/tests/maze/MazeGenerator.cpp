#include "MazeGenerator.h"

#include <algorithm>
#include <random>

namespace test
{
    std::vector<std::string> MazeGenerator::Generate(int cols, int rows,
                                                     unsigned int seed,
                                                     int coinCount)
    {
        // The cell/wall layout only works on odd dimensions (cells at odd
        // coordinates, walls between them), so round up if needed.
        if (cols % 2 == 0) cols++;
        if (rows % 2 == 0) rows++;

        // Start with solid rock: every tile is a wall until we carve it.
        std::vector<std::string> grid(rows, std::string(cols, '#'));

        // Distance (in tiles, along carved paths) of each tile from the start.
        // -1 means "not reached". This is filled in as a side effect of the
        // carve and lets us place the door/key at interesting locations.
        std::vector<std::vector<int>> dist(rows, std::vector<int>(cols, -1));

        // Mersenne Twister seeded explicitly: identical seed -> identical maze.
        std::mt19937 rng(seed);

        // =====================================================================
        // RECURSIVE BACKTRACKER (iterative, with an explicit stack)
        // =====================================================================
        // 1. Start at cell (1,1), mark it open.
        // 2. Look at the 4 neighbouring cells two tiles away, in random order.
        // 3. If one is still solid, knock through the wall between, step into
        //    it, and push it on the stack.
        // 4. If none are available, pop the stack (backtrack) and try again.
        // 5. When the stack empties, every cell has been visited exactly once.
        struct Cell { int col, row; };
        std::vector<Cell> stack;

        grid[1][1] = '.';
        dist[1][1] = 0;
        stack.push_back({ 1, 1 });

        const int directions[4][2] = { { 2, 0 }, { -2, 0 }, { 0, 2 }, { 0, -2 } }; // (dCol, dRow)

        while (!stack.empty())
        {
            Cell current = stack.back();

            int order[4] = { 0, 1, 2, 3 };
            std::shuffle(std::begin(order), std::end(order), rng);

            bool carved = false;
            for (int i = 0; i < 4 && !carved; i++)
            {
                int dCol = directions[order[i]][0];
                int dRow = directions[order[i]][1];
                int nextCol = current.col + dCol;
                int nextRow = current.row + dRow;

                // Stay inside the outer border and only enter unvisited cells
                if (nextCol > 0 && nextCol < cols - 1 &&
                    nextRow > 0 && nextRow < rows - 1 &&
                    grid[nextRow][nextCol] == '#')
                {
                    // Knock through the wall tile between the two cells
                    int wallCol = current.col + dCol / 2;
                    int wallRow = current.row + dRow / 2;
                    grid[wallRow][wallCol] = '.';
                    grid[nextRow][nextCol] = '.';

                    dist[wallRow][wallCol] = dist[current.row][current.col] + 1;
                    dist[nextRow][nextCol] = dist[current.row][current.col] + 2;

                    stack.push_back({ nextCol, nextRow });
                    carved = true;
                }
            }

            if (!carved)
                stack.pop_back(); // Dead end reached - backtrack
        }

        // =====================================================================
        // EXIT DOOR PLACEMENT
        // =====================================================================
        // Find the open tile adjacent to the outer border with the greatest
        // path distance from the start, and turn the border wall next to it
        // into the door. Maximizing distance guarantees a non-trivial route.
        int doorCol = -1, doorRow = -1;       // The 'D' tile (in the border)
        int doorInnerCol = -1, doorInnerRow = -1; // The open tile beside it
        int bestDoorDist = -1;

        for (int row = 1; row < rows - 1; row++)
        {
            for (int col = 1; col < cols - 1; col++)
            {
                if (grid[row][col] != '.' || dist[row][col] <= bestDoorDist)
                    continue;

                // Which border (if any) does this open tile touch?
                int bCol = -1, bRow = -1;
                if (col == 1)             { bCol = 0;        bRow = row; }
                else if (col == cols - 2) { bCol = cols - 1; bRow = row; }
                else if (row == 1)        { bCol = col;      bRow = 0; }
                else if (row == rows - 2) { bCol = col;      bRow = rows - 1; }

                if (bCol != -1)
                {
                    bestDoorDist = dist[row][col];
                    doorCol = bCol;       doorRow = bRow;
                    doorInnerCol = col;   doorInnerRow = row;
                }
            }
        }
        grid[doorRow][doorCol] = 'D';

        // =====================================================================
        // DEAD-END DETECTION (for key and coin placement)
        // =====================================================================
        // A dead-end is an open tile with exactly one open orthogonal
        // neighbour - the natural "reward spots" of a perfect maze.
        struct DeadEnd { int col, row, distance; };
        std::vector<DeadEnd> deadEnds;

        for (int row = 1; row < rows - 1; row++)
        {
            for (int col = 1; col < cols - 1; col++)
            {
                if (grid[row][col] != '.') continue;
                if (col == 1 && row == 1) continue;                       // start tile
                if (col == doorInnerCol && row == doorInnerRow) continue; // door approach

                int openNeighbours = 0;
                if (grid[row][col - 1] != '#') openNeighbours++;
                if (grid[row][col + 1] != '#') openNeighbours++;
                if (grid[row - 1][col] != '#') openNeighbours++;
                if (grid[row + 1][col] != '#') openNeighbours++;

                if (openNeighbours == 1)
                    deadEnds.push_back({ col, row, dist[row][col] });
            }
        }

        // Key goes at the FARTHEST dead-end: finding it is the real challenge.
        std::sort(deadEnds.begin(), deadEnds.end(),
            [](const DeadEnd& a, const DeadEnd& b) { return a.distance > b.distance; });

        if (!deadEnds.empty())
        {
            grid[deadEnds[0].row][deadEnds[0].col] = 'K';
            deadEnds.erase(deadEnds.begin());
        }

        // Coins are scattered over the remaining dead-ends at random.
        std::shuffle(deadEnds.begin(), deadEnds.end(), rng);
        int placed = 0;
        for (const DeadEnd& deadEnd : deadEnds)
        {
            if (placed >= coinCount) break;
            grid[deadEnd.row][deadEnd.col] = 'C';
            placed++;
        }

        // If the maze had fewer dead-ends than requested coins, fall back to
        // random open floor tiles so the requested count is still honoured.
        if (placed < coinCount)
        {
            std::vector<Cell> openTiles;
            for (int row = 1; row < rows - 1; row++)
                for (int col = 1; col < cols - 1; col++)
                    if (grid[row][col] == '.' && !(col == 1 && row == 1))
                        openTiles.push_back({ col, row });

            std::shuffle(openTiles.begin(), openTiles.end(), rng);
            for (const Cell& tile : openTiles)
            {
                if (placed >= coinCount) break;
                grid[tile.row][tile.col] = 'C';
                placed++;
            }
        }

        grid[1][1] = 'S';
        return grid;
    }
}
