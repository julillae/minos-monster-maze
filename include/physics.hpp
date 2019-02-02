#pragma once

#include "../include/characters/enemy.hpp"
#include "../include/characters/player.hpp"
#include "../include/mazeComponents/floor.hpp"
#include "../include/characters/turtle.hpp"
#include "../include/mazeComponents/exit.hpp"

class Physics
{

public:

    Physics();

    ~Physics();

    struct CollisionNode {
        bool isCollided;
        float angleOfCollision;
    };

    CollisionNode collisionWithFixedWalls(Player *p, const Floor *m);

    CollisionNode collideWithEnemy(Player *p, const Enemy *t);

    CollisionNode collideWithExit (Player *p, const Exit *e);

    float lineIntersection(float x_pos1, float x_pos2, float y_pos1, float y_pos2);

    float boundingBox(float bb_x, float bb_y, float scale_x, float scale_y);


};