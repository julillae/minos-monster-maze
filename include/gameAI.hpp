#pragma once


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

class GameAI {

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

    public:
        // Function to determine a tile, given a location
        TileCoord static findTileFromLocation(vec2 location){};

        // Function to determine location, given the tile
        vec2 static findLocationFromTile(TileCoord cell){};

        // Helper function to check if given tile is within maze
        bool static isValid(int row, int col){};

        // Helper function to check if character can move to given tile (i.e. is it occupied by a fixed component)
        // TODO: confirm boolean logic here works
        bool static canMoveHere(std::vector<std::vector<int>> grid, int row, int col){};

        // Helper function to see if current tile is destination tile
        bool static isDestination(int row, int col, TileCoord dest){};

        // Helper function to calculate 'h' (i.e. heuristic of tile).
        // Currently using Manhattan distance to calculate the heuristic
        double static calculateHValue(int row, int col, TileCoord dest){};

        // Returns calculated path from src to dest.
        // TODO: want this to return the path and should call A*, not other way around.
        stack<vec2> static getPath(std::vector<std::vector<tile>> tileDetails, TileCoord dest){};

        // A helper function to process a child for A* search
        // If the tile details haven't been filled in or if they have and
        // this new path is better, add to frontier and update tile details
        void static processChildAStar(TileCoord parent, TileCoord child, TileCoord dest, set<cTileCoord> *frontier, std::vector<std::vector<tile>> *tileDetails){};

        // Find the shortest path using A*
        stack<vec2> static aStarSearch(vec2 source, vec2 destination){};
};