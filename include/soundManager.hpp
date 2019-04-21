#pragma once

#include "common.hpp"
using namespace std;
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

// Enum for sounds
// set SoundMAX = last item in enum, needed to know how many sounds are available
enum Sound { 
	playerDead, 
	playerJump, 
	levelComplete, 
	buttonSelect, 
	buttonEnter, 
	levelSelect, 
	minotaurIdle, 
	minotaurAttack, 
	minotaurPrepare, 
	rotationLoop,
	SoundMAX = rotationLoop
};

class SoundManager
{

public:

	bool init();

	void play_level_bg_music();

	void play_menu_bg_music();

	void play_sound(Sound sound);

	void play_sound_looped(Sound sound, int loop);

	void fade_in_sound_looped(Sound sound, int fadeInTime);

	void fade_out_sound(Sound sound, int fadeOutTime);

	void destroy();

private:
	int soundChannels[SoundMAX + 1];
	Mix_Chunk* soundList[SoundMAX+1];
	Mix_Music* m_background_music;
	Mix_Music* menu_background_music;
	int rotation_loop_channel = 0;
};