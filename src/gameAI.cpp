#include <cfloat>
#include <iostream>
#include <set>
#include <stack>
#include <stdlib.h>
#include <string.h>
#include <utility>

using namespace std;

#include "../include/gameAI.hpp"
#include "../include/level.hpp"
#include "../include/a1.hpp"

// code related to AI logic goes here

// TODO: Get maze and its dimensions dynamically from the current level
float ROW = world.get_maze_height();
float COL = world.get_maze_width();

std::vector<std::vector<int>> maze = world.get_original_maze();

// Create a type to store the location of a tile
typedef pair<int,int> TileCoord;

// Create type to store the cost and location of a tile
typedef pair<double, pair<int,int>> cTileCoord;

// A struct for tiles
struct tile
{
    // coords for parent tile
    int parentI;
    int parentJ;

    // Info for determining cost to reach tile
    // totalCost = pathCost + h, where h is the heuristic value
    // pathCost is the cost of the path so far
    double totalCost;
    double pathCost;
    double h;
};

// Function to determine a tile, given a location
TileCoord findTileFromLocation(vec2 location)
{
    float tile_width = world.get_tile_width();
    float tile_height = world.get_tile_height();

    return {location.x / tile_width, location.y / tile_height};
}

// Function to determine location, given the tile
vec2 findLocationFromTile(TileCoord cell)
{
    float tile_width = world.get_tile_width();
    float tile_height = world.get_tile_height();

    return {cell.first * tile_width, cell.second * tile_height};
}

// Helper function to check if given tile is within maze
bool isValid(int row, int col)
{
    return (row >= 0) && (row < ROW) && (col >= 0) && (col < COL);
}

// Helper function to check if character can move to given tile (i.e. is it occupied by a fixed component)
// TODO: confirm boolean logic here works
bool canMoveHere(std::vector<std::vector<int>> grid, int row, int col)
{
    // Returns true if the tile isn't a fixed component
    if (grid[row][col] == 1)
        return (false);
    else
        return (true);
}

// Helper function to see if current tile is destination tile
bool isDestination(int row, int col, TileCoord dest)
{
    if (row == dest.first && col == dest.second)
        return (true);
    else
        return (false);
}

// Helper function to calculate 'h' (i.e. heuristic of tile).
// Currently using Manhattan distance to calculate the heuristic
double calculateHValue(int row, int col, TileCoord dest)
{
    return (abs(row - dest.first) + abs(col - dest.second));
}

// Returns calculated path from src to dest.
// TODO: want this to return the path and should call A*, not other way around.
stack<vec2> getPath(std::vector<std::vector<tile>> tileDetails, TileCoord dest)
{
    int row = dest.first;
    int col = dest.second;

    stack<vec2> path;

    while (!(tileDetails[row][col].parentI == row && tileDetails[row][col].parentJ == col))
    {
        vec2 node = findLocationFromTile(make_pair(row, col));
        path.push(node);
        int currRow = tileDetails[row][col].parentI;
        int currCol = tileDetails[row][col].parentJ;
        row = currRow;
        col = currCol;
    }

    path.push(vec2({row,col}));

    return path;
}

// A helper function to process a child for A* search
// If the tile details haven't been filled in or if they have and
// this new path is better, add to frontier and update tile details
void processChildAStar(TileCoord parent, TileCoord child, TileCoord dest, set<cTileCoord> *frontier, std::vector<std::vector<tile>> *tileDetails)
{
    set<cTileCoord> f = *frontier;
    std::vector<std::vector<tile>> td = *tileDetails;
    int i = child.first;
    int j = child.second;
    double pathCostNew = td[i][j].pathCost + 1.0;
    double hNew = calculateHValue(i, j, dest);
    double totalCostNew = pathCostNew + hNew;

    if (td[i][j].totalCost == FLT_MAX || td[i][j].totalCost > totalCostNew)
    {
        f.insert(make_pair(totalCostNew, make_pair(i, j)));

        td[i][j].parentI = parent.first;
        td[i][j].parentJ = parent.second;
        td[i][j].totalCost = totalCostNew;
        td[i][j].pathCost = pathCostNew;
        td[i][j].h = hNew;
    }
}

