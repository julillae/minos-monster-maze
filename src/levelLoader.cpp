#include "../include/levelLoader.hpp"

// stlib
#include <stdio.h>
#include <string.h>
#include <cassert>
#include <sstream>
#include <time.h>
#include <iostream>
#include <fstream>

std::vector<std::vector <int>> LevelLoader::load_level(int levelNumber, Physics* physicsHandler) {
    this->physicsHandler = physicsHandler;
	floors.init(m_tile_width, m_tile_height);
	ices.init(m_tile_width, m_tile_height);
	spiders.init(physicsHandler);
	harpies.init(physicsHandler);
    
    read_level_data(levelNumber);

    generate_maze();
    return m_maze;
}

void LevelLoader::read_level_data(int levelNumber) {
	std::string level = std::to_string(levelNumber);
	fprintf(stderr, "Loading level %s\n", level.c_str());
	std::string levelPath(level_path());
	std::string fullPath = levelPath + "level" + level + ".txt";
    std::ifstream filein(fullPath.c_str());

	std::string firstLine;
	std::getline(filein, firstLine);
	canRotate = firstLine.compare("1") == 0;

	std::string secondLine;
	std::getline(filein, secondLine);
	cameraTracking = secondLine.compare("1") == 0;

	std::string thirdLine;
	std::getline(filein, thirdLine);
	minotaurPresent = thirdLine.compare("1") == 0;
	if (minotaurPresent) {
		canRotate = false;
		minotaurPresent = true;
	}

	std::string fourthLine;
	std::getline(filein, fourthLine);
	hasPrompt = fourthLine.compare("1") == 0;

	std::string fifthLine;
    std::getline(filein, fifthLine);
	hasLightingEffect = fifthLine.compare("1") == 0;

    for (std::string line; std::getline(filein, line);) {
        std::vector <int> row;
        for(char& c : line) {
            // Covert char to ascii dec and push to row
			int x = static_cast<unsigned char>( c );
			row.push_back(x);
        }
        // Push row to maze array
        m_maze.push_back(row);
    }
}

// Generates maze
void LevelLoader::generate_maze()
{

	bool setting_enemy = false;
	bool setting_rotated_enemy = false;
	bool setting_minotaur = false;
	vec2 enemy_start_pos;

    float i = 0.f;
	float j = 0.f;

	for (auto &row : m_maze) {
        j = 0.f;
		for (int &cell : row) {

			float x_pos = (j * m_tile_width);
			float y_pos = (i * m_tile_height);

			if ((setting_enemy && cell != 52) || (setting_rotated_enemy && cell != 53) || (setting_minotaur && cell != 69)) {
				// If we were setting enemy positions, and we hit a cell with no enemy,
				// Spawn the enemy we were setting

				float last_x_pos = x_pos - m_tile_width;
				float distance = abs(last_x_pos - enemy_start_pos.x);

				if (setting_enemy || setting_rotated_enemy) {
					spiders.spawn_spider_enemy(enemy_start_pos, distance, setting_rotated_enemy);
					setting_enemy = false;
					setting_rotated_enemy = false;
				} else if (setting_minotaur) {
					spawn_minotaur(enemy_start_pos, distance);
					minotaurPresent = true;
					setting_minotaur = false;
				}
			}

			if (cell == 49) {
				// Spawn platform
				if (floors.spawn_floor({ x_pos, y_pos })) {
					store_platform_coords({ x_pos, y_pos }, cell);
				}
			} else if (cell == 50) {
				// Add exit
				Exit new_exit;

				if ( new_exit.init({x_pos, y_pos}) ) {
					store_platform_coords({x_pos, y_pos}, cell);

					m_exit = new_exit;
				}
			} else if (cell == 51) {
				// Set initial position of player
				m_initial_position = {x_pos, y_pos};
			} else if (cell == 52) {
				// Begin setting enemy path
				if (!setting_enemy) {
					setting_enemy = true;
					enemy_start_pos = {x_pos, y_pos};
				}
			} else if (cell == 53) {
				// Begin setting enemy path
				if (!setting_rotated_enemy) {
					setting_rotated_enemy = true;
					enemy_start_pos = {x_pos, y_pos};
				}
			} else if (cell == 54) {

                if (ices.spawn_ice({x_pos, y_pos}))
                	store_platform_coords({x_pos, y_pos}, cell);

            } else if (cell >= 65 && cell <= 68) {
                load_spikes(cell, vec2({x_pos, y_pos}));
			} else if (cell == 57) {
				harpies.spawn_harpy_enemy(vec2({x_pos, y_pos}));
			} else if (cell == 69) {
				if (!setting_minotaur) {
					setting_minotaur = true;
					enemy_start_pos = {x_pos, y_pos};
				}
			} else if (cell == 98) {
				if (blades.spawn_blades(vec2({x_pos, y_pos}))) {
					store_platform_coords({x_pos, y_pos}, cell);
				}
			}

            j = j + 1.f;
		}
        i = i + 1.f;
	}
    // Set global variables
    m_maze_width = j;
    m_maze_height = i;
}

