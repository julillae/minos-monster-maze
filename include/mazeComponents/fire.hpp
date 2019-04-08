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
    void originUpdate(float ox, float oy, float p_x, float p_y);
    void set_player_dead();
    void reset_fire();
    void set_success();

private:
    float tx = 0.f;
    float ty = 0.f;
    float px = 0.f;
    float py = 0.f;
    float isAlive = 1.f;
    int is_At_Door = 0;

};