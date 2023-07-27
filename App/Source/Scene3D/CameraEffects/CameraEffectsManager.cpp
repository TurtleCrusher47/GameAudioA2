/**
 CCameraEffectsManager
 By: Toh Da Jun
 Date: Apr 2020
 */

#include "CameraEffectsManager.h"

 // Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

// Include Settings
#include "GameControl/Settings.h"

#include <iostream>
using namespace std;

/**
 @brief Default Constructor
 */
CCameraEffectsManager::CCameraEffectsManager(void)
{
}

/**
 @brief Destructor
 */
CCameraEffectsManager::~CCameraEffectsManager(void)
{
	// Remove all CCameraEffects
	std::map<std::string, CCameraEffects*>::iterator it, end;
	end = CameraEffectsMap.end();
	for (it = CameraEffectsMap.begin(); it != end; ++it)
	{
		delete it->second;
	}
	// Clear the map
	CameraEffectsMap.clear();
}

/**
 @brief Initialise this class instance
 */
bool CCameraEffectsManager::Init(void)
{
	CameraEffectsMap.clear();

	return true;
}

/**
 @brief Check if a CCameraEffects* exists in the map
 @param _name A const std::string& variable contains the name of the Camera Effects
 @return A bool variable to indicate this method successfully completed its tasks
 */
bool CCameraEffectsManager::CheckGameStateExist(const std::string& _name)
{
	return CameraEffectsMap.count(_name) != 0;
}

/**
 @brief Add a CCameraEffects* to this class instance
 @param _name A const std::string& variable contains the name of the Camera Effects
 @param cCameraEffects A CCameraEffects* variable contains the camera effects to add in
 @return A bool variable to indicate this method successfully completed its tasks
 */
bool CCameraEffectsManager::Add(const std::string& _name, CCameraEffects* cCameraEffects)
{
	// Check if this _name already exists in the map...
	if (CheckGameStateExist(_name))
	{
		// If it does not exist, then unable to proceed
		cout << "CCameraEffectsManager::Add - Duplicate scene name provided" << endl;
		return false;
	}

	// Check if the _scene is valid
	if (cCameraEffects == nullptr)
	{
		cout << "CCameraEffectsManager::Add - Invalid CCameraEffects pointer" << endl;
		return false;
	}

	// If nothing is wrong, then add the scene to our map
	CameraEffectsMap[_name] = cCameraEffects;

	return true;
}

/**
 @brief Remove a CCameraEffects* from this class instance
 @param _name A const std::string& variable contains the name of the Camera Effects to erase
 @return A bool variable to indicate this method successfully completed its tasks
 */
bool CCameraEffectsManager::Erase(const std::string& _name)
{
	// Check if this _name does not exists in the map...
	if (!CheckGameStateExist(_name))
	{
		// If it does not exist, then unable to proceed
		cout << "CCameraEffectsManager::Erase - scene name does not exists" << endl;
		return false;
	}

	// Find the CGameState*
	CCameraEffects* target = CameraEffectsMap[_name];

	// Delete and remove from our map
	delete target;
	CameraEffectsMap.erase(_name);

	return true;
}

/**
 @brief Get a CCameraEffects* from this class instance
 @param _name A const std::string& variable contains the name of the Camera Effects
 @return A CCameraEffects* variable
 */
CCameraEffects* CCameraEffectsManager::Get(const std::string& _name)
{
	// Check if this _name does not exists in the map...
	if (!CheckGameStateExist(_name))
	{
		// If it does not exist, then unable to proceed
		cout << "CCameraEffectsManager::Get - scene name does not exists" << endl;
		return nullptr;
	}

	// Return the CGameState*
	return CameraEffectsMap[_name];
}

/**
 @brief Update
 @param dElapsedTime A const double variable contains the time since the last frame
 @return A bool variable to indicate this method successfully completed its tasks
 */
bool CCameraEffectsManager::Update(const double dElapsedTime)
{
	// Update all CCameraEffects
	std::map<std::string, CCameraEffects*>::iterator it, end;
	end = CameraEffectsMap.end();
	for (it = CameraEffectsMap.begin(); it != end; ++it)
	{
		if (it->second->GetStatus() == true)
		{
			it->second->Update(dElapsedTime);
		}
	}

	return true;
}

/**
 @brief Render this class instance
 */
void CCameraEffectsManager::Render(void)
{
	// Update all CCameraEffects
	std::map<std::string, CCameraEffects*>::iterator it, end;
	end = CameraEffectsMap.end();
	for (it = CameraEffectsMap.begin(); it != end; ++it)
	{
		if (it->second->GetStatus() == true)
		{
			it->second->PreRender();
			it->second->Render();
			it->second->PostRender();
		}
	}
}

/**
 @brief Print Self
 */
void CCameraEffectsManager::PrintSelf(void)
{
	cout << "CCameraEffectsManager::PrintSelf()" << endl;
	cout << "==================================" << endl;

	// Print out all CCameraEffects
	int iCounter = 0;
	std::map<std::string, CCameraEffects*>::iterator it, end;
	end = CameraEffectsMap.end();
	for (it = CameraEffectsMap.begin(); it != end; ++it)
	{
		cout << ++iCounter << " : " << it->first << ", Status = " << it->second->GetStatus() << endl;
	}

}