#pragma once

#include <engine_tier0/DLL.h>
#include <imguihlp.h>

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

// Size of console text input buffer
#define _ENGINE_CONBUFFERSZ 256
#define Log engine::CConsole::Instance().EmitMessage

#define DevLog(P, ...) engine::CConsole::Instance().EmitMessageEx(_CONSOLEAPI_DEVELOPER, P, __VA_ARGS__) 

#define _CONSOLEAPI_NONEWLINE 0x1000000LL
#define _CONSOLEAPI_DEVELOPER 0x0800000LL

namespace engine
{

typedef void (*CONCMDCALLBACK)(std::vector<std::string>);
typedef void (__stdcall *PFN_CONVARCOMPLETIONCALLBACK)(class CConVar*);

struct LogBufferMgr
{
	ImGuiTextBuffer Buffer;
	bool ScrollToBottom;

	void Clear();
	template <typename... _Ty>
	void AddLog(long long _Flags, const char* _Fmt, _Ty&&... _Extra) const
	{
		ImGuiTextBuffer* bf = const_cast<ImGuiTextBuffer*>(&Buffer);
		bool* scrb = const_cast<bool*>(&ScrollToBottom);
		bf->appendf(_Fmt, _Extra...);
		if (!(_Flags & _CONSOLEAPI_NONEWLINE))
		{
			bf->append("\n");
		}
		
		(*scrb) = true;
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
	void InterpretFile(std::wstring _Filename);
	void CallConCmd(std::string _Name, std::vector<std::string> _Args) const;

	template<class... _Ty_Args>
	void EmitMessage(const char* _Fmt, _Ty_Args&&... _Args) const
	{
		m_LogMgr->AddLog(0LL, _Fmt, _Args...);
	};

	template<class... _Ty_Args>
	void EmitMessageEx(long long _Flags, const char* _Fmt, _Ty_Args&&... _Args) const
	{
		if (_Flags == 0)
		{
			this->EmitMessage(_Fmt, _Args...);
			return;
		}

		if (_Flags & _CONSOLEAPI_DEVELOPER
			&& this->QueryDeveloperLevel() > 0)
		{
			m_LogMgr->AddLog(_Flags & ~(_CONSOLEAPI_DEVELOPER), _Fmt, _Args...);
		}
	}

	static void SpawnWindow();

private:
	uint16_t QueryDeveloperLevel() const noexcept;
	void ExecuteCommand(std::string _Cmd);
	std::unordered_map<std::string, CConVar*> m_Vars;
	std::unordered_map<std::string, CConCmd*> m_Cmds;
	std::string m_CmdFormation;
	bool m_IsOnMore;
	LogBufferMgr* m_LogMgr;

	static char s_WndBuffer[_ENGINE_CONBUFFERSZ];

	void AddConVar(std::string _Name, CConVar* _Variable);
	void AddConCmd(std::string _Name, CConCmd* _ConsoleCommand);
	CConsole();
	~CConsole();
};

class _ENGINE_DLLEXP CConVar
{
	friend CConsole;

public:
	CConVar(std::string _Name, std::string _Default = "");
	CConVar(const char* _Name, const char* _Default = "");
	CConVar(std::string _Name, std::string _Default = "", 
		std::string _Help = "", PFN_CONVARCOMPLETIONCALLBACK _Compl_Callback = nullptr);

	std::string GetString() const noexcept;
	int64_t GetInteger() const;
	float GetFloat() const;
	bool GetBool() const;
	
	inline operator int64_t() const
	{
		return this->GetInteger();
	}

	inline operator float() const
	{
		return this->GetFloat();
	}

	inline operator bool() const
	{
		return this->GetBool();
	}

	inline operator std::string() const
	{
		return this->GetString();
	}

	std::string GetHelp() const noexcept;

	void SetValue(std::string _Value);
private:
	std::string m_Value;
	std::string m_Help;
	PFN_CONVARCOMPLETIONCALLBACK m_Compl_Callback = nullptr;
};

}
