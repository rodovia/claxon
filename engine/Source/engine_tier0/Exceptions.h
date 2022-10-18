#pragma once

#include <tier0lib/Win32.h>
#include <stdexcept>
#include <string>
#include <format>

#include "DXGIInfoManager.h"
#include "DLL.h"

#define _ENGINE_CREATE_VAR HRESULT hr
#define _ENGINE_MAYTHROW_GRAPHICS(E) \
	if (FAILED(hr = (E)))            \
	throw engine::CGraphicsException(__LINE__, __FILE__, hr)
#define _ENGINE_CREATE_FROMLASTERROR engine::CFromHResultException(__LINE__, __FILE__, GetLastError())
#define _ENGINE_CREATE_DEVC(E) engine::CDeviceRemovedException((E));

#define _CModelException(M) engine::CModelException(__LINE__, __FILE__, (M))

#define FastFailCheck(H)                                                  \
	do                                                                    \
	{                                                                     \
		if (FAILED(H))                                                    \
		{                                                                 \
			throw engine::CFromHResultException(__LINE__, __FILE__, (H)); \
		}                                                                 \
	} while (0)

#if defined(__hl2_want_crt_assert)
#define FailIf(E) assert(E)
#else
#define FailIf(E) do { if (!E) engine::FastFailEx(nullptr, "**{}** was {}", #E ## , (E)) } while(0) //clang-format: keep
#endif

namespace engine
{

enum class EFM_Type
{
	WIN32_EXCEPTION = 0x100,
	DIRECTX_EXCEPTION = 0x101
};

void _ENGINE_DLLEXP FastFail(const char* _Msg = nullptr,
							 HWND _Wnd = nullptr) noexcept;

template<class..._Ty_Args>
void FastFailEx(HWND _Wnd, char* _Msg, _Ty_Args&&..._Args) noexcept
{
	std::string cns = _Msg;
	cns = std::format(cns, _Args...);
	FastFail(cns.c_str(), _Wnd);
}


// _Buffer param must have this size
#define _ENGINE_EFM_BUFFER_SIZE 512
bool _ENGINE_DLLEXP ExcFormatMessage(HRESULT _Hres, EFM_Type _Type,
									 char** _Buffer) noexcept;

class _ENGINE_DLLEXP CBaseException : public std::exception
{
public:
	CBaseException() = default;
	CBaseException(int line, const char* file);
	const char* what() const noexcept override;
	int GetLine() const noexcept;
	const std::string& GetFile() const noexcept;
	virtual const char* GetType() const noexcept;
	virtual std::string GetOriginString() const noexcept;

protected:
	int m_Line;
	std::string m_File;
	mutable std::string m_WhatBuffer;
};

class _ENGINE_DLLEXP CEngineError : public CBaseException
{
public:
	CEngineError();
	CEngineError(std::string _Message);
	const char* what() const noexcept override;
	const char* GetType() const noexcept override;

private:
	std::string m_Message;
};

class _ENGINE_DLLEXP CFromHResultException : public CBaseException
{
public:
	CFromHResultException(int _Line, const char* _File, HRESULT _Result);
	CFromHResultException(int _Line, const char* _File, HRESULT _Result,
						  std::vector<std::string> _ExtraInfo);
	virtual std::string GetErrorDescription() noexcept;
	std::string GetExtraInformation() const noexcept;

protected:
	HRESULT m_Result;

private:
	std::vector<std::string> m_Info;
};

class _ENGINE_DLLEXP CGraphicsException : public CFromHResultException
{
public:
	using CFromHResultException::CFromHResultException;
	const char* GetType() const noexcept;
	std::string GetOriginString() const noexcept override;
};

class _ENGINE_DLLEXP CDeviceRemovedException : public CBaseException
{
public:
	CDeviceRemovedException(HRESULT _DvcReason);
	const char* what() const noexcept override;

private:
	HRESULT m_Result;
};

class _ENGINE_DLLEXP CModelException : public CBaseException
{
public:
	CModelException(int _Line, const char* _File, std::string _Note) noexcept;
	const char* what() const noexcept override;
	const char* GetType() const noexcept override;
	const std::string& GetNote() const noexcept;

private:
	std::string m_Note;
};

} // namespace engine
