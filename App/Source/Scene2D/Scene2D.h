/**
 CScene2D
 @brief A class which manages the 2D game scene
 By: Toh Da Jun
 Date: Mar 2020
 */
#pragma once

// Include SingletonTemplate
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

// Include Shader Manager
#include "RenderControl\ShaderManager.h"

// Include the Map2D as we will use it to check the player's movements and actions
#include "Map2D.h"
// Include CPlayer2D
#include "Player2D.h"
// Include CEnemy2D
//#include "Enemy2D.h"
// Include CEnemy2DManager
#include "Enemy2DManager.h"

// Include vector
#include <vector>

// Include Keyboard controller
#include "Inputs\KeyboardController.h"

// GUI_Scene2D
#include "GUI_Scene2D.h"

// Game Manager
#include "GameManager.h"

// Include SoundController
#include "..\SoundController\SoundController.h"

class CScene2D : public CSingletonTemplate<CScene2D>
{
	friend CSingletonTemplate<CScene2D>;
public:
	// Init
	bool Init(void);

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

	// The handler containing the instance of the 2D Map
	CMap2D* cMap2D;

	// The handler containing the instance of CPlayer2Ds
	CPlayer2D* cPlayer2D;

	// The handler containing the instance of CEnemy2DManager
	CEnemy2DManager* cEnemy2DManager;

	// Keyboard Controller singleton instance
	CKeyboardController* cKeyboardController;

	// GUI_Scene2D
	CGUI_Scene2D* cGUI_Scene2D;

	// Game Manager
	CGameManager* cGameManager;

	int mapNumber = 0;

	// Maps
	std::string map1 = "Maps/DM2213_Map_Assignment_01.csv";
	std::string map2 = "Maps/DM2213_Map_Assignment_01_2.csv";

	// Constructor
	CScene2D(void);
	// Destructor
	virtual ~CScene2D(void);
};

