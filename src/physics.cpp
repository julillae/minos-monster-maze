#include "../include/physics.hpp"
#include <algorithm>
#include "../include/common.hpp"
#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

// logic for gravity and potentially friction calculations go here

Physics::Physics() = default;

Physics::~Physics() = default;

bool circleToCircleIntersection(vec2 c1, vec2 c2, float r1, float r2)
{
    float xDistance = c1.x - c2.x;
    float yDistance = c1.y - c2.y;
    float radius = std::max(r1, r2);
    radius *= 0.85f;
    return xDistance * xDistance + yDistance * yDistance <= radius * radius;
}

bool valueInRange(float value, float min, float max) {
    return (value >= min) && (value <= max);
}

bool rectToRectIntersection(vec2 rectA, vec2 rectB, vec2 boundA, vec2 boundB)
{
    bool xOverlap = valueInRange(rectA.x, rectB.x, rectB.x + boundB.x)
                    || valueInRange(rectB.x, rectA.x, rectA.x + boundA.x);

    bool yOverlap = valueInRange(rectA.y, rectB.y, rectB.y + boundB.y)
                    || valueInRange(rectB.y, rectA.y, rectA.y + boundA.y);

    return xOverlap && yOverlap;
}

vec2 calculateNewPosition(vec2 c1, vec2 c2) {
    vec2 colNormal = normalize(add(c2, negateVec(c1)));
    vec2 newPos = add(c1, negateVec(colNormal));
    return newPos;
}

vec2 calculateMinkowskiSum(vec2 rectA, vec2 rectB, vec2 boundA, vec2 boundB)
{
    float wy = ((boundA.x + boundB.x) * (rectA.y - rectB.y));
    float hx = ((boundA.y + boundB.y) * (rectA.x - rectB.x));

    vec2 sum = {wy, hx};

    return sum;
}

Physics::CollisionNode Physics::collideWithEnemy (Player *p, const Enemy *e) {

    bool isCollided = rectToRectIntersection
            (p->get_position(), e->get_position(), p->get_bounding_box(), e->get_bounding_box());

    Physics::CollisionNode collisionNode{};
    collisionNode.isCollided = isCollided;
    collisionNode.angleOfCollision = 0;
    return collisionNode;
}

Physics::CollisionNode Physics::collisionWithFixedWalls(Player *p, const Floor *f) {
    float other_r = std::max(p->get_bounding_box().x, f->get_bounding_box().y);
    float my_r = std::max(p->width, p->height);

    bool isCollided = circleToCircleIntersection(p->get_position(), f->get_position(), other_r, my_r);

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
	float other_r = std::max(p->get_bounding_box().x, e->get_bounding_box().y);
	float my_r = std::max(p->width, p->height);

	bool isCollided = circleToCircleIntersection(p->get_position(), e->get_position(), other_r, my_r);

    Physics::CollisionNode collisionNode{};
    collisionNode.isCollided = isCollided;
    collisionNode.angleOfCollision = 0;
    return collisionNode;
}

void Physics::characterCollisionsWithFixedComponents(Player* c, std::vector<Floor> fixedComponents) {
	bool isOnAtLeastOnePlatform = false;
	bool isLeftOfAtLeastOnePlatform = false;
	bool isRightOfAtLeastOnePlatform = false;
	bool isBelowAtLeastOnePlatform = false;

	Physics::CollisionNode collisionNode;
	for (const auto &floor : fixedComponents) {
		collisionNode = collisionWithFixedWalls(c, &floor);
		if (collisionNode.isCollided) {

		    vec2 floorPos = floor.get_position();
		    vec2 cPos = c->get_position();
		    vec2 floorBound = floor.get_bounding_box();
		    vec2 cBound = c->get_bounding_box();

		    vec2 collisionVector = calculateMinkowskiSum
                    (floorPos, cPos, floorBound, cBound);

		    collisionVector = rotateVec(collisionVector, rotation);

		    float wy = collisionVector.x;
		    float hx = collisionVector.y;

            if (wy > hx) {
                if (wy > -hx) {
                    c->set_on_platform();
                    isOnAtLeastOnePlatform = true;
                } else {
                    isRightOfAtLeastOnePlatform = true;
                }
            } else {
                if (wy > -hx) {
                    isLeftOfAtLeastOnePlatform = true;
                } else {
                    isBelowAtLeastOnePlatform = true;
                }
            }

//			float collisionAngle = collisionNode.angleOfCollision;
//
//			// logic needed to get new angle (collisionAngle + rotation) within
//			// the needed -pi to pi range
//			collisionAngle = static_cast<float>(fmod(collisionAngle + rotation, 2 * M_PI));
//			float anglePastPi = 0.f;
//			if (collisionAngle > M_PI) {
//				anglePastPi = static_cast<float>(collisionAngle - M_PI);
//				collisionAngle = static_cast<float>(-M_PI + anglePastPi);
//			}
//			else if (collisionAngle < -M_PI) {
//				anglePastPi = static_cast<float>(collisionAngle + M_PI);
//				collisionAngle = static_cast<float>(M_PI + anglePastPi);
//			}
//
//			if (collisionAngle > -3 * M_PI / 4 && collisionAngle < -M_PI / 4) {
//				c->set_on_platform();
//				isOnAtLeastOnePlatform = true;
//			}
//
//			if (collisionAngle > -M_PI / 4 && collisionAngle < M_PI / 4) {
//				isLeftOfAtLeastOnePlatform = true;
//			}
//			if (collisionAngle > M_PI / 4 && collisionAngle < 3 * M_PI / 4) {
//				isBelowAtLeastOnePlatform = true;
//			}
//			if (collisionAngle > 3 * M_PI / 4 || collisionAngle < -3 * M_PI / 4) {
//				isRightOfAtLeastOnePlatform = true;
//			}



//			vec2 playPos = c->get_position();
//			int floor_tolerance = 40;
//
//            Direction h_direction = c->get_h_direction();
//
//			vec2 newPos = calculateNewPosition(playPos, floorPos);
//
//			if (floorPos.y - playPos.y < floor_tolerance) {
//                c->set_position(newPos);
//            }
		}
	}

	if (!isOnAtLeastOnePlatform) c->set_in_free_fall();
	c->isLeftOfPlatform = isLeftOfAtLeastOnePlatform;
	c->isRightOfPlatform = isRightOfAtLeastOnePlatform;
	c->isBelowPlatform = isBelowAtLeastOnePlatform;

}

