/**
 Map2D
 @brief A class which manages the map in the game
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "Map2D.h"

// Include Shader Manager
#include "RenderControl\ShaderManager.h"

// Include Filesystem
#include "System\filesystem.h"
// Include ImageLoader
#include "System\ImageLoader.h"
#include "Primitives/MeshBuilder.h"

#include <iostream>
#include <vector>
using namespace std;

// For AStar PathFinding
using namespace std::placeholders;

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CMap2D::CMap2D(void)
	: uiCurLevel(0)
	, quadMesh(NULL)
	, TileSize(glm::vec2(18.75f, 18.75f))
	, TileHalfSize(glm::vec2(9.375f, 9.375f))
{
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CMap2D::~CMap2D(void)
{
	// Delete AStar lists
	DeleteAStarLists();

	// Dynamically deallocate the 3D array used to store the map information
	for (unsigned int uiLevel = 0; uiLevel < uiNumLevels; uiLevel++)
	{
		for (unsigned int iRow = 0; iRow < cSettings->NUM_TILES_YAXIS; iRow++)
		{
			delete[] arrMapInfo[uiLevel][iRow];
		}
		delete [] arrMapInfo[uiLevel];
	}
	delete[] arrMapInfo;

	if (quadMesh)
	{
		delete quadMesh;
		quadMesh = NULL;
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	// Set this to NULL since it was created elsewhere, so we let it be deleted there.
	cSettings = NULL;
}

/**
@brief Init Initialise this instance
*/ 
bool CMap2D::Init(	const unsigned int uiNumLevels,
					const unsigned int uiNumRows,
					const unsigned int uiNumCols)
{
	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();

	// Create the arrMapInfo and initialise to 0
	// Start by initialising the number of levels
	arrMapInfo = new Grid** [uiNumLevels];
	for (unsigned uiLevel = 0; uiLevel < uiNumLevels; uiLevel++)
	{
		arrMapInfo[uiLevel] = new Grid* [uiNumRows];
		for (unsigned uiRow = 0; uiRow < uiNumRows; uiRow++)
		{
			arrMapInfo[uiLevel][uiRow] = new Grid[uiNumCols];
			for (unsigned uiCol = 0; uiCol < uiNumCols; uiCol++)
			{
				arrMapInfo[uiLevel][uiRow][uiCol].value = 0;
			}
		}
	}

	// Store the map sizes in cSettings
	uiCurLevel = 0;
	this->uiNumLevels = uiNumLevels;
	cSettings->NUM_TILES_XAXIS = uiNumCols;
	cSettings->NUM_TILES_YAXIS = uiNumRows;
	cSettings->UpdateSpecifications();

	// Set up the projection matrix
	projection = glm::ortho(0.0f,
							static_cast<float>(cSettings->iWindowWidth),
							0.0f,
							static_cast<float>(cSettings->iWindowHeight),
							-1.0f, 1.0f);
	model = glm::mat4(1.0f);
	//CS: Init the colour to white
	vec4ColourTint = glm::vec4(1.0, 1.0, 1.0, 1.0);

	// Generate the VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//CS: Create the Quad Mesh using the mesh builder
	//quadMesh = CMeshBuilder::GenerateQuad(glm::vec4(1, 1, 1, 1), cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
	quadMesh = CMeshBuilder::GenerateQuad(glm::vec4(1, 1, 1, 1), 1, 1);

	// Load and create textures
	// Load the ground texture
	// Ground Surface
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Tiles/Surface.png", true);
	if (iTextureID == 0)
	{
		cout << "Unable to load Image/Scene2D_Surface.png" << endl;
		return false;
	}
	else
	{
		// Store the texture ID into MapOfTextureIDs
		MapOfTextureIDs.insert(pair<int, int>(100, iTextureID));
	}

	// Underground
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Tiles/Ground.png", true);
	if (iTextureID == 0)
	{
		cout << "Unable to load Image/Scene2D_Ground.png" << endl;
		return false;
	}
	else
	{
		// Store the texture ID into MapOfTextureIDs
		MapOfTextureIDs.insert(pair<int, int>(101, iTextureID));
	}

	// Roof
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Tiles/Roof.png", true);
	if (iTextureID == 0)
	{
		cout << "Unable to load Image/Scene2D_Roof.png" << endl;
		return false;
	}
	else
	{
		// Store the texture ID into MapOfTextureIDs
		MapOfTextureIDs.insert(pair<int, int>(102, iTextureID));
	}

	// Right Wall
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Tiles/Left_Wall.png", true);
	if (iTextureID == 0)
	{
		cout << "Unable to load Image/Scene2D_Left_Wall.png" << endl;
		return false;
	}
	else
	{
		// Store the texture ID into MapOfTextureIDs
		MapOfTextureIDs.insert(pair<int, int>(103, iTextureID));
	}

	// Left Wall
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Tiles/Right_Wall.png", true);
	if (iTextureID == 0)
	{
		cout << "Unable to load Image/Scene2D_Right_Wall.png" << endl;
		return false;
	}
	else
	{
		// Store the texture ID into MapOfTextureIDs
		MapOfTextureIDs.insert(pair<int, int>(104, iTextureID));
	}

	// Top Left
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Tiles/Top_Left.png", true);
	if (iTextureID == 0)
	{
		cout << "Unable to load Image/Scene2D_Top_Left.png" << endl;
		return false;
	}
	else
	{
		// Store the texture ID into MapOfTextureIDs
		MapOfTextureIDs.insert(pair<int, int>(105, iTextureID));
	}

	// Top Right
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Tiles/Top_Right.png", true);
	if (iTextureID == 0)
	{
		cout << "Unable to load Image/Scene2D_Top_Right.png" << endl;
		return false;
	}
	else
	{
		// Store the texture ID into MapOfTextureIDs
		MapOfTextureIDs.insert(pair<int, int>(106, iTextureID));
	}

	// Bottom Left
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Tiles/Bottom_Left.png", true);
	if (iTextureID == 0)
	{
		cout << "Unable to load Image/Scene2D_Bottom_Left.png" << endl;
		return false;
	}
	else
	{
		// Store the texture ID into MapOfTextureIDs
		MapOfTextureIDs.insert(pair<int, int>(107, iTextureID));
	}

	// Bottom Right
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Tiles/Bottom_Right.png", true);
	if (iTextureID == 0)
	{
		cout << "Unable to load Image/Scene2D_Bottom_Right.png" << endl;
		return false;
	}
	else
	{
		// Store the texture ID into MapOfTextureIDs
		MapOfTextureIDs.insert(pair<int, int>(108, iTextureID));
	}

	// Load the tree texture
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Collectibles/Coin.png", true);
	if (iTextureID == 0)
	{
		cout << "Unable to load Image/Scene2D_Coin.png" << endl;
		return false;
	}
	else
	{
		// Store the texture ID into MapOfTextureIDs
		MapOfTextureIDs.insert(pair<int, int>(2, iTextureID));
	}
	// Load the Life texture
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Scene2D_Lives.tga", true);
	if (iTextureID == 0)
	{
		cout << "Unable to load Image/Scene2D_Lives.tga" << endl;
		return false;
	}
	else
	{
		// Store the texture ID into MapOfTextureIDs
		MapOfTextureIDs.insert(pair<int, int>(10, iTextureID));
	}
	// Load the spike texture
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Tiles/Spikes.png", true);
	if (iTextureID == 0)
	{
		cout << "Unable to load Image/Tiles/Spikes.png" << endl;
		return false;
	}
	else
	{
		// Store the texture ID into MapOfTextureIDs
		MapOfTextureIDs.insert(pair<int, int>(20, iTextureID));
	}
	// Load the spike base texture
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Tiles/Spikes_Base.png", true);
	if (iTextureID == 0)
	{
		cout << "Unable to load Image/Tiles/Spikes_Base.png" << endl;
		return false;
	}
	else
	{
		// Store the texture ID into MapOfTextureIDs
		MapOfTextureIDs.insert(pair<int, int>(21, iTextureID));
	}
	// Load the top spike texture
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Tiles/Spikes_Top.png", true);
	if (iTextureID == 0)
	{
		cout << "Unable to load Image/Tiles/Spikes_Top.png" << endl;
		return false;
	}
	else
	{
		// Store the texture ID into MapOfTextureIDs
		MapOfTextureIDs.insert(pair<int, int>(22, iTextureID));
	}
	// Load the spike texture
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Tiles/Spikes_Top_Base.png", true);
	if (iTextureID == 0)
	{
		cout << "Unable to load Image/Tiles/Spikes_Top_Base.png" << endl;
		return false;
	}
	else
	{
		// Store the texture ID into MapOfTextureIDs
		MapOfTextureIDs.insert(pair<int, int>(23, iTextureID));
	}
	// Load the up arrow texture
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Tiles/Arrow_Up.png", true);
	if (iTextureID == 0)
	{
		cout << "Unable to load Image/Tiles/Arrow_Up.png" << endl;
		return false;
	}
	else
	{
		// Store the texture ID into MapOfTextureIDs
		MapOfTextureIDs.insert(pair<int, int>(24, iTextureID));
	}
	// Load the down arrow texture
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Tiles/Arrow_Down.png", true);
	if (iTextureID == 0)
	{
		cout << "Unable to load Image/Tiles/Arrow_Down.png" << endl;
		return false;
	}
	else
	{
		// Store the texture ID into MapOfTextureIDs
		MapOfTextureIDs.insert(pair<int, int>(25, iTextureID));
	}
	// Load the left arrow texture
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Tiles/Arrow_Left.png", true);
	if (iTextureID == 0)
	{
		cout << "Unable to load Image/Tiles/Arrow_Left.png" << endl;
		return false;
	}
	else
	{
		// Store the texture ID into MapOfTextureIDs
		MapOfTextureIDs.insert(pair<int, int>(26, iTextureID));
	}
	// Load the right arrow texture
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Tiles/Arrow_Right.png", true);
	if (iTextureID == 0)
	{
		cout << "Unable to load Image/Tiles/Arrow_Right.png" << endl;
		return false;
	}
	else
	{
		// Store the texture ID into MapOfTextureIDs
		MapOfTextureIDs.insert(pair<int, int>(27, iTextureID));
	}
	// Load the speed potion texture
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Collectibles/Jump_Potion.png", true);
	if (iTextureID == 0)
	{
		cout << "Unable to load Image/Collectibles/Jump_Potion.png" << endl;
		return false;
	}
	else
	{
		// Store the texture ID into MapOfTextureIDs
		MapOfTextureIDs.insert(pair<int, int>(28, iTextureID));
	}
	
	// Load the Portal
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Tiles/Portal.png", true);
	if (iTextureID == 0)
	{
		cout << "Unable to load Image/Tiles/Portal.png" << endl;
		return false;
	}
	else
	{
		// Store the texture ID into MapOfTextureIDs
		MapOfTextureIDs.insert(pair<int, int>(98, iTextureID));
	}
	// Load the EndPortal
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Tiles/End_Portal.png", true);
	if (iTextureID == 0)
	{
		cout << "Unable to load Image/Tiles/End_Portal.png" << endl;
		return false;
	}
	else
	{
		// Store the texture ID into MapOfTextureIDs
		MapOfTextureIDs.insert(pair<int, int>(99, iTextureID));
	}

	// Initialise the variables for AStar
	m_weight = 1;
	m_startPos = glm::vec2(0, 0);
	m_targetPos = glm::vec2(0, 0);
	//m_size = cSettings->NUM_TILES_YAXIS* cSettings->NUM_TILES_XAXIS;

	m_nrOfDirections = 4;
	m_directions = { { -1, 0 }, { 1, 0 }, { 0, 1 }, { 0, -1 },
						{ -1, -1 }, { 1, 1 }, { -1, 1 }, { 1, -1 } };

	// Resize these 2 lists
	m_cameFromList.resize(cSettings->NUM_TILES_YAXIS * cSettings->NUM_TILES_XAXIS);
	m_closedList.resize(cSettings->NUM_TILES_YAXIS * cSettings->NUM_TILES_XAXIS, false);

	//// Clear AStar memory
	//ClearAStar();

	return true;
}

