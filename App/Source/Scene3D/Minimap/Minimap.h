/**
 CMinimap
 By: Toh Da Jun
 Date: Apr 2020
 */
#pragma once

// Include SingletonTemplate
#include "DesignPatterns\SingletonTemplate.h"

#include <Primitives\Entity3D.h>

#include <GL/glew.h>
#include <includes\glm.hpp>

#include <RenderControl\shader.h>
#include <vector>

class CMinimap : public CEntity3D, public CSingletonTemplate<CMinimap>
{
	friend CSingletonTemplate<CMinimap>;
public:
	// Variables for OpenGL rendering
	GLuint VBO;
	GLuint FBO;
	GLuint uiTextureColourBuffer;
	GLuint RBO;
	GLuint VAO_BORDER, VBO_BORDER;
	GLuint VAO_ARROW, VBO_ARROW;

	// Initialise this class instance
	bool Init(void);

	// Activate rendering to the Minimap
	virtual void Activate(void);
	// Deactivate rendering to the Minimap
	virtual void Deactivate(void);

	// Update this class instance
	virtual bool Update(const double dElapsedTime);

	// PreRender
	virtual void PreRender(void);
	// Render
	virtual void Render(void);
	// PostRender
	virtual void PostRender(void);

	//Change the player current colour 
	void SetPlayerArrowCurrentColour(glm::vec4);

	// PrintSelf
	void PrintSelf(void);

protected:
	// Constructor
	CMinimap(void);

	// Destructor
	~CMinimap(void);

	// Highlight Colour of the arrow in mini map
	glm::vec4 currentColour;

	// The texture ID for the arrow
	GLuint iArrowTextureID;

};
