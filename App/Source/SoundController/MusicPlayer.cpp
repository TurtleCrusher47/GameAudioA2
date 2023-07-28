/**
 CMusicPlayer
 @brief A class which plays background music like a jukebox
 By: Toh Da Jun
 Date: May 2023
 */
#include "MusicPlayer.h"

#include <algorithm>    // std::shuffle

// For generating random numbers
#include <cstdlib>
#include <time.h>

// Include filesystem
#include "System\filesystem.h"

#include <iostream>
using namespace std;

// A class to pass to irrklang to receive isound stop events
class CSoundStopReceiver : public irrklang::ISoundStopEventReceiver
{
public:
	virtual void OnSoundStopped(irrklang::ISound* sound, irrklang::E_STOP_EVENT_CAUSE reason, void* userData)
	{
		// This is called when the sound has ended playing
		cout << "Sound has ended playback" << endl;

		CMusicPlayer::GetInstance()->SetMusicPlaybackFinished(true);

		switch (CMusicPlayer::GetInstance()->GetPlayMode())
		{
		case CMusicPlayer::PLAYMODE::SINGLE:
			CMusicPlayer::GetInstance()->SetStatus(CMusicPlayer::STATUS::STOP);
			break;
		// Can customise how the music player will behave for these modes after completing a music playback.
		//case CMusicPlayer::PLAYMODE::CONTINUOUS:
		//case CMusicPlayer::PLAYMODE::SHUFFLE:
		//	break;
		//case CMusicPlayer::PLAYMODE::SINGLE_LOOP:
		//case CMusicPlayer::PLAYMODE::CONTINUOUS_LOOP:
		//case CMusicPlayer::PLAYMODE::SHUFFLE_LOOP:
		//	break;
		default:
			break;
		}

		// Try to play another music
		if (CMusicPlayer::GetInstance()->GetStatus() == CMusicPlayer::STATUS::PLAY)
			CMusicPlayer::GetInstance()->PlayMusic();
	}
};

CSoundStopReceiver* cSoundStopReceiver = NULL;

/**
 @brief Constructor
 */
CMusicPlayer::CMusicPlayer(void)
	: cSoundEngine(NULL)
	, bMusicPlaybackFinished(true)
	, ePlayMode(PLAYMODE::SINGLE)
	, eStatus(STATUS::STOP)
	, currentISound(NULL)
	, iCurrentMusicVector(0)
{
	cSoundStopReceiver = new CSoundStopReceiver();

	// Reset the instance
	Reset();
}

/**
 @brief Destructor
 */
CMusicPlayer::~CMusicPlayer(void)
{
	// Clear the musicVector without deleting, since it will be deleted in musicMap
	musicVector.clear();

	// Iterate through the musicMap
	for (std::map<int, CSoundInfo*>::iterator it = musicMap.begin(); it != musicMap.end(); ++it)
	{
		// If the value/second was not deleted elsewhere, then delete it here
		if (it->second != NULL)
		{
			delete it->second;
			it->second = NULL;
		}
	}

	// Drop the MySoundEndReceiver instance
	if (cSoundStopReceiver != NULL)
	{
		delete cSoundStopReceiver;
		cSoundStopReceiver = NULL;
	}

	// Remove all elements in musicMap
	musicMap.clear();

	// Drop the sound engine
	if (cSoundEngine)
	{
		cSoundEngine->drop();
		cSoundEngine = NULL;
	}
}

/**
 @brief Initialise this class instance
 @return A bool value. true is this class instance was initialised, else false
 */
bool CMusicPlayer::Init(void)
{
	// Initialise the sound engine with default parameters
	cSoundEngine = createIrrKlangDevice(ESOD_WIN_MM, ESEO_MULTI_THREADED);
	if (cSoundEngine == NULL)
	{
		cout << "Unable to initialise the IrrKlang sound engine" << endl;
		return false;
	}

	return true;
}

/**
 @brief Add a music file
 @param filename A string variable storing the name of the file to read from
 @param ID A const int variable which will be the ID of the iSoundSource in the map
 @param bPreload A const bool variable which indicates if this iSoundSource will be pre-loaded into memory now.
 @return A bool value. True if the sound was loaded, else false.
 */
