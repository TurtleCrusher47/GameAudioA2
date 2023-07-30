// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// Include GLFW
#include <GLFW/glfw3.h>

// Include SoundController
#include "../SoundController/SoundController.h"
#include "../SoundController/MusicPlayer.h"

// Include Settings
#include "GameControl/Settings.h"

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

#include "PauseState.h"

// Include CGameStateManager
#include "GameStateManager.h"

// Include Mesh Builder
#include "Primitives/MeshBuilder.h"
// Include ImageLoader
#include "System\ImageLoader.h"
// Include Shader Manager
#include "RenderControl\ShaderManager.h"

 // Include shader
#include "RenderControl\shader.h"

// Include CSettings
#include "GameControl/Settings.h"

// Include CKeyboardController
#include "Inputs/KeyboardController.h"

#include <iostream>
using namespace std;

/**
 @brief Constructor
 */
CPauseState::CPauseState(void)
{

}

/**
 @brief Destructor
 */
CPauseState::~CPauseState(void)
{

}

/**
 @brief Init this class instance
 */
bool CPauseState::Init(void)
{
	// Get the CSettings instance
	cSettings = CSettings::GetInstance();

	cSoundController = CSoundController::GetInstance();
	cMusicPlayer = CMusicPlayer::GetInstance();

	CShaderManager::GetInstance()->Use("Shader2D");
	CShaderManager::GetInstance()->activeShader->setInt("texture1", 0);

	
	//static const char* themes[] = { "Normal", "8-Bit" };
	//static const char* currentTheme = "Normal";
	//static const char* previousTheme;
	////Dropdown menu to change music
	//if (ImGui::BeginCombo("Theme", currentTheme))
	//{
	//	previousTheme = currentTheme;
	//	for (int i = 0; i < IM_ARRAYSIZE(themes); i++)
	//	{
	//		bool is_selected = (currentTheme == themes[i]);
	//		if (ImGui::Selectable(themes[i], is_selected))
	//		{
	//			currentTheme = themes[i];
	//			if (previousTheme != currentTheme)
	//			{
	//				if (currentTheme == "Normal")
	//				{
	//					cSoundController->soundModifier = 0;
	//					cMusicPlayer->SetTheme(0);
	//				}

	//				if (currentTheme == "8-Bit")
	//				{
	//					cSoundController->soundModifier = 9;
	//					cMusicPlayer->SetTheme(1);
	//				}
	//			}
	//		}
	//		if (is_selected)
	//			ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
	//	}
	//	ImGui::EndCombo();
	//}

	//// Define the window flags
	//window_flags = 0;
	//window_flags |= ImGuiWindowFlags_NoBackground;
	//window_flags |= ImGuiWindowFlags_NoTitleBar;
	//window_flags |= ImGuiWindowFlags_NoMove;
	//window_flags |= ImGuiWindowFlags_NoResize;
	//window_flags |= ImGuiWindowFlags_NoCollapse;

	return true;
}

/**
 @brief Update this class instance
 */
bool CPauseState::Update(const double dElapsedTime)
{
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoScrollbar;
	//window_flags |= ImGuiWindowFlags_MenuBar;
	window_flags |= ImGuiWindowFlags_NoBackground;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_NoNav;

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		// Calculate the relative scale to our default windows width
		const float relativeScale_x = cSettings->iWindowWidth / 800.0f;
		const float relativeScale_y = cSettings->iWindowHeight / 600.0f;

		//Pause screen
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(30.f / 255.f, 33.f / 255.f, 39.f / 255.f, 0.6f));  // Set a background colour
		ImGuiWindowFlags winScreen = ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoScrollWithMouse |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoScrollbar;

		ImGui::Begin("Settings", NULL, winScreen);
		ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
		ImGui::SetWindowSize(ImVec2((float)cSettings->iWindowWidth, (float)cSettings->iWindowHeight));
		ImGui::SetWindowFontScale(1.5f * relativeScale_y);

		string titleText = "Paused";
		auto windowWidth = ImGui::GetWindowSize().x;
		auto textWidth = ImGui::CalcTextSize(titleText.c_str()).x;
		ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
		ImGui::Text(titleText.c_str());

		float musicVolume = cMusicPlayer->GetMasterVolume();
		ImGui::SliderFloat("Music Master Volume", &musicVolume, 0.0f, 1.0f);
		cMusicPlayer->SetMasterVolume(musicVolume);

		float soundMasterVolume = cSoundController->GetMasterVolume();
		ImGui::SliderFloat("SFX Master Volume", &soundMasterVolume, 0.0f, 1.0f);
		cSoundController->SetMasterVolume(soundMasterVolume);

		// Player SFX
		float playerVolume = cSoundController->GetVolume(1);
		ImGui::SliderFloat("Player SFX Volume", &playerVolume, 0.0f, 1.0f);
		cSoundController->SetVolume(1, playerVolume);
		cSoundController->SetVolume(2, playerVolume);
		cSoundController->SetVolume(3, playerVolume);
		cSoundController->SetVolume(4, playerVolume);
		cSoundController->SetVolume(5, playerVolume);

		// World SFX
		float worldVolume = cSoundController->GetVolume(7);
		ImGui::SliderFloat("World SFX Volume", &worldVolume, 0.0f, 1.0f);
		cSoundController->SetVolume(7, worldVolume);
		cSoundController->SetVolume(8, worldVolume);
		cSoundController->SetVolume(9, worldVolume);
		cSoundController->SetVolume(10, worldVolume);
		cSoundController->SetVolume(11, worldVolume);

		ImGui::End();
		ImGui::PopStyleColor();
	}

	//For keyboard controls
	if (CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_ESCAPE))
	{
		// Reset the CKeyboardController
		CKeyboardController::GetInstance()->Reset();

		// Unload the menu state
		cout << "UnLoading PauseState" << endl;
		CGameStateManager::GetInstance()->SetPauseGameState(nullptr);
		return true;
	}

	return true;
}

/**
 @brief Render this class instance
 */
void CPauseState::Render(void)
{
	// Clear the screen and buffer
	glClearColor(0.0f, 0.55f, 1.00f, 1.00f);

	//cout << "CPauseState::Render()\n" << endl;
}

/**
 @brief Destroy this class instance
 */
void CPauseState::Destroy(void)
{
	// cout << "CPauseState::Destroy()\n" << endl;
}
