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

    CollisionNode collisionWithFixedWalls(Player *p, const Floor *m);

    CollisionNode collideWithEnemy(Player *p, const Enemy *t);

    CollisionNode collideWithExit (Player *p, const Exit *e);

	//Note: eventually, we will want to make Player into the more generic Character class
	//      so that we can use the logic for Smart Enemies as well.
	//      Also will eventually want to make Floor into the more generic FixedComponent
	//      So that this will work when we have multiple types of platforms
	void characterCollisionsWithFixedComponents(Player *p, std::vector<Floor> fixedComponents);

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