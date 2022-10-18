#pragma once

#include <string>
#include <vector>

namespace tier0
{

std::wstring ConvertToWideString(const std::string& str);
std::string ConvertToMultiByteString(const std::wstring& wstr);

std::vector<std::string> SplitStr(std::string _String, char _Delim) noexcept;
std::vector<std::wstring> SplitStr(std::wstring _String, wchar_t _Delim) noexcept;

// TODO: Make wstring variants
std::string JoinStr(std::vector<std::string> _String, char _Joiner) noexcept;

std::string ReplaceStr(std::string _String, char _Org, char _Another) noexcept;

}
