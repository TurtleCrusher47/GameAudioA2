/**
 Application.cpp
 @brief This file contains the Application class for this project, which initialises and control the OpenGL environment
*/
#include "Application.h"

#include <Windows.h>
//Include the standard C++ headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#ifndef GLEW_STATIC
	#include <GL/glew.h>
	#define GLEW_STATIC
#endif

// Include SoundController
#include "SoundController/SoundController.h"
#include "SoundController/MusicPlayer.h"

#include "GameControl\Settings.h"

// Inputs
#include "Inputs\KeyboardController.h"
#include "Inputs\MouseController.h"

#include <iostream>
using namespace std;

// Include GLFW
#include <GLFW/glfw3.h>

// Include CGameStateManager
#include "GameStateManagement/GameStateManager.h"
// Include CIntroState
#include "GameStateManagement/IntroState.h"
// Include CMenuState
#include "GameStateManagement/MenuState.h"
// Include CPlayGameState
#include "GameStateManagement/PlayGameState.h"
// Include CPlay3DGameState
#include "GameStateManagement/Play3DGameState.h"
// Include CPauseState
#include "GameStateManagement/PauseState.h"

#include "System\filesystem.h"

/**
 @brief Define an error callback
 @param error The error code
 @param description The details about the error
 */
static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
	_fgetchar();
}

/**
 @brief Define the key input callback
 @param window The window to receive the the instructions
 @param key The key code
 @param scancode The scan code
 @param action The action to take
 @param mods The modifications
 */
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	CKeyboardController::GetInstance()->Update(key, action);
}

/**
 @brief Callback function when the OpenGL window is repositioned
 @param window The window to receive the the instructions
 @param xpos integer value of the new x position of the window
 @param ypos integer value of the new y position of the window
*/
void repos_callback(GLFWwindow* window, int xpos, int ypos)
{
	// Store the window width position
	CSettings::GetInstance()->iWindowPosX = xpos;
	CSettings::GetInstance()->iWindowPosY = ypos;
	// Update the glViewPort
	glViewport(0, 0, CSettings::GetInstance()->iWindowWidth, CSettings::GetInstance()->iWindowHeight);
}

/**
 @brief Callback function when the OpenGL window is resized
 @param window The window to receive the the instructions
 @param w integer value of the new width of the window
 @param h integer value of the new height of the window
 */
void resize_callback(GLFWwindow* window, int width, int height)
{
	// Store the window width and height
	CSettings::GetInstance()->iWindowWidth = width;
	CSettings::GetInstance()->iWindowHeight = height;
	// Update the glViewPort
	glViewport(0, 0, width, height);
}

/**
 @brief Callback function whenever the mouse has events
 @param window The window to receive the the instructions
 @param button An integer value of the mouse button causing the event
 @param action An integer value of the action caused by the mouse button
 @param mods An integer value storing the mods of the event
 */
void MouseButtonCallbacks(GLFWwindow* window, int button, int action, int mods)
{
	// Send the callback to the mouse controller to handle
	if (action == GLFW_PRESS)
		CMouseController::GetInstance()->UpdateMouseButtonPressed(button);
	else
		CMouseController::GetInstance()->UpdateMouseButtonReleased(button);
}

/**
 @brief Callback function whenever the mouse has events
 @param window The window to receive the the instructions
 @param xoffset A double value of the mouse scroll offset in the x-axis
 @param yoffset A double value of the mouse scroll offset in the y-axis
 */
void MouseScrollCallbacks(GLFWwindow* window, double xoffset, double yoffset)
{
	CMouseController::GetInstance()->UpdateMouseScroll(xoffset, yoffset);
}

/**
@brief Destructor
*/
Application::~Application(void)
{
}

/**
 @brief Initialise this class instance
 */
