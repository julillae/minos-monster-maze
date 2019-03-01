// Header
#include "../include/level.hpp"
#include "../include/physics.hpp"

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
	int increment = 0;
	float rotation = 0.f;
	bool isRotating = false;
	bool previouslyFrozen = false;
    void glfw_err_cb(int error, const char* desc)
    {
        fprintf(stderr, "%d: %s", error, desc);
    }
}


Level::Level() : m_seed_rng(0.f)
{
// Seeding rng with random device
	m_rng = std::default_random_engine(std::random_device()());
}

Level::~Level()
{

}

void Level::read_level_data() {
	std::string level = std::to_string(current_level);
	fprintf(stderr, "Loading level %s\n", level.c_str());
	std::string fileName = "levels/level" + level + ".txt";
    std::ifstream filein(fileName);

    for (std::string line; std::getline(filein, line);) {
        std::vector <int> row;
        for(char& c : line) {
            // Covert char to int and push to row
            row.push_back(c - '0');
        }
        // Push row to maze array
        m_maze.push_back(row);
    }
}

bool Level::spawn_spider_enemy(vec2 position, float bound)
{
	Spider enemy;
	if (enemy.init(position, physicsHandler))
	{
		enemy.set_bound(bound);
		m_enemies.emplace_back(enemy);
		return true;
	}
	fprintf(stderr, "Failed to spawn enemy");
	return false;
}

bool Level::spawn_floor(vec2 position)
{
	Floor floor;
	if (floor.init(position))
	{
		m_floor.emplace_back(floor);
		return true;
	}
	fprintf(stderr, "Failed to spawn floor");
	return false;
}

