#include <cfloat>
#include <iostream>
#include <set>
#include <stack>
#include <stdlib.h>
#include <string.h>
#include <utility>
#include <queue>
#include <functional>

using namespace std;

#include "../include/gameAI.hpp"
#include "../include/level.hpp"
#include "../include/a1.hpp"

// code related to AI logic goes here

float ROW;
float COL;

std::vector<std::vector<int>> maze;

// Function to determine a tile, given a location
TileCoord GameAI::findTileFromLocation(vec2 location)
{
    float tile_width = world.get_tile_width();
    float tile_height = world.get_tile_height();

    return {location.y / tile_width, location.x / tile_height};
}

// Function to determine location, given the tile
vec2 GameAI::findLocationFromTile(TileCoord cell)
{
    float tile_width = world.get_tile_width();
    float tile_height = world.get_tile_height();
    vec2 result = vec2({cell.second * tile_width, cell.first * tile_height});

    return result;
}

// Helper function to check if given tile is within maze
bool GameAI::isValid(int row, int col)
{
    return (row >= 0) && (row < ROW) && (col >= 0) && (col < COL);
}

// Helper function to check if character can move to given tile (i.e. is it occupied by a fixed component)
bool GameAI::canMoveHere(int row, int col)
{
    // Returns true if the tile isn't a fixed component
    if (maze[row][col] == 1) {
        return (false);
    } else { 
        return (true);
    }
}

// Helper function to see if current tile is destination tile
bool GameAI::isDestination(int row, int col, TileCoord dest)
{
    if (row == dest.first && col == dest.second) {
        return (true);
    } else {
        return (false);
    }
}

// Helper function to calculate 'h' (i.e. heuristic of tile).
// Currently using Manhattan distance to calculate the heuristic
double GameAI::calculateHValue(int row, int col, TileCoord dest)
{
    return (abs(row - dest.first) + abs(col - dest.second));
}

// Returns calculated path from src to dest.
stack<vec2> GameAI::getPath(std::vector<std::vector<tile>> tileDetails, TileCoord src, TileCoord dest)
{
    int row = dest.first;
    int col = dest.second;

    stack<vec2> path;

    while (!(tileDetails[row][col].parentI == src.first && tileDetails[row][col].parentJ == src.second))
    {
        vec2 node = findLocationFromTile(make_pair(row, col));
        path.push(node);
        int currRow = tileDetails[row][col].parentI;
        int currCol = tileDetails[row][col].parentJ;
        row = currRow;
        col = currCol;
    }
    vec2 last = findLocationFromTile(make_pair(row, col));
    path.push(last);

    return path;
}

// A helper function to process a child for A* search
// If the tile details haven't been filled in or if they have and
// this new path is better, add to frontier and update tile details
void GameAI::processChildAStar(TileCoord parent, TileCoord child, TileCoord dest, priority_queue<cTileCoord, std::vector<cTileCoord>, std::function<bool(cTileCoord, cTileCoord)>> &frontier, std::vector<std::vector<tile>> &tileDetails)
{
    int i = child.first;
    int j = child.second;
    double pathCostNew = tileDetails[parent.first][parent.second].pathCost + 1.0;
    double hNew = calculateHValue(i, j, dest);
    double totalCostNew = pathCostNew + hNew;

    if (tileDetails[i][j].totalCost == FLT_MAX || tileDetails[i][j].totalCost > totalCostNew)
    {
        frontier.push(make_pair(totalCostNew, make_pair(i, j)));

        tileDetails[i][j].parentI = parent.first;
        tileDetails[i][j].parentJ = parent.second;
        tileDetails[i][j].totalCost = totalCostNew;
        tileDetails[i][j].pathCost = pathCostNew;
        tileDetails[i][j].h = hNew;
    }
}

stack<vec2> GameAI::followPlayer(vec2 source, SearchMethod method)
{
    if (method == SearchMethod::aStar) {
        vec2 dest = world.m_player.get_position();
        return aStarSearch(source, dest);
    } else {
        printf("Must specify valid search method\n");
        return stack<vec2>();
    }
}

