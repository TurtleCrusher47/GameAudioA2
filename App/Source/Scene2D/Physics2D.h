/**
 CPhysics2D
 By: Toh Da Jun
 Date: Sep 2021
 */
#pragma once

#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

class CPhysics2D
{
public:
	// Physics statuses
	enum class VERTICALSTATUS : unsigned int
	{
		IDLE = 0,
		JUMP,
		DOUBLEJUMP,
		FALL,
		NUM_VERTICALSTATUS
	};
	enum class HORIZONTALSTATUS : unsigned int
	{
		IDLE = 0,
		WALK,
		DASH,
		NUM_HORIZONTALSTATUS
	};

	// Constructor
	CPhysics2D(void);

	// Destructor
	virtual ~CPhysics2D(void);

	// Init
	bool Init(void);

	// Set methods
	void SetInitialVelocity(const glm::vec2 v2InitialVelocity);				// Set Initial velocity
	void SetFinalVelocity(const glm::vec2 v2FinalVelocity);					// Set Final velocity
	void SetAcceleration(const glm::vec2 v2Acceleration);					// Set Acceleration
	void SetDisplacement(const glm::vec2 v2Displacement);					// Set Displacement
	void SetTotalElapsedTime(const float fTotalElapsedTime);				// Set Total Elapsed Time
	void SetElapsedTime(const float fElapsedTime);							// Set Elapsed Time
	void SetVerticalStatus(	const VERTICALSTATUS sVerticalStatus, 
							const bool bInit = true);						// Set Vertical Status
	void SetHorizontalStatus(	const HORIZONTALSTATUS sHorizontalalStatus,
								const bool bInit = false);					// Set Horizontal Status
	void SetNewJump(const bool bNewJump);									// Set bNewJump

	// Get methods
	glm::vec2 GetInitialVelocity(void) const;			// Get Initial velocity
	glm::vec2 GetFinalVelocity(void) const;				// Get Final velocity
	glm::vec2 GetAcceleration(void) const;				// Get Acceleration
	glm::vec2 GetDeltaDisplacement(void) const;			// Get Delta Displacement
	glm::vec2 GetDisplacement(void) const;				// Get Displacement
	float GetTotalElapsedTime(void) const;				// Get Total Elapsed Time
	VERTICALSTATUS GetVerticalStatus(void) const;		// Get Vertical Status
	HORIZONTALSTATUS GetHorizontalStatus(void) const;	// Get Horizontal Status
	bool GetNewJump(void) const;						// Get bNewJump

	// Update
	bool Update(const double dElapsedTime);

	// Add elapsed time
	void AddElapsedTime(const float fElapsedTime);

	// Calculate the distance between two vec2 varables
	float CalculateDistance(glm::vec2 source, glm::vec2 destination);

	// PrintSelf
	void PrintSelf(void);

protected:
	// Variables for SUVAT calculations
	glm::vec2 v2InitialVelocity;	// Initial velocity
	glm::vec2 v2FinalVelocity;		// Final velocity
	glm::vec2 v2Acceleration;		// Acceleration
	glm::vec2 v2PrevDisplacement;	// Previous Displacement
	glm::vec2 v2Displacement;		// Displacement
	float fTotalTime;				// Total time since the motion was started
	float fElapsedTime;				// Elapsed time between the previous and current update

	// Gravity constant
	const glm::vec2 v2Gravity = glm::vec2(0.0f, -200.0f);

	// Current status
	VERTICALSTATUS sCurrentVerticalStatus;
	HORIZONTALSTATUS sCurrentHorizontalStatus;
	
	bool bNewJump;
};