/**
@brief Update Update this instance
*/
bool CMap2D::Update(const double dElapsedTime)
{
	return true;
}

/**
 @brief Constraint the player's position within a boundary
 @return true if the vec2Position was constained by this method, otherwise false
 */
bool CMap2D::Constraint(glm::vec2& vec2Position)
{
	if (vec2Position.x <= TileHalfSize.x)
	{
		vec2Position.x = TileHalfSize.x;
		return true;
	}
	else if (vec2Position.x >= cSettings->iWindowWidth - TileHalfSize.x)
	{
		vec2Position.x = cSettings->iWindowWidth - TileHalfSize.x;
		return true;
	}

	if (vec2Position.y >= cSettings->iWindowHeight - TileHalfSize.y)
	{
		vec2Position.y = cSettings->iWindowHeight - TileHalfSize.y;
		return true;
	}
	else if (vec2Position.y <= TileHalfSize.y)
	{
		vec2Position.y = max(vec2Position.y, TileHalfSize.y);
		return true;
	}

	return false;
}

/**
 @brief Check for collision with the tile maps vertically when an object moves from vec2StartPosition to vec2EndPosition
 @param vec2Position A glm::vec2 variable which is the position to check for
 @param vec2HalfSize A glm::vec2 variable which is the half size of the bounding box to check for
 @param vec2Displacement A glm::vec2 variable which is the displacement from the vec2Position
 @return true if the vec2EndPosition was modified due to collision, otherwise false
 */
