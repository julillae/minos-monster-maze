#include "../include/physics.hpp"
#include <algorithm>
#include "../include/common.hpp"
#define _USE_MATH_DEFINES
#include <math.h>

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
    float r = boundingBox(f->get_bounding_box().x, f->get_bounding_box().y, p->width, p->height);

    r *= 0.9f;
    bool isCollided = false;
    if (d_sq < r * r) {
        isCollided = true;
    }

    CollisionNode collisionNode{};
    collisionNode.isCollided = isCollided;

    if (isCollided) {
		float dy = p->get_position().y - f->get_position().y;
		float dx = f->get_position().x - p->get_position().x;
		float collisionAngle = atan2(dy, dx);
        collisionNode.angleOfCollision = collisionAngle;
    } else {
        collisionNode.angleOfCollision = 0;
    }
    return collisionNode;
}

Physics::CollisionNode Physics::collideWithExit (Player *p, const Exit *e) {
    float d_sq = lineIntersection(p->get_position().x, e->get_position().x, p->get_position().y, e->get_position().y);
    float r = boundingBox(e->get_bounding_box().x, e->get_bounding_box().y, p->get_scale().x, p->get_scale().y);

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

void Physics::characterCollisionsWithFixedComponents(Player* c, std::vector<Floor> fixedComponents)
{
	bool isOnAtLeastOnePlatform = false;
	bool isLeftOfAtLeastOnePlatform = false;
	bool isRightOfAtLeastOnePlatform = false;
	bool isBelowAtLeastOnePlatform = false;

	Physics::CollisionNode collisionNode;
	for (const auto& floor : fixedComponents)
	{
		collisionNode = collisionWithFixedWalls(c, &floor);
		if (collisionNode.isCollided)
		{
			float collisionAngle = collisionNode.angleOfCollision;
			if (collisionAngle > -3 * M_PI / 4 && collisionAngle < -M_PI / 4) {
				c->set_on_platform(c->get_position().y);
				isOnAtLeastOnePlatform = true;
			}

			if (collisionAngle > -M_PI / 4 && collisionAngle < M_PI / 4) {
				isLeftOfAtLeastOnePlatform = true;
			}
			if (collisionAngle > M_PI / 4 && collisionAngle < 3 * M_PI / 4) {
				isBelowAtLeastOnePlatform = true;
			}
			if (collisionAngle > 3 * M_PI / 4 || collisionAngle < -3 * M_PI / 4) {
				isRightOfAtLeastOnePlatform = true;
			}
		}
	}
	if (!isOnAtLeastOnePlatform) c->set_in_free_fall();
	c->isLeftOfPlatform = isLeftOfAtLeastOnePlatform;
	c->isRightOfPlatform = isRightOfAtLeastOnePlatform;
	c->isBelowPlatform = isBelowAtLeastOnePlatform;

}

void Physics::characterVelocityUpdate(Player* c)
{
	float platformDrag = 0.75; //eventually make this a property of the platform

	vec2 cAcc = c->get_acceleration();
	vec2 cVelocity = c->get_velocity();
	float maxVelocity = c->maxVelocity;

	cVelocity.x += cAcc.x;
	cVelocity.y += cAcc.y;

	if (cVelocity.x > maxVelocity) cVelocity.x = maxVelocity;
	if (cVelocity.x < -maxVelocity) cVelocity.x = -maxVelocity;

	if (cAcc.x < g_tolerance && cAcc.x > -g_tolerance && c->isOnPlatform)
		cVelocity.x *= platformDrag;

	if (c->isOnPlatform) {
		cVelocity.y = std::min(0.f, cVelocity.y);
	}
	if (c->isBelowPlatform) {
		cVelocity.y = std::max(0.f, cVelocity.y);
	}
	if (c->isLeftOfPlatform) {
		cVelocity.x = std::min(0.f, cVelocity.x);
	}
	if (c->isRightOfPlatform) {
		cVelocity.x = std::max(0.f, cVelocity.x);
	}

	c->set_velocity(cVelocity);
}

void Physics::characterAccelerationUpdate(Player * c)
{
	float vAcc;
	float hAcc;
	Direction h_direction = c->get_h_direction();
	float accStep = c->accStep;

	if (c->is_alive()) {
		switch (h_direction) {
			case Direction::left: hAcc = -accStep; break;
			case Direction::right: hAcc = accStep; break;
			default: hAcc = 0.f; break;
		}
		vAcc = gravityAcc;
		c->set_acceleration({ hAcc, vAcc });
	} else {
		c->set_rotation(M_PI);
	}

}

