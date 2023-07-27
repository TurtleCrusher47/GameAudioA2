#include "FrustumCulling.h"

// Include ShaderManager
#include "RenderControl/ShaderManager.h"

// Include this for glm::to_string() function
#define GLM_ENABLE_EXPERIMENTAL
#include <includes/gtx/string_cast.hpp>

#include <iostream>
using namespace std;

/**
@brief Constructor
*/
CFrustumCulling::CFrustumCulling(void)
	: fLineWidth(3.0f)
{
}

/**
@brief Destructor
*/
CFrustumCulling::~CFrustumCulling(void)
{
}

/**
@brief Initialise this grid
		//m_points[0] = <Left, Bottom, Near>;
		//m_points[1] = <Left, Top, Near>;
		//m_points[2] = <Right, Bottom, Near>;
		//m_points[3] = <Right, Top, Near>;
		//m_points[4] = <Left, Bottom, Far>;
		//m_points[5] = <Left, Top, Far>;
		//m_points[6] = <Right, Bottom, Far>;
		//m_points[7] = <Right, Top, Far>;
*/
bool CFrustumCulling::Init(void)
{
	// Update the Frustum first before we setup the rendering details.
	UpdateFrustum();

	// set up vertex data (and buffer(s)) and configure vertex attributes
	float vertices[] = {
		//bottom
		m_points[0].x, m_points[0].y, m_points[0].z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		m_points[2].x, m_points[2].y, m_points[2].z, vec4Colour.x, vec4Colour.y, vec4Colour.z,

		m_points[2].x, m_points[2].y, m_points[2].z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		m_points[6].x, m_points[6].y, m_points[6].z, vec4Colour.x, vec4Colour.y, vec4Colour.z,

		m_points[6].x, m_points[6].y, m_points[6].z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		m_points[4].x, m_points[4].y, m_points[4].z, vec4Colour.x, vec4Colour.y, vec4Colour.z,

		m_points[4].x, m_points[4].y, m_points[4].z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		m_points[0].x, m_points[0].y, m_points[0].z, vec4Colour.x, vec4Colour.y, vec4Colour.z,

		//top
		m_points[1].x, m_points[1].y, m_points[1].z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		m_points[3].x, m_points[3].y, m_points[3].z, vec4Colour.x, vec4Colour.y, vec4Colour.z,

		m_points[3].x, m_points[3].y, m_points[3].z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		m_points[7].x, m_points[7].y, m_points[7].z, vec4Colour.x, vec4Colour.y, vec4Colour.z,

		m_points[7].x, m_points[7].y, m_points[7].z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		m_points[5].x, m_points[5].y, m_points[5].z, vec4Colour.x, vec4Colour.y, vec4Colour.z,

		m_points[5].x, m_points[5].y, m_points[5].z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		m_points[1].x, m_points[1].y, m_points[1].z, vec4Colour.x, vec4Colour.y, vec4Colour.z,

		//join top to bottom
		m_points[1].x, m_points[1].y, m_points[1].z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		m_points[0].x, m_points[0].y, m_points[0].z, vec4Colour.x, vec4Colour.y, vec4Colour.z,

		m_points[3].x, m_points[3].y, m_points[3].z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		m_points[2].x, m_points[2].y, m_points[2].z, vec4Colour.x, vec4Colour.y, vec4Colour.z,

		m_points[7].x, m_points[7].y, m_points[7].z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		m_points[6].x, m_points[6].y, m_points[6].z, vec4Colour.x, vec4Colour.y, vec4Colour.z,

		m_points[5].x, m_points[5].y, m_points[5].z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		m_points[4].x, m_points[4].y, m_points[4].z, vec4Colour.x, vec4Colour.y, vec4Colour.z
	};

	// Tell OpenGL to generate a value for VAO
	glGenVertexArrays(1, &VAO);
	// Tell OpenGL to generate a value for VBO
	glGenBuffers(1, &VBO);

	// Bind the VAO to the following set of operations
	glBindVertexArray(VAO);

	// Bind the VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Reserve the memory area in OpenGL display card first. Note the NULL variable to indicate nothing is uploaded now
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), NULL, GL_DYNAMIC_DRAW);
	// Get pointer to the memory area indicated by the VBO
	ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	// Copy data into this memory area
	memcpy(ptr, vertices, sizeof(vertices));
	// Tell OpenGL to unlink to this pointer
	glUnmapBuffer(GL_ARRAY_BUFFER);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// Tell OpenGL to unlink to the VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return true;
}