CSettings::RESULTS CMap2D::CheckVerticalCollision(	glm::vec2 vec2StartPosition,
													glm::vec2 vec2HalfSize,
													glm::vec2& vec2EndPosition)
{
	glm::vec2 vec2StartLeft, vec2StartRight, vec2EndLeft, vec2EndRight;

	// 1. Get the bounding box of the area/grids which is occupied by object
	//    The start and end positions are in the from- and to- direction of the movement
	if (vec2EndPosition.y > vec2StartPosition.y)
	{
		// Jumping up since vec2EndPosition.y > vec2StartPosition.y
		vec2StartLeft = vec2StartPosition - vec2HalfSize + vec2SizeTolerance;
		vec2StartRight = glm::vec2(vec2StartPosition.x + vec2HalfSize.x - vec2SizeTolerance.x, vec2StartPosition.y - vec2HalfSize.y + vec2SizeTolerance.y);
		vec2EndLeft = glm::vec2(vec2EndPosition.x - vec2HalfSize.x + vec2SizeTolerance.x, vec2EndPosition.y + vec2HalfSize.y - vec2SizeTolerance.y);
		vec2EndRight = vec2EndPosition + vec2HalfSize - vec2SizeTolerance;
	}
	else
	{
		// Falling down since vec2EndPosition.y <= vec2StartPosition.y
		vec2StartLeft = glm::vec2(vec2StartPosition.x - vec2HalfSize.x + vec2SizeTolerance.x, vec2StartPosition.y + vec2HalfSize.y - vec2SizeTolerance.y);
		vec2StartRight = vec2StartPosition + vec2HalfSize - vec2SizeTolerance;
		vec2EndLeft = vec2EndPosition - vec2HalfSize + vec2SizeTolerance;
		vec2EndRight = glm::vec2(vec2EndPosition.x + vec2HalfSize.x - vec2SizeTolerance.x, vec2EndPosition.y - vec2HalfSize.y + vec2SizeTolerance.y);
	}

	// 2. Check if the bounding box is within the map
	// Get the indices for vec2StartLeft
	int iStartLeftX = 0;
	int iStartLeftY = 0;
	if (GetTileIndexAtPosition(vec2StartLeft, iStartLeftX, iStartLeftY) == false)
	{
		cout << "A. This position is outside of the map: " << vec2StartLeft.x << ", " << vec2StartLeft.y << endl;
		return CSettings::RESULTS::INVALID;
	}

	// Get the indices for vec2BottomRight
	int iStartRightX = 0;
	int iStartRightY = 0;
	if (GetTileIndexAtPosition(vec2StartRight, iStartRightX, iStartRightY) == false)
	{
		cout << "B. This position is outside of the map: " << vec2StartRight.x << ", " << vec2StartRight.y << endl;
		return CSettings::RESULTS::INVALID;
	}

	// Get the indices for vec2TopLeft
	int iEndLeftX = 0;
	int iEndLeftY = 0;
	if (GetTileIndexAtPosition(vec2EndLeft, iEndLeftX, iEndLeftY) == false)
	{
		cout << "C. This position is outside of the map: " << vec2EndLeft.x << ", " << vec2EndLeft.y << endl;
		return CSettings::RESULTS::INVALID;
	}

	// Get the indices for vec2TopRight
	int iEndRightX = 0;
	int iEndRightY = 0;
	if (GetTileIndexAtPosition(vec2EndRight, iEndRightX, iEndRightY) == false)
	{
		cout << "D. This position is outside of the map: " << vec2EndRight.x << ", " << vec2EndRight.y << endl;
		return CSettings::RESULTS::INVALID;
	}

	// 3. Check the direction of the movement and check for collision in that direction

	// Check the top-bottom for collision a.k.a. check for objects falling down
	if ((vec2EndPosition - vec2StartPosition).y < 0.0f)
	{
		// Check the bottom side for collisions
		for (int iRow = iStartLeftY; iRow >= iEndLeftY; iRow--)
		{
			for (int iCol = iEndLeftX; iCol <= iEndRightX; iCol++)
			{
				int iTileInfo = GetMapInfo(iRow, iCol, true);
				if ((iTileInfo >= 100) && (iTileInfo < 200))
				{
					//Calculate new vec2EndPosition
					if (iRow == iStartLeftY)
					{
						cout << "The starting index for collision check CANNOT have collision. We will skip this" << endl;
						continue;
					}
					
					//Calculate new vec2EndPosition
					vec2EndPosition.y = (iRow + 1) * TileSize.y + vec2HalfSize.y;

					return CSettings::RESULTS::POSITIVE;
				}
			}
		}
	}
	else if ((vec2EndPosition - vec2StartPosition).y > 0.0f)
	{
		// Check the top side for collisions a.k.a. check for objects jumping up
		for (int iRow = iStartLeftY; iRow <= iEndLeftY; iRow++)
		{
			for (int iCol = iEndLeftX; iCol <= iEndRightX; iCol++)
			{
				int iTileInfo = GetMapInfo(iRow, iCol, true);
				if ((iTileInfo >= 100) && (iTileInfo < 200))
				{
					if (iRow == iStartLeftY)
					{
						cout << "The starting index for collision check CANNOT have collision. We will skip this" << endl;
						continue;
					}

					//Calculate new vec2EndPosition
					vec2EndPosition.y = (iRow - 1) * TileSize.y + vec2HalfSize.y;

					return CSettings::RESULTS::POSITIVE;
				}
			}
		}
	}

	return CSettings::RESULTS::NEGATIVE;
}

