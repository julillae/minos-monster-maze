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

namespace
{
	float rotationStart = 0.f;
	float rotation = 0.f;
	float rotationDeg = 0.f;
	float rotationSpeed;
	float maxRotationSpeed = 1.0f;
	float rotationEnergyIncrement = 30.f;
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

// Level initialization
bool Level::init(vec2 screen, Physics* physicsHandler, int startLevel)
{
	this->physicsHandler = physicsHandler;
	this->m_window = game->m_window;
	name = LEVEL;

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
	call_level_loader();

	m_help_menu.init(initialPosition);
	initialize_camera_position(w, h);
	initialize_message_prompt();
	
	return m_water.init() && m_player.init(initialPosition, physicsHandler);
}

void Level::check_platform_collisions() {
	if (m_player.is_alive()) {
		m_player.set_world_vertex_coord();
		physicsHandler->characterCollisionsWithSpikes(&m_player, m_spikes.get_spike_vector());
		physicsHandler->characterCollisionsWithFloors(&m_player, m_floors.get_floor_vector());
		physicsHandler->characterCollisionsWithIce(&m_player, m_ice.get_ice_vector());

		if (!physicsHandler->isOnAtLeastOnePlatform) m_player.set_in_free_fall();

		if (!m_player.is_alive()) {
			Mix_PlayChannel(-1, m_player_dead_sound, 0);
			m_water.set_player_dead();
		}

		physicsHandler->isOnAtLeastOnePlatform = false;
	}
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
	destroy_enemies();
	destroy_platforms();
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
	m_player.set_rotation(rotation);
	physicsHandler->updateWorldRotation(rotation);

	// freezes and unfreezes character for rotation
	if (show_help_menu) {
		if (!previouslyFrozen) {
			applyFreeze = true;
			previouslyFrozen = true;
		}
	}
	else if (isRotating && rotationEnergy > 0.f) {
		currentIntervalPos++;
		currentIntervalPos = min(currentIntervalPos, maxIntervalLength);
		normalizedIntervalPos = currentIntervalPos / maxIntervalLength;
		rotationSpeed = hermiteSplineVal(0.f, maxRotationSpeed, 0.f, 0.f, normalizedIntervalPos);

		if (rotationEnergy - fabs(rotationSpeed) < 0) rotationSpeed = rotationEnergy;
		if (rotateCW) rotationSpeed *= -1;

		rotationDeg = fmod(rotationDeg + rotationSpeed, 360.f);

		rotation = static_cast<float>((rotationDeg * M_PI) / 180);
		if (!previouslyFrozen) {
			applyFreeze = true;
			previouslyFrozen = true;
		}

		rotationEnergy -= fabs(rotationSpeed);
	}
	else if (previouslyFrozen) {
		applyThaw = true;
		previouslyFrozen = false;

		m_water.set_rotation_end_time();
	}

	// Checking Player - Spider Collision
	for (auto& enemy : m_spiders.get_spider_vector()) {
		if (physicsHandler->collideWithEnemy(&m_player, &enemy))
		{
			set_player_death();
		}
	}

	for (auto& enemy : m_harpies.get_harpy_vector()) {
		if (physicsHandler->collideWithEnemy(&m_player, &enemy))
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
		m_player.freeze();
		m_player.set_invincibility(true);

		if (hasPrompt)
			m_message.destroy();
	}

	// checking player - platform collision
	check_platform_collisions();

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
	
	float timeToLoadRotationEnergy = 4.f;
	if (m_water.get_time_since_rotation_end() > timeToLoadRotationEnergy) {
		rotationEnergy += rotationEnergyIncrement;
		m_water.set_rotation_end_time();

		if (rotationEnergy >= maxRotationEnergy) {
			m_water.reset_rotation_end_time();
			rotationEnergy = maxRotationEnergy;
		}
	}

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
	if (canRotate) {
		// Round energy to two decimal places for printing
		float roundedEnergy = roundf(rotationEnergy * 100.f) / 100.f;
		title_ss << " || Energy left to rotate: " << roundedEnergy << " / " << maxRotationEnergy;
	}
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

	draw_platforms(projection_2D);
	draw_enemies(projection_2D);
	m_exit.draw(projection_2D);
	m_player.draw(projection_2D);

	render_to_screen(w, h);

	m_water.draw(projection_2D);

	m_help_menu.draw(projection_2D);

	if (hasPrompt) {
		float screen_height = static_cast<float>(h);
		float message_y_shift = (screen_height / 2.f) - (m_tile_height * 3.f);
		float message_y_pos = cameraCenter.y - message_y_shift;
		m_message.set_position({cameraCenter.x, message_y_pos});
		m_message.draw(projection_2D);
	}

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

		if (key == GLFW_KEY_ESCAPE) {
            isRotating = false;
			m_player.set_direction(Direction::none);

            PauseMenuState* pauseMenuState = (PauseMenuState*) game->get_state(PAUSE);
            pauseMenuState->reset_buttons();
            game->set_current_state(pauseMenuState);

			if (hasPrompt)
				m_message.destroy();
		}
	}

