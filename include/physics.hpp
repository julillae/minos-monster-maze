#pragma once

#include "../include/characters/enemy.hpp"
#include "../include/characters/player.hpp"
#include "../include/mazeComponents/floor.hpp"
#include "../include/mazeComponents/exit.hpp"

#include <vector>

class Physics
{

public:

    Physics();

    ~Physics();

	float rotation = 0.f;	// world rotation in radians
	vec2 gravityAcc = {0.f,  9.81f * 0.07f };

    std::pair<vec2, float> MTV;
    // MTV.first = direction
    // MTV.second = magnitude

    struct Projection
    {
        Projection(float mi, float ma)
        {
            min = mi;
            max = ma;
        }
        float min;
        float max;

        bool overlap(Projection p)
        {
            bool isSeparated = (p.max < min || max < p.min);
            return !isSeparated;
        }
    };



    struct CollisionNode {
        bool isCollided;
        float angleOfCollision;
    };

    CollisionNode fastCollisionWithFixedComponents(Player *p, std::unique_ptr<FixedComponent> const &f);

    CollisionNode collideWithEnemy(Player *p, const Enemy *t);

    CollisionNode collideWithExit (Player *p, const Exit *e);

	// produces true if player collides with a fixed component that kills the player
    bool characterCollisionsWithFixedComponents(Player *c, const std::vector<std::unique_ptr<FixedComponent>> &fixedComponents);

	void characterVelocityUpdate(Character *c);

	void characterAccelerationUpdate(Character *c);

	void updateWorldRotation(float currentRotation);

	void updateCharacterVelocityRotation(Character * c, float vecRotation);

	bool isZero(float f);
  
	bool notZero(float f);

    std::vector<vec2> getVertices(vec2 object, vec2 bounds, float rotation)const;

    std::vector<vec2> getAxes(std::vector<vec2> vertices)const;

    Projection getProjection(vec2 axis, std::vector<vec2> vertices)const;

    bool collisionWithGeometry(const std::vector<vec2> &vertArr1, const std::vector<vec2> &vertArr2, vec2 pos1, vec2 pos2);

    float getOverlap(Projection p1, Projection p2);


};