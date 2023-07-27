#include "Grid.h"

// Include ShaderManager
#include "RenderControl/ShaderManager.h"

// Include CCollisionManager
#include "Primitives/CollisionManager.h"

// Include CPlayer3D
#include "../Entities/Player3D.h"

// Include CCameraEffectsManager
#include "../CameraEffects/CameraEffectsManager.h"

#include <iostream>
using namespace std;

/**
@brief Constructor
*/
CGrid::CGrid(void)
	: vec3TopRight(glm::vec3(1.0f))
	, vec3BottomLeft(glm::vec3(1.0f))
	, vec4Colour(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f))
	, fLineWidth(3.0f)
	, bVisible(false)
{
	SolidObjectMap.clear();

	// Set the current LOD to the default value
	cCurrentLOD.SetDetailLevel();
}

/**
@brief Destructor
*/
CGrid::~CGrid(void)
{
	Reset();
}

/**
@brief Initialise this grid
@param vec3TopRight A glm::vec3 variable storing the (+x, +y, +z) position of the grid
@param vec3BottomLeft A glm::vec3 variable storing the (-x, -y, -z) position of the grid
*/
void CGrid::Init(glm::vec3 vec3TopRight, glm::vec3 vec3BottomLeft)
{
	this->vec3TopRight = vec3TopRight;
	this->vec3BottomLeft = vec3BottomLeft;

	// set up vertex data (and buffer(s)) and configure vertex attributes
	float vertices[] = {
		vec3BottomLeft.x, vec3BottomLeft.y, vec3BottomLeft.z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		vec3TopRight.x, vec3BottomLeft.y, vec3BottomLeft.z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		vec3TopRight.x, vec3BottomLeft.y, vec3TopRight.z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		vec3TopRight.x, vec3BottomLeft.y, vec3TopRight.z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		vec3BottomLeft.x, vec3BottomLeft.y, vec3TopRight.z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		vec3BottomLeft.x, vec3BottomLeft.y, vec3BottomLeft.z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Set the current LOD to the default value
	cCurrentLOD.SetDetailLevel();
}

/**
@brief Initialise this grid
@param vec3TopRight A glm::vec3 variable storing the (+x, +y, +z) position of the grid
@param vec3BottomLeft A glm::vec3 variable storing the (-x, -y, -z) position of the grid
@param fTopLeftHeight A float variable storing the height at (+x, y, -z) position of the grid
@param fBottomRightHeight A float variable storing the height at (-x, y, -z) position of the grid
*/
void CGrid::Init(glm::vec3 vec3TopRight, glm::vec3 vec3BottomLeft, float fTopLeftHeight, float fBottomRightHeight)
{
	this->vec3TopRight = vec3TopRight;
	this->vec3BottomLeft = vec3BottomLeft;

	// set up vertex data (and buffer(s)) and configure vertex attributes
	float vertices[] = {
		vec3BottomLeft.x, vec3BottomLeft.y, vec3BottomLeft.z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		vec3TopRight.x, fBottomRightHeight, vec3BottomLeft.z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		vec3TopRight.x, vec3TopRight.y, vec3TopRight.z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		vec3TopRight.x, vec3TopRight.y, vec3TopRight.z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		vec3BottomLeft.x, fTopLeftHeight, vec3TopRight.z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		vec3BottomLeft.x, vec3BottomLeft.y, vec3BottomLeft.z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

/**
@brief Update this class instance
@param dElapsedTime A const double containing the elapsed time since the last frame
*/
bool CGrid::Update(const double dElapsedTime)
{
	std::map<unsigned int, CSolidObject*>::iterator it, it_other, end;
	glm::vec3 lowerBoxMin, lowerBoxMax, upperBoxMin, upperBoxMax;
	
	// Boolean variable to check if the player has collided with another CSolidObject
	bool bResult = false;

	// Check for collisions between CSolidObjects within this grid
	end = SolidObjectMap.end();
	for (it = SolidObjectMap.begin(); it != end; ++it)
	{
		// If the entity is not active, then skip it
		if ((it->second)->GetStatus() == false)
			continue;

		for (it_other = it; it_other != end; ++it_other)
		{
			// If the entity is not active, then skip it
			if ((it_other->second)->GetStatus() == false)
				continue;

			// If the 2 entities to check are the same, then skip this iteration
			if (it_other->second == it->second)
				continue;

			//// Prepare the 2 bounding boxes for calculation 
			//// with consideration for their heights
			//if ((*it)->GetPosition().y < (*it_other)->GetPosition().y)
			//{
			lowerBoxMin = (it->second)->GetPosition() + (it->second)->boxMin;
			lowerBoxMax = (it->second)->GetPosition() + (it->second)->boxMax;
			upperBoxMin = (it_other->second)->GetPosition() + (it_other->second)->boxMin;
			upperBoxMax = (it_other->second)->GetPosition() + (it_other->second)->boxMax;
			//}
			//else
			//{
			//	lowerBoxMin = (*it_other)->GetPosition() + (*it_other)->boxMin;
			//	lowerBoxMax = (*it_other)->GetPosition() + (*it_other)->boxMax;
			//	upperBoxMin = (*it)->GetPosition() + (*it)->boxMin;
			//	upperBoxMax = (*it)->GetPosition() + (*it)->boxMax;
			//}

			// Check for collisions between the 2 entities
			if (CCollisionManager::BoxBoxCollision(lowerBoxMin, lowerBoxMax,
				upperBoxMin, upperBoxMax) == true)
			{
				// Check if a movable entity collides with another movable entity
				if (
					((it->second)->GetType() == CSolidObject::TYPE::PLAYER)
					&&
					(((it_other->second)->GetType() >= CSolidObject::TYPE::NPC) &&
						((it_other->second)->GetType() <= CSolidObject::TYPE::OTHERS))
					)
				{
					(it->second)->RollbackPosition();
					(it_other->second)->RollbackPosition();
					bResult = true;
					cout << "** Collision between Player and an Entity ***" << endl;
					break;
				}
				else if (
					(((it->second)->GetType() >= CSolidObject::TYPE::NPC) &&
						((it->second)->GetType() <= CSolidObject::TYPE::OTHERS))
					&&
					(((it_other->second)->GetType() >= CSolidObject::TYPE::NPC) &&
						((it_other->second)->GetType() <= CSolidObject::TYPE::OTHERS))
					)
				{
					(it->second)->RollbackPosition();
					(it_other->second)->RollbackPosition();
					cout << "** Collision between 2 Entities ***" << endl;
					break;
				}
				// Check if a movable entity collides with a non-movable entity
				if (
					(((it->second)->GetType() >= CSolidObject::TYPE::PLAYER) &&
						((it->second)->GetType() <= CSolidObject::TYPE::OTHERS))
					&&
					((it_other->second)->GetType() == CSolidObject::TYPE::STRUCTURE)
					)
				{
					(it->second)->RollbackPosition();
					if (((it->second)->GetType() == CSolidObject::TYPE::PLAYER))
					{
						cout << "Player collide with structure: " << (it->second)->GetPosition().y << endl;
						//((CPlayer3D*)(*it))->SetPosition(glm::vec3((*it)->GetPosition().x, (*it)->GetPosition().y + 0.01f, (*it)->GetPosition().z));
						((CPlayer3D*)(it->second))->SetToIdle();

						// Check if the collision is in top-bottom direction
						bResult = true;
					}
					cout << "** Collision between Entity and Structure ***" << endl;
					break;
				}
			}
		}
	}

	// If the player had collided with another CSolidObject, then set the bloodscreen to true
	if (bResult == true)
	{
		CCameraEffectsManager::GetInstance()->Get("BloodScreen")->SetStatus(true);
	}

	// If this grid is visible...
	if (bVisible)
	{
		// Update the LOD of all CSolidObject in this Grid
		end = SolidObjectMap.end();
		for (it = SolidObjectMap.begin(); it != end; ++it)
		{
			// If this CSolidObject has LOD, then assign a new LOD value to it
			if ((it->second)->GetLODStatus()==true)
				(it->second)->SetDetailLevel(cCurrentLOD.GetDetailLevel());
		}

		return true;
	}

	return false;
}

/**
@brief PreRender
*/
void CGrid::PreRender(void)
{
	// change depth function so depth test passes when values are equal to depth buffer's content
	glDepthFunc(GL_LEQUAL);

	// Activate shader
	CShaderManager::GetInstance()->Use(sShaderName);

	glGetFloatv(GL_LINE_WIDTH, &fPreviousLineWidth);

	//if ((int)ListOfObjects.size() > 0)
	//	glLineWidth(fLineWidth * 3);
	//else
	glLineWidth(fLineWidth);
}

/**
@brief Render
*/
void CGrid::Render(void)
{
	CShaderManager::GetInstance()->activeShader->setMat4("model", model);
	CShaderManager::GetInstance()->activeShader->setMat4("view", view);
	CShaderManager::GetInstance()->activeShader->setMat4("projection", projection);

	// render box
	glBindVertexArray(VAO);
	glDrawArrays(GL_LINE_LOOP, 0, 6);
	glBindVertexArray(0);

	// Render the CSolidObjects in this CGrid
	std::map<unsigned int, CSolidObject*>::iterator it, end;
	end = SolidObjectMap.end();
	for (it = SolidObjectMap.begin(); it != end; ++it)
	{
		(it->second)->SetView(view);
		(it->second)->SetProjection(projection);
		(it->second)->PreRender();
		(it->second)->Render();
		(it->second)->PostRender();

		// Set it's visibility to false after rendering as this CSolidObject may be rendered in other Grids
		(it->second)->SetVisibility(false);
	}
}

/**
@brief PostRender
*/
void CGrid::PostRender(void)
{
	glLineWidth(fPreviousLineWidth);
}

// Reset the list of CSolidObjects in this CGrid
void CGrid::Reset(void)
{
	// Delete all CSolidObjects from the map
	SolidObjectMap.clear();
}

/**
@brief Add a new object to this grid
@param cSolidObject A CSolidObject* variable storing the Scene Node to add to this grid
*/
void CGrid::Add(CSolidObject* cSolidObject)
{
	if (IsHere(cSolidObject) == false)
	{
		SolidObjectMap[cSolidObject->GetID()] = cSolidObject;
	}
}

/**
@brief Remove but not delete an object from this grid
@param cSolidObject A CSolidObject* variable storing the Scene Node to remove from this grid
*/
bool CGrid::Remove(CSolidObject* cSolidObject)
{
	if (IsHere(cSolidObject) == true)
	{
		SolidObjectMap.erase(cSolidObject->GetID());
		return true;
	}
	return false;
}

/**
@brief Check if an object is in this grid
@param cSolidObject A CSolidObject* variable storing the Scene Node to check for
*/
bool CGrid::IsHere(CSolidObject* cSolidObject) const
{
	if (SolidObjectMap.count(cSolidObject->GetID()) == 0)
		return false;
	return true;
}

/**
@brief Get number of objects in this grid
*/
int CGrid::GetNumOfObject(void) const
{
	return SolidObjectMap.size();
}

/**
@brief Update the LOD for this grid
@param fDistance A const float variable storing the distance from the camera to this grid
*/
void CGrid::UpdateLOD(const float fDistance)
{
	cCurrentLOD.Update(fDistance);
}

/**
@brief Set the value of bVisible
@param bVisible A const bool variable storing the new value of bVisible
*/
void CGrid::SetVisiblity(const bool bVisible)
{
	// Check if there is a change in the visibility of this grid
	if (this->bVisible != bVisible)
	{
		// Update the boolean flag to the new value
		this->bVisible = bVisible;
	}

	// If this grid is going to be visible, then set the CSolidObjects in it to be visible too.
	if (bVisible)
	{
		// Update the visibility of all CSolidObjects in this Grid
		std::map<unsigned int, CSolidObject*>::iterator it, end;
		end = SolidObjectMap.end();
		for (it = SolidObjectMap.begin(); it != end; ++it)
		{
			(it->second)->SetVisibility(bVisible);
		}
	}
}

/**
@brief Get the value of bVisible
*/
const bool CGrid::GetVisibility(void) const
{
	return bVisible;
}

/**
@brief PrintSelf
*/
void CGrid::PrintSelf(void)
{
	if (SolidObjectMap.size() == 0)
		return;

	cout << "CGrid::PrintSelf()" << endl;
	cout << "\tList of objects in this Grid #" << GetID() << ":" << endl;
	if (bVisible)
		cout << "\tVisibility Status: Yes" << endl;
	else
		cout << "\tVisibility Status: No" << endl;
	cout << "\t------------------------------------------------------------------------" << endl;
	std::map<unsigned int, CSolidObject*>::iterator it, end;
	end = SolidObjectMap.end();
	for (it = SolidObjectMap.begin(); it != end; ++it)
	{
		cout << "#" << (it->second)->GetID() << ":" << endl;
		(it->second)->PrintSelf();
	}
	cout << "\t------------------------------------------------------------------------" << endl;
}
