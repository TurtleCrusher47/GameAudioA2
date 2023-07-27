#include "MeshBuilder.h"

// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

#include <vector>
#include <iostream>
using namespace std;

/**
 @brief Generate a quad and load it into OpenGL
 @param numRow An unsigned integer containing the number of rows in the sprite sheet
 @param numCol An unsigned integer containing the number of rows in the sprite sheet
 @param tile_width A float variable containing the width of a tile in the sprite sheet
 @param tile_height A float variable containing the height of a tile in the sprite sheet
 @return A pointer to a CSpriteAnimation class instance
 */
CSpriteAnimation* CMeshBuilder::GenerateSpriteAnimation(unsigned int numRow, unsigned int numCol, float tile_width, float tile_height)
{
	Vertex v;
	std::vector<Vertex> vertex_buffer_data;
	std::vector<GLuint> index_buffer_data;

	float width = 1.f / numCol;
	float height = 1.f / numRow;
	int offset = 0;
	for (unsigned i = 0; i < numRow; ++i)
	{
		for (unsigned j = 0; j < numCol; ++j)
		{
			float u1 = j * width;
			float v1 = 1.f - height - i * height;
			v.position = glm::vec3(-0.5f * tile_width, -0.5f * tile_height, 0);
			v.texCoord = glm::vec2(u1, v1);
			vertex_buffer_data.push_back(v);

			v.position = glm::vec3(0.5f * tile_width, -0.5f * tile_height, 0);
			v.texCoord = glm::vec2(u1 + width, v1);
			vertex_buffer_data.push_back(v);

			v.position = glm::vec3(0.5f * tile_width, 0.5f * tile_height, 0);
			v.texCoord = glm::vec2(u1 + width, v1 + height);
			vertex_buffer_data.push_back(v);

			v.position = glm::vec3(-0.5f * tile_width, 0.5f * tile_height, 0);
			v.texCoord = glm::vec2(u1, v1 + height);
			vertex_buffer_data.push_back(v);

			index_buffer_data.push_back(offset + 3);
			index_buffer_data.push_back(offset + 0);
			index_buffer_data.push_back(offset + 2);
			index_buffer_data.push_back(offset + 1);
			index_buffer_data.push_back(offset + 2);
			index_buffer_data.push_back(offset + 0);
			offset += 4;
		}
	}

	CSpriteAnimation* mesh = new CSpriteAnimation(numRow, numCol);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex), &vertex_buffer_data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(GLuint), &index_buffer_data[0], GL_STATIC_DRAW);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = CMesh::DRAW_TRIANGLES;

	return mesh;
}

/**
 @brief Generate a quad and load it into OpenGL
 @param colour A glm::vec4 variable containing the colour of this quad
 @param width A const float variable containing the width of the quad
 @param height A const float variable containing the height of the quad
 @param bCentralisedOrigin A bool variable to decide if the quad is centralised around the origin
 */
CMesh* CMeshBuilder::GenerateQuad(glm::vec4 colour, const float width, const float height, bool bCentralisedOrigin)
{
	Vertex v;
	std::vector<Vertex> vertex_buffer_data;
	std::vector<GLuint> index_buffer_data;

	if (bCentralisedOrigin == true)
		v.position = glm::vec3(-0.5f * width, -0.5f * height, 0);
	else
		v.position = glm::vec3(0.0f, 0.0f, 0);
	v.colour = colour;
	v.texCoord = glm::vec2(0, 0);
	vertex_buffer_data.push_back(v);

	if (bCentralisedOrigin == true)
		v.position = glm::vec3(0.5f * width, -0.5f * height, 0);
	else
		v.position = glm::vec3(width, 0.0f, 0);
	v.colour = colour;
	v.texCoord = glm::vec2(1.0f, 0);
	vertex_buffer_data.push_back(v);

	if (bCentralisedOrigin == true)
		v.position = glm::vec3(0.5f * width, 0.5f * height, 0);
	else
		v.position = glm::vec3(width, height, 0);
	v.colour = colour;
	v.texCoord = glm::vec2(1.0f, 1.0f);
	vertex_buffer_data.push_back(v);

	if (bCentralisedOrigin == true)
		v.position = glm::vec3(-0.5f * width, 0.5f * height, 0);
	else
		v.position = glm::vec3(0.0f, height, 0);
	v.colour = colour;
	v.texCoord = glm::vec2(0, 1.0f);
	vertex_buffer_data.push_back(v);

	index_buffer_data.push_back(3);
	index_buffer_data.push_back(0);
	index_buffer_data.push_back(2);
	index_buffer_data.push_back(1);
	index_buffer_data.push_back(2);
	index_buffer_data.push_back(0);

	CMesh* mesh = new CMesh();

	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex), &vertex_buffer_data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(GLuint), &index_buffer_data[0], GL_STATIC_DRAW);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = CMesh::DRAW_TRIANGLES;

	return mesh;
}

