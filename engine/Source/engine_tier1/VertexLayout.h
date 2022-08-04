#pragma once

#include <vector>
#include <DirectXMath.h>
#include <type_traits>
#include <format>
#include <cstdio>

#include "GraphicalOutput.h"

#define _ENGINE_MAP_SPECIALIZ(T, SyT, St, S, M)                     \
	template <>                                                     \
	struct Map<T>                                                   \
	{                                                               \
		using SystemType = SyT;                                     \
		static constexpr DXGI_FORMAT DxgiFormat = DXGI_FORMAT_##St; \
		static constexpr const char* Semantic = #S##;               \
		static constexpr const char* Mangled = #M##;                \
	}

namespace engine
{

namespace layout
{

using BYTE = char;
using UBYTE = unsigned char;
using VERTEXSIZE = size_t;

struct BGRAElement
{
	unsigned char A, R, G, B;
};

// BUG: In some case-switches, when Position2D is matched,
// it uses the Map<Texture2D> SystemType! This is not a priority
// (Since it's kinda hard to Position2D change the system type)

enum ElementType
{
	Position2D,
	Position3D,
	Texture2D,
	Normal,
	Float3Color,
	Float4Color,
	BGRAColor,
	Tangent,
	Bitangent
};

template <ElementType>
struct Map;
_ENGINE_MAP_SPECIALIZ(Position2D, DirectX::XMFLOAT2, R32G32_FLOAT, Position, "ZA");
_ENGINE_MAP_SPECIALIZ(Position3D, DirectX::XMFLOAT3, R32G32B32_FLOAT, Position, "YA");
_ENGINE_MAP_SPECIALIZ(Texture2D, DirectX::XMFLOAT2, R32G32_FLOAT, TexCoord, "PP");
_ENGINE_MAP_SPECIALIZ(Normal, DirectX::XMFLOAT3, R32G32B32_FLOAT, Normal, "!O");
_ENGINE_MAP_SPECIALIZ(Float3Color, DirectX::XMFLOAT3, R32G32B32_FLOAT, Color, "A)");
_ENGINE_MAP_SPECIALIZ(Float4Color, DirectX::XMFLOAT4, R32G32B32A32_FLOAT, Color, "XA");
_ENGINE_MAP_SPECIALIZ(BGRAColor, BGRAElement, R8G8B8A8_UNORM, Color, "PL");
_ENGINE_MAP_SPECIALIZ(Tangent, DirectX::XMFLOAT3, R32G32B32_FLOAT, Tangent, "HA");
_ENGINE_MAP_SPECIALIZ(Bitangent, DirectX::XMFLOAT3, R32G32B32_FLOAT, Bitangent, "CS");


class _ENGINE_DLLEXP CElement
{
public:
	CElement(ElementType _Type, size_t _Offset)
		: m_Type(_Type),
		  m_Offset(_Offset){};

	size_t OffsetAfter() const noexcept;
	size_t Offset() const noexcept;
	size_t Size() const noexcept;
	static constexpr size_t SizeOf(ElementType _Type) noexcept;
	ElementType Type() const noexcept;
	D3D11_INPUT_ELEMENT_DESC D3DDescriptor() const noexcept;
	const char* Mangle() const noexcept;

private:
	template <ElementType _Ty_Type>
	static constexpr D3D11_INPUT_ELEMENT_DESC GenerateDescriptor(size_t _Offset)
	{
		using _Mt = Map<_Ty_Type>;
		D3D11_INPUT_ELEMENT_DESC desc = {_Mt::Semantic, 0, _Mt::DxgiFormat, 0, (UINT)_Offset, D3D11_INPUT_PER_VERTEX_DATA, 0};
		return desc;
	}

	ElementType m_Type;
	size_t m_Offset;
};

class CVertexLayout
{
public:
	template <ElementType _Ty_Type>
	const CElement& Query() const noexcept
	{
		for (auto& e : m_Elements)
		{
			if (e.Type() == _Ty_Type)
			{
				return e;
			}
		}

		assert("Could not query element type" && false);
		return m_Elements.front();
	}

