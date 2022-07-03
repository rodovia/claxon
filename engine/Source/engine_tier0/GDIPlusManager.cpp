#define FULL_WINTARD
#include <tier0lib/Win32.h>
#include <algorithm>

namespace Gdiplus
{
	using std::min;
	using std::max;
}

#include "GDIPlusManager.h"
#include <gdiplus.h>

ULONG_PTR engine::CGDIPlusManager::m_Token = 0;
int engine::CGDIPlusManager::m_RefC = 0;

engine::CGDIPlusManager::CGDIPlusManager()
{
	if (m_RefC++ == 0)
	{
		Gdiplus::GdiplusStartupInput inp;
		inp.GdiplusVersion = 1;
		inp.SuppressBackgroundThread = false;
		Gdiplus::GdiplusStartup(&m_Token, &inp, nullptr);
	}
}

engine::CGDIPlusManager::~CGDIPlusManager()
{
	if (--m_RefC == 0)
	{
		Gdiplus::GdiplusShutdown(m_Token);
	}
}
