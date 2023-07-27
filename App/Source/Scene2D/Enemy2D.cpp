/**
 CEnemy2D
 @brief A class which represents the enemy object
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "Enemy2D.h"

#include <iostream>
using namespace std;

// Include Shader Manager
#include "RenderControl\ShaderManager.h"
// Include Mesh Builder
#include "Primitives/MeshBuilder.h"

// Include GLEW
#include <GL/glew.h>

// Include ImageLoader
#include "System\ImageLoader.h"

// Include the Map2D as we will use it to check the player's movements and actions
#include "Map2D.h"
// Include math.h
#include <math.h>

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CEnemy2D::CEnemy2D(void)
	: bIsActive(false)
	, vec2Destination(glm::vec2(0, 0))
	, vec2Direction(glm::vec2(0, 0))
	, vec2MovementVelocity(glm::vec2(1, 1))
	, cMap2D(NULL)
	, cPlayer2D(NULL)
	, sCurrentFSM(FSM::IDLE)
	, iFSMCounter(0)
{
	// Initialise position of the enemy
	vec2Position = glm::vec2(0);
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CEnemy2D::~CEnemy2D(void)
{
	// Delete the quadMesh
	if (p2DMesh)
	{
		delete p2DMesh;
		p2DMesh = NULL;
	}

	// We won't delete this since it was created elsewhere
	cPlayer2D = NULL;

	// We won't delete this since it was created elsewhere
	cMap2D = NULL;

	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

/**
  @brief Initialise this instance
  @return true is the initialisation is successful, otherwise false
  */
bool CEnemy2D::Init(void)
{
	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();

	// Get the handler to the CMap2D instance
	cMap2D = CMap2D::GetInstance();

	// Get the handler to the CPlayer2D instance
	cPlayer2D = CPlayer2D::GetInstance();
	// Update Direction so it goes to the cPlayer2D
	UpdateDirection();

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Set up the projection matrix
	projection = glm::ortho(0.0f,
							static_cast<float>(cSettings->iWindowWidth),
							0.0f,
							static_cast<float>(cSettings->iWindowHeight),
							-1.0f, 1.0f);

	//CS: Create the Quad Mesh using the mesh builder
	p2DMesh = CMeshBuilder::GenerateQuad(glm::vec4(1, 1, 1, 1), 1, 1);

	// Load the enemy2D texture
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Scene2D_EnemyTile.tga", true);
	if (iTextureID == 0)
	{
		cout << "Unable to load Image/Scene2D_EnemyTile.tga" << endl;
		return false;
	}

	//CS: Init the colour to white
	vec4ColourTint = glm::vec4(1.0, 1.0, 1.0, 1.0);

	// Set the Physics to fall status by default
	cPhysics2D.Init();
	cPhysics2D.SetHorizontalStatus(CPhysics2D::HORIZONTALSTATUS::IDLE);
	cPhysics2D.SetVerticalStatus(CPhysics2D::VERTICALSTATUS::FALL);

	// If this class is initialised properly, then set the bIsActive to true
	bIsActive = true;

	return true;
}

/**
 @brief Update this instance
 @param dElapsedTime A const double variable contains the time since the last frame
 @return A bool variable to indicate this method successfully completed its tasks
 */
