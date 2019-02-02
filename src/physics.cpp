#include "../include/physics.hpp"
#include <algorithm>

// logic for gravity and potentially friction calculations go here

Physics::Physics() = default;

Physics::~Physics() = default;

float Physics::lineIntersection(float x_pos1, float x_pos2, float y_pos1, float y_pos2) {
    float dx = x_pos1 - x_pos2;
    float dy = y_pos1 - y_pos2;
    float d_sq = dx * dx + dy * dy;
    return d_sq;
}

float Physics::boundingBox(float bb_x, float bb_y, float scale_x, float scale_y)
{
    float other_r = std::max(bb_x, bb_y);
    float my_r = std::max(scale_x, scale_y);
    float r = std::max(other_r, my_r);
    return r;
}


Physics::CollisionNode Physics::collideWithEnemy (Player *c, const Enemy *t) {
    float d_sq = lineIntersection(c->get_position().x, t->get_position().x, c->get_position().y, t->get_position().y);
    float r = boundingBox(t->get_bounding_box().x, t->get_bounding_box().y, c->get_scale().x, c->get_scale().y);

    r *= 0.6f;
    bool isCollided = false;
    if (d_sq < r * r) {
        isCollided = true;
    }

    Physics::CollisionNode collisionNode{};
    collisionNode.isCollided = isCollided;
    collisionNode.angleOfCollision = 0;
    return collisionNode;
}

Physics::CollisionNode Physics::collisionWithFixedWalls(Player *p, const Floor *f) {
    float d_sq = lineIntersection(p->get_position().x, f->get_position().x, p->get_position().y, f->get_position().y);
    float r = boundingBox(f->get_bounding_box().x, f->get_bounding_box().y, p->get_scale().x, p->get_scale().y);

    r *= 0.6f;
    bool isCollided = false;
    if (d_sq < r * r) {
        isCollided = true;
    }

    CollisionNode collisionNode{};
    collisionNode.isCollided = isCollided;

    if (isCollided) {
        collisionNode.angleOfCollision = f->get_rotation();
    } else {
        collisionNode.angleOfCollision = 0;
    }
    return collisionNode;
}

