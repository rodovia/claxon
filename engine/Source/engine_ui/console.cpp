#include "Console.h"
#include <imguihlp.h>
#include <tier0lib/String0.h>
#include <MountedPaths.h>
#include <algorithm>
#include <tier0lib/Win32.h>
#include <engine_tier0/Exceptions.h>

#ifndef ZeroMemory
#define ZeroMemory(_E) memset(_E, 0, sizeof(_E))
#endif

/* 
*  A little explanation on why we need such console instance param instead of using the singleton:
*     Log macro will call CConsole::Instance, which, for some reason, will create a CConsole instance.
*  Since the below ReadFile routine is called from inside the CConsole ctor, the CRT will deadlock.
*  (Both ctors will wait on each other. The deadlock is created)
*/

static bool ReadFile(engine::CConsole& _Cn, std::wstring _Filename, std::string& _Cnt)
{
	char* errorBuffer = new char[512];
	char* cntBuffer = nullptr;
	DWORD dwError;
	DWORD dwFileSize;
	HANDLE hwd = CreateFile(
		_Filename.c_str(),
		GENERIC_READ, FILE_SHARE_READ,
		nullptr, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, nullptr);

	if (hwd == INVALID_HANDLE_VALUE)
	{
		dwError = GetLastError();
		engine::ExcFormatMessage(HRESULT_FROM_WIN32(dwError), engine::EFM_Type::WIN32_EXCEPTION, &errorBuffer);
		_Cn.EmitMessage("CConsole::InterpretFile: Cannot open file %s! CreateFile failed. %s (%ll)",
			tier0::ConvertToMultiByteString(_Filename), errorBuffer, dwError);
		return false;
	}

	LARGE_INTEGER li;
	GetFileSizeEx(hwd, &li);
	cntBuffer = new char[li.QuadPart];
	ReadFile(hwd, &cntBuffer, li.QuadPart, &dwFileSize, nullptr);

	_Cnt = cntBuffer;

	delete[] cntBuffer;
	delete[] errorBuffer;
	return true;
}

static void Help([[maybe_unused]] std::vector<std::string>)
{
	engine::CConsole& con = engine::CConsole::Instance();
	con.EmitMessage("Cmds:");
	for (auto cmd : con.GetCmds())
	{
		con.EmitMessage("%s", cmd.first.c_str());
	}

	con.EmitMessage(""); // empty newline

	con.EmitMessage("Vars:");
	for (auto cmd : con.GetVars())
	{
		con.EmitMessage("%s = %s", cmd.first.c_str(), cmd.second->GetString().c_str());
	}
}

static void Echo(std::vector<std::string> _Args)
{
	 for (auto arg = _Args.begin(); arg != _Args.end(); ++arg)
	{
		int flg;
		if (arg++ != _Args.end())
		{
			flg |= _CONSOLEAPI_NONEWLINE;
		}
		engine::CConsole::Instance().EmitMessageEx(flg, arg->c_str());
	}
}

engine::CConCmd help("help", Help);
engine::CConCmd echo("echo", Echo);
engine::CConVar developer("developer", "0");

char engine::CConsole::s_WndBuffer[_ENGINE_CONBUFFERSZ];

#pragma region LogBufferMgr

void engine::LogBufferMgr::Clear()
{
	Buffer.clear();
}

// Draw assumes it is being called inside a ImGui::Begin block.
void engine::LogBufferMgr::Draw()
{
	if (ImGui::Button("Limpar"))
	{
		this->Clear();
	}

	ImGui::SameLine();
	bool cpy = ImGui::Button("Copiar");
	ImGui::Separator();

	ImGui::BeginChild("scrolling");
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 1));
	if (cpy)
	{
		ImGui::LogToClipboard();
	}

	ImGui::TextUnformatted(Buffer.begin());
	ScrollToBottom = false;
	ImGui::PopStyleVar();
	ImGui::EndChild();
}

