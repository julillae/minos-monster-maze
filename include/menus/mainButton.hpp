#pragma once

#include "../common.hpp"
#include "../renderManager.hpp"
#include "menu.hpp"
#include "button.hpp"

class MainButton : public Button
{

public:

    bool init(vec2 position, const char* textureFile, button name);

    void draw(const mat3& projection)override;

    button buttonName;

};