/**
@brief Update this class instance
@param dElapsedTime A const double containing the elapsed time since the last frame
*/
bool CFrustumCulling::Update(const double dElapsedTime)
{
	UpdateFrustum();

	// Update the vertex data (and buffer(s)) and configure vertex attributes
	float vertices[] = {
		m_points[0].x, m_points[0].y, m_points[0].z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		m_points[2].x, m_points[2].y, m_points[2].z, vec4Colour.x, vec4Colour.y, vec4Colour.z,

		m_points[2].x, m_points[2].y, m_points[2].z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		m_points[6].x, m_points[6].y, m_points[6].z, vec4Colour.x, vec4Colour.y, vec4Colour.z,

		m_points[6].x, m_points[6].y, m_points[6].z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		m_points[4].x, m_points[4].y, m_points[4].z, vec4Colour.x, vec4Colour.y, vec4Colour.z,

		m_points[4].x, m_points[4].y, m_points[4].z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		m_points[0].x, m_points[0].y, m_points[0].z, vec4Colour.x, vec4Colour.y, vec4Colour.z,

		//top
		m_points[1].x, m_points[1].y, m_points[1].z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		m_points[3].x, m_points[3].y, m_points[3].z, vec4Colour.x, vec4Colour.y, vec4Colour.z,

		m_points[3].x, m_points[3].y, m_points[3].z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		m_points[7].x, m_points[7].y, m_points[7].z, vec4Colour.x, vec4Colour.y, vec4Colour.z,

		m_points[7].x, m_points[7].y, m_points[7].z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		m_points[5].x, m_points[5].y, m_points[5].z, vec4Colour.x, vec4Colour.y, vec4Colour.z,

		m_points[5].x, m_points[5].y, m_points[5].z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		m_points[1].x, m_points[1].y, m_points[1].z, vec4Colour.x, vec4Colour.y, vec4Colour.z,

		//join top to bottom
		m_points[1].x, m_points[1].y, m_points[1].z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		m_points[0].x, m_points[0].y, m_points[0].z, vec4Colour.x, vec4Colour.y, vec4Colour.z,

		m_points[3].x, m_points[3].y, m_points[3].z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		m_points[2].x, m_points[2].y, m_points[2].z, vec4Colour.x, vec4Colour.y, vec4Colour.z,

		m_points[7].x, m_points[7].y, m_points[7].z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		m_points[6].x, m_points[6].y, m_points[6].z, vec4Colour.x, vec4Colour.y, vec4Colour.z,

		m_points[5].x, m_points[5].y, m_points[5].z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		m_points[4].x, m_points[4].y, m_points[4].z, vec4Colour.x, vec4Colour.y, vec4Colour.z
	};

	// Bind the VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Get pointer to the memory area indicated by the VBO
	ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	// Copy data into this memory area
	memcpy(ptr, vertices, sizeof(vertices));
	// Tell OpenGL to unlink to this pointer
	glUnmapBuffer(GL_ARRAY_BUFFER);

	return true;
}

/**
@brief PreRender
*/
void CFrustumCulling::PreRender(void)
{
	// change depth function so depth test passes when values are equal to depth buffer's content
	glDepthFunc(GL_LEQUAL);

	// Activate shader
	CShaderManager::GetInstance()->Use(sShaderName);

	// Store the previous line width
	glGetFloatv(GL_LINE_WIDTH, &fPreviousLineWidth);
	
	// Set to the new line width
	glLineWidth(fLineWidth);
}

/**
@brief Render
*/
void CFrustumCulling::Render(void)
{
	CShaderManager::GetInstance()->activeShader->setMat4("model", model);
	CShaderManager::GetInstance()->activeShader->setMat4("view", view);
	CShaderManager::GetInstance()->activeShader->setMat4("projection", projection);

	// render box
	glBindVertexArray(VAO);
	//glDrawArrays(GL_LINE_LOOP, 0, 36);
	glDrawArrays(GL_LINES, 0, 36);
	glBindVertexArray(0);
}

/**
@brief PostRender
*/
void CFrustumCulling::PostRender(void)
{
	// Restore to the previous line width
	glLineWidth(fPreviousLineWidth);
}

