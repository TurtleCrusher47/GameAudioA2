/**
 CStopWatch
 @brief A class to calculate time duration
 By: Toh Da Jun
 Date: Mar 2020
 */
#pragma once

#include <windows.h>

class CStopWatch
{
public:
	// Constructor
	CStopWatch(void);

	// Destructor
	~CStopWatch(void);
	
	// Initialise this class instance
	void Init(void);

	// Start Timer
	void StartTimer(void);

	// Stop Timer
	void StopTimer(void);

	// Get delta time in seconds since the last call to this function
	double GetDeltaTime(void);

	// Wait until this time in milliseconds has passed
	void WaitUntil(const long long llTime);

protected:
	LARGE_INTEGER frequency;
	LARGE_INTEGER prevTime, currTime;
	UINT     wTimerRes;

	// Convert from Large Integer to seconds in double
	double LIToSecs(LARGE_INTEGER & L);
};
