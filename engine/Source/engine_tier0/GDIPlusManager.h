#pragma once

#define _FULL_WINTARD
#include <tier0lib/Win32.h>
#include "DLL.h"

namespace engine
{

class _ENGINE_DLLEXP CGDIPlusManager
{
public:
	CGDIPlusManager();
	~CGDIPlusManager();
private:
	static ULONG_PTR m_Token;
	static int m_RefC;
};

}
