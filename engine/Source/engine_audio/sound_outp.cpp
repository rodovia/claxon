#include "SoundOutput.h"

#include <engine_tier0/Exceptions.h>

#define RIFF_SIGNATURE 'FFIR'
#define RIFF_DATA_SIGNATURE 'atad'
#define RIFF_FMT_SIGNATURE 'tmf'
#define RIFF_WAVE_SIGNATURE 'EVAW'
#define RIFF_XWMA_SIGNATURE 'AMWX'
#define RIFF_DPDS_SIGNATURE 'sdpd'

// For simplicity sake, CSoundOutput assumes COM is
// already initialized

static HRESULT FindChunk(HANDLE _File, DWORD _Fourcc, DWORD& _ChunkSize, DWORD& _ChunkDataPos)
{
	HRESULT hr = S_OK;
	if (SetFilePointer(_File, 0, nullptr, FILE_BEGIN))
	{
		return HRESULT_FROM_WIN32(GetLastError());
	}

	DWORD chunkType,
		chunkDataSize,
		riffDataSize = 0,
		fileType,
		bytesRead = 0,
		offset = 0;

	while (hr == S_OK)
	{
		DWORD read;
		if (ReadFile(_File, &chunkType, sizeof(DWORD), &read, nullptr) == 0)
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		if (ReadFile(_File, &chunkDataSize, sizeof(DWORD), &read, nullptr) == 0)
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		switch (chunkType)
		{
		case RIFF_SIGNATURE:
			riffDataSize = chunkDataSize;
			chunkDataSize = 4;
			if (ReadFile(_File, &fileType, sizeof(DWORD), &read, nullptr) == 0)
			{
				hr = HRESULT_FROM_WIN32(GetLastError());
			}
			break;
		default:
			if (SetFilePointer(_File, chunkDataSize, nullptr, FILE_CURRENT))
			{
				return HRESULT_FROM_WIN32(GetLastError());
			}
			break;
		}

		offset += sizeof(DWORD) * 2;
		if (chunkType == _Fourcc)
		{
			_ChunkSize = chunkDataSize;
			_ChunkDataPos = offset;
			return S_OK;
		}

		offset += chunkDataSize;
		if (bytesRead >= riffDataSize)
		{
			return S_FALSE;
		}
	}

	return S_OK;
}

static HRESULT ReadChunkData(HANDLE _File, void* _Buffer, DWORD _BufferSize, DWORD _BufferOffset)
{
	if (SetFilePointer(_File, _BufferOffset, nullptr, FILE_BEGIN))
	{
		return HRESULT_FROM_WIN32(GetLastError());
	}

	DWORD read;
	if (ReadFile(_File, _Buffer, _BufferSize, &read, nullptr) == 0)
	{
		return HRESULT_FROM_WIN32(GetLastError());
	}

	return S_OK;
}

#pragma region CSoundOutput

engine::CSoundOutput::CSoundOutput()
{
	HRESULT hr = XAudio2Create(&m_Audio);
	FastFailCheck(hr);

	m_Audio->CreateMasteringVoice(&m_Mvoice);
}

engine::CSound engine::CSoundOutput::PrecacheSound(std::wstring _Filename)
{
	WAVEFORMATEXTENSIBLE wav = {0};
	XAUDIO2_BUFFER buffer = {0};
	DWORD chunkSize;
	DWORD chunkPos;
	DWORD flt;

	HANDLE file = CreateFile(
		_Filename.c_str(),
		GENERIC_READ,
		FILE_SHARE_READ,
		nullptr,
		OPEN_EXISTING,
		0,
		nullptr);

	FindChunk(file, RIFF_SIGNATURE, chunkSize, chunkPos);
	ReadChunkData(file, &flt, sizeof(DWORD), chunkPos);

	FindChunk(file, RIFF_FMT_SIGNATURE, chunkSize, chunkPos);
	ReadChunkData(file, &wav, chunkSize, chunkPos);

	FindChunk(file, RIFF_DATA_SIGNATURE, chunkSize, chunkPos);
	unsigned char* cbuffer = new unsigned char[chunkSize];
	ReadChunkData(file, cbuffer, chunkSize, chunkPos);
	buffer.AudioBytes = chunkSize;
	buffer.pAudioData = cbuffer;
	buffer.Flags = XAUDIO2_END_OF_STREAM;
}

#pragma endregion CSoundOutput

#pragma region CSound

engine::CSound::CSound(CUtl_ComPtr<IXAudio2> _Audio, WAVEFORMATEX _Wave, XAUDIO2_BUFFER _Buffer)
	: m_Audio(_Audio),
	  m_Wave(_Wave),
	  m_Buffer(std::move(_Buffer))
{
}

void engine::CSound::Play()
{
	IXAudio2SourceVoice* voice;
	HRESULT hr = m_Audio->CreateSourceVoice(&voice, &m_Wave);
	FastFailCheck(hr);
	FastFailCheck(voice->SubmitSourceBuffer(&m_Buffer));

	voice->Start();
}

#pragma endregion CSound
