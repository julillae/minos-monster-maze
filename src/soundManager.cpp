#include "../include/soundManager.hpp"

bool SoundManager::init()
{
	//-------------------------------------------------------------------------
	// Loading music and sounds
	if (SDL_Init(SDL_INIT_AUDIO) < 0)
	{
		fprintf(stderr, "Failed to initialize SDL Audio");
		return false;
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
	{
		fprintf(stderr, "Failed to open audio device");
		return false;
	}

	//Note: the following music credits needs to be added to a credit scene at the end of the game
	
	//Charging Loop 1 by Javier Zumer

	//UI Confirmation Alert, B4 by InspectorJ

	//Click_Soft_01
	//Menu_Navigate_03
	//by LittleRobotSoundFactory
	
	//Strange Game Menu
	//Secret Catacombs
	//by Eric Matyas
	//www.soundimage.org

	m_background_music = Mix_LoadMUS(audio_path("secret_catacombs.wav"));
	menu_background_music = Mix_LoadMUS(audio_path("menu_music.wav"));
	m_player_dead_sound = Mix_LoadWAV(audio_path("death.wav"));
	m_player_jump_sound = Mix_LoadWAV(audio_path("jump.wav"));
	level_complete_sound = Mix_LoadWAV(audio_path("nextLevel.wav"));
	button_select_sound = Mix_LoadWAV(audio_path("button_select.wav"));
	button_enter_sound = Mix_LoadWAV(audio_path("button_enter.wav"));
	level_select_sound = Mix_LoadWAV(audio_path("level_select.wav"));
	pre_rotate_sound = Mix_LoadWAV(audio_path("pre_rotate_growl.wav"));
	rotation_loop = Mix_LoadWAV(audio_path("rotation_loop.wav"));


	if (m_background_music == nullptr)
	{
		fprintf(stderr, "Failed to load sound\n %s\n make sure the data directory is present",
			audio_path("secret_catacombs.wav"));
		return false;
	}

	Mix_VolumeMusic(20);
	Mix_VolumeChunk(rotation_loop, 40);
	Mix_VolumeChunk(pre_rotate_sound, 60);
	fprintf(stderr, "Loaded music\n");
	return true;
}

void SoundManager::play_level_bg_music()
{
	Mix_FadeInMusic(m_background_music, -1, 1000);
}

void SoundManager::play_menu_bg_music()
{
	 Mix_FadeInMusic(menu_background_music, -1, 1000);
}

void SoundManager::play_dead_sound()
{
	Mix_PlayChannel(-1, m_player_dead_sound, 0);
}

void SoundManager::play_jump_sound()
{
	Mix_PlayChannel(-1, m_player_jump_sound, 0);
}

void SoundManager::play_level_complete_sound()
{
	Mix_PlayChannel(-1, level_complete_sound, 0);
}

void SoundManager::play_button_select_sound()
{
	Mix_PlayChannel(-1, button_select_sound, 0);
}

void SoundManager::play_button_enter_sound()
{
	Mix_PlayChannel(-1, button_enter_sound, 0);
}

void SoundManager::play_level_select_sound()
{
	Mix_PlayChannel(-1, level_select_sound, 0);
}

void SoundManager::play_pre_rotate_sound()
{
	Mix_PlayChannel(-1, pre_rotate_sound, 0);
}

void SoundManager::play_rotation_loop()
{
	rotation_loop_channel = Mix_FadeInChannelTimed(-1, rotation_loop, -1, 500, -1);
}

void SoundManager::stop_rotation_loop()
{
	Mix_FadeOutChannel(rotation_loop_channel, 200);
}

void SoundManager::destroy()
{
	if (m_background_music != nullptr)
		Mix_FreeMusic(m_background_music);
	if (menu_background_music != nullptr)
		Mix_FreeMusic(menu_background_music);
	if (m_player_dead_sound != nullptr)
		Mix_FreeChunk(m_player_dead_sound);
	if (m_player_jump_sound != nullptr)
		Mix_FreeChunk(m_player_jump_sound);
	if (level_complete_sound != nullptr)
		Mix_FreeChunk(level_complete_sound);
	if (button_select_sound != nullptr)
		Mix_FreeChunk(button_select_sound);
	if (button_enter_sound != nullptr)
		Mix_FreeChunk(button_enter_sound);
	if (level_select_sound != nullptr)
		Mix_FreeChunk(level_select_sound);
	if (pre_rotate_sound != nullptr)
		Mix_FreeChunk(pre_rotate_sound);
	if (rotation_loop != nullptr)
		Mix_FreeChunk(rotation_loop);

	Mix_CloseAudio();
}
