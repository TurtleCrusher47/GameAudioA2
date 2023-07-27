/**
 CGUI_Scene3D
 By: Toh Da Jun
 Date: Sep 2021
 */
#pragma once

// Include SingletonTemplate
#include "DesignPatterns/SingletonTemplate.h"

// Include CEntity2D
#include "Primitives/Entity2D.h"

// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

// Include IMGUI
// Important: GLEW and GLFW must be included before IMGUI
#ifndef IMGUI_ACTIVE
#include "GUI\imgui.h"
#include "GUI\backends\imgui_impl_glfw.h"
#include "GUI\backends\imgui_impl_opengl3.h"
#define IMGUI_ACTIVE
#endif

// CSettings
#include "GameControl/Settings.h"

// FPS Counter
#include "TimeControl\FPSCounter.h"

// Include CInventoryManager
#include "..\Scene2D\InventoryManager.h"

// Include CCameraEffectsManager
#include "CameraEffects/CameraEffectsManager.h"
// Include Minimap
#include "Minimap/Minimap.h"

// Include CPlayer3D
#include "Entities\Player3D.h"

#include <string>
using namespace std;

class CGUI_Scene3D : public CSingletonTemplate<CGUI_Scene3D>, public CEntity2D
{
	friend CSingletonTemplate<CGUI_Scene3D>;
public:
	// Init
	bool Init(void);

	// Update
	bool Update(const double dElapsedTime);

	// PreRender
	virtual void PreRender(void);

	// Render
	virtual void Render(void);

	// PostRender
	virtual void PostRender(void);

protected:
	// Constructor
	CGUI_Scene3D(void);

	// Destructor
	virtual ~CGUI_Scene3D(void);

	// FPS Control
	CFPSCounter* cFPSCounter;

	// Flags for IMGUI
	ImGuiWindowFlags window_flags;
	float m_fProgressBar;

	// Handler to the CSettings
	CSettings* cSettings;

	// Handler to the CCameraEffectsManager
	CCameraEffectsManager* cCameraEffectsManager;
	// Handler to the CMinimap
	CMinimap* cMinimap;

	// The handler containing the instance of CInventoryManager
	CInventoryManager* cInventoryManager;
	// The handler containing the instance of CInventoryItem
	CInventoryItem* cInventoryItem;

	// The handler containing the instance of CPlayer3D
	CPlayer3D* cPlayer3D;
};
