#pragma once

#include "../common.hpp"
#include "../renderManager.hpp"

class Fire : public Renderable
{

public:

    bool init();

	void draw(const mat3& projection)override;
	
    void destroy();

    void originUpdate(float ox, float oy, float p_x, float p_y);
    void set_player_dead();
    void reset_fire();
    void set_success();
    void set_light_mode(bool isLightOn);
    void set_level_complete_time() ;
    void reset_player_win_time();
    void set_player_dead_time();
    void reset_player_dead_time();
    void set_rotation_end_time();
    float get_time_since_rotation_end()const;
    float get_time_since_level_complete() const;
    float get_time_since_death() const;
    void reset_rotation_end_time();


private:
    float tx = 0.f;
    float ty = 0.f;
    float px = 0.f;
    float py = 0.f;
    float isAlive = 1.f;
    bool isAtDoor = false;
    int isLightMode = 1;
    float m_win_time;
    float m_dead_time;
    float m_rotation_end_time;

};