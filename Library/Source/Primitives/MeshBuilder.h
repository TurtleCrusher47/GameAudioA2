/**
 MeshBuilder.h
 By: Lim Chian Song
 Date: Apr 2021
 This MeshBuilder follows the style of NYP Computer Graphics Module for the ease of students.
 The Job of the meshbuilder is to create mesh with vertices, filling up the vertices and 
 indices buffer and return to the entity to be used.
 */
#ifndef MESH_BUILDER_H
#define MESH_BUILDER_H

#include "Mesh.h"
#include "SpriteAnimation.h"

class CMeshBuilder
{
	public:
		// Generate sprite animation
		static CSpriteAnimation* GenerateSpriteAnimation(	const unsigned numRow, 
															const unsigned numCol, 
															float tile_width = 1.0f, 
															float tile_height = 1.0f);

		// Generate a quad
		static CMesh* GenerateQuad(	glm::vec4 colour = glm::vec4(1,1,1,1),
									float width = 1.0f, 
									float height = 1.0f,
									bool bCentralisedOrigin = true);

		// Generate a quad at a specific position
		static CMesh* GenerateQuadWithPosition(	const glm::vec4 colour = glm::vec4(1, 1, 1, 1),
												const float x = 0.0f, 
												const float y = 0.0f,
												const float width = 1.0f, 
												const float height = 1.0f);

		// Generate a box
		static CMesh* GenerateBox(	glm::vec4 colour = glm::vec4(1, 1, 1, 1), 
									float width = 1.0f, 
									float height = 1.0f, 
									float depth = 1.0f);
};

#endif