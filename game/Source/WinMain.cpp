#include "tier0lib/Win32.h"

#include <string>
#include <sstream>
#include <resource.h>
#include <libconfig.h++>
#include <imguihlp.h>
#include <engine_tier0/Exceptions.h>
#include <tier1/Window.hh>
#include "ApplicationEntry.h"

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	PSTR pCmdLine,
	int nCmdShow)
{
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	WPARAM retval = 0;
	try
	{
		hl2::CApplication app;
		retval = app.Main();
	}
	catch (const hl2::CBaseException& exc)
	{
		MessageBoxA(nullptr, exc.what(), "Engine Error", MB_OK | MB_ICONERROR);
	}
	catch (const engine::CBaseException& exc)
	{
		MessageBoxA(nullptr, exc.what(), "Engine Error", MB_OK | MB_ICONERROR);
	}
	catch (const libconfig::ParseException& exc)
	{
		std::string cnt = std::format("O ficheiro está mal formado.\n{} ({}:{})",
			exc.getError(), exc.getFile(), exc.getLine());
		MessageBoxA(nullptr, cnt.c_str(), "Engine Error", MB_OK | MB_ICONERROR);
	}
	catch (const std::exception& exc)
	{
		MessageBoxA(nullptr, exc.what(), "Engine Error", MB_OK | MB_ICONERROR);
	}
	catch (...)
	{
		MessageBoxA(nullptr, "Unknown", "Engine Error", MB_OK | MB_ICONERROR);
	}

	CoUninitialize();
	return (int)retval;
}
