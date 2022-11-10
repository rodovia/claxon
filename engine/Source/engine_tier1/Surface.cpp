#define FULL_WINTARD
#define _CRT_SECURE_NO_WARNINGS
#include <tier0lib/Win32.h>
#include <algorithm>
#include <assert.h>
#include <sstream>
#include "Surface.h"
#include <tier0lib/String0.h>
#include <engine_tier0/Exceptions.h>
#define STBI_WINDOWS_UTF8
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

engine::CSurface::CSurface(unsigned int _Width, unsigned int _Height) noexcept
{
}

engine::CSurface::CSurface(engine::CSurface&& _Source) noexcept
	: m_Buffer(_Source.m_Buffer),
	  m_BufferSize(_Source.m_BufferSize),
	  m_Filename(_Source.m_Filename),
	  m_Width(_Source.m_Width),
	  m_Height(_Source.m_Height),
	  m_HasAlpha(_Source.m_HasAlpha)
{
}

engine::CSurface& engine::CSurface::operator=(engine::CSurface&& _Source) noexcept
{
	m_Buffer = new engine::CColor[_Source.m_BufferSize];
	std::memmove(m_Buffer, _Source.m_Buffer, _Source.m_BufferSize);
	m_BufferSize = _Source.m_BufferSize;
	m_Filename = _Source.m_Filename;
	m_Width = _Source.m_Width;
	m_Height = _Source.m_Height;
	m_HasAlpha = _Source.m_HasAlpha;
	return *this;
}

engine::CSurface::~CSurface()
{
	delete[] m_Buffer;
}

void engine::CSurface::Clear(engine::CColor _FillValue) noexcept
{
	std::memset(m_Buffer, _FillValue.m_Dword, m_BufferSize);
}

void engine::CSurface::PutPixel(unsigned int _X, unsigned int _Y, CColor _C) noexcept
{
	assert(_X >= 0);
	assert(_Y >= 0);
	assert(_X < m_Width);
	assert(_Y < m_Height);

	m_Buffer[_Y * m_Width + _X] = _C;
}

engine::CColor engine::CSurface::GetPixel(unsigned int _X, unsigned int _Y) const noexcept
{
	assert(_X >= 0);
	assert(_Y >= 0);
	assert(_X < m_Width);
	assert(_Y < m_Height);

	return m_Buffer[_Y * m_Width + _X];
}

unsigned int engine::CSurface::GetWidth() const noexcept
{
	return m_Width;
}

unsigned int engine::CSurface::GetHeight() const noexcept
{
	return m_Height;
}

engine::CColor* engine::CSurface::GetBufferPointer() noexcept
{
	return m_Buffer;
}

const engine::CColor* engine::CSurface::GetBufferPointer() const noexcept
{
	return m_Buffer;
}

const engine::CColor* engine::CSurface::GetBufferPointerConst() const noexcept
{
	return m_Buffer;
}

engine::CSurface engine::CSurface::FromFile(const std::wstring& _FileName)
{
	FILE* fp = _wfopen(_FileName.data(), L"rb");
	bool hasAlpha = false;

	int width, height;
	int imgChn;
	unsigned char* imgB = stbi_load_from_file(fp, &width, &height, &imgChn, 4);
	if (imgB == nullptr)
	{
		engine::FastFail(stbi_failure_reason());
	}
	engine::CColor* clr = new engine::CColor[width * height];
	for (unsigned int x = 0; x < width; x++)
	{
		for (unsigned int y = 0; y < height; y++)
		{
			unsigned char* pxlOffs = imgB + (x + width * y) * 4;

			unsigned char r = pxlOffs[0];
			unsigned char g = pxlOffs[1];
			unsigned char b = pxlOffs[2];
			unsigned char a = pxlOffs[3];
			if (a != 255)
			{
				hasAlpha = true;
			}
			clr[x + width * y] = engine::CColor(a, r, g, b);
		}
	}
	stbi_image_free(imgB);

	return CSurface(clr, width * height, _FileName.data(), width, height, hasAlpha);
}

void engine::CSurface::WriteToFile(std::wstring_view _Filename) const
{
}

const wchar_t* engine::CSurface::GetFilename() const noexcept
{
	return m_Filename;
}

engine::CSurface::CSurface(engine::CColor* _Buffer, size_t _BufferSize,
						   const wchar_t* _Filename, int _Width, int _Height,
						   bool _HasAlpha) noexcept
	: m_Buffer(_Buffer),
	  m_BufferSize(_BufferSize),
	  m_Filename(_Filename),
	  m_Width(_Width),
	  m_Height(_Height),
	  m_HasAlpha(_HasAlpha)
{
}

bool engine::CSurface::HasAlpha() const noexcept
{
	return m_HasAlpha;
}