bool GameAI::compare(cTileCoord left, cTileCoord right)
{
    return (left.first) > (right.first);
}

// Find the shortest path using A*
stack<vec2> GameAI::aStarSearch(vec2 source, vec2 destination)
{
    ROW = world.get_maze_height();
    COL = world.get_maze_width();
    maze = world.get_original_maze();
    TileCoord src = findTileFromLocation(source);
    TileCoord dest = findTileFromLocation(destination);

    // Check dest is valid and we can move to that location
    if (!(isValid(src.first, src.second) && canMoveHere(src.first, src.second)))
    {
        printf("Source is invalid or is a wall/floor component\n");
        return stack<vec2>();
    }

    // Check dest is valid and we can move to that location
    if (!(isValid(dest.first, dest.second) && canMoveHere(dest.first, dest.second)))
    {
        printf("Destination is invalid or is a wall/floor component\n");
        return stack<vec2>();
    }

    // If the destination tile is the same as source tile
    if (isDestination(src.first, src.second, dest) == true)
    {
        printf("We are already at the destination\n");
        return stack<vec2>();
    }

    // Create a grid of 'visited' tiles and initialize them all to false
    // will update to true once a given tile is visited
    std::vector<std::vector<bool>> visited(ROW, std::vector<bool>(COL, false));
    // Declare a grid to hold the default details of each tile
    std::vector<std::vector<tile>> tileDetails(ROW, std::vector<tile>(COL));
    int i;
    int j;

    for (i = 0; i < ROW; i++)
    {
        for (j = 0; j < COL; j++)
        {

            tileDetails[i][j].parentI = -1;
            tileDetails[i][j].parentJ = -1;
            tileDetails[i][j].totalCost = FLT_MAX;
            tileDetails[i][j].pathCost = FLT_MAX;
            tileDetails[i][j].h = FLT_MAX;
        }
    }

    // Initialize the src tile
    i = src.first;
    j = src.second;
    tileDetails[i][j].parentI = i;
    tileDetails[i][j].parentJ = j;
    tileDetails[i][j].totalCost = 0.0;
    tileDetails[i][j].pathCost = 0.0;
    tileDetails[i][j].h = 0.0;

    // Create an frontier of cTileCoords to explore, where each has the totalCost and i,j coordinates
    priority_queue<cTileCoord, std::vector<cTileCoord>, std::function<bool(cTileCoord, cTileCoord)>> frontier(compare);
    // Put the starting tile on the frontier and initialize the totalCost to zero
    frontier.push(make_pair(0.0, make_pair(i, j)));

    while (!frontier.empty())
    {
        cTileCoord curr = frontier.top();

        frontier.pop();

        // Add to visited
        i = curr.second.first;
        j = curr.second.second;
        visited[i][j] = true;

        // Check if destination
        if (isDestination(i, j, dest) == true)
        {
            return getPath(tileDetails, src, dest);
        }

        // Go through children of current tile and determine whether they should be added to the frontier/if are destination

        if (isValid(i - 1, j))
        {
            // If child hasn't been visited and it is not a fixed component, process it
            if ((visited[i - 1][j] == false) && canMoveHere(i - 1.f, j))
            {
                processChildAStar({i, j}, {i - 1, j}, dest, frontier, tileDetails);
            }
        }
        if (isValid(i + 1, j))
        {
            if ((visited[i + 1][j] == false) && canMoveHere(i + 1.f, j))
            {
                processChildAStar({i, j}, {i + 1, j}, dest, frontier, tileDetails);
            }
        }
        if (isValid(i, j + 1))
        {
            if ((visited[i][j + 1] == false) && canMoveHere(i, j + 1.f))
            {
                processChildAStar({i, j}, {i, j + 1}, dest, frontier, tileDetails);
            }
        }
        if (isValid(i, j - 1))
        {
            if ((visited[i][j - 1] == false) && canMoveHere(i, j - 1.f))
            {
                processChildAStar({i, j}, {i, j - 1}, dest, frontier, tileDetails);
            }
        }
    }

    // When frontier is empty and node is not yet found, print that result and return and empty path
    printf("Failed to find the Destination Cell\n");

    return stack<vec2>();
}