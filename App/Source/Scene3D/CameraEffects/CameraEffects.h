/**
 CCameraEffects
 By: Toh Da Jun
 Date: Apr 2020
 */
#pragma once

// Include Entity3D
#include <Primitives/Entity2D.h>

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

class CCameraEffects : public CEntity2D
{
public:
	// Constructor
	CCameraEffects(void);
	// Destructor
	virtual ~CCameraEffects(void);

	// Initialise this class instance
	virtual bool Init(void);

	// Set Countdown Timer
	virtual bool SetTimer(const float fCountdownTimer = 2.0f);
	// Get Countdown Timer
	virtual float GetTimer(void) const;

	// Update this class instance
	virtual bool Update(const double dElapsedTime);

	// PreRender
	virtual void PreRender(void);
	// Render
	virtual void Render(void);
	// PostRender
	virtual void PostRender(void);

	// Print Self
	virtual void PrintSelf(void);

protected:
	// Countdown Timer
	float fCountdownTimer;
};
