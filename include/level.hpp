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
#include "mazeComponents/ice.hpp"
#include "mazeComponents/spikes.hpp"
#include "renderEffects.hpp"
#include "physics.hpp"
#include "menus/helpMenu.hpp"
#include "gameStates/gameState.hpp"
#include "gameStates/mainMenuState.hpp"

// stlib
#include <vector>
#include <random>
#include <map>
#include <memory>
#include <algorithm>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>
#include <time.h>

// Level class

typedef std::vector<std::unique_ptr<Enemy>> Enemies;
typedef std::vector<std::unique_ptr<FixedComponent>> Platforms;
enum SpikeDir { UP, DOWN, LEFT, RIGHT};

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

	std::string get_platform_by_coordinates(std::pair<float, float> coords);
	bool maze_is_platform(std::pair<int,int> coords);
	std::vector<std::vector <int>> get_original_maze();

	float get_maze_width();
	float get_maze_height();
	float get_tile_width();
	float get_tile_height();
private:
	// !!! INPUT CALLBACK FUNCTIONS
	void on_key(GLFWwindow*, int key, int, int action, int mod)override;
	void on_mouse_move(GLFWwindow* window, double xpos, double ypos);

    void read_level_data();

	// Generate a spider enemy
	bool spawn_spider_enemy(vec2 position, float bound, bool upsideDown);

	// Generate harpy enemy
	bool spawn_harpy_enemy(vec2 position);

	// Generates a new floor
	bool spawn_floor(vec2 position);
    bool spawn_ice(vec2 position);
    bool spawn_spikes(vec2 position, SpikeDir dir);

	void initialize_camera_position(int w, int h);
	void load_new_level();
	void reset_game();
	void freeze_all_enemies();
	void unfreeze_all_enemies();
	void update_all_enemies(float elapsed_ms);


	// Generates hard-coded maze in each level
	void generate_maze();
	void print_maze();
	void store_platform_coords(vec2 coords, int platform_key);

	void set_player_death();

	void load_spikes(int cell, vec2 position);
private:

	// Water effect
	RenderEffects m_water;

	Exit m_exit;
	Enemies m_enemies;
	Platforms m_platforms;
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
	std::map<std::pair<float, float>, std::string> platforms_by_coords;

    bool show_help_menu = false;
	bool cameraTracking = true;
	bool canRotate = true;
};