/**
 @brief Check for collision with the tile maps horizontically when an object moves from vec2StartPosition to vec2EndPosition
 @param vec2Position A glm::vec2 variable which is the position to check for
 @param vec2HalfSize A glm::vec2 variable which is the half size of the bounding box to check for
 @param vec2Displacement A glm::vec2 variable which is the displacement from the vec2Position
 @return true if the vec2EndPosition was modified due to collision, otherwise false
 */
CSettings::RESULTS CMap2D::CheckHorizontalCollision(	glm::vec2 vec2StartPosition,
														glm::vec2 vec2HalfSize,
														glm::vec2& vec2EndPosition)
{
	glm::vec2 vec2StartTop, vec2StartBottom, vec2EndTop, vec2EndBottom;

	// 1. Get the bounding box of the area/grids which is occupied by object
	if (vec2EndPosition.x > vec2StartPosition.x)
	{
		// Moving to the right since vec2EndPosition.x > vec2StartPosition.x
		vec2StartTop = glm::vec2(vec2StartPosition.x - vec2HalfSize.x + vec2SizeTolerance.x, vec2StartPosition.y + vec2HalfSize.y - vec2SizeTolerance.y);
		vec2StartBottom = vec2StartPosition - vec2HalfSize + vec2SizeTolerance;
		vec2EndTop = vec2EndPosition + vec2HalfSize - vec2SizeTolerance;
		vec2EndBottom = glm::vec2(vec2EndPosition.x + vec2HalfSize.x - vec2SizeTolerance.x, vec2EndPosition.y - vec2HalfSize.y + vec2SizeTolerance.y);
	}
	else
	{
		// Moving to the left since vec2EndPosition.x <= vec2StartPosition.x
		vec2StartTop = vec2StartPosition + vec2HalfSize - vec2SizeTolerance;
		vec2StartBottom = glm::vec2(vec2StartPosition.x + vec2HalfSize.x - vec2SizeTolerance.x, vec2StartPosition.y - vec2HalfSize.y + vec2SizeTolerance.y);
		vec2EndTop = glm::vec2(vec2EndPosition.x - vec2HalfSize.x + vec2SizeTolerance.x, vec2EndPosition.y + vec2HalfSize.y - vec2SizeTolerance.y);
		vec2EndBottom = vec2EndPosition - vec2HalfSize + vec2SizeTolerance;
	}

	// 2. Check if the bounding box is within the map
	// Get the indices for vec2StartTop
	int iStartTopX = 0;
	int iStartTopY = 0;
	if (GetTileIndexAtPosition(vec2StartTop, iStartTopX, iStartTopY) == false)
	{
		cout << "A. This position is outside of the map: " << vec2StartTop.x << ", " << vec2StartTop.y << endl;
		return CSettings::RESULTS::INVALID;
	}

	// Get the indices for vec2StartBottom
	int iStartBottomX = 0;
	int iStartBottomY = 0;
	if (GetTileIndexAtPosition(vec2StartBottom, iStartBottomX, iStartBottomY) == false)
	{
		cout << "B. This position is outside of the map: " << vec2StartBottom.x << ", " << vec2StartBottom.y << endl;
		return CSettings::RESULTS::INVALID;
	}

	// Get the indices for vec2EndTop
	int iEndTopX = 0;
	int iEndTopY = 0;
	if (GetTileIndexAtPosition(vec2EndTop, iEndTopX, iEndTopY) == false)
	{
		cout << "C. This position is outside of the map: " << vec2EndTop.x << ", " << vec2EndTop.y << endl;
		return CSettings::RESULTS::INVALID;
	}

	// Get the indices for vec2EndBottom
	int iEndBottomX = 0;
	int iEndBottomY = 0;
	if (GetTileIndexAtPosition(vec2EndBottom, iEndBottomX, iEndBottomY) == false)
	{
		cout << "D. This position is outside of the map: " << vec2EndBottom.x << ", " << vec2EndBottom.y << endl;
		return CSettings::RESULTS::INVALID;
	}

	// 3. Check the direction of the movement and check for collision in that direction

	// Check the right-left for collision a.k.a. check for objects moving to the left
	if ((vec2EndPosition - vec2StartPosition).x < 0.0f)
	{
		// Check the right-left for collision a.k.a. check for objects moving to the left
		for (int iCol = iStartBottomX; iCol >= iEndBottomX; iCol--)
		{
			for (int iRow = iStartBottomY; iRow <= iStartTopY; iRow++)
			{
				int iTileInfo = GetMapInfo(iRow, iCol, true);
				if ((iTileInfo >= 100) && (iTileInfo < 200))
				{
					if (iCol == iStartBottomX)
					{
						cout << "The starting index for collision check CANNOT have collision. We will skip this" << endl;
						continue;
					}

					//Calculate new vec2EndPosition
					vec2EndPosition.x = (iCol + 1) * TileSize.x + vec2HalfSize.x;

					return CSettings::RESULTS::POSITIVE;
				}
			}
		}
	}
	else if ((vec2EndPosition - vec2StartPosition).x > 0.0f)
	{
		// Check the left-right for collision a.k.a. check for objects moving to the right
		for (int iCol = iStartBottomX; iCol <= iEndBottomX; iCol++)
		{
			for (int iRow = iStartBottomY; iRow <= iStartTopY; iRow++)
			{
				int iTileInfo = GetMapInfo(iRow, iCol, true);
				if ((iTileInfo >= 100) && (iTileInfo < 200))
				{
					if (iCol == iStartBottomX)
					{
						cout << "The starting index for collision check CANNOT have collision. We will skip this" << endl;
						continue;
					}

					//Calculate new vec2EndPosition
					vec2EndPosition.x = (iCol - 1) * TileSize.x + vec2HalfSize.x;

					return CSettings::RESULTS::POSITIVE;
				}
			}
		}
	}

	return CSettings::RESULTS::NEGATIVE;
}

