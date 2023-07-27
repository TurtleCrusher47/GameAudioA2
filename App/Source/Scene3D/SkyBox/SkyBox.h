/**
 CSkyBox
 By: Toh Da Jun
 Date: Apr 2020
 */
#pragma once

// Include SingletonTemplate
#include "DesignPatterns\SingletonTemplate.h"

#include <Primitives\Entity3D.h>

// Include GLEW
#ifndef GLEW_STATIC
#define GLEW_STATIC
#include <GL/glew.h>
#endif

#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

#include <vector>

#include "GameControl\Settings.h"

class CSkyBox : public CEntity3D, public CSingletonTemplate<CSkyBox>
{
	friend CSingletonTemplate<CSkyBox>;
public:
	// Initialise this class instance
	bool Init(void);

	// Set view
	virtual void SetView(glm::mat4 view);

	// Add a set of texture files to this SkyBox class
	bool Add(const std::string& _name_right, 
			 const std::string& _name_left, 
			 const std::string& _name_top,
			 const std::string& _name_bottom,
			 const std::string& _name_front,
			 const std::string& _name_back);

	// Set the index of the SkyBox to render
	bool SetIndex(const int m_iIndex);
	// Get the index of the SkyBox being rendered
	int GetIndex(void) const;

	// Update this class instance
	virtual bool Update(const double dElapsedTime);

	// PreRender
	virtual void PreRender(void);
	// Render
	virtual void Render(void);
	// PostRender
	virtual void PostRender(void);
	
	// Print Self
	void PrintSelf(void);

protected:
	// The index of the current SkyBox textureID
	int m_iIndex;
	// The vector of SkyBox textureID
	std::vector<int> skyboxTextureIDs;
	// The vector of names of skybox texture files
	std::vector<std::string> skyboxTexture;

	// Constructor
	CSkyBox(void);

	// Destructor
	~CSkyBox(void);

	// Load SkyBox textures
	GLuint LoadSkyBoxTextures(void);
};
