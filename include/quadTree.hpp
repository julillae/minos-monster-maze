//
// Created by Henry Deng on 2019-03-24.
//

#pragma once

#include <vector>
#include <array>
#include "mazeComponents/floor.hpp"
#include <memory>


class QuadTreeNode {
public:
    QuadTreeNode(const int &level, vec2 initialPosition, float width, float height);
    ;

    /**
     * Clears out the tree
     */
    void clear();

    /**
     * Inserts a floor node into the QuadTree
     * @param floor
     */
    void insert(const Floor &floor);

    /**
     * Retrieves floor components that are closet to coordinate
     * @param pos
     * @param aabb
     * @return list of floor components
     */
    std::vector<Floor> getNearbyFloorComponents(vec2 pos, vec2 aabb) const;

private:

    static const int MAX_ENTITIES;

    static const int MAX_LEVELS;

    /**
     * A node can have up to four children or none at all
    */

    std::array<std::unique_ptr<QuadTreeNode>, 4> m_children;

    /**
     * The floors contained by this node
     */
    std::vector<Floor> m_floors;

    /**
     * The node's level
     */
    int m_level;

    /**
     * Initial position
     */
    vec2 init_position;

    /**
     * Width of map
     */
    float m_width;

    /**
     * Height of map
     */
    float m_height;

private:

    void split();

    /**
     * Helper function to retrieve nearby floor components
     * @param nearbyFloor
     * @param pos
     * @param aabb
     */
    void getNearbyFloorComponentsHelper(std::vector<Floor> &nearbyFloor,
                                 vec2 pos, vec2 aabb) const;

    /**
     * Checks whether an object is within a node of the quad tree
     * @param pos
     * @param aabb
     * @return
     */
    bool isEntityInNode(vec2 pos, vec2 aabb) const;

};
