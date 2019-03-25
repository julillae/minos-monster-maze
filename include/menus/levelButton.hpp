#pragma once

#include "../common.hpp"
#include "../renderManager.hpp"
#include "menu.hpp"
#include "button.hpp"



class LevelButton : public Button
{
public:

    bool init(vec2 position, const char* textureFile, int level);

    void draw(const mat3& projection)override;

    int level;

};