void Physics::characterVelocityUpdate(Character* c)
{
	if (c->characterState->currentState != frozen) {
		float platformDrag = 0.75; //eventually make this a property of the platform
		
		vec2 cVelocity = c->get_velocity();

		// rotate velocity vector back to normal orientation to reuse existing logic
		cVelocity = rotateVec(cVelocity, -rotation);
		vec2 cAcc = c->get_acceleration();
		cVelocity = add(cVelocity, cAcc);

		float maxHorzSpeed = c->maxHorzSpeed;
		float horzDirection = 1.f;
		if (cVelocity.x < 0) horzDirection = -1;
		float horzSpeed = fabs(cVelocity.x);
		horzSpeed = min(maxHorzSpeed, horzSpeed) * horzDirection;
		cVelocity.x = horzSpeed;

		if (c->characterState->currentState == jumping) {
			cVelocity.y += c->jumpVel;
			c->characterState->changeState(rising);
		}

		if (isZero(cAcc.x) && c->isOnPlatform)
			cVelocity.x *= platformDrag;

		if (c->isBelowPlatform) {
			cVelocity.y = std::max(0.f, cVelocity.y);
		}
		if (c->isLeftOfPlatform) {
			cVelocity.x = std::min(0.f, cVelocity.x);
		}
		if (c->isRightOfPlatform) {
			cVelocity.x = std::max(0.f, cVelocity.x);
		}

		if (c->isOnPlatform) {
			cVelocity.y = std::min(0.f, cVelocity.y);
			if (isZero(cAcc.x))
				c->characterState->changeState(idle);
			else
				c->characterState->changeState(running);
		}
		else {
			if (cVelocity.y < 0)
				c->characterState->changeState(rising);
			else
				c->characterState->changeState(falling);
		}

		// rotate back to current rotation
		cVelocity = rotateVec(cVelocity, rotation);
		c->set_velocity(cVelocity);
	}
}

void Physics::characterAccelerationUpdate(Character * c) {
	vec2 horzAcc = {0.f, 0.f};
    Direction h_direction = c->get_h_direction();
    float accStep = c->accStep;

    if (c->is_alive()) {
		if (h_direction == Direction::left)
			horzAcc = { -accStep, 0.f };
		else if (h_direction == Direction::right)
			horzAcc = { accStep, 0.f };
    }

	//TODO: get angle of the platform player is standing on and apply angle as appropriate
	//		for player's horizontal acceleration.
	//		note that the angle should be different depending on whether the player's moving left or right
	//		as long as the platform is not perfectly horizontal
	vec2 newAcc = add(horzAcc, gravityAcc);
    c->set_acceleration(newAcc);
}

void Physics::updateWorldRotation(float currentRotation)
{
	rotation = currentRotation;
}

void Physics::updateCharacterVelocityRotation(Character *c, float vecRotation)
{
	vec2 currentVelocityVector = c->get_velocity();
	vec2 rotatedVelocityVector = rotateVec(currentVelocityVector, vecRotation);
	c->set_velocity(rotatedVelocityVector);
}


bool Physics::isZero(float f) {
	return (fabs(f) < g_tolerance);
}

bool Physics::notZero(float f) {
	return !isZero(f);
}