bool CMusicPlayer::AddMusic(	string filename,
								const int ID,
								const bool bPreload)
{
	if (FileSystem::DoesFileExists(filename) == false)
	{
		cout << "Unable to load sound " << filename.c_str() << endl;
		return false;
	}

	// Load the sound from the file
	ISoundSource* pSoundSource = cSoundEngine->addSoundSourceFromFile(filename.c_str(),
																	E_STREAM_MODE::ESM_AUTO_DETECT, 
																	bPreload);


	// Trivial Rejection : Invalid pointer provided
	if (pSoundSource == nullptr)
	{
		cout << "Unable to load sound " << filename.c_str() << endl;
		return false;
	}

	// Force the sound source not to have any streaming
	pSoundSource->setForcedStreamingThreshold(-1);

	// Clean up first if there is an existing Entity with the same name
	RemoveMusic(ID);

	// Add the entity now
	CSoundInfo* cSoundInfo = new CSoundInfo();
	cSoundInfo->Init(ID, pSoundSource);

	// Set to musicMap
	musicMap[ID] = cSoundInfo;

	Reset();

	return true;
}

/**
 @brief Remove a music from this map
 @param ID A const int variable which will be the ID of the iSoundSource in the map
 @return true if the sound was successfully removed, else false
 */
bool CMusicPlayer::RemoveMusic(const int ID)
{
	CSoundInfo* pSoundInfo = GetMusic(ID);
	if (pSoundInfo != nullptr)
	{
		delete pSoundInfo;
		musicMap.erase(ID);
		return true;
	}
	return false;
}

/**
@brief Get the number of music in this map
@return The number of sounds currently stored in the Map
*/
int CMusicPlayer::GetNumOfMusic(void) const
{
	return musicMap.size();
}

/**
 @brief Set the music playback status
 @param bMusicPlaybackFinished A const bool variable which indicates if the music playback has finished
 */
void CMusicPlayer::SetMusicPlaybackFinished(const bool bMusicPlaybackFinished)
{
	this->bMusicPlaybackFinished = bMusicPlaybackFinished;
}

/**
 @brief Play a sound by its ID
 @param ID A const int variable which will be the ID of the iSoundSource in the map
 */
void CMusicPlayer::PlayMusicByID(const int ID)
{
	// Set the play mode
	ePlayMode = PLAYMODE::SINGLE;

	CSoundInfo* pSoundInfo = GetMusic(ID);
	if (!pSoundInfo)
	{
		cout << "Sound #" << ID << " is not playable." << endl;
		return;
	}
	else if (cSoundEngine->isCurrentlyPlaying(pSoundInfo->GetSound()))
	{
		cout << "Sound #" << ID << " is currently being played." << endl;
		return;
	}

	cSoundEngine->play2D(pSoundInfo->GetSound(), pSoundInfo->GetLoopStatus());
}

/**
 @brief Play a music according to the current mode
 */
void CMusicPlayer::PlayMusic(void)
{
	// Set the play mode
	SetStatus(CMusicPlayer::STATUS::PLAY);

	cout << "PlayMusic: Mode = " << ePlayMode << endl;

	// Playback the music according to the playback mode
	switch (ePlayMode)
	{
	case SINGLE:
	case SINGLE_LOOP:
		{
			// Get the next music to play
			CSoundInfo* pSoundInfo = GetMusic();
			if (pSoundInfo == nullptr)
				return;
			else
			{
				if (ePlayMode == SINGLE)
				{
					currentISound = cSoundEngine->play2D(pSoundInfo->GetSound(), false, false, true);
					currentISound->setSoundStopEventReceiver(cSoundStopReceiver, 0);
				}
				else
				{
					currentISound = cSoundEngine->play2D(pSoundInfo->GetSound(), true, false, true);
					currentISound->setSoundStopEventReceiver(cSoundStopReceiver, 0);
				}
			}
		}
		break;
	case CONTINUOUS:
	case CONTINUOUS_LOOP:
		{
			if ((currentISound) && (currentISound->isFinished() == true))
				NextMusic();

			// Get the next music to play
			CSoundInfo* pSoundInfo = GetMusic();
			if (pSoundInfo == nullptr)
				return;
			else
			{
				currentISound = cSoundEngine->play2D(pSoundInfo->GetSound(), false, false, true);
				currentISound->setSoundStopEventReceiver(cSoundStopReceiver, 0);
				bMusicPlaybackFinished = false;
			}
		}
		break;
	case SHUFFLE:
	case SHUFFLE_LOOP:
		{
			if ((currentISound) && (currentISound->isFinished() == true))
				NextMusic();

			// If iCurrentMusicVector != -1, then we repeat the playback
			if (iCurrentMusicVector != -1)
			{
				// Update the mapCurrent
				mapCurrent = musicMap.find(musicVector[iCurrentMusicVector]);

				// Get the next music to play
				CSoundInfo* pSoundInfo = GetMusic();
				if (pSoundInfo == nullptr)
					return;
				else
				{
					currentISound = cSoundEngine->play2D(pSoundInfo->GetSound(), false, false, true);
					currentISound->setSoundStopEventReceiver(cSoundStopReceiver, 0);
					bMusicPlaybackFinished = false;
				}
			}
		}
		break;
	default:
		break;
	}
}

