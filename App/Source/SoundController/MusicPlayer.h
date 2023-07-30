/**
 CMusicPlayer
 @brief A class which plays background music like a jukebox
 By: Toh Da Jun
 Date: May 2023
 */
#pragma once

// Include SingletonTemplate
#include <DesignPatterns\SingletonTemplate.h>

#include <includes/glm.hpp>

// Include irrklang
#include <includes/irrKlang.h>
using namespace irrklang;
#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

// Include string
#include <string>
// Include map storage
#include <map>
// Include vector storage
#include <vector>
using namespace std;

// Include SoundInfo class; it stores the sound and other information
#include "SoundInfo.h"

class CMusicPlayer : public CSingletonTemplate<CMusicPlayer>
{
	friend CSingletonTemplate<CMusicPlayer>;
public:
	// The Play Mode
	enum PLAYMODE
	{
		SINGLE = 0,
		SINGLE_LOOP,
		CONTINUOUS,
		CONTINUOUS_LOOP,
		SHUFFLE,
		SHUFFLE_LOOP,
		NUM_PLAYMODE
	};

	// The Status
	enum STATUS
	{
		STOP = 0,
		PLAY,
		PAUSE,
		NUM_STATUS
	};

	// Initialise this class instance
	bool Init(void);

	// Add a music file
	bool AddMusic(	string filename, 
					const int ID,
					const bool bPreload = true);
	// Remove a music files  from this map
	bool RemoveMusic(const int ID);
	// Get the number of music files in this map
	int GetNumOfMusic(void) const;

	// Set the music playback status
	void SetMusicPlaybackFinished(const bool bMusicPlaybackFinished);

	// Play a music by its ID
	void PlayMusicByID(const int ID);
	// Play a music according to the current mode
	void PlayMusic(void);
	// Stop the music player
	void StopPlayMusic(void);
	// Reset the music player
	void Reset(void);

	// Previous Music
	void PreviousMusic(void);
	// Next Music
	void NextMusic(void);

	// Get the play mode
	CMusicPlayer::PLAYMODE GetPlayMode(void);
	// Set the play mode
	void SetPlayMode(CMusicPlayer::PLAYMODE newPlayMode);

	// Get the STATUS
	CMusicPlayer::STATUS GetStatus(void);
	// Set the STATUS
	void SetStatus(CMusicPlayer::STATUS newStatus);

	// Increase Master volume
	bool MasterVolumeIncrease(void);
	// Decrease Master volume
	bool MasterVolumeDecrease(void);

	float GetMasterVolume();
	void SetMasterVolume(float volume);

	// Increase volume of a ISoundSource
	bool VolumeIncrease(const int ID);
	// Decrease volume of a ISoundSource
	bool VolumeDecrease(const int ID);

protected:
	// Constructor
	CMusicPlayer(void);

	// Destructor
	virtual ~CMusicPlayer(void);

	// Get a music files  from this map
	CSoundInfo* GetMusic(void);
	// Get a music files  from this map
	CSoundInfo* GetMusic(const int ID);

	// The current mode of music playback
	PLAYMODE ePlayMode;

	// The current status of music playback
	STATUS eStatus;

	// The handler to the irrklang Sound Engine
	ISoundEngine* cSoundEngine;

	// bool flag to indicated if the current music has finished playing
	bool bMusicPlaybackFinished;

	// The map of all the musicMap created
	std::map<int, CSoundInfo*> musicMap;
	// iterators for navigating through musicMap
	std::map<int, CSoundInfo*>::iterator mapStart, mapEnd, mapCurrent;
	// Pointer to current irrklang iSound being played
	irrklang::ISound* currentISound;

	// The vector of all shuffled music created
	std::vector<int> musicVector;
	// Variable storing index of music in musicVector being played
	int iCurrentMusicVector;
};