// Find the shortest path using A*
stack<vec2> aStarSearch(vec2 source, vec2 destination)
{
    TileCoord src = findTileFromLocation(source);
    TileCoord dest = findTileFromLocation(destination);

    // Check src is valid and we can be at that location
    if (!(isValid(src.first, src.second) && canMoveHere(maze, src.first, src.second)))
    {
        printf("Source is invalid or is a wall/floor component \n");
        return stack<vec2>();
    }

    // Check dest is valid and we can move to that location
    if (!(isValid(dest.first, dest.second) && canMoveHere(maze, dest.first, dest.second)))
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
    std::vector<std::vector<bool>> visited;

    int w;
    int v;
    for (w = 0; w < ROW; w++)
    {
        for (v = 0; v < COL; v++)
        {
            visited[w][v] = false;
        }
    }

    // Declare a grid to hold the default details of each tile
    std::vector<std::vector<tile>> tileDetails;

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
    set<cTileCoord> frontier;

    // Put the starting tile on the frontier and initialize the totalCost to zero
    frontier.insert(make_pair(0.0, make_pair(i, j)));

    while (!frontier.empty())
    {
        cTileCoord curr = *frontier.begin();

        frontier.erase(frontier.begin());

        // Add to visited
        i = curr.second.first;
        j = curr.second.second;
        visited[i][j] = true;

        // Check if destination
        if (isDestination(i, j, dest) == true)
        {
            return getPath(tileDetails, dest);
        }

        // Go through children of current tile and determine whether they should be added to the frontier/if are destination

        if (isValid(i - 1, j) == true)
        {
            // If child hasn't been visited and it is not a fixed component, process it
            if (visited[i - 1][j] == false && canMoveHere(maze, i - 1.f, j) == true)
            {
                processChildAStar({i, j}, {i - 1, j}, dest, &frontier, &tileDetails);
            }
        }
        if (isValid(i + 1, j) == true)
        {
            if (visited[i + 1][j] == false && canMoveHere(maze, i + 1.f, j) == true)
            {
                processChildAStar({i, j}, {i + 1, j}, dest, &frontier, &tileDetails);
            }
        }
        if (isValid(i, j + 1) == true)
        {
            if (visited[i][j + 1] == false && canMoveHere(maze, i, j + 1.f) == true)
            {
                processChildAStar({i, j}, {i, j + 1}, dest, &frontier, &tileDetails);
            }
        }
        if (isValid(i, j - 1) == true)
        {
            if (visited[i][j - 1] == false && canMoveHere(maze, i, j - 1.f) == true)
            {
                processChildAStar({i, j}, {i, j - 1}, dest, &frontier, &tileDetails);
            }
        }
        // if (isValid(i-1, j+1) == true)
        // {
        //     if (visited[i-1][j+1] == false && canMoveHere(grid, i-1, j+1) == true)
        //     {
        //         processChildAStar({i,j}, {i-1,j+1}, dest, &frontier, &tileDetails);
        //     }
        // }
        // if (isValid (i-1, j-1) == true)
        // {
        //    if (visited[i-1][j-1] == false && canMoveHere(grid, i-1, j-1) == true)
        //     {
        //         processChildAStar({i,j}, {i-1,j-1}, dest, &frontier, &tileDetails);
        //     }
        // }
        // if (isValid(i+1, j+1) == true)
        // {
        //     if (visited[i+1][j+1] == false && canMoveHere(grid, i+1, j+1) == true)
        //     {
        //        processChildAStar({i,j}, {i+1,j+1}, dest, &frontier, &tileDetails);
        //     }
        // }
        // if (isValid (i+1, j-1) == true)
        // {
        //     if (visited[i+1][j-1] == false &&  canMoveHere(grid, i+1, j-1) == true)
        //     {
        //         processChildAStar({i,j}, {i+1,j-1}, dest, &frontier, &tileDetails);
        //     }
        // }
    }

    // When frontier is empty and node is not yet found, print that result and return and empty path
    printf("Failed to find the Destination Cell\n");

    return stack<vec2>();
}