	const CElement& QueryByIndex(size_t _Index) const noexcept;
	CVertexLayout& Append(ElementType _Type) noexcept;
	size_t Size() const noexcept;
	size_t ElementCount() const noexcept;
	std::vector<D3D11_INPUT_ELEMENT_DESC> D3DLayout() const noexcept;
	std::string Mangle() const noexcept;

private:
	std::vector<CElement> m_Elements;
};

class CVertex
{
	friend class CVertexBuffer;

public:
	template <ElementType _Ty_Type>
	auto& Attrib() noexcept
	{
		const CElement& elem = m_Layout.Query<_Ty_Type>();
		auto patt = m_Data + elem.Offset();
		return *reinterpret_cast<typename Map<_Ty_Type>::SystemType*>(patt);
	}

	template <class _Ty>
	void SetAttribByIndex(size_t _Index, _Ty&& _Value) noexcept
	{
		const auto& element = m_Layout.QueryByIndex(_Index);
		auto pat = m_Data + element.Offset();
		switch (element.Type())
		{
		case Position2D:
		case Texture2D:
			this->SetAttrib<Position2D>(pat, std::forward<_Ty>(_Value));
			break;
		case Position3D:
		case Normal:
		case Float3Color:
			this->SetAttrib<Position3D>(pat, std::forward<_Ty>(_Value));
			break;
		case Float4Color:
			this->SetAttrib<Float4Color>(pat, std::forward<_Ty>(_Value));
			break;
		case BGRAColor:
			this->SetAttrib<BGRAColor>(pat, std::forward<_Ty>(_Value));
			break;
		case Tangent:
			this->SetAttrib<Tangent>(pat, std::forward<_Ty>(_Value));
			break;
		case Bitangent:
			this->SetAttrib<Bitangent>(pat, std::forward<_Ty>(_Value));
			break;
		default:
			assert("Bad element type" && false);
		}
	}

private:
	CVertex(BYTE* _Data, const CVertexLayout& _Layout)
		: m_Data(_Data),
		  m_Layout(_Layout)
	{
		assert(_Data != nullptr);
	};

	template <class _Ty_First, class... _Ty_Rest>
	void SetAttribByIndex(size_t _Index, _Ty_First&& _First, _Ty_Rest&&... _Rest) noexcept
	{
		this->SetAttribByIndex(_Index, std::forward<_Ty_First>(_First));
		this->SetAttribByIndex(_Index + 1, std::forward<_Ty_Rest>(_Rest)...);
	}

	template <ElementType _Ty_Dst_, class _Ty_Src>
	void SetAttrib(BYTE* _Attrib, _Ty_Src&& _Value) noexcept
	{
		using _Ty_Dst = typename Map<_Ty_Dst_>::SystemType;

		if constexpr (std::is_assignable<_Ty_Dst, _Ty_Src>())
		{
			*(reinterpret_cast<_Ty_Dst*>(_Attrib)) = _Value;
		}
		else
		{
			assert("Parameter attrib type mismatch" && false);
		}
	}

private:
	BYTE* m_Data = nullptr;
	const CVertexLayout& m_Layout;
};

class CConstVertex
{
public:
	CConstVertex(const CVertex& _Vert) noexcept
		: m_Vertex(_Vert)
	{
	}

	template <ElementType _Ty_Type>
	const auto& Attrib() const noexcept
	{
		return const_cast<CVertex&>(m_Vertex).Attrib<_Ty_Type>();
	}

private:
	CVertex m_Vertex;
};

class CVertexBuffer
{
public:
	CVertexBuffer(CVertexLayout _Layout) noexcept
		: m_Layout(std::move(_Layout))
	{
	}

	const CVertexLayout& Layout() const noexcept;
	const BYTE* Data() const noexcept;
	VERTEXSIZE Size() const noexcept;
	size_t SizeBytes() const noexcept;

	template <class... _Ty_Params>
	void EmplaceBack(_Ty_Params&&... _Params) noexcept
	{
		assert(sizeof...(_Params) == m_Layout.ElementCount() && "Param count does not match layout's elem count!");
		m_Buffer.resize(m_Buffer.size() + m_Layout.Size());
		this->Back().SetAttribByIndex(0u, std::forward<_Ty_Params>(_Params)...);
	}

	CVertex Back() noexcept;
	CConstVertex Back() const noexcept;

	CVertex Front() noexcept;
	CConstVertex Front() const noexcept;

	CVertex operator[](size_t _Index) noexcept;
	CConstVertex operator[](size_t _Index) const noexcept;

private:
	std::vector<BYTE> m_Buffer;
	CVertexLayout m_Layout;
};


} // namespace layout

} // namespace engine