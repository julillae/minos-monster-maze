#pragma once

#include "../include/characters/character.hpp"
#include "../include/characters/enemy.hpp"
#include "../include/mazeComponents/fixedComponent.hpp"
#include "../include/characters/salmon.hpp"
#include "../include/characters/fish.hpp"
#include "../include/characters/turtle.hpp"

class Physics
{
public:
    Physics();

    ~Physics();

    bool collisionWithWalls(Character *c, FixedComponent *f);

    bool collisionWithEnemy(Character *c, Enemy *e);

    bool collisionWithFish(Salmon *s, Fish *f);

    bool collisionWithTurtle(Salmon *s, const Turtle *t);
};