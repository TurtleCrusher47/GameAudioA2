/**
 CPlayer3D
 By: Toh Da Jun
 Date: Apr 2020
 */
#include "Player3D.h"

 // Include ShaderManager
#include "RenderControl/ShaderManager.h"

// Include ImageLoader
#include "System/ImageLoader.h"

//For allowing creating of Mesh 
#include "Primitives/MeshBuilder.h"

// Include CSolidObjectManager
#include "SolidObjectManager.h"

// Include this for glm::to_string() function
#define GLM_ENABLE_EXPERIMENTAL
#include <includes/gtx/string_cast.hpp>

#include <iostream>
using namespace std;

/**
 @brief Default Constructor
 */
CPlayer3D::CPlayer3D(void)
	: vec3Up(glm::vec3(0.0f, 1.0f, 0.0f))
	, vec3Right(glm::vec3(1.0f, 1.0f, 0.0f))
	, vec3WorldUp(glm::vec3(0.0f, 1.0f, 0.0f))
	, fYaw(0.0f)
	, fPitch(0.0f)
	, fMouseSensitivity(0.1f)
	, fZoom(45.0f)
	, cCamera(NULL)
	, cTerrainManager(NULL)
	, cPrimaryWeapon(NULL)
	, cSecondaryWeapon(NULL)
	, iCurrentWeapon(0)
	, bMoved(false)
{
	// Set the default position so it is above the ground
	vec3Position = glm::vec3(0.0f, 0.5f, 0.0f);

	// Update the player vectors
	UpdatePlayerVectors();
}

/**
 @brief Constructor with vectors
 @param position A const glm::vec3 variable which contains the position of the camera
 @param up A const glm::vec3 variable which contains the up direction of the camera
 @param yaw A const float variable which contains the yaw of the camera
 @param pitch A const float variable which contains the pitch of the camera
 */
CPlayer3D::CPlayer3D(	const glm::vec3 vec3Position,
						const glm::vec3 vec3Front,
						const float fYaw,
						const float fPitch)
	: vec3Up(glm::vec3(0.0f, 1.0f, 0.0f))
	, vec3Right(glm::vec3(1.0f, 1.0f, 0.0f))
	, vec3WorldUp(glm::vec3(0.0f, 1.0f, 0.0f))
	, fYaw(fYaw)
	, fPitch(fPitch)
	, fMouseSensitivity(0.1f)
	, fZoom(45.0f)
	, cCamera(NULL)
	, cTerrainManager(NULL)
	, cPrimaryWeapon(NULL)
	, cSecondaryWeapon(NULL)
	, iCurrentWeapon(0)
	, bMoved(false)
{
	p3DMesh = NULL;

	this->vec3Position = vec3Position;
	this->vec3Front = vec3Front;
	this->fYaw = fYaw;
	this->fPitch = fPitch;

	// Update the player vectors
	UpdatePlayerVectors();
}

/**
 @brief Destructor
 */
CPlayer3D::~CPlayer3D(void)
{
	if (cSecondaryWeapon)
	{
		delete cSecondaryWeapon;
		cSecondaryWeapon = NULL;
	}

	if (cPrimaryWeapon)
	{
		delete cPrimaryWeapon;
		cPrimaryWeapon = NULL;
	}

	if (cTerrainManager)
	{
		// We set it to NULL only since it was declared somewhere else
		cTerrainManager = NULL;
	}
	
	if (cCamera)
	{
		// We set it to NULL only since it was declared somewhere else
		cCamera = NULL;
	}
}

/**
 @brief Initialise this class instance
 @return true is successfully initialised this class instance, else false
 */
bool CPlayer3D::Init(void)
{
	// Call the parent's Init()
	CSolidObject::Init();

	// Set the type
	SetType(CEntity3D::TYPE::PLAYER);

	// Generate and bind the VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//CS: Create the Quad Mesh using the mesh builder
	p3DMesh = CMeshBuilder::GenerateBox(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
		1.0f, 1.0f, 1.0f);

	// load and create a texture 
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Scene3D_Player.tga", false);
	if (iTextureID == 0)
	{
		cout << "Unable to load Image/Scene3D_Player.tga" << endl;
		return false;
	}

	// Store the handler to the cTerrainManager
	cTerrainManager = CTerrainManager::GetInstance();
	// Update the y-axis position of the player
	vec3Position.y = cTerrainManager->GetHeight(vec3Position.x, vec3Position.z) + fHeightOffset;

	// Set the Physics to fall status by default
	cPhysics3D.SetStatus(CPhysics3D::STATUS::FALL);

	// Set the postures
	iPreviousPosture = PLAYERPOSTURE::STANDING;
	iCurrentPosture = PLAYERPOSTURE::STANDING;
	iNextPosture = PLAYERPOSTURE::STANDING;
	fPostureOffset = 0.0f;

	// Update the player vectors
	UpdatePlayerVectors();

	return true;
}

