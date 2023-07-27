/**
 CPlayer2D
 @brief A class representing the player object
 By: Toh Da Jun
 Date: Mar 2020
 */
#pragma once

// Include Singleton template
#include "DesignPatterns\SingletonTemplate.h"

// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

// Include CEntity2D
#include "Primitives/Entity2D.h"

// Include the Map2D as we will use it to check the player's movements and actions
class CMap2D;

// Include Keyboard controller
#include "Inputs\KeyboardController.h"

// Include Physics2D
#include "Physics2D.h"

// Include AnimatedSprites
#include "Primitives/SpriteAnimation.h"

// Include InventoryManager
#include "InventoryManager.h"

// Include SoundController
#include "..\SoundController\SoundController.h"

class CPlayer2D : public CSingletonTemplate<CPlayer2D>, public CEntity2D
{
	friend CSingletonTemplate<CPlayer2D>;
public:

	// Init
	bool Init(void);

	// Reset
	bool Reset(void);

	// Respawn
	bool Respawn(void);

	// Update
	bool Update(const double dElapsedTime);

	// PreRender
	void PreRender(void);

	// Render
	void Render(void);

	// PostRender
	void PostRender(void);

protected:
	// Handler to the CSoundController
	CSoundController* cSoundController;

	const glm::vec2 vec2JumpSpeed = glm::vec2(0.0f, 150.0f);
	const glm::vec2 vec2WalkSpeed = glm::vec2(50.0f, 50.0f);

	const glm::vec2 vec2HalfSize = glm::vec2(9.f, 18.75f);
	glm::vec2 vec2MovementVelocity;

	// Char to store player sprite
	const char* playerTexture = "Image/redhood_spritesheet.png";

	// Timer to check how long it has been since the player last input any movement
	double idleTimer;

	// Char to store previous directional input (for animation)
	char lastInputHorizontal;

	// Char to store keys being pressed while dashing
	char keyPressedWhileDashingHorizontal;
	char keyPressedWhileDashingVertical;

	// int to store number of jumps
	int maxJumps = 2;
	int jumps = 0;

	// Float for how much time is left in dash and bool for whether the player can dash
	float dashTime = 0;
	bool canDash = true;

	// Vec2 to store the last position the player was idle at
	glm::vec2 lastIdlePosition;


	// Bool to store whether the player got boosted or not
	bool boosted = false;
	bool boostable = true;
	char boostDirection;

	// Double to store jump boost timer
	double jumpBoostTimer = 0;

	//glm::vec2 vec2OldIndex;

	// Handler to the CMap2D instance
	CMap2D* cMap2D;

	// Keyboard Controller singleton instance
	CKeyboardController* cKeyboardController;

	// Physics
	CPhysics2D cPhysics2D;

	//CS: Animated Sprite
	CSpriteAnimation* animatedSprites;

	// Current colour
	glm::vec4 vec4ColourTint;

	// InventoryManager
	CInventoryManager* cInventoryManager;
	// InventoryItem
	CInventoryItem* cInventoryItem;

	// Constructor
	CPlayer2D(void);

	// Destructor
	virtual ~CPlayer2D(void);

	// Check if the player is in mid-air
	//bool IsMidAir(void);

	//// Update Jump or Fall
	//void UpdateJumpFall(const double dElapsedTime = 0.0166666666666667);

	// Let player interact with the map
	void InteractWithMap(void);

	// Update the health and lives
	void UpdateHealthLives(void);

	// Boost player
	void BoostPlayer(const double dElapsedTime);
};

