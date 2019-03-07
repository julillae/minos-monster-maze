#include "../include/physics.hpp"
#include <algorithm>
#include "../include/common.hpp"
#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
#include <cfloat>

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

vec2 calculateNewPosition(vec2 c1, vec2 c2) {
    vec2 colNormal = normalize(add(c2, negateVec(c1)));
    vec2 newPos = add(c1, negateVec(colNormal));
    return newPos;
}

// Grab the vertices
std::vector <vec2> Physics::getVertices(vec2 object, vec2 bounds, float rotation)const
{

    std::vector<vec2> verticesArr;
    auto offset = static_cast<float>(sqrt(pow(bounds.x/2, 2) + pow(bounds.y/2, 2)));
    float offsetAngle = atan2(bounds.y, bounds.x);

    float x_pos = object.x;
    float y_pos = object.y;

    vec2 vert1 = {x_pos + offset * cosf(rotation + offsetAngle), y_pos + offset * sinf(rotation + offsetAngle)};
    vec2 vert2 = {static_cast<float>(x_pos + offset * cos(rotation + M_PI - offsetAngle)),
                  static_cast<float>(y_pos + offset * sin(rotation + M_PI - offsetAngle))};
    vec2 vert3 = {static_cast<float>(x_pos + offset * cos(rotation - M_PI + offsetAngle)),
                  static_cast<float>(y_pos + offset * sin(rotation - M_PI + offsetAngle))};
    vec2 vert4 = {x_pos + offset * cos(rotation - offsetAngle), y_pos + offset * sin(rotation - offsetAngle)};

    verticesArr.push_back(vert1);
    verticesArr.push_back(vert2);
    verticesArr.push_back(vert3);
    verticesArr.push_back(vert4);

    return verticesArr;

}

std::vector<vec2> Physics::getAxes(std::vector<vec2> vertices)const
{

    std::vector<vec2> axisVector;

    for (int i = 0; i < vertices.size(); i++)
    {
        vec2 v1 = vertices[i];
        vec2 v2 = vertices[i + 1 == vertices.size() ? 0 : i + 1];
        vec2 edge = subtract(v1, v2); // Get vector representing edge of shape
        vec2 normal = {edge.y, -edge.x}; // Get the normal (a vector perpendicular to the edge)

        // Normalize the normal vector
        auto length = static_cast<float>(sqrt(pow(normal.x, 2) + pow(normal.y, 2)));
        normal.x /= length;
        normal.y /= length;

        axisVector.push_back(normal);
    }
    return axisVector;

}

Physics::Projection Physics::getProjection(vec2 axis, std::vector<vec2> vertices) const
{
    float min = dot(axis, vertices[0]);
    float max = min;

    for (int i = 1; i < vertices.size(); i++) {
        float p = dot(axis, vertices[i]);
        if (p < min) {
            min = p;
        } else if (p > max) {
            max = p;
        }
    }

    Projection proj = Projection(min, max);
    return proj;
}

float Physics::getOverlap(Projection p1, Projection p2)
{
    if (p1.overlap(p2)) {
        return std::min(p1.max, p2.max) - std::max(p1.min, p2.min);
    }
    return 0;
}

// Separating Axis Theorem
/**
 * Compares set of vertices and checks if projections overlap. If they overlap, sets the minimum translation vector
 * with the direction and magnitude required to push object out of collision
 * @param vertArr1
 * @param vertArr2
 * @param pos1
 * @param pos2
 * @return true if collided, false if not
 */
bool Physics::collisionWithGeometry(const std::vector<vec2> &vertArr1, const std::vector<vec2> &vertArr2, vec2 pos1, vec2 pos2)
{

    float overlap = FLT_MAX;
    vec2 smallest = {0.f, 0.f};

    // retrieve axes
    std::vector<vec2> axis1 = getAxes(vertArr1);

    // All the vertices of colliding shape; Represents the shape in projections
    std::vector<vec2> axis2 = getAxes(vertArr2);

   for (auto axis : axis1) {
       // grab the projection of axis
       Projection p1 = getProjection(axis, vertArr1);
       Projection p2 = getProjection(axis, vertArr2);

       if (!p1.overlap(p2))
       {
           return false;
       } else {
           float o = getOverlap(p1, p2);
           if (o < overlap) {
               overlap = o;
               smallest = axis;
           }
       }
   }

   for (auto axis : axis2) {
       Projection p1 = getProjection(axis, vertArr1);
       Projection p2 = getProjection(axis, vertArr2);

       if (!p1.overlap(p2))
       {
           return false;
       } else {
           float o = getOverlap(p1, p2);
           if (o < overlap) {
               overlap = o;
               smallest = axis;
           }
       }
   }

   vec2 between = subtract(pos2, pos1);
   if (dot(between, smallest) < 0) {
       smallest = negateVec(smallest);
   }

   MTV.first = smallest;
   MTV.second = overlap;

   return true;
}

