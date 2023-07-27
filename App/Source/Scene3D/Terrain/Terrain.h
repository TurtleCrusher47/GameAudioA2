/**
CTerrain
@brief Wraps FreeType heightmap loading and rendering, also allowing
	   to use multiple layers of textures with transitions between them.
Adapted from: https://www.mbsoftworks.sk/tutorials/opengl3/21-multilayered-terrain/
Adapted By: Toh Da Jun
Date: Sep 2021
*/
#pragma once

//// Include SingletonTemplate
//#include "DesignPatterns\SingletonTemplate.h"

// Include Entity3D
#include <Primitives/Entity3D.h>

#include "vertexBufferObject.h"

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

#include <string>
using namespace std;

class CTerrain : /*public CSingletonTemplate<CTerrain>,*/ public CEntity3D
{
//	friend CSingletonTemplate<CTerrain>;
public:
	// Constructor
	CTerrain(void);
	// Destructor
	~CTerrain(void);

	// Initialise this class instance
	bool Init(void);
	bool Init(const string sImagePath);

	// Update this class instance
	virtual bool Update(const double dElapsedTime);

	// PreRender
	virtual void PreRender(void);
	// Render
	virtual void Render(void);
	// PostRender
	virtual void PostRender(void);

	bool LoadHeightMapFromImage(const string sImagePath);
	void ReleaseHeightmap();

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
	unsigned int uiVAO;

	bool bLoaded;
	bool bShaderProgramLoaded;
	int iRows;
	int iCols;

	// The Render Scale for this terrain
	glm::vec3 vRenderScale;
	// The min and max positions for this terrain
	glm::vec3 vMinPos, vMaxPos;

	// The VBOs of the heightmap
	CVertexBufferObject vboHeightmapData;
	// The indices for the VBOs of the heightmap
	CVertexBufferObject vboHeightmapIndices;
	// The raw vertex data
	vector< vector< glm::vec3> > vVertexData;
	// The raw coordinates data
	vector< vector< glm::vec2> > vCoordsData;

};