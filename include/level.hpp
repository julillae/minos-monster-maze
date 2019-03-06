#pragma once

// internal
#include "common.hpp"
#include "characters/player.hpp"
#include "characters/enemy.hpp"
#include "characters/simple.hpp"
#include "characters/spider.hpp"
#include "mazeComponents/mazeComponent.hpp"
#include "mazeComponents/fixedComponent.hpp"
#include "mazeComponents/floor.hpp"
#include "mazeComponents/exit.hpp"
#include "mazeComponents/ice.hpp"
#include "renderEffects.hpp"
#include "physics.hpp"
#include "helpMenu.hpp"

// stlib
#include <vector>
#include <random>
#include <map>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>
#include <time.h>

// Level class

class Level
{
public:
	Level();
	~Level();

    // Creates a window, sets up events and begins the game
	bool init(vec2 screen, Physics* physicsHandler, int startLevel);

	// Releases all associated resource
    void destroy();

	// Steps the game ahead by ms milliseconds
    bool update(float elapsed_ms);

    // Renders our scene
	void draw();

	// Should the game be over ?
	bool is_over()const;

	std::string get_platform_by_coordinates(std::pair<float, float> coords);
	std::vector<std::vector <int>> get_original_maze();

	float get_maze_width();
	float get_maze_height();
	float get_tile_width();
	float get_tile_height();
private:
	// !!! INPUT CALLBACK FUNCTIONS
	void on_key(GLFWwindow*, int key, int, int action, int mod);
	void on_mouse_move(GLFWwindow* window, double xpos, double ypos);

    void read_level_data();

	// Generate a spider enemy
	bool spawn_spider_enemy(vec2 position, float bound);

	// Generates a new floor
	bool spawn_floor(vec2 position);

	void load_new_level();
	void reset_game();
	void freeze_all_enemies();
	void unfreeze_all_enemies();
	void update_all_enemies(float elapsed_ms);


	// Generates hard-coded maze in each level
	void generate_maze();
	void print_maze();
	void store_platform_coords(vec2 coords, int platform_key);

private:
	// Window handle
	GLFWwindow* m_window;

    // Screen texture
	// The draw loop first renders to this texture, then it is used for the water shader
	GLuint m_frame_buffer;
	Texture m_screen_tex;

	// Water effect
	RenderEffects m_water;

    Player m_player;
	Exit m_exit;
	std::vector<Spider> m_enemies;
    std::vector<Floor> m_floor;
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
	// Part of hack needed to deal with the MacOs Retina Display issue where it doubles the pixels rendered
	float osScaleFactor = 1.f;

	float tx;
	float ty;
	bool is_keyMappingSetA = true;

	int num_levels = 2;
	int current_level = 0;

	const map<int, std::string> platform_types = {
		{1, "FLOOR"},
		{2, "EXIT"}
	};

    // Variables determined by level data
	vec2 initialPosition;
	float m_maze_width;
    float m_maze_height;

	float m_tile_width = 0.f;
	float m_tile_height = 0.f;

	// Rows of the maze where:
	// 1 = platform
	// 2 = exit
	// 3 = initial position
	// 4 = spider enemy (and its path)
    std::vector<std::vector <int>> m_maze;
	std::map<std::pair<float, float>, std::string> platforms_by_coords;

    bool show_help_menu = false;
};