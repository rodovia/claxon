#pragma once

#include <xaudio2.h>
#include <engine_tier0/DLL.h>
#include <engine_tier0/ComPtr.h>
#include <string>

namespace engine
{

_ENGINE_DLLEXP class CSound
{
public:
	/* Em condições normais, este constructor não há de ser chamado
	* noutros lugares exceto a CSoundOutput.
	*/
	CSound(CUtl_ComPtr<IXAudio2> _Audio, WAVEFORMATEX _Wave, XAUDIO2_BUFFER buffer);
	CSound(CSound&&) = delete;
	void Play();

private:
	CUtl_ComPtr<IXAudio2> m_Audio;
	WAVEFORMATEX m_Wave;
	XAUDIO2_BUFFER m_Buffer;
};

_ENGINE_DLLEXP class CSoundOutput
{
public:
	CSoundOutput();
	CSound PrecacheSound(std::wstring _Filename);
	
private:
	CUtl_ComPtr<IXAudio2> m_Audio;
	// m_Mvoice is not inside a CUtl_ComPtr because it does
	// not inherit from IUnknown
	IXAudio2MasteringVoice* m_Mvoice = nullptr;
};

}
