/**
 CInventoryItem
 @brief A class which represents an inventory item which can be collected in the game
 By: Toh Da Jun
 Date: Mar 2020
 */
#pragma once

// Include CEntity2D
#include "Primitives/Entity2D.h"

#include <map>
#include <string>

class CInventoryItem : public CEntity2D
{
public:
	// Constructor
	CInventoryItem(const char* imagePath = NULL);

	// Destructor
	virtual ~CInventoryItem(void);

	// Add a number of items from this class instance
	void Add(const int iNumItems);
	// Remove the number of items from this class instance
	void Remove(const int iNumItems);
	// Get the number of items from this class instance
	int GetCount(void) const;
	// Get the maximum number of items from this class instance
	int GetMaxCount(void) const;

	// Get the texture ID
	unsigned int GetTextureID(void) const;

	// Name of the inventory item
	std::string sName;
	// The size of the image to render in the GUI
	glm::vec2 vec2Size;

	// The amount of this item
	int iItemCount;
	// The total amount of this item
	int iItemMaxCount;
};
