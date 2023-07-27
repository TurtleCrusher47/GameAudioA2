/**
 Player2D
 @brief A class representing the player object
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "Player2D.h"

#include <iostream>
using namespace std;

// Include Shader Manager
#include "RenderControl\ShaderManager.h"

// Include ImageLoader
#include "System\ImageLoader.h"

// Include the Map2D as we will use it to check the player's movements and actions
#include "Map2D.h"
#include "Primitives/MeshBuilder.h"

// Include Game Manager
#include "GameManager.h"

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CPlayer2D::CPlayer2D(void)
	: cMap2D(NULL)
	, cKeyboardController(NULL)
	, cInventoryManager(NULL)
	, cInventoryItem(NULL)
{
	// Initialise position of the player
	vec2Position = glm::vec2(0);
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CPlayer2D::~CPlayer2D(void)
{
	// We won't delete this since it was created elsewhere
	cInventoryManager = NULL;

	// We won't delete this since it was created elsewhere
	cKeyboardController = NULL;

	// We won't delete this since it was created elsewhere
	cMap2D = NULL;

	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
}

/**
  @brief Initialise this instance
  */
bool CPlayer2D::Init(void)
{
	// Load the sounds to CSoundController
	cSoundController = CSoundController::GetInstance();

	// Store the keyboard controller singleton instance here
	cKeyboardController = CKeyboardController::GetInstance();
	// Reset all keys since we are starting a new game
	cKeyboardController->Reset();

	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();

	// Get the handler to the CMap2D instance
	cMap2D = CMap2D::GetInstance();
	// Find the indices for the player in arrMapInfo, and assign it to cPlayer2D
	unsigned int uiRow = -1;
	unsigned int uiCol = -1;
	if (cMap2D->FindValue(200, uiRow, uiCol) == false)
		return false;	// Unable to find the start position of the player, so quit this game

	// Erase the value of the player in the arrMapInfo
	cMap2D->SetMapInfo(uiRow, uiCol, 0);

	// Set the start position of the Player to iRow and iCol
	vec2Position = glm::vec2(	uiCol * cMap2D->GetTileSize().x + cMap2D->GetTileHalfSize().x, 
								uiRow * cMap2D->GetTileSize().y + cMap2D->GetTileHalfSize().y);
	vec2MovementVelocity = glm::vec2(1, 1);

	// Set up the projection matrix
	projection = glm::ortho(0.0f,
		static_cast<float>(cSettings->iWindowWidth),
		0.0f,
		static_cast<float>(cSettings->iWindowHeight),
		-1.0f, 1.0f);

	// Generate the VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Load the player texture 
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID(playerTexture, true);
	if (iTextureID == 0)
	{
		cout << "Unable to load " << playerTexture << endl;
		return false;
	}
	
	//CS: Create the animated sprite and setup the animation 
	animatedSprites = CMeshBuilder::GenerateSpriteAnimation(16, 12, 2, 2);// , cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
	
	animatedSprites->AddAnimation("idleRight", 96, 113);
	animatedSprites->AddAnimation("idleLeft", 120, 137);
	animatedSprites->AddAnimation("facingRight", 96, 98);
	animatedSprites->AddAnimation("facingLeft", 120, 122);
	animatedSprites->AddAnimation("movingRight", 0, 23);
	animatedSprites->AddAnimation("movingLeft", 24, 47);
	animatedSprites->AddAnimation("jumpingRight", 48, 59);
	animatedSprites->AddAnimation("jumpingLeft", 72, 83);
	animatedSprites->AddAnimation("fallingRight", 60, 66);
	animatedSprites->AddAnimation("fallingLeft", 83, 90);
	animatedSprites->AddAnimation("dashingRight", 144, 150);
	animatedSprites->AddAnimation("dashingLeft", 156, 162);

	//CS: Play the "idle" animation as default
	animatedSprites->PlayAnimation("idleRight", -1, 3.0f);

	//CS: Init the colour to white
	vec4ColourTint = glm::vec4(1.0, 1.0, 1.0, 1.0);

	// Set the Physics to fall status by default
	cPhysics2D.Init();
	cPhysics2D.SetHorizontalStatus(CPhysics2D::HORIZONTALSTATUS::IDLE);
	cPhysics2D.SetVerticalStatus(CPhysics2D::VERTICALSTATUS::FALL);

	// Get the handler to the CInventoryManager instance
	cInventoryManager = CInventoryManager::GetInstance();

	// Add a coin as one of the inventory items
	cInventoryItem = cInventoryManager->Add("Coins", "Image/Collectibles/Coin_Bag.png", 5, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	// Add a Lives icon as one of the inventory items
	cInventoryItem = cInventoryManager->Add("Lives", "Image/Scene2D_Lives.tga", 3, 3);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	// Add a jump potion as one of the inventory items
	cInventoryItem = cInventoryManager->Add("Jump_Potion", "Image/Collectibles/Jump_Potion.png", 5, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	return true;
}

/**
 @brief Reset this instance
 */
bool CPlayer2D::Reset()
{
	unsigned int uiRow = -1;
	unsigned int uiCol = -1;
	if (cMap2D->FindValue(200, uiRow, uiCol) == false)
		return false;	// Unable to find the start position of the player, so quit this game

	// Erase the value of the player in the arrMapInfo
	cMap2D->SetMapInfo(uiRow, uiCol, 0);

	// Set the start position of the Player to iRow and iCol
	vec2Position = glm::vec2(uiCol * cMap2D->GetTileSize().x + cMap2D->GetTileHalfSize().x,
		uiRow * cMap2D->GetTileSize().y + cMap2D->GetTileHalfSize().y);

	//Set it to fall upon entering new level
	cPhysics2D.SetHorizontalStatus(CPhysics2D::HORIZONTALSTATUS::IDLE);
	cPhysics2D.SetVerticalStatus(CPhysics2D::VERTICALSTATUS::FALL);

	//CS: Play the "idle" animation as default
	animatedSprites->PlayAnimation("idleRight", -1, 3.0f);

	//CS: Init the colour to white
	vec4ColourTint = glm::vec4(1.0, 1.0, 1.0, 1.0);

	// Reset player's lives
	cInventoryItem = cInventoryManager->GetItem("Lives");
	for (int i = 0; i < cInventoryItem->GetMaxCount(); i++)
	cInventoryItem->Add(1);

	cInventoryItem = cInventoryManager->GetItem("Coins");
	cInventoryItem->Remove(cInventoryItem->GetCount());
	return true;
}

/**
 @brief Update this instance
 @param dElapsedTime A const double variable contains the time since the last frame
 @return A bool variable to indicate this method successfully completed its tasks
 */
bool CPlayer2D::Update(const double dElapsedTime)
{
	#pragma region sound

	cSoundController->SetMasterVolume(0.01);

	#pragma endregion

	// Reset vec2MovementVelocity
	vec2MovementVelocity = glm::vec2(0.0f);
	// Set the physics horizontal status to idle
	if (dashTime <= 0)
	cPhysics2D.SetHorizontalStatus(CPhysics2D::HORIZONTALSTATUS::IDLE);

	if (dashTime <= 0)
	{
		// Does not contain the possibility that player was holding A or D while dashing as that is stored as lastInputHorizontal already
		// and the player should be dashing towards their last horizontal input if no keys are held
		if (cKeyboardController->IsKeyPressed(GLFW_KEY_LEFT_SHIFT) && canDash)
		{
			// Diagonally up and towards left
			if (cKeyboardController->IsKeyDown(GLFW_KEY_A) && cKeyboardController->IsKeyDown(GLFW_KEY_W))
			{
				keyPressedWhileDashingHorizontal = 'A';
				keyPressedWhileDashingVertical = 'W';

			}
			// Diagonally up and towards right
			else if (cKeyboardController->IsKeyDown(GLFW_KEY_D) && cKeyboardController->IsKeyDown(GLFW_KEY_W))
			{
				keyPressedWhileDashingHorizontal = 'D';
				keyPressedWhileDashingVertical = 'W';

			}
			// Diagonally down and towards left
			else if (cKeyboardController->IsKeyDown(GLFW_KEY_A) && cKeyboardController->IsKeyDown(GLFW_KEY_S))
			{
				keyPressedWhileDashingHorizontal = 'A';
				keyPressedWhileDashingVertical = 'S';

			}
			// Diagonally down and towards right
			else if (cKeyboardController->IsKeyDown(GLFW_KEY_D) && cKeyboardController->IsKeyDown(GLFW_KEY_S))
			{
				keyPressedWhileDashingHorizontal = 'D';
				keyPressedWhileDashingVertical = 'S';

			}
			// Up
			else if (cKeyboardController->IsKeyDown(GLFW_KEY_W))
			{
				keyPressedWhileDashingVertical = 'W';
			}
			// Down
			else if (cKeyboardController->IsKeyDown(GLFW_KEY_S))
			{
				keyPressedWhileDashingVertical = 'S';
			}

			if (cPhysics2D.GetHorizontalStatus() == CPhysics2D::HORIZONTALSTATUS::IDLE || cPhysics2D.GetHorizontalStatus() == CPhysics2D::HORIZONTALSTATUS::WALK)
				cPhysics2D.SetHorizontalStatus(CPhysics2D::HORIZONTALSTATUS::DASH);

			dashTime = 0.25f;
			canDash = false;
		}
		if (cKeyboardController->IsKeyPressed(GLFW_KEY_SPACE) && jumps < maxJumps)
		{
			// For jump
			cPhysics2D.SetVerticalStatus(CPhysics2D::VERTICALSTATUS::JUMP);
			if (jumpBoostTimer > 0)
			{
				cPhysics2D.SetInitialVelocity(vec2JumpSpeed * glm::vec2(0, 1.5f));
			}
			else
			cPhysics2D.SetInitialVelocity(vec2JumpSpeed);

			// Play a sound for jump
			cSoundController->PlaySoundByID(3);
			cPhysics2D.SetNewJump(true);
			idleTimer = 0;

			jumps++;
		}
		// Left-right movement
		if ((cKeyboardController->IsKeyDown(GLFW_KEY_A)) && (cKeyboardController->IsKeyDown(GLFW_KEY_D)))
			vec2MovementVelocity.x = 0.0f;
		else if (cKeyboardController->IsKeyDown(GLFW_KEY_A))
		{
			animatedSprites->PlayAnimation("movingLeft", -1, 2.0f);
			lastInputHorizontal = 'A';
			idleTimer = 0;
			vec2MovementVelocity.x -= vec2WalkSpeed.x;
			if (cPhysics2D.GetHorizontalStatus() == CPhysics2D::HORIZONTALSTATUS::IDLE)
				cPhysics2D.SetHorizontalStatus(CPhysics2D::HORIZONTALSTATUS::WALK);
		}
		else if (cKeyboardController->IsKeyDown(GLFW_KEY_D))
		{
			animatedSprites->PlayAnimation("movingRight", -1, 2.0f);
			lastInputHorizontal = 'D';
			idleTimer = 0;
			vec2MovementVelocity.x += vec2WalkSpeed.x;
			if (cPhysics2D.GetHorizontalStatus() == CPhysics2D::HORIZONTALSTATUS::IDLE)
				cPhysics2D.SetHorizontalStatus(CPhysics2D::HORIZONTALSTATUS::WALK);
		}
	}
	// Dash
	if (dashTime > 0)
	{
		// If player was inputting a diagonal dash
		if (keyPressedWhileDashingHorizontal == 'A' && keyPressedWhileDashingVertical == 'W')
		{
			vec2MovementVelocity.x = -vec2WalkSpeed.x * 10;
			vec2MovementVelocity.y = vec2WalkSpeed.y * 10;
		}
		else if (keyPressedWhileDashingHorizontal == 'D' && keyPressedWhileDashingVertical == 'W')
		{
			vec2MovementVelocity.x = vec2WalkSpeed.x * 10;
			vec2MovementVelocity.y = vec2WalkSpeed.y * 10;
		}
		else if (keyPressedWhileDashingHorizontal == 'A' && keyPressedWhileDashingVertical == 'S')
		{
			vec2MovementVelocity.x = -vec2WalkSpeed.x * 10;
			vec2MovementVelocity.y = -vec2WalkSpeed.y * 10;
		}
		else if (keyPressedWhileDashingHorizontal == 'D' && keyPressedWhileDashingVertical == 'S')
		{
			vec2MovementVelocity.x = vec2WalkSpeed.x * 10;
			vec2MovementVelocity.y = -vec2WalkSpeed.y * 10;
		}
		// If player was inputting a vertical dash
		else if (keyPressedWhileDashingVertical == 'W')
			vec2MovementVelocity.y = vec2WalkSpeed.y * 10;
		else if (keyPressedWhileDashingVertical == 'S')
			vec2MovementVelocity.y = -vec2WalkSpeed.y * 10;
		else if (lastInputHorizontal == 'A')
		{
			vec2MovementVelocity.x = -vec2WalkSpeed.x * 10;
		}
		else 
		{
			vec2MovementVelocity.x = vec2WalkSpeed.x * 10;
		}

		glm::vec2 vec2NewPositionHorizontal = vec2Position + vec2MovementVelocity * (float)dElapsedTime;
		if (cMap2D->CheckHorizontalCollision(vec2Position, vec2HalfSize, vec2NewPositionHorizontal) == CSettings::RESULTS::POSITIVE)
		{
			vec2MovementVelocity.x = 0;
			cPhysics2D.SetHorizontalStatus(CPhysics2D::HORIZONTALSTATUS::IDLE);
		}

		glm::vec2 vec2NewPositionVertical = vec2Position + vec2MovementVelocity * (float)dElapsedTime;
		if (cMap2D->CheckVerticalCollision(vec2Position, vec2HalfSize, vec2NewPositionVertical) == CSettings::RESULTS::POSITIVE)
		{
			vec2MovementVelocity.y = 0;
			cPhysics2D.SetVerticalStatus(CPhysics2D::VERTICALSTATUS::IDLE);
		}

		dashTime -= dElapsedTime;
		cPhysics2D.SetVerticalStatus(CPhysics2D::VERTICALSTATUS::JUMP);
	}

	// Boost player
	if (boosted)
		BoostPlayer(dElapsedTime);
	// Jump boost
	if (jumpBoostTimer > 0)
		jumpBoostTimer -= dElapsedTime;

	// If player has not input any movement and the timer reaches 20 or more
	if (idleTimer >= 15)
	{
		if (lastInputHorizontal == 'A')
			animatedSprites->PlayAnimation("idleLeft", -1, 5.f);
		else
			animatedSprites->PlayAnimation("idleRight", -1, 5.f);
	}
	// If dashing
	else if (cPhysics2D.GetHorizontalStatus() == CPhysics2D::HORIZONTALSTATUS::DASH)
	{
		// If the last direction the player moved in was left
		if (lastInputHorizontal == 'A')
		{
			animatedSprites->PlayAnimation("dashingLeft", 1, 1.f);
		}
		// If the last direction the player moved in was right
		else
		{
			animatedSprites->PlayAnimation("dashingRight", 1, 1.f);
		}
	}
	// If no keys are being held
	else if (cPhysics2D.GetHorizontalStatus() == CPhysics2D::HORIZONTALSTATUS::IDLE && cPhysics2D.GetVerticalStatus() == CPhysics2D::VERTICALSTATUS::IDLE)
	{
		// If the last direction the player moved in was left
		if (lastInputHorizontal == 'A')
		{
			idleTimer += dElapsedTime;
			animatedSprites->PlayAnimation("facingLeft", -1, 3.0f);
		}
		// If the last direction the player moved in was right
		else
		{
			idleTimer += dElapsedTime;
			animatedSprites->PlayAnimation("facingRight", -1, 3.0f);
		}
	}

	// If player is in the jump status
	if (cPhysics2D.GetVerticalStatus() == CPhysics2D::VERTICALSTATUS::JUMP)
	{
		if (lastInputHorizontal == 'A')
			animatedSprites->PlayAnimation("jumpingLeft", 1, 0.25f);
		else
			animatedSprites->PlayAnimation("jumpingRight", 1, 0.25f);
	}
	// If the player is in the fall status
	else if (cPhysics2D.GetVerticalStatus() == CPhysics2D::VERTICALSTATUS::FALL)
	{
		if (lastInputHorizontal == 'A')
			animatedSprites->PlayAnimation("fallingLeft", 1, 0.25f);
		else
			animatedSprites->PlayAnimation("fallingRight", 1, 0.25f);
	}
	else if (cPhysics2D.GetVerticalStatus() == CPhysics2D::VERTICALSTATUS::IDLE)
	{
		jumps = 0;
		canDash = true;

		// Resetting the key pressed while dashing
		keyPressedWhileDashingHorizontal = '\0';
		keyPressedWhileDashingVertical = '\0';
	}

	// Calculate the physics for JUMP/DOUBLE JUMP/FALL movement
	if ((cPhysics2D.GetVerticalStatus() >= CPhysics2D::VERTICALSTATUS::JUMP)
		&& (cPhysics2D.GetVerticalStatus() <= CPhysics2D::VERTICALSTATUS::FALL))
	{
		// Update the elapsed time to the physics engine
		cPhysics2D.AddElapsedTime((float)dElapsedTime);
		// Call the physics engine update method to calculate the final velocity and displacement
		cPhysics2D.Update(dElapsedTime);
		// Get the displacement from the physics engine and update the player position
		vec2MovementVelocity += cPhysics2D.GetFinalVelocity();

		// Set the physics vertical status from jump/double jump to fall if the movement direction changes to negative
		if (cPhysics2D.GetVerticalStatus() >= CPhysics2D::VERTICALSTATUS::JUMP)
			//&& (cPhysics2D.GetVerticalStatus() <= CPhysics2D::VERTICALSTATUS::DOUBLEJUMP))
		{
			if (cPhysics2D.GetFinalVelocity().y < 0.0f)
			{
				cPhysics2D.SetVerticalStatus(CPhysics2D::VERTICALSTATUS::FALL, false);
			}
		}
	}

	// Update vec2Position
	glm::vec2 vec2NewPosition = vec2Position + vec2MovementVelocity * (float)dElapsedTime;

	// Check for collision with the Tile Maps vertically
	if (cPhysics2D.GetHorizontalStatus() == CPhysics2D::HORIZONTALSTATUS::WALK)
	{
		//cout << "Check horizontal movement" << endl;
		// Check if the player walks into an obstacle
		if (cMap2D->CheckHorizontalCollision(vec2Position, vec2HalfSize, vec2NewPosition) == CSettings::RESULTS::POSITIVE)
		{
			//cout << "Horizontal collision!" << endl;
			cPhysics2D.SetHorizontalStatus(CPhysics2D::HORIZONTALSTATUS::IDLE);
		}

		if (cPhysics2D.GetVerticalStatus() == CPhysics2D::VERTICALSTATUS::IDLE)
		{
			// Check if he is walking on air; let him fall down
			glm::vec2 vec2InAirPosition = vec2Position - glm::vec2(0.0f, vec2HalfSize.y);
			if (cMap2D->CheckVerticalCollision(vec2Position, vec2HalfSize, vec2InAirPosition) == CSettings::RESULTS::NEGATIVE)
			{
				cPhysics2D.SetVerticalStatus(CPhysics2D::VERTICALSTATUS::FALL);
				//cout << "*** Walked off a platform. Set to Fall ***" << endl;
			}
		}
	}

	// Check for collision with the Tile Maps vertically
	if ((cPhysics2D.GetVerticalStatus() >= CPhysics2D::VERTICALSTATUS::JUMP) && 
		(cPhysics2D.GetVerticalStatus() <= CPhysics2D::VERTICALSTATUS::DOUBLEJUMP) &&
		(cMap2D->CheckVerticalCollision(vec2Position, vec2HalfSize, vec2NewPosition) == CSettings::RESULTS::POSITIVE))
	{
		//cout << "Vertical collision when jumping!" << endl;
		if ((cPhysics2D.GetVerticalStatus() == CPhysics2D::VERTICALSTATUS::JUMP))
		{
			cPhysics2D.SetVerticalStatus(CPhysics2D::VERTICALSTATUS::FALL);
			//cout << "*** Hit a platform on top: Set to Fall ***" << endl;
		}
	}

	// Check for collision with the Tile Maps vertically
	if ((cPhysics2D.GetVerticalStatus() == CPhysics2D::VERTICALSTATUS::FALL) &&
		(cMap2D->CheckVerticalCollision(vec2Position, vec2HalfSize, vec2NewPosition) == CSettings::RESULTS::POSITIVE))
	{
			cPhysics2D.SetVerticalStatus(CPhysics2D::VERTICALSTATUS::IDLE);

			lastIdlePosition = vec2Position;
			boostable = true;
	}

	// Update the vec2Position with the new position
	vec2Position = vec2NewPosition;
	//cout << "vec2Position= " << vec2Position.x << ", " << vec2Position.y << endl;

	// Constraint the player within the map
	if (cMap2D->Constraint(vec2Position) == true)
	{
		if (vec2MovementVelocity.y > 0.0f)
		{
			vec2MovementVelocity.y = 0.0f;
		}
		if ((cPhysics2D.GetVerticalStatus() == CPhysics2D::VERTICALSTATUS::JUMP))
		{
			cPhysics2D.SetVerticalStatus(CPhysics2D::VERTICALSTATUS::FALL);
		}
		else if (cPhysics2D.GetVerticalStatus() == CPhysics2D::VERTICALSTATUS::FALL)
		{
			cPhysics2D.SetVerticalStatus(CPhysics2D::VERTICALSTATUS::IDLE);
		}
	}

	// Update the Health and Lives
	 UpdateHealthLives();

	// Interact with the Map
	InteractWithMap();

	//CS: Update the animated sprite
	animatedSprites->Update(dElapsedTime);

	return true;
}

/**
 @brief Set up the OpenGL display environment before rendering
 */
void CPlayer2D::PreRender(void)
{
	// Activate blending mode
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Activate the shader
	CShaderManager::GetInstance()->Use(sShaderName);
}

/**
 @brief Render this instance
 */
void CPlayer2D::Render(void)
{
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(vec2Position, 0.0f));

	//model = glm::translate(model, glm::vec3(0.5f * cSettings->TILE_WIDTH, 0.5f * cSettings->TILE_HEIGHT, 0.0f));
	//model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::translate(model, glm::vec3(-0.5f * cSettings->TILE_WIDTH, -0.5f * cSettings->TILE_HEIGHT, 0.0f));

	model = glm::scale(model, glm::vec3(18.75f, 18.75f, 1.0f));
	
	// note: currently we set the projection matrix each frame, but since the projection 
	// matrix rarely changes it's often best practice to set it outside the main loop only once.
	CShaderManager::GetInstance()->activeShader->setMat4("Model", model);
	CShaderManager::GetInstance()->activeShader->setMat4("Projection", projection);
	unsigned int colourLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "ColourTint");
	glUniform4fv(colourLoc, 1, glm::value_ptr(vec4ColourTint));

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	// Get the texture to be rendered
	glBindTexture(GL_TEXTURE_2D, iTextureID);
		//CS: Render the animated sprite
		glBindVertexArray(VAO);
			animatedSprites->Render();
		glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	return;
}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CPlayer2D::PostRender(void)
{
	// Disable blending
	glDisable(GL_BLEND);
}

