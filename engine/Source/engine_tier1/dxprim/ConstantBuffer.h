/* -*- c++ -*- */
#pragma once

#include <engine_tier1/GraphicalOutput.h>
#include <engine_tier0/Exceptions.h>
#include <typeinfo>

namespace engine
{

template<class T>
class CBasicConstantBuffer : public CBase_Bind
{
public:
	CBasicConstantBuffer(CGraphicalOutput& _Gfx, const T& _Constants, UINT _Slot = 0)
		: m_Slot(_Slot)
	{
		D3D11_BUFFER_DESC bfd;
		bfd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bfd.Usage = D3D11_USAGE_DYNAMIC;
		bfd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bfd.MiscFlags = 0u;
		bfd.ByteWidth = sizeof(_Constants);
		bfd.StructureByteStride = 0u;

		D3D11_SUBRESOURCE_DATA csd = {};
		csd.pSysMem = &m_ConstBuffer;
		GetDevice(_Gfx)->CreateBuffer(&bfd, &csd, &m_ConstBuffer);
	}

	CBasicConstantBuffer(CGraphicalOutput& _Gfx, UINT _Slot = 0)
		: m_Slot(_Slot)
	{
		D3D11_BUFFER_DESC bfd;
		bfd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bfd.Usage = D3D11_USAGE_DYNAMIC;
		bfd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bfd.MiscFlags = 0u;
		bfd.ByteWidth = sizeof(T);
		bfd.StructureByteStride = 0u;
		ID3D11Device* devc = GetDevice(_Gfx);
		HRESULT hr;
		_ENGINE_MAYTHROW_GRAPHICS(devc->CreateBuffer(&bfd, nullptr, &m_ConstBuffer));
	}

	void Update(CGraphicalOutput& _Gfx, const T& _Constants)
	{
		D3D11_MAPPED_SUBRESOURCE msr;
		GetContext(_Gfx)->Map(
			m_ConstBuffer, 0u,
			D3D11_MAP_WRITE_DISCARD, 0u,
			&msr
		);
		memcpy(msr.pData, &_Constants, sizeof(_Constants));
		GetContext(_Gfx)->Unmap(m_ConstBuffer, 0u);
	}

	~CBasicConstantBuffer()
	{
		m_ConstBuffer->Release();
	}
protected:
	UINT m_Slot;
	ID3D11Buffer* m_ConstBuffer;
};

template<class T>
class CConstantVertexBuffer : public CBasicConstantBuffer<T>
{
	using CBase_Bind::GetContext;
	using CBasicConstantBuffer<T>::m_ConstBuffer;
	using CBasicConstantBuffer<T>::m_Slot;
public:
	using CBasicConstantBuffer<T>::CBasicConstantBuffer;
	void Bind(CGraphicalOutput& _Gfx) override
	{
		GetContext(_Gfx)->VSSetConstantBuffers(m_Slot, 1u, &m_ConstBuffer);
	}

	static std::string Discriminate() noexcept
	{
		return typeid(CConstantVertexBuffer).name();
	}

	static std::string Discriminate(const T& _Ignored, UINT _Ignored2 = 0) noexcept
	{
		return Discriminate();
	}

	std::string GenerateDiscriminator() const noexcept
	{
		return this->Discriminate();
	}
};

template<class T>
class CConstantPixelBuffer : public CBasicConstantBuffer<T>
{
	using CBasicConstantBuffer<T>::m_ConstBuffer;
	using CBasicConstantBuffer<T>::m_Slot;
	using CBase_Bind::GetContext;
public:
	using CBasicConstantBuffer<T>::CBasicConstantBuffer;
	void Bind(CGraphicalOutput& _Gfx) override
	{
		GetContext(_Gfx)->PSSetConstantBuffers(m_Slot, 1u, &m_ConstBuffer);
	}

	static std::string Discriminate() noexcept
	{
		return typeid(CConstantPixelBuffer).name();
	}

	static std::string Discriminate(const T& _Ignored, UINT _Ignored2 = 0u) noexcept
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
