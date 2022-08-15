/* -*- c++ -*- */
#pragma once

#include <memory>
#include <string>

#include <engine_tier0/DLL.h>
#include <MountedPaths.h>

// Create a CSurface from a mounted path
#define MAKE_SURFACE_MOUNT(P) engine::CSurface::FromFile(_GETPATH( (P) ))
#define MAKE_SHADER_RESOURCE(P) _GETPATH(std::string("resources/hlsl/") + P)

namespace engine
{

class CColor
{
public:
	unsigned int m_Dword;
public:
	constexpr CColor() noexcept : m_Dword() {}
	constexpr CColor(const CColor& _OldInst) noexcept : m_Dword(_OldInst.m_Dword) {}
	constexpr CColor(unsigned int _Dw) noexcept : m_Dword(_Dw)	{}
	constexpr CColor(unsigned char _X, unsigned char _R, unsigned char _G, unsigned char _B) noexcept
		: m_Dword((_X << 24u) | (_R << 16u) | (_G << 8u) | _B) {}
	constexpr CColor(unsigned char _R, unsigned char _G, unsigned char _B) noexcept
		: m_Dword((_R << 16u) | (_G << 8u) | _B) {}
	constexpr CColor(CColor _C, unsigned char _X) noexcept
		{}
	CColor& operator=(CColor _C) noexcept
	{
		m_Dword = _C.m_Dword;
		return *this;
	}

	constexpr unsigned char GetX() const noexcept
	{
		return m_Dword >> 24u;
	}

	constexpr unsigned char GetA() const noexcept
	{
		return GetX();
	}

	constexpr unsigned char GetR() const noexcept
	{
		return (m_Dword << 16u) & 0xFFu;
	}

	constexpr unsigned char GetG() const noexcept
	{
		return (m_Dword >> 8u) & 0xFFu;
	}

	constexpr unsigned char GetB() const noexcept
	{
		return m_Dword & 0xFFu;
	}

	void SetX(unsigned char _X) noexcept
	{
		m_Dword = (m_Dword & 0xFFFFFFu) | (_X << 24u);
	}

	void SetA(unsigned char _A) noexcept
	{
		this->SetX(_A);
	}

	void SetR(unsigned char _R) noexcept
	{
		m_Dword = (m_Dword & 0xFF00FFFFu) | (_R << 16u);
	}

	void SetG(unsigned char _G) noexcept
	{
		m_Dword = (m_Dword & 0xFFFF00FFu) | (_G << 8u);
	}

	void SetB(unsigned char _B) noexcept
	{
		m_Dword = (m_Dword & 0xFFFFFF00u) | _B;
	}
};

class _ENGINE_DLLEXP CSurface
{
public:
	CSurface(unsigned int _Width, unsigned int _Height) noexcept;
	CSurface(CSurface&& _Source) noexcept;
	CSurface(CSurface&) = delete;
	CSurface& operator=(CSurface&& _Donor) noexcept;
	CSurface& operator=(const CSurface&) = delete;
	~CSurface();

	void Clear(CColor _FillValue) noexcept;
	void PutPixel(unsigned int _X, unsigned int _Y, CColor _C) const noexcept;
	CColor GetPixel(unsigned int _X, unsigned int _Y) const noexcept;
	unsigned int GetWidth() const noexcept;
	unsigned int GetHeight() const noexcept;
	CColor* GetBufferPointer() noexcept;
	const CColor* GetBufferPointer() const noexcept;
	const CColor* GetBufferPointerConst() const noexcept;
	const wchar_t* GetFilename() const noexcept;
	bool HasAlpha() const noexcept;

	static CSurface FromFile(const std::wstring& _Filename);
	void WriteToFile(std::wstring_view _Filename) const;
	void Copy(const CSurface& _Src) noexcept;

private:
	CSurface(unsigned int _Width, unsigned int _Height, 
		std::unique_ptr<CColor[]> _BufferPtr, const wchar_t* _Filename, bool _HasAlpha) noexcept;

private:
	const wchar_t* m_Filename = nullptr;
	std::unique_ptr<CColor[]> m_Buffer;
	unsigned int m_Width;
	unsigned int m_Height;
	bool m_Alpha;
};

}
