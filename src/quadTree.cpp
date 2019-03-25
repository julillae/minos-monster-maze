//
// Created by Henry Deng on 2019-03-24.
//

#include <algorithm>
#include <iostream>
#include "../include/quadTree.hpp"

const int QuadTreeNode::MAX_ENTITIES = 5;
const int QuadTreeNode::MAX_LEVELS = 3;

QuadTreeNode::QuadTreeNode(const int &level, vec2 &initialPosition, float &width, float &height):
        m_level(level), init_position(initialPosition), m_width(width), m_height(height) {};

void QuadTreeNode::clear() {
    m_entities.clear();

    for (auto &childNode: m_children) {
        if (childNode) {
            childNode->clear();
            childNode.reset();
        }
    }
}

void QuadTreeNode::insert(const std::shared_ptr<MazeComponent> &mazeComponent) {
    if (isEntityInNode(mazeComponent)) {
        // Check if there are any children
        if (m_children[0]) {
            for (auto &childNode: m_children) {
                childNode->insert(mazeComponent);
            }
        } else {
            // reached leaf node, so add it to this node
            m_entities.emplace_back(mazeComponent);

            // check if we need to split - can only split up to a certain depth, though
            if (m_entities.size() > MAX_ENTITIES && m_level < MAX_LEVELS) {
                split();
            }
        }
    }
}

std::vector<std::shared_ptr<MazeComponent>> QuadTreeNode::getNearbyEntities
        (const std::shared_ptr<MazeComponent> &mazeComponent) const {
    std::vector<std::shared_ptr<MazeComponent>> nearbyEntities;
    getNearbyEntitiesHelper(nearbyEntities, mazeComponent);

    // remove duplicates
    std::sort(nearbyEntities.begin(), nearbyEntities.end());
    nearbyEntities.erase(std::unique(nearbyEntities.begin(), nearbyEntities.end()), nearbyEntities.end());

    return nearbyEntities;
}

void QuadTreeNode::split() {
    vec2 quadrantSize = {m_width / 2, m_height / 2};

    // create 4 new nodes
    vec2 topLeftOrigin = {init_position.x, init_position.y};
    m_children[0].reset(new QuadTreeNode(m_level + 1, topLeftOrigin, quadrantSize.x, quadrantSize.y));
    vec2 topRightOrigin = {init_position.x + quadrantSize.x, init_position.y};
    m_children[1].reset(new QuadTreeNode(m_level + 1, topRightOrigin, quadrantSize.x, quadrantSize.y));
    vec2 bottomLeftOrigin = {init_position.x, init_position.y + quadrantSize.y};
    m_children[2].reset(new QuadTreeNode(m_level + 1, bottomLeftOrigin, quadrantSize.x, quadrantSize.y));
    vec2 bottomRightOrigin = {init_position.x + quadrantSize.x, init_position.y + quadrantSize.y};
    m_children[3].reset(new QuadTreeNode(m_level + 1, bottomRightOrigin, quadrantSize.x, quadrantSize.y));

    for (auto &entity: m_entities) {
        for (auto &childNode: m_children) {
            childNode->insert(entity);
        }
        entity.reset();
    }
}

void QuadTreeNode::getNearbyEntitiesHelper(std::vector<std::shared_ptr<MazeComponent>> &nearbyEntities,
                                           const std::shared_ptr<MazeComponent> &mazeComponent) const {
    if (isEntityInNode(mazeComponent)) {
        if (m_children[0]) {
            for (auto &childNode: m_children) {
                childNode->getNearbyEntitiesHelper(nearbyEntities, mazeComponent);
            }
        } else {
            for (auto &nearbyEntity: m_entities) {
                if (nearbyEntity != mazeComponent) {
                    nearbyEntities.emplace_back(nearbyEntity);
                }
            }
        }
    }
}

bool QuadTreeNode::isEntityInNode(const std::shared_ptr<MazeComponent> &mazeComponent) const {
    vec2 entityBoundingBox = mazeComponent->get_bounding_box();
    vec2 entityPos = mazeComponent->get_position();
    bool isWithinLeftBounds = entityPos.x + entityBoundingBox.x >= m_width;
    bool isWithinRightBounds = entityPos.x <= init_position.x + m_width;
    bool isWithinTopBounds = entityPos.y + entityBoundingBox.y >= init_position.y;
    bool isWithinBottomBounds = entityPos.y <= init_position.y + m_height;

    return isWithinLeftBounds && isWithinRightBounds && isWithinTopBounds && isWithinBottomBounds;
}

