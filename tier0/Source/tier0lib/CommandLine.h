#pragma once

#include <string>
#include <vector>
#include <optional>

namespace tier0
{

class CCmdInputParser
{
public:
	CCmdInputParser(std::string _Args);
	CCmdInputParser(std::vector<std::string> _Args);

	bool HasOption(std::string _Opt) const noexcept;
	std::optional<std::string> GetOptionValue(std::string _Opt) const noexcept;
private:
	std::vector<std::string> m_Args;
	std::string m_Prefix = "/";
};

}