// Generates maze
void Level::generate_maze()
{
	fprintf(stderr, "Generating maze\n");
	const float initial_x = 40.0;
	const float initial_y = 30.0;
	// Initial tile
	spawn_floor({initial_x, initial_y});

	float tile_width = 0.f;
	float tile_height = 0.f;
	
	bool setting_enemy = false;
	vec2 enemy_start_pos;

    float i = 0.f; 
	float j = 0.f;

	for (auto &row : m_maze) {
        j = 0.f;
		for (int &cell : row) {	

			float x_pos = (j * tile_width) + initial_x;
			float y_pos = (i * tile_height) + initial_y;

			if (setting_enemy && cell != 4) {
				// If we were setting enemy positions, and we hit a cell with no enemy,
				// Spawn the enemy we were setting

				float last_x_pos = x_pos - tile_width;
				float distance = abs(last_x_pos - enemy_start_pos.x);
				spawn_spider_enemy(enemy_start_pos, distance);
				setting_enemy = false;
			}

			if (cell == 1) {
				// Spawn platform
				MazeComponent& new_floor = m_floor.back();	

				// Assuming all tiles are the same size, we only need to grab these values once
				if (tile_width == 0.f || tile_height == 0.f) {
					tile_width = new_floor.get_width();
					tile_height = new_floor.get_height();

					// Fix x and y positions if tile_width was zero
					x_pos = (j * tile_width) + initial_x;
					y_pos = (i * tile_height) + initial_y;
				}

				spawn_floor({x_pos, y_pos});
			} else if (cell == 2) {
				// Add exit
				Exit new_exit;

				new_exit.init({x_pos, y_pos});

				m_exit = new_exit;
			} else if (cell == 3) {
				// Set initial position of player
				initialPosition = {x_pos, y_pos};
			} else if (cell == 4) {
				// Begin setting enemy path
				if (!setting_enemy) {
					setting_enemy = true;
					enemy_start_pos = {x_pos, y_pos};
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

// Level initialization
bool Level::init(vec2 screen, Physics* physicsHandler, int startLevel)
{
	this->physicsHandler = physicsHandler;

	//-------------------------------------------------------------------------
	// GLFW / OGL Initialization
	// Core Opengl 3.
	glfwSetErrorCallback(glfw_err_cb);
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_RESIZABLE, 0);
	m_window = glfwCreateWindow((int)screen.x, (int)screen.y, "A1 Assignment", nullptr, nullptr);
	if (m_window == nullptr)
		return false;

	// hack used to make sure the display for macOS with retina display issue is consistent with display on other systems
	int testWidth;
	glfwGetFramebufferSize(const_cast<GLFWwindow *>(m_window), &testWidth, nullptr);
	osScaleFactor = testWidth / screen.x;

	glfwMakeContextCurrent(m_window);
	glfwSwapInterval(1); // vsync

	// Load OpenGL function pointers
	gl3w_init();

	// Setting callbacks to member functions (that's why the redirect is needed)
	// Input is handled using GLFW, for more info see
	// http://www.glfw.org/docs/latest/input_guide.html
	glfwSetWindowUserPointer(m_window, this);
	auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((Level*)glfwGetWindowUserPointer(wnd))->on_key(wnd, _0, _1, _2, _3); };
	auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((Level*)glfwGetWindowUserPointer(wnd))->on_mouse_move(wnd, _0, _1); };
	glfwSetKeyCallback(m_window, key_redirect);
	glfwSetCursorPosCallback(m_window, cursor_pos_redirect);

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
	m_salmon_dead_sound = Mix_LoadWAV(audio_path("salmon_dead.wav"));

	if (m_background_music == nullptr)
	{
		fprintf(stderr, "Failed to load sound\n %s,%s\n make sure the data directory is present",
			audio_path("salmon_dead.wav"),
			audio_path("secret_catacombs.wav"));
		return false;
	}

	// Playing background music undefinitely
	Mix_PlayMusic(m_background_music, -1);
	
	fprintf(stderr, "Loaded music\n");

	is_player_at_goal = false;
	int w, h;
	glfwGetFramebufferSize(m_window, &w, &h);
	glViewport(0, 0, w, h);
	float left = 0.f;// *-0.5;
	float right = (float)w;// *0.5;
	prev_tx = -(700.f*2.f)/(right - left);

	current_level = startLevel;
    read_level_data();
	generate_maze();

	m_help_menu.init(initialPosition);
	
	return m_water.init() && m_player.init(initialPosition, physicsHandler);
}

// Releases all the associated resources
void Level::destroy()
{
	glDeleteFramebuffers(1, &m_frame_buffer);

	if (m_background_music != nullptr)
		Mix_FreeMusic(m_background_music);
	if (m_salmon_dead_sound != nullptr)
		Mix_FreeChunk(m_salmon_dead_sound);

	Mix_CloseAudio();

	m_player.destroy();
	for (auto& enemy : m_enemies)
		enemy.destroy();
	for (auto& floor : m_floor)
		floor.destroy();
	m_enemies.clear();
	m_floor.clear();
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
	if (isRotating && !previouslyFrozen) {
		applyFreeze = true;
		previouslyFrozen = true;
	}
	else if (!isRotating && previouslyFrozen) {
		applyThaw = true;
		previouslyFrozen = false;
	}

	// Checking Player - Enemy Collision
	for (Enemy& enemy : m_enemies) {
		if (physicsHandler->collideWithEnemy(&m_player, &enemy).isCollided)
		{
			if (!m_player.is_invincible() && m_player.is_alive()) {
				Mix_PlayChannel(-1, m_salmon_dead_sound, 0);
				m_player.kill();
				m_water.set_player_dead();
			}
		}
	}

//	 Checking Player - Exit Collision
	if (physicsHandler->collideWithExit(&m_player, &m_exit).isCollided && !is_player_at_goal)
	{
		m_water.set_level_complete_time();
		is_player_at_goal = true;
		m_player.set_invincibility(true);
	}

	physicsHandler->characterCollisionsWithFixedComponents(&m_player, m_floor);
	m_player.set_rotation(rotation);
	if (applyFreeze) {
		m_player.freeze();
	}
	else if (applyThaw) {
		physicsHandler->updateCharacterVelocityRotation(&m_player);
		m_player.unfreeze();
	}
	m_player.update(elapsed_ms);

	for (auto& enemy : m_enemies) {
		if (applyFreeze) {
			enemy.freeze();
		} else if (applyThaw) {
			enemy.unfreeze();
		}
		enemy.update(elapsed_ms);
	}

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
	const float clear_color[3] = { 0.f, 0.f, 1.f };

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
	
	float tx = 0.f;
	//float ty = 0.f;
	bool cameraTracking = true;
	//float prev_ty = -(2*p_position.y)/(top - bottom);
	if (cameraTracking){
		acc_rotate = acc_rotate-rotation;

	while (abs(acc_rotate)>360.f){
		if(acc_rotate>0){
			acc_rotate=acc_rotate-360.f;
		}else{
			acc_rotate = acc_rotate+360.f;
		}
	}

		// translation if camera tracks player
		rotateVec(p_position, -rotation);
		if (m_player.isOnPlatform) {
			float target = -p_position.y;
			float difference = target - prev_ty;
			float delta = difference * 0.1f;
			ty = prev_ty + delta;
			prev_ty = ty;
		}
		else {
			ty = prev_ty;
		}

		float tem_x = -p_position.x;
		if (tem_x > rightbound) {
			float range = 100.f;
			rightbound = tem_x;
			leftbound = rightbound - range;
			tx = rightbound - range / 2.f;
			prev_tx = tx;
		}
		else if (tem_x < leftbound) {
			float range = 100.f;
			leftbound = tem_x;
			rightbound = leftbound + range;
			tx = leftbound + range / 2.f;
			prev_tx = tx;
			if (abs(acc_rotate)>=45.f && abs(acc_rotate)<=135.f){
				float tmp = tx;
				tx = ty;
			}
		}
		else {
			tx = prev_tx;
		}
		rotateVec(p_position, rotation);
		if ((abs(acc_rotate)>=45.f && abs(acc_rotate)<=135.f) || (abs(acc_rotate)>=225.f && abs(acc_rotate)<=315.f)){
			if (m_player.isOnPlatform) {
				float target = -p_position.x;
				float difference = target - prev_tx;
				float delta = difference * 0.1f;
				tx = prev_tx + delta;
				prev_tx = tx;
			}
			else {
				tx = prev_tx;
			}

			float tem_y = -p_position.y;
			if (tem_y > rightbound) {
				float range = 100.f;
				rightbound = tem_y;
				leftbound = rightbound - range;
				ty = rightbound - range / 2.f;
				prev_ty = ty;
			}
			else if (tem_y < leftbound) {
				float range = 100.f;
				leftbound = tem_y;
				rightbound = leftbound + range;
				ty = leftbound + range / 2.f;
				prev_ty = ty;
			}else {
			ty = prev_ty;
		}
		}
		
		}
	else {
		// translation for fixed camera
		tx = -(right + left)/2;
		ty = -(top + bottom)/2;
	}

	vec2 rot = rotateVec({tx,ty}, rotation);
	tx = rot.x;
	ty = rot.y;

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

    for (auto& floor : m_floor)
		floor.draw(projection_2D);
	for (auto& enemy : m_enemies)
		enemy.draw(projection_2D);
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
bool Level::is_over()const
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

	if (!show_help_menu)
	{
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			if (key == GLFW_KEY_Z) {
				isRotating = true;
				increment = (increment + 1) % 360;
			}
			if (key == GLFW_KEY_X) {
				isRotating = true;
				increment = (increment - 1) % 360;
			}
			rotation = static_cast<float>((increment * M_PI) / 180);
		}
		else if (action == GLFW_RELEASE && (key == GLFW_KEY_Z || key == GLFW_KEY_X)) {
			isRotating = false;
		}
	}

	if (action == GLFW_PRESS && key == GLFW_KEY_H) {
		show_help_menu = !show_help_menu;
		if (show_help_menu) {
			for(Enemy& e : m_enemies) {
				e.freeze();
			}
			m_help_menu.set_position(m_player.get_position());
			m_player.freeze();
		} else {
			for(Enemy& e : m_enemies) {
				e.unfreeze();
			}
			m_player.unfreeze();
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

void Level::load_new_level()
{
	for (auto& floor : m_floor)
		floor.destroy();

	for (auto& enemy : m_enemies)
		enemy.destroy();
	
	m_floor.clear();
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
	
	if (is_player_at_goal)
		load_new_level();
	else
		for (Enemy& enemy : m_enemies) {
			enemy.reset_position();
			enemy.unfreeze();
		};

	m_player.init(initialPosition, physicsHandler);

	m_water.reset_player_win_time();
	m_water.reset_player_dead_time();
	is_player_at_goal = false;
	increment = 0;
	rotation = 0.f;
	previouslyFrozen = false;
}

void Level::delay(float secs)
{
	float end = clock()/CLOCKS_PER_SEC + secs;
	while((clock()/CLOCKS_PER_SEC) < end);
}
