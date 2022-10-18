#pragma once

#include "tier0lib/Win32.h"
#include "tier0/DLL.h"
#include "tier0/Exceptions.h"

#include <imguihlp.h>
#include <engine_tier1/GraphicalOutput.h>

#include "Keyboard.h"
#include "Mouse.h"

#include <string>
#include <memory>
#include <optional>

#define CREATE_WINDOWEXC(R) hl2::CWindowException(__LINE__, __FILE__, (R))

struct ResolutionWH
{
	int Width, Height;
};

namespace hl2
{

class DLLEXP CWindowException : public hl2::CBaseException
{
public:
	CWindowException(int line, const char* file, HRESULT hr);
	const char* what() const noexcept override;
	virtual const char* GetType() const noexcept;
	static std::string TranslateErrorCode(HRESULT hr) noexcept;
	std::string GetErrorString() const noexcept;
	HRESULT GetErrorCode() const noexcept;
private:
	HRESULT m_hResult;
};

class DLLEXP CWindow
{
public:
	class Class // (the Windows programming concept)
	{
	public:
		static const wchar_t* Name() noexcept;
		static HINSTANCE Instance() noexcept;
	private:
		Class() noexcept;
		~Class() noexcept;
		Class(const Class&) = delete;
		Class& operator=(const Class&) = delete;

		static constexpr const wchar_t* s_ClassName = L"__hl2wnd";
		static CWindow::Class s_WndClass;
		HINSTANCE m_Instance;
	};
	using Exception = hl2::CWindowException;

public:
	/* 
		subwnd -- If true, the process will not be killed when this window closes
	*/
	CWindow(int width, int height, const wchar_t* name, ImGuiContext* ctx = nullptr, bool subwnd = false);
	~CWindow() noexcept;

	CWindow(const CWindow&) = delete;
	CWindow& operator=(const CWindow&) = delete;
	void SetTitle(std::wstring _NewTitle);
	void SetIcon(HICON icon, HICON iconSm = nullptr);
	[[nodiscard]] std::shared_ptr<engine::CGraphicalOutput> GetGraphicalOutput() const noexcept;
	
	[[nodiscard]] ResolutionWH GetSize() const noexcept;

	static std::optional<WPARAM> ProcessMessage();
	void ToggleCursorDisplay(bool _AlsoConfine = true);
	bool CursorEnabled() const noexcept;
public:
	hl2::CKeyboard m_Keyboard;
	hl2::CMouse m_Mouse;
private:
	void HideCursor();
	void ShowCursor();

	void ConfineCursor();
	void FreeCursor();

	static LRESULT CALLBACK HandleMessageSetup(HWND, UINT, WPARAM, LPARAM);
	static LRESULT CALLBACK HandleMessageThunk(HWND, UINT, WPARAM, LPARAM);
	LRESULT CALLBACK HandleMessage(HWND, UINT, WPARAM, LPARAM);
private:
	int m_Width;
	int m_Height;
	HWND m_hWnd;
	bool m_IsSubwindow;
	bool m_CursorEnabl;
	std::vector<char> m_RawInputBuffer;
	std::shared_ptr<engine::CGraphicalOutput> m_Gfx;
};

} // namespace hl2
