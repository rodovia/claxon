#include "SoundOutput.h"

#include <engine_tier0/Exceptions.h>
#include <assert.h>

#pragma region CSoundOutput

engine::CSoundOutput::CSoundOutput()
{
	m_Engine = irrklang::createIrrKlangDevice();
}

engine::CSoundOutput::~CSoundOutput()
{
	m_Engine->drop();
}

engine::CSound engine::CSoundOutput::PrecacheSound(std::wstring _Filename)
{
	char* buffer = new char[_Filename.length() * sizeof(wchar_t)];
	irrklang::makeUTF8fromUTF16string(_Filename.c_str(), buffer, _Filename.length() * sizeof(wchar_t));
	irrklang::ISoundSource* src = m_Engine->addSoundSourceFromFile(buffer);

	if (src == nullptr)
	{
		src = m_Engine->getSoundSource(buffer);
	}

	return engine::CSound(src, m_Engine);
}

#pragma endregion CSoundOutput

#pragma region CSound

engine::CSound::CSound(class CSoundOutput* _So, std::wstring _Filename)
{
	assert(_So != nullptr);
	CSound snd = _So->PrecacheSound(_Filename);
	m_Eng = snd.m_Eng;
	m_SoundSrc = snd.m_SoundSrc;

	m_Eng->grab();
	m_SoundSrc->grab();
}

engine::CSound::CSound(irrklang::ISoundSource* _Src, irrklang::ISoundEngine* _Eng)
	: m_SoundSrc(_Src),
	  m_Eng(_Eng)
{}

engine::CSound::~CSound()
{
	// m_SoundSrc->drop();
	// m_Eng->drop();
}

void engine::CSound::Play()
{
	if (m_Sound != nullptr)
	{
		m_Sound->drop();
		m_Sound = nullptr;
	}

	m_Sound = m_Eng->play2D(m_SoundSrc, false, false, true);
}

#pragma endregion CSound
