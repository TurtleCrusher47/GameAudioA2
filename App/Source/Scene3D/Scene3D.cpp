/**
 CScene3D
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "Scene3D.h"

// Include GLEW
#ifndef GLEW_STATIC
	#define GLEW_STATIC
	#include <GL/glew.h>
#endif

// Include this for glm::to_string() function
#define GLM_ENABLE_EXPERIMENTAL
#include <includes/gtx/string_cast.hpp>

// Include filesystem to read from a file
#include "System\filesystem.h"

// Include CShaderManager
#include "RenderControl/ShaderManager.h"

// Include CPistol
#include "WeaponInfo\Pistol.h"

// Include CRock3D
#include "Entities/Rock3D.h"
// Include CTreeKabak3D
#include "Entities/TreeKabak3D.h"

// Include CSpinTower
#include "SceneGraph/SpinTower.h"
// Include CSpinTower
#include "Entities/Hut_Concrete.h"

// Include CCameraShake
#include "CameraEffects/CameraShake.h"

#include <iostream>
using namespace std;

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CScene3D::CScene3D(void)
	: cSettings(NULL)
	, cKeyboardController(NULL)
	, cMouseController(NULL)
	, cFPSCounter(NULL)
	, cGUI_Scene3D(NULL)
	, cSolidObjectManager(NULL)
	, cPlayer3D(NULL)
	, cCamera(NULL)
	, cSkyBox(NULL)
	, cTerrainManager(NULL)
	, cSpatialPartition(NULL)
{
}

/**
 @brief Destructor
 */
CScene3D::~CScene3D(void)
{
	// Destroy the Spatial Partition
	if (cSpatialPartition)
	{
		cSpatialPartition->Destroy();
		cSpatialPartition = NULL;
	}

	// Destroy the cTerrainManager
	if (cTerrainManager)
	{
		cTerrainManager->Destroy();
		cTerrainManager = NULL;
	}

	// Destroy the cSkyBox
	if (cSkyBox)
	{
		cSkyBox->Destroy();
		cSkyBox = NULL;
	}

	// Destroy the camera
	if (cCamera)
	{
		cCamera->Destroy();
		cCamera = NULL;
	}

	// Destroy the cEntityManager
	if (cEntityManager)
	{
		cEntityManager->Destroy();
		cEntityManager = NULL;
	}

	// Destroy the cProjectileManager
	if (cProjectileManager)
	{
		cProjectileManager->Destroy();
		cProjectileManager = NULL;
	}

	// Destroy the cPlayer3D
	if (cPlayer3D)
	{
		cSolidObjectManager->Erase(cPlayer3D);
		cPlayer3D->Destroy();
		cPlayer3D = NULL;
	}

	// Destroy the cSolidObjectManager
	if (cSolidObjectManager)
	{
		cSolidObjectManager->Destroy();
		cSolidObjectManager = NULL;
	}

	// Destroy the cGUI_Scene3D
	if (cGUI_Scene3D)
	{
		cGUI_Scene3D->Destroy();
		cGUI_Scene3D = NULL;
	}

	// We won't delete this since it was created elsewhere
	cFPSCounter = NULL;

	// We won't delete this since it was created elsewhere
	cMouseController = NULL;

	// We won't delete this since it was created elsewhere
	cKeyboardController = NULL;
	
	// We won't delete this since it was created elsewhere
	cSettings = NULL;
}

/**
 @brief Init Initialise this instance
 @return true if the initialisation is successful, else false
 */ 
