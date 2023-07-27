/**
 CCrossHair
 By: Toh Da Jun
 Date: Sep 2021
 */
#include "CrossHair.h"

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
CCrossHair::CCrossHair(void)
{
}

/**
 @brief Destructor
 */
CCrossHair::~CCrossHair(void)
{
}

/**
 @brief Initialise this class instance
 @return true is successfully initialised this class instance, else false
 */
bool CCrossHair::Init(void)
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
	float fAspectRatio = (float)cSettings->iWindowHeight / (float)cSettings->iWindowWidth;
	//p2DMesh = CMeshBuilder::GenerateQuad(glm::vec4(1, 1, 1, 1), fAspectRatio, 1.0f, false);
	p2DMesh = CMeshBuilder::GenerateQuadWithPosition(	glm::vec4(1, 1, 1, 1), 
														0.5f, 
														0.5f, 
														fAspectRatio, 1.0f);

	// load and create a texture 
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/GUI/CameraEffects_CrossHair.tga", true);
	if (iTextureID == 0)
	{
		cout << "Unable to load Image/GUI/CameraEffects_CrossHair.tga" << endl;
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
bool CCrossHair::Update(const double dElapsedTime)
{
	return true;
}
