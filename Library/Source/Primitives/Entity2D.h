/**
 CEntity2D
 By: Toh Da Jun
 Date: Mar 2020
 */
#pragma once

// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

// Include Settings
#include "..\GameControl\Settings.h"
#include <string>

//CS: Include Mesh.h to use to draw (include vertex and index buffers)
#include "Mesh.h"
using namespace std;

class CEntity2D
{
public:
	// Constructor
	CEntity2D(void);

	// Destructor
	virtual ~CEntity2D(void);

	// Init
	virtual bool Init(void);

	// Set projection
	virtual void SetProjection(glm::mat4 projection);
	// Set projection
	virtual glm::mat4 GetProjection(void) const;


	// Set the status of this CEntity2D
	virtual void SetStatus(const bool bStatus);
	// Get the status of this CEntity2D
	virtual const bool GetStatus(void) const;

	// Set the name of the shader to be used in this class instance
	virtual void SetShader(const std::string& _name);

	// Update this class instance
	virtual bool Update(const double dElapsedTime);

	// PreRender
	virtual void PreRender(void);

	// Render
	virtual void Render(void);

	// PostRender
	virtual void PostRender(void);

	// The vec2 which stores the position of an Entity2D in the Map2D
	glm::vec2 vec2Position;

protected:
	// Name of Shader Program instance
	std::string sShaderName;

	//CS: The mesh that is used to draw objects
	CMesh* p2DMesh;

	// Colour Tint
	glm::vec4 vec4ColourTint;

	// OpenGL objects
	unsigned int VBO, VAO, EBO;

	// The texture ID in OpenGL
	unsigned int iTextureID;

	// A transformation matrix representing the entity's translation, rotation, scale and shearing
	glm::mat4 model;
	// A transformation matrix representing the projection of the scene to the framebuffer
	glm::mat4 projection;

	// Settings
	CSettings* cSettings;

	// Boolean flag to indicate if this CEntity3D is active
	// true == active, false == inactive
	bool bStatus;
};
