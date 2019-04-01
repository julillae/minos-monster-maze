#pragma once

#include "../common.hpp"
#include "../renderManager.hpp"
#include "../menus/menu.hpp"

class SavePopup : public Menu
{
    static Texture texture;
public:

    bool init(vec2 position);

    void draw(const mat3& projection)override;

    void set_visibility(bool show);

    void set_popup_time();
    float get_time_since_popup();

private:
    bool is_hidden;
    float m_popup_time;
};