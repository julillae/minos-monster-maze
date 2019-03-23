// Header
#include "../include/level.hpp"
#include "../include/physics.hpp"
#include "../include/common.hpp"

// stlib
#include <stdio.h>
#include <string.h>
#include <cassert>
#include <sstream>
#include <time.h>
#include <iostream>
#include <fstream>
#include <memory>

namespace
{
	float rotationStart = 0.f;
	float rotation = 0.f;
	float rotationDeg = 0.f;
	float rotationSpeed;
	float maxRotationSpeed = 1.0f;
	float currentIntervalPos = 0.f;
	float maxIntervalLength = 50.f;
	float normalizedIntervalPos;
	bool isRotating = false;
	bool rotateCW = false;
	bool previouslyFrozen = false;
	vec2 cameraCenter;
	vec2 prevCameraCenter;
    void glfw_err_cb(int error, const char* desc)
    {
        fprintf(stderr, "%d: %s", error, desc);
    }
}


Level::Level(Game* game) : m_seed_rng(0.f)
{
// Seeding rng with random device
	m_rng = std::default_random_engine(std::random_device()());
	this->game = game;
}

Level::~Level()
{

}

void Level::store_platform_coords(vec2 coords, int platform_key) {
	std::string platformType = platform_types.find(platform_key)->second;
	std::pair <float,float> coords_pair (coords.x,coords.y);
	platforms_by_coords.emplace(coords_pair, platformType);
}

