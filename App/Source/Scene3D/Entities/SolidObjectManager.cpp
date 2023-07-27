/**
 CSolidObjectManager
 By: Toh Da Jun
 Date: Apr 2020
 */

#include "SolidObjectManager.h"

// Include CCollisionManager
#include "Primitives/CollisionManager.h"

// Include CCameraEffectsManager
#include "../CameraEffects/CameraEffectsManager.h"

// Include CPlayer3D
#include "Player3D.h"

#include <limits>
#include <algorithm>

#include <iostream>
using namespace std;

/**
 @brief Default Constructor
 */
CSolidObjectManager::CSolidObjectManager(void)
	: model(glm::mat4(1.0f))
	, view(glm::mat4(1.0f))
	, projection(glm::mat4(1.0f))
	, cProjectileManager(NULL)
{
}

/**
 @brief Destructor
 */
CSolidObjectManager::~CSolidObjectManager(void)
{
	// We won't delete this since it was created elsewhere
	cProjectileManager = NULL;

	// Remove all CSolidObject
	std::list<CSolidObject*>::iterator it = lSolidObject.begin(), end = lSolidObject.end();
	while (it != end)
	{
		// Delete if done
		delete *it;
		it = lSolidObject.erase(it);
	}
}

/**
 @brief Initialise this class instance
 @return A bool variable to indicate this method successfully completed its tasks
 */
bool CSolidObjectManager::Init(void)
{
	lSolidObject.clear();

	cProjectileManager = CProjectileManager::GetInstance();

	return true;
}

/**
 @brief Set model
 @param model A const glm::mat4 variable containing the model for this class instance
 */
void CSolidObjectManager::SetModel(const glm::mat4 model)
{
	this->model = model;
}

/**
 @brief Set view
 @param view A const glm::mat4 variable containing the model for this class instance
 */
void CSolidObjectManager::SetView(const glm::mat4 view)
{
	this->view = view;
}

/**
 @brief Set projection
 @param projection A const glm::mat4 variable containing the model for this class instance
 */
void CSolidObjectManager::SetProjection(const glm::mat4 projection)
{
	this->projection = projection;
}

/**
 @brief Add a CSolidObject* to this class instance
 @param cSolidObject The CSolidObject* variable to be added to this class instance
 */
void CSolidObjectManager::Add(CSolidObject* cSolidObject)
{
	lSolidObject.push_back(cSolidObject);
}

/**
 @brief Remove a CSolidObject* from this class instance
 @param cSolidObject The CSolidObject* variable to be erased from this class instance
 @return A bool variable to indicate this method successfully completed its tasks
 */
bool CSolidObjectManager::Erase(CSolidObject* cSolidObject)
{
	// Find the entity's iterator
	std::list<CSolidObject*>::iterator findIter = std::find(lSolidObject.begin(), lSolidObject.end(), cSolidObject);

	// Delete the entity if found
	if (findIter != lSolidObject.end())
	{
		// Delete the CSolidObject
		//delete *findIter;
		// Go to the next iteration after erasing from the list
		findIter = lSolidObject.erase(findIter);
		return true;
	}
	// Return false if not found
	return false;
}

/**
 @brief Update the elapsed time
 @param dt A const double variable containing the elapsed time since the last frame
 @return A bool variable to indicate this method successfully completed its tasks
 */
bool CSolidObjectManager::Update(const double dElapsedTime)
{
	std::list<CSolidObject*>::iterator it, end;

	// Update all CSolidObject
	end = lSolidObject.end();
	for (it = lSolidObject.begin(); it != end; ++it)
	{
		(*it)->Update(dElapsedTime);
	}

	return true;
}

/**
 @brief Collision Check for all entities and projectiles
 @return A bool variable to indicate this method successfully completed its tasks. True == collision detected, false == no collision detected.
 */
