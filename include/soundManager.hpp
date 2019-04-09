#pragma once

#include "common.hpp"
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

class SoundManager
{

public:

	bool init();

	void play_level_bg_music();

	void play_menu_bg_music();

	void play_dead_sound();

	void play_jump_sound();

	void play_level_complete_sound();

	void destroy();

private:
	Mix_Music* m_background_music;
	Mix_Music* menu_background_music;
	Mix_Chunk* m_player_dead_sound;
	Mix_Chunk* m_player_jump_sound;
	Mix_Chunk* level_complete_sound;
};