/**
 @brief PreRender Set up the OpenGL display environment before rendering
 */
void CMap2D::PreRender(void)
{
	// Activate blending mode
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Activate the shader
	CShaderManager::GetInstance()->Use(sShaderName);
}

/**
 @brief Render Render this instance
 */
void CMap2D::Render(void)
{
	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);

	for (unsigned int uiRow = 0; uiRow < cSettings->NUM_TILES_YAXIS; uiRow++)
	{
		for (unsigned int uiCol = 0; uiCol < cSettings->NUM_TILES_XAXIS; uiCol++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(uiCol * 18.75f + this->TileHalfSize.x,
													(cSettings->NUM_TILES_YAXIS - 1 - uiRow) * 18.75f + this->TileHalfSize.y,
													0.0f));

			//model = glm::translate(model, glm::vec3(0.5f * cSettings->TILE_WIDTH, 0.5f * cSettings->TILE_HEIGHT, 0.0f));
			//model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f));
			//model = glm::translate(model, glm::vec3(-0.5f * cSettings->TILE_WIDTH, -0.5f * cSettings->TILE_HEIGHT, 0.0f));

			model = glm::scale(model, glm::vec3(18.75f, 18.75f, 1.0f));

			// Upload the model to OpenGL shader
			CShaderManager::GetInstance()->activeShader->setMat4("Model", model);
			// Upload the Projection to OpenGL shader
			CShaderManager::GetInstance()->activeShader->setMat4("Projection", projection);
			unsigned int colourLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "ColourTint");
			glUniform4fv(colourLoc, 1, glm::value_ptr(vec4ColourTint));

			// Render a tile
			RenderTile(uiRow, uiCol);
		}
	}
}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CMap2D::PostRender(void)
{
	// Disable blending
	glDisable(GL_BLEND);
}

/**
 @brief Set Tile Size, and it automatically sets the half tile size
 @param x The tile size in the x-axis
 @param y The tile size in the y-axis
 */
void CMap2D::SetTileSize(const float x, const float y)
{
	TileSize = glm::vec2(x, y);
	SetTileHalfSize(x * 0.5f, y * 0.5f);
}
/**
 @brief Set half Tile Size
 @param x The half tile size in the x-axis
 @param y The half tile size in the y-axis
 */
void CMap2D::SetTileHalfSize(const float x, const float y)
{
	TileHalfSize = glm::vec2(x, y);
}

/**
 @brief Get Tile Size
 @return The tile size
 */
glm::vec2 CMap2D::GetTileSize(void)
{
	return TileSize;
}

/**
 @brief Get half Tile Size
 @return The half tile size
 */
glm::vec2 CMap2D::GetTileHalfSize(void)
{
	return TileHalfSize;
}

// Set the specifications of the map
void CMap2D::SetNumTiles(const CSettings::AXIS sAxis, const unsigned int uiValue)
{
	// Check if the value is valid
	if (uiValue <= 0)
	{
		cout << "CMap2D::SetNumTiles() : value must be more than 0" << endl;
		return;
	}

	if (sAxis == CSettings::x)
	{
		cSettings->NUM_TILES_XAXIS = uiValue;
		cSettings->UpdateSpecifications();
	}
	else if (sAxis == CSettings::y)
	{
		cSettings->NUM_TILES_YAXIS = uiValue;
		cSettings->UpdateSpecifications();
	}
	else if (sAxis == CSettings::z)
	{
		// Not used in here
	}
	else
	{
		cout << "Unknown axis" << endl;
	}
}

