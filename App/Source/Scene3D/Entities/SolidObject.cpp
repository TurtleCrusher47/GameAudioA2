/**
 CSolidObject
 By: Toh Da Jun
 Date: Sep 2021
 */
#include "SolidObject.h"

// Include ShaderManager
#include "RenderControl/ShaderManager.h"

// Include MeshBuilder
#include "Primitives/MeshBuilder.h"

// Include ImageLoader
#include "System\ImageLoader.h"

#include <iostream>
using namespace std;

/**
 @brief Default Constructor
 */
CSolidObject::CSolidObject(void)
{
	// Set iIndicesSize to 0
	iIndicesSize = 0;

	// Set the default position to the origin
	vec3Position = glm::vec3(0.0f, 0.0f, 0.0f);
}

/**
 @brief Constructor with vectors
 @param vec3Position A const glm::vec3 variable which contains the position of the camera
 @param vec3Front A const glm::vec3 variable which contains the up direction of the camera
 */
CSolidObject::CSolidObject(	const glm::vec3 vec3Position,
							const glm::vec3 vec3Front)
{
	// Set the default position to the origin
	this->vec3Position = vec3Position;
	this->vec3Front = vec3Front;
}

/**
 @brief Destructor
 */
CSolidObject::~CSolidObject(void)
{
}

/**
 @brief Initialise this class instance
 @return true is successfully initialised this class instance, else false
 */
bool CSolidObject::Init(void)
{
	// Call the parent's Init()
	return CEntity3D::Init();
}

/**
 @brief Initialise the collider
 @param _name A const std::string& variable which stores the name of the shader
 @param boxColour A glm::vec4 variable which is the colour of the collider box
 @param boxMin A glm::vec4 variable which is the minimum size of the collider box
 @param boxMax A glm::vec4 variable which is the maximum size of the collider box
 @return true is successfully initialised this class instance, else false
 */
bool CSolidObject::InitCollider(const std::string& _name,
								glm::vec4 boxColour,
								glm::vec3 boxMin,
								glm::vec3 boxMax)
{
	return CCollider::InitCollider(_name, boxColour, boxMin, boxMax);
}

/**
 @brief Update the elapsed time
 @param dt A const double variable containing the elapsed time since the last frame
 @return A bool variable to indicate this method successfully completed its tasks
 */
bool CSolidObject::Update(const double dElapsedTime)
{
	return CEntity3D::Update(dElapsedTime);
}

/**
 @brief PreRender Set up the OpenGL display environment before rendering
 */
void CSolidObject::PreRender(void)
{
	// change depth function so depth test passes when values are equal to depth buffer's content
	glDepthFunc(GL_LEQUAL);  

	// Activate shader
	CShaderManager::GetInstance()->Use(sShaderName);
}

/**
 @brief Render Render this instance
 */
void CSolidObject::Render(void)
{
	if (!bVisible)
		return;

	// note: currently we set the projection matrix each frame, but since the projection 
	// matrix rarely changes it's often best practice to set it outside the main loop only once.
	CShaderManager::GetInstance()->activeShader->setMat4("projection", projection);
	CShaderManager::GetInstance()->activeShader->setMat4("view", view);
	CShaderManager::GetInstance()->activeShader->setMat4("model", model);

	// If this CSolidObject does not have LOD then run this set of codes
	if (m_bLODStatus == false)
	{
		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, iTextureID);
		// Render the p3DMesh
		glBindVertexArray(VAO);
		if (p3DMesh)
			p3DMesh->Render();
		else
			glDrawElements(GL_TRIANGLES, iIndicesSize, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	// If this CSolidObject have LOD then run this set of codes
	else
	{
		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, arriTextureID[eDetailLevel]);
		// Render the p3DMesh
		glBindVertexArray(arrVAO[eDetailLevel]);
			glDrawElements(GL_TRIANGLES, arrIndicesSize[eDetailLevel], GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	// Render the CCollider if needed
	if (this->bIsDisplayed)
	{
		CCollider::colliderModel = CEntity3D::model;
		CCollider::colliderView = CEntity3D::view;
		CCollider::colliderProjection = CEntity3D::projection;
		CCollider::PreRender();
		CCollider::Render();
		CCollider::PostRender();
	}
}

/**
@brief PostRender Set up the OpenGL display environment after rendering.
*/
void CSolidObject::PostRender(void)
{
	glDepthFunc(GL_LESS); // set depth function back to default
}

/**
 @brief Print Self
 */
void CSolidObject::PrintSelf(void)
{
	cout << "CSolidObject::PrintSelf()" << endl;
	cout << "========================" << endl;
}
