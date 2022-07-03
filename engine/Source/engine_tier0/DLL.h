#pragma once

#ifdef ENGINE_DLL
#define _ENGINE_DLLEXP __declspec(dllexport)
#else
#define _ENGINE_DLLEXP __declspec(dllimport)
#endif
