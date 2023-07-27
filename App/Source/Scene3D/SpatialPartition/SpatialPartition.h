#pragma once

// Include SingletonTemplate
#include "DesignPatterns\SingletonTemplate.h"

// Include Entity3D
#include <Primitives/Entity3D.h>

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

#include "Grid.h"
//#include "../SceneGraph/SceneNode.h"

#include "FrustumCulling.h"

//#include "../Player3D.h"
#include "../Entities/SolidObjectManager.h"


class CSpatialPartition : public CSingletonTemplate<CSpatialPartition>, public CEntity3D
{
	friend class CSingletonTemplate<CSpatialPartition>;
public:
	// Destroy the Singleton instance
	void Destroy();

	// Initialise the spatial partition
	bool Init(glm::vec3 vec3MapSize, glm::i32vec3 i32vec3NumTiles);

	// Update this class instance
	virtual bool Update(const double dElapsedTime);

	// PreRender
	virtual void PreRender(void);
	// Render
	virtual void Render(void);
	// PostRender
	virtual void PostRender(void);

	// Get a particular grid
	CGrid* GetGrid(const int iRow, const int iCol) const;
	// Get a particular grid using a position
	CGrid* GetGrid(glm::vec3 vecPosition) const;

	// Reset the list of CSolidObjects in the Spatial Partition
	void Reset(CSolidObjectManager* cSolidObjectManager);

	// Add a new object
	void Add(CSolidObject* cSolidObject);
	// Remove but not delete object from this grid
	void Remove(CSolidObject* cSolidObject);

	// Collision Check for an entity against Spatial Partition's entities
	virtual bool CheckForCollisionWithEntity(CEntity3D* cEntity3D);

	//PrintSelf
	void PrintSelf() const;

protected:
	// Constructor
	CSpatialPartition(void);
	// Destructor
	virtual ~CSpatialPartition();

	// Spatial Partition Settings
	// The size of the spatial partition in X-, Z-axes
	glm::vec3 vec3SpatialPartitionSize;
	// The size of each grid in X-, Z-axes
	glm::vec3 vec3GridSize;
	// The number of the grids in X-, Z-axes
	glm::i32vec3 i32vec3NumGrid;

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

	// Vectors of Vectors containing the CGrids
	vector<vector<CGrid*>> vGrids;

	// Handler to the FrustumCulling instance
	CFrustumCulling* cFrustumCulling;
	// This view is for calculating frustum culling 
	// as we may detach the camera from the player.
	glm::mat4 frustumCullingView;

};