#pragma endregion LogBufferMgr

#pragma region ConVar

engine::CConVar::CConVar(std::string _Name, std::string _Default)
	: m_Value(_Default)
{
	CConsole::Instance().AddConVar(_Name, this);
}

engine::CConVar::CConVar(const char* _Name, const char* _Default)
	: m_Value(_Default)
{
	CConsole::Instance().AddConVar(_Name, this);
}

engine::CConVar::CConVar(std::string _Name, std::string _Default,
						 std::string _Help, PFN_CONVARCOMPLETIONCALLBACK _Compl_Callback)
	: m_Value(_Default),
	m_Help(_Help)
{
	m_Compl_Callback = _Compl_Callback;
	CConsole::Instance().AddConVar(_Name, this);
}

std::string engine::CConVar::GetString() const noexcept
{
	return m_Value;
}

int64_t engine::CConVar::GetInteger() const
{
	return std::stoll(m_Value);
}

float engine::CConVar::GetFloat() const
{
	return std::stof(m_Value);
}

bool engine::CConVar::GetBool() const
{
	return (m_Value == "1" || m_Value == "y" ? true : false);
}

void engine::CConVar::SetValue(std::string _Value)
{
	m_Value = _Value;
}

std::string engine::CConVar::GetHelp() const noexcept
{
	return m_Help;
}

#pragma endregion ConVar

#pragma region Console

engine::CConsole::CConsole()
{
	std::memset(s_WndBuffer, 0, sizeof(s_WndBuffer));
	m_LogMgr = new LogBufferMgr;
	// this->InterpretFile(_GetPath("cfg/autoexec.cfg"));
}

engine::CConsole::~CConsole()
{
	/* "Why shouldn't m_Vars and m_Cmds be freed right here?" One may ask.
	 *  Well, engine::CConVar and engine::CConCmd are meant to be created as global
	 *  variables, therefore, the lifetime of these pointers are static. They are
	 *  managed by the CRT. And these ptrs are retrieved from the class' *this* ptr.
	 *
	 *  (See: CConVar and CConCmd ctors)
	 */

	delete m_LogMgr;
}

void engine::CConsole::AddConVar(std::string _Name, CConVar* _Variable)
{
	if (_Variable == nullptr)
	{
		this->EmitMessage("Cannot register convar '%s': _Variable is nullptr.", _Name.c_str());
		return;
	}

	if (this->QueryConVar(_Name) != nullptr)
	{
		this->EmitMessage("Cannot register ConVar with name '%s' because it is already reserved", _Name.c_str());
		return;
	}

	m_Vars.emplace(_Name, _Variable);
}

void engine::CConsole::AddConCmd(std::string _Name, CConCmd* _Variable)
{
	if (_Variable == nullptr)
	{
		this->EmitMessage("Cannot register concmd '%s': _Variable is nullptr.", _Name.c_str());
		return;
	}

	if (this->QueryConCmd(_Name) != nullptr)
	{
		this->EmitMessage("Cannot register ConCmd with name '%s' because it is already reserved!", _Name.c_str());
		return;
	}

	m_Cmds.insert(
		std::make_pair(_Name, _Variable));
}

engine::CConVar* engine::CConsole::QueryConVar(std::string _Name) const
{
	auto c = m_Vars.find(_Name);
	if (c != m_Vars.end())
	{
		return c->second;
	}
	return nullptr;
}

engine::CConCmd* engine::CConsole::QueryConCmd(std::string _Name) const
{
	auto c = m_Cmds.find(_Name);
	if (c != m_Cmds.end())
	{
		return c->second;
	}
	return nullptr;
}

void engine::CConsole::CallConCmd(std::string _Name, std::vector<std::string> _Args) const
{
	CConCmd* cmd;
	if ((cmd = this->QueryConCmd(_Name)) == nullptr)
	{
		this->EmitMessage("Cannot call concmd '%s': Is nullptr", _Name.c_str());
		return;
	}
	cmd->Execute(_Args);
}

