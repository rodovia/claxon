#pragma once

#include "DLL.h"

#include <string>
#include <vector>

#define _GETPATH gamepaths::CMountedPaths::GetInstance().GetPath

namespace gamepaths
{

class _GP_DLLEXP CMountedPaths
{
public:
	static CMountedPaths& GetInstance()
	{
		static CMountedPaths instance;
		return instance;
	}

	CMountedPaths(const CMountedPaths&) = delete;
	void operator=(CMountedPaths&) = delete;

	std::wstring GetPath(std::string _Path);
	void AddPath(std::string _Path);
private:
	std::vector<std::string> m_Inner;

	CMountedPaths() {}
};

}

