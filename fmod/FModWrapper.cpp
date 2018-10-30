#include ".\fmodwrapper.h"

FModWrapper::FModWrapper(void)
{
}

FModWrapper::~FModWrapper(void)
{
}

void FModWrapper::Load(char* module)
{
    //if (!FSOUND_Init(44000, 64, 0)) return;
    //if (!(m_mod = FMUSIC_LoadSong(module))) return;

	if (!FSOUND_Init(44100, 32, FSOUND_INIT_GLOBALFOCUS)) return;

	if (!(m_pSound = FSOUND_Sample_Load(FSOUND_FREE, module, 0, 0, 0))) return;
}

void FModWrapper::Start()
{
	//FMUSIC_PlaySong(m_mod);
	m_soundChannel = FSOUND_PlaySound(FSOUND_FREE, m_pSound);
}
void FModWrapper::Seek(int ms)
{
//	FSOUND_Stream_SetTime(m_pSound,ms);
}

void FModWrapper::Stop()
{
	FSOUND_StopSound(m_soundChannel);
    //FMUSIC_FreeSong(m_mod);
    FSOUND_Close();
}