#pragma once

#include "../include/characters/enemy.hpp"
#include "../include/characters/player.hpp"
#include "../include/mazeComponents/mazeComponent.hpp"
#include "../include/characters/salmon.hpp"
#include "../include/characters/fish.hpp"
#include "../include/characters/turtle.hpp"

class Physics
{
public:
    Physics();

    ~Physics();

    bool collisionWithWalls(Player *p, MazeComponent *m);

    bool collisionWithEnemy(Player *p, const Turtle *t);

    bool collisionWithFish(Salmon *s, Fish *f);

    bool collisionWithTurtle(Salmon *s, const Turtle *t);
};