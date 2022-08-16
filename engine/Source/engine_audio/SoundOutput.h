#pragma once

#include <irrklang.h>
#include <engine_tier0/DLL.h>
#include <engine_tier0/ComPtr.h>
#include <string>

namespace engine
{

class _ENGINE_DLLEXP CSound
{
	friend class CSoundOutput;

public:
	/* Em condições normais, este constructor não há de ser chamado
	* noutros lugares exceto a CSoundOutput.
	*/
	CSound(class CSoundOutput* _So, std::wstring _Filename);
	CSound(CSound&&) = delete;
	~CSound();
	void Play();
	bool IsPlaying();
	void Join();

private:
	CSound(irrklang::ISoundSource* _Src, irrklang::ISoundEngine* _Eng);

	irrklang::ISoundSource* m_SoundSrc = nullptr;
	irrklang::ISound* m_Sound = nullptr;
	irrklang::ISoundEngine* m_Eng = nullptr;
};

 class _ENGINE_DLLEXP CSoundOutput
{
public:
	CSoundOutput();
	~CSoundOutput();
	CSound PrecacheSound(std::wstring _Filename);

private:
	irrklang::ISoundEngine* m_Engine;
};

}