bool Application::Init(void)
{
	// glfw: initialize and configure
	// ------------------------------
	//Initialize GLFW
	if (!glfwInit())
	{
		// return false if the GLFW was not initialised successfully
		return false;
	}

	// Initialise cMusicPlayer
	if (CMusicPlayer::GetInstance()->Init() == false)
	{
		std::cout << "Failed to initialise CMusicPlayer class instance" << std::endl;
		return false;
	}

	// Initialise the CSoundController singleton
	if (CSoundController::GetInstance()->Init() == false)
	{
		std::cout << "Failed to initialise CSoundController class instance" << std::endl;
		return false;
	}

	cMusicPlayer = CMusicPlayer::GetInstance();

	cMusicPlayer->AddMusic(FileSystem::getPath("Music\\Music_Level.ogg"), 1, true);

	cMusicPlayer->PlayMusicByID(1);
	cMusicPlayer->SetPlayMode(CMusicPlayer::PLAYMODE::SINGLE_LOOP);
	cMusicPlayer->SetMasterVolume(0.5f);
	cMusicPlayer->PlayMusic();

	// Get the CSettings instance
	cSettings = CSettings::GetInstance();

	// Set the file location for the digital assets
	// This is backup, in case filesystem cannot find the current directory
	cSettings->logl_root = "C:/Users/tohdj/Documents/2021_2022_SEM1/DM2213 2D Game Creation/Teaching Materials/NYP_Framework";

	//Set the GLFW window creation hints - these are optional
	if (cSettings->bUse4XAntiliasing == true)
		glfwWindowHint(GLFW_SAMPLES, 4); //Request 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //Request a specific OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //Request a specific OpenGL version
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL 

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	//Create a window and create its OpenGL context
	cSettings->pWindow = glfwCreateWindow(	cSettings->iWindowWidth, cSettings->iWindowHeight,
											"NYP Framework", NULL, NULL);
	//If the window couldn't be created, then return false
	if (cSettings->pWindow == NULL)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		return false;
	}

	// Set OpenGL window position
	glfwSetWindowPos(cSettings->pWindow, cSettings->iWindowPosX, cSettings->iWindowPosY);

	//This function makes the context of the specified window current on the calling thread. 
	glfwMakeContextCurrent(cSettings->pWindow);

	//Sets the callback functions for GLFW
	//Set the window position callback function
	glfwSetWindowPosCallback(cSettings->pWindow, repos_callback);
	//Set the window resize resize callback function
	glfwSetWindowSizeCallback(cSettings->pWindow, resize_callback);
	//Set the frame buffer resize callback function
	glfwSetFramebufferSizeCallback(cSettings->pWindow, resize_callback);
	//Set the error callback function
	glfwSetErrorCallback(error_callback);
	// Set the keyboard callback function
	glfwSetKeyCallback(cSettings->pWindow, key_callback);
	//Set the mouse button callback function
	glfwSetMouseButtonCallback(cSettings->pWindow, MouseButtonCallbacks);
	//Set the mouse scroll callback function
	glfwSetScrollCallback(cSettings->pWindow, MouseScrollCallbacks);

	// Additional customisation for the GLFW environment
	// Disable the cursor
	if (cSettings->bDisableMousePointer == true)
		glfwSetInputMode(cSettings->pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	else
		// Hide the cursor
		if (cSettings->bShowMousePointer == false)
			glfwSetInputMode(cSettings->pWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	// glewExperimental is a variable that is already defined by GLEW. You must set it to GL_TRUE before calling glewInit(). 
	glewExperimental = GL_TRUE;
	// Initialize GLEW
	GLenum glewInitErr = glewInit();
	//If GLEW hasn't initialized, then return false
	if (glewInitErr != GLEW_OK)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(glewInitErr));
		glfwTerminate();
		return false;
	}

	// Add the shaders to the ShaderManager
	CShaderManager::GetInstance()->Add("Shader2D", "Shader//Shader2D.vs", "Shader//Shader2D.fs");
	CShaderManager::GetInstance()->Add("Shader2D_Colour", "Shader//Shader2D_Colour.vs", "Shader//Shader2D_Colour.fs");

	CShaderManager::GetInstance()->Add("Shader3D", "Shader//Shader3D.vs", "Shader//Shader3D.fs");
	CShaderManager::GetInstance()->Add("Shader3D_Line", "Shader//Shader3D_LineShader.vs", "Shader//Shader3D_LineShader.fs");
	CShaderManager::GetInstance()->Add("Shader3D_SkyBox", "Shader//Shader3D_SkyBox.vs", "Shader//Shader3D_SkyBox.fs");
	CShaderManager::GetInstance()->Add("Shader3D_NoColour", "Shader//Shader3D_NoColour.vs", "Shader//Shader3D_NoColour.fs");
	CShaderManager::GetInstance()->Add("Shader3D_Model", "Shader//Shader3D_Model.vs", "Shader//Shader3D_Model.fs");
	CShaderManager::GetInstance()->Add("Shader3D_RenderToTexture", "Shader//Shader3D_RenderToTexture.vs", "Shader//Shader3D_RenderToTexture.fs");
	CShaderManager::GetInstance()->Add("Shader3D_Instancing", "Shader//Shader3D_Instancing.vs", "Shader//Shader3D_Instancing.fs");
	CShaderManager::GetInstance()->Add("Shader3D_Terrain", "Shader//Shader3D_Terrain.vs", "Shader//Shader3D_Terrain.fs");

	// Initialise the CFPSCounter instance
	cFPSCounter = CFPSCounter::GetInstance();
	cFPSCounter->Init();

	// Create the Game States
	CGameStateManager::GetInstance()->AddGameState("IntroState", new CIntroState());
	CGameStateManager::GetInstance()->AddGameState("MenuState", new CMenuState());
	CGameStateManager::GetInstance()->AddGameState("PlayGameState", new CPlayGameState());
	CGameStateManager::GetInstance()->AddGameState("Play3DGameState", new CPlay3DGameState());
	CGameStateManager::GetInstance()->AddGameState("PauseState", new CPauseState());

	// Set the active scene
	CGameStateManager::GetInstance()->SetActiveGameState("IntroState");

	return true;
}

