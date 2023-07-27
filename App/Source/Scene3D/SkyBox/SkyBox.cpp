/**
 CSkyBox
 By: Toh Da Jun
 Date: Apr 2020
 */
#include "SkyBox.h"

#include <includes\gtc\matrix_transform.hpp>

// Include ImageLoader
#include "System\ImageLoader.h"

#include "RenderControl/ShaderManager.h"

#include <iostream>
using namespace std;

// Load SkyBox textures
unsigned int LoadSkyBoxTextures(void);

/**
 @brief Default Constructor
 */
CSkyBox::CSkyBox(void)
	: m_iIndex(0)
{
	// Clear the vector of SkyBox textureID
	skyboxTextureIDs.clear();
}

/**
 @brief Destructor
 */
CSkyBox::~CSkyBox(void)
{
	// Clear the vector of SkyBox textureID
	skyboxTextureIDs.clear();
}

/**
 @brief Initialise this class instance
 @return true is successfully initialised this class instance, else false
 */
bool CSkyBox::Init(void)
{
	// Call the parent's Init()
	CEntity3D::Init();

	// Set the type
	SetType(CEntity3D::TYPE::OTHERS);

	float vertices[] = {
		// positions     
		// Rear
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,

		// Left
		-1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		// Right
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		// Front
		-1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		// Top
		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,

		// Bottom
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f
	};

	// skybox VAO and VBO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	// Clear the vector of SkyBox textureID
	skyboxTextureIDs.clear();

	return true;
}

/**
 @brief Set view
 @param view A glm::mat4 variable containing the model for this class instance
 */
void CSkyBox::SetView(glm::mat4 view)
{
	this->view = glm::mat4(glm::mat3(view)); // remove translation from the view matrix
}

/**
 @brief  Add a set of texture files to this SkyBox class
 @param _name_right A const std::string& variable containing the name and path of right skybox texture file
 @param _name_left A const std::string& variable containing the name and path of left skybox texture file
 @param _name_top A const std::string& variable containing the name and path of top skybox texture file
 @param _name_bottom A const std::string& variable containing the name and path of bottom skybox texture file
 @param _name_front A const std::string& variable containing the name and path of front skybox texture file
 @param _name_back A const std::string& variable containing the name and path of back skybox texture file
 @return true if the addition is successful, false is otherwise.
 */
bool CSkyBox::Add(	const std::string& _name_right,
					const std::string& _name_left,
					const std::string& _name_top,
					const std::string& _name_bottom,
					const std::string& _name_front,
					const std::string& _name_back)
{
	// Create the vector of skybox textures
	skyboxTexture.push_back(_name_right);
	skyboxTexture.push_back(_name_left);
	skyboxTexture.push_back(_name_top);
	skyboxTexture.push_back(_name_bottom);
	skyboxTexture.push_back(_name_front);
	skyboxTexture.push_back(_name_back);

	// Reset iTextureID to 0
	iTextureID = 0;

	// Load the skybox textures into the Graphics Card
	iTextureID = LoadSkyBoxTextures();

	// Since we have loaded the files, we can clear the vector of skyboxTexture names
	skyboxTexture.clear();

	// Return false if the iTextureID is still 0 as the loading has failed
	if (iTextureID == 0)
		return false;

	// Add the itextureID into the vector of skyboxTextureIDs
	skyboxTextureIDs.push_back(iTextureID);

	return true;
}


/**
 @brief Set the index of the SkyBox to render
 @param m_iIndex A const int variable containing the new m_iIndex
 @return true if the operation is successful, false is otherwise.
 */
bool CSkyBox::SetIndex(const int m_iIndex)
{
	if ((m_iIndex >= 0) && (m_iIndex < int(skyboxTextureIDs.size())))
	{
		this->m_iIndex = m_iIndex;
		return true;;
	}

	return false;
}

/**
 @brief Get the index of the SkyBox being rendered
 @return The m_iIndex value 
 */
int CSkyBox::GetIndex(void) const
{
	return m_iIndex;
}

/**
 @brief Update this class instance
 @param dElapsedTime A const double variable contains the time since the last frame
 @return A bool variable to indicate this method successfully completed its tasks
 */
bool CSkyBox::Update(const double dElapsedTime)
{
	return true;
}

/**
 @brief PreRender Set up the OpenGL display environment before rendering
 */
void CSkyBox::PreRender(void)
{
	// draw skybox as last
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content

	// Activate shader
	CShaderManager::GetInstance()->Use(sShaderName);
}

/**
 @brief Render Render this instance
 @param cShader A Shader* variable which contains the Shader to use in this class instance
 */
void CSkyBox::Render(void)
{
	CShaderManager::GetInstance()->activeShader->setMat4("view", view);
	CShaderManager::GetInstance()->activeShader->setMat4("projection", projection);

	// Render skybox cube
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureIDs[m_iIndex]);
		glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CSkyBox::PostRender(void)
{
	glDepthFunc(GL_LESS); // set depth function back to default
}

// Load SkyBox textures
// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
// -------------------------------------------------------
GLuint CSkyBox::LoadSkyBoxTextures(void)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 5);

	// Get the image loader
	CImageLoader* cImageLoader = CImageLoader::GetInstance();

	// Load each of the textures as a single texture ID
	int width, height, nrChannels;
	for (unsigned int i = 0; i < skyboxTexture.size(); i++)
	{
		unsigned char *data = cImageLoader->Load(skyboxTexture[i].c_str(), width, height, nrChannels, false);
		if (data)
		{
			std::cout << skyboxTexture[i] << " successfully loaded. " << std::endl;
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << skyboxTexture[i] << std::endl;
		}
		// Free up the memory of the file data read in
		free(data);
	}
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	//// Generate mipmaps
	//glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	return textureID;
}

/**
 @brief Print Self
 */
void CSkyBox::PrintSelf(void)
{
	cout << "CSkyBox::PrintSelf()" << endl;
	cout << "========================" << endl;
}