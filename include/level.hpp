#pragma once

// internal
#include "common.hpp"
#include "characters/player.hpp"
#include "characters/enemy.hpp"
#include "characters/simple.hpp"
#include "characters/spider.hpp"
#include "characters/harpy.hpp"
#include "characters/minotaur.hpp"
#include "mazeComponents/mazeComponent.hpp"
#include "mazeComponents/fixedComponent.hpp"
#include "mazeComponents/floor.hpp"
#include "mazeComponents/exit.hpp"
#include "mazeComponents/ice.hpp"
#include "mazeComponents/spike.hpp"
#include "mazeComponents/blade.hpp"
#include "renderEffects.hpp"
#include "physics.hpp"
#include "menus/helpMenu.hpp"
#include "gameStates/gameState.hpp"
#include "gameStates/pauseMenuState.hpp"
#include "levelLoader.hpp"
#include "flashMessage.hpp"
#include "quadTree.hpp"
#include "timer.hpp"

// stlib
#include <vector>
#include <random>
#include <map>
#include <algorithm>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>
#include <time.h>

// Level class

class Level : public GameState
{
public:
	Player m_player;
	
	Level(Game* game);
	~Level();

	struct Platform {
		bool isPlatform;
		int platformType;
	};

    // Creates a window, sets up events and begins the game
	bool init(vec2 screen, Physics* physicsHandler, int startLevel);

	// Releases all associated resource
    void destroy()override;

	// Steps the game ahead by ms milliseconds
    bool update(float elapsed_ms)override;

    // Renders our scene
	void draw()override;

	// Should the game be over ?
	bool is_over()override;

	Platform maze_is_platform(std::pair<int,int> coords);
	std::vector<std::vector <int>> get_original_maze();

	float get_maze_width();
	float get_maze_height();
	float get_tile_width();
	float get_tile_height();

	void load_select_level(int level);
    int get_current_level();
    float get_rotation();
    float get_rotationDeg();
    float get_rotationEnergy();

    std::vector<Spider> get_spiders();
    std::vector<Harpy> get_harpies();
	Minotaur get_minotaur();
	std::vector<Floor> get_floors();
	std::vector<Blade> get_blades();

    void load_saved_game();
	// Boss controls
	void boss_rotation_set(bool enable, bool ccw);

	bool minotaurPresent = false;

    void reset_game();
	// !!! INPUT CALLBACK FUNCTIONS
	void on_key(GLFWwindow*, int key, int, int action, int mod)override;
	void on_mouse_move(GLFWwindow* window, double xpos, double ypos);

	void check_platform_collisions(std::vector<Floor> nearbyFloorComponents);

	void draw_enemies(mat3 projection_2D);
	void reset_enemies();
	void destroy_enemies();
	void draw_platforms(mat3 projection_2D);
	void destroy_platforms();

	void initialize_message_prompt();
	void initialize_camera_position(int w, int h);
	void call_level_loader();
	void load_new_level();
	void reset_player_camera();
	void freeze_all_enemies();
	void unfreeze_all_enemies();
	void update_all_enemies(float elapsed_ms);
	void update_all_platforms(float elapsed_ms);

	void set_player_death();

	void load_player();
	void load_spiders();
	void load_harpies();
	void load_minotaur();
	void load_blades();

	float get_level_time();
	float get_cumulative_time();
	float get_pause_start();
	void reset_pause_start();
	void return_from_pause();
	void record_pause_time();
private:

	// Water effect
	RenderEffects m_water;

	Minotaur m_minotaur;
	Spiders m_spiders;
	Harpies m_harpies;

	Exit m_exit;
	Floors m_floors;
	Spikes m_spikes;
	Ices m_ice;
	Blades m_blades;

    HelpMenu m_help_menu;

	FlashMessage m_message;

    float m_seed_rng;

    Mix_Music* m_background_music;
	Mix_Chunk* m_player_dead_sound;
	Mix_Chunk* m_player_jump_sound;
	Mix_Chunk* level_complete_sound;

    // C++ rng
	std::default_random_engine m_rng;
	std::uniform_real_distribution<float> m_dist; // default 0..1

	Physics* physicsHandler;

	bool is_player_at_goal;

	int rotateCWKey = GLFW_KEY_X;
	int rotateCCWKey = GLFW_KEY_Z;

	int num_levels = 12;
	int current_level = 0;

	const map<int, std::string> platform_types = {
		{49, "FLOOR"},       //1
		{50, "EXIT"},        //2
        {54, "ICE"},         //6
        {65, "SPIKE LEFT"},  //A
        {66, "SPIKE UP"},    //B
        {67, "SPIKE DOWN"},  //C
        {68, "SPIKE RIGHT"}, //D
		{98, "BLADE" }	     //b
	};

    // Variables determined by level data
	vec2 initialPosition;
	float m_maze_width;
    float m_maze_height;

	float m_tile_width = 25.f;
	float m_tile_height = 25.f;

	// Rows of the maze where:
	// 1 = platform
	// 2 = exit
	// 3 = initial position
	// 4 = spider enemy (and its path)
	// 9 = harpy enemy
    std::vector<std::vector <int>> m_maze;

    bool show_help_menu = false;
	bool cameraTracking = true;
	bool canRotate = true;
	bool hasPrompt = false;

	float maxRotationEnergy = 180.f;
	float rotationEnergy = maxRotationEnergy;

	QuadTreeNode m_quad;

	Timer level_timer;
	float timer_pause_start = -1.0f;
	float timer_pause_end = -1.0f;

};