#include <tier0lib/Win32.h>
#include <InfoParser.h>

BOOL WINAPI DllMain(
	HINSTANCE hInstance,
	DWORD fdwReason,
	LPVOID lpReserved
)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		gamepaths::CInfoParser::FromFile("gameinfo.txt");
		return TRUE;
	default:
		return TRUE;
	}
}