// Set the specifications of the map
void CMap2D::SetNumSteps(const CSettings::AXIS sAxis, const unsigned int uiValue)
{
	// Check if the value is valid
	if (uiValue <= 0)
	{
		cout << "CMap2D::SetNumSteps() : value must be more than 0" << endl;
		return;
	}

	if (sAxis == CSettings::x)
	{
		cSettings->NUM_STEPS_PER_TILE_XAXIS = (float)uiValue;
		cSettings->UpdateSpecifications();
	}
	else if (sAxis == CSettings::y)
	{
		cSettings->NUM_STEPS_PER_TILE_YAXIS = (float)uiValue;
		cSettings->UpdateSpecifications();
	}
	else if (sAxis == CSettings::z)
	{
		// Not used in here
	}
	else
	{
		cout << "Unknown axis" << endl;
	}
}

/**
 @brief Get indices of tiles at a position
 @param vec2Position A glm::vec2 variable which is the position to get the tile indices for
 @param iTileIndexEndX A int& variable to store the tile index in the x-axis
 @param iTileIndexEndY A int& variable to store the tile index in the y-axis
 @return true if the tile indices are return, false if the position is not within the tile map
 */
bool CMap2D::GetTileIndexAtPosition(const glm::vec2 vec2Position, int& iTileIndexEndX, int& iTileIndexEndY)
{
	if (GetTileIndexXAtPosition(vec2Position, iTileIndexEndX) == false)
		return false;

	if (GetTileIndexYAtPosition(vec2Position, iTileIndexEndY) == false)
		return false;

	return true;
}

/**
 @brief Get indices in X-axis of a tile at a position
 @param vec2Position A glm::vec2 variable which is the position to get the tile indices for
 @param iTileIndexEndX A int& variable to store the tile index in the x-axis
 @return true if the tile indices are return, false if the position is not within the tile map
 */
bool CMap2D::GetTileIndexXAtPosition(const glm::vec2 vec2Position, int& iTileIndexEndX)
{
	if ((vec2Position.x < 0) || (vec2Position.x > (float)cSettings->iWindowWidth))
	{
		return false;
	}

	iTileIndexEndX = (int)(vec2Position.x / TileSize.x);

	return true;
}

/**
 @brief Get indices in Y-axis of a tile at a position
 @param vec2Position A glm::vec2 variable which is the position to get the tile indices for
 @param iTileIndexEndY A int& variable to store the tile index in the y-axis
 @return true if the tile indices are return, false if the position is not within the tile map
 */
bool CMap2D::GetTileIndexYAtPosition(const glm::vec2 vec2Position, int& iTileIndexEndY)
{
	if ((vec2Position.y < 0) || (vec2Position.y > (float)cSettings->iWindowHeight))
	{
		return false;
	}

	iTileIndexEndY = (int)(vec2Position.y / TileSize.y);

	return true;
}


/**
 @brief Set the value at certain indices in the arrMapInfo
 @param iRow A const int variable containing the row index of the element to set to
 @param iCol A const int variable containing the column index of the element to set to
 @param iValue A const int variable containing the value to assign to this arrMapInfo
 */
void CMap2D::SetMapInfo(const unsigned int uiRow, const unsigned int uiCol, const int iValue, const bool bInvert)
{
	if (bInvert)
		arrMapInfo[uiCurLevel][cSettings->NUM_TILES_YAXIS - uiRow - 1][uiCol].value = iValue;
	else
		arrMapInfo[uiCurLevel][uiRow][uiCol].value = iValue;
}

/**
 @brief Get the value at certain indices in the arrMapInfo
 @param iRow A const int variable containing the row index of the element to get from
 @param iCol A const int variable containing the column index of the element to get from
 @param bInvert A const bool variable which indicates if the row information is inverted
 */
int CMap2D::GetMapInfo(const unsigned int uiRow, const int unsigned uiCol, const bool bInvert) const
{
	if (bInvert)
		return arrMapInfo[uiCurLevel][cSettings->NUM_TILES_YAXIS - uiRow - 1][uiCol].value;
	else
		return arrMapInfo[uiCurLevel][uiRow][uiCol].value;
}

/**
 @brief Load a map
 */ 
bool CMap2D::LoadMap(string filename, const unsigned int uiCurLevel)
{
	doc = rapidcsv::Document(FileSystem::getPath(filename).c_str());

	// Check if the sizes of CSV data matches the declared arrMapInfo sizes
	if ((cSettings->NUM_TILES_XAXIS != (unsigned int)doc.GetColumnCount()) ||
		(cSettings->NUM_TILES_YAXIS != (unsigned int)doc.GetRowCount()))
	{
		cout << "Sizes of CSV map does not match declared arrMapInfo sizes." << endl;
		return false;
	}

	// Read the rows and columns of CSV data into arrMapInfo
	for (unsigned int uiRow = 0; uiRow < cSettings->NUM_TILES_YAXIS; uiRow++)
	{
		// Read a row from the CSV file
		std::vector<std::string> row = doc.GetRow<std::string>(uiRow);
		
		// Load a particular CSV value into the arrMapInfo
		for (unsigned int uiCol = 0; uiCol < cSettings->NUM_TILES_XAXIS; ++uiCol)
		{
			arrMapInfo[uiCurLevel][uiRow][uiCol].value = (int)stoi(row[uiCol]);
		}
	}

	return true;
}

/**
 @brief Save the tilemap to a text file
 @param filename A string variable containing the name of the text file to save the map to
 */
bool CMap2D::SaveMap(string filename, const unsigned int uiCurLevel)
{
	// Update the rapidcsv::Document from arrMapInfo
	for (unsigned int uiRow = 0; uiRow < cSettings->NUM_TILES_YAXIS; uiRow++)
	{
		for (unsigned int uiCol = 0; uiCol < cSettings->NUM_TILES_XAXIS; uiCol++)
		{
			doc.SetCell(uiCol, uiRow, arrMapInfo[uiCurLevel][uiRow][uiCol].value);
		}
		cout << endl;
	}

	// Save the rapidcsv::Document to a file
	doc.Save(FileSystem::getPath(filename).c_str());

	return true;
}

