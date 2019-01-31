#pragma once

#include "../include/characters/enemy.hpp"
#include "../include/characters/player.hpp"
#include "../include/mazeComponents/fixedComponent.hpp"
#include "../include/mazeComponents/freeComponent.hpp"
#include "../include/characters/salmon.hpp"
#include "../include/characters/fish.hpp"
#include "../include/characters/turtle.hpp"

class Physics
{
    struct CollisionNode {
        bool isCollided;
        float angleOfCollision;
    };

public:
    Physics();

    ~Physics();

    CollisionNode collisionWithFixedWalls(Player *p, FixedComponent *m);

    CollisionNode collideWithEnemy(Player *p, const Turtle *t);

    bool collisionWithFish(Salmon *s, Fish *f);

    float lineIntersection(float x_pos1, float x_pos2, float y_pos1, float y_pos2);

    float boundingBox(float bb_x, float bb_y, float scale_x, float scale_y);


};