/**
@brief Returns the view matrix calculated using Euler Angles and the LookAt Matrix
@return A glm::mat4 variable which contains the view matrix
*/
glm::mat4 CPlayer3D::GetViewMatrix(void) const
{
	return glm::lookAt(vec3Position, vec3Position + vec3Front, vec3Up);
}

/**
@brief Returns the Perspective Matrix
@return A glm::mat4 variable which contains the Perspective matrix
*/
glm::mat4 CPlayer3D::GetPerspectiveMatrix(void) const
{
	return  glm::perspective(glm::radians(fZoom),
		(float)CSettings::GetInstance()->iWindowWidth / (float)CSettings::GetInstance()->iWindowHeight,
		0.1f, 1000.0f);
}

/**
 @brief Attach a camera to this class instance
 @param cCamera A CCamera* variable which contains the camera
 */
void CPlayer3D::AttachCamera(CCamera* cCamera)
{
	// Set the camera to the player
	this->cCamera = cCamera;

	// Update the camera's attributes with the player's attributes
	if (cCamera)
	{
		cCamera->vec3Position = vec3Position;
		cCamera->vec3Front = vec3Front;
		cCamera->vec3Up = vec3Up;
		cCamera->vec3Right = vec3Right;
		cCamera->fYaw = fYaw;
		cCamera->fPitch = fPitch;
		// We should not update the fZoom as this will affect the Frustum Culling and spatial partition
		// The player should still have the default view of the environment
		//cCamera->fZoom = fZoom;	
	}
}

/**
 @brief Check if a camera ia attached to this class instance
 @return true if a camera is attached, else false
 */
bool CPlayer3D::IsCameraAttached(void)
{
	if (cCamera)
		return true;
	return false;
}

/**
 @brief Set Weapon to this class instance
 @param iSlot A const int variable which contains the weapon info to check for. 0 == Primary, 1 == Secondary
 @param cWeaponInfo A CWeaponInfo* variable which contains the weapon info
 */
void CPlayer3D::SetWeapon(const int iSlot, CWeaponInfo* cWeaponInfo)
{
	if (iSlot == 0)
		cPrimaryWeapon = cWeaponInfo;
	else if (iSlot == 1)
		cSecondaryWeapon = cWeaponInfo;
}

/**
 @brief Get Weapon of this class instance
 @param iSlot A const int variable which contains the weapon info to check for. 0 == Primary, 1 == Secondary
 */
CWeaponInfo* CPlayer3D::GetWeapon(void) const
{
	if (iCurrentWeapon == 0)
		return cPrimaryWeapon;
	else if (iCurrentWeapon == 1)
		return cSecondaryWeapon;

	return NULL;
}

/**
 @brief Set current weapon
 @param iSlot A const int variable which contains the weapon info to check for. 0 == Primary, 1 == Secondary
 */
void CPlayer3D::SetCurrentWeapon(const int iSlot)
{
	iCurrentWeapon = iSlot;
}

/**
 @brief Discharge weapon
 @param iSlot A const int variable which contains the weapon info to check for. 0 == Primary, 1 == Secondary
 @return A bool variable to indicate this method successfully completed its tasks
 */
bool CPlayer3D::DischargeWeapon(void) const
{
	if ((iCurrentWeapon == 0) && (cPrimaryWeapon))
	{
		return cPrimaryWeapon->Discharge(vec3Position, vec3Front, (CSolidObject*)this);
	}
	else if ((iCurrentWeapon == 1) && (cSecondaryWeapon))
	{
		return cSecondaryWeapon->Discharge(vec3Position, vec3Front, (CSolidObject*)this);
	}
	return false;
}

/**
 @brief Set to Jump
 */
void CPlayer3D::SetToJump(void)
{
	if (cPhysics3D.GetStatus() == CPhysics3D::STATUS::IDLE)
	{
		cPhysics3D.SetStatus(CPhysics3D::STATUS::JUMP);
		cPhysics3D.SetNewJump(true);
		cPhysics3D.SetInitialVelocity(glm::vec3(0.0f, 4.0f, 0.0f));
	}
}

