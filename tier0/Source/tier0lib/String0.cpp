#define _CRT_SECURE_NO_WARNINGS
#include "String0.h"
#include "Win32.h"
#include <locale.h>
#include <sstream>
#include <vector>
#include <iterator>

std::wstring tier0::ConvertToWideString(const std::string& _String)
{
	size_t sz = _mbstowcs_l(nullptr, _String.c_str(), _String.size() + 1, _get_current_locale());
	if (sz < 0)
	{
		return L"";
	}

	wchar_t* buffer = new wchar_t[sz + 1];
	_mbstowcs_l(buffer, _String.c_str(), _String.size() + 1, _get_current_locale());

	std::wstring neostr = buffer;
	delete[] buffer;
	return std::move(neostr);
}

std::string tier0::ConvertToMultiByteString(const std::wstring& wstr)
{
	std::wstring cpy = wstr;
	int num_chars = WideCharToMultiByte(CP_UTF8, 0, cpy.c_str(), cpy.length(), NULL, 0, NULL, NULL);
	std::string strTo;
	if (num_chars > 0)
	{
		strTo.resize(num_chars);
		WideCharToMultiByte(CP_UTF8, 0, cpy.c_str(), cpy.length(), &strTo[0], num_chars, NULL, NULL);
	}
	return strTo;
}

template<class _Ty_Chr = char, class _Ty_Out>
static void Split0(const std::basic_string<_Ty_Chr>& _String, _Ty_Chr _Delim, _Ty_Out _Result)
{
	using _Ty_String = std::basic_string<_Ty_Chr>;

	std::basic_istringstream<_Ty_Chr> iss(_String);
	_Ty_String item;
	while (std::getline(iss, item, _Delim))
	{
		*_Result++ = item;
	}
}

std::vector<std::string>
tier0::SplitStr(std::string _String, char _Delim) noexcept
{
	std::vector<std::string> elems;
	Split0(_String, _Delim, std::back_inserter(elems));
	return std::move(elems);
}

std::vector<std::wstring>
tier0::SplitStr(std::wstring _String, wchar_t _Delim) noexcept
{
	std::vector<std::wstring> elems;
	Split0(_String, _Delim, std::back_inserter(elems));
	return std::move(elems);
}

std::string
tier0::JoinStr(std::vector<std::string> _String, char _Joiner) noexcept
{
	std::string ret;
	for (const auto& i : _String)
	{
		ret += _Joiner;
	}

	return std::move(ret);
}

std::string
tier0::ReplaceStr(std::string _String, char _Org, char _Repl) noexcept
{
	std::string neo;
	neo.reserve(_String.length());

	std::string::size_type lastPos = 0, 
		findPos;

	while ((findPos = _String.find(_Org, lastPos)) != std::string::npos)
	{
		neo.append(_String, lastPos, findPos - lastPos);
		neo += _Repl;
		lastPos = findPos;
	}

	neo += _String.substr(lastPos);
	return std::move(neo);
}
