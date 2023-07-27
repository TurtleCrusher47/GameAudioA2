#include "LevelOfDetails.h"

/**
 @brief Constructor
*/
CLevelOfDetails::CLevelOfDetails(void)
	: m_bLODStatus(false)
	, eDetailLevel(HIGH_DETAILS)
{
	arrLODDistance[HIGH_DETAILS]	= 0.0f;
	arrLODDistance[MID_DETAILS]		= 15.0f;
	arrLODDistance[LOW_DETAILS]		= 30.0f;
}

/** 
@brief Destructor
*/
CLevelOfDetails::~CLevelOfDetails(void)
{
}

/**
@brief Set the distances for different LODs
@param fDistance_HighDetails A const float variable storing the distance from the camera to this grid
@param fDistance_MidDetails A const float variable storing the distance from the camera to this grid
@param fDistance_LowDetails A const float variable storing the distance from the camera to this grid
*/
void CLevelOfDetails::SetLODDistances(	const float fDistance_HighDetails,
										const float fDistance_MidDetails, 
										const float fDistance_LowDetails)
{
	arrLODDistance[HIGH_DETAILS] = fDistance_HighDetails;
	arrLODDistance[MID_DETAILS] = fDistance_MidDetails;
	arrLODDistance[LOW_DETAILS] = fDistance_LowDetails;
}


/** 
@brief Get the LOD Status
*/
bool CLevelOfDetails::GetLODStatus(void) const
{
	return m_bLODStatus;
}

/** 
@brief Set the status of the LOD
*/
void CLevelOfDetails::SetLODStatus(const bool m_bLODStatus)
{
	this->m_bLODStatus = m_bLODStatus;

	// If we disable LOD, then let it always return HIGH_DETAILS when you call GetDetailLevel()
	if (this->m_bLODStatus == false)
	{
		eDetailLevel = HIGH_DETAILS;
	}
}

/** 
@brief Get the current detail level
*/
CLevelOfDetails::DETAIL_LEVEL CLevelOfDetails::GetDetailLevel(void) const
{
	return eDetailLevel;
}

/**
@brief Update this class instance
@param fDistance A const float variable storing the distance from the camera to this grid
*/
void CLevelOfDetails::Update(const float fDistance)
{
	if (fDistance > arrLODDistance[CLevelOfDetails::LOW_DETAILS])
		eDetailLevel = CLevelOfDetails::LOW_DETAILS;
	else if ((fDistance > arrLODDistance[CLevelOfDetails::MID_DETAILS]) && (fDistance <= arrLODDistance[CLevelOfDetails::LOW_DETAILS]))
		eDetailLevel = CLevelOfDetails::MID_DETAILS;
	else
		eDetailLevel = CLevelOfDetails::HIGH_DETAILS;
}

/** 
@brief Set the current detail level
*/
bool CLevelOfDetails::SetDetailLevel(const DETAIL_LEVEL eDetailLevel)
{
	if ((eDetailLevel >= HIGH_DETAILS) && (eDetailLevel < NUM_DETAIL_LEVEL))
	{
		this->eDetailLevel = eDetailLevel;
		return true;
	}
	return false;
}
