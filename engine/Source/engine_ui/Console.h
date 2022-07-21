#pragma once

#include <engine_tier0/DLL.h>
#include <imguihlp.h>

#include <functional>
#include <vector>
#include <unordered_map>
#include <string>

// Size of console text input buffer
#define _ENGINE_CONBUFFERSZ 256

namespace engine
{

using CONCMDCALLBACK = std::function<void(std::vector<std::string>)>;

struct LogBufferMgr
{
	ImGuiTextBuffer Buffer;
	bool ScrollToBottom;

	void Clear();
	void AddLog(const char* _Fmt, ...);
	void Draw();
};

class CConVar;
class _ENGINE_DLLEXP CConCmd
{
	friend class CConsole;
public:
	CConCmd(std::string _Name, CONCMDCALLBACK _Callback, bool _FromCon = false);
	void Execute(std::vector<std::string> _Args) const;
private:
	CONCMDCALLBACK m_Routine;
};

class _ENGINE_DLLEXP CConsole
{
	friend CConVar;
	friend CConCmd;
public:
	static CConsole& Instance()
	{
		static CConsole con;
		return con;
	}

	CConVar* QueryConVar(std::string _Name) const;
	CConCmd* QueryConCmd(std::string _Name) const;

	void CallConCmd(std::string _Name, std::vector<std::string> _Args) const;
	void EmitMessage(const char* _Fmt, ...);
	static void SpawnWindow();
private:
	void ExecuteCommand(std::string_view _Cmd);
	std::unordered_map<std::string, std::shared_ptr<CConVar>> m_Vars;
	std::unordered_map<std::string, std::shared_ptr<CConCmd>> m_Cmds;
	LogBufferMgr m_LogMgr;

	static char s_WndBuffer[_ENGINE_CONBUFFERSZ];

	void AddConVar(std::string _Name, CConVar _Variable);
	void AddConCmd(std::string _Name, CConCmd _ConsoleCommand);
	CConsole()
	{
		std::memset(s_WndBuffer, 0, sizeof(s_WndBuffer));
	};
};

class _ENGINE_DLLEXP CConVar
{
public:
	CConVar(std::string _Name, std::string _Default = "");

	std::string GetString() noexcept;
	int64_t GetInteger();
	float GetFloat();
	bool GetBool();

	void SetValue(std::string _Value);
private:
	std::string m_Value;
};


}