void engine::CConsole::SpawnWindow()
{
	if (ImGui::Begin("Consola"))
	{
		CConsole& con = CConsole::Instance();

		int flags = ImGuiInputTextFlags_EnterReturnsTrue;
		if (ImGui::InputText("Entrada", s_WndBuffer, _ENGINE_CONBUFFERSZ,
							 flags))
		{
			std::string s = std::string(s_WndBuffer);
			size_t mmo = s.find_last_of('^');
			if (mmo != std::string::npos)
			{
				con.m_IsOnMore = true;
				con.EmitMessage("Mais?");
				con.m_CmdFormation += s_WndBuffer;

				memset(s_WndBuffer, 0, sizeof(s_WndBuffer));
				ImGui::End();
				return;
			}

			// It has been verified the input does not end with "^".
			if (con.m_IsOnMore)
			{
				using namespace std::string_literals;
				con.m_CmdFormation += s_WndBuffer;
				con.m_IsOnMore = false;
				s = con.m_CmdFormation;

				con.m_CmdFormation.clear();
				s = tier0::ReplaceStr(s, '^', ' ');
			}

			std::vector<std::string> cmds = tier0::SplitStr(s, ';');
			memset(s_WndBuffer, 0, sizeof(s_WndBuffer));

			for (const std::string& cmd : cmds)
			{
				con.ExecuteCommand(cmd);
			}
		}

		con.m_LogMgr->Draw();
	}

	ImGui::End();
}

void engine::CConsole::ExecuteCommand(std::string _Cmd)
{
	this->EmitMessage("] %s", _Cmd.c_str());
	std::string cmdcpy = std::string(_Cmd);

	std::vector<std::string> args = tier0::SplitStr(cmdcpy, ' ');
	std::string name = args[0];

	CConCmd* cmd = this->QueryConCmd(name);
	CConVar* var = this->QueryConVar(name);

	if (cmd == nullptr && var == nullptr)
	{
		this->EmitMessage("Bad command or cvar: '%s'", name.c_str());
		return;
	}

	if (cmd != nullptr)
	{
		cmd->Execute(args);
		return;
	}

	// It's a convar
	__assume(args.size() > 0);
	if (args.size() > 1)
	{
		var->SetValue(args[1]);
		if (var->m_Compl_Callback != nullptr)
		{
			var->m_Compl_Callback(var);
		}
		return;
	}

	// args.size() == 1
	this->EmitMessage("%s = %s", args[0].c_str(), var->GetString().c_str());
}

std::unordered_map<std::string, engine::CConCmd*> engine::CConsole::GetCmds() const noexcept
{
	return m_Cmds;
}

uint16_t engine::CConsole::QueryDeveloperLevel() const noexcept
{
	return developer.GetInteger();
}

std::unordered_map<std::string, engine::CConVar*> engine::CConsole::GetVars() const noexcept
{
	return m_Vars;
}

void engine::CConsole::InterpretFile(std::wstring _Filename)
{
	std::string nwd;
	ReadFile(*this, _Filename, nwd);
	nwd = tier0::ReplaceStr(nwd, '\n', ' ');
	this->ExecuteCommand(nwd);
}

#pragma endregion Console

#pragma region ConCmd

engine::CConCmd::CConCmd(std::string _Name, engine::CONCMDCALLBACK _Callback, bool _FromCon)
	: m_Routine(_Callback)
{
	/* TODO: Remove this if-guard (as CConsole::AddConCmd does not recursively
	 *  calls this ctor anymore.
	 */
	if (!_FromCon)
		engine::CConsole::Instance().AddConCmd(_Name, this);
}

void engine::CConCmd::Execute(std::vector<std::string> _Args) const
{
	m_Routine(_Args);
}

#pragma endregion ConCmd
