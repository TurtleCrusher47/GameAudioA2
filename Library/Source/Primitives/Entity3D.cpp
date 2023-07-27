/**
 CEntity3D
 By: Toh Da Jun
 Date: Apr 2020
 */
#include "Entity3D.h"

#include "IDGenerator.h"

// Include ImageLoader
#include "..\System\ImageLoader.h"

#include <iostream>
using namespace std;

/**
@brief Default Constructor
*/
CEntity3D::CEntity3D()
	: cSettings(NULL)
	, sShaderName("")
	, ID(0)
	, VAO(0)
	, VBO(0)
	, IBO(0)
	, iIndicesSize(0)
	, p3DMesh(NULL)
	, iTextureID(0)
	, model(glm::mat4(1.0f))
	, view(glm::mat4(1.0f))
	, projection(glm::mat4(1.0f))
	, eType(TYPE::OTHERS)
	, vec3Position(0.0f)
	, vec3Front(glm::vec3(0.0f, 0.0f, -1.0f))
	, vec3PreviousPosition(0.0f)
	, vec3Scale(1.0f)
	, fRotationAngle(0.0f)
	, vec3RotationAxis(1.0f)
	, vec4Colour(1.0f)
	, fMovementSpeed(1.0f)
	, bStatus(false)
	, bToDelete(false)
	, bVisible(false)
{
	ID = CIDGenerator::GetInstance()->GenerateID();
}

/**
@brief Default Destructor
*/
CEntity3D::~CEntity3D(void)
{
	if (p3DMesh)
	{
		delete p3DMesh;
		p3DMesh = NULL;
	}

	// We set this to NULL, since it was created elsewhere so we don't delete it here
	cSettings = NULL;

	// Delete the rendering objects in the graphics card
	if (VAO != 0)
	{
		glDeleteVertexArrays(1, &VAO);
		VAO = 0;
	}
	if (VBO != 0)
	{
		glDeleteBuffers(1, &VBO);
		VBO = 0;
	}
	if (IBO != 0)
	{
		glDeleteBuffers(1, &IBO);
		IBO = 0;
	}
}

/**
@brief Initialise this instance to default values
*/
bool CEntity3D::Init(void)
{
	// Get the handler to the CSettings Singleton
	cSettings = CSettings::GetInstance();

	bStatus = true;

	return true;
}


/**
 @brief Set the name of the shader to be used in this class instance
 @param _name The name of the Shader instance in the CShaderManager
 */
void CEntity3D::SetShader(const std::string& _name)
{
	this->sShaderName = _name;
}

/**
 @brief Set model
 @param model A const glm::mat4 variable containing the model for this class instance
 */
void CEntity3D::SetModel(const glm::mat4 model)
{
	this->model = model;
}

/**
 @brief Set view
 @param view A const glm::mat4 variable containing the model for this class instance
 */
void CEntity3D::SetView(const glm::mat4 view)
{
	this->view = view;
}

/**
 @brief Set projection
 @param projection A const glm::mat4 variable containing the model for this class instance
 */
void CEntity3D::SetProjection(const glm::mat4 projection)
{
	this->projection = projection;
}

/**
 @brief Get model
 @param A const glm::mat4 variable containing the model for this class instance
 */
glm::mat4 CEntity3D::GetModel(void) const
{
	return model;
}

/**
 @brief Get view
 @param A const glm::mat4 variable containing the model for this class instance
 */
glm::mat4 CEntity3D::GetView(void) const
{
	return view;
}

/**
 @brief Get projection
 @return A const glm::mat4 variable containing the model for this class instance
 */
glm::mat4 CEntity3D::GetProjection() const
{
	return projection;
}

// Set methods
void CEntity3D::SetTextureID(const GLuint iTextureID)
{
	this->iTextureID = iTextureID;
}
void CEntity3D::SetType(const TYPE eType)
{
	this->eType = eType;
}
void CEntity3D::SetPosition(const glm::vec3 vec3Position)
{
	this->vec3Position = vec3Position;
}
void CEntity3D::SetPreviousPosition(const glm::vec3 vec3PreviousPosition)
{
	this->vec3PreviousPosition = vec3PreviousPosition;
}
void CEntity3D::SetFront(const glm::vec3 vec3Front)
{
	this->vec3Front = vec3Front;
}
void CEntity3D::SetScale(const glm::vec3 vec3Scale)
{
	this->vec3Scale = vec3Scale;
}

void CEntity3D::SetRotation(const float fRotationAngle, const glm::vec3 vec3RotationAxis)
{
	this->fRotationAngle = fRotationAngle;
	this->vec3RotationAxis = vec3RotationAxis;
}
void CEntity3D::SetColour(const glm::vec4 vec4Colour)
{
	this->vec4Colour = vec4Colour;
}
void CEntity3D::SetMovementSpeed(const float fMovementSpeed)
{
	this->fMovementSpeed = fMovementSpeed;
}
void CEntity3D::SetStatus(const bool bStatus)
{
	this->bStatus = bStatus;
}
void CEntity3D::SetVisibility(const bool bVisible)
{
	this->bVisible = bVisible;
}

// Get methods
const unsigned int CEntity3D::GetID(void) const
{
	return ID;
}

const GLuint CEntity3D::GetTextureID(void) const
{
	return iTextureID;
}
const CEntity3D::TYPE CEntity3D::GetType(void) const
{
	return eType;
}
const glm::vec3 CEntity3D::GetPosition(void) const
{
	return vec3Position;
}
const glm::vec3 CEntity3D::GetPreviousPosition(void) const
{
	return vec3PreviousPosition;
}
const glm::vec3 CEntity3D::GetFront(void) const
{
	return vec3Front;
}
const glm::vec3 CEntity3D::GetScale(void) const
{
	return vec3Scale;
}
const float CEntity3D::GetRotationAngle(void) const
{
	return fRotationAngle;
}
const glm::vec3 CEntity3D::GetRotationAxis(void) const
{
	return vec3RotationAxis;
}
const glm::vec4 CEntity3D::GetColour(void) const
{
	return vec4Colour;
}
const float CEntity3D::GetMovementSpeed(void) const
{
	return fMovementSpeed;
}
const bool CEntity3D::GetStatus(void) const
{
	return bStatus;
}
const bool CEntity3D::GetVisibility(void) const
{
	return bVisible;
}

// These methods are for marking this CEntity3D for deletion
/**
 @brief Mark this CEntity3D for deletion
 @param bToDelete A const bool variable which means this CEntity3D is to be deleted if true, else false 
 */
void CEntity3D::SetToDelete(const bool bToDelete)
{
	this->bToDelete = bToDelete;
}

/**
 @brief Check if this CEntity3D is to be deleted
 @return A bool value of true means this CEntity3D is to be deleted, else false
 */
const bool CEntity3D::IsToDelete(void) const
{
	return bToDelete;
}

/**
 @brief Store position for rollback
 */
void CEntity3D::StorePositionForRollback(void)
{
	vec3PreviousPosition = vec3Position;
}

/**
 @brief Rollback the position to the previous position
 */
void CEntity3D::RollbackPosition(void)
{
	vec3Position = vec3PreviousPosition;
}

/**
 @brief Update this class instance
 @param dt A const double variable containing the elapsed time since the last frame
 @return A bool variable
 */
bool CEntity3D::Update(const double dElapsedTime)
{
	// create transformations
	model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
	model = glm::translate(model, vec3Position);
	model = glm::scale(model, vec3Scale);
	model = glm::rotate(model, glm::radians(fRotationAngle), vec3RotationAxis);

	return true;
}