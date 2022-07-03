#pragma once

#include <map>
#include <string>
#include <vector>

#include "MountedPaths.h"
#include "DLL.h"

#include <libconfig.h++>

namespace gamepaths
{

class _GP_DLLEXP CInfoParser
{
public:
	CInfoParser(std::string _Data);
	static CInfoParser FromFile(std::string _Filename);

	std::string GetTitle() const noexcept;
private:
	CInfoParser(libconfig::Config& _Cfg);
	void SetupMountPaths(libconfig::Config& _Cfg);
private:
	static std::map<std::string, CInfoParser> m_Parsed;
private:
	std::string m_Title;
};

}
