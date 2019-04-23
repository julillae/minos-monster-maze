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
#include <iomanip>

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


Level::Level(Game* game) :
	m_seed_rng(0.f)
{
// Seeding rng with random device
	m_rng = std::default_random_engine(std::random_device()());
	this->game = game;
	soundManager = &game->soundManager;
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

	is_player_at_goal = false;
	int w, h;
	glfwGetFramebufferSize(m_window, &w, &h);
	glViewport(0, 0, w, h);

	current_level = startLevel;
	call_level_loader();

	w /= osScaleFactor;
	h /= osScaleFactor;
	initialize_camera_position(w, h);

 	m_fire.init();
	initialize_message_prompt();
	level_timer.init();

	m_timer_text.init(fonts_path("8bitwonder.ttf"), 40);
	m_energy_text.init(fonts_path("egyptian.ttf"), 40);

	m_timer_text.set_visibility(true);

	m_rotationUI.init();
	m_rotationUIEnergy.init();
	set_rotationUI_position();
	set_rotationUI_visibility(canRotate);
	set_lights();

	return m_fire.init() && m_player.init(initialPosition, physicsHandler);
}

void Level::check_platform_collisions(std::vector<Floor> nearbyFloorComponents, std::vector<Ice> nearbyIce, std::vector<Spike> nearbySpikes, std::vector<Blade> nearbyBlades) {
	if (m_player.is_alive()) {
		m_player.set_world_vertex_coord();
		physicsHandler->characterCollisionsWithSpikes(&m_player, nearbySpikes);
        physicsHandler->characterCollisionsWithFloors(&m_player, nearbyFloorComponents);
		physicsHandler->characterCollisionsWithIce(&m_player, nearbyIce);
		physicsHandler->characterCollisionsWithBlades(&m_player, nearbyBlades);

		if (!physicsHandler->isOnAtLeastOnePlatform) m_player.set_in_free_fall();

		if (!m_player.is_alive()) {
		    auto emitter = new Emitter(
		            m_player.get_position(),
		            70);
		    emitter->init();
		    m_emitters.emplace_back(emitter);
			set_death_effects();
		}

		physicsHandler->isOnAtLeastOnePlatform = false;
	}
}

// Releases all the associated resources
void Level::destroy()
{
	glDeleteFramebuffers(1, &m_frame_buffer);

	m_player.destroy();
	destroy_enemies();
	destroy_platforms();

	m_fire.destroy();
	m_timer_text.destroy();
	m_energy_text.destroy();


	glfwDestroyWindow(m_window);
}

