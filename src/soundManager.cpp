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

	soundList[playerDead] = Mix_LoadWAV(audio_path("death.wav"));
	soundList[playerJump] = Mix_LoadWAV(audio_path("jump.wav"));
	soundList[levelComplete] = Mix_LoadWAV(audio_path("nextLevel.wav"));
	soundList[buttonSelect] = Mix_LoadWAV(audio_path("button_select.wav"));
	soundList[buttonEnter] = Mix_LoadWAV(audio_path("button_enter.wav"));
	soundList[levelSelect] = Mix_LoadWAV(audio_path("level_select.wav"));
	soundList[minotaurIdle] = Mix_LoadWAV(audio_path("pig_grunt.wav"));
	soundList[minotaurAttack] = Mix_LoadWAV(audio_path("swing_sound.wav"));
	soundList[minotaurPrepare] = Mix_LoadWAV(audio_path("pre_rotate_growl.wav"));
	soundList[rotationLoop] = Mix_LoadWAV(audio_path("rotation_loop.wav"));

	m_background_music = Mix_LoadMUS(audio_path("secret_catacombs.wav"));
	menu_background_music = Mix_LoadMUS(audio_path("menu_music.wav"));


	if (m_background_music == nullptr)
	{
		fprintf(stderr, "Failed to load sound\n %s\n make sure the data directory is present",
			audio_path("secret_catacombs.wav"));
		return false;
	}

	Mix_VolumeMusic(20);
	Mix_VolumeChunk(soundList[rotationLoop], 40);
	Mix_VolumeChunk(soundList[minotaurIdle], 20);
	Mix_VolumeChunk(soundList[minotaurAttack], 30);
	Mix_VolumeChunk(soundList[minotaurPrepare], 60);
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

void SoundManager::play_sound(Sound sound)
{
	Mix_PlayChannel(-1, soundList[sound], 0);
}

void SoundManager::play_sound_looped(Sound sound, int loop)
{
	soundChannels[sound] = Mix_PlayChannel(-1, soundList[sound], loop);
}

void SoundManager::fade_in_sound_looped(Sound sound, int fadeInTime)
{
	soundChannels[sound] = Mix_FadeInChannelTimed(-1, soundList[sound], -1, fadeInTime, -1);
}

void SoundManager::fade_out_sound(Sound sound, int fadeOutTime)
{
	Mix_FadeOutChannel(soundChannels[sound], fadeOutTime);
}

void SoundManager::destroy()
{
	for (auto& sound : soundList) {
		if (sound != nullptr)
			Mix_FreeChunk(sound);
	}

	if (m_background_music != nullptr)
		Mix_FreeMusic(m_background_music);
	if (menu_background_music != nullptr)
		Mix_FreeMusic(menu_background_music);

	Mix_CloseAudio();
}
