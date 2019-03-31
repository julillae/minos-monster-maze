#pragma once

// internal
#include "common.hpp"
#include "characters/player.hpp"
#include "characters/enemy.hpp"
#include "characters/simple.hpp"
#include "characters/spider.hpp"
#include "characters/harpy.hpp"
#include "mazeComponents/mazeComponent.hpp"
#include "mazeComponents/fixedComponent.hpp"
#include "mazeComponents/floor.hpp"
#include "mazeComponents/exit.hpp"
#include "mazeComponents/fire.hpp"
#include "mazeComponents/ice.hpp"
#include "mazeComponents/spikes.hpp"
#include "renderEffects.hpp"
#include "physics.hpp"
#include "menus/helpMenu.hpp"
#include "gameStates/gameState.hpp"
#include "gameStates/pauseMenuState.hpp"
#include "levelLoader.hpp"

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

	bool maze_is_platform(std::pair<int,int> coords);
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

    void load_saved_game();
    void reset_game();
private:
	// !!! INPUT CALLBACK FUNCTIONS
	void on_key(GLFWwindow*, int key, int, int action, int mod)override;
	void on_mouse_move(GLFWwindow* window, double xpos, double ypos);

	void check_platform_collisions();

	void draw_enemies(mat3 projection_2D);
	void reset_enemies();
	void destroy_enemies();
	void draw_platforms(mat3 projection_2D);
	void destroy_platforms();

	void initialize_camera_position(int w, int h);
	void call_level_loader();
	void load_new_level();
	void reset_player_camera();
	void freeze_all_enemies();
	void unfreeze_all_enemies();
	void update_all_enemies(float elapsed_ms);

	void set_player_death();

	void load_player();
	void load_spiders();
	void load_harpies();
private:

	// Water effect
	RenderEffects m_water;

	std::vector<Spider> m_spiders;
	std::vector<Harpy> m_harpies;

	Exit m_exit;
	Fire m_fire;
	std::vector<Floor> m_floors;
	std::vector<Spikes> m_spikes;
	std::vector<Ice> m_ice;
    HelpMenu m_help_menu;

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

	int num_levels = 11;
	int current_level = 0;

	const map<int, std::string> platform_types = {
		{49, "FLOOR"},       //1
		{50, "EXIT"},        //2
        {54, "ICE"},         //6
        {65, "SPIKE LEFT"},  //A
        {66, "SPIKE UP"},    //B
        {67, "SPIKE DOWN"},  //C
        {68, "SPIKE RIGHT"}  //D
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

	float maxRotationEnergy = 180.f;
	float rotationEnergy = maxRotationEnergy;
};