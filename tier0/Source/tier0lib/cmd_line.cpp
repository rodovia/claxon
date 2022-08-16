#include "CommandLine.h"
#include "String0.h"

tier0::CCmdInputParser::CCmdInputParser(std::string _Args)
	: m_Args(tier0::SplitStr(_Args, ' '))
{}

tier0::CCmdInputParser::CCmdInputParser(std::vector<std::string> _Args)
	: m_Args(_Args)
{}

std::optional<std::string> tier0::CCmdInputParser::GetOptionValue(std::string _Opt) const noexcept
{
	for (auto it = m_Args.begin(); it != m_Args.end(); ++it)
	{
		auto next = it++;
		if (*it == m_Prefix + _Opt && next != m_Args.end() && !it->starts_with(m_Prefix))
		{
			std::string cpy;
			std::copy(it->begin(), it->end(), cpy.begin());
			return std::move(cpy);
		}
	}

	return std::nullopt;
}

bool tier0::CCmdInputParser::HasOption(std::string _Opt) const noexcept
{
	for (auto it = m_Args.begin(); it != m_Args.end(); ++it)
	{
		if (*it == m_Prefix + _Opt)
		{
			return true;
		}
	}

	return false;
}