	if (action == GLFW_PRESS && canRotate) {
		currentIntervalPos = 0;
		if (key == rotateCCWKey) {
			isRotating = true;
			rotateCW = false;

			if (hasPrompt)
				m_message.set_visibility(false);
		}
		if (key == rotateCWKey) {
			isRotating = true;
			rotateCW = true;

			if (hasPrompt)
				m_message.set_visibility(false);
		}
	}

	else if (action == GLFW_RELEASE) {
		currentIntervalPos = 0;
		if (((key == rotateCCWKey && !rotateCW) || (key == rotateCWKey && rotateCW))) {
				isRotating = false;
			}
	}

}

void Level::on_mouse_move(GLFWwindow* window, double xpos, double ypos)
{

}

void Level::initialize_message_prompt() {
	if (hasPrompt) {
		int messageNumber = current_level;
		if (current_level == 4) {
			std::string newMessageNumber = std::to_string(current_level);
			if (rotateCWKey == GLFW_KEY_X) {
				newMessageNumber.append("1");
				messageNumber = std::atoi(newMessageNumber.c_str());
			} else {
				newMessageNumber.append("2");
				messageNumber = std::atoi(newMessageNumber.c_str());
			}
		}

		m_message.init(messageNumber);
	}
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

void Level::draw_enemies(mat3 projection_2D) {
	m_spiders.draw(projection_2D);
	m_harpies.draw(projection_2D);
}

void Level::reset_enemies() {
	m_spiders.reset();
	m_harpies.reset();
}

void Level::destroy_enemies() {
	m_spiders.destroy();
	m_harpies.destroy();
}

void Level::draw_platforms(mat3 projection_2D) {
	m_floors.draw(projection_2D);
	m_ice.draw(projection_2D);
	m_spikes.draw(projection_2D);
}

void Level::destroy_platforms() {
	m_floors.destroy();
	m_spikes.destroy();
	m_ice.destroy();
}

void Level::call_level_loader()
{
	LevelLoader levelLoader;
	m_maze = levelLoader.load_level(current_level, physicsHandler);

	m_maze_width = levelLoader.get_maze_width();
	m_maze_height = levelLoader.get_maze_height();

	canRotate = levelLoader.can_rotate();
	cameraTracking = levelLoader.can_camera_track();
	hasPrompt = levelLoader.has_prompt();

	initialPosition = levelLoader.get_player_position();
	m_exit = levelLoader.get_exit();

	m_spiders = levelLoader.get_spiders();
	m_harpies = levelLoader.get_harpies();

	m_floors = levelLoader.get_floors();
	m_ice = levelLoader.get_ice();
	m_spikes = levelLoader.get_spikes();
}

void Level::load_new_level()
{
	destroy_platforms();
	destroy_enemies();
	
	m_maze.clear();

	current_level++;
	if (current_level >= num_levels)
		current_level = 0;

	call_level_loader();
	initialize_message_prompt();
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
		reset_enemies();
	}
	
	reset_player_camera();
}

void Level::reset_player_camera()
{
	m_player.init(initialPosition, physicsHandler);

	m_water.reset_rotation_end_time();
	m_water.reset_player_win_time();
	m_water.reset_player_dead_time();
	is_player_at_goal = false;
	rotationDeg = 0;
	rotation = 0.f;
	rotationEnergy = maxRotationEnergy;
	previouslyFrozen = false;
}

void Level::freeze_all_enemies()
{
	m_spiders.freeze();
	m_harpies.freeze();
}

void Level::unfreeze_all_enemies()
{
	m_spiders.unfreeze();
	m_harpies.unfreeze();
}

