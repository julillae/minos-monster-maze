#pragma once

#include "../common.hpp"
#include "../renderManager.hpp"

class MainMenu : public Renderable
{
    static Texture texture;
public:

    bool init(vec2 position);

    void destroy();

    void draw(const mat3& projection)override;

    void set_position(vec2 position);

private:
    Texture m_texture;
    vec2 m_position;
    vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
    float m_rotation; // in radians

};