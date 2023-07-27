/**
 CEntity3D
 By: Toh Da Jun
 Date: Apr 2020
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

//CS: Include Mesh.h to use to draw (include vertex and index buffers)
#include "Mesh.h"

#include <string>
using namespace std;

class CEntity3D
{
public:
	// Types of entities
	enum class TYPE : unsigned int
	{
		PLAYER = 0,
		NPC,
		OTHERS,		// Default value: Other entities such as ground and skybox
		STRUCTURE,	// Buildings etc
		PROJECTILE,
		NUM_TYPES
	};

	// Constructor(s)
    CEntity3D(void);

	// Destructor
	virtual ~CEntity3D(void);

	// Init
	virtual bool Init(void);

	// Set a shader to this class instance
	virtual void SetShader(const std::string& _name);

	// Set model
	virtual void SetModel(glm::mat4 model);
	// Set view
	virtual void SetView(glm::mat4 view);
	// Set projection
	virtual void SetProjection(glm::mat4 projection);

	// Set model
	virtual glm::mat4 GetModel(void) const;
	// Set view
	virtual glm::mat4 GetView(void) const;
	// Set projection
	virtual glm::mat4 GetProjection(void) const;

	// Set Methods
	virtual void SetTextureID(const GLuint iTextureID);
	virtual void SetType(const TYPE eType);
	virtual void SetPosition(const glm::vec3 vec3Position);
	virtual void SetPreviousPosition(const glm::vec3 vec3PreviousPosition);
	virtual void SetFront(const glm::vec3 vec3Front);
	virtual void SetScale(const glm::vec3 vec3Scale);
	virtual void SetRotation(const float fRotationAngle, const glm::vec3 vec3RotationAxis);
	virtual void SetColour(const glm::vec4 vec4Colour);
	virtual void SetMovementSpeed(const float fMovementSpeed);
	virtual void SetStatus(const bool bStatus);
	virtual void SetVisibility(const bool bVisible);

	// Get Methods
	virtual const unsigned int GetID(void) const;
	virtual const GLuint GetTextureID(void) const;
	virtual const CEntity3D::TYPE GetType(void) const;
	virtual const glm::vec3 GetPosition(void) const;
	virtual const glm::vec3 GetPreviousPosition(void) const;
	virtual const glm::vec3 GetFront(void) const;
	virtual const glm::vec3 GetScale(void) const;
	virtual const float GetRotationAngle(void) const;
	virtual const glm::vec3 GetRotationAxis(void) const;
	virtual const glm::vec4 GetColour(void) const;
	virtual const float GetMovementSpeed(void) const;
	virtual const bool GetStatus(void) const;
	virtual const bool GetVisibility(void) const;

	// These methods are for marking this CEntity3D for deletion
	virtual void SetToDelete(const bool bToDelete);
	virtual const bool IsToDelete(void) const;

	// Store position for rollback
	virtual void StorePositionForRollback(void);
	// Rollback the position to the previous position
	virtual void RollbackPosition(void);

	// Update this class instance
	virtual bool Update(const double dElapsedTime);

	// PreRender
	virtual void PreRender(void) = 0;
	// Render
	virtual void Render(void) = 0;
	// PostRender
	virtual void PostRender(void) = 0;

protected:
	// The handle to the CSettings instance
	CSettings* cSettings;

	// Name of Shader Program instance
	std::string sShaderName;

	// ID of this CEntity3D
	unsigned int ID;

	//CS: The mesh that is used to draw objects
	CMesh* p3DMesh;

	// OpenGL objects
	GLuint VAO, VBO, IBO;

	// The number of indices in the model loaded in
	unsigned int iIndicesSize;

	// The texture ID in OpenGL
	GLuint iTextureID;

	// Render Settings
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;

	// Entity Type
	TYPE eType;

	// Position
	glm::vec3 vec3Position;
	// Previous Position
	glm::vec3 vec3PreviousPosition;
	// Direction
	glm::vec3 vec3Front;
	// Scale
	glm::vec3 vec3Scale;
	// Rotation angle and axis
	float fRotationAngle;
	glm::vec3 vec3RotationAxis;
	// Colour
	glm::vec4 vec4Colour;
	// Movement speed
	float fMovementSpeed;
	// Height of this entity above the CTerrain
	float fHeightOffset = 0.5f; 

	// Boolean flag to indicate if this CEntity3D is active
	// true == active, false == inactive
	bool bStatus;

	// Boolean flag to indicate if this CEntity3D is to be deleted
	bool bToDelete;

	// Boolean flag to indicate if this CEntity3D is visible
	bool bVisible;
};