// Check if a bounding box is visible, a.k.a, it is within this Frustum
bool CFrustumCulling::IsBoxVisible(const glm::vec3& minp, const glm::vec3& maxp) const
{
	// check box outside/inside of frustum
	for (int i = 0; i < Count; i++)
	{
		if ((glm::dot(m_planes[i], glm::vec4(minp.x, minp.y, minp.z, 1.0f)) < 0.0) &&
			(glm::dot(m_planes[i], glm::vec4(maxp.x, minp.y, minp.z, 1.0f)) < 0.0) &&
			(glm::dot(m_planes[i], glm::vec4(minp.x, maxp.y, minp.z, 1.0f)) < 0.0) &&
			(glm::dot(m_planes[i], glm::vec4(maxp.x, maxp.y, minp.z, 1.0f)) < 0.0) &&
			(glm::dot(m_planes[i], glm::vec4(minp.x, minp.y, maxp.z, 1.0f)) < 0.0) &&
			(glm::dot(m_planes[i], glm::vec4(maxp.x, minp.y, maxp.z, 1.0f)) < 0.0) &&
			(glm::dot(m_planes[i], glm::vec4(minp.x, maxp.y, maxp.z, 1.0f)) < 0.0) &&
			(glm::dot(m_planes[i], glm::vec4(maxp.x, maxp.y, maxp.z, 1.0f)) < 0.0))
		{
			return false;
		}
	}

	// check frustum outside/inside box
	int out;
	out = 0; for (int i = 0; i<8; i++) out += ((m_points[i].x > maxp.x) ? 1 : 0); if (out == 8) return false;
	out = 0; for (int i = 0; i<8; i++) out += ((m_points[i].x < minp.x) ? 1 : 0); if (out == 8) return false;
	out = 0; for (int i = 0; i<8; i++) out += ((m_points[i].y > maxp.y) ? 1 : 0); if (out == 8) return false;
	out = 0; for (int i = 0; i<8; i++) out += ((m_points[i].y < minp.y) ? 1 : 0); if (out == 8) return false;
	out = 0; for (int i = 0; i<8; i++) out += ((m_points[i].z > maxp.z) ? 1 : 0); if (out == 8) return false;
	out = 0; for (int i = 0; i<8; i++) out += ((m_points[i].z < minp.z) ? 1 : 0); if (out == 8) return false;

	return true;
}

/**
@brief Check if a point is within this Frustum
@param vec3Point A const glm::vec3& variable which contains the position to check
*/ 
bool CFrustumCulling::IsPointWithin(const glm::vec3& vec3Point) const
{
	glm::vec4 vec3PointInClipSpace = projection * (view * glm::vec4(vec3Point, 1.0f));

	// If it is zero, then that means that the object is in the plane of projection; 
	// it's view-space depth is zero. And such vertices are automatically clipped by OpenGL.
	if (vec3PointInClipSpace.w > 0.0f)
	{
		//cout << "WORLD COORDINATE : " << vec3Point.x << ", " << vec3Point.y << ", " << vec3Point.z << ". ";
		//cout << "ClipSpace : " << vec3PointInClipSpace.x << ", " << vec3PointInClipSpace.y << ", " << vec3PointInClipSpace.z << ". ";
		glm::vec3 ndcSpacePos = vec3PointInClipSpace / vec3PointInClipSpace.w;
		if (((ndcSpacePos.x >= -1.0f) && (ndcSpacePos.x <= 1.0f)) &&
			((ndcSpacePos.y >= -1.0f) && (ndcSpacePos.y <= 1.0f)))
		{
			//cout << "*** ACCEPTED *** : " << ndcSpacePos.x << ", " << ndcSpacePos.y << ", " << ndcSpacePos.z << endl;
			return true;
		}
		//else
		//{
		//	cout << "*** REJECTED *** : " << ndcSpacePos.x << ", " << ndcSpacePos.y << ", " << ndcSpacePos.z << endl;
		//}
	}
	//else
	//	cout << "BEHIND CLIPSPACE" << endl;

	return false;
}