bool CEnemy2D::Update(const double dElapsedTime)
{
	if (!bIsActive)
		return false;

	// Reset vec2MovementVelocity
	vec2MovementVelocity = glm::vec2(0.0f);
	// Set the physics horizontal status to idle
	cPhysics2D.SetHorizontalStatus(CPhysics2D::HORIZONTALSTATUS::IDLE);

	// Get updates from AI
	switch (sCurrentFSM)
	{
	case IDLE:
		if (iFSMCounter > iMaxFSMCounter)
		{
			sCurrentFSM = PATROL;
			iFSMCounter = 0;
			cout << "Switching to Patrol State" << endl;
		}
		iFSMCounter++;
		break;
	case PATROL:
		if (iFSMCounter > iMaxFSMCounter)
		{
			sCurrentFSM = IDLE;
			iFSMCounter = 0;
			cout << "Switching to Idle State" << endl;
		}
		else if (glm::distance(vec2Position, cPlayer2D->vec2Position) <= glm::length(vec2HalfSize) * 10.0f)
		{
 			sCurrentFSM = ATTACK;
			iFSMCounter = 0;
		}
		else
		{
			// Patrol around
			// Update the Enemy2D's position for patrol
			UpdatePosition();
		}
		iFSMCounter++;
		break;
	case ATTACK:
		if (glm::distance(vec2Position, cPlayer2D->vec2Position) <= glm::length(vec2HalfSize) * 10.0f)
		{
			int iStartX = 0;
			int iStartY = 0;
			int iTargetX = 0;
			int iTargetY = 0;
			if ((cMap2D->GetTileIndexAtPosition(vec2Position, iStartX, iStartY) == true) &&
				(cMap2D->GetTileIndexAtPosition(cPlayer2D->vec2Position, iTargetX, iTargetY) == true))
			{

				// Calculate a path to the player
				auto path = cMap2D->PathFind(	glm::vec2(iStartX, iStartY), 
												glm::vec2(iTargetX, iTargetY),
												heuristic::euclidean,
												10);

				//cout << "=== Printing out the path ===" << endl;
				// Calculate new destination
				bool bFirstPosition = true;
				glm::vec2 vec2PathPoint;
				for (const auto& coord : path)
				{
					// Take a path point from path via coord
					vec2PathPoint = coord;
					if (bFirstPosition == true)
					{
						// Set a destination
						vec2Destination = glm::vec2(vec2PathPoint.x * cMap2D->GetTileSize().x, vec2PathPoint.y * cMap2D->GetTileSize().y);
						// Calculate the direction between enemy2D and this destination
						vec2Direction = glm::normalize(vec2PathPoint - glm::vec2(iStartX, iStartY));
						bFirstPosition = false;
					}
					else
					{
						// If the next path point is in the same direction, use it as the new vec2Destination
						if (glm::normalize(vec2PathPoint - glm::vec2(iStartX, iStartY)) == vec2Direction)
						{
							// Set a destination
							vec2Destination = glm::vec2(vec2PathPoint.x * cMap2D->GetTileSize().x, vec2PathPoint.y * cMap2D->GetTileSize().y);
						}
						else
							break;
					}
				}

				//cout << "vec2Destination : " << vec2Destination.x 
				//		<< ", " << vec2Destination.y << endl;
				//cout << "vec2Direction : " << vec2Direction.x 
				//		<< ", " << vec2Direction.y << endl;
				//system("pause");

				// Update the Enemy2D's position for attack
				UpdatePosition();
			}
		}
		else
		{
			if (iFSMCounter > iMaxFSMCounter)
			{
				sCurrentFSM = PATROL;
				iFSMCounter = 0;
				cout << "ATTACK : Reset counter: " << iFSMCounter << endl;
			}
			iFSMCounter++;
		}
		break;
	default:
		break;
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
		if ((cPhysics2D.GetVerticalStatus() >= CPhysics2D::VERTICALSTATUS::JUMP)
			&& (cPhysics2D.GetVerticalStatus() <= CPhysics2D::VERTICALSTATUS::DOUBLEJUMP))
		{
			if (cPhysics2D.GetFinalVelocity().y < 0.0f)
				cPhysics2D.SetVerticalStatus(CPhysics2D::VERTICALSTATUS::FALL, false);
		}
	}

	// Update vec2Position
	glm::vec2 vec2NewPosition = vec2Position + vec2MovementVelocity * (float)dElapsedTime;

	// Check for collision with the Tile Maps vertically
	if (cPhysics2D.GetHorizontalStatus() == CPhysics2D::HORIZONTALSTATUS::WALK)
	{
		// Check if the player walks into an obstacle
		if (cMap2D->CheckHorizontalCollision(vec2Position, vec2HalfSize, vec2NewPosition) == CSettings::RESULTS::POSITIVE)
		{
			cPhysics2D.SetHorizontalStatus(CPhysics2D::HORIZONTALSTATUS::IDLE);
			// Flip the direction since this direction is blocked
			FlipHorizontalDirection();
		}

		if (cPhysics2D.GetVerticalStatus() == CPhysics2D::VERTICALSTATUS::IDLE)
		{
			// Check if he is walking on air; let him fall down
			glm::vec2 vec2InAirPosition = vec2Position - glm::vec2(0.0f, vec2HalfSize.y);
			if (cMap2D->CheckVerticalCollision(vec2Position, vec2HalfSize, vec2InAirPosition) == CSettings::RESULTS::NEGATIVE)
			{
				cPhysics2D.SetVerticalStatus(CPhysics2D::VERTICALSTATUS::FALL);
			}
		}
	}

	// Check for collision with the Tile Maps vertically
	if ((cPhysics2D.GetVerticalStatus() >= CPhysics2D::VERTICALSTATUS::JUMP) &&
		(cPhysics2D.GetVerticalStatus() <= CPhysics2D::VERTICALSTATUS::DOUBLEJUMP) &&
		(cMap2D->CheckVerticalCollision(vec2Position, vec2HalfSize, vec2NewPosition) == CSettings::RESULTS::POSITIVE))
	{
		if ((cPhysics2D.GetVerticalStatus() == CPhysics2D::VERTICALSTATUS::JUMP) || (cPhysics2D.GetVerticalStatus() == CPhysics2D::VERTICALSTATUS::DOUBLEJUMP))
		{
			cPhysics2D.SetVerticalStatus(CPhysics2D::VERTICALSTATUS::FALL);
		}
	}

	// Check for collision with the Tile Maps vertically
	if ((cPhysics2D.GetVerticalStatus() == CPhysics2D::VERTICALSTATUS::FALL) &&
		(cMap2D->CheckVerticalCollision(vec2Position, vec2HalfSize, vec2NewPosition) == CSettings::RESULTS::POSITIVE))
	{
		if (cPhysics2D.GetVerticalStatus() == CPhysics2D::VERTICALSTATUS::FALL)
		{
			cPhysics2D.SetVerticalStatus(CPhysics2D::VERTICALSTATUS::IDLE);
		}
	}

	// Update the vec2Position with the new position
	vec2Position = vec2NewPosition;

	// Constraint the player within the map
	if (cMap2D->Constraint(vec2Position) == true)
	{
		if (vec2MovementVelocity.y > 0.0f)
		{
			vec2MovementVelocity.y = 0.0f;
		}
		if ((cPhysics2D.GetVerticalStatus() == CPhysics2D::VERTICALSTATUS::JUMP) || (cPhysics2D.GetVerticalStatus() == CPhysics2D::VERTICALSTATUS::DOUBLEJUMP))
		{
			cPhysics2D.SetVerticalStatus(CPhysics2D::VERTICALSTATUS::FALL);
		}
		else if (cPhysics2D.GetVerticalStatus() == CPhysics2D::VERTICALSTATUS::FALL)
		{
			cPhysics2D.SetVerticalStatus(CPhysics2D::VERTICALSTATUS::IDLE);
		}
	}

	// Interact with the Player
	InteractWithPlayer();

	return true;
}

