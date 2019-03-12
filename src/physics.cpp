#include "../include/physics.hpp"
#include <algorithm>
#include "../include/common.hpp"
#define _USE_MATH_DEFINES
#include <math.h>
#include <cfloat>

// logic for gravity and potentially friction calculations go here

Physics::Physics() = default;

Physics::~Physics() = default;

bool circleToCircleIntersection(vec2 c1, vec2 c2, float r1, float r2)
{
    float xDistance = c1.x - c2.x;
    float yDistance = c1.y - c2.y;
    float radius = std::max(r1, r2);
    radius *= 1.0f;
    return xDistance * xDistance + yDistance * yDistance <= radius * radius;
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

// Grab the vertices
std::vector <vec2> Physics::getVertices(vec2 object, vec2 bounds, float rotation)const
{

    std::vector<vec2> verticesArr;
    auto offset = static_cast<float>(sqrt(pow(bounds.x/2, 2) + pow(bounds.y/2, 2)));
    float offsetAngle = atan2(bounds.y, bounds.x);

    float x_pos = object.x;
    float y_pos = object.y;

    vec2 vert1 = {x_pos + offset * cosf(rotation + offsetAngle), y_pos + offset * sinf(rotation + offsetAngle)};
    vec2 vert2 = {(x_pos + offset * cosf(static_cast<float>(rotation + M_PI - offsetAngle))),
                  (y_pos + offset * sinf(static_cast<float>(rotation + M_PI - offsetAngle)))};
    vec2 vert3 = {(x_pos + offset * cosf(static_cast<float>(rotation - M_PI + offsetAngle))),
                  static_cast<float>(y_pos + offset * sin(rotation - M_PI + offsetAngle))};
    vec2 vert4 = {static_cast<float>(x_pos + offset * cos(rotation - offsetAngle)), static_cast<float>(y_pos + offset * sin(rotation - offsetAngle))};


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

        axisVector.push_back(normalize(normal));
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

std::vector<vec2> Physics::getPlayerVertices(Player *p)const
{
    vec2 playPos = p->get_position();
    vec2 playBound = p->get_bounding_box();

    std::vector<vec2> playArray;

    vec2 topLeft = {(playPos.x - playBound.x / 2), (playPos.y + playBound.y / 2)};
    vec2 topRight = {(playPos.x + playBound.x / 2), (playPos.y + playBound.y / 2)};
    vec2 bottomRight = {(playPos.x + playBound.x / 2), (playPos.y - playBound.y / 2)};
    vec2 bottomLeft = {(playPos.x - playBound.x / 2), (playPos.y - playBound.y / 2)};

    playArray.push_back(topLeft);
    playArray.push_back(topRight);
    playArray.push_back(bottomRight);
    playArray.push_back(bottomLeft);

    for (int i = 0; i < 4; i++) {
        playArray[i] = {((playArray[i].x - playPos.x) * cosf(-rotation)) - ((playArray[i].y - playPos.y) * sinf(-rotation)) + playPos.x,
                        ((playArray[i].y - playPos.y) * cosf(-rotation)) + ((playArray[i].x - playPos.x) * sinf(-rotation)) + playPos.y};
    }

    return playArray;

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
Physics::MTV Physics::collisionWithGeometry(const std::vector<vec2> &vertArr1, const std::vector<vec2> &vertArr2, vec2 pos1, vec2 pos2)
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

Physics::CollisionNode Physics::collideWithEnemy (Player *p, std::unique_ptr<Enemy> const &e) {

    float other_r = std::max(p->get_bounding_box().x, e->get_bounding_box().y);
    float my_r = std::max(p->width, p->height);
    bool isCollided = circleToCircleIntersection(p->get_position(), e->get_position(), other_r, my_r);

    vec2 playPos = p->get_position();
    vec2 ePos = e->get_position();
    std::vector<vec2> playVert = getVertices(playPos, p->get_bounding_box(), rotation);
    std::vector<vec2> enemyVert = getVertices(ePos, e->get_bounding_box(), 0);

    Physics::CollisionNode collisionNode{};
    collisionNode.isCollided = isCollided;
    collisionNode.angleOfCollision = 0;
    return collisionNode;
}

Physics::CollisionNode Physics::fastCollisionWithFixedComponents(Player *p, std::unique_ptr<FixedComponent> const &f) {
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


bool Physics::characterCollisionsWithFixedComponents(Player* c, const std::vector<std::unique_ptr<FixedComponent>> &fixedComponents) {

    bool isOnAtLeastOnePlatform = false;
    bool isBelowAtLeastOnePlatform = false;

    for (const auto& fc : fixedComponents) {

        vec2 cPos = c->get_position();
        vec2 fPos = fc->get_position();
        vec2 cBound = c->get_bounding_box();
        vec2 fBound = fc->get_bounding_box();

        if (fastCollisionWithFixedComponents(c, fc).isCollided) {
//            std::vector<vec2> playArray = getVertices(cPos, cBound, rotation);
            std::vector<vec2> playArray = getPlayerVertices(c);
            std::vector<vec2> fixedComponentArray;


            if (fc->can_kill) {
                fixedComponentArray = fc->get_vertex_coord();
                if (collisionWithGeometry(playArray, fixedComponentArray, cPos, fPos).isCollided)
                    return true;
            }
            else {

                fixedComponentArray = getVertices(fPos, fBound, 0);

                MTV mtv = collisionWithGeometry(playArray, fixedComponentArray, cPos, fPos);

                if (mtv.isCollided) {

                    vec2 normal = mtv.normal;
                    float magnitude = mtv.magnitude;

                    // grab the vector that pushes the player to the tangent of the platform
                    vec2 translation = { normal.x * magnitude, normal.y * magnitude};

                    vec2 currentPos = c->get_position();
                    // translate the player
                    vec2 newPos = subtract(currentPos, translation);

                    c->set_position(newPos);
					// add MTV to list of collision normals stored in Player
					c->collisionNormals.push_back(mtv.normal);

                    float dy = newPos.y - fPos.y;
                    float dx = fPos.x - newPos.x;
                    float collisionAngle = atan2(dy, dx);
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

                    // place player on platform
                    if (collisionAngle > -3 * M_PI / 4 && collisionAngle < -M_PI / 4) {
                        c->set_on_platform();
                        isOnAtLeastOnePlatform = true;
                        c->m_platform_drag = fc->get_drag();
                    }
                    if (collisionAngle > M_PI / 4 && collisionAngle < 3 * M_PI / 4) {
                        isBelowAtLeastOnePlatform = true;
                    }
                }
            }
        }
    }

    if (!isOnAtLeastOnePlatform) c->set_in_free_fall();
    c->isBelowPlatform = isBelowAtLeastOnePlatform;

    return false;
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
