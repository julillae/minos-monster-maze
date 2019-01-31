#pragma once

#include "../include/characters/enemy.hpp"
#include "../include/characters/player.hpp"
#include "../include/mazeComponents/fixedComponent.hpp"
#include "../include/mazeComponents/freeComponent.hpp"
#include "../include/characters/turtle.hpp"

class Physics
{

public:

    Physics();

    ~Physics();

    struct CollisionNode {
        bool isCollided;
        float angleOfCollision;
    };

    CollisionNode collisionWithFixedWalls(Player *p, const FixedComponent *m);

    CollisionNode collideWithEnemy(Player *p, const Turtle *t);

    float lineIntersection(float x_pos1, float x_pos2, float y_pos1, float y_pos2);

    float boundingBox(float bb_x, float bb_y, float scale_x, float scale_y);


};