#include "MountedPaths.h"
#include <tier0lib/String0.h>
#include <filesystem>
#include <tier0lib/Win32.h>

std::wstring gamepaths::CMountedPaths::GetPath(std::string _Path)
{
	for (const std::string& b : m_Inner)
	{
		std::wstring path = tier0::ConvertToWideString(b.c_str());
		std::wstring more = tier0::ConvertToWideString(_Path);

		std::filesystem::path pa = path;
		pa.append(more);
		if (std::filesystem::exists(pa))
		{
			return pa.native();
		}
	}

	return tier0::ConvertToWideString(_Path);
}

void gamepaths::CMountedPaths::AddPath(std::string _Path)
{
	std::vector<std::string>::iterator iter = 
		std::find(m_Inner.begin(), m_Inner.end(), _Path);
	if (iter == m_Inner.end())
	{
		m_Inner.push_back(_Path);
	}
}
