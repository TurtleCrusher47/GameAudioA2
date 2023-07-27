/**
CTerrainManager
@brief This class manages the instances of CTerrain.
By: Toh Da Jun
Date: Nov 2022
*/
#pragma once

// Include SingletonTemplate
#include "DesignPatterns\SingletonTemplate.h"

// Include CTerrain
#include "Terrain.h"

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

#include <string>
using namespace std;

class CTerrainManager : public CSingletonTemplate<CTerrainManager>
{
	friend CSingletonTemplate<CTerrainManager>;
public:
	// Initialise this class instance
	bool Init(void);

	// Add a terrain to this class instance
	bool Add(const std::string& _name,
			 const string sImagePath,
			 const float fRenderX, const float fHeight, const float fRenderZ);

	// Set the index of the Terrain to render
	bool SetIndex(const int m_iIndex);
	// Get the index of the Terrain being rendered
	int GetIndex(void) const;

	// Update this class instance
	virtual bool Update(const double dElapsedTime);

	// Set model
	virtual void SetModel(const glm::mat4 model);
	// Set view
	virtual void SetView(const glm::mat4 view);
	// Set projection
	virtual void SetProjection(const glm::mat4 projection);

	// PreRender
	virtual void PreRender(void);
	// Render
	virtual void Render(void);
	// PostRender
	virtual void PostRender(void);

	void SetRenderSize(const float fQuadSize, const float fHeight);
	void SetRenderSize(const float fRenderX, const float fHeight, const float fRenderZ);

	// Get Height Map rows
	int GetNumHeightmapRows(void) const;
	// Get Height Map columns
	int GetNumHeightmapCols(void) const;
	// Get the height at a X- and Z-coordinate
	float GetHeight(const float fX, const float fZ);
	// Get the vMinPos
	glm::vec3 GetMinPos(void) const;
	// Get the vMaxPos
	glm::vec3 GetMaxPos(void) const;

	// Print Self
	void PrintSelf(void);

protected:
	// The index of the current SkyBox textureID
	int m_iIndex;
	// The vector of CTerrain instances
	std::vector<CTerrain*> terrainInstances;

	// Constructor
	CTerrainManager(void);
	// Destructor
	~CTerrainManager(void);
};