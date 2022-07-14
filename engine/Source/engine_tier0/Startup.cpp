#include <tier0lib/Win32.h>
#include <tier0lib/String0.h>
#include <libconfig.h++>
#include <format>
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
		try
		{
			gamepaths::CInfoParser::FromFile("gameinfo.txt");
		}
		catch (libconfig::FileIOException)
		{
			MessageBoxW(nullptr, L"O ficheiro gameinfo.txt não existe", 
				L"engine.dll Startup Error", MB_ICONEXCLAMATION | MB_OK);
			return FALSE;
		}
		catch (libconfig::ParseException& exc)
		{
			std::wstring ffs = std::format(
				L"O ficheiro está mal formado.\n{} ({} {})", 
				tier0::ConvertToWideString(exc.getError()), 
				exc.getLine(), 
				tier0::ConvertToWideString(exc.getFile())
			);
			MessageBoxW(nullptr, ffs.c_str(), 
				L"engine.dll Startup Error", MB_ICONEXCLAMATION | MB_OK);
			return FALSE;
		}
		return TRUE;
	default:
		return TRUE;
	}
}
