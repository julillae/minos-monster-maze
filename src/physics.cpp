#include "../include/physics.hpp"
#include <algorithm>
#include "../include/common.hpp"
#define _USE_MATH_DEFINES
#include <math.h>
#include <cfloat>
#include <map>

// logic for gravity and potentially friction calculations go here

Physics::Physics() = default;

Physics::~Physics() = default;

bool outerCircleToCircleIntersection(vec2 c1, vec2 c2, float r1, float r2)
{
	float roundingTolerance = 1.1f;	// some float > 1
	float xDistance = c1.x - c2.x;
	float yDistance = c1.y - c2.y;
	float radius = r1 + r2;
	return xDistance * xDistance + yDistance * yDistance <= radius * radius * roundingTolerance;
}

/**
 * Calculates the Minkowski Sum between two rectangles and returns true if rectangles are colliding
 * @param r1
 * @param r2
 * @param bb1
 * @param bb2
 * @return true if colliding, false if not
 */
bool rectToRectIntersection(vec2 r1, vec2 r2, vec2 bb1, vec2 bb2) {
    float w = 0.5f * (bb1.x + bb2.x);
    float h = 0.5f * (bb1.y + bb2.y);
    float dx = r1.x - r2.x;
    float dy = r1.y - r2.y;

    return fabs(dx) <= w && fabs(dy) <= h;
}

std::vector<vec2> Physics::getAxes(std::vector<vec2> vertices)const
{

    std::vector<vec2> axisVector;

    for (size_t i = 0; i < vertices.size(); i++)
    {
        vec2 v1 = vertices[i];
        vec2 v2 = vertices[i + 1 == vertices.size() ? 0 : i + 1];
        vec2 edge = subtract(v1, v2); // Get vector representing edge of shape
        vec2 normal = {edge.y, -edge.x}; // Get the normal (a vector perpendicular to the edge)

        axisVector.push_back(normalize(normal));
    }
    return axisVector;

}

