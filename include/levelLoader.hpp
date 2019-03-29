#pragma once

// internal
#include "common.hpp"
#include "mazeComponents/floor.hpp"
#include "mazeComponents/exit.hpp"
#include "mazeComponents/ice.hpp"
#include "mazeComponents/spikes.hpp"
#include "physics.hpp"
#include "characters/spider.hpp"
#include "characters/harpy.hpp"

// stlib
#include <vector>
#include <random>
#include <map>
#include <algorithm>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>
#include <time.h>

enum SpikeDir { UP, DOWN, LEFT, RIGHT};

class LevelLoader
{
public:
    std::vector<std::vector <int>> load_level(int levelNumber, Physics* physicsHandler);

    std::string get_platform_by_coordinates(std::pair<float, float> coords);

    float get_maze_width();
	float get_maze_height();

    bool can_rotate();
    bool can_camera_track();
    bool has_prompt();

    vec2 get_player_position();
    Exit get_exit();

    std::vector<Spider> get_spiders();
	std::vector<Harpy> get_harpies();

	std::vector<Floor> get_floors();
	std::vector<Spikes> get_spikes();
	std::vector<Ice> get_ice();
private:
    void read_level_data(int levelNumber);
    void generate_maze();

    void store_platform_coords(vec2 coords, int platform_key);

    bool spawn_spider_enemy(vec2 position, float bound, bool upsideDown);
    bool spawn_harpy_enemy(vec2 position);
    bool spawn_floor(vec2 position);
    bool spawn_ice(vec2 position);
    bool spawn_spikes(vec2 position, SpikeDir dir);
    void load_spikes(int cell, vec2 position);

    void print_maze();
private:
    std::vector<std::vector <int>> m_maze;
	std::map<std::pair<float, float>, std::string> m_platforms_by_coords;

    Physics* physicsHandler;

    std::map<int, std::string> platform_types = {
		{49, "FLOOR"},       //1
		{50, "EXIT"},        //2
        {54, "ICE"},         //6
        {65, "SPIKE LEFT"},  //A
        {66, "SPIKE UP"},    //B
        {67, "SPIKE DOWN"},  //C
        {68, "SPIKE RIGHT"}  //D
	};

    bool canRotate;
    bool cameraTracking;
    bool hasPrompt;

    float m_maze_width;
    float m_maze_height;

    float m_tile_width = 25.f;
	float m_tile_height = 25.f;

    vec2 m_initial_position;
    Exit m_exit;

    std::vector<Spider> m_spiders;
	std::vector<Harpy> m_harpies;

	std::vector<Floor> m_floors;
	std::vector<Spikes> m_spikes;
	std::vector<Ice> m_ice;
};