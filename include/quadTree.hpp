//
// Created by Henry Deng on 2019-03-24.
//

#pragma once


#include <vector>
#include <array>
#include <memory>
#include "../include/mazeComponents/mazeComponent.hpp"

class QuadTreeNode {
public:
    QuadTreeNode(const int &level, vec2 &initialPosition, float &width, float &height);
    ;

    /**
     * Clear the tree
     */
    void clear();

    void insert(const std::shared_ptr<MazeComponent> &mazeComponent);

    std::vector<std::shared_ptr<MazeComponent>> getNearbyEntities(const std::shared_ptr<MazeComponent> &mazeComponent) const;

private:

    static const int MAX_ENTITIES;

    static const int MAX_LEVELS;

    /**
     * A node can either be childless or have four children.
     * Indices map to quadrants as follows:
     * 0 - Top left
     * 1 - Top right
     * 2 - Bottom left
     * 3 - Bottom right
     */
    std::array<std::unique_ptr<QuadTreeNode>, 4> m_children;

    /**
     * The entities contained by this node
     */
    std::vector<std::shared_ptr<MazeComponent>> m_entities;

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

    /**
     * Divide this node's entities amongst four new nodes that are created as children to this node
     */
    void split();

    void getNearbyEntitiesHelper(std::vector<std::shared_ptr<MazeComponent>> &nearbyMazeComponents,
                                 const std::shared_ptr<MazeComponent> &mazeComponent) const;

    /**
     * Checks if the given entity is in this node's region
     * @param entity
     * @return true if entity is within node
     */
    bool isEntityInNode(const std::shared_ptr<MazeComponent> &mazeComponent) const;

};