// Update our game world
bool Level::update(float elapsed_ms)
{
	int w, h;
        glfwGetFramebufferSize(m_window, &w, &h);
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
			soundManager->fade_out_sound(rotationLoop, 200);	// hack to prevent minotaur from causing multiple rotation loop sounds
			soundManager->fade_in_sound_looped(rotationLoop, 500);
		}

		// Don't decrease the rotation energy if minotaur is rotating maze
		if (!minotaurPresent) {
			rotationEnergy -= fabs(rotationSpeed);
		}
	}
	else if (previouslyFrozen) {
		soundManager->fade_out_sound(rotationLoop, 200);
		applyThaw = true;
		previouslyFrozen = false;
		m_fire.set_rotation_end_time();
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

	if (minotaurPresent) {
		if (minotaur_prev_state != idle && m_minotaur.characterState->currentState == idle) {
			soundManager->play_sound_looped(minotaurIdle, -1);
		}
		if (minotaur_prev_state == idle && m_minotaur.characterState->currentState != idle) {
			soundManager->fade_out_sound(minotaurIdle, 0);
		}
		if (minotaur_prev_state != preparing && m_minotaur.characterState->currentState == preparing) {
			soundManager->play_sound(minotaurPrepare);
		}
		if (minotaur_prev_state != swinging && m_minotaur.characterState->currentState == swinging) {
			soundManager->play_sound_looped(minotaurAttack, 1);
		}
		minotaur_prev_state = m_minotaur.characterState->currentState;
		if (physicsHandler->collideWithEnemy(&m_player, &m_minotaur) &&
			 m_minotaur.characterState->currentState == swinging) {
			set_player_death();
		}
	}

//	 Checking Player - Exit Collision
	if (physicsHandler->collideWithExit(&m_player, &m_exit) && !is_player_at_goal)
	{
		soundManager->play_sound(levelComplete);
		m_fire.set_level_complete_time();
		is_player_at_goal = true;
		m_fire.set_success();
		m_player.freeze();
		m_player.set_invincibility(true);
		set_rotationUI_visibility(false);
		m_timer_text.set_visibility(false);

		if (hasPrompt)
			m_message.destroy();
	}

	vec2 play_pos = m_player.get_position();
	float play_radius = m_player.boundingCircleRadius;
	// create a copy of the floor vectors
	// get rid of all floors that are not in a certain radius
	std::copy_if(vector_of_floors.begin(), vector_of_floors.end(), back_inserter(nearbyFloors),
		[&](Floor const& v)
	{ return physicsHandler->outerCircleToCircleIntersection(play_pos, v.get_position(), play_radius, v.boundingCircleRadius); });

    if (!vector_of_ices.empty()) {
		std::copy_if(vector_of_ices.begin(), vector_of_ices.end(), back_inserter(nearbyIce),
			[&](Ice const& v)
		{ return physicsHandler->outerCircleToCircleIntersection(play_pos, v.get_position(), play_radius, v.boundingCircleRadius); });
    }

	if (!vector_of_spikes.empty()) {
		std::copy_if(vector_of_spikes.begin(), vector_of_spikes.end(), back_inserter(nearbySpikes),
			[&](Spike const& v)
		{ return physicsHandler->outerCircleToCircleIntersection(play_pos, v.get_position(), play_radius, v.boundingCircleRadius); });
	}

	vector<Blade> vector_of_blades = m_blades.get_blade_vector();
	if (!vector_of_blades.empty()) {
		std::copy_if(vector_of_blades.begin(), vector_of_blades.end(), back_inserter(nearbyBlades),
			[&](Blade const& v)
		{ return physicsHandler->outerCircleToCircleIntersection(play_pos, v.get_position(), play_radius, v.boundingCircleRadius); });
	}

    // checking player - platform collision with nearby floors
	check_platform_collisions(nearbyFloors, nearbyIce, nearbySpikes, nearbyBlades);
	nearbyFloors.clear();
	nearbyIce.clear();
	nearbySpikes.clear();
	nearbyBlades.clear();

	// update particle emitters

	for (auto& emitter : m_emitters) {
	    emitter->update(elapsed_ms);
	}

	// delete dead particles
    for (auto it = m_emitters.begin(); it != m_emitters.end();) {
        if ((*it)->get_alive_particles() == 0) {
            delete *it;
            it = m_emitters.erase(it);
        }
        else {
            ++it;
        }
    }

	if (applyFreeze) {
		m_player.freeze();
		m_blades.freeze();
		freeze_all_enemies();
		rotationStart = rotation;
	}
	else if (applyThaw) {
		float rotateVelVec = rotation - rotationStart;
		physicsHandler->updateCharacterVelocityRotation(&m_player, rotateVelVec);
		m_player.unfreeze();
		m_blades.unfreeze();
		unfreeze_all_enemies();
	}
	if (m_player.characterState->currentState == jumping)
		soundManager->play_sound(playerJump);
	m_player.update(elapsed_ms);

	update_all_enemies(elapsed_ms);
	update_all_platforms(elapsed_ms);

	// If player is dead or beat the game, restart the game after the fading animation
	if (!m_player.is_alive() && m_fire.get_time_since_death() > 1.5)
		reset_game();

	if (m_player.is_alive() && is_player_at_goal && m_fire.get_time_since_level_complete() > 1.5)
		reset_game();

	float timeToLoadRotationEnergy = 4.f;
	if (m_fire.get_time_since_rotation_end() > timeToLoadRotationEnergy) {
		rotationEnergy += rotationEnergyIncrement;
		m_fire.set_rotation_end_time();

		if (rotationEnergy >= maxRotationEnergy) {
			m_fire.reset_rotation_end_time();
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
			shrinkingTetherVector = scalarMultiply(deviationVector, 0.1f);
		}
		cameraCenter = add(prevCameraCenter, shrinkingTetherVector);
		prevCameraCenter = cameraCenter;
	}
	tx = -cameraCenter.x;
	ty = -cameraCenter.y;

	float c = cosf(-rotation);
	float s = sinf(-rotation);

	mat3 projection_noRotation;

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

	projection_noRotation = projection_2D;

	projection_2D = mul(projection_2D, rotation_matrix);
	projection_2D = mul(projection_2D, translation_matrix);

    projection_noRotation = mul(projection_noRotation, translation_matrix);

	draw_platforms(projection_2D);
	draw_enemies(projection_2D);
	m_exit.draw(projection_2D);
	m_player.draw(projection_2D);
	for (auto& emitter: m_emitters) {
	    emitter->draw(projection_2D);
	}

    render_to_screen(w, h);

	m_fire.draw(projection_2D);

	if (hasPrompt) {
		float screen_height = static_cast<float>(h)/osScaleFactor;
		float message_y_shift = (screen_height / 2.f) - (m_tile_height * 4.f);
		float message_y_pos = cameraCenter.y - message_y_shift;
		m_message.set_position({cameraCenter.x, message_y_pos});
		m_message.draw(projection_noRotation);
	}

	vec2 deviationVector2 = add(p_position, negateVec(cameraCenter));

	deviationVector2 = rotateVec(deviationVector2, -rotation);
	m_fire.originUpdate(w, h, deviationVector2.x*osScaleFactor, -deviationVector2.y*osScaleFactor);

    update_rotationUI();
	m_rotationUI.draw(projection_noRotation);
	m_rotationUIEnergy.draw(projection_noRotation);

	set_timer_text_position();
    stringstream stream;
    stream << fixed << setprecision(0) << level_timer.getTime();
    m_timer_text.render(projection_noRotation, "TIME " + stream.str());

    draw_energyText(projection_noRotation);

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
			soundManager->fade_out_sound(rotationLoop, 200);
			soundManager->fade_out_sound(minotaurIdle, 0);
			m_player.set_direction(Direction::none);
			timer_pause_start = level_timer.getTime();

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

}

