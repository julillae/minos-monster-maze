#pragma once

// internal
#include "common.hpp"
#include "mazeComponents/floor.hpp"
#include "mazeComponents/exit.hpp"
#include "mazeComponents/ice.hpp"
#include "mazeComponents/spike.hpp"
#include "mazeComponents/blade.hpp"
#include "physics.hpp"
#include "characters/spider.hpp"
#include "characters/harpy.hpp"
#include "characters/minotaur.hpp"

// stlib
#include <vector>
#include <random>
#include <map>
#include <algorithm>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>
#include <time.h>

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


    Minotaur get_minotaur();
    bool minotaurInLevel();
    Spiders get_spiders();
	Harpies get_harpies();
	Floors get_floors();
	Spikes get_spikes();
	Ices get_ice();
	Blades get_blades();

private:
    void read_level_data(int levelNumber);
    void generate_maze();

    void store_platform_coords(vec2 coords, int platform_key);

    bool spawn_minotaur(vec2 position, float bound);

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
        {68, "SPIKE RIGHT"}, //D
		{115, "BLADE" }	     //s
	};

    bool canRotate;
    bool cameraTracking;
    bool minotaurPresent;
    bool hasPrompt;

    float m_maze_width;
    float m_maze_height;

    float m_tile_width = 25.f;
	float m_tile_height = 25.f;

    vec2 m_initial_position;
    Exit m_exit;


    Minotaur m_minotaur;
	Spiders spiders;
	Harpies harpies;
	Floors floors;
	Spikes spikes;
	Ices ices;
	Blades blades;
};