/**
 @brief Let player interact with the map. You can add collectibles such as powerups and health here.
 */
void CPlayer2D::InteractWithMap(void)
{
	int iPositionX = 0;
	int iPositionY = 0;
	if (cMap2D->GetTileIndexAtPosition(vec2Position, iPositionX, iPositionY) == false)
		return;

	switch (cMap2D->GetMapInfo(iPositionY, iPositionX))
	{
	case 2:
		// Erase the coin from this position
		cMap2D->SetMapInfo(iPositionY, iPositionX, 0);
		// Increase the coins by one
		cInventoryItem = cInventoryManager->GetItem("Coins");
		cInventoryItem->Add(1);
		// Play a bell sound
		cSoundController->PlaySoundByID(1);
		break;
	case 10:
		// Erase the life from this position
		cMap2D->SetMapInfo(iPositionY, iPositionX, 0);
		// Increase the lives by 1
		cInventoryItem = cInventoryManager->GetItem("Lives");
		cInventoryItem->Add(1);
		break;
	case 20:
		// Decrease the lives by 1
		cInventoryItem = cInventoryManager->GetItem("Lives");
		cInventoryItem->Remove(1);
		vec2Position = lastIdlePosition;
		break;
	case 21:
		// Decrease the lives by 1
		cInventoryItem = cInventoryManager->GetItem("Lives");
		cInventoryItem->Remove(1);
		vec2Position = lastIdlePosition;
		break;
	case 22:
		// Decrease the lives by 1
		cInventoryItem = cInventoryManager->GetItem("Lives");
		cInventoryItem->Remove(1);
		vec2Position = lastIdlePosition;
		break;
	case 23:
		// Decrease the lives by 1
		cInventoryItem = cInventoryManager->GetItem("Lives");
		cInventoryItem->Remove(1);
		vec2Position = lastIdlePosition;
		break;
	case 24:
		boosted = true;
		boostDirection = 'W';
		break;
	case 25:
		boosted = true;
		boostDirection = 'S';
		break;
	case 26:
		boosted = true;
		boostDirection = 'A';
		break;
	case 27:
		boosted = true;
		boostDirection = 'D';
		break;
	case 28:
		cMap2D->SetMapInfo(iPositionY, iPositionX, 0);
		jumpBoostTimer = 20;

		cInventoryItem = cInventoryManager->GetItem("Jump_Potion");
		cInventoryItem->Add(1);
		break;
	case 98:
		cInventoryItem = cInventoryManager->GetItem("Coins");
		// Level has been completed if all coins have been collected
		if (cInventoryItem->GetCount() >= cInventoryItem->GetMaxCount())
		CGameManager::GetInstance()->bLevelCompleted = true;
		break;
	case 99:
		cInventoryItem = cInventoryManager->GetItem("Coins");
		// Game has been completed if all coins have been collected
		if (cInventoryItem->GetCount() >= cInventoryItem->GetMaxCount())
		CGameManager::GetInstance()->bPlayerWon = true;
		break;
	default:
		break;
	}
}