bool CSolidObjectManager::CheckForCollision(void)
{
	bool bResult = false;

	std::list<CSolidObject*>::iterator it, end;
	std::list<CSolidObject*>::iterator it_other;

	//// Check for collisions among entities
	//end = lSolidObject.end();
	//for (it = lSolidObject.begin(); it != end; ++it)
	//{
	//	// If the entity is not active, then skip it
	//	if ((*it)->GetStatus() == false)
	//		continue;

	//	for (it_other = it; it_other != end; ++it_other)
	//	{
	//		// If the entity is not active, then skip it
	//		if ((*it_other)->GetStatus() == false)
	//			continue;

	//		// If the 2 entities to check are the same, then skip this iteration
	//		if (it_other == it)
	//			continue;

	//		//// Prepare the 2 bounding boxes for calculation 
	//		//// with consideration for their heights
	//		//if ((*it)->GetPosition().y < (*it_other)->GetPosition().y)
	//		//{
	//			lowerBoxMin = (*it)->GetPosition() + (*it)->boxMin;
	//			lowerBoxMax = (*it)->GetPosition() + (*it)->boxMax;
	//			upperBoxMin = (*it_other)->GetPosition() + (*it_other)->boxMin;
	//			upperBoxMax = (*it_other)->GetPosition() + (*it_other)->boxMax;
	//		//}
	//		//else
	//		//{
	//		//	lowerBoxMin = (*it_other)->GetPosition() + (*it_other)->boxMin;
	//		//	lowerBoxMax = (*it_other)->GetPosition() + (*it_other)->boxMax;
	//		//	upperBoxMin = (*it)->GetPosition() + (*it)->boxMin;
	//		//	upperBoxMax = (*it)->GetPosition() + (*it)->boxMax;
	//		//}

	//		// Check for collisions between the 2 entities
	//		if (CCollisionManager::BoxBoxCollision(	lowerBoxMin, lowerBoxMax,
	//												upperBoxMin, upperBoxMax) == true)
	//		{
	//			// Check if a movable entity collides with another movable entity
	//			if (
	//				((*it)->GetType() == CSolidObject::TYPE::PLAYER)
	//				&&
	//				(((*it_other)->GetType() >= CSolidObject::TYPE::NPC) &&
	//					((*it_other)->GetType() <= CSolidObject::TYPE::OTHERS))
	//				)
	//			{
	//				(*it)->RollbackPosition();
	//				(*it_other)->RollbackPosition();
	//				bResult = true;
	//				cout << "** Collision between Player and an Entity ***" << endl;
	//				break;
	//			}
	//			else if (
	//					(((*it)->GetType() >= CSolidObject::TYPE::NPC) &&
	//					((*it)->GetType() <= CSolidObject::TYPE::OTHERS))
	//					&&
	//					(((*it_other)->GetType() >= CSolidObject::TYPE::NPC) &&
	//					((*it_other)->GetType() <= CSolidObject::TYPE::OTHERS))
	//				)
	//			{
	//				(*it)->RollbackPosition();
	//				(*it_other)->RollbackPosition();
	//				cout << "** Collision between 2 Entities ***" << endl;
	//				break;
	//			}
	//			// Check if a movable entity collides with a non-movable entity
	//			if (
	//					(((*it)->GetType() >= CSolidObject::TYPE::PLAYER) &&
	//						((*it)->GetType() <= CSolidObject::TYPE::OTHERS))
	//					&&
	//					((*it_other)->GetType() == CSolidObject::TYPE::STRUCTURE)
	//				)
	//			{
	//				(*it)->RollbackPosition();
	//				if (((*it)->GetType() == CSolidObject::TYPE::PLAYER))
	//				{
	//					cout << "Player collide with structure: " << (*it)->GetPosition().y << endl;
	//					//((CPlayer3D*)(*it))->SetPosition(glm::vec3((*it)->GetPosition().x, (*it)->GetPosition().y + 0.01f, (*it)->GetPosition().z));
	//					((CPlayer3D*)(*it))->SetToIdle();

	//					// Check if the collision is in top-bottom direction
	//					bResult = true;
	//				}
	//				cout << "** Collision between Entity and Structure ***" << endl;
	//				break;
	//			}
	//		}
	//	}
	//}

	// Check for collisions between entities and projectiles
	end = lSolidObject.end();
	for (it = lSolidObject.begin(); it != end; ++it)
	{
		// If the entity is not active, then skip it
		if ((*it)->GetStatus() == false)
			continue;

		for (unsigned int i = 0; i < cProjectileManager->vProjectile.size(); i++)
		{
			// If the entity is not active, then skip it
			if ((cProjectileManager->vProjectile[i])->GetStatus() == false)
				continue;

			// Use ray tracing to check for collisions between the 2 entities
			if (CCollisionManager::RayBoxCollision((*it)->GetPosition() + (*it)->boxMin,
				(*it)->GetPosition() + (*it)->boxMax,
				(cProjectileManager->vProjectile[i])->GetPreviousPosition(),
				(cProjectileManager->vProjectile[i])->GetPosition()))
			{
				if ((*it)->GetType() == CSolidObject::TYPE::PLAYER)
				{
					// If this projectile is fired by the player, then skip it
					if ((cProjectileManager->vProjectile[i])->GetSource() == (*it))
						continue;
					(cProjectileManager->vProjectile[i])->SetStatus(false);
					cout << "** RayBoxCollision between Player and Projectile ***" << endl;
					bResult = true;
					break;
				}
				else if ((*it)->GetType() == CSolidObject::TYPE::NPC)
				{
					// If this projectile is fired by the NPC, then skip it
					if ((cProjectileManager->vProjectile[i])->GetSource() == (*it))
						continue;
					(*it)->SetStatus(false);
					(cProjectileManager->vProjectile[i])->SetStatus(false);
					cout << "** RayBoxCollision between NPC and Projectile ***" << endl;
					break;
				}
				else if ((*it)->GetType() == CSolidObject::TYPE::STRUCTURE)
				{
					(cProjectileManager->vProjectile[i])->SetStatus(false);
					cout << "** RayBoxCollision between Structure and Projectile ***" << endl;
					break;
				}
			}

			// Check for collisions between the 2 entities
			if (CCollisionManager::BoxBoxCollision((*it)->GetPosition() + (*it)->boxMin,
				(*it)->GetPosition() + (*it)->boxMax,
				(cProjectileManager->vProjectile[i])->GetPosition() + (cProjectileManager->vProjectile[i])->boxMin,
				(cProjectileManager->vProjectile[i])->GetPosition() + (cProjectileManager->vProjectile[i])->boxMax) == true)
			{
				if ((*it)->GetType() == CSolidObject::TYPE::PLAYER)
				{
					(cProjectileManager->vProjectile[i])->SetStatus(false);
					cout << "** BoxBoxCollision between Player and Projectile ***" << endl;
					bResult = true;
					break;
				}
				else if ((*it)->GetType() == CSolidObject::TYPE::NPC)
				{
					(*it)->SetStatus(false);
					(cProjectileManager->vProjectile[i])->SetStatus(false);
					cout << "** BoxBoxCollision between NPC and Projectile ***" << endl;
					break;
				}
				else if ((*it)->GetType() == CSolidObject::TYPE::STRUCTURE)
				{
					(cProjectileManager->vProjectile[i])->SetStatus(false);
					cout << "** BoxBoxCollision between Structure and Projectile ***" << endl;
					break;
				}
			}
		}
	}
	
	if (bResult == true)
	{
		CCameraEffectsManager::GetInstance()->Get("BloodScreen")->SetStatus(true);
	}

	return true;
}

