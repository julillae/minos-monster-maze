#pragma once

#include "../common.hpp"
#include "../renderManager.hpp"
#include "../menus/menu.hpp"

class HelpMenu : public Menu
{
    static Texture texture;
public:

    bool init(vec2 position);

    void draw(const mat3& projection)override;

    void set_visibility(bool show);

private:
    bool is_hidden;
};