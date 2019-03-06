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
	vec2 gravityAcc = {0.f,  9.81f * 0.12f };

    struct CollisionNode {
        bool isCollided;
        float angleOfCollision;
    };

    CollisionNode collisionWithFixedWalls(Player *p, std::unique_ptr<FixedComponent> const &f);

    CollisionNode collideWithEnemy(Player *p, const Enemy *t);

    CollisionNode collideWithExit (Player *p, const Exit *e);

	// produces true if player collides with a fixed component that kills the player
    bool characterCollisionsWithFixedComponents(Player *c, const std::vector<std::unique_ptr<FixedComponent>> &fixedComponents);

	void characterVelocityUpdate(Character *c);

	void characterAccelerationUpdate(Character *c);

	void updateWorldRotation(float currentRotation);

	void updateCharacterVelocityRotation(Character * c);

	vec2 calculateDimensionsAfterRotation(vec2 c1, vec2 bound);

	bool isZero(float f);
  
	bool notZero(float f);
};