/**
@brief Update the Frustum values
*/
inline void CFrustumCulling::UpdateFrustum(void)
{
	m = glm::transpose(projection * view);
	m_planes[Left] = m[3] + m[0];
	m_planes[Right] = m[3] - m[0];
	m_planes[Bottom] = m[3] + m[1];
	m_planes[Top] = m[3] - m[1];
	m_planes[Near] = m[3] + m[2];
	m_planes[Far] = m[3] - m[2];

	glm::vec3 crosses[Combinations] = {
		glm::cross(glm::vec3(m_planes[Left]), glm::vec3(m_planes[Right])),
		glm::cross(glm::vec3(m_planes[Left]), glm::vec3(m_planes[Bottom])),
		glm::cross(glm::vec3(m_planes[Left]), glm::vec3(m_planes[Top])),
		glm::cross(glm::vec3(m_planes[Left]), glm::vec3(m_planes[Near])),
		glm::cross(glm::vec3(m_planes[Left]), glm::vec3(m_planes[Far])),
		glm::cross(glm::vec3(m_planes[Right]), glm::vec3(m_planes[Bottom])),
		glm::cross(glm::vec3(m_planes[Right]), glm::vec3(m_planes[Top])),
		glm::cross(glm::vec3(m_planes[Right]), glm::vec3(m_planes[Near])),
		glm::cross(glm::vec3(m_planes[Right]), glm::vec3(m_planes[Far])),
		glm::cross(glm::vec3(m_planes[Bottom]), glm::vec3(m_planes[Top])),
		glm::cross(glm::vec3(m_planes[Bottom]), glm::vec3(m_planes[Near])),
		glm::cross(glm::vec3(m_planes[Bottom]), glm::vec3(m_planes[Far])),
		glm::cross(glm::vec3(m_planes[Top]), glm::vec3(m_planes[Near])),
		glm::cross(glm::vec3(m_planes[Top]), glm::vec3(m_planes[Far])),
		glm::cross(glm::vec3(m_planes[Near]), glm::vec3(m_planes[Far]))
	};

	m_points[0] = intersection<Left, Bottom, Near>(crosses);
	m_points[1] = intersection<Left, Top, Near>(crosses);
	m_points[2] = intersection<Right, Bottom, Near>(crosses);
	m_points[3] = intersection<Right, Top, Near>(crosses);
	m_points[4] = intersection<Left, Bottom, Far>(crosses);
	m_points[5] = intersection<Left, Top, Far>(crosses);
	m_points[6] = intersection<Right, Bottom, Far>(crosses);
	m_points[7] = intersection<Right, Top, Far>(crosses);

}

template<CFrustumCulling::Planes a, CFrustumCulling::Planes b, CFrustumCulling::Planes c>
inline glm::vec3 CFrustumCulling::intersection(const glm::vec3* crosses) const
{
	float D = glm::dot(glm::vec3(m_planes[a]), crosses[ij2k<b, c>::k]);
	glm::vec3 res = glm::mat3(crosses[ij2k<b, c>::k], -crosses[ij2k<a, c>::k], crosses[ij2k<a, b>::k]) *
		glm::vec3(m_planes[a].w, m_planes[b].w, m_planes[c].w);
	return res * (-1.0f / D);
}

/**
@brief PrintSelf
*/
void CFrustumCulling::PrintSelf(void)
{
	cout << "CFrustumCulling::PrintSelf()" << endl;
	cout << "********************************************************************************" << endl;
	cout << "LBN: " << m_points[0].x << ", " << m_points[0].y << ", " << m_points[0].z << endl;
	cout << "LTN: " << m_points[1].x << ", " << m_points[1].y << ", " << m_points[1].z << endl;
	cout << "RBN: " << m_points[2].x << ", " << m_points[2].y << ", " << m_points[2].z << endl;
	cout << "RBN: " << m_points[3].x << ", " << m_points[3].y << ", " << m_points[3].z << endl;

	cout << "LBF: " << m_points[4].x << ", " << m_points[4].y << ", " << m_points[4].z << endl;
	cout << "LTF: " << m_points[5].x << ", " << m_points[5].y << ", " << m_points[5].z << endl;
	cout << "RBF: " << m_points[6].x << ", " << m_points[6].y << ", " << m_points[6].z << endl;
	cout << "RBF: " << m_points[7].x << ", " << m_points[7].y << ", " << m_points[7].z << endl;

	cout << "Model: " << endl << glm::to_string(model) << endl;
	cout << "View: " << endl << glm::to_string(view) << endl;
	cout << "Projection: " << endl << glm::to_string(projection) << endl;
}
