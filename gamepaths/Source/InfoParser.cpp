#include <tier0lib/Win32.h>
#include "InfoParser.h"
#include <fstream>
#include <filesystem>
#include <libconfig.h++>
#include <assert.h>

#define _FROMFILE_BUFFER_SIZE 2048

std::map<std::string, gamepaths::CInfoParser> gamepaths::CInfoParser::m_Parsed;

static bool _ReadFile(std::string _Filename, std::string* _Out)
{
	FILE* haF = std::fopen(_Filename.c_str(), "r");
	std::fseek(haF, 0, SEEK_END);
	long pos = std::ftell(haF);
	std::fseek(haF, 0, SEEK_SET);

	char* buffer = new char[pos];
	size_t cnt = std::fread((void*) buffer, pos, 1, haF);
	buffer[pos] = '\0';

	std::string mm = buffer;
	(*_Out) = mm;
	return true;
}

gamepaths::CInfoParser::CInfoParser(std::string _Data)
{
	libconfig::Config cfg;
	cfg.readString(_Data);
	const char* llp = cfg.lookup("Game.Title");

	m_Title = llp;
	this->SetupMountPaths(cfg);
}

gamepaths::CInfoParser::CInfoParser(libconfig::Config& _Cfg)
	: m_Title(_Cfg.lookup("Game.Title").operator std::string())
{
	this->SetupMountPaths(_Cfg);
}

gamepaths::CInfoParser gamepaths::CInfoParser::FromFile(std::string _Filename)
{
	for (auto it = m_Parsed.begin(); it != m_Parsed.end(); ++it)
	{
		if (it->first == _Filename)
		{
			return it->second;
		}
	}

	libconfig::Config cfg;
	cfg.readFile(_Filename);
	gamepaths::CInfoParser ff = gamepaths::CInfoParser(cfg);
	m_Parsed.insert(std::make_pair(_Filename, ff));

	return ff;
}

void gamepaths::CInfoParser::SetupMountPaths(libconfig::Config& _Cfg)
{
	libconfig::Setting& root = _Cfg.getRoot();
	const libconfig::Setting& searchpaths = root["Game"]["SearchPaths"];

	for (int i = 0; i < searchpaths.getLength(); ++i)
	{
		std::string obj = searchpaths[i];
		std::filesystem::path pat = std::filesystem::absolute(obj);
		CMountedPaths::GetInstance().AddPath(pat.generic_string());
	}
}

std::string gamepaths::CInfoParser::GetTitle() const noexcept
{
	return m_Title;
}


