#include "DXGIInfoManager.h"
#include "Exceptions.h"
#include <tier0lib/dxerr.h>
#include <cstddef>
#include <memory>
#include <engine_tier1/GraphicalOutput.h>

typedef HRESULT(__stdcall* DXGIGetDebugInterface_t)(REFIID, void**);

engine::CDXGIInfoManager::CDXGIInfoManager()
{
	const HMODULE modj = LoadLibraryEx(L"dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (modj == nullptr)
	{
		throw _ENGINE_CREATE_FROMLASTERROR;
	}

	// one liners!!
	const DXGIGetDebugInterface_t DXGIGetDebugInterface = ((DXGIGetDebugInterface_t)
							(void*)(GetProcAddress(modj, "DXGIGetDebugInterface")));
	if (DXGIGetDebugInterface == nullptr)
	{
		throw _ENGINE_CREATE_FROMLASTERROR;
	}

	HRESULT hr;
	_ENGINE_MAYTHROW_GRAPHICS(DXGIGetDebugInterface(__uuidof(IDXGIInfoQueue), reinterpret_cast<void**>(&m_InfoQueue)));
}

engine::CDXGIInfoManager::~CDXGIInfoManager()
{
	if (m_InfoQueue != nullptr)
	{
		m_InfoQueue->Release();
	}
}

void engine::CDXGIInfoManager::Set() noexcept
{
	m_Next = m_InfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
}

std::vector<std::string> engine::CDXGIInfoManager::GetMessages() const
{
	std::vector<std::string> messages;
	const UINT64 end = m_InfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
	for (UINT64 i = m_Next; i < end; i++)
	{
		size_t messageLength;
		m_InfoQueue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &messageLength);
		std::unique_ptr<std::byte[]> bytes = std::make_unique<std::byte[]>(messageLength);
		DXGI_INFO_QUEUE_MESSAGE* message = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(bytes.get());
		m_InfoQueue->GetMessage(DXGI_DEBUG_ALL, i, message, &messageLength);

		messages.emplace_back(message->pDescription);
	}

	return messages;
}