Physics::Projection Physics::getProjection(vec2 axis, std::vector<vec2> vertices) const
{
    float min = dot(axis, vertices[0]);
    float max = min;

    for (size_t i = 1; i < vertices.size(); i++) {
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
MTV Physics::collisionWithGeometry(const std::vector<vec2> &vertArr1, const std::vector<vec2> &vertArr2, vec2 pos1, vec2 pos2)
{

    MTV mtv = MTV{};

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
           mtv.normal = {0.f, 0.f};
           mtv.magnitude = 0.f;
           mtv.isCollided = false;
           return mtv;
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
           mtv.normal = {0.f, 0.f};
           mtv.magnitude = 0.f;
           mtv.isCollided = false;
           return mtv;
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

   mtv.normal = smallest;
   mtv.magnitude = overlap;
   mtv.isCollided = true;

   return mtv;
}

bool Physics::collideWithEnemy (Player *p, Enemy *e) {
	bool isCollided = false;
	vec2 pPos = p->get_position();
	vec2 ePos = e->get_position();
	float pRadius = p->boundingCircleRadius;
	float eRadius = e->boundingCircleRadius;
	bool broadBasedCollisionCheck = outerCircleToCircleIntersection(pPos, ePos, pRadius, eRadius);

	if (broadBasedCollisionCheck) {
		e->set_world_vertex_coord();
		std::vector<vec2> enemyVertexArray = e->get_vertex_coord();
		std::vector<vec2> playerVertexArray = p->get_vertex_coord();

		isCollided = collisionWithGeometry(playerVertexArray, enemyVertexArray, pPos, ePos).isCollided;
	}
    return isCollided;
}

bool Physics::collideWithExit (Player *p, Exit *e) {
	bool isCollided = false;
	vec2 pPos = p->get_position();
	vec2 ePos = e->get_position();
	float pRadius = p->boundingCircleRadius;
	float eRadius = e->boundingCircleRadius;

	bool broadBasedCollisionCheck = outerCircleToCircleIntersection(pPos, ePos, pRadius, eRadius);
	if (broadBasedCollisionCheck) {
		std::vector<vec2> playerVertexArray = p->get_vertex_coord();
		std::vector<vec2> exitVertexArray = e->get_vertex_coord();
		isCollided = collisionWithGeometry(playerVertexArray, exitVertexArray, pPos, ePos).isCollided;
	}

    return isCollided;
}

void Physics::characterCollisionsWithFloors(Player* c, vector<Floor> floors) {
	vec2 cPos = c->get_position();
	float cRadius = c->boundingCircleRadius;
	float fRadius = 0.f;
	if (!floors.empty()) fRadius = floors.front().boundingCircleRadius;
    for (Floor f: floors) {
		if (outerCircleToCircleIntersection(cPos, f.get_position(), cRadius, fRadius))
			characterCollisionsWithFixedComponent(c, &f);
    }
}

void Physics::characterCollisionsWithSpikes(Player* c, std::vector<Spike> spikes) {
	vec2 cPos = c->get_position();
	float cRadius = c->boundingCircleRadius;
    for (Spike s : spikes) {
		float sRadius = s.boundingCircleRadius;
		if (outerCircleToCircleIntersection(cPos, s.get_position(), cRadius, sRadius))
			characterCollisionsWithFixedComponent(c, &s);

        if (!c->is_alive()) return;
    }
}

void Physics::characterCollisionsWithIce(Player* c, vector<Ice> ice) {
	vec2 cPos = c->get_position();
	float cRadius = c->boundingCircleRadius;
	float iRadius = 0.f;
	if (!ice.empty()) iRadius = ice.front().boundingCircleRadius;
	for (Ice i : ice) {
		if (outerCircleToCircleIntersection(cPos, i.get_position(), cRadius, iRadius))
	        characterCollisionsWithFixedComponent(c, &i);
    }
}

void Physics::characterCollisionsWithBlades(Player* c, std::vector<Blade> blades) {
	vec2 cPos = c->get_position();
	float cRadius = c->boundingCircleRadius;
	for (Blade b : blades) {
		float bRadius = b.boundingCircleRadius;
		if (outerCircleToCircleIntersection(cPos, b.get_position(), cRadius, bRadius)) {
			characterCollisionsWithFixedComponent(c, &b);

			if (!c->is_alive()) return;
		}
	}
}

void mtvAdjustment(Character* c, MTV mtv) {
	vec2 normal = mtv.normal;
	float magnitude = mtv.magnitude;

	// grab the vector that pushes the player to the tangent of the platform
	vec2 translation = { normal.x * magnitude, normal.y * magnitude };

	vec2 currentPos = c->get_position();
	// translate the player
	vec2 newPos = subtract(currentPos, translation);

	c->set_position(newPos);
}

vector<MTV> mtvAggregation(vector<MTV> mtvs, Character* c) {
	std::map<pair<float, float>, MTV> mtvMap;
	vector<MTV> normals;
	for (MTV mtv : mtvs) {
		mtvMap.emplace(vec2ToPair(mtv.normal), mtv);
	}
	for (std::pair<pair<float, float>, MTV> element : mtvMap) {
		mtvAdjustment(c, element.second);
		normals.push_back(element.second);
	}
	return normals;
}

vec2 adjustVelocity(vec2 velocity, vector<MTV> mtvs) {
	float velocityX = velocity.x;
	float velocityY = velocity.y;

	for (MTV mtv : mtvs) {
		if (fabs(velocityX - mtv.normal.x) < fabs(velocityX)) {
			velocityX *= (1 - fabs(mtv.normal.x));
		}
		if (fabs(velocityY - mtv.normal.y) < fabs(velocityY)) {
			velocityY *= (1 - fabs(mtv.normal.y));
		}
	}
	return vec2({ velocityX, velocityY });
}

void Physics::characterCollisionsWithFixedComponent(Player* c, FixedComponent* fc) {
    vec2 cPos = c->get_position();
    vec2 fPos = fc->get_position();
	std::vector<vec2> playArray = c->get_vertex_coord();
	std::vector<vec2> fixedComponentArray = fc->get_vertex_coord();

	MTV mtv = collisionWithGeometry(playArray, fixedComponentArray, cPos, fPos);

	if (mtv.isCollided) {
		if (fc->can_kill) {
			c->kill();
			return;
		}
		c->collisionMTVs.push_back(mtv);

		float dy = -mtv.normal.y;
		float dx = mtv.normal.x;
		vec2 collisionVector = vec2({ dx, dy });
		vec2 rotatedCollisionVector = rotateVec(collisionVector, rotation);
		float collisionAngle = atan2(rotatedCollisionVector.y, rotatedCollisionVector.x);

		// logic needed to get new angle (collisionAngle + rotation) within
		// the needed -pi to pi range
		float anglePastPi = 0.f;
		if (collisionAngle > M_PI) {
			anglePastPi = static_cast<float>(collisionAngle - M_PI);
			collisionAngle = static_cast<float>(-M_PI + anglePastPi);
		}
		else if (collisionAngle < -M_PI) {
			anglePastPi = static_cast<float>(collisionAngle + M_PI);
			collisionAngle = static_cast<float>(M_PI + anglePastPi);
		}

		// place player on platform
		if (collisionAngle > -7 * M_PI / 8 && collisionAngle < -M_PI / 8) {
			c->set_on_platform();
			isOnAtLeastOnePlatform = true;
			c->m_platform_drag = fc->get_drag();
		}
    }
}

void Physics::characterVelocityUpdate(Character* c)
{
	vector<MTV> uniqueMTVs = mtvAggregation(c->collisionMTVs, c);
	c->collisionMTVs.clear();
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
			c->isOnPlatform = false;
		}

		if (c->isOnPlatform) {
			if (isZero(cAcc.x)) {
				c->characterState->changeState(idle);
				cVelocity.x *= platformDrag;
				cVelocity.y = std::min(0.f, cVelocity.y);
			}
			else {
				c->characterState->changeState(running);
			}
		}
		else {
			if (cVelocity.y < 0)
				c->characterState->changeState(rising);
			else
				c->characterState->changeState(falling);
		}

		// rotate back to current rotation
		cVelocity = rotateVec(cVelocity, rotation);
		cVelocity = adjustVelocity(cVelocity, uniqueMTVs);
		c->set_velocity(cVelocity);
	}
}

void Physics::characterAccelerationUpdate(Character * c) {
	vec2 horzAcc = {0.f, 0.f};
    Direction h_direction = c->get_h_direction();
	float platformDrag = c->m_platform_drag;
    float accStep = c->accStep;

	if (platformDrag > 0.9f) accStep *= 0.5f;

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