Physics::CollisionNode Physics::collideWithEnemy (Player *p, const Enemy *e) {

//    float other_r = std::max(p->get_bounding_box().x, e->get_bounding_box().y);
//    float my_r = std::max(p->width, p->height);
//    bool isCollided = circleToCircleIntersection(p->get_position(), e->get_position(), other_r, my_r);

    vec2 playPos = p->get_position();
    vec2 ePos = e->get_position();
    std::vector<vec2> playVert = getVertices(playPos, p->get_bounding_box(), rotation);
    std::vector<vec2> enemyVert = getVertices(ePos, e->get_bounding_box(), 0);

    bool isCollided = collisionWithGeometry(playVert, enemyVert, playPos, ePos);

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

        vec2 cPos = c->get_position();
        vec2 cBound = c->get_bounding_box();
        vec2 fPos = floor.get_position();
        vec2 fBound = floor.get_bounding_box();

        std::vector<vec2> playVert = getVertices(cPos, cBound, rotation);
        // unrotated floor
        std::vector<vec2> floorVert = getVertices(fPos, fBound, 0);
//        bool isCollided = collisionWithGeometry(playVert, floorVert, cPos, fPos);
        bool isCollided = collisionNode.isCollided;

        if (isCollided) {

            // test
            vec2 newPos = subtract(cPos, MTV.first);

            float dy = cPos.y - fPos.y;
            float dx = fPos.x - cPos.x;
            float collisionAngle = atan2(dy, dx);

            // set on platform

//            cPos = subtract(cPos, MTV.first);
//            if (rotation > 0) {
//                MTV.first = rotateVec(MTV.first, rotation);
//            }
//
//            if (MTV.first.y < 0) {
//                isBelowAtLeastOnePlatform = true;
//            } else if (MTV.first.y > 0) {
//                c->set_on_platform();
//                isOnAtLeastOnePlatform = true;
//            } else if (MTV.first.x > 0) {
//                isLeftOfAtLeastOnePlatform = true;
//            } else if (MTV.first.x < 0) {
//                isRightOfAtLeastOnePlatform = true;
//            }


//            float collisionAngle = collisionNode.angleOfCollision;
            // logic needed to get new angle (collisionAngle + rotation) within
            // the needed -pi to pi range
            collisionAngle = static_cast<float>(fmod(collisionAngle + rotation, 2 * M_PI));
            float anglePastPi = 0.f;
            if (collisionAngle > M_PI) {
                anglePastPi = static_cast<float>(collisionAngle - M_PI);
                collisionAngle = static_cast<float>(-M_PI + anglePastPi);
            }
            else if (collisionAngle < -M_PI) {
                anglePastPi = static_cast<float>(collisionAngle + M_PI);
                collisionAngle = static_cast<float>(M_PI + anglePastPi);
            }

            if (collisionAngle > -3 * M_PI / 4 && collisionAngle < -M_PI / 4) {
                c->set_on_platform();
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
//
            float floor_tolerance = 40;

//            vec2 newPos = calculateNewPosition(c->get_position(), floor.get_position());

//			if (fPos.y - newPos.y > 39) {
//                c->set_position(newPos);
//            }
        }
    }


    if (!isOnAtLeastOnePlatform) c->set_in_free_fall();
    c->isLeftOfPlatform  = isLeftOfAtLeastOnePlatform;
    c->isBelowPlatform = isBelowAtLeastOnePlatform;
    c->isRightOfPlatform = isRightOfAtLeastOnePlatform;


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
