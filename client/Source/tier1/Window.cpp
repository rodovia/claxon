#include "Window.hh"

#include <imgui/imgui_impl_win32.h>
#include <tier0lib/String0.h>
#include <InfoParser.h>

#include <d3d11.h>

#include <format>
#include <sstream>
#include <resource.h>

#define MAKEICON(I, R, X, Y) static_cast<HICON>(LoadImage((I), MAKEINTRESOURCE(R), IMAGE_ICON, (X), (Y), 0))

#define CREATE_NOGFX_EXC() 0
// Used in CWindow ctor
#define WINDOW_STYLE WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU

hl2::CWindow::Class hl2::CWindow::Class::s_WndClass;

hl2::CWindow::Class::Class() noexcept
	: m_Instance(GetModuleHandle(nullptr))
{
	gamepaths::CInfoParser::FromFile("gameinfo.txt");

	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = nullptr;
	wc.hCursor = nullptr;
	wc.hIcon = MAKEICON(m_Instance, IDI_ICON1, 48, 48);
	wc.hIconSm = MAKEICON(m_Instance, IDI_ICON1, 16, 16);
	wc.hInstance = Class::Instance();
	wc.lpfnWndProc = &hl2::CWindow::HandleMessageSetup;
	wc.lpszClassName = s_ClassName;
	wc.lpszMenuName = nullptr;
	wc.style = CS_OWNDC;
	RegisterClassEx(&wc);
}

hl2::CWindow::Class::~Class()
{
	UnregisterClass(s_ClassName, Instance());
}

const wchar_t* hl2::CWindow::Class::Name() noexcept
{
	return s_ClassName;
}

HINSTANCE hl2::CWindow::Class::Instance() noexcept
{
	return s_WndClass.m_Instance;
}

// hl2::CWindow::Class ^^^^

static std::string StringDxFeatLevel(D3D_FEATURE_LEVEL _FeatLevel)
{
	switch (_FeatLevel)
	{
	case D3D_FEATURE_LEVEL_9_1:
		return "9_1";
	case D3D_FEATURE_LEVEL_9_2:
		return "9_2";
	case D3D_FEATURE_LEVEL_9_3:
		return "9_3";
	case D3D_FEATURE_LEVEL_10_0:
		return "10_0";
	case D3D_FEATURE_LEVEL_10_1:
		return "10_1";
	case D3D_FEATURE_LEVEL_11_0:
		return "11_0";
	case D3D_FEATURE_LEVEL_12_0:
		return "12_0";
	case D3D_FEATURE_LEVEL_12_1:
		return "12_1";
	default:
		return "PLS fix StringDxFeatLevel";
	}
}

// hl2::CWindow vvvv

hl2::CWindow::CWindow(int width, int height, const wchar_t* name, ImGuiContext* ctx, bool subwnd) 
	: m_IsSubwindow(subwnd),
	  m_Width(width),
	  m_Height(height)
{
	ImGui::SetCurrentContext(ctx);
	engine::imgui::SetAllocProcs();

	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;
	if (AdjustWindowRect(&wr, WINDOW_STYLE, FALSE) == 0)
	{
		throw CREATE_WINDOWEXC(GetLastError());
	}
	m_hWnd = CreateWindow(
		Class::Name(),
		name,
		WINDOW_STYLE,
		CW_USEDEFAULT, CW_USEDEFAULT,
		wr.right - wr.left, wr.bottom - wr.top,
		nullptr, nullptr,
		Class::Instance(), this
	);
	if (m_hWnd == nullptr)
	{
		throw CREATE_WINDOWEXC(GetLastError());
	}
	m_Gfx = std::make_unique<engine::CGraphicalOutput>(m_hWnd, ctx);
	ShowWindow(m_hWnd, SW_SHOWDEFAULT);
	ImGui_ImplWin32_Init(m_hWnd);

	std::string ti = gamepaths::CInfoParser::FromFile("gameinfo.txt").GetTitle();
	std::string tit = std::format("{} - Direct3D 11 ({})", ti, 
						StringDxFeatLevel(m_Gfx->GetFeatureLevel()));
	this->SetTitle(tier0::ConvertToWideString(tit));
}

void hl2::CWindow::SetIcon(HICON icon, HICON iconSm)
{
	SendMessage(m_hWnd, WM_SETICON, ICON_BIG, (LPARAM) icon);
	if (iconSm != nullptr)
	{
		SendMessage(m_hWnd, WM_SETICON, ICON_SMALL, (LPARAM) iconSm);
		return;
	}
	SendMessage(m_hWnd, WM_SETICON, ICON_SMALL, (LPARAM)icon);
}

engine::CGraphicalOutput& hl2::CWindow::Graphics()
{
	if (!m_Gfx)
	{
		throw CREATE_NOGFX_EXC();
	}
	return *m_Gfx;
}

std::optional<WPARAM> hl2::CWindow::ProcessMessage()
{
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return msg.wParam;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return { };
}

hl2::CWindow::~CWindow()
{
	ImGui_ImplWin32_Shutdown();
	DestroyWindow(m_hWnd);
}

