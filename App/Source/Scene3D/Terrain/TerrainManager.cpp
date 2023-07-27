/**
CTerrainManager
@brief This class manages the instances of CTerrain.
By: Toh Da Jun
Date: Nov 2022
*/
#include "TerrainManager.h"

#include <vector>

using namespace std;

// Include GLEW
#ifndef GLEW_STATIC
#define GLEW_STATIC
#include <GL/glew.h>
#endif

// Include ShaderManager
#include "RenderControl/ShaderManager.h"
// Include CTexture
#include "Texture.h"

// Include ImageLoader
#include "System\ImageLoader.h"

/**
 @brief Constructor
 */
CTerrainManager::CTerrainManager(void)
	: m_iIndex(0)
{
	terrainInstances.clear();
}

/**
 @brief Destructor
 */
CTerrainManager::~CTerrainManager(void)
{
	for (unsigned int i = 0; i < terrainInstances.size(); i++)
	{
		delete terrainInstances[i];
	}
	terrainInstances.clear();
}

/**
 @brief Initialise this class instance
 @return A bool variable to indicate this method successfully completed its tasks
 */
bool CTerrainManager::Init(void)
{
	terrainInstances.clear();

	return true;
}

/**
 @brief Add a terrain to this class instance
 @param m_iIndex A const int variable containing the new m_iIndex
 @return true if the operation is successful, false is otherwise.
 */
bool CTerrainManager::Add(const std::string& _name,
						const string sImagePath,
						const float fRenderX, const float fHeight, const float fRenderZ)
{
	CTerrain* cTerrain = new CTerrain();
	cTerrain->SetShader("Shader3D_Terrain");
	if (cTerrain->Init(sImagePath) == false)
	{
		cout << "Unable to add terrain: " << sImagePath << endl;
		return false;
	}

	// Set the size of the Terrain
	cTerrain->SetRenderSize(100.0f, 5.0f, 100.0f);

	// Push this new CTerrain into the vector
	terrainInstances.push_back(cTerrain);

	return true;
}

/**
 @brief Set the index of the Terrain to render
 @param m_iIndex A const int variable containing the new m_iIndex
 @return true if the operation is successful, false is otherwise.
 */
bool CTerrainManager::SetIndex(const int m_iIndex)
{
	if ((m_iIndex >= 0) && (m_iIndex < int(terrainInstances.size())))
	{
		this->m_iIndex = m_iIndex;
		return true;;
	}

	return false;
}

/**
 @brief Get the index of the Terrain being rendered
 @return The m_iIndex value
 */
int CTerrainManager::GetIndex(void) const
{
	return m_iIndex;
}

/**
 @brief Set model
 @param model A glm::mat4 variable to be assigned to this class instance
 */
void CTerrainManager::SetModel(const glm::mat4 model)
{
	terrainInstances[m_iIndex]->SetModel(model);
}

/**
 @brief Set view
 @param view A glm::mat4 variable to be assigned to this class instance
 */
void CTerrainManager::SetView(const glm::mat4 view)
{
	terrainInstances[m_iIndex]->SetView(view);
}

/**
 @brief Set projection
 @param projection A glm::mat4 variable to be assigned to this class instance
 */
void CTerrainManager::SetProjection(const glm::mat4 projection)
{
	terrainInstances[m_iIndex]->SetProjection(projection);
}

/**
 @brief Update this class instance
 @param dElapsedTime A const double variable contains the time since the last frame
 @return A bool variable to indicate this method successfully completed its tasks
 */
bool CTerrainManager::Update(const double dElapsedTime)
{
	terrainInstances[m_iIndex]->Update(dElapsedTime);
	return true;
}

/**
 @brief PreRender.
 */
void CTerrainManager::PreRender(void)
{
	terrainInstances[m_iIndex]->PreRender();
}

/**
 @brief Render.
 */
void CTerrainManager::Render(void)
{
	terrainInstances[m_iIndex]->Render();
}

/**
 @brief PostRender
 */
void CTerrainManager::PostRender(void)
{
	terrainInstances[m_iIndex]->PostRender();
}

/**
 @brief Sets rendering size (scaling) of heightmap.
 @param fRenderX Size in X-axis
 @param fHeight Height in Y-axis
 @param fRenderZ Size in Z-axis
 */
void CTerrainManager::SetRenderSize(const float fRenderX, const float fHeight, const float fRenderZ)
{
	terrainInstances[m_iIndex]->SetRenderSize(fRenderX, fHeight, fRenderZ);
}

/**
 @brief Sets rendering size (scaling) of heightmap.
 @param fQuadSize How big should be one quad
 @param fHeight - Height of heightmap
 */
void CTerrainManager::SetRenderSize(const float fQuadSize, const float fHeight)
{
	terrainInstances[m_iIndex]->SetRenderSize(fQuadSize, fHeight);
}

/**
 @brief Get Height Map rows
 @return A int variable
 */
int CTerrainManager::GetNumHeightmapRows(void) const
{
	return terrainInstances[m_iIndex]->GetNumHeightmapRows();
}

/**
 @brief Get Height Map columns
 @return A int variable
 */
int CTerrainManager::GetNumHeightmapCols(void) const
{
	return terrainInstances[m_iIndex]->GetNumHeightmapCols();
}

/**
 @brief Get the height at a X- and Z-coordinate
 @param fX A const float variable containing the x coordinate to check for the height
 @param fZ A const float variable containing the z coordinate to check for the height
 @return A float variable
 */
float CTerrainManager::GetHeight(const float fX, const float fZ)
{
	return terrainInstances[m_iIndex]->GetHeight(fX, fZ);
}

/**
 @brief Get the vMinPos
 @return A glm::vec3 variable
 */
glm::vec3 CTerrainManager::GetMinPos(void) const
{
	return terrainInstances[m_iIndex]->GetMinPos();
}

/**
 @brief Get the vMaxPos
 @return A glm::vec3 variable
 */
glm::vec3 CTerrainManager::GetMaxPos(void) const
{
	return terrainInstances[m_iIndex]->GetMaxPos();
}

/**
 @brief Print Self
 */
void CTerrainManager::PrintSelf(void)
{
	return terrainInstances[m_iIndex]->PrintSelf();
}