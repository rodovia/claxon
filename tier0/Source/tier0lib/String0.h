#pragma once

#include <string>

namespace tier0
{

std::wstring ConvertToWideString(const std::string& str);
std::string ConvertToMultiByteString(const std::wstring& wstr);

}
