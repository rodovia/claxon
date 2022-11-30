#define _CRT_SECURE_NO_WARNINGS
#include "Exceptions.h"
#include <tier0lib/dxerr.h>
#include <sstream>
#include <malloc.h>

void engine::FastFail(const char* _Msg, HWND _Wnd) noexcept
{
	static const char* cnclmsg = "\n(Press cancel to debug)";
	char* cpy = new char[std::strlen(_Msg) + std::strlen(cnclmsg)];
	memcpy(cpy, _Msg, std::strlen(_Msg));
	if (_Msg == nullptr)
	{
		cpy = (char*)"FastFail was called.";
	}

	strcat(cpy, cnclmsg);
	int ch = MessageBoxA(_Wnd, _Msg, "Engine error", MB_ICONERROR | MB_OKCANCEL | MB_TASKMODAL);
	switch (ch)
	{
	case IDOK:
		exit(0xA);
	case IDCANCEL:
		__debugbreak();
		break;
	}
	delete[] cpy;
}

engine::CBaseException::CBaseException(int line, const char* file)
	: m_Line(line),
	m_File(file)
{}

const char* engine::CBaseException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << "\n" << GetOriginString();
	m_WhatBuffer = oss.str();
	return m_WhatBuffer.c_str();
}

const std::string& engine::CBaseException::GetFile() const noexcept
{
	return m_File;
}

const char* engine::CBaseException::GetType() const noexcept
{
	return "Base Exception (hl2::CBaseException)";
}

int engine::CBaseException::GetLine() const noexcept
{
	return m_Line;
}

std::string engine::CBaseException::GetOriginString() const noexcept
{
	std::ostringstream oss;
	oss << "[Line] " << m_Line <<
		"\n[File] " << GetFile();
	return oss.str();
}

// engine::CBaseException ^^^^

// engine::CEngineError vvvv

engine::CEngineError::CEngineError()
	: m_Message("Unknown")
{

}

engine::CEngineError::CEngineError(std::string _Message)
	: m_Message(_Message)
{}

const char* engine::CEngineError::what() const noexcept
{
	return m_Message.c_str();
}

const char* engine::CEngineError::GetType() const noexcept
{
	return "Engine error";
}

// engine::CFromHResultException vvvv

engine::CFromHResultException::CFromHResultException(int _Line, const char* _File, HRESULT _Result)
	: m_Result(_Result),
	  CBaseException(_Line, _File)
{
}

engine::CFromHResultException::CFromHResultException(int _Line, const char* _File, HRESULT _Result,
													std::vector<std::string> _ExtraInfo)
	: m_Result(_Result),
	  m_Info(_ExtraInfo),
      CBaseException(_Line, _File)
{
}


std::string engine::CFromHResultException::GetErrorDescription() noexcept
{
	std::ostringstream oss;
	char* buffer = new char[_ENGINE_EFM_BUFFER_SIZE];
	engine::ExcFormatMessage(m_Result, engine::EFM_Type::WIN32_EXCEPTION, &buffer);

	oss << "[Description]" << buffer
		<< "[Code] " << m_Result;
	delete[] buffer;
	return oss.str();
}

std::string engine::CFromHResultException::GetExtraInformation() const noexcept
{
	std::ostringstream oss;
	for (const std::string& i : m_Info)
	{
		oss << i << "\n";
	}
	return oss.str();
}

// engine::CFromHResultException ^^^^

// engine::CGraphicsException vvvv

const char* engine::CGraphicsException::GetType() const noexcept
{
	return "DirectX Exception!";
}

std::string engine::CGraphicsException::GetOriginString() const noexcept
{
	char* buffer = new char[_ENGINE_EFM_BUFFER_SIZE];
	engine::ExcFormatMessage(m_Result, engine::EFM_Type::DIRECTX_EXCEPTION, &buffer);
	std::ostringstream oss;
	oss << "[Description] " << buffer
		<< "[Code] " << m_Result << "\n"
		<< "[Extra Information] " << this->GetExtraInformation() << "\n\n"
		<< "Se houver um depurador associado ao processo, aceda-o para mais informações.";
	
	delete[] buffer;
	return oss.str();
}

// engine::CGraphicsException ^^^^ // engine::CDeviceRemovedException

engine::CDeviceRemovedException::CDeviceRemovedException(HRESULT _DvcReason)
	: m_Result(_DvcReason)
{ }

const char* engine::CDeviceRemovedException::what() const noexcept
{
	char* buffer = new char[_ENGINE_EFM_BUFFER_SIZE];
	engine::ExcFormatMessage(m_Result, EFM_Type::DIRECTX_EXCEPTION, &buffer);
	std::string cpy = buffer;
	delete[] buffer;

	return cpy.c_str();
}

// engine::CDeviceRemovedException ^^^^^ // engine::CModelException vvvv

engine::CModelException::CModelException(int _Line, const char* _File, std::string _Note) noexcept
	: CBaseException(_Line, _File),
	m_Note(_Note)
{}

const char* engine::CModelException::GetType() const noexcept
{
	return "Model Error!";
}

const char* engine::CModelException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << "\n" << GetOriginString() << "\nMessage: " << m_Note;
	m_WhatBuffer = oss.str();
	return m_WhatBuffer.c_str();
}

const std::string& engine::CModelException::GetNote() const noexcept
{
	return m_Note;
}

// Extracts the Win32 error code from the HRESULT
static DWORD CvtResultToWin32Error(HRESULT Hr)
{
	if ((Hr & 0xFFFF0000) == MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, 0))
	{
		return HRESULT_CODE(Hr);
	}
	if (Hr == S_OK)
	{
		return ERROR_SUCCESS;
	}

	return ERROR_CAN_NOT_COMPLETE;
}

bool engine::ExcFormatMessage(HRESULT _Hres, engine::EFM_Type _Type, char** _Buffer) noexcept
{
	switch (_Type)
	{
	case engine::EFM_Type::DIRECTX_EXCEPTION:
		DXGetErrorDescriptionA(_Hres, *_Buffer, _ENGINE_EFM_BUFFER_SIZE);
		return true;
	case engine::EFM_Type::WIN32_EXCEPTION:
		FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr,
			CvtResultToWin32Error(_Hres),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
			*_Buffer,
			0, nullptr 
		);
		return true;
	}

	return false;
}
