#define _CRT_SECURE_NO_WARNINGS
#include "String0.h"
#include "Win32.h"
#include <locale.h>

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