bool CScene3D::Init(void)
{
	cSettings = CSettings::GetInstance();

	// configure global opengl state
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Configure the camera
	cCamera = CCamera::GetInstance();
	cCamera->vec3Position = glm::vec3(0.0f, 0.0f, 0.0f);
	cCamera->vec3Front = glm::vec3(0.0f, 0.0f, -1.0f);

	// Store the keyboard controller singleton instance here
	cKeyboardController = CKeyboardController::GetInstance();

	// Store the mouse controller singleton instance here
	cMouseController = CMouseController::GetInstance();

	// Store the CFPSCounter singleton instance here
	cFPSCounter = CFPSCounter::GetInstance();

	// Load the Environment Entities
	// Load the SkyBox
	cSkyBox = CSkyBox::GetInstance();
	// Set a shader to this class instance of CSkyBox
	cSkyBox->SetShader("Shader3D_SkyBox");
	cSkyBox->Init();

	// Add a set of SkyBox textures into Skybox Class
	if (cSkyBox->Add("Image/SkyBox/skybox_right.jpg",
					 "Image/SkyBox/skybox_left.jpg",
					 "Image/SkyBox/skybox_top.jpg",
					 "Image/SkyBox/skybox_bottom.jpg",
					 "Image/SkyBox/skybox_front.jpg",
					 "Image/SkyBox/skybox_back.jpg") == false)
	{
		cout << "Unable to load SkyBox texture files set #1" << endl;
		return false;
	}

	// Add a set of SkyBox textures into Skybox Class
	if (cSkyBox->Add("Image/SkyBox/skybox1_right.jpg",
					"Image/SkyBox/skybox1_left.jpg",
					"Image/SkyBox/skybox1_top.jpg",
					"Image/SkyBox/skybox1_bottom.jpg",
					"Image/SkyBox/skybox1_front.jpg",
					"Image/SkyBox/skybox1_back.jpg") == false)
	{
		cout << "Unable to load SkyBox texture files set #1" << endl;
		return false;
	}

	// Add a set of SkyBox textures into Skybox Class
	if (cSkyBox->Add("Image/SkyBox/skybox2_right.jpg",
					"Image/SkyBox/skybox2_left.jpg",
					"Image/SkyBox/skybox2_top.jpg",
					"Image/SkyBox/skybox2_bottom.jpg",
					"Image/SkyBox/skybox2_front.jpg",
					"Image/SkyBox/skybox2_back.jpg") == false)
	{
		cout << "Unable to load SkyBox texture files set #1" << endl;
		return false;
	}

	// Load the Terrain
	cTerrainManager = CTerrainManager::GetInstance();
	cTerrainManager->Add("Shader3D_Terrain",
						"Image/Terrain/World/terrain.bmp",
						100.0f, 5.0f, 100.0f);
	//cTerrainManager->Add("Shader3D_Terrain",
	//					"Image/Terrain/World/terrain1.bmp",
	//					100.0f, 5.0f, 100.0f);
	//cTerrainManager->Add("Shader3D_Terrain",
	//					"Image/Terrain/World/terrain2.bmp",
	//					100.0f, 5.0f, 100.0f);

	// Load the movable Entities
	// Initialise the CSolidObjectManager
	cSolidObjectManager = CSolidObjectManager::GetInstance();
	cSolidObjectManager->Init();

	// Initialise the cPlayer3D
	cPlayer3D = CPlayer3D::GetInstance();
	cPlayer3D->SetPosition(glm::vec3(0.0f, 0.5f, 0.0f));
	cPlayer3D->SetShader("Shader3D");
	cPlayer3D->Init();
	cPlayer3D->InitCollider("Shader3D_Line", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	cPlayer3D->AttachCamera(cCamera);

	// Add the cPlayer3D to the cSolidObjectManager
	cSolidObjectManager->Add(cPlayer3D);

	// Initialise the projectile manager
	cProjectileManager = CProjectileManager::GetInstance();
	cProjectileManager->Init();
	cProjectileManager->SetShader("Shader3D");

	// Assign a cPistol to the cPlayer3D
	CPistol* cPistol = new CPistol();
	// Set the position, rotation and scale of this weapon
	cPistol->SetPosition(glm::vec3(0.05f, -0.075f, -0.3f));
	cPistol->SetRotation(3.14159f, glm::vec3(0.0f, 1.0f, 0.0f));
	cPistol->SetScale(glm::vec3(0.75f, 0.75f, 0.75f));
	// Initialise the instance
	cPistol->Init();
	cPistol->SetShader("Shader3D_Model");
	cPlayer3D->SetWeapon(0, cPistol);

	// Initialise the cEnemy3D
	float fCheckHeight = cTerrainManager->GetHeight(0.0f, -10.0f);
	CEnemy3D* cEnemy3D = new CEnemy3D(glm::vec3(0.0f, fCheckHeight, -10.0f));
	cEnemy3D->SetShader("Shader3D");
	cEnemy3D->Init();
	cEnemy3D->InitCollider("Shader3D_Line", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	//cEnemy3D->SetScale(glm::vec3(0.5f));

	// Assign a cPistol to the cEnemy3D
	CPistol* cEnemyPistol = new CPistol();
	// Set the position, rotation and scale of this weapon
	//cEnemyPistol->SetPosition(glm::vec3(0.05f, -0.075f, 0.5f));
	//cEnemyPistol->SetRotation(3.14159f, glm::vec3(0.0f, 1.0f, 0.0f));
	cEnemyPistol->SetScale(glm::vec3(1.75f, 1.75f, 1.75f));
	// Initialise the instance
	cEnemyPistol->Init();
	cEnemyPistol->SetShader("Shader3D_Model");
	cEnemy3D->SetWeapon(0, cEnemyPistol);

	// Add the cEnemy3D to the cSolidObjectManager
	cSolidObjectManager->Add(cEnemy3D);

	// Initialise a CStructure3D
	fCheckHeight = cTerrainManager->GetHeight(2.0f, -2.0f);
	CStructure3D* cStructure3D = new CStructure3D(glm::vec3(2.0f, fCheckHeight, -2.0f));
	cStructure3D->SetShader("Shader3D");
	cStructure3D->Init();
	cStructure3D->InitCollider("Shader3D_Line", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
	//cStructure3D->SetScale(glm::vec3(0.5f));

	// Add the cStructure3D to the cSolidObjectManager
	cSolidObjectManager->Add(cStructure3D);

	// Load the GUI Entities
	// Store the cGUI_Scene3D singleton instance here
	cGUI_Scene3D = CGUI_Scene3D::GetInstance();
	cGUI_Scene3D->Init();

	// Load the non-movable Entities with no collisions
	// Initialise the CEntityManager
	cEntityManager = CEntityManager::GetInstance();
	cEntityManager->Init();

	// Initialise the CRock3D
	CRock3D* cRock3D = new CRock3D();
	cRock3D->SetInstancingMode(true);
	if (cRock3D->IsInstancedRendering() == true)
	{
		cRock3D->SetScale(glm::vec3(5.0f));
		cRock3D->SetNumOfInstance(1000);
		cRock3D->SetSpreadDistance(100.0f);

		cRock3D->SetShader("Shader3D_Instancing");	// FOR INSTANCED RENDERING
	}
	else
	{
		fCheckHeight = cTerrainManager->GetHeight(2.0f, 2.0f);
		cRock3D->SetPosition(glm::vec3(2.0f, fCheckHeight, 2.0f));
		cRock3D->SetScale(glm::vec3(0.5f));
		cRock3D->SetShader("Shader3D_NoColour");			// FOR NORMAL RENDERING
	}
	if (cRock3D->Init() == true)
	{
		cEntityManager->Add(cRock3D);
	}
	else
	{
		delete cRock3D;
	}

	// Initialise the CTreeKabak3D
	CTreeKabak3D* cTreeKabak3D = new CTreeKabak3D(glm::vec3(0.0f, 0.0f, 0.0f));
	cTreeKabak3D->SetInstancingMode(true);
	if (cTreeKabak3D->IsInstancedRendering() == true)
	{
		cTreeKabak3D->SetScale(glm::vec3(1.0f));
		cTreeKabak3D->SetNumOfInstance(1000);
		cTreeKabak3D->SetSpreadDistance(100.0f);

		cTreeKabak3D->SetShader("Shader3D_Instancing");	// FOR INSTANCED RENDERING
	}
	if (cTreeKabak3D->Init() == true)
	{
		cEntityManager->Add(cTreeKabak3D);
	}
	else
	{
		delete cTreeKabak3D;
	}

	// Initialise a CSpinTower
	CSpinTower::Create();

	// Initialise a CHut_Concrete
	fCheckHeight = cTerrainManager->GetHeight(-2.0f, 2.0f);
	CHut_Concrete* cHut_Concrete = new CHut_Concrete(glm::vec3(-2.0f, fCheckHeight, 2.0f));
	cHut_Concrete->SetShader("Shader3D_NoColour");
	cHut_Concrete->SetLODStatus(true);
	cHut_Concrete->Init();
	cHut_Concrete->InitCollider("Shader3D_Line", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

	// Add the cHut_Concrete to the cSolidObjectManager
	cSolidObjectManager->Add(cHut_Concrete);

	// Initialise a Spatial Partition
	cSpatialPartition = CSpatialPartition::GetInstance();
	cSpatialPartition->SetShader("Shader3D_Line");
	cSpatialPartition->SetView(cCamera->GetViewMatrix());
	cSpatialPartition->SetProjection(cCamera->GetPerspectiveMatrix());
	cSpatialPartition->Init(glm::vec3(100.0f, 1.0f, 100.0f), glm::i32vec3(10, 1, 10));

	return true;
}

/**
 @brief Update Update this instance
 @param dElapsedTime A const double variable contains the time since the last frame
 @return A bool variable to indicate this method successfully completed its tasks
*/
bool CScene3D::Update(const double dElapsedTime)
{
	// Store the current position, if rollback is needed.
	cPlayer3D->StorePositionForRollback();
	
	// Get keyboard updates for player3D
	if (CKeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_W))
	{
		cPlayer3D->ProcessMovement(CPlayer3D::PLAYERMOVEMENT::FORWARD, (float)dElapsedTime);
		((CCameraShake*)CCameraEffectsManager::GetInstance()->Get("CameraShake"))->bToBeUpdated = true;
	}
	else if (CKeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_S))
	{
		cPlayer3D->ProcessMovement(CPlayer3D::PLAYERMOVEMENT::BACKWARD, (float)dElapsedTime);
		((CCameraShake*)CCameraEffectsManager::GetInstance()->Get("CameraShake"))->bToBeUpdated = true;
	}
	if (CKeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_A))
	{
		cPlayer3D->ProcessMovement(CPlayer3D::PLAYERMOVEMENT::LEFT, (float)dElapsedTime);
	}
	else if (CKeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_D))
	{
		cPlayer3D->ProcessMovement(CPlayer3D::PLAYERMOVEMENT::RIGHT, (float)dElapsedTime);
	}
	if (CKeyboardController::GetInstance()->IsKeyPressed(GLFW_KEY_SPACE))
		cPlayer3D->SetToJump();
	if (CKeyboardController::GetInstance()->IsKeyPressed(GLFW_KEY_LEFT_CONTROL))
		cPlayer3D->TogglePosture();

	// Get keyboard updates for camera
	if (!cPlayer3D->IsCameraAttached())
	{
		if (CKeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_I))
			cCamera->ProcessKeyboard(CCamera::CAMERAMOVEMENT::FORWARD, (float)dElapsedTime);
		if (CKeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_K))
			cCamera->ProcessKeyboard(CCamera::CAMERAMOVEMENT::BACKWARD, (float)dElapsedTime);
		if (CKeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_J))
			cCamera->ProcessKeyboard(CCamera::CAMERAMOVEMENT::LEFT, (float)dElapsedTime);
		if (CKeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_L))
			cCamera->ProcessKeyboard(CCamera::CAMERAMOVEMENT::RIGHT, (float)dElapsedTime);

		// Get mouse updates
		cCamera->ProcessMouseMovement((float)cMouseController->GetMouseDeltaX(), (float)cMouseController->GetMouseDeltaY());
		cCamera->ProcessMouseScroll((float)cMouseController->GetMouseScrollStatus(CMouseController::SCROLL_TYPE::SCROLL_TYPE_YOFFSET));
	}
	else
	{
		// Get mouse updates
		cPlayer3D->ProcessRotate((float)cMouseController->GetMouseDeltaX(), (float)cMouseController->GetMouseDeltaY());
		cCamera->ProcessMouseScroll((float)cMouseController->GetMouseScrollStatus(CMouseController::SCROLL_TYPE::SCROLL_TYPE_YOFFSET));
	}

	if (CKeyboardController::GetInstance()->IsKeyPressed(GLFW_KEY_0))
	{
		if (cPlayer3D->IsCameraAttached())
		{
			cPlayer3D->AttachCamera();
			cCamera->fZoom = 45.0f;
		}
		else
			cPlayer3D->AttachCamera(cCamera);

		// Reset the key so that it will not repeat until the key is released and pressed again
		CKeyboardController::GetInstance()->ResetKey(GLFW_KEY_0);
	}
	if (CKeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_1))
	{
		cPlayer3D->SetCurrentWeapon(0);
	}
	if (CKeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_2))
	{
		cPlayer3D->SetCurrentWeapon(1);
	}
	if (CKeyboardController::GetInstance()->IsKeyPressed(GLFW_KEY_8))
	{
		bool bStatus = CCameraEffectsManager::GetInstance()->Get("CameraShake")->GetStatus();
		CCameraEffectsManager::GetInstance()->Get("CameraShake")->SetStatus(!bStatus);

		// Reset the key so that it will not repeat until the key is released and pressed again
		CKeyboardController::GetInstance()->ResetKey(GLFW_KEY_8);
	}
	if (CKeyboardController::GetInstance()->IsKeyPressed(GLFW_KEY_9))
	{
		bool bStatus = CCameraEffectsManager::GetInstance()->Get("CrossHair")->GetStatus();
		CCameraEffectsManager::GetInstance()->Get("CrossHair")->SetStatus(!bStatus);

		// Reset the key so that it will not repeat until the key is released and pressed again
		CKeyboardController::GetInstance()->ResetKey(GLFW_KEY_9);
	}
	if (CKeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_R))
	{
		cPlayer3D->GetWeapon()->Reload();
	}

	if (CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_LEFT_BRACKET))
	{
		cSkyBox->SetIndex(cSkyBox->GetIndex() - 1);
		cTerrainManager->SetIndex(cTerrainManager->GetIndex() - 1);
	}
	else if (CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_RIGHT_BRACKET))
	{
		cSkyBox->SetIndex(cSkyBox->GetIndex() + 1);
		cTerrainManager->SetIndex(cTerrainManager->GetIndex() + 1);
	}

	// Get mouse button updates
	if (cMouseController->IsButtonPressed(CMouseController::BUTTON_TYPE::LMB))
	{
		cPlayer3D->DischargeWeapon();
	}
	if (cMouseController->IsButtonPressed(CMouseController::BUTTON_TYPE::RMB))
	{
		// Switch on Scope mode and zoom in
		cCamera->fZoom = 1.0f;
		CCameraEffectsManager::GetInstance()->Get("ScopeScreen")->SetStatus(true);
	}
	else if (cMouseController->IsButtonReleased(CMouseController::BUTTON_TYPE::RMB))
	{
		// Switch off Scope mode and zoom out
		cCamera->fZoom = 45.0f;
		CCameraEffectsManager::GetInstance()->Get("ScopeScreen")->SetStatus(false);
	}

	// Update the Solid Objects
	cSolidObjectManager->Update(dElapsedTime);

	// Update the projectiles
	cProjectileManager->Update(dElapsedTime);

	// Update the entities
	cEntityManager->Update(dElapsedTime);

	// Reset the Spatial Partition and 
	// Add the CSolidObjects back into the Spatial Partition
	cSpatialPartition->Reset(cSolidObjectManager);

	// Update the Spatial Partition
	cSpatialPartition->Update(dElapsedTime);

	// Check for collisions among Entities and also with Projectiles
	// Check for collisions between Entities and Projectiles
	cSolidObjectManager->CheckForCollision();

	// Call the cGUI_Scene3D's update method
	cGUI_Scene3D->Update(dElapsedTime);

	// Post Update the mouse controller
	cMouseController->PostUpdate();

	return true;
}