LRESULT CALLBACK hl2::CWindow::HandleMessageSetup(HWND hWnd, UINT msg, WPARAM wParam, 
												LPARAM lParam)
{
	if (msg == WM_NCCREATE)
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		CWindow* const pWnd = static_cast<hl2::CWindow*>(pCreate->lpCreateParams);

		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		// Redirecting the procedure to CWindow::HandleMessageThunk
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, 
					reinterpret_cast<LONG_PTR>(&CWindow::HandleMessageThunk));
		return pWnd->HandleMessage(hWnd, msg, wParam, lParam);
	}

	// maybe unreachable
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK hl2::CWindow::HandleMessageThunk(HWND hWnd, UINT msg, WPARAM wParam,
												LPARAM lParam)
{
	CWindow* const pWnd = reinterpret_cast<hl2::CWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return pWnd->HandleMessage(hWnd, msg, wParam, lParam);
}

LRESULT hl2::CWindow::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam,
												LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	{
		return true;
	}

	switch (msg)
	{
	case WM_CLOSE:
		if (!m_IsSubwindow)
		{
			PostQuitMessage(0);
			return 0;
		}
			
		break;

	case WM_KILLFOCUS:
		m_Keyboard.ClearState();
		break;

	// vvvv KEYBOARD MESSAGES vvvv
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN: // Listen for ALT
		if (ImGui::GetIO().WantCaptureKeyboard)
		{
			break;
		}

		if (!(lParam & 0x40000000) || m_Keyboard.AutorepeatState())
		{
			m_Keyboard.OnKeyPressed(static_cast<uint8_t>(wParam));
			break;
		}
	case WM_KEYUP:
	case WM_SYSKEYUP:
		m_Keyboard.OnKeyReleased(static_cast<uint8_t>(wParam));
		break;
	case WM_CHAR:
		m_Keyboard.OnChar(static_cast<char>(wParam));
		break;
	/// ^^^^ KEYBOARD MESSAGES ^^^^ /// vvvv MOUSE MESSAGES vvv
	case WM_MOUSEMOVE:
	{
		POINTS pt = MAKEPOINTS(lParam);
		if (pt.x >= 0 && pt.x < m_Width && pt.y >= 0 && pt.y < m_Height)
		{
			m_Mouse.OnMouseMove(pt.x, pt.y);
			if (!m_Mouse.IsInClientRegion())
			{
				SetCapture(m_hWnd);
				m_Mouse.OnMouseEnter();
			}
		}
		else
		{
			if (wParam & (MK_LBUTTON | MK_RBUTTON))
			{
				m_Mouse.OnMouseMove(pt.x, pt.y);
			}
			else
			{
				ReleaseCapture();
				m_Mouse.OnMouseLeave();
			}
		}
		
		break;
	}
	case WM_LBUTTONDOWN:
	{
		POINTS pt = MAKEPOINTS(lParam);
		m_Mouse.OnLeftPressed(pt.x, pt.y);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		POINTS pt = MAKEPOINTS(lParam);
		m_Mouse.OnRightPressed(pt.x, pt.y);
		break;
	}
	case WM_LBUTTONUP:
	{
		POINTS pt = MAKEPOINTS(lParam);
		m_Mouse.OnLeftReleased(pt.x, pt.y);
		break;
	}
	case WM_RBUTTONUP:
	{
		POINTS pt = MAKEPOINTS(lParam);
		m_Mouse.OnRightReleased(pt.x, pt.y);
		break;
	}
	case WM_MOUSEWHEEL:
	{
		POINTS pt = MAKEPOINTS(lParam);
		int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		m_Mouse.OnWheelDelta(pt.x, pt.y, delta);
		break;
	}
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void hl2::CWindow::SetTitle(std::wstring _NewTitle) 
{
	if (SetWindowText(m_hWnd, _NewTitle.c_str()) == 0)
	{
		throw CREATE_WINDOWEXC(GetLastError());
	}
}

// hl2::CWindowException vvvvv

hl2::CWindowException::CWindowException(int line, const char* file, HRESULT hr)
	: hl2::CBaseException(line, file),
	  m_hResult(hr)
{ }

const char* hl2::CWindowException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << "\n" <<
		"[Error Code] " << GetErrorCode() << "\n" <<
		"[Description] " << GetErrorString() << "\n" << 
		GetOriginString();
	m_WhatBuffer = oss.str();
	return m_WhatBuffer.c_str();
}

const char* hl2::CWindowException::GetType() const noexcept
{
	return "Window Exception (hl2::CWindow::Exception)";
}

HRESULT hl2::CWindowException::GetErrorCode() const noexcept
{
	return m_hResult;
}

std::string hl2::CWindowException::GetErrorString() const noexcept
{
	return TranslateErrorCode(m_hResult);
}

std::string hl2::CWindowException::TranslateErrorCode(HRESULT hr) noexcept
{
	char* pMsgBuf = nullptr;
	DWORD nMsgLen = FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr);
	if (nMsgLen == 0)
	{
		return "Unknown";
	}

	std::string errorString = pMsgBuf;
	LocalFree(pMsgBuf);
	return errorString;
}
