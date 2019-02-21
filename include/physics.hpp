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
	float gravityAcc = 9.81f * 0.12f;

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
	void characterCollisionsWithFixedComponents(Player *c, std::vector<Floor> fixedComponents);

	void characterVelocityUpdate(Player *c);

	void characterAccelerationUpdate(Player *c);

    float lineIntersection(float x_pos1, float x_pos2, float y_pos1, float y_pos2);

    float boundingBox(float bb_x, float bb_y, float scale_x, float scale_y);

	bool isZero(float f);

	bool notZero(float f);
};