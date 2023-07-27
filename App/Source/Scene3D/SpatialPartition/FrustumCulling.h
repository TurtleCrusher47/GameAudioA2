#pragma once

// Include Entity3D
#include <Primitives/Entity3D.h>

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

#include <vector>
using namespace std;

class CFrustumCulling : public CEntity3D
{
public:
	// Constructor
	CFrustumCulling(void);
	// Destructor
	~CFrustumCulling(void);

	// Init
	bool Init(void);
	//void Init(glm::vec3 vec3TopRight, glm::vec3 vec3BottomLeft);

	// Update this class instance
	virtual bool Update(const double dElapsedTime);

	// PreRender
	virtual void PreRender(void);
	// Render
	virtual void Render(void);
	// PostRender
	virtual void PostRender(void);

	// Check if a bounding box is visible, a.k.a, it is within this Frustum
	bool IsBoxVisible(const glm::vec3& minp, const glm::vec3& maxp) const;

	// Check if a point is within this Frustum
	bool IsPointWithin(const glm::vec3& vec3Point) const;

	// PrintSelf
	void PrintSelf(void);

protected:
	enum Planes
	{
		Left = 0,
		Right,
		Bottom,
		Top,
		Near,
		Far,
		Count,
		Combinations = Count * (Count - 1) / 2
	};

	template<Planes i, Planes j>
	struct ij2k
	{
		enum { k = i * (9 - i) / 2 + j - 1 };
	};

	// Update the Frustum values. m = ProjectionMatrix * ViewMatrix 
	void UpdateFrustum(void);

	template<Planes a, Planes b, Planes c>
	glm::vec3 intersection(const glm::vec3* crosses) const;

	glm::mat4   m;
	glm::vec4   m_planes[Count];
	glm::vec3   m_points[8];

	// Line width
	float fLineWidth;
	// Previous Line width
	float fPreviousLineWidth;

	// Pointer to the currently bound buffer's memory for us to operate on:
	void* ptr;
};
