/*
	-- DirectX Exception Handling
*/

#pragma once

#include <string>
#include <vector>
#include <dxgidebug.h>

#include <tier0lib/Win32.h>

#define _ENGINE_DXEXCH_BUFFER_SIZE 512

namespace engine
{

class CDXGIInfoManager
{
public:
	CDXGIInfoManager();
	~CDXGIInfoManager();
	CDXGIInfoManager(const CDXGIInfoManager&) = delete;
	CDXGIInfoManager& operator=(CDXGIInfoManager&) = delete;
	void Set() noexcept;
	std::vector<std::string> GetMessages() const;
private:
	unsigned long long m_Next = 0u;
	struct IDXGIInfoQueue* m_InfoQueue = nullptr;
};

}
