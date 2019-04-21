#pragma once

#include "common.hpp"
#include "../include/characters/enemy.hpp"
#include "../include/characters/player.hpp"
#include "../include/mazeComponents/floor.hpp"
#include "../include/mazeComponents/spike.hpp"
#include "../include/mazeComponents/ice.hpp"
#include "../include/mazeComponents/exit.hpp"
#include "../include/mazeComponents/fire.hpp"
#include "../include/mazeComponents/blade.hpp"

#include <vector>
#include <map>
#include <list>

class Physics
{

public:

    Physics();

    ~Physics();

	bool outerCircleToCircleIntersection(vec2 c1, vec2 c2, float r1, float r2);

	float rotation = 0.f;	// world rotation in radians
	vec2 gravityAcc = {0.f,  9.81f * 0.06f };

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

    bool collideWithEnemy(Player *p, Enemy *e);

    bool collideWithExit (Player *p, Exit *e);

    bool isOnAtLeastOnePlatform = false;

	// checks for collisions and reacts appropriately
    void characterCollisionsWithFloors(Player *c, std::vector<Floor> floors);
    void characterCollisionsWithSpikes(Player *c, std::vector<Spike> spikes);
    void characterCollisionsWithIce(Player *c, std::vector<Ice> ice);
    void characterCollisionsWithBlades(Player *c, std::vector<Blade> blades);
	vec2 adjustVelocity(vec2 velocity, vector<MTV> mtvs);
	vec2 adjustVelocity(vec2 velocity, Player * p);
	void characterCollisionsWithFixedComponent(Player *c, FixedComponent* fc);

	void characterVelocityUpdate(Player *c);

	void characterAccelerationUpdate(Character *c);

	void updateWorldRotation(float currentRotation);

	void updateCharacterVelocityRotation(Character * c, float vecRotation);

	bool isZero(float f);
  
	bool notZero(float f);

    std::vector<vec2> getAxes(std::vector<vec2> vertices)const;

    Projection getProjection(vec2 axis, std::vector<vec2> vertices)const;

    MTV collisionWithGeometry(const std::vector<vec2> &vertArr1, const std::vector<vec2> &vertArr2, vec2 pos1, vec2 pos2);

    float getOverlap(Projection p1, Projection p2);

	void checkCornerCollisions(Player * p, vec2 playArray[4], FixedComponent* fC);
};