void Level::read_level_data() {
	std::string level = std::to_string(current_level);
	fprintf(stderr, "Loading level %s\n", level.c_str());
	std::string fileName = "levels/level" + level + ".txt";
    std::ifstream filein(fileName);

	std::string firstLine;
	std::getline(filein, firstLine);
	canRotate = firstLine.compare("1") == 0;

	std::string secondLine;
	std::getline(filein, secondLine);
	cameraTracking = secondLine.compare("1") == 0;

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

bool Level::spawn_spider_enemy(vec2 position, float bound, bool upsideDown)
{
	std::unique_ptr<Spider> enemy = std::unique_ptr<Spider>(new Spider);

	if (enemy->init(position, physicsHandler))
	{
		if (upsideDown) {
			enemy->set_rotation(M_PI);
			vec2 enemy_scale = enemy->get_scale();
			enemy->set_scale({enemy_scale.x * -1.f, enemy_scale.y});
		}

		enemy->set_bound(bound);
		m_enemies.emplace_back(std::move(enemy));

		return true;
	}
	fprintf(stderr, "Failed to spawn enemy");
	return false;
}

bool Level::spawn_harpy_enemy(vec2 position)
{
	std::unique_ptr<Harpy> enemy = std::unique_ptr<Harpy>(new Harpy);
	if (enemy->init(position, physicsHandler))
	{	
		m_enemies.emplace_back(std::move(enemy));
		return true;
	}
	fprintf(stderr, "Failed to spawn harpy");
	return false;
}

bool Level::spawn_floor(vec2 position)
{
	std::unique_ptr<Floor> floor = std::unique_ptr<Floor>(new Floor);

	if (floor->init(position))
	{
		vec2 textureSize = floor->get_texture_size();
		float x_scale = m_tile_width / textureSize.x;
		float y_scale = m_tile_height / textureSize.y;
		floor->set_scale(vec2({x_scale, y_scale}));
		floor->set_size(vec2({m_tile_width, m_tile_height}));
		m_platforms.emplace_back(std::move(floor));
		return true;
	}
	fprintf(stderr, "Failed to spawn floor");
	return false;
}

bool Level::spawn_ice(vec2 position)
{
	std::unique_ptr<Ice> ice = std::unique_ptr<Ice>(new Ice);

	if (ice->init(position))
	{
		vec2 textureSize = ice->get_texture_size();
		float x_scale = m_tile_width / textureSize.x;
		float y_scale = m_tile_height / textureSize.y;
		ice->set_scale(vec2({x_scale, y_scale}));
		ice->set_size(vec2({m_tile_width, m_tile_height}));
		m_platforms.emplace_back(std::move(ice));
		return true;
	}
	fprintf(stderr, "Failed to spawn ice");
	return false;
}

bool Level::spawn_spikes(vec2 position, SpikeDir dir)
{
    std::unique_ptr<Spikes> spikes = std::unique_ptr<Spikes>(new Spikes);

    if (spikes->init(position))
    {
    	switch (dir)
		{
			case DOWN:
				spikes->set_down();
				break;
			case LEFT:
				spikes->set_left();
				break;
			case RIGHT:
				spikes->set_right();
				break;
			default:
				spikes->set_up();
				break;
		}

        m_platforms.emplace_back(std::move(spikes));
        return true;
    }
    fprintf(stderr, "Failed to spawn spikes");
    return false;
}

// Generates maze
void Level::generate_maze()
{
	fprintf(stderr, "Generating maze\n");

	bool setting_enemy = false;
	bool setting_rotated_enemy = false;
	vec2 enemy_start_pos;

    float i = 0.f;
	float j = 0.f;

	for (auto &row : m_maze) {
        j = 0.f;
		for (int &cell : row) {

			float x_pos = (j * m_tile_width);
			float y_pos = (i * m_tile_height);

			if ((setting_enemy && cell != 52) || (setting_rotated_enemy && cell != 53)) {
				// If we were setting enemy positions, and we hit a cell with no enemy,
				// Spawn the enemy we were setting

				float last_x_pos = x_pos - m_tile_width;
				float distance = abs(last_x_pos - enemy_start_pos.x);
				spawn_spider_enemy(enemy_start_pos, distance, setting_rotated_enemy);
				setting_enemy = false;
				setting_rotated_enemy = false;
			}

			if (cell == 49) {
				// Spawn platform
				if ( spawn_floor({x_pos, y_pos}) ) {
					store_platform_coords({x_pos, y_pos}, cell);
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
				initialPosition = {x_pos, y_pos};
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

                if (spawn_ice({x_pos, y_pos}))
                	store_platform_coords({x_pos, y_pos}, cell);

            } else if (cell >= 65 && cell <= 68) {
                load_spikes(cell, vec2({x_pos, y_pos}));
			} else if (cell == 57) {

				spawn_harpy_enemy(vec2({x_pos, y_pos}));
			}

            j = j + 1.f;
		}
        i = i + 1.f;
	}

    // Set global variables
    m_maze_width = j;
    m_maze_height = i;
}

// Level initialization
bool Level::init(vec2 screen, Physics* physicsHandler, int startLevel)
{
	this->physicsHandler = physicsHandler;
	this->m_window = game->m_window;

	// hack used to make sure the display for macOS with retina display issue is consistent with display on other systems
	int testWidth;
	glfwGetFramebufferSize(m_window, &testWidth, nullptr);
	osScaleFactor = testWidth / screen.x;

	// Load OpenGL function pointers
	gl3w_init();

	set_onKey();

	// Create a frame buffer
	m_frame_buffer = 0;
	glGenFramebuffers(1, &m_frame_buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_frame_buffer);

	// Initialize the screen texture
	m_screen_tex.create_from_screen(m_window);

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
	//Secret Catacombs
	//by Eric Matyas
	//www.soundimage.org

	m_background_music = Mix_LoadMUS(audio_path("secret_catacombs.wav"));
	m_player_dead_sound = Mix_LoadWAV(audio_path("death.wav"));
	m_player_jump_sound = Mix_LoadWAV(audio_path("jump.wav"));
	level_complete_sound = Mix_LoadWAV(audio_path("nextLevel.wav"));

	if (m_background_music == nullptr)
	{
		fprintf(stderr, "Failed to load sound\n %s,%s\n make sure the data directory is present",
			audio_path("salmon_dead.wav"),
			audio_path("secret_catacombs.wav"));
		return false;
	}

	// Playing background music undefinitely
	Mix_PlayMusic(m_background_music, -1);
	Mix_VolumeMusic(50);
	
	fprintf(stderr, "Loaded music\n");

	is_player_at_goal = false;
	int w, h;
	glfwGetFramebufferSize(m_window, &w, &h);
	glViewport(0, 0, w, h);

	current_level = startLevel;
    read_level_data();
	generate_maze();

	m_help_menu.init(initialPosition);
	initialize_camera_position(w, h);
	
	return m_water.init() && m_player.init(initialPosition, physicsHandler);
}

// Releases all the associated resources
void Level::destroy()
{
	glDeleteFramebuffers(1, &m_frame_buffer);

	if (m_background_music != nullptr)
		Mix_FreeMusic(m_background_music);
	if (m_player_dead_sound != nullptr)
		Mix_FreeChunk(m_player_dead_sound);
	if (m_player_jump_sound != nullptr)
		Mix_FreeChunk(m_player_jump_sound);
	if (level_complete_sound != nullptr)
		Mix_FreeChunk(level_complete_sound);

	Mix_CloseAudio();

	m_player.destroy();
	for (auto& enemy : m_enemies)
		enemy->destroy();
	for (auto& platform: m_platforms)
		platform->destroy();
	m_enemies.clear();
	m_platforms.clear();
	m_help_menu.destroy();

	glfwDestroyWindow(m_window);
}

// Update our game world
bool Level::update(float elapsed_ms)
{
	int w, h;
        glfwGetFramebufferSize(m_window, &w, &h);
	vec2 screen = { (float)w, (float)h };
	bool applyFreeze = false;
	bool applyThaw = false;

	physicsHandler->updateWorldRotation(rotation);

	// freezes and unfreezes character for rotation
	if (show_help_menu) {
		if (!previouslyFrozen) {
			applyFreeze = true;
			previouslyFrozen = true;
		}
	}
	else if (isRotating) {
		currentIntervalPos++;
		currentIntervalPos = min(currentIntervalPos, maxIntervalLength);
		normalizedIntervalPos = currentIntervalPos / maxIntervalLength;
		rotationSpeed = hermiteSplineVal(0.f, maxRotationSpeed, 0.f, 0.f, normalizedIntervalPos);
		if (rotateCW) rotationSpeed *= -1;
		rotationDeg = fmod(rotationDeg + rotationSpeed, 360.f);

		rotation = static_cast<float>((rotationDeg * M_PI) / 180);
		if (!previouslyFrozen) {
			applyFreeze = true;
			previouslyFrozen = true;
		}
	}
	else if (previouslyFrozen) {
		applyThaw = true;
		previouslyFrozen = false;
	}

	// Checking Player - Enemy Collision
	for (auto& enemy : m_enemies) {
		if (physicsHandler->collideWithEnemy(&m_player, enemy))
		{
			set_player_death();
		}
	}

//	 Checking Player - Exit Collision
	if (physicsHandler->collideWithExit(&m_player, &m_exit) && !is_player_at_goal)
	{
		Mix_PlayChannel(-1, level_complete_sound, 0);
		m_water.set_level_complete_time();
		is_player_at_goal = true;
		m_player.set_invincibility(true);
	}

	// checking player - platform collision
	if (physicsHandler->characterCollisionsWithFixedComponents(&m_player, m_platforms))
	{
		set_player_death();
	}

	m_player.set_rotation(rotation);
	if (applyFreeze) {
		m_player.freeze();
		freeze_all_enemies();
		rotationStart = rotation;
	}
	else if (applyThaw) {
		float rotateVelVec = rotation - rotationStart;
		physicsHandler->updateCharacterVelocityRotation(&m_player, rotateVelVec);
		m_player.unfreeze();
		unfreeze_all_enemies();
	}
	if (m_player.characterState->currentState == jumping) 
		Mix_PlayChannel(-1, m_player_jump_sound, 0);
	m_player.update(elapsed_ms);

	update_all_enemies(elapsed_ms);

	m_help_menu.set_visibility(show_help_menu);

	// If player is dead or beat the game, restart the game after the fading animation
	if (!m_player.is_alive() && m_water.get_time_since_death() > 1.5)
		reset_game();

	if (m_player.is_alive() && is_player_at_goal && m_water.get_time_since_level_complete() > 1.5)
		reset_game();

	return true;
}

// Render our game world
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
void Level::draw()
{
	// Clearing error buffer
	gl_flush_errors();

	// Getting size of window
	int w, h;
    glfwGetFramebufferSize(m_window, &w, &h);

	// Updating window title with points
	std::stringstream title_ss;
	title_ss << "Minos' Monster Maze";
	glfwSetWindowTitle(m_window, title_ss.str().c_str());

	if (is_player_at_goal)
	{
		title_ss.str("");
		title_ss << "You Win!";
		glfwSetWindowTitle(m_window, title_ss.str().c_str());
	}

	/////////////////////////////////////
	// First render to the custom framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, m_frame_buffer);

	// Clearing backbuffer
	glViewport(0, 0, w, h);
	glDepthRange(0.00001, 10);
	const float clear_color[3] = { 0.5f, 0.5f, 0.5f };

	glClearColor(clear_color[0], clear_color[1], clear_color[2], 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Fake projection matrix, scales with respect to window coordinates
	// PS: 1.f / w in [1][1] is correct.. do you know why ? (:
	float left = 0.f;// *-0.5;
	float top = 0.f;// (float)h * -0.5;
	float right = (float)w;// *0.5;
	float bottom = (float)h;// *0.5;

	vec2 p_position = m_player.get_position();

	float sx = 2.f * osScaleFactor / (right - left);
	float sy = 2.f * osScaleFactor / (top - bottom); //this is where you play around with the camera
	
	if (cameraTracking && !show_help_menu){
		vec2 deviationVector = add(p_position, negateVec(prevCameraCenter));
		vec2 shrinkingTetherVector = { 0.f,0.f };
		if (vecLength(deviationVector) > g_tolerance) {
			shrinkingTetherVector = scalarMultiply(deviationVector, 0.05f);
		}
		cameraCenter = add(prevCameraCenter, shrinkingTetherVector);
		prevCameraCenter = cameraCenter;
	}
	tx = -cameraCenter.x;
	ty = -cameraCenter.y;

	float c = cosf(-rotation);
	float s = sinf(-rotation);

	mat3 scaling_matrix = { {sx, 0.f, 0.f },
							{ 0.f, sy, 0.f },
							{ 0.f, 0.f, 1.f } };

    mat3 rotation_matrix = { { c, s, 0.f },
							{ -s, c, 0.f },
							{ 0.f, 0.f, 1.f } };

    mat3 translation_matrix = { {1.f, 0.f, 0.f},
								{0.f, 1.f, 0.f},
								{tx, ty, 1.f} };

    mat3 projection_2D{ { 1.f, 0.f, 0.f },
						{ 0.f, 1.f, 0.f },
						{ 0.f, 0.f, 1.f } };

    projection_2D = mul(projection_2D, scaling_matrix);
    if (!show_help_menu)
	{
		projection_2D = mul(projection_2D, rotation_matrix);
	}

	projection_2D = mul(projection_2D, translation_matrix);

    for (auto& platform : m_platforms)
        platform->draw(projection_2D);
	for (auto& enemy : m_enemies)
		enemy->draw(projection_2D);
	m_exit.draw(projection_2D);
	m_player.draw(projection_2D);

	/////////////////////
	// Truely render to the screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Clearing backbuffer
	glViewport(0, 0, w, h);
	glDepthRange(0, 10);
	glClearColor(0, 0, 0, 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_screen_tex.id);

	m_water.draw(projection_2D);

	m_help_menu.draw(projection_2D);

	//////////////////
	// Presenting
	glfwSwapBuffers(m_window);
}

// Should the game be over ?
bool Level::is_over()
{
	return glfwWindowShouldClose(m_window);
}

// On key callback
void Level::on_key(GLFWwindow*, int key, int, int action, int mod)
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// key is of 'type' GLFW_KEY_
	// action can be GLFW_PRESS GLFW_RELEASE GLFW_REPEAT
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	m_player.on_key(key, action);

	if (action == GLFW_PRESS){
		if(key == GLFW_KEY_1){
			rotateCWKey = GLFW_KEY_X;
			rotateCCWKey = GLFW_KEY_Z;
			m_player.jumpKey = GLFW_KEY_UP;
		}
		if(key == GLFW_KEY_2){
			rotateCWKey = GLFW_KEY_S;
			rotateCCWKey = GLFW_KEY_A;
			m_player.jumpKey = GLFW_KEY_SPACE;
		}

		//TODO: make pause menu
		if (key == GLFW_KEY_ESCAPE) {
			GameState* mainMenuState = game->get_states().front();
		    game->set_current_state(mainMenuState);
		}
	}

	if (action == GLFW_PRESS && canRotate) {
		currentIntervalPos = 0;
		if (key == rotateCCWKey) {
			isRotating = true;
			rotateCW = false;
		}
		if (key == rotateCWKey) {
			isRotating = true;
			rotateCW = true;
		}
	}

	else if (action == GLFW_RELEASE) {
		currentIntervalPos = 0;
		if (((key == rotateCCWKey && !rotateCW) || (key == rotateCWKey && rotateCW))) {
				isRotating = false;
			}
	}

	if (action == GLFW_PRESS && key == GLFW_KEY_H) {
		show_help_menu = !show_help_menu;
		if (show_help_menu) {
			m_help_menu.set_position(cameraCenter);
		}

	}

	// Resetting game
	if (action == GLFW_RELEASE && key == GLFW_KEY_R)
	{
		reset_game();
	}

}

void Level::on_mouse_move(GLFWwindow* window, double xpos, double ypos)
{

}

void Level::initialize_camera_position(int w, int h)
{
	if (cameraTracking) {
		cameraCenter = (initialPosition);
		prevCameraCenter = cameraCenter;
	}
	else {
		float txOffset = w / 2;
		float tyOffset = h / 2;
		cameraCenter = vec2({ txOffset, tyOffset});
	}
}

void Level::load_new_level()
{
	for (auto& platform: m_platforms)
		platform->destroy();

	for (auto& enemy : m_enemies)
		enemy->destroy();
	
	m_platforms.clear();
	m_enemies.clear();
	m_maze.clear();

	current_level++;
	if (current_level >= num_levels)
		current_level = 0;

	read_level_data();
	generate_maze();
}

void Level::reset_game()
{
	int w, h;
	glfwGetWindowSize(m_window, &w, &h);
	m_player.destroy();
	
	if (is_player_at_goal) {
		load_new_level();
		initialize_camera_position(w, h);
	} else {
		for (auto& enemy : m_enemies) {
			enemy->freeze();
			enemy->reset_position();
			enemy->unfreeze();
		};
	}
	

	m_player.init(initialPosition, physicsHandler);

	m_water.reset_player_win_time();
	m_water.reset_player_dead_time();
	is_player_at_goal = false;
	rotationDeg = 0;
	rotation = 0.f;
	previouslyFrozen = false;
}

void Level::freeze_all_enemies()
{
	for (auto& e : m_enemies) e->freeze();
}

void Level::unfreeze_all_enemies()
{
	for (auto& e : m_enemies) e->unfreeze();
}

void Level::update_all_enemies(float elapsed_ms)
{
	for (auto& e : m_enemies) e->update(elapsed_ms);
}

// Returns the platform type if there is a platform at these coordinates
// If no platform exists, returns ""
std::string Level::get_platform_by_coordinates(std::pair<float, float> coords) {
	std::pair<float, float> coords_check (coords.first, coords.second);
	if (platforms_by_coords.find(coords_check) != platforms_by_coords.end()) {
		return platforms_by_coords.find(coords_check)->second;
	}

	return "";
}

bool Level::maze_is_platform(std::pair<int,int> coords){
	int val_at_coords = m_maze[coords.first][coords.second];
	for (auto& p : platform_types) {
		if (val_at_coords == p.first){
			return true;
		}
	}
	return false;
}

// Method for visualizing full maze in console for debugging purposes
// 1 = platform
// 2 = exit
// 3 = initial player position
// 4 = enemy path
// 5 = upside down enemy path
void Level::print_maze() {
	for (int i = 0; i < m_maze.size(); i++)
	{
		cout << "\n";
		for (int j = 0; j < m_maze[i].size(); j++)
		{
			cout << m_maze[i][j];
		}
	}
}

std::vector<std::vector <int>> Level::get_original_maze() {
	return m_maze;
}

float Level::get_maze_width() {
	return m_maze_width;
}

float Level::get_maze_height() {
	return m_maze_height;
}

float Level::get_tile_width() {
	return m_tile_width;
}

float Level::get_tile_height() {
	return m_tile_height;
}

void Level::set_player_death()
{
	if (!m_player.is_invincible() && m_player.is_alive()) {
		Mix_PlayChannel(-1, m_player_dead_sound, 0);
		m_player.kill();
		m_water.set_player_dead();
	}
}

void Level::load_spikes(int cell, vec2 position)
{
	std::string platformType = platform_types.find(cell)->second;

    if (platformType == "SPIKE LEFT") {
        float spike_x = position.x - m_tile_width / 2;

        if (spawn_spikes({spike_x, position.y}, LEFT))
            store_platform_coords({spike_x, position.y}, cell);

    } else if (platformType == "SPIKE UP") {

        float spike_y = position.y - m_tile_height / 2;
        if (spawn_spikes({position.x, spike_y}, UP))
            store_platform_coords({position.x, spike_y}, cell);

    } else if (platformType == "SPIKE DOWN") {

        float spike_y = position.y + m_tile_height / 2;
        if (spawn_spikes({position.x, spike_y}, DOWN))
            store_platform_coords({position.x, spike_y}, cell);

    } else {
        float spike_x = position.x + m_tile_width / 2;

        if (spawn_spikes({spike_x, position.y}, RIGHT))
            store_platform_coords({spike_x, position.y}, cell);
    }

}