/**
@brief Find the indices of a certain value in arrMapInfo
@param iValue A const int variable containing the row index of the found element
@param iRow A const int variable containing the row index of the found element
@param iCol A const int variable containing the column index of the found element
@param bInvert A const bool variable which indicates if the row information is inverted
*/
bool CMap2D::FindValue(const int iValue, unsigned int& uirRow, unsigned int& uirCol, const bool bInvert)
{
	for (unsigned int uiRow = 0; uiRow < cSettings->NUM_TILES_YAXIS; uiRow++)
	{
		for (unsigned int uiCol = 0; uiCol < cSettings->NUM_TILES_XAXIS; uiCol++)
		{
			if (arrMapInfo[uiCurLevel][uiRow][uiCol].value == iValue)
			{
				if (bInvert)
					uirRow = cSettings->NUM_TILES_YAXIS - uiRow - 1;
				else
					uirRow = uiRow;
				uirCol = uiCol;
				return true;	// Return true immediately if the value has been found
			}
		}
	}
	return false;
}

/**
 @brief Set current level
 */
void CMap2D::SetCurrentLevel(unsigned int uiCurLevel)
{
	if (uiCurLevel < uiNumLevels)
	{
		this->uiCurLevel = uiCurLevel;
	}
}
/**
 @brief Get current level
 */
unsigned int CMap2D::GetCurrentLevel(void) const
{
	return uiCurLevel;
}

/**
 @brief Render a tile at a position based on its tile index
 @param iRow A const int variable containing the row index of the tile
 @param iCol A const int variable containing the column index of the tile
 */
