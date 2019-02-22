#pragma once

#include "character.hpp"

class Player : public Character
{
public:
	// Creates all the associated render resources and default transform
	bool init(vec2 initialPosition, Physics* physicsHandler)override;
	
	// Update player position based on velocity vector
	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms)override;
	
	// Renders the player
	void draw(const mat3& projection)override;

	// Returns the local bounding coordinates scaled by the current size of the fish 
	vec2 get_bounding_box()const;

	// Let character know it's currently on a platform (and its position)
	void set_on_platform();

	// Let character know it's currently in freefall
	void set_in_free_fall();

	// Change player's state based on keyboard input
	void on_key(int key, int action);

	// Sets the correct sprite sheet tile for appropriate animation
	void set_animation();

	// Returns true if the character is in a state that can jump
	bool can_jump();


	bool isBelowPlatform;
	bool isLeftOfPlatform;
	bool isRightOfPlatform;
<<<<<<< HEAD
	bool isOnPlatform;
	double width;
	double height;

private:
	double tolerance = 0.000001;
	double maxVelocity = 12;
	double accStep = 2.f;
	double drag = 0.8;
	vec2 currentVelocity;
	vec2 currentAcceleration;
	//bool isOnPlatform;
	double fakeFloorPos = 750.f;
	double currentFloorPos = fakeFloorPos;
	double gravityAcc = 9.81 * 0.2;
	double jumpVel = -25.f;
=======
	float maxVelocity = 10;
	float accStep = 1.f;
	float jumpVel = -18.f;
	
>>>>>>> 5f111dc06446d8373ae494a1d1da1dadb680ea11
};
