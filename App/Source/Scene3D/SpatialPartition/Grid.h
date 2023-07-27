#pragma once

// Include Entity3D
#include <Primitives/Entity3D.h>

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

//#include "../SceneGraph/SceneNode.h"
#include "../Entities/SolidObject.h"

// Include CLevelOfDetails
#include "Primitives/LevelOfDetails.h"

//#include <vector>
#include <map>
using namespace std;

class CGrid : public CEntity3D
{
public:
	// Constructor
	CGrid(void);
	// Destructor
	~CGrid(void);

	// Init
	void Init(glm::vec3 vec3TopRight, glm::vec3 vec3BottomLeft);
	// Init
	void Init(glm::vec3 vec3TopRight, glm::vec3 vec3BottomLeft, float fTopLeftHeight, float fBottomRightHeight);

	// Update this class instance
	virtual bool Update(const double dElapsedTime);

	// PreRender
	virtual void PreRender(void);
	// Render
	virtual void Render(void);
	// PostRender
	virtual void PostRender(void);

	// Reset the list of CSolidObjects in this CGrid
	void Reset(void);

	// Add a new object to this grid
	void Add(CSolidObject* cSolidObject);
	// Remove but not delete an object from this grid
	bool Remove(CSolidObject* cSolidObject);

	// Check if an object is in this grid
	bool IsHere(CSolidObject* cSolidObject) const;

	// Get number of objects in this grid
	int GetNumOfObject(void) const;

	// Update the LOD for this grid
	void UpdateLOD(const float fDistance);

	// Set the value of bVisible
	virtual void SetVisiblity(const bool bVisible);
	// Get the value of bVisible
	virtual const bool GetVisibility(void) const;

	// PrintSelf
	void PrintSelf(void);

	// Bounding Box specifications
	// Bounding Box top right position
	glm::vec3 vec3TopRight;
	// Bounding Box bottom left position
	glm::vec3 vec3BottomLeft;
	// Bounding Box colour
	glm::vec4 vec4Colour;
	// Line width
	float fLineWidth;
	// Previous Line width
	float fPreviousLineWidth;

	// The map of objects in this grid
	std::map<unsigned int, CSolidObject*> SolidObjectMap;

protected:
	// Boolean flag to indicate if this Grid is visible
	bool bVisible;

	// Current level of details for the entities in this grid
	CLevelOfDetails cCurrentLOD;
};
