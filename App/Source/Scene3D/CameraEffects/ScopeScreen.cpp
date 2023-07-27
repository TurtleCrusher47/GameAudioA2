/**
 CScopeScreen
 By: Toh Da Jun
 Date: Sep 2021
 */
#include "ScopeScreen.h"

 // Include GLEW
#ifndef GLEW_STATIC
#define GLEW_STATIC
#include <GL/glew.h>
#endif

// Include ShaderManager
#include "RenderControl/ShaderManager.h"

// Include ImageLoader
#include "System\ImageLoader.h"

// Include MeshBuilder
#include "Primitives/MeshBuilder.h"

#include <iostream>
using namespace std;

/**
 @brief Default Constructor
 */
CScopeScreen::CScopeScreen(void)
{
}

/**
 @brief Destructor
 */
CScopeScreen::~CScopeScreen(void)
{
}

/**
 @brief Initialise this class instance
 @return true is successfully initialised this class instance, else false
 */
bool CScopeScreen::Init(void)
{
	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();

	// Set up the projection matrix
	projection = glm::ortho(0.0f,
		static_cast<float>(cSettings->iWindowWidth),
		0.0f,
		static_cast<float>(cSettings->iWindowHeight),
		-1.0f, 1.0f);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Generate the p2DMesh
	p2DMesh = CMeshBuilder::GenerateQuad(glm::vec4(1, 1, 1, 1), 1.0f, 1.0f, false);

	// load and create a texture 
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/GUI/CameraEffects_Scoped.tga", true);
	if (iTextureID == 0)
	{
		cout << "Unable to load Image/GUI/CameraEffects_Scoped.tga" << endl;
		return false;
	}

	// Init runtimeColour to white
	vec4ColourTint = glm::vec4(1.0, 1.0, 1.0, 1.0);

	// Set this Camera Effect to be disabled initially
	bStatus = false;

	return true;
}

/**
 @brief Update the elapsed time
 @param dt A const double variable containing the elapsed time since the last frame
 @return A bool variable to indicate this method successfully completed its tasks
 */
bool CScopeScreen::Update(const double dElapsedTime)
{
	if (bStatus == false)
		return false;

	return true;
}