/**
 @brief Set to Fall
 */
void CPlayer3D::SetToFall(void)
{
	if (cPhysics3D.GetStatus() == CPhysics3D::STATUS::JUMP)
	{
		cPhysics3D.SetStatus(CPhysics3D::STATUS::FALL);
	}
	else if (cPhysics3D.GetStatus() == CPhysics3D::STATUS::IDLE)
	{
		cPhysics3D.SetStatus(CPhysics3D::STATUS::FALL);
	}
}

/**
 @brief Set to Idle
 */
void CPlayer3D::SetToIdle(void)
{
	if (cPhysics3D.GetStatus() == CPhysics3D::STATUS::FALL)
	{
		cPhysics3D.SetStatus(CPhysics3D::STATUS::IDLE);
	}
	else if (cPhysics3D.GetStatus() == CPhysics3D::STATUS::FALL)
	{
		cPhysics3D.SetStatus(CPhysics3D::STATUS::IDLE);
	}
}

/**
 @brief Processes input received from any keyboard-like input system as player movements. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
 @param direction A const Player_Movement variable which contains the movement direction of the camera
 @param deltaTime A const float variable which contains the delta time for the realtime loop
 */
void CPlayer3D::ProcessMovement(const PLAYERMOVEMENT direction, const float deltaTime)
{
	// Since W-A-S-D was pressed, then set this bool flag to true
	bMoved = true;

	// Calculate the velocity
	float velocity = fMovementSpeed * deltaTime;

	vec3Front.y = 0.0f;
	vec3Right.y = 0.0f;
	// Update the player's position
	if (direction == PLAYERMOVEMENT::FORWARD)
		vec3Position += vec3Front * velocity;
	if (direction == PLAYERMOVEMENT::BACKWARD)
		vec3Position -= vec3Front * velocity;
	if (direction == PLAYERMOVEMENT::LEFT)
		vec3Position -= vec3Right * velocity;
	if (direction == PLAYERMOVEMENT::RIGHT)
		vec3Position += vec3Right * velocity;
	
	//// Snap to the terrain height
	//vec3Position.y = cTerrainManager->GetHeight(vec3Position.x, vec3Position.z) + fHeightOffset;

}

/**
 @brief Processes input received from a mouse input system as player rotation. Expects the offset value in both the x and y direction.
 @param xoffset A const float variable which contains the x axis of the mouse movement
 @param yoffset A const float variable which contains the y axis of the mouse movement
 @param constrainPitch A const bool variable which indicates if the pitch will be constrained at upright positions
 */
void CPlayer3D::ProcessRotate(float fXOffset, float fYOffset, const bool constrainPitch)
{
	fXOffset *= fMouseSensitivity;
	fYOffset *= fMouseSensitivity;

	fYaw += fXOffset;
	fPitch += fYOffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
		if (fPitch > 89.0f)
			fPitch = 89.0f;
		if (fPitch < -89.0f)
			fPitch = -89.0f;
	}

	// Update vec3Front, Right and vec3Up Vectors using the updated Euler angles
	UpdatePlayerVectors();
}

/**
 @brief Toggle the player's posture
 */
void CPlayer3D::TogglePosture(void)
{
	switch (iCurrentPosture)
	{
	case PLAYERPOSTURE::STANDING:
		iNextPosture = PLAYERPOSTURE::CROUCH;
		break;
	case PLAYERPOSTURE::CROUCH:
		if (iPreviousPosture == PLAYERPOSTURE::STANDING)
			iNextPosture = PLAYERPOSTURE::PRONE;
		else if(iPreviousPosture == PLAYERPOSTURE::PRONE)
			iNextPosture = PLAYERPOSTURE::STANDING;
		break;
	case PLAYERPOSTURE::PRONE:
		iNextPosture = PLAYERPOSTURE::CROUCH;
		break;
	default:
		break;
	}
}


/**
 @brief Update the elapsed time
 @param dt A const double variable containing the elapsed time since the last frame
 @return A bool variable to indicate this method successfully completed its tasks
 */
