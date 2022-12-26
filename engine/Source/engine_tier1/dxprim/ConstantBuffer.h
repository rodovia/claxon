// |=============================================================|
// |ConstantBuffer.h - Transmit simple data from CPU code to GPU |
// |=============================================================|
#pragma once

#include <engine_tier1/GraphicalOutput.h>
#include "dynamic_constant.h"
#include <engine_tier0/Exceptions.h>
#include <typeinfo>

namespace engine
{

class CBasicConstantBuffer : public CBase_Bind
{
public:
	CBasicConstantBuffer(CGraphicalOutput& _Gfx, const buffer::CBuffer& _Constants, UINT _Slot = 0)
		: m_Slot(_Slot),
		  m_Signature(_Constants.GetLayoutElement().GetSignature())
	{
		D3D11_BUFFER_DESC bfd;
		bfd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bfd.Usage = D3D11_USAGE_DYNAMIC;
		bfd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bfd.MiscFlags = 0u;
		bfd.ByteWidth = _Constants.SizeOf();
		bfd.StructureByteStride = 0u;
		
		D3D11_SUBRESOURCE_DATA csd = {};
		csd.pSysMem = _Constants.GetData();
		GetDevice(_Gfx)->CreateBuffer(&bfd, &csd, &m_ConstBuffer);
	}

	CBasicConstantBuffer(CGraphicalOutput& _Gfx, const buffer::CLayoutView& ly, UINT _Slot = 0)
		: m_Slot(_Slot)
	{
		D3D11_BUFFER_DESC bfd;
		bfd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bfd.Usage = D3D11_USAGE_DYNAMIC;
		bfd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bfd.MiscFlags = 0u;
		bfd.ByteWidth = ly.GetSizeBytes();
		bfd.StructureByteStride = 0u;
		ID3D11Device* devc = GetDevice(_Gfx);
		HRESULT hr;
		_ENGINE_MAYTHROW_GRAPHICS(devc->CreateBuffer(&bfd, nullptr, &m_ConstBuffer));
	}

	void Update(CGraphicalOutput& _Gfx, const buffer::CBuffer& _Constants)
	{
		D3D11_MAPPED_SUBRESOURCE msr;
		GetContext(_Gfx)->Map(
			m_ConstBuffer, 0u,
			D3D11_MAP_WRITE_DISCARD, 0u,
			&msr
		);
		memcpy(msr.pData, _Constants.GetData(), _Constants.SizeOf());
		GetContext(_Gfx)->Unmap(m_ConstBuffer, 0u);
	}

	~CBasicConstantBuffer()
	{
		m_ConstBuffer->Release();
	}
protected:
	UINT m_Slot;
	std::string m_Signature;
	ID3D11Buffer* m_ConstBuffer;
};

class CConstantVertexBuffer : public CBasicConstantBuffer
{
	using CBase_Bind::GetContext;
	using CBasicConstantBuffer::m_ConstBuffer;
	using CBasicConstantBuffer::m_Slot;
public:
	using CBasicConstantBuffer::CBasicConstantBuffer;
	void Bind(CGraphicalOutput& _Gfx) override
	{
		GetContext(_Gfx)->VSSetConstantBuffers(m_Slot, 1u, &m_ConstBuffer);
	}

	// LITTLE WARNING: Even though constant buffer classes have these
	// discrimination functions, THEY ARE NOT meant to be called anymore,
	// due to the advent of dynamic constant subsystem.
	static std::string Discriminate() noexcept
	{
		return typeid(CConstantVertexBuffer).name();
	}

	static std::string Discriminate(const buffer::CBuffer&, UINT _1 = 0) noexcept
	{
		return Discriminate();
	}

	std::string GenerateDiscriminator() const noexcept
	{
		return this->Discriminate();
	}
};

class CConstantPixelBuffer : public CBasicConstantBuffer
{
	using CBasicConstantBuffer::m_ConstBuffer;
	using CBasicConstantBuffer::m_Slot;
	using CBase_Bind::GetContext;
public:
	using CBasicConstantBuffer::CBasicConstantBuffer;
	void Bind(CGraphicalOutput& _Gfx) override
	{
		GetContext(_Gfx)->PSSetConstantBuffers(m_Slot, 1u, &m_ConstBuffer);
	}

	static std::string Discriminate() noexcept
	{
		return typeid(CConstantPixelBuffer).name();
	}

	static std::string Discriminate(const buffer::CBuffer&, UINT _1 = 0) noexcept
	{
		return Discriminate();
	}

	static std::string Discriminate(UINT _Ignored) noexcept
	{
		return Discriminate();
	}

	std::string GenerateDiscriminator() const noexcept
	{
		return this->Discriminate();
	}
};


}
