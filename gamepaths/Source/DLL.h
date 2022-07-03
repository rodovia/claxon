#pragma once

#ifdef _GAMEPATHS_DLL
#	define _GP_DLLEXP __declspec(dllexport)
#else
#	define _GP_DLLEXP __declspec(dllimport)
#endif
