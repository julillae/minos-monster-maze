#pragma once

#include "common.hpp"
#include "renderManager.hpp"

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>
#include <time.h>

// Renders a prompt that shows for m_flash_duration

class FlashMessage : public Renderable
{
    public:
        bool init(int levelNumber);

        void destroy();

        void draw(const mat3 &projection);
    
        void set_position(vec2 pos);
        void set_visibility(bool is_visible);

        void set_flash_time();
        float get_time_since_flash();
    private:
        Texture m_texture;

        const float m_flash_duration = 4.5;

        float m_flash_time;
        bool is_hidden = false;
};