void CMap2D::RenderTile(const unsigned int uiRow, const unsigned int uiCol)
{
	if (arrMapInfo[uiCurLevel][uiRow][uiCol].value != 0)
	{
		//if (arrMapInfo[uiCurLevel][uiRow][uiCol].value < 3)
		glBindTexture(GL_TEXTURE_2D, MapOfTextureIDs.at(arrMapInfo[uiCurLevel][uiRow][uiCol].value));
			glBindVertexArray(VAO);
				//CS: Render the tile
				quadMesh->Render();
			glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}


/**
 @brief Find a path
 */
std::vector<glm::vec2> CMap2D::PathFind(	const glm::vec2& startPos, 
											const glm::vec2& targetPos, 
											HeuristicFunction heuristicFunc, 
											const int weight)
{
	// Check if the startPos and targetPost are blocked
	if (isBlocked(startPos.y, startPos.x) ||
		(isBlocked(targetPos.y, targetPos.x)))
	{
		cout << "Invalid start or target position." << endl;
		// Return an empty path
		std::vector<glm::vec2> path;
		return path;
	}

	// Set up the variables and lists
	m_startPos = startPos;
	m_targetPos = targetPos;
	m_weight = weight;
	m_heuristic = std::bind(heuristicFunc, _1, _2, _3);

	// Reset AStar lists
	ResetAStarLists();

	// Add the start pos to 2 lists
	m_cameFromList[ConvertTo1D(m_startPos)].parent = m_startPos;
	m_openList.push(Grid(m_startPos, 0));

	unsigned int fNew, gNew, hNew;
	glm::vec2 currentPos;

	// Start the path finding...
	while (!m_openList.empty())
	{
		// Get the node with the least f value
		currentPos = m_openList.top().pos;
		//cout << endl << "*** New position to check: " << currentPos.x << ", " << currentPos.y << endl;
		//cout << "*** targetPos: " << m_targetPos.x << ", " << m_targetPos.y << endl;

		// If the targetPos was reached, then quit this loop
		if (currentPos == m_targetPos)
		{
			//cout << "=== Found the targetPos: " << m_targetPos.x << ", " << m_targetPos.y << endl;
			while (m_openList.size() != 0)
				m_openList.pop();
			break;
		}

		m_openList.pop();
		m_closedList[ConvertTo1D(currentPos)] = true;

		// Check the neighbors of the current node
		for (unsigned int i = 0; i < m_nrOfDirections; ++i)
		{
			const auto neighborPos = currentPos + m_directions[i];
			const auto neighborIndex = ConvertTo1D(neighborPos);

			//cout << "\t#" << i << ": Check this: " << neighborPos.x << ", " << neighborPos.y << ":\t";
			if (!isValid(neighborPos) || 
				isBlocked(neighborPos.y, neighborPos.x) || 
				m_closedList[neighborIndex] == true)
			{
				//cout << "This position is not valid. Going to next neighbour." << endl;
				continue;
			}

			gNew = m_cameFromList[ConvertTo1D(currentPos)].g + 1;
			hNew = m_heuristic(neighborPos, m_targetPos, m_weight);
			fNew = gNew + hNew;

			if (m_cameFromList[neighborIndex].f == 0 || fNew < m_cameFromList[neighborIndex].f)
			{
				//cout << "Adding to Open List: " << neighborPos.x << ", " << neighborPos.y;
				//cout << ". [ f : " << fNew << ", g : " << gNew << ", h : " << hNew << "]" << endl;
				m_openList.push(Grid(neighborPos, fNew));
				m_cameFromList[neighborIndex] = { neighborPos, currentPos, fNew, gNew, hNew };
			}
			else
			{
				//cout << "Not adding this" << endl;
			}
		}
		//system("pause");
	}

	return BuildPath();
}

/**
 @brief Build a path
 */
std::vector<glm::vec2> CMap2D::BuildPath() const
{
	std::vector<glm::vec2> path;
	auto currentPos = m_targetPos;
	auto currentIndex = ConvertTo1D(currentPos);

	while (!(m_cameFromList[currentIndex].parent == currentPos))
	{
		path.push_back(currentPos);
		currentPos = m_cameFromList[currentIndex].parent;
		currentIndex = ConvertTo1D(currentPos);
	}

	// If the path has only 1 entry, then it is the the target position
	if (path.size() == 1)
	{
		// if m_startPos is next to m_targetPos, then having 1 path point is OK
		if (m_nrOfDirections == 4)
		{
			if (abs(m_targetPos.y - m_startPos.y) + abs(m_targetPos.x - m_startPos.x) > 1)
				path.clear();
		}
		else
		{
			if (abs(m_targetPos.y - m_startPos.y) + abs(m_targetPos.x - m_startPos.x) > 2)
				path.clear();
			else if (abs(m_targetPos.y - m_startPos.y) + abs(m_targetPos.x - m_startPos.x) > 1)
				path.clear();
		}
	}
	else
		std::reverse(path.begin(), path.end());

	return path;
}

/**
 @brief Toggle the checks for diagonal movements
 */
void CMap2D::SetDiagonalMovement(const bool bEnable)
{
	m_nrOfDirections = (bEnable) ? 8 : 4;
}

/**
 @brief Print out the details about this class instance in the console
 */
void CMap2D::PrintSelf(void) const
{
	cout << endl << "AStar::PrintSelf()" << endl;

	for (unsigned uiLevel = 0; uiLevel < uiNumLevels; uiLevel++)
	{
		cout << "Level: " << uiLevel << endl;
		for (unsigned uiRow = 0; uiRow < cSettings->NUM_TILES_YAXIS; uiRow++)
		{
			for (unsigned uiCol = 0; uiCol < cSettings->NUM_TILES_XAXIS; uiCol++)
			{
				cout.fill('0');
				cout.width(3);
				cout << arrMapInfo[uiLevel][uiRow][uiCol].value;
				if (uiCol != cSettings->NUM_TILES_XAXIS - 1)
					cout << ", ";
				else
					cout << endl;
			}
		}
	}

	cout << "m_openList: " << m_openList.size() << endl;
	cout << "m_cameFromList: " << m_cameFromList.size() << endl;
	cout << "m_closedList: " << m_closedList.size() << endl;

	cout << "===== AStar::PrintSelf() =====" << endl;
}

/**
 @brief Check if a position is valid
 */
bool CMap2D::isValid(const glm::vec2& pos) const
{
	return (pos.x >= 0) && (pos.x < cSettings->NUM_TILES_XAXIS) &&
		(pos.y >= 0) && (pos.y < cSettings->NUM_TILES_YAXIS);
}

/**
 @brief Check if a grid is blocked
 */
bool CMap2D::isBlocked(const unsigned int uiRow, const unsigned int uiCol, const bool bInvert) const
{
	if (bInvert == true)
	{
		if ((arrMapInfo[uiCurLevel][cSettings->NUM_TILES_YAXIS - uiRow - 1][uiCol].value >= 100) &&
			(arrMapInfo[uiCurLevel][cSettings->NUM_TILES_YAXIS - uiRow - 1][uiCol].value < 200))
			return true;
		else
			return false;
	}
	else
	{
		if ((arrMapInfo[uiCurLevel][uiRow][uiCol].value >= 100) &&
			(arrMapInfo[uiCurLevel][uiRow][uiCol].value < 200))
			return true;
		else
			return false;
	}
}

/**
 @brief Returns a 1D index based on a 2D coordinate using row-major layout
 @param pos A const glm::vec2& variable containing the position to convert to 1D
 @return The int value containing the 1D value
 */
int CMap2D::ConvertTo1D(const glm::vec2& pos) const
{
	//return (pos.y * m_dimensions.x) + pos.x;
	return (pos.y * cSettings->NUM_TILES_XAXIS) + pos.x;
}

/**
 @brief Delete AStar lists
 @return true is successfully delete the AStar Lists, otherwise false.
 */
bool CMap2D::DeleteAStarLists(void)
{
	// Delete m_openList
	while (m_openList.size() != 0)
		m_openList.pop();
	// Delete m_cameFromList
	m_cameFromList.clear();
	// Delete m_closedList
	m_closedList.clear();

	return true;
}

/**
 @brief Reset AStar lists
 @return true is successfully reset the AStar Lists, otherwise false.
 */
bool CMap2D::ResetAStarLists(void)
{
	// Delete m_openList
	while (m_openList.size() != 0)
		m_openList.pop();
	// Reset m_cameFromList
	for (int i = 0; i < m_cameFromList.size(); i++)
	{
		m_cameFromList[i].pos = glm::vec2(0,0);
		m_cameFromList[i].parent = glm::vec2(0, 0);
		m_cameFromList[i].f = 0;
		m_cameFromList[i].g = 0;
		m_cameFromList[i].h = 0;
	}
	// Reset m_closedList
	for (int i = 0; i < m_closedList.size(); i++)
	{
		m_closedList[i] = false;
	}

	return true;
}


/**
 @brief manhattan calculation method for calculation of h
 @return An unsigned int of the value of h
 */
unsigned int heuristic::manhattan(const glm::vec2& v1, const glm::vec2& v2, int weight)
{
	glm::vec2 delta = v2 - v1;
	return static_cast<unsigned int>(weight * (delta.x + delta.y));
}

/**
 @brief euclidean calculation method for calculation of h
 @return An unsigned int of the value of h
 */
unsigned int heuristic::euclidean(const glm::vec2& v1, const glm::vec2& v2, int weight)
{
	glm::vec2 delta = v2 - v1;
	return static_cast<unsigned int>(weight * sqrt((delta.x * delta.x) + (delta.y * delta.y)));
}