void LevelLoader::store_platform_coords(vec2 coords, int platform_key) {
	std::string platformType = platform_types.find(platform_key)->second;
	std::pair <float,float> coords_pair (coords.x,coords.y);
	m_platforms_by_coords.emplace(coords_pair, platformType);
}

bool LevelLoader::spawn_minotaur(vec2 position, float bound) 
{
	if (m_minotaur.init(position, physicsHandler))
	{
		m_minotaur.set_bound(bound);
		return true;
	}
	fprintf(stderr, "Failed to spawn minotaur");
	return false;
}

void LevelLoader::load_spikes(int cell, vec2 position)
{
	std::string platformType = platform_types.find(cell)->second;

    if (platformType == "SPIKE LEFT") {
        float spike_x = position.x - m_tile_width / 2;

        if (spikes.spawn_spike({spike_x, position.y}, LEFT))
            store_platform_coords({spike_x, position.y}, cell);

    } else if (platformType == "SPIKE UP") {

        float spike_y = position.y - m_tile_height / 2;
        if (spikes.spawn_spike({position.x, spike_y}, UP))
            store_platform_coords({position.x, spike_y}, cell);

    } else if (platformType == "SPIKE DOWN") {

        float spike_y = position.y + m_tile_height / 2;
        if (spikes.spawn_spike({position.x, spike_y}, DOWN))
            store_platform_coords({position.x, spike_y}, cell);

    } else {
        float spike_x = position.x + m_tile_width / 2;

        if (spikes.spawn_spike({spike_x, position.y}, RIGHT))
            store_platform_coords({spike_x, position.y}, cell);
    }
}

// Returns the platform type if there is a platform at these coordinates
// If no platform exists, returns ""
std::string LevelLoader::get_platform_by_coordinates(std::pair<float, float> coords) {
	std::pair<float, float> coords_check (coords.first, coords.second);
	if (m_platforms_by_coords.find(coords_check) != m_platforms_by_coords.end()) {
		return m_platforms_by_coords.find(coords_check)->second;
	}

	return "";
}

// Method for visualizing full maze in console for debugging purposes
// 1 = platform
// 2 = exit
// 3 = initial player position
// 4 = enemy path
// 5 = upside down enemy path
void LevelLoader::print_maze() {
	for (size_t i = 0; i < m_maze.size(); i++)
	{
		cout << "\n";
		for (size_t j = 0; j < m_maze[i].size(); j++)
		{
			cout << m_maze[i][j];
		}
	}
}

float LevelLoader::get_maze_width() { return m_maze_width; }

float LevelLoader::get_maze_height() { return m_maze_height; }

bool LevelLoader::can_rotate() { return canRotate; }

bool LevelLoader::can_camera_track() { return cameraTracking; }

bool LevelLoader::has_prompt() { return hasPrompt; }

bool LevelLoader::has_lighting_effect() { return hasLightingEffect; }

vec2 LevelLoader::get_player_position() { return m_initial_position; }

Exit LevelLoader::get_exit() { return m_exit; }

Spiders LevelLoader::get_spiders() { return spiders; }

Harpies LevelLoader::get_harpies() { return harpies; }

Minotaur LevelLoader::get_minotaur() { return m_minotaur; }

bool LevelLoader::minotaurInLevel() { return minotaurPresent; }

Floors LevelLoader::get_floors() { return floors; }

Spikes LevelLoader::get_spikes() { return spikes; }

Ices LevelLoader::get_ice() { return ices; }

Blades LevelLoader::get_blades() { return blades; }