/**
 @brief Set up the OpenGL display environment before rendering
 */
void CEnemy2D::PreRender(void)
{
	if (!bIsActive)
		return;

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);

	// Activate blending mode
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Activate the shader
	CShaderManager::GetInstance()->Use(sShaderName);
}

/**
 @brief Render this instance
 */
void CEnemy2D::Render(void)
{
	if (!bIsActive)
		return;

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(vec2Position, 0.0f));

	//model = glm::translate(model, glm::vec3(0.5f * cSettings->TILE_WIDTH, 0.5f * cSettings->TILE_HEIGHT, 0.0f));
	//model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::translate(model, glm::vec3(-0.5f * cSettings->TILE_WIDTH, -0.5f * cSettings->TILE_HEIGHT, 0.0f));

	model = glm::scale(model, glm::vec3(25.0f, 25.0f, 1.0f));

	// Upload the model to OpenGL shader
	CShaderManager::GetInstance()->activeShader->setMat4("Model", model);
	// Upload the Projection to OpenGL shader
	CShaderManager::GetInstance()->activeShader->setMat4("Projection", projection);
	unsigned int colourLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "ColourTint");
	glUniform4fv(colourLoc, 1, glm::value_ptr(vec4ColourTint));

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	// Get the texture to be rendered
	glBindTexture(GL_TEXTURE_2D, iTextureID);
		//CS: Render the sprite
		glBindVertexArray(VAO);
			p2DMesh->Render();
		glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CEnemy2D::PostRender(void)
{
	if (!bIsActive)
		return;

	// Disable blending
	glDisable(GL_BLEND);
}

/**
 @brief Set the handle to cPlayer to this class instance
 @param cPlayer2D A CPlayer2D* variable which contains the pointer to the CPlayer2D instance
 */