bool CPlayer3D::Update(const double dElapsedTime)
{
	if (cPrimaryWeapon)
		cPrimaryWeapon->Update(dElapsedTime);
	if (cSecondaryWeapon)
		cSecondaryWeapon->Update(dElapsedTime);

	if (cPhysics3D.GetStatus() == CPhysics3D::STATUS::IDLE)
	{
		if (bMoved == true)
		{
			// Check if the player walked off a platform
			float fCheckHeight = cTerrainManager->GetHeight(vec3Position.x, vec3Position.z) + fHeightOffset;

			// If the player is below the terrain, then snap to the terrain height
			if (vec3Position.y - fCheckHeight > 0.1f)
			{
				float param_t;
				glm::vec3 rayDirection = glm::vec3(0.0f, -1.0f, 0.0f);
				if (CSolidObjectManager::GetInstance()->CheckForCollision(	vec3Position, 
																			rayDirection,
 																			param_t) == true)
				{
					if (param_t > fHeightOffset)
						cPhysics3D.SetStatus(CPhysics3D::STATUS::FALL);
				}
				else
				{
					cPhysics3D.SetStatus(CPhysics3D::STATUS::FALL);
				}
			}
			else
			{
				vec3Position.y = fCheckHeight;
			}
		}

		// Update the posture
		UpdatePosture(dElapsedTime);
	}
	else
	{
		// Update the Jump/Fall
		UpdateJumpFall(dElapsedTime);
		// Constraint the player's position
		SnapToTerrain();
	}

	// If the camera is attached to this player, then update the camera
	if (cCamera)
	{
		cCamera->vec3Position = vec3Position;
		cCamera->vec3Front = vec3Front;
		cCamera->vec3Up = vec3Up;
		cCamera->vec3Right = vec3Right;
		cCamera->fYaw = fYaw;
		cCamera->fPitch = fPitch;
		// We should not update the fZoom as this will affect the Frustum Culling and spatial partition
		// The player should still have the default view of the environment
		//fZoom = cCamera->fZoom;
	}

	// Update the parent class
	CSolidObject::Update(dElapsedTime);

	// Reset bMoved to false for the next frame
	bMoved = false;

	// Update the model
	model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
	model = glm::translate(model, vec3Position);
	model = glm::scale(model, vec3Scale);
	model = glm::rotate(model, glm::radians(-fYaw), glm::vec3(0.0f, 1.0f, 0.0f));

	return true;
}

/**
 @brief Calculates the front vector from the Camera's (updated) Euler Angles
 */
void CPlayer3D::UpdatePlayerVectors(void)
{
	// Calculate the new vec3Front vector
	glm::vec3 front;
	front.x = cos(glm::radians(fYaw)) * cos(glm::radians(fPitch));
	front.y = sin(glm::radians(fPitch));
	front.z = sin(glm::radians(fYaw)) * cos(glm::radians(fPitch));
	vec3Front = glm::normalize(front);
	// Also re-calculate the Right and Up vector
	// Normalize the vectors, because their length gets closer to 0 the more 
	// you look up or down which results in slower movement.
	vec3Right = glm::normalize(glm::cross(vec3Front, vec3WorldUp));

	// If the camera is attached to this player, then update the camera
	if (cCamera)
	{
		cCamera->vec3Front = vec3Front;
		cCamera->vec3Up = vec3Up;
		cCamera->vec3Right = vec3Right;
		cCamera->fYaw = fYaw;
		cCamera->fPitch = fPitch;
	}
}

/**
 @brief PreRender Set up the OpenGL display environment before rendering
 */
void CPlayer3D::PreRender(void)
{
	CSolidObject::PreRender();
}

/**
 @brief Render Render this instance
 */
void CPlayer3D::Render(void)
{
	// Don't render the player if the camera is attached to it
	if (cCamera)
		return;

	CSolidObject::Render();
}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CPlayer3D::PostRender(void)
{
	CSolidObject::PostRender();
}

/**
 @brief Snap the player's position to the terrain
 */
void CPlayer3D::SnapToTerrain(void)
{
	//// If the player is not jumping nor falling, then we snap his position to the terrain
	//if (cPhysics3D.GetStatus() == CPhysics3D::STATUS::IDLE)
	//{
	//	vec3Position.y = cTerrainManager->GetHeight(vec3Position.x, vec3Position.z) + fHeightOffset;
	//}
	//else
	if (cPhysics3D.GetStatus() == CPhysics3D::STATUS::FALL)
	{
		// If the player is jumping or falling, then we only snap his position 
		// if the player's y-coordinate is below the cTerrainManager 
		float fCheckHeight = cTerrainManager->GetHeight(vec3Position.x, vec3Position.z) + fHeightOffset;

		// If the player is below the terrain, then snap to the terrain height
		if (fCheckHeight > vec3Position.y)
			vec3Position.y = fCheckHeight;
	}
}