/**
 @brief Stop the music player
 */ 
void CMusicPlayer::StopPlayMusic(void)
{
	// If it is not playing any music, then return
	if (currentISound == NULL)
		return;

	// Update the status
	SetStatus(STOP);

	// If cSoundEngine is valid, then stop all sounds
	if (cSoundEngine)
		cSoundEngine->stopAllSounds();

	// Reset the key parameters
	currentISound = NULL;
}

/**
 @brief Reset the music player
 */
void CMusicPlayer::Reset(void)
{
	StopPlayMusic();

	// Reset the musicMap iterators and musicvector variable whenever we load a new music into musicMap
	if (!musicMap.empty())
	{
		// Reset the key parameters
		currentISound = NULL;
		mapStart = musicMap.begin();
		mapEnd = --musicMap.end();
		mapCurrent = mapStart;
		// Reset to start of playlist
		iCurrentMusicVector = 0;
	}
}

/**
 @brief Previous Music
 */ 
void CMusicPlayer::PreviousMusic(void)
{

}

/**
 @brief Next Music
 */ 
void CMusicPlayer::NextMusic(void)
{
	// If a music being played, then stop the playback
	if ((currentISound) && (currentISound->isFinished() == false))
	{
		// Stop the music
		StopPlayMusic();

		// Set back to play
		SetStatus(CMusicPlayer::STATUS::PLAY);
	}

	if ((ePlayMode >= SINGLE) && (ePlayMode <= CONTINUOUS_LOOP))
	{
		// Go to next song in the playlist
		mapCurrent++;

		// if the currentISound has completed playback, then we get the next music
		if (mapCurrent == musicMap.end())
		{
			// Reset to start of musicMap
			mapCurrent = mapStart;
			// Reset the currentISound so that the Music Player is not playing any music
			currentISound = NULL;
		}
	}
	else if (ePlayMode == SHUFFLE)
	{
		// Go to the next music
		iCurrentMusicVector++;
		if (iCurrentMusicVector >= musicVector.size())
		{
			// Reset to 0 so it won't play any music in musicMap
			iCurrentMusicVector = -1;
			// Reset to end of playlist
			mapCurrent = musicMap.end();
			// Reset the currentISound so that the Music Player is not playing any music
			currentISound = NULL;
		}
	}
	else if (ePlayMode == SHUFFLE_LOOP)
	{
		// Go to the next music
		iCurrentMusicVector++;
		if (iCurrentMusicVector >= musicVector.size())
		{
			// Reset to 0 since we start playing the music in SHUFFLE/SHUFFLE_LOOP mode
			iCurrentMusicVector = 0;
			// Reset to start of playlist
			mapCurrent = mapStart;
			// Reset the currentISound so that the Music Player is not playing any music
			currentISound = NULL;
		}
	}

	// If current status is Play, then play the music
	if (GetStatus() == CMusicPlayer::STATUS::PLAY)
	{
		PlayMusic();
	}
}

/**
 @brief Get the play mode
 */
CMusicPlayer::PLAYMODE CMusicPlayer::GetPlayMode(void)
{
	return ePlayMode;
}

/**
 @brief Set the play mode
 */
void CMusicPlayer::SetPlayMode(PLAYMODE newPlayMode)
{
	// Set the new mode
	ePlayMode = newPlayMode;
	if (ePlayMode >= PLAYMODE::NUM_PLAYMODE)
		ePlayMode = PLAYMODE::SINGLE;

	switch (ePlayMode)
	{
	case SINGLE:
		cout << "ePlayMode = SINGLE" << endl;
		break;
	case SINGLE_LOOP:
		cout << "ePlayMode = SINGLE_LOOP" << endl;
		break;
	case CONTINUOUS:
		cout << "ePlayMode = CONTINUOUS" << endl;
		break;
	case CONTINUOUS_LOOP:
		cout << "ePlayMode = CONTINUOUS_LOOP" << endl;
		break;
	case SHUFFLE:
		cout << "ePlayMode = SHUFFLE" << endl;
		break;
	case SHUFFLE_LOOP:
		cout << "ePlayMode = SHUFFLE_LOOP" << endl;
		break;
	default:
		cout << "ePlayMode = " << ePlayMode << " is unknown." << endl;
	}

	// Reset the music player
	Reset();

	// Create the musicVector ofr SHUFFLE and SHUFFLE_LOOP playback mode
	if ((ePlayMode == SHUFFLE) || (ePlayMode == SHUFFLE_LOOP))
	{
		// Clear the musicVector before we repopulate it
		if (musicVector.size() > 0)
			musicVector.clear();

		// Reset iCurrentMusicVector to the start of musicVector
		iCurrentMusicVector = 0;

		// Iterate through the musicMap
		for (std::map<int, CSoundInfo*>::iterator it = musicMap.begin(); it != musicMap.end(); ++it)
		{
			musicVector.push_back(it->first);
		}

		// Use current time as seed for random generator
		srand(time(NULL));
		// Use built-in random generator to shuffle the music
		std::random_shuffle(musicVector.begin(), musicVector.end());
	}
}

