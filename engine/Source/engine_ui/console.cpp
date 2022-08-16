#include "Console.h"
#include <imguihlp.h>
#include <tier0lib/String0.h>

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

engine::CConCmd help("help", Help);

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

engine::CConVar::CConVar(std::string _Name, std::string _Default, bool _FromCon)
	: m_Value(_Default)
{
	/* TODO: Remove this if-guard (as CConsole::AddConVar does not recursively
	 *  calls this ctor anymore.
	 */
	if (!_FromCon)
	{
		CConsole::Instance().AddConVar(_Name, this);
	}
}

std::string engine::CConVar::GetString() noexcept
{
	return m_Value;
}

int64_t engine::CConVar::GetInteger()
{
	return std::stoll(m_Value);
}

float engine::CConVar::GetFloat()
{
	return std::stof(m_Value);
}

bool engine::CConVar::GetBool()
{
	return (m_Value == "1" || m_Value == "y" ? true : false);
}

void engine::CConVar::SetValue(std::string _Value)
{
	m_Value = _Value;
}

#pragma endregion ConVar

#pragma region Console

engine::CConsole::CConsole()
{
	std::memset(s_WndBuffer, 0, sizeof(s_WndBuffer));
	m_LogMgr = new LogBufferMgr;
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
	if (this->QueryConVar(_Name) != nullptr)
	{
		return;
	}

	m_Vars.emplace(_Name, _Variable);
}

void engine::CConsole::AddConCmd(std::string _Name, CConCmd* _Variable)
{
	if (this->QueryConCmd(_Name) != nullptr)
	{
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
			std::vector<std::string> cmds = tier0::SplitStr(s_WndBuffer, ';');
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
		return;
	}

	// args.size() == 1
	this->EmitMessage("%s = %s", args[0].c_str(), var->GetString().c_str());
}

std::unordered_map<std::string, engine::CConCmd*> engine::CConsole::GetCmds() const noexcept
{
	return m_Cmds;
}

std::unordered_map<std::string, engine::CConVar*> engine::CConsole::GetVars() const noexcept
{
	return m_Vars;
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
