#include <algorithm>
namespace Gdiplus
{
	using std::min;
	using std::max;
}

#include <assert.h>
#include <sstream>
#include "Surface.h"

#include <tier0lib/String0.h>

#define FULL_WINTARD
#include <tier0lib/Win32.h>
#include <gdiplus.h>

#include <engine_tier0/Exceptions.h>

static void GetEncoderCLSID(const WCHAR* _Format, CLSID* _Clsid)
{
	UINT num = 0;			// number of img encoders
	UINT size = 0;			// size of image encoder array in bytes

	Gdiplus::ImageCodecInfo* ici = nullptr;
	Gdiplus::GetImageEncodersSize(&num, &size);

	ici = (Gdiplus::ImageCodecInfo*)(malloc(size));

	GetImageEncoders(num, size, ici);
	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(ici[j].MimeType, _Format) == 0)
		{
			(*_Clsid) = ici[j].Clsid;
			free(ici);
			return;
		}
	}

	free(ici);
}

engine::CSurface::CSurface(unsigned int _Width, unsigned int _Height) noexcept
	: m_Buffer(std::make_unique<engine::CColor[]>(_Width * _Height)),
	  m_Width(_Width),
	  m_Height(_Height)
{}

engine::CSurface::CSurface(engine::CSurface&& _Source) noexcept
	: m_Buffer(std::move(_Source.m_Buffer)),
	  m_Width(_Source.m_Width),
	  m_Height(_Source.m_Height)
{}

engine::CSurface& engine::CSurface::operator=(engine::CSurface&& _Donor) noexcept
{
	m_Width = _Donor.m_Width;
	m_Height = _Donor.m_Height;
	m_Buffer = std::move(_Donor.m_Buffer);
	_Donor.m_Buffer = nullptr;
	return *this;
}

engine::CSurface::~CSurface()
{}

void engine::CSurface::Clear(engine::CColor _FillValue) noexcept
{
	memset(m_Buffer.get(), _FillValue.m_Dword, m_Width * m_Height);
}

void engine::CSurface::PutPixel(unsigned int _X, unsigned int _Y, CColor _C) const noexcept
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
	return m_Buffer.get();
}

const engine::CColor* engine::CSurface::GetBufferPointer() const noexcept
{
	return m_Buffer.get();
}

const engine::CColor* engine::CSurface::GetBufferPointerConst() const noexcept
{
	return m_Buffer.get();
}

engine::CSurface engine::CSurface::FromFile(const std::wstring& _FileName)
{
	using namespace std::string_literals;
	unsigned int width = 0;
	unsigned int height = 0;
	std::unique_ptr<engine::CColor[]> buffer;

	Gdiplus::Bitmap bitm(_FileName.c_str());
	if (bitm.GetLastStatus() != Gdiplus::Status::Ok)
	{
		std::string mb = tier0::ConvertToMultiByteString(_FileName);
		throw engine::CEngineError("CSurface: cannot open file '"s + mb + "'. "s + std::to_string(bitm.GetLastStatus()));
	}

	width = bitm.GetWidth();
	height = bitm.GetHeight();
	buffer = std::make_unique<engine::CColor[]>(width * height);

	for (unsigned int y = 0; y < height; y++)
	{
		for (unsigned int x = 0; x < width; x++)
		{
			Gdiplus::Color c;
			bitm.GetPixel(x, y, &c);
			buffer[y * width + x] = c.GetValue();
		}
	}

	return CSurface(width, height, std::move(buffer), _FileName.c_str());
}

void engine::CSurface::WriteToFile(std::wstring_view _Filename) const
{
	CLSID bmpid;
	GetEncoderCLSID(L"image/bmp", &bmpid);

	Gdiplus::Bitmap bitmap(m_Width, m_Height, m_Width * sizeof(CColor), 
		PixelFormat32bppARGB, (BYTE*) m_Buffer.get());

	bitmap.Save(_Filename.data(), &bmpid);
}

void engine::CSurface::Copy(const CSurface& _Src) noexcept
{
	assert(m_Width == _Src.m_Width);
	assert(m_Height == _Src.m_Height);
	memcpy(m_Buffer.get(), _Src.m_Buffer.get(), m_Width * m_Height * sizeof(engine::CColor));
}

const wchar_t* engine::CSurface::GetFilename() const noexcept
{
	return m_Filename;
}

engine::CSurface::CSurface(unsigned int _Width, unsigned int _Height, 
	std::unique_ptr<CColor[]> _BufferPtr, const wchar_t* _Filename) noexcept
	: m_Height(_Height),
	  m_Width(_Width),
	  m_Buffer(std::move(_BufferPtr)),
	m_Filename(_Filename)
{

}
