#pragma once

#include <exception>
#include <string>

#include "DLL.h"

namespace hl2
{

class DLLEXP CBaseException : public std::exception
{
public:
	CBaseException(int line, const char* file);
	const char* what() const noexcept override;
	virtual const char* GetType() const noexcept;
	int GetLine() const noexcept;
	const std::string& GetFile() const noexcept;
	std::string GetOriginString() const noexcept;
protected:
	int m_Line;
	std::string m_File;
protected:
	mutable std::string m_WhatBuffer;
};

}
