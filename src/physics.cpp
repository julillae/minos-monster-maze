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

vec2 Physics::calculateDimensionsAfterRotation(vec2 c1, vec2 bound) {
	float midX = c1.x + bound.x/2;
	float midY = c1.y + bound.y/2;

	float cornersX[4] = {c1.x - midX, c1.x - midX, c1.x + bound.x - midX, c1.x + bound.x - midX};
	float cornersY[4] = {c1.y - midY, c1.y - midY, c1.y + bound.y - midY, c1.y + bound.y - midY};

	float newX = 1e10;
	float newY = 1e10;

	for (int i=0; i<4; i=i+1) {
		newX = min(newX, cornersX[i]*cosf(rotation) - cornersY[i]*sin(rotation) + midX);
		newY = min(newY, cornersX[i]*sinf(rotation) - cornersY[i]*cos(rotation) + midY);
	}

	float newWidth = midX - newX;
	float newHeight = midY - newY;

	vec2 newDimensions = {newWidth, newHeight};

	return newDimensions;
}

Physics::CollisionNode Physics::collideWithEnemy (Player *p, const Enemy *e) {

    bool isCollided = rectToRectIntersection
            (p->get_position(), e->get_position(), p->get_bounding_box(), e->get_bounding_box());

    Physics::CollisionNode collisionNode{};
    collisionNode.isCollided = isCollided;
    collisionNode.angleOfCollision = 0;
    return collisionNode;
}

Physics::CollisionNode Physics::collisionWithFixedWalls(Player *p, std::unique_ptr<FixedComponent> const &f) {
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

void Physics::characterCollisionsWithFixedComponents(Player* c, const std::vector<std::unique_ptr<FixedComponent>> &fixedComponents) {
	bool isOnAtLeastOnePlatform = false;
	bool isLeftOfAtLeastOnePlatform = false;
	bool isRightOfAtLeastOnePlatform = false;
	bool isBelowAtLeastOnePlatform = false;

	Physics::CollisionNode collisionNode;
	for (const auto& fc : fixedComponents) {
		collisionNode = collisionWithFixedWalls(c, std::move(fc));
		if (collisionNode.isCollided) {
			float collisionAngle = collisionNode.angleOfCollision;

			// logic needed to get new angle (collisionAngle + rotation) within
			// the needed -pi to pi range
			collisionAngle = fmod(collisionAngle + rotation, 2 * M_PI);
			float anglePastPi = 0.f;
			if (collisionAngle > M_PI) {
				anglePastPi = collisionAngle - M_PI;
				collisionAngle = -M_PI + anglePastPi;
			}
			else if (collisionAngle < -M_PI) {
				anglePastPi = collisionAngle + M_PI;
				collisionAngle = M_PI + anglePastPi;
			}

			if (collisionAngle > -3 * M_PI / 4 && collisionAngle < -M_PI / 4) {
				c->set_on_platform();
				isOnAtLeastOnePlatform = true;
				c->m_platform_drag = fc->get_drag();
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


			//TODO: doublecheck the logic that pushes player back "up" to see if it still works properly with rotation
			// get the floor position
			vec2 floorPos = fc->get_position();
			vec2 playPos = c->get_position();
			int floor_tolerance = 40;

			// get character's direction
            Direction h_direction = c->get_h_direction();

			// get the normalized vector
			vec2 newPos = calculateNewPosition(playPos, floorPos);

//			if (rotation > 0 && rotation < M_PI/2 && h_direction == Direction::right) {
//			   // calculate new dimensions
//
//            } else if (rotation > 0 && rotation < M_PI/2 && h_direction == Direction::left) {
//				// calculate new dimensions
//            }

            // if the player position deviates too much from the floor position, push the player back up
			if (floorPos.y - playPos.y < floor_tolerance) {
                c->set_position(newPos);
            }
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
		float platformDrag = c->m_platform_drag;
		
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

void Physics::updateCharacterVelocityRotation(Character *c)
{
	vec2 currentVelocityVector = c->get_velocity();
	vec2 rotatedVelocityVector = rotateVec(currentVelocityVector, rotation);
	c->set_velocity(rotatedVelocityVector);
}


bool Physics::isZero(float f) {
	return (fabs(f) < g_tolerance);
}

bool Physics::notZero(float f) {
	return !isZero(f);
}