/**
 @brief Update Jump or Fall
 @param deltaTime A const float variable which contains the delta time for the realtime loop
 */
void CPlayer3D::UpdateJumpFall(const double dElapsedTime)
{
	if (cPhysics3D.GetStatus() == CPhysics3D::STATUS::JUMP)
	{
		// Update the elapsed time to the physics engine
		cPhysics3D.AddElapsedTime((float)dElapsedTime);
		// Call the physics engine update method to calculate the final velocity and displacement
		cPhysics3D.Update(dElapsedTime);
		// Get the displacement from the physics engine and update the player position
		//vec3Position = vec3Position + cPhysics3D.GetDisplacement();
		vec3Position.y += cPhysics3D.GetFinalVelocity().y * dElapsedTime;

		// If the player is still jumping and the initial velocity has reached zero or below zero, 
		// then it has reach the peak of its jump
		if ((cPhysics3D.GetStatus() == CPhysics3D::STATUS::JUMP) && (cPhysics3D.GetDeltaDisplacement().y <= 0.0f))
		{
			// Set status to fall
			cPhysics3D.SetStatus(CPhysics3D::STATUS::FALL);
		}
	}
	else if (cPhysics3D.GetStatus() == CPhysics3D::STATUS::FALL)
	{
		// Update the elapsed time to the physics engine
		cPhysics3D.AddElapsedTime((float)dElapsedTime);
		// Call the physics engine update method to calculate the final velocity and displacement
		cPhysics3D.Update(dElapsedTime);
		// Get the displacement from the physics engine and update the player position
		//vec3Position = vec3Position + cPhysics3D.GetDisplacement();
		vec3Position.y += cPhysics3D.GetFinalVelocity().y * dElapsedTime;

		// Constaint the player's position to the terrain
		float fCheckHeight = cTerrainManager->GetHeight(vec3Position.x, vec3Position.z) + fHeightOffset;

		// Set the Physics to idle status
		if (fCheckHeight > vec3Position.y)
		{
			vec3Position.y = fCheckHeight;
			cPhysics3D.SetStatus(CPhysics3D::STATUS::IDLE);
		}
	}
}

/**
 @brief Update Posture
 @param deltaTime A const float variable which contains the delta time for the realtime loop
 */
void CPlayer3D::UpdatePosture(const double dElapsedTime)
{
	// Return if there is no chance in posture
	if (iCurrentPosture == iNextPosture)
		return;

	switch (iNextPosture)
	{
	case PLAYERPOSTURE::STANDING:
		fPostureOffset -= 0.01f;
		if (fPostureOffset < 0.0f)
		{
			fPostureOffset = 0.0f;
			iPreviousPosture = iCurrentPosture;
			iCurrentPosture = iNextPosture;
		}
		break;
	case PLAYERPOSTURE::CROUCH:
		if (iCurrentPosture == PLAYERPOSTURE::STANDING)
		{
			fPostureOffset += 0.01f;
			if (fPostureOffset > 0.25f)
			{
				fPostureOffset = 0.25f;
				iPreviousPosture = iCurrentPosture;
				iCurrentPosture = iNextPosture;
			}
		}
		else if (iCurrentPosture == PLAYERPOSTURE::PRONE)
		{
			fPostureOffset -= 0.01f;
			if (fPostureOffset < 0.25f)
			{
				fPostureOffset = 0.25f;
				iPreviousPosture = iCurrentPosture;
				iCurrentPosture = iNextPosture;
			}
		}
		break;
	case PLAYERPOSTURE::PRONE:
		fPostureOffset += 0.01f;
		if (fPostureOffset > 0.35f)
		{
			fPostureOffset = 0.35f;
			iPreviousPosture = iCurrentPosture;
			iCurrentPosture = iNextPosture;
		}
		break;
	default:
		break;
	}

	// Finalise the player's height
	vec3Position.y = vec3Position.y - fPostureOffset;
}

/**
 @brief Print Self
 */
void CPlayer3D::PrintSelf(void)
{
	cout << "CPlayer3D::PrintSelf()" << endl;
	cout << "========================" << endl;
}
