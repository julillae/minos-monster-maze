#pragma once

#include "common.hpp"
#include "renderManager.hpp"

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>
#include <time.h>

// Renders a flashing prompt that flashes 10 times
// to get the users attention and then stays on the screen

class FlashMessage : public Renderable
{
    public:
        bool init(int levelNumber);

        void destroy();

        void draw(const mat3 &projection);
        //void flash_message(int image_number);
    
        void set_position(vec2 pos);
        void set_visibility(bool is_visible);

        void set_flash_time();
        float get_time_since_flash();
    private:
        Texture m_texture;

        int numFlashes = 0;
        const int maxFlashes = 10;

        const float m_flash_duration = 0.8;

        float m_flash_time;
        bool is_hidden = false;
};