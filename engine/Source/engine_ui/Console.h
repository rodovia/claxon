#pragma once

#include <engine_tier0/DLL.h>
#include <imguihlp.h>

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

// Size of console text input buffer
#define _ENGINE_CONBUFFERSZ 256
#define Msg engine::CConsole::Instance().EmitMessage

namespace engine
{

typedef void (*CONCMDCALLBACK)(std::vector<std::string>);

struct LogBufferMgr
{
	ImGuiTextBuffer Buffer;
	bool ScrollToBottom;

	void Clear();
	template <typename... _Ty>
	void AddLog(const char* _Fmt, _Ty&&... _Extra)
	{
		Buffer.appendf(_Fmt, _Extra...);
		Buffer.append("\n");

		ScrollToBottom = true;
	};
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

	std::unordered_map<std::string, CConVar*> GetVars() const noexcept;
	std::unordered_map<std::string, CConCmd*> GetCmds() const noexcept;

	CConVar* QueryConVar(std::string _Name) const;
	CConCmd* QueryConCmd(std::string _Name) const;

	void CallConCmd(std::string _Name, std::vector<std::string> _Args) const;
	template<class... _Ty_Args>
	void EmitMessage(const char* _Fmt, _Ty_Args&&... _Args)
	{
		m_LogMgr->AddLog(_Fmt, _Args...);
	};
	static void SpawnWindow();
private:
	void ExecuteCommand(std::string _Cmd);
	std::unordered_map<std::string, CConVar*> m_Vars;
	std::unordered_map<std::string, CConCmd*> m_Cmds;
	LogBufferMgr* m_LogMgr;

	static char s_WndBuffer[_ENGINE_CONBUFFERSZ];

	void AddConVar(std::string _Name, CConVar* _Variable);
	void AddConCmd(std::string _Name, CConCmd* _ConsoleCommand);
	CConsole();
	~CConsole();
};

class _ENGINE_DLLEXP CConVar
{
public:
	CConVar(std::string _Name, std::string _Default = "", bool _FromCon = false);

	std::string GetString() noexcept;
	int64_t GetInteger();
	float GetFloat();
	bool GetBool();

	void SetValue(std::string _Value);
private:
	std::string m_Value;
};

struct ConInput
{
	union
	{
		CConVar ConVar;
		CConCmd ConCmd;
	};

	bool IsVar;
};

class IConInputCompletion
{
public:
	virtual void CommandComplete(std::pair<std::string, ConInput>) = 0;
};

}
