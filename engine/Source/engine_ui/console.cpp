#include "Console.h"
#include <imguihlp.h>

char engine::CConsole::s_WndBuffer[_ENGINE_CONBUFFERSZ];

void engine::LogBufferMgr::AddLog(const char* _Fmt, ...)
{
	int oldS = Buffer.size();
	va_list argl;
	va_start(argl, _Fmt);
	Buffer.appendfv(_Fmt, argl);
	va_end(argl);

	ScrollToBottom = true;
}

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

constexpr static std::vector<std::string> 
SplitStr(std::string_view _String, std::string_view _Delim)
{
	std::string token, strcpy;
	std::copy(_String.begin(), _String.end(), strcpy.begin());

	std::vector<std::string> vecs;

	while (token != _String)
	{
		token = strcpy.substr(0, strcpy.find_first_of(_Delim));
		strcpy = strcpy.substr(strcpy.find_first_of(_Delim) + 1);
		vecs.push_back(token);
	}

	return std::move(vecs);
}

engine::CConVar::CConVar(std::string _Name, std::string _Default)
	: m_Value(_Default)
{
	CConsole::Instance().AddConVar(_Name, *this);
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

// engine::CConVar ^^^^^ // engine::CConsole vvvvv

void engine::CConsole::AddConVar(std::string _Name, CConVar _Variable)
{
	if (this->QueryConVar(_Name) != nullptr)
	{
		return;
	}

	m_Vars.emplace(_Name, std::make_shared<engine::CConVar>(_Variable.GetString()));
}

void engine::CConsole::AddConCmd(std::string _Name, CConCmd _Variable)
{
	if (this->QueryConCmd(_Name) != nullptr)
	{
		return;
	}

	m_Cmds.insert(
		std::make_pair(
			_Name, 
			std::make_shared<engine::CConCmd>(_Name, _Variable.m_Routine, true)
		)
	);
}

engine::CConVar* engine::CConsole::QueryConVar(std::string _Name) const
{
	auto c = m_Vars.find(_Name);
	if (c != m_Vars.end())
	{
		return c->second.get();
	}
	return nullptr;
}

engine::CConCmd* engine::CConsole::QueryConCmd(std::string _Name) const
{
	auto c = m_Cmds.find(_Name);
	if (c != m_Cmds.end())
	{
		return c->second.get();
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
		con.m_LogMgr.Draw();

		int flags = ImGuiInputTextFlags_EnterReturnsTrue;
		if (ImGui::InputText("Entrada", s_WndBuffer, _ENGINE_CONBUFFERSZ,
			flags))
		{
			std::vector<std::string> cmds = SplitStr(s_WndBuffer, ";");
			for (const std::string& cmd : cmds)
			{
				con.ExecuteCommand(cmd);
			}
		}
	}

	ImGui::End();
}

void engine::CConsole::ExecuteCommand(std::string_view _Cmd)
{
	this->EmitMessage("] %s", _Cmd.data());
	std::vector<std::string> args = SplitStr(_Cmd, " ");
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

	var->SetValue(args[1]);
}

void engine::CConsole::EmitMessage(const char* _Fmt, ...)
{
	va_list argl;
	va_start(argl, _Fmt);
	m_LogMgr.AddLog(_Fmt, argl);
	va_end(argl);
}

// engine::CConsole ^^^^^ // engine::CConCmd vvvvv

engine::CConCmd::CConCmd(std::string _Name, engine::CONCMDCALLBACK _Callback, bool _FromCon)
	: m_Routine(_Callback)
{
	if (!_FromCon)
		engine::CConsole::Instance().AddConCmd(_Name, *this);
}

void engine::CConCmd::Execute(std::vector<std::string> _Args) const
{
	m_Routine(_Args);
}