void CEnemy2D::SetPlayer2D(CPlayer2D* cPlayer2D)
{
	this->cPlayer2D = cPlayer2D;

	// Update the enemy's direction
	UpdateDirection();
}

/**
 @brief PrintSelf
 */ 
void CEnemy2D::PrintSelf(void)
{
	cout << "CEnemy2D::PrintSelf()" << endl;
	cout << "=======================" << endl;

	cout << "vec2Position\t=\t[" << vec2Position.x << ", " << vec2Position.y << "]" << endl;
	cout << "vec2Destination\t=\t[" << vec2Destination.x << ", " << vec2Destination.y << "]" << endl;
	cout << "vec2Direction\t=\t[" << vec2Direction.x << ", " << vec2Direction.y << "]" << endl;
	cout << "vec2MovementVelocity\t=\t[" << vec2MovementVelocity.x << ", " << vec2MovementVelocity.y << "]" << endl;
	cout << "sCurrentFSM\t=\t" << sCurrentFSM << endl;
	
	cPhysics2D.PrintSelf();
}

/**
 @brief Let enemy2D interact with the player.
 */
bool CEnemy2D::InteractWithPlayer(void)
{
	// Check if the enemy2D is within 1 tile size of the player2D
	if (glm::distance(vec2Position, cPlayer2D->vec2Position) <= glm::length(vec2HalfSize) * 2.0f)
	{
		cout << "Gotcha!" << endl;
		// Since the player has been caught, then reset the FSM
		sCurrentFSM = IDLE;
		iFSMCounter = 0;
		return true;
	}

	return false;
}

/**
 @brief Update the enemy's direction.
 */
void CEnemy2D::UpdateDirection(void)
{
	// Set the destination to the player
	vec2Destination = cPlayer2D->vec2Position;

	// Calculate the direction between enemy2D and player2D
	vec2Direction = CalculateDirection(vec2Position, vec2Destination);
}

/**
 @brief Flip horizontal direction. For patrol use only
 */
void CEnemy2D::FlipHorizontalDirection(void)
{
	vec2Direction.x *= -1;
}

/**
@brief Update position.
*/
void CEnemy2D::UpdatePosition(void)
{
	// If the player is to the left of the enemy2D, then move to the left
	if (vec2Direction.x < 0)
	{
		// Move left
		vec2MovementVelocity.x -= vec2WalkSpeed.x;
		if (cPhysics2D.GetHorizontalStatus() == CPhysics2D::HORIZONTALSTATUS::IDLE)
			cPhysics2D.SetHorizontalStatus(CPhysics2D::HORIZONTALSTATUS::WALK);
	}
	else if (vec2Direction.x > 0)
	{
		// Move right
		vec2MovementVelocity.x += vec2WalkSpeed.x;
		if (cPhysics2D.GetHorizontalStatus() == CPhysics2D::HORIZONTALSTATUS::IDLE)
			cPhysics2D.SetHorizontalStatus(CPhysics2D::HORIZONTALSTATUS::WALK);
	}
	else
	{
		vec2MovementVelocity.x = 0.0f;
	}

	// if the player is above the enemy2D, then jump to attack
	if (vec2Direction.y > 0)
	{
		if (cPhysics2D.GetVerticalStatus() == CPhysics2D::VERTICALSTATUS::IDLE)
		{
			cPhysics2D.SetVerticalStatus(CPhysics2D::VERTICALSTATUS::JUMP);
			cPhysics2D.SetInitialVelocity(vec2JumpSpeed);
			cPhysics2D.SetNewJump(true);
		}
	}
}


/**
@brief Calculate Direction using coordinates, not indices
*/
glm::vec2 CEnemy2D::CalculateDirection(const glm::vec2 vec2StartPosition, const glm::vec2 vec2EndPosition)
{
	int iStartPositionX = 0;
	int iStartPositionY = 0;
	int iEndPositionX = 0;
	int iEndPositionY = 0;
	if ((cMap2D->GetTileIndexAtPosition(vec2Position, iStartPositionX, iStartPositionY) == true) &&
		(cMap2D->GetTileIndexAtPosition(cPlayer2D->vec2Position, iEndPositionX, iEndPositionY) == true))
	{
		glm::vec2 vec2Direction = glm::normalize(glm::vec2(iEndPositionX, iEndPositionY) - glm::vec2(iStartPositionX, iStartPositionY));
		return vec2Direction;
	}
	return glm::vec2(0);
}
