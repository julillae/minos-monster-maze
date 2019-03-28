#pragma once

#include "../common.hpp"
#include "../renderManager.hpp"

enum button { CONTINUE, NEWGAME, CONTROLS, QUIT};

class Menu : public Renderable
{
public:

    void destroy();

    void set_position(vec2 position);

};