/**
 @brief Get the STATUS
 */
CMusicPlayer::STATUS CMusicPlayer::GetStatus(void)
{
	return eStatus;
}
/**
 @brief Set the STATUS
 */
void CMusicPlayer::SetStatus(CMusicPlayer::STATUS newStatus)
{
	eStatus = newStatus;
}

/**
 @brief Increase Master volume
 @return true if successfully increased volume, else false
 */
bool CMusicPlayer::MasterVolumeIncrease(void)
{
	// Get the current volume
	float fCurrentVolume = cSoundEngine->getSoundVolume() + 0.1f;
	// Check if the maximum volume has been reached
	if (fCurrentVolume > 1.0f)
		fCurrentVolume = 1.0f;

	// Update the Mastervolume
	cSoundEngine->setSoundVolume(fCurrentVolume);
	cout << "MasterVolumeIncrease: fCurrentVolume = " << fCurrentVolume << endl;

	return true;
}

/**
 @brief Decrease Master volume
 @return true if successfully decreased volume, else false
 */
bool CMusicPlayer::MasterVolumeDecrease(void)
{
	// Get the current volume
	float fCurrentVolume = cSoundEngine->getSoundVolume() - 0.1f;
	// Check if the minimum volume has been reached
	if (fCurrentVolume < 0.0f)
		fCurrentVolume = 0.0f;

	// Update the Mastervolume
	cSoundEngine->setSoundVolume(fCurrentVolume);
	cout << "MasterVolumeDecrease: fCurrentVolume = " << fCurrentVolume << endl;

	return true;
}

void CMusicPlayer::SetMasterVolume(float volume)
{
	cSoundEngine->setSoundVolume(glm::clamp(volume, 0.0f, 1.0f));
}


/**
 @brief Increase volume of a ISoundSource
 @param ID A const int variable which contains the ID of the iSoundSource in the map
 @return true if successfully decreased volume, else false
 */
bool CMusicPlayer::VolumeIncrease(const int ID)
{
	// Get the ISoundSource
	ISoundSource* pISoundSource = GetMusic(ID)->GetSound();
	if (pISoundSource == nullptr)
	{
		return false;
	}

	// Get the current volume
	float fCurrentVolume = pISoundSource->getDefaultVolume();

	// Check if the maximum volume has been reached
	if (fCurrentVolume >= 1.0f)
	{
		pISoundSource->setDefaultVolume(1.0f);
		return false;
	}

	// Increase the volume by 10%
	pISoundSource->setDefaultVolume(fCurrentVolume + 0.1f);

	return true;
}

/**
 @brief Decrease volume of a ISoundSource
 @param ID A const int variable which contains the ID of the iSoundSource in the map
 @return true if successfully decreased volume, else false
 */
bool CMusicPlayer::VolumeDecrease(const int ID)
{
	// Get the ISoundSource
	ISoundSource* pISoundSource = GetMusic(ID)->GetSound();
	if (pISoundSource == nullptr)
	{
		return false;
	}

	// Get the current volume
	float fCurrentVolume = pISoundSource->getDefaultVolume();

	// Check if the minimum volume has been reached
	if (fCurrentVolume <= 0.0f)
	{
		pISoundSource->setDefaultVolume(0.0f);
		return false;
	}

	// Decrease the volume by 10%
	pISoundSource->setDefaultVolume(fCurrentVolume - 0.1f);

	return true;
}

/**
 @brief Get a music from this map
 @return A CSoundInfo* variable
 */
CSoundInfo* CMusicPlayer::GetMusic(void)
{
	if (mapCurrent != musicMap.end())
	{
		cout << "Current Music : " << mapCurrent->first << endl;

		return (CSoundInfo*)(mapCurrent->second);
	}

	return nullptr;
}

/**
 @brief Get a music from this map
 @param ID A const int variable which will be the ID of the iSoundSource in the map
 @return A CSoundInfo* variable
 */
CSoundInfo* CMusicPlayer::GetMusic(const int ID)
{
	if (musicMap.count(ID) != 0)
		return musicMap[ID];

	return nullptr;
}