/**
@brief Run this class instance
*/ 
void Application::Run(void)
{
	// Initialise the CFPSCounter instance
	cFPSCounter->Init(true, 0.0333333333333333);
	// Start timer to calculate how long it takes to render this frame
	cFPSCounter->StartTimer();

	double dDeltaTime = 0.0;
	
	// Render loop
	while (!glfwWindowShouldClose(cSettings->pWindow))
	{
		// Call the active Game State's Update method
		if (CGameStateManager::GetInstance()->Update(dDeltaTime) == false)
		{
			break;
		}
		
		// Call the active Game State's Render method
		CGameStateManager::GetInstance()->Render();

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(cSettings->pWindow);

		// Perform Post Update Input Devices
		PostUpdateInputDevices();

		// Poll events
		glfwPollEvents();

		// Update Input Devices
		UpdateInputDevices();

		// Update the FPS Counter
		cFPSCounter->Update();

		// Calculate the delta time since the last frame
		dDeltaTime = cFPSCounter->GetDeltaTime();
	}

	// Stop the timer
	cFPSCounter->StopTimer();
}

/**
 @brief Destroy this class instance
 */
void Application::Destroy(void)
{

	// Destroy the CSoundController instance
	CSoundController::GetInstance()->Destroy();

	// Destroy the CGameStateManager
	CGameStateManager::GetInstance()->Destroy();
	// Destory the ShaderManager
	CShaderManager::GetInstance()->Destroy();

	// Destroy the mouse instance
	CMouseController::GetInstance()->Destroy();
	// Destroy the keyboard instance
	CKeyboardController::GetInstance()->Destroy();

	// Destroy the CFPSCounter instance
	if (cFPSCounter)
	{
		cFPSCounter->Destroy();
		cFPSCounter = NULL;
	}

	//Close OpenGL window and terminate GLFW
	glfwDestroyWindow(cSettings->pWindow);
	//Finalize and clean up GLFW
	glfwTerminate();

	// Destroy the CSettings instance
	CSettings::GetInstance()->Destroy();
}

/**
 @brief Get window height
 */
int Application::GetWindowHeight(void) const
{
	return cSettings->iWindowHeight;
}

/**
 @brief Get window width
 */
int Application::GetWindowWidth(void) const
{
	return cSettings->iWindowWidth;
}

/**
 @brief Constructor
 */
Application::Application(void)
	: cFPSCounter(NULL)
{
}

/**
@brief Get updates from the input devices
*/
void Application::UpdateInputDevices(void)
{
	// Update Mouse Position
	double dMouse_X, dMouse_Y;
	glfwGetCursorPos( cSettings->pWindow, &dMouse_X, &dMouse_Y);
	CMouseController::GetInstance()->UpdateMousePosition( dMouse_X, dMouse_Y);
}

/**
 @brief End updates from the input devices. This method is not used anymore
 */
void Application::PostUpdateInputDevices(void)
{
	CKeyboardController::GetInstance()->PostUpdate();
}