void Level::on_mouse_move(GLFWwindow* window, double xpos, double ypos)
{

}

void Level::initialize_message_prompt() {
	if (hasPrompt) {
		int messageNumber = current_level;
		if (current_level == 5) {
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
		float txOffset = static_cast<float> (w / 2);
		float tyOffset = static_cast<float> (h / 2);
		cameraCenter = vec2({ txOffset, tyOffset});
	}
}

void Level::draw_enemies(mat3 projection_2D) {
	m_spiders.draw(projection_2D);
	m_harpies.draw(projection_2D);
	if (minotaurPresent) {
		m_minotaur.draw(projection_2D);
	}
}

void Level::reset_enemies() {
	m_spiders.reset();
	m_harpies.reset();
	if (minotaurPresent) {
		m_minotaur.freeze();
		m_minotaur.reset_position();
		m_minotaur.unfreeze();
		m_minotaur.characterState->changeState(idle);
	}
}

void Level::destroy_enemies() {
	m_spiders.destroy();
	m_harpies.destroy();
	if (minotaurPresent) {
		m_minotaur.stopRotating();
		m_minotaur.destroy();
	}
}

void Level::draw_platforms(mat3 projection_2D) {
	m_floors.draw(projection_2D);
	m_ice.draw(projection_2D);
	m_spikes.draw(projection_2D);
	m_blades.draw(projection_2D);
}

void Level::destroy_platforms() {
	m_floors.destroy();
	m_spikes.destroy();
	m_ice.destroy();
	m_blades.destroy();
}

void Level::load_intro()
{
	int w, h;
    glfwGetFramebufferSize(m_window, &w, &h);
	vec2 screen = { (float)w, (float)h };

	if (!introLoaded) {
		introState = new IntroState(game);
		introState->init(screen);
		game->push_state(introState);
		introLoaded = true;
	}

	game->set_current_state(introState);
}

void Level::load_minotaur_intro()
{
	int w, h;
    glfwGetFramebufferSize(m_window, &w, &h);
	vec2 screen = { (float)w, (float)h };

	MinotaurIntroState* introState = new MinotaurIntroState(game);
	introState->init(screen);

	game->push_state(introState);
	game->set_current_state(introState);
}

void Level::load_credits()
{
	int w, h;
    glfwGetFramebufferSize(m_window, &w, &h);
	vec2 screen = { (float)w, (float)h };

	CreditsState* creditsState = new CreditsState(game);
	creditsState->init(screen);

	game->push_state(creditsState);
	game->set_current_state(creditsState);
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
	hasLightingEffect = levelLoader.has_lighting_effect();

	initialPosition = levelLoader.get_player_position();
	m_exit = levelLoader.get_exit();

	m_spiders = levelLoader.get_spiders();
	m_harpies = levelLoader.get_harpies();

	minotaurPresent = levelLoader.minotaurInLevel();
	if (minotaurPresent) {
		m_minotaur = levelLoader.get_minotaur();
	}

	m_floors = levelLoader.get_floors();
	m_ice = levelLoader.get_ice();
	m_spikes = levelLoader.get_spikes();
	m_blades = levelLoader.get_blades();

    vector_of_floors = m_floors.get_floor_vector();
    vector_of_ices = m_ice.get_ice_vector();
	vector_of_spikes = m_spikes.get_spike_vector();
}

void Level::load_new_level()
{
	clear_resources();

	current_level++;
	if (current_level >= num_levels) {
		level_timer.resetCumulativeTime();

		load_credits();
	} else if (current_level == minotaur_level) {
		load_minotaur_intro();
	} else {
		level_timer.addCumulativeTime(level_timer.getTime());
		call_level_loader();
		initialize_message_prompt();
		set_rotationUI_visibility(canRotate);
        set_lights();
		// if moved on to new level, reset saved time to zero.
		level_timer.recordSavedTime(0.f);
		level_timer.reset();
		m_timer_text.set_visibility(true);
	}

}

void Level::reset_game()
{
	int w, h;
	glfwGetWindowSize(m_window, &w, &h);
	// if the player died, don't want to reset timer stuff
	if (m_player.is_alive()) {
		reset_pause_start();
		level_timer.cleanSlate();
	}
	m_player.destroy();
	set_rotationUI_visibility(canRotate);

	if (is_player_at_goal) {
		load_new_level();
		initialize_camera_position(w, h);
	} else {
		reset_enemies();
	}
	soundManager->fade_out_sound(rotationLoop, 200);
	if (minotaurPresent) soundManager->fade_out_sound(minotaurIdle, 0);
	reset_player_camera();
	m_fire.reset_fire();
	initialize_message_prompt();
	m_timer_text.set_visibility(true);
}

void Level::reset_player_camera()
{
	m_player.init(initialPosition, physicsHandler);

	m_fire.reset_rotation_end_time();
	m_fire.reset_player_win_time();
    m_fire.reset_player_dead_time();
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
	if (minotaurPresent){
		m_minotaur.freeze();
	}
}

void Level::unfreeze_all_enemies()
{
	m_spiders.unfreeze();
	m_harpies.unfreeze();
	if (minotaurPresent){
		m_minotaur.unfreeze();
	}
}

void Level::update_all_enemies(float elapsed_ms)
{
	m_spiders.update(elapsed_ms);
	m_harpies.update(elapsed_ms);
	if (minotaurPresent){
		m_minotaur.update(elapsed_ms);
	}

}

void Level::update_all_platforms(float elapsed_ms)
{
	m_blades.update();
}

void Level::update_rotationUI()
{

	float prev_energyPercent = m_rotationUIEnergy.get_energy_level();
	float energyPercent = rotationEnergy / maxRotationEnergy;
	m_rotationUIEnergy.set_energy_level(energyPercent);
	set_rotationUI_position();

	if (energyPercent != prev_energyPercent) {
		m_rotationUIEnergy.update(energyPercent);
	}

}

void Level::set_rotationUI_position()
{
	int w, h;
	glfwGetFramebufferSize(m_window, &w, &h);
	w /= osScaleFactor;
	h /= osScaleFactor;

	vec2 newPosition = vec2({cameraCenter.x - w/2 + m_rotationUI.get_width() / 2 + 34 , cameraCenter.y + h/2 - 80 });
	m_rotationUI.set_position(newPosition);
	m_rotationUIEnergy.set_position(vec2({newPosition.x, newPosition.y + 7 }));
}

void Level::set_rotationUI_visibility(bool visible)
{
	m_rotationUI.set_visibility(visible);
	m_rotationUIEnergy.set_visibility(visible);
	m_energy_text.set_visibility(visible);

}

void Level::set_timer_text_position()
{
    int w, h;
    glfwGetFramebufferSize(m_window, &w, &h);
    w /= osScaleFactor;
    h /= osScaleFactor;

	vec2 newPosition = vec2({cameraCenter.x + w/2 - 150 , cameraCenter.y - h/2 + 30 });
    m_timer_text.setPosition(newPosition);
}

void Level::draw_energyText(mat3 projection_2D)
{
	int w, h;
	glfwGetFramebufferSize(m_window, &w, &h);
	w /= osScaleFactor;
	h /= osScaleFactor;

	vec2 newPosition = vec2({cameraCenter.x - w/2 + 145 , cameraCenter.y + h/2 - 30 });
	m_energy_text.setPosition(newPosition);

	stringstream energyStream, maxEnergyStream;

	// Round energy to two decimal places for printing
	float roundedEnergy = roundf(rotationEnergy * 100.f) / 100.f;

	energyStream << fixed << setprecision(0) << roundedEnergy;

	// adjust spacing when numbers get lower than 3 digits
	if (roundedEnergy < 10) {
		m_energy_text.render(projection_2D, "     " + energyStream.str());
	} else if (roundedEnergy < 100){
		m_energy_text.render(projection_2D, "  " + energyStream.str());

	} else {
		m_energy_text.render(projection_2D, energyStream.str());
	}

	newPosition = vec2({cameraCenter.x - w/2 + 185, cameraCenter.y + h/2 - 30});
	m_energy_text.setPosition(newPosition);
	maxEnergyStream << fixed << setprecision(0) << maxRotationEnergy;
	m_energy_text.render(projection_2D, "  / " + maxEnergyStream.str());
}

Level::Platform Level::maze_is_platform(std::pair<int,int> coords){
	Platform platform = Platform{};
	int val_at_coords = m_maze[coords.first][coords.second];
	for (auto& p : platform_types) {
		if (val_at_coords == p.first){
			platform.isPlatform = true;
			platform.platformType = p.first;
			return platform;
		}
	}
	platform.isPlatform = false;
	platform.platformType = 0;
	return platform;
}

std::vector<std::vector <int>> Level::get_original_maze() {
	return m_maze;
}

float Level::get_maze_width() { return m_maze_width; }

float Level::get_maze_height() { return m_maze_height; }

float Level::get_tile_width() { return m_tile_width; }

float Level::get_tile_height() { return m_tile_height; }

void Level::set_player_death()
{
	if (!m_player.is_invincible() && m_player.is_alive()) {
		m_player.kill();
		m_fire.set_player_dead();
		// emitter for blood
        auto emitter = new Emitter(
                m_player.get_position(),
                70);
        emitter->init();
        m_emitters.emplace_back(emitter);
		set_death_effects();
	}
}

void Level::set_death_effects()
{
	if (hasPrompt)
		m_message.destroy();

	set_rotationUI_visibility(false);
	m_timer_text.set_visibility(false);

	soundManager->play_sound(playerDead);
	m_fire.set_player_dead_time();
}

void Level::load_select_level(int level)
{
	clear_resources();

	current_level = level;
	call_level_loader();

	int w, h;
	glfwGetWindowSize(m_window, &w, &h);
	initialize_camera_position(w, h);

	reset_player_camera();

	initialize_message_prompt();
	m_timer_text.set_visibility(true);
	set_rotationUI_visibility(canRotate);
	set_lights();
	reset_pause_start();
	level_timer.cleanSlate();
}

int Level::get_current_level() { return current_level; }

float Level::get_rotation() { return rotation; }

float Level::get_rotationDeg() { return rotationDeg; }

float Level::get_rotationEnergy() { return rotationEnergy; }

float Level::get_level_time() { return level_timer.getTime(); }

std::vector<Spider> Level::get_spiders() { return m_spiders.get_spider_vector(); }

std::vector<Harpy> Level::get_harpies() { return m_harpies.get_harpy_vector(); }

Minotaur Level::get_minotaur() { return m_minotaur; }

std::vector<Floor> Level::get_floors() { return m_floors.get_floor_vector(); }

std::vector<Blade> Level::get_blades() { return m_blades.get_blade_vector();}

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
        load_rotation_energy();
        load_spiders();
        load_harpies();
		if (minotaurPresent) {
			load_minotaur();
		}
		load_blades();
    }

    if (hasPrompt) {
        m_message.set_visibility(false);
    }

}

