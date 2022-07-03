#pragma once

/*
*	This file should be the first include in the translation unit.
*	Windows.h may get included by indirection.
*/
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601 // WIN7
#endif
#include <sdkddkver.h>

/* Getting undefined symbol errors? Try commenting some of the
*	#define macros below, though, it will increase build time and all TUs that
*	includes this file will need to be rebuilt.
*/

#define WIN32_LEAN_AND_MEAN
#define GDICAPMASKS
/* #define NOSYSMETRICS /* I want system metrics in my program :( */
#define NOMENUS
#define NOSYSCOMMANDS
#define NORASTEROPS
#define QEMRESOURCE
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
#define NOCTLMGR
#define NODRAWTEXT
#define NOMETAFILE
#define NOMINMAX
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOTEXTMETRIC
#define NOWH
#define NOCOMM
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#define NORPC
#define NOPROXYSIUB
#define NOIMAGE
#define NOTAPE

#ifndef STRICT
#define STRICT
#endif

#include <windows.h>