/**
 @brief Generate a quad at a specific position and load it into OpenGL
 @param colour A glm::vec4 variable containing the colour of this quad
 @param x A const float variable of the x-coordinate of the quad
 @param y A const float variable of the y-coordinate of the quad
 @param width A const float variable containing the width of the quad
 @param height A const float variable containing the height of the quad
 */
CMesh* CMeshBuilder::GenerateQuadWithPosition(	const glm::vec4 colour,
												const float x, 
												const float y,
												const float width, 
												const float height)
{
	Vertex v;
	std::vector<Vertex> vertex_buffer_data;
	std::vector<GLuint> index_buffer_data;

	v.position = glm::vec3(x - 0.5f * width, y - 0.5f * height, 0);
	v.colour = colour;
	v.texCoord = glm::vec2(0, 0);
	vertex_buffer_data.push_back(v);

	v.position = glm::vec3(x + 0.5f * width, y - 0.5f * height, 0);
	v.colour = colour;
	v.texCoord = glm::vec2(1.0f, 0);
	vertex_buffer_data.push_back(v);

	v.position = glm::vec3(x + 0.5f * width, y + 0.5f * height, 0);
	v.colour = colour;
	v.texCoord = glm::vec2(1.0f, 1.0f);
	vertex_buffer_data.push_back(v);

	v.position = glm::vec3(x - 0.5f * width, y + 0.5f * height, 0);
	v.colour = colour;
	v.texCoord = glm::vec2(0, 1.0f);
	vertex_buffer_data.push_back(v);

	index_buffer_data.push_back(3);
	index_buffer_data.push_back(0);
	index_buffer_data.push_back(2);
	index_buffer_data.push_back(1);
	index_buffer_data.push_back(2);
	index_buffer_data.push_back(0);

	CMesh* mesh = new CMesh();

	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex), &vertex_buffer_data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(GLuint), &index_buffer_data[0], GL_STATIC_DRAW);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = CMesh::DRAW_TRIANGLES;

	return mesh;
}

/**
 @brief Generate a box and load it into OpenGL
 @param colour A glm::vec4 variable containing the colour of this box
 @param width A const float variable containing the width of the box
 @param height A const float variable containing the height of the box
 @param depth A const float variable containing the depth of the box
 */