void Level::load_player()
{
	float player_x, player_y, player_scaleX, player_scaleY;

	Value& player = GameSave::document["player"];

    Value::ConstMemberIterator itr = player.GetObject().FindMember("pos_x");
	player_x = itr->value.GetFloat();

	itr = player.GetObject().FindMember("pos_y");
	player_y = itr->value.GetFloat();

	itr = player.GetObject().FindMember("scale_x");
	player_scaleX = itr->value.GetFloat();

	player_scaleY = m_player.get_scale().y;

    vec2 originalPosition = m_player.get_position();

	m_player.set_position(vec2({player_x, player_y}));
	m_player.set_scale(vec2({player_scaleX, player_scaleY}));
	m_player.set_world_vertex_coord();

	initialPosition = m_player.get_position();

    int w, h;
    glfwGetWindowSize(m_window, &w, &h);
    initialize_camera_position(w, h);

	level_timer.recordSavedTime(GameSave::document["levelTime"].GetFloat());

    // reset initialPosition for restarting game
    initialPosition = originalPosition;

}

void Level::load_minotaur()
{
	float minotaur_x, minotaur_y, minotaur_scaleX, minotaur_scaleY, minotaur_velx;

	Value& minotaur = GameSave::document["minotaur"];

    Value::ConstMemberIterator itr = minotaur.GetObject().FindMember("pos_x");
	minotaur_x = itr->value.GetFloat();

	itr = minotaur.GetObject().FindMember("pos_y");
	minotaur_y = itr->value.GetFloat();

	itr = minotaur.GetObject().FindMember("scale_x");
	minotaur_scaleX = itr->value.GetFloat();

	minotaur_scaleY = m_minotaur.get_scale().y;

	itr = minotaur.GetObject().FindMember("velocity_x");
	minotaur_velx = itr->value.GetFloat();

	m_minotaur.set_velocity(vec2{minotaur_velx, 0.f});

	m_minotaur.set_position(vec2({minotaur_x, minotaur_y}));
	m_minotaur.set_scale(vec2({minotaur_scaleX, minotaur_scaleY}));
	minotaur_prev_state = initialized;
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

void Level::load_blades()
{
	const Value& blades = GameSave::document["blades"];

	for (SizeType i = 0;  i < blades.Size(); i++)
	{
		float rotation = blades[i].GetFloat();

		m_blades.setBladeProperties(i, rotation);
	}
}

void Level::load_rotation_energy()
{
	rotation = GameSave::document["rotation"].GetFloat();
	rotationDeg = GameSave::document["rotationDeg"].GetFloat();
    rotationEnergy = GameSave::document["rotationEnergy"].GetFloat();
    if (rotationEnergy < maxRotationEnergy)
        m_fire.set_rotation_end_time();

}

void Level::boss_rotation_set(bool enable, bool cw)
{
	isRotating = enable;
	rotateCW = cw;
}

float Level::get_pause_start() {
	return timer_pause_start;
}

void Level::return_from_pause() {
	level_timer.recordPausedTime(timer_pause_start, level_timer.getTime());
	if (minotaurPresent && m_minotaur.characterState->currentState == idle) {
		soundManager->play_sound_looped(minotaurIdle, -1);
	}
	reset_pause_start();
}

void Level::reset_pause_start() {
	timer_pause_start = -1.0f;
}

void Level::clear_resources() {
	m_player.destroy();
    destroy_platforms();
    destroy_enemies();
    vector_of_floors.clear();
    vector_of_ices.clear();
	vector_of_spikes.clear();
    m_maze.clear();
    nearbyFloors.clear();
    nearbyIce.clear();
	nearbySpikes.clear();
	nearbyBlades.clear();
}

void Level::set_lights(){

	m_fire.set_light_mode(hasLightingEffect);
	set_UI_colour();
}

void Level::set_UI_colour()
{
	vec3 colour;
    if (hasLightingEffect) {
    	colour = {1.f, 1.f, 1.f};

    } else {
    	colour = {0.f, 0.f, 0.f};
    }

	m_timer_text.setColour(colour);
	m_energy_text.setColour(colour);

	m_rotationUI.set_colour_mode(hasLightingEffect);
}