/**
 @brief Update the health and lives.
 */
void CPlayer2D::UpdateHealthLives(void)
{
	// Update lives
	cInventoryItem = cInventoryManager->GetItem("Lives");

	if (cInventoryItem->GetCount() == 0)
	{
		// Player loses the game
		CGameManager::GetInstance()->bPlayerLost = true;
	}
}

void CPlayer2D::BoostPlayer(const double dElapsedTime)
{
	if (boostable == true)
	{
		if (boostDirection == 'W')
			vec2MovementVelocity.y = vec2WalkSpeed.y * 100;
		else if (boostDirection == 'S')
			vec2MovementVelocity.y = -vec2WalkSpeed.y * 100;
		else if (boostDirection == 'A')
			vec2MovementVelocity.x = -vec2WalkSpeed.x * 100;
		else if (boostDirection == 'D')
			vec2MovementVelocity.x = vec2WalkSpeed.x * 100;

		glm::vec2 vec2NewPositionHorizontal = vec2Position + vec2MovementVelocity * (float)dElapsedTime;
		if (cMap2D->CheckHorizontalCollision(vec2Position, vec2HalfSize, vec2NewPositionHorizontal) == CSettings::RESULTS::POSITIVE)
		{
			vec2MovementVelocity.x = 0;
			cPhysics2D.SetHorizontalStatus(CPhysics2D::HORIZONTALSTATUS::IDLE);
		}

		glm::vec2 vec2NewPositionVertical = vec2Position + vec2MovementVelocity * (float)dElapsedTime;
		if (cMap2D->CheckVerticalCollision(vec2Position, vec2HalfSize, vec2NewPositionVertical) == CSettings::RESULTS::POSITIVE)
		{
			vec2MovementVelocity.y = 0;
			cPhysics2D.SetVerticalStatus(CPhysics2D::VERTICALSTATUS::IDLE);
		}
	}

		boostDirection = '\0';
		boostable = false;
		boosted = false;
}
