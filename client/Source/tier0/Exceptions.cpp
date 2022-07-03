#include "Exceptions.h"
#include <sstream>

hl2::CBaseException::CBaseException(int line, const char* file)
	: m_Line(line),
	  m_File(file)
{}

const char* hl2::CBaseException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << "\n" << GetOriginString();
	m_WhatBuffer = oss.str();
	return m_WhatBuffer.c_str();
}

const std::string& hl2::CBaseException::GetFile() const noexcept
{
	return m_File;
}

const char* hl2::CBaseException::GetType() const noexcept
{
	return "Base Exception (hl2::CBaseException)";
}

int hl2::CBaseException::GetLine() const noexcept
{
	return m_Line;
}


std::string hl2::CBaseException::GetOriginString() const noexcept
{
	std::ostringstream oss;
	oss << "[Line] " << m_Line << 
		   "\n[File] " << GetFile();
	return oss.str();
}
