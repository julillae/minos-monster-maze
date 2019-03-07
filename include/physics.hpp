#pragma once

#include "../include/characters/enemy.hpp"
#include "../include/characters/player.hpp"
#include "../include/mazeComponents/floor.hpp"
#include "../include/mazeComponents/exit.hpp"

#include <vector>
#include <memory>

class Physics
{

public:

    Physics();

    ~Physics();

	float rotation = 0.f;	// world rotation in radians
	vec2 gravityAcc = {0.f,  9.81f * 0.07f };

    struct CollisionNode {
        bool isCollided;
        float angleOfCollision;
    };

    CollisionNode collisionWithFixedWalls(Player *p, const Floor *m);

    CollisionNode collideWithEnemy(Player *p, std::unique_ptr<Enemy> const &t);

    CollisionNode collideWithExit (Player *p, const Exit *e);

	//Note: eventually, we will want to make Player into the more generic Character class
	//      so that we can use the logic for Smart Enemies as well.
	//      Also will eventually want to make Floor into the more generic FixedComponent
	//      So that this will work when we have multiple types of platforms
	void characterCollisionsWithFixedComponents(Player *c, std::vector<Floor> fixedComponents);

	void characterVelocityUpdate(Character *c);

	void characterAccelerationUpdate(Character *c);

	void updateWorldRotation(float currentRotation);

	void updateCharacterVelocityRotation(Character * c, float vecRotation);

	vec2 calculateDimensionsAfterRotation(vec2 c1, vec2 bound);

	bool isZero(float f);
  
	bool notZero(float f);
};