/**
 CFPSCounter
 @brief A class to calculate the Frames-Per-Second
 By: Toh Da Jun
 Date: Mar 2020
 */
#pragma once

// Include SingtonTemplate
#include "../DesignPatterns/SingletonTemplate.h"

#include "StopWatch.h"

#include <string>

class CFPSCounter : public CSingletonTemplate<CFPSCounter>
{
	friend CSingletonTemplate<CFPSCounter>;

public:
	// Destructor
	virtual ~CFPSCounter(void);

	// Initialise the class instance
	void Init(const bool bLimitFrameRate = false, const double dIdealDeltaTime = 0.0333333333333333, const double dWorstIdealDeltaTime = 0.05);

	// Start Timer
	void StartTimer(void);

	// Update the class instance
	void Update(void);

	// Stop Timer
	void StopTimer(void);

	// Get the current frame rate
	int GetFrameRate(void) const;

	// Get a string containing the current frame rate
	std::string GetFrameRateString(void) const;

	// Get the current frame time
	double GetFrameTime(void) const;

	// Get the current number of frames within this 1 second
	double GetNumFrames(void) const;

	// Get the current delta time
	double GetDeltaTime(void) const;

	// PrintSelf
	void PrintSelf(void) const;

protected:
	// Count the delta time since the last frame
	double dDeltaTime;
	// The ideal delta time
	double dIdealDeltaTime;
	// The Worst Ideal delta time
	double dWorstIdealDeltaTime;
	// Count the number of frames for the current second
	int nFrames;
	// The frame count for the last 1 second
	int iFrameRate;
	// Count the elapsed time since the last reset
	double dFrameTime;
	// The amount of time to delay until 1 second is up
	double dDelayTime;
	// Boolean variable to indicate if the framerate is to be limited.
	bool bLimitFrameRate;

	// Declare timer for calculating time per frame
	CStopWatch cStopWatch;

	// Constructor
	CFPSCounter(void);
};
