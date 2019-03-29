//
// Created by Henry Deng on 2019-03-24.
//

#include <algorithm>
#include <iostream>
#include <cmath>
#include "../include/quadTree.hpp"

const int QuadTreeNode::MAX_ENTITIES = 5;
const int QuadTreeNode::MAX_LEVELS = 3;

QuadTreeNode::QuadTreeNode(const int &level, vec2 initialPosition, float width, float height):
        m_level(level), init_position(initialPosition), m_width(width), m_height(height) {};

void QuadTreeNode::clear() {
    m_floors.clear();

    for (auto &childNode: m_children) {
        if (childNode) {
            childNode->clear();
            childNode.reset();
        }
    }
}

void QuadTreeNode::insert(const Floor &floor) {
    if (isEntityInNode(floor.get_position(), floor.get_bounding_box())) {
        // check if there are any children
        if (m_children[0]) {
            for (auto &childNode: m_children) {
                childNode->insert(floor);
            }
        } else {
            // reached leaf node, so add it to this node
            m_floors.emplace_back(floor);

            // split until we reach the max levels of indirection
            if (m_floors.size() > MAX_ENTITIES && m_level < MAX_LEVELS) {
                split();
            }
        }
    }
}

std::vector<Floor> QuadTreeNode::getNearbyFloorComponents
        (vec2 pos, vec2 aabb) const {
    std::vector<Floor> nearbyFloorComponents;
    getNearbyFloorComponentsHelper(nearbyFloorComponents, pos, aabb);

    return nearbyFloorComponents;
}

void QuadTreeNode::split() {
    vec2 quadrantSize = {m_width/2, m_height/2};

    // create 4 new nodes
    vec2 topLeft = {init_position.x, init_position.y};
    m_children[0].reset(new QuadTreeNode(m_level + 1, topLeft, quadrantSize.x, quadrantSize.y));
    vec2 topRight = {init_position.x + quadrantSize.x, init_position.y};
    m_children[1].reset(new QuadTreeNode(m_level + 1, topRight, quadrantSize.x, quadrantSize.y));
    vec2 bottomLeft = {init_position.x, init_position.y + quadrantSize.y};
    m_children[2].reset(new QuadTreeNode(m_level + 1, bottomLeft, quadrantSize.x, quadrantSize.y));
    vec2 bottomRight = {init_position.x + quadrantSize.x, init_position.y + quadrantSize.y};
    m_children[3].reset(new QuadTreeNode(m_level + 1, bottomRight, quadrantSize.x, quadrantSize.y));

    for (auto &floor: m_floors) {
        for (auto &childNode: m_children) {
            childNode->insert(floor);
        }
    }
}

void QuadTreeNode::getNearbyFloorComponentsHelper(std::vector<Floor> &nearbyFloors,
                                           vec2 pos, vec2 aabb) const {
    if (isEntityInNode(pos, aabb)) {
        if (m_children[0]) {
            for (auto &childNode: m_children) {
                childNode->getNearbyFloorComponentsHelper(nearbyFloors, pos, aabb);
            }
        } else {
            for (auto& floor: m_floors) {
                nearbyFloors.emplace_back(floor);
            }
        }
    }
}

bool QuadTreeNode::isEntityInNode(vec2 pos, vec2 aabb) const {
    vec2 entityBoundingBox = aabb;
    vec2 entityPos = pos;
    bool isWithinLeftBounds = entityPos.x + entityBoundingBox.x >= init_position.x;
    bool isWithinRightBounds = entityPos.x <= init_position.x + m_width;
    bool isWithinTopBounds = entityPos.y + entityBoundingBox.y >= init_position.y;
    bool isWithinBottomBounds = entityPos.y <= init_position.y + m_height;

    return isWithinLeftBounds && isWithinRightBounds && isWithinTopBounds && isWithinBottomBounds;
}

