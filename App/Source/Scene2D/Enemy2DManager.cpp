/**
 CEnemy2DManager
 By: Toh Da Jun
 Date: March 2023
 */
#include "Enemy2DManager.h"

#include <iostream>
using namespace std;

/**
 @brief Default Constructor
 */
CEnemy2DManager::CEnemy2DManager(void)
	: uiTotalElements(2)
	, uiIndexFirst(0)
	, uiIndexLast(0)
{
}

/**
 @brief Destructor
 */
CEnemy2DManager::~CEnemy2DManager(void)
{
	// Remove all CEnemy2D*
	for (unsigned int i = 0; i < vEnemy2D.size(); i++)
	{
		delete vEnemy2D[i];
		vEnemy2D[i] = NULL;
	}
	vEnemy2D.clear();
}

/**
 @brief Set a shader to this class instance
 @param _name The name of the Shader instance in the CShaderManager
 */
void CEnemy2DManager::SetShader(const std::string& _name)
{
	this->sShaderName = _name;
}

/**
 @brief Initialise this class instance
 @return A bool variable to indicate this method successfully completed its tasks
 */
bool CEnemy2DManager::Init(void)
{
	// Clear the vector
	vEnemy2D.clear();

	// Reserve the size of the vector
	vEnemy2D.reserve(uiTotalElements);

	// Create the instances of CEnemy2D* and store them in the vector
	for (unsigned int i = 0; i < uiTotalElements; i++)
	{
		vEnemy2D.push_back(new CEnemy2D());
	}
	return true;
}

/**
 @brief Activate a CEnemy2D* to this class instance
 @param vec3Position A const glm::vec3 variable containing the source position of the cEnemy2D
 @return A bool variable which is true if the CEnemy2D was activated successfully, otherwise false.
 */
bool CEnemy2DManager::Activate(glm::vec2 vec2Position, int& uiIndex)
{
	// Since a cEnemy2D has been added, we activate the next element in the vector
	vEnemy2D[uiIndexLast]->vec2Position = vec2Position;
	if (vEnemy2D[uiIndexLast]->Init() == false)
		return false;
	vEnemy2D[uiIndexLast]->SetShader(sShaderName);

	// Increase the uiIndexLast by 1 since a cEnemy2D is going to be added
	if (uiIndexLast + 1 == vEnemy2D.size())
		uiIndexLast = 0;
	else
		uiIndexLast++;

	uiIndex = uiIndexLast;

	return true;
}

/**
 @brief Deactivate a CEnemy2D* from this class instance
 @param iIndex A const unsigned int variable containing the index of the cEnemy2D to deactivate
 @return A bool variable to indicate this method successfully completed its tasks
 */
bool CEnemy2DManager::Deactivate(const unsigned int iIndex)
{
	if (iIndex<vEnemy2D.size())
	{
		// Set the cEnemy2D to inactive
		vEnemy2D[iIndex]->SetStatus(false);
		// Return true
		return true;
	}

	// Return false if not found
	return false;
}

/**
 @brief Update this instance
 @param dElapsedTime A const double containing the elapsed time since the last frame
 */
void CEnemy2DManager::Update(const double dElapsedTime)
{
	uiIndexFirst = uiIndexLast + 1;
	if (uiIndexFirst >= vEnemy2D.size())
	{
		uiIndexFirst = 0;

		// CollisionCheck all projectiles against CSolidObject
		// Check the oldest index until the end of 
		for (unsigned int i = uiIndexFirst; i <= uiIndexLast; i++)
		{
			// If this cEnemy2D is not active, then skip it
			if (vEnemy2D[i]->GetStatus() == false)
				continue;

			// Update a cEnemy2D
			vEnemy2D[i]->Update(dElapsedTime);
		}
	}
	else
	{
		// CollisionCheck all projectiles against CSolidObject
		// Check the oldest index until the end of 
		for (unsigned int i = uiIndexFirst; i < vEnemy2D.size(); i++)
		{
			// If this cEnemy2D is not active, then skip it
			if (vEnemy2D[i]->GetStatus() == false)
				continue;

			// Update a cEnemy2D
			vEnemy2D[i]->Update(dElapsedTime);
		}

		// CollisionCheck all projectiles against CSolidObject
		for (unsigned int i = 0; i <= uiIndexLast; i++)
		{
			// If this cEnemy2D is not active, then skip it
			if (vEnemy2D[i]->GetStatus() == false)
				continue;

			// Update a cEnemy2D
			vEnemy2D[i]->Update(dElapsedTime);
		}
	}
}

/**
 @brief PreRender this class instance
 */
void CEnemy2DManager::PreRender(void)
{
}

/**
 @brief Render this class instance
 */
void CEnemy2DManager::Render(void)
{
	// Render all entities
	for (unsigned int i = 0; i < vEnemy2D.size(); i++)
	{
		// If this cEnemy2D is not active, then skip it
		if (vEnemy2D[i]->GetStatus() == false)
			continue;

		vEnemy2D[i]->PreRender();
		vEnemy2D[i]->Render();
		vEnemy2D[i]->PostRender();
	}
}

/**
 @brief PostRender this class instance
 */
void CEnemy2DManager::PostRender(void)
{
}

/**
 @brief PrintSelf
 */
void CEnemy2DManager::PrintSelf(void)
{
	cout << "===============================" << endl;
	cout << "CEnemy2DManager::PrintSelf()" << endl;
	cout << "-------------------------------" << endl;
	// Render all entities
	for (unsigned int i = 0; i < vEnemy2D.size(); i++)
	{
		cout << i << "\t: ";
		vEnemy2D[i]->PrintSelf();
	}
	cout << "===============================" << endl;
}
