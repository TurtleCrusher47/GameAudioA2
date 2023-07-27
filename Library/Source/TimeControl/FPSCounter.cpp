/**
 CFPSCounter
 @brief A class to calculate the Frames-Per-Second
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "FPSCounter.h"
#include <iostream>
using namespace std;

CFPSCounter::CFPSCounter()
	: dDeltaTime(0.0)
	, dIdealDeltaTime(0.0333333333333333)
	, dWorstIdealDeltaTime(0.05)
	, nFrames(0)
	, iFrameRate(0)
	, dFrameTime(0.0)
	, dDelayTime(0.0)
{
	Init();
}


CFPSCounter::~CFPSCounter()
{
}


// Initialise the class instance
void CFPSCounter::Init(const bool bLimitFrameRate, const double dIdealDeltaTime, const double dWorstIdealDeltaTime)
{
	dDeltaTime = 0.0;
	nFrames = 0;
	iFrameRate = 0;
	this->dIdealDeltaTime = dIdealDeltaTime;
	this->dWorstIdealDeltaTime = dWorstIdealDeltaTime;
	dFrameTime = 1.0;
	this->bLimitFrameRate = bLimitFrameRate;
}

// Start Timer
void CFPSCounter::StartTimer(void)
{
	// Start timer to calculate how long it takes to render this frame
	cStopWatch.StartTimer();
}

// Stop Timer
void CFPSCounter::StopTimer(void)
{

}

// Update the class instance
void CFPSCounter::Update(void)
{
	// Calculate the delta time since the last frame
	dDeltaTime = cStopWatch.GetDeltaTime();

	// This is to prevent the program from crashing due to long dDeltaTime
	// Causing Physics to calculate a large jump/fall for the player
	if (dDeltaTime < dIdealDeltaTime)
	{
		dDeltaTime = dIdealDeltaTime;
	}
	if (dDeltaTime > dWorstIdealDeltaTime)
	{
		dDeltaTime = dWorstIdealDeltaTime;
	}
	

	if (bLimitFrameRate)
	{
		// Frame rate limiter. Limits each frame to a specified time in ms by sleeping.  
 		dDelayTime = dIdealDeltaTime - dDeltaTime * 1000;
		if (dDelayTime < 0.0)
			dDelayTime = 0.0;

		// Wait until the delay time is up
		cStopWatch.WaitUntil((const long long)dDelayTime);

		// Update the FPS Counter
		dFrameTime += this->dDeltaTime + dDelayTime * 0.001;
	}
	else
	{
		// Update the FPS Counter
		dFrameTime += this->dDeltaTime;
	}

	// Update the frame count
	nFrames++;
	 
 	if (dFrameTime >= 1.0){ // If last update was more than 1 sec ago...
		if (nFrames > 100)
		{
			PrintSelf();
		}

		// Calculate the current frame rate
		dFrameTime = 1000.0 / double(nFrames);

		// Update the frame count for the last 1 second
		iFrameRate = nFrames;

		// Reset timer and update the lastTime
		nFrames = 0;
		dFrameTime = 0.0;
	}
}

// Get the current frame rate
int CFPSCounter::GetFrameRate(void) const
{
	return iFrameRate;
}

// Get a string containing the current frame rate
std::string CFPSCounter::GetFrameRateString(void) const
{
	return to_string(iFrameRate);
}

// Get the current frame time
double CFPSCounter::GetFrameTime(void) const
{
	return dFrameTime;
}

// Get the current number of frames within this 1 second
double CFPSCounter::GetNumFrames(void) const
{
	return nFrames;
}

// Get the current delta time
double CFPSCounter::GetDeltaTime(void) const
{
	return dDeltaTime;
}

// PrintSelf
void CFPSCounter::PrintSelf(void) const
{
	cout << "CFPSCounter::PrintSelf()" << endl;
	cout << "========================" << endl;
	cout << "dDeltaTime\t\t:\t" << dDeltaTime << endl;
	cout << "dIdealDeltaTime\t:\t" << dIdealDeltaTime << endl;
	cout << "dWorstIdealDeltaTime\t:\t" << dWorstIdealDeltaTime << endl;
	cout << "nFrames\t\t:\t" << nFrames << endl;
	cout << "iFrameRate\t\t:\t" << iFrameRate << endl;
	cout << "dFrameTime\t\t:\t" << dFrameTime << endl;
	cout << "dDelayTime\t\t:\t" << dDelayTime << endl;
	cout << "bLimitFrameRate\t:\t" << bLimitFrameRate << endl;
	cout << "========================" << endl;
}
