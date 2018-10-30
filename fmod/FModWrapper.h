#pragma once

// fmod
#include "fmod.h"
#include "fmod_errors.h"    /* optional */


class FModWrapper
{
public:
	FModWrapper(void);
	~FModWrapper(void);

	void Load(char* module);
	void Start();
	void Seek(int ms);
	void Stop();
	FMUSIC_MODULE *GetModule() { return m_mod; }

private:
	FMUSIC_MODULE *m_mod;
	FSOUND_SAMPLE *m_pSound;

	// This holds the sound channel that is returned from FSOUND_PlaySound()
	// Since we pick any free channel, we need to know which one the sound was assigned.
	int m_soundChannel;
};