/**
 @brief PreRender Set up the OpenGL display environment before rendering
 */
void CScene3D::PreRender(void)
{
	// Reset the OpenGL rendering environment
	glLoadIdentity();

	// Clear the screen and buffer
	glClearColor(0.0f, 0.1f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/**
 @brief Render Render this instance
 */
void CScene3D::Render(void)
{
	// Part 1: Render for the minimap by binding to framebuffer and render to colour texture
	//         But the camera is move to top-view of the scene

	// Backup some key settings for the camera and player
	glm::vec3 storePlayerPosition = cPlayer3D->GetPosition();
	float storeCameraYaw = cCamera->fYaw;
	float storeCameraPitch = cCamera->fPitch;
	glm::vec3 storeCameraPosition = cCamera->vec3Position;
	// Adjust camera yaw and pitch so that it is looking from a top-view of the terrain
	cCamera->fYaw += 180.0f;
	cCamera->fPitch = -90.0f;
	// We store the player's position into the camera as we want the minimap to focus on the player
	cCamera->vec3Position = glm::vec3(storePlayerPosition.x, 10.0f, storePlayerPosition.z);
	// Recalculate all the camera vectors. 
	// We disable pitch constrains for this specific case as we want the camera to look straight down
	cCamera->ProcessMouseMovement(0, 0, false);

	// Generate the view and projection
	glm::mat4 playerView = cCamera->GetViewMatrix();
	glm::mat4 playerProjection = glm::perspective(	glm::radians(45.0f),
													(float)cSettings->iWindowWidth / (float)cSettings->iWindowHeight,
													0.1f, 1000.0f);

	// Set the camera parameters back to the previous values
	cCamera->fYaw = storeCameraYaw;
	cCamera->fPitch = storeCameraPitch;
	cCamera->vec3Position = storeCameraPosition;
	cCamera->ProcessMouseMovement(0, 0, true); // call this to make sure it updates its camera vectors, note that we disable pitch constrains for this specific case (otherwise we can't reverse camera's pitch values)

	// Activate the minimap system
	CMinimap::GetInstance()->Activate();
	// Setup the rendering environment
	CMinimap::GetInstance()->PreRender();

	glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

	// Render the Terrain
	cTerrainManager->SetView(playerView);
	cTerrainManager->SetProjection(playerProjection);
	cTerrainManager->PreRender();
	cTerrainManager->Render();
	cTerrainManager->PostRender();

	// Render the entities
	cEntityManager->SetView(playerView);
	cEntityManager->SetProjection(playerProjection);
	cEntityManager->Render();

	// Render the entities for the minimap
	cSolidObjectManager->SetView(playerView);
	cSolidObjectManager->SetProjection(playerProjection);
	cSolidObjectManager->Render();

	// Deactivate the cMinimap so that we can render as per normal
	CMinimap::GetInstance()->Deactivate();

	// Part 2: Render the entire scene as per normal
	// Get the camera view and projection
	glm::mat4 view = CCamera::GetInstance()->GetViewMatrix();
	glm::mat4 projection = CCamera::GetInstance()->GetPerspectiveMatrix();
	//glm::mat4 projection = glm::perspective(	glm::radians(CCamera::GetInstance()->fZoom),
	//											(float)cSettings->iWindowWidth / (float)cSettings->iWindowHeight,
	//											0.1f, 1000.0f);
	glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render the SkyBox
	cSkyBox->SetView(view);
	cSkyBox->SetProjection(projection);
	cSkyBox->PreRender();
	cSkyBox->Render();
	cSkyBox->PostRender();

	// Render the Terrain
	cTerrainManager->SetView(view);
	cTerrainManager->SetProjection(projection);
	cTerrainManager->PreRender();
	cTerrainManager->Render();
	cTerrainManager->PostRender();

	// Render the entities
	cEntityManager->SetView(view);
	cEntityManager->SetProjection(projection);
	cEntityManager->Render();

	//// Render the solid objects
	//cSolidObjectManager->SetView(view);
	//cSolidObjectManager->SetProjection(projection);
	//cSolidObjectManager->Render();

	// Render the Spatial Partition
	//cSpatialPartition->frustumCullingView = cPlayer3D->GetViewMatrix();
	cSpatialPartition->SetView(view);
	cSpatialPartition->SetProjection(projection);
	cSpatialPartition->PreRender();
	cSpatialPartition->Render();
	cSpatialPartition->PostRender();

	// Render the projectiles
	cProjectileManager->SetView(view);
	cProjectileManager->SetProjection(projection);
	cProjectileManager->PreRender();
	cProjectileManager->Render();
	cProjectileManager->PostRender();

	// now draw the mirror quad with screen texture
	// --------------------------------------------
	glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.

	cGUI_Scene3D->SetProjection(projection);
	// Call the cGUI_Scene3D's PreRender()
	cGUI_Scene3D->PreRender();
	// Call the cGUI_Scene3D's Render()
	cGUI_Scene3D->Render();
	// Call the cGUI_Scene3D's PostRender()
	cGUI_Scene3D->PostRender();

	return;
}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CScene3D::PostRender(void)
{
}