void Level::update_all_enemies(float elapsed_ms)
{
	m_spiders.update(elapsed_ms);
	m_harpies.update(elapsed_ms);
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

void Level::load_select_level(int level)
{
	destroy_platforms();
	destroy_enemies();
	m_maze.clear();

	current_level = level;
	call_level_loader();

	int w, h;
	glfwGetWindowSize(m_window, &w, &h);
	initialize_camera_position(w, h);

	reset_player_camera();

	initialize_message_prompt();
}

int Level::get_current_level() { return current_level; }

float Level::get_rotation() { return rotation; }

float Level::get_rotationDeg() { return rotationDeg; }

float Level::get_rotationEnergy() { return rotationEnergy; }

std::vector<Spider> Level::get_spiders() { return m_spiders.get_spider_vector(); }

std::vector<Harpy> Level::get_harpies() { return m_harpies.get_harpy_vector(); }

void Level::load_saved_game()
{
	fprintf(stderr, "loading saved game\n");

    Value& player = GameSave::document["player"];

    Value::ConstMemberIterator itr = player.GetObject().FindMember("alive");
    bool alive = itr->value.GetBool();

    if (!alive) {
        reset_game();
    } else {
        load_player();
        load_spiders();
        load_harpies();
    }

}

void Level::load_player()
{
	float player_x, player_y, player_scaleX, player_scaleY;
	bool alive;

	Value& player = GameSave::document["player"];

    Value::ConstMemberIterator itr = player.GetObject().FindMember("pos_x");
	player_x = itr->value.GetFloat();

	itr = player.GetObject().FindMember("pos_y");
	player_y = itr->value.GetFloat();

	itr = player.GetObject().FindMember("scale_x");
	player_scaleX = itr->value.GetFloat();

	player_scaleY = m_player.get_scale().y;

	m_player.set_position(vec2({player_x, player_y}));
	m_player.set_scale(vec2({player_scaleX, player_scaleY}));
	m_player.set_world_vertex_coord();

	initialPosition = m_player.get_position();

    int w, h;
    glfwGetWindowSize(m_window, &w, &h);
    initialize_camera_position(w, h);

    rotation = GameSave::document["rotation"].GetFloat();
    rotationDeg = GameSave::document["rotationDeg"].GetFloat();
    rotationEnergy = GameSave::document["rotationEnergy"].GetFloat();

    if (rotationEnergy < maxRotationEnergy)
    	m_water.set_rotation_end_time();
    
}

void Level::load_spiders()
{
	const Value& spiders = GameSave::document["spiders"];

	for (SizeType i = 0; i < spiders.Size(); i++) // Uses SizeType instead of size_t
	{
		float pos_x, pos_y, vel_x, vel_y, scale_x, scale_y;
		Value::ConstMemberIterator itr = spiders[i].GetObject().FindMember("pos_x");
		pos_x = itr->value.GetFloat();

		itr = spiders[i].GetObject().FindMember("pos_y");
		pos_y = itr->value.GetFloat();

		itr = spiders[i].GetObject().FindMember("vel_x");
		vel_x = itr->value.GetFloat();

		itr = spiders[i].GetObject().FindMember("vel_y");
		vel_y = itr->value.GetFloat();

		itr = spiders[i].GetObject().FindMember("scale_x");
		scale_x = itr->value.GetFloat();

		scale_y = m_spiders.get_spider_vector()[i].get_scale().y;

		vec2 position = vec2({ pos_x, pos_y });
		vec2 velocity = vec2({ vel_x, vel_y });
		vec2 scale = vec2({ scale_x, scale_y });

		m_spiders.setSpiderProperties(i, position, velocity, scale);
	}
}

void Level::load_harpies()
{
	const Value& harpies = GameSave::document["harpies"];

	for (SizeType i = 0; i < harpies.Size(); i++) // Uses SizeType instead of size_t
	{
		float pos_x, pos_y, vel_x, vel_y, scale_x, scale_y;
		Value::ConstMemberIterator itr = harpies[i].GetObject().FindMember("pos_x");
		pos_x = itr->value.GetFloat();

		itr = harpies[i].GetObject().FindMember("pos_y");
		pos_y = itr->value.GetFloat();

		itr = harpies[i].GetObject().FindMember("vel_x");
		vel_x = itr->value.GetFloat();

		itr = harpies[i].GetObject().FindMember("vel_y");
		vel_y = itr->value.GetFloat();

		itr = harpies[i].GetObject().FindMember("scale_x");
		scale_x = itr->value.GetFloat();

		scale_y = m_harpies.get_harpy_vector()[i].get_scale().y;

		vec2 position = vec2({ pos_x, pos_y });
		vec2 velocity = vec2({ vel_x, vel_y });
		vec2 scale = vec2({ scale_x, scale_y });
		
		m_harpies.setHarpyProperties(i, position, velocity, scale);
	}
}