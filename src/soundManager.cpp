#include "..\include\soundManager.hpp"

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
	
	//UI Confirmation Alert, B4.wav by InspectorJ

	//Click_Soft_01.wav
	//Menu_Navigate_03.wav
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


	if (m_background_music == nullptr)
	{
		fprintf(stderr, "Failed to load sound\n %s,%s\n make sure the data directory is present",
			audio_path("secret_catacombs.wav"));
		return false;
	}

	Mix_VolumeMusic(20);
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

	Mix_CloseAudio();
}
