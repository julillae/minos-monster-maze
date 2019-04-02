#pragma once

#include "../common.hpp"
#include "../renderManager.hpp"

class Fire : public Renderable
{
    //static Texture texture;

public:

    bool init();

    // Renders the component
	void draw(const mat3& projection)override;

    //Sets the new position
	//void set_position(vec2 position);

	// Returns the bounding box for collision detection, called by collides_with()
	vec2 get_bounding_box()const;

    //void set_size();
    void destroy();

    //void WindowTrans(float tx, float ty);

private:
    //Texture m_texture;
    //vec2 m_position;
    //vec2 m_scale; // 1.f in each dimension. 1.f is as big as the associated texture
    //float m_rotation; // in radians
    //float is_hide; // 0.f if show, 1.f if hide
    //float m_width;
	//float m_height;
    //float tx;
    //float ty;
};