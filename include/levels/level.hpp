#pragma once

// internal
#include "../common.hpp"
#include "../characters/player.hpp"
#include "../characters/enemy.hpp"
#include "../characters/simple.hpp"
#include "../characters/spider.hpp"
#include "../mazeComponents/mazeComponent.hpp"
#include "../mazeComponents/fixedComponent.hpp"
#include "../mazeComponents/floor.hpp"
#include "../mazeComponents/exit.hpp"
#include "../mazeComponents/ice.hpp"
#include "../renderEffects.hpp"
#include "../physics.hpp"

// stlib
#include <vector>
#include <random>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

// Level class

class Level
{
public:
	Level();
	~Level();

    // Creates a window, sets up events and begins the game
	bool init(vec2 screen, Physics* physicsHandler, char* levelName);

	// Releases all associated resource
    void destroy();

	// Steps the game ahead by ms milliseconds
    bool update(float elapsed_ms);

    // Renders our scene
	void draw();

	// Should the game be over ?
	bool is_over()const;

    int get_maze();
private:
	// !!! INPUT CALLBACK FUNCTIONS
	void on_key(GLFWwindow*, int key, int, int action, int mod);
	void on_mouse_move(GLFWwindow* window, double xpos, double ypos);

    void read_txt_file(char* levelName);

	// Add enemies with hardcoded positions
	void spawn_enemies();

	// Helper to generates a new enemy
	bool spawn_spider_enemy(vec2 position, float bound);

	// Generates a new floor
	bool spawn_floor(vec2 position);

	void reset_game();

	// Generates hard-coded maze in each level
	void generate_maze();
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

    float m_seed_rng;

    Mix_Music* m_background_music;
	Mix_Chunk* m_salmon_dead_sound;

    // C++ rng
	std::default_random_engine m_rng;
	std::uniform_real_distribution<float> m_dist; // default 0..1

	Physics* physicsHandler;

	bool is_player_at_goal;
	// Part of hack needed to deal with the MacOs Retina Display issue where it doubles the pixels rendered
	float osScaleFactor = 1.f;

    // Variables set by subclass
	vec2 initialPosition = { 700.f, 625.f };

    std::vector<std::vector <int>> m_maze;
    float m_maze_width;
    float m_maze_height;
};