/**
 @brief Collision Check for a ray from the player against all solid objects
 @param rayOrigin A const glm::vec3 which stores the ray origin
 @param rayDirection A const glm::vec3 which stores the ray direction
 @param t A float& variable which returns the parameter t on the ray vector
 @return A bool variable to indicate this method successfully completed its tasks. True == collision detected, false == no collision detected.
 */ 
bool CSolidObjectManager::CheckForCollision(const glm::vec3 rayOrigin,
											const glm::vec3 rayDirection,
											float& t)
{
	t = FLT_MAX;
	float temp_t;

	std::list<CSolidObject*>::iterator it, end;
	std::list<CSolidObject*>::iterator it_other;

	// Check for collisions between entities and projectiles
	end = lSolidObject.end();
	for (it = lSolidObject.begin(); it != end; ++it)
	{
		// If the entity is not active, then skip it
		if ((*it)->GetStatus() == false)
			continue;

		if ((*it)->GetType() == CSolidObject::TYPE::PLAYER)
		{
			continue;
		}

		// Use ray tracing to check for collisions between the 2 entities
		if (CCollisionManager::RayBoxCollision((*it)->GetPosition() + (*it)->boxMin,
												(*it)->GetPosition() + (*it)->boxMax,
												rayOrigin, rayDirection, temp_t))
		{
			// Get the smaller parameter t value of the 2.
			if (temp_t < t)
				t = temp_t;
		}
	}
	// if any collision was detected, then return true
	if (t < FLT_MAX)
		return true;

	return false;
}

/**
 @brief CleanUp all CEntity3Ds which are marked for deletion
 */
void CSolidObjectManager::CleanUp(void)
{
	std::list<CSolidObject*>::iterator it, end;
	it = lSolidObject.begin();
	end = lSolidObject.end();
	while (it != end)
	{
		if ((*it)->IsToDelete())
		{
			// Delete the CSolidObject
			delete *it;
			// Go to the next iteration after erasing from the list
			it = lSolidObject.erase(it);
		}
		else
		{
			// Go to the next iteration
			++it;
		}
	}
}

/**
 @brief Render this class instance
 */
void CSolidObjectManager::Render(void)
{
	// Render all entities
	std::list<CSolidObject*>::iterator it, end;
	end = lSolidObject.end();
	for (it = lSolidObject.begin(); it != end; ++it)
	{
		(*it)->SetView(view);
		(*it)->SetProjection(projection);
		(*it)->PreRender();
		(*it)->Render();
		(*it)->PostRender();
	}
}