CMesh* CMeshBuilder::GenerateBox(	const glm::vec4 colour,
									const float width, 
									const float height, 
									const float depth)
{
	Vertex v;
	std::vector<Vertex> vertex_buffer_data;
	std::vector<GLuint> index_buffer_data;

	// 0. Left-Bottom-Rear
	v.position = glm::vec3(-0.5f * width, -0.5f * height, -0.5f * depth);
	v.colour = colour;
	v.texCoord = glm::vec2(0.0f, 0.0f);
	vertex_buffer_data.push_back(v);
	// 1. Right-Bottom-Rear
	v.position = glm::vec3(0.5f * width, -0.5f * height, -0.5f * depth);
	v.colour = colour;
	v.texCoord = glm::vec2(1.0f, 0.0f);
	vertex_buffer_data.push_back(v);
	// 2. Right-Top-Rear
	v.position = glm::vec3(0.5f * width, 0.5f * height, -0.5f * depth);
	v.colour = colour;
	v.texCoord = glm::vec2(1.0f, 1.0f);
	vertex_buffer_data.push_back(v);
	// 3. Left-Top-Rear
	v.position = glm::vec3(-0.5f * width, 0.5f * height, -0.5f * depth);
	v.colour = colour;
	v.texCoord = glm::vec2(0.0f, 1.0f);
	vertex_buffer_data.push_back(v);
	// 4. Left-Bottom-Front
	v.position = glm::vec3(-0.5f * width, -0.5f * height, 0.5f * depth);
	v.colour = colour;
	v.texCoord = glm::vec2(0.0f, 0.0f);
	vertex_buffer_data.push_back(v);
	// 5. Right-Bottom-Front
	v.position = glm::vec3(0.5f * width, -0.5f * height, 0.5f * depth);
	v.colour = colour;
	v.texCoord = glm::vec2(1.0f, 0.0f);
	vertex_buffer_data.push_back(v);
	// 6. Right-Top-Front
	v.position = glm::vec3(0.5f * width, 0.5f * height, 0.5f * depth);
	v.colour = colour;
	v.texCoord = glm::vec2(1.0f, 1.0f);
	vertex_buffer_data.push_back(v);
	// 7. Left-Top-Front
	v.position = glm::vec3(-0.5f * width, 0.5f * height, 0.5f * depth);
	v.colour = colour;
	v.texCoord = glm::vec2(0.0f, 1.0f);
	vertex_buffer_data.push_back(v);

	// The textures are swapped so the textures will render correctly
	// 8. Left-Bottom-Front : Swapped textures : 1st set
	v.position = glm::vec3(-0.5f * width, -0.5f * height, 0.5f * depth);
	v.colour = colour;
	v.texCoord = glm::vec2(1.0f, 0.0f);
	vertex_buffer_data.push_back(v);
	// 9. Right-Bottom-Front : Swapped textures : 1st set
	v.position = glm::vec3(0.5f * width, -0.5f * height, 0.5f * depth);
	v.colour = colour;
	v.texCoord = glm::vec2(0.0f, 0.0f);
	vertex_buffer_data.push_back(v);
	// 10. Right-Top-Front : Swapped textures : 1st set
	v.position = glm::vec3(0.5f * width, 0.5f * height, 0.5f * depth);
	v.colour = colour;
	v.texCoord = glm::vec2(0.0f, 1.0f);
	vertex_buffer_data.push_back(v);
	// 11. Left-Top-Front : Swapped textures : 1st set
	v.position = glm::vec3(-0.5f * width, 0.5f * height, 0.5f * depth);
	v.colour = colour;
	v.texCoord = glm::vec2(1.0f, 1.0f);
	vertex_buffer_data.push_back(v);

	// 12. Right-Top-Front : Swapped textures : 2nd set
	v.position = glm::vec3(0.5f * width, 0.5f * height, 0.5f * depth);
	v.colour = colour;
	v.texCoord = glm::vec2(1.0f, 0.0f);
	vertex_buffer_data.push_back(v);
	// 13. Left-Top-Front : Swapped textures : 2nd set
	v.position = glm::vec3(-0.5f * width, 0.5f * height, 0.5f * depth);
	v.colour = colour;
	v.texCoord = glm::vec2(0.0f, 0.0f);
	vertex_buffer_data.push_back(v);
	// 14. Right-Bottom-Front : Swapped textures : 2nd set
	v.position = glm::vec3(0.5f * width, -0.5f * height, 0.5f * depth);
	v.colour = colour;
	v.texCoord = glm::vec2(1.0f, 1.0f);
	vertex_buffer_data.push_back(v);
	// 15. Left-Bottom-Front : Swapped textures : 2nd set
	v.position = glm::vec3(-0.5f * width, -0.5f * height, 0.5f * depth);
	v.colour = colour;
	v.texCoord = glm::vec2(0.0f, 1.0f);
	vertex_buffer_data.push_back(v);

	// Rear face
	index_buffer_data.push_back(0);
	index_buffer_data.push_back(3);
	index_buffer_data.push_back(2);
	index_buffer_data.push_back(2);
	index_buffer_data.push_back(1);
	index_buffer_data.push_back(0);

	// Front face
	index_buffer_data.push_back(4);
	index_buffer_data.push_back(5);
	index_buffer_data.push_back(6);
	index_buffer_data.push_back(6);
	index_buffer_data.push_back(7);
	index_buffer_data.push_back(4);

	// Left face
	index_buffer_data.push_back(11);
	index_buffer_data.push_back(3);
	index_buffer_data.push_back(0);
	index_buffer_data.push_back(0);
	index_buffer_data.push_back(8);
	index_buffer_data.push_back(11);

	// Right face
	index_buffer_data.push_back(10);
	index_buffer_data.push_back(9);
	index_buffer_data.push_back(1);
	index_buffer_data.push_back(1);
	index_buffer_data.push_back(2);
	index_buffer_data.push_back(10);

	// Bottom face
	index_buffer_data.push_back(0);
	index_buffer_data.push_back(1);
	index_buffer_data.push_back(14);
	index_buffer_data.push_back(14);
	index_buffer_data.push_back(15);
	index_buffer_data.push_back(0);

	// Top face
	index_buffer_data.push_back(3);
	index_buffer_data.push_back(13);
	index_buffer_data.push_back(12);
	index_buffer_data.push_back(12);
	index_buffer_data.push_back(2);
	index_buffer_data.push_back(3);

	CMesh* mesh = new CMesh();

	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex), &vertex_buffer_data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(GLuint), &index_buffer_data[0], GL_STATIC_DRAW);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = CMesh::DRAW_TRIANGLES;

	return mesh;
}
