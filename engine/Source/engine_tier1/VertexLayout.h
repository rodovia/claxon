#pragma once

#include <vector>
#include <DirectXMath.h>
#include <type_traits>

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

enum ElementType
{
	Position2D,
	Position3D,
	Texture2D,
	Normal,
	Float3Color,
	Float4Color,
	BGRAColor
};

class CElement
{
public:
	CElement(ElementType _Type, size_t _Offset)
		: m_Type(_Type),
		  m_Offset(_Offset)
	{};

	size_t OffsetAfter() const noexcept
	{
		return m_Offset + this->Size();
	}

	size_t Offset() const noexcept
	{
		return m_Offset;
	}

	size_t Size() const noexcept
	{
		return this->SizeOf(m_Type);
	}

	static constexpr size_t SizeOf(ElementType _Type) noexcept
	{
		switch (_Type)
		{
		case Position2D:
		case Texture2D:
			return sizeof(DirectX::XMFLOAT2);
		case Normal:
		case Float3Color:
		case Float4Color:
		case Position3D:
			return sizeof(DirectX::XMFLOAT3);
		case BGRAColor:
			return sizeof(unsigned int);
		}

		assert("Invalid element type" && false);
		return 0u;
	}

	ElementType Type() const noexcept
	{
		return m_Type;
	}
private:
	ElementType m_Type;
	size_t m_Offset;
};

class CVertexLayout
{
public:
	template<ElementType _Ty_Type>
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

	const CElement& QueryByIndex(size_t _Index) const noexcept
	{
		return m_Elements[_Index];
	}

	template<ElementType _Ty_Type>
	CVertexLayout& Append() noexcept
	{
		m_Elements.emplace_back(_Ty_Type, this->Size());
	}

	size_t Size() const noexcept
	{
		return m_Elements.empty() ? 0u : m_Elements.back().OffsetAfter();
	}
private:
	std::vector<CElement> m_Elements;
};

class CVertex
{
	friend class CVertexBuffer;
public:
	template<ElementType _Ty_Type>
	auto& Attrib() noexcept
	{
		const CElement& elem = m_Layout.Query<_Ty_Type>();
		auto patt = m_Data + elem.Offset();
		if constexpr 
			(_Ty_Type == Position2D ||
			_Ty_Type == Texture2D)
		{
			return *reinterpret_cast<DirectX::XMFLOAT2*>(patt);
		}
		else if constexpr 
			(_Ty_Type == Position3D ||
				_Ty_Type == Normal  ||
			_Ty_Type == Float3Color ||
			_Ty_Type == Float4Color)
		{
			return *reinterpret_cast<DirectX::XMFLOAT3*>(patt);
		}
		else if constexpr
			(_Ty_Type == BGRAColor)
		{
			return *reinterpret_cast<BGRAElement*>(patt);
		}
	}

	template<class _Ty>
	void SetAttribByIndex(size_t _Index, _Ty&& _Value) noexcept
	{
		const auto& element = m_Layout.QueryByIndex(_Index);
		auto pat = m_Data + element.Offset();
		switch (element.Type())
		{
			case Position2D;
			case Texture2D:
				this->SetAttrib<DirectX::XMFLOAT2>(pat, std::forward<_Ty>(_Value));
				break;
			case Position3D:
			case Normal:
			case Float3Color:
				this->SetAttrib<DirectX::XMFLOAT3>(pat, std::forward<_Ty>(_Value));
			case Float4Color:
				this->SetAttrib<DirectX::XMFLOAT4>(pat, std::forward<_Ty>(_Value));
				break;
			case BGRAColor:
				this->SetAttrib<BGRAElement>(pat, std::forward<_Ty>(_Value));
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

	template<class _Ty_First, class ..._Ty_Rest>
	void SetAttribByIndex(size_t _Index, _Ty_First&& _First, _Ty_Rest&&... _Rest) noexcept
	{
		this->SetAttribByIndex(_Index, std::forward<_Ty_First>(_First));
		this->SetAttribByIndex(_Index, std::forward<_Ty_Rest>(_Rest));
	}

	template<class _Ty_Dst, typename _Ty_Src>
	void SetAttrib(BYTE* _Attrib, _Ty_Src&& _Value) noexcept
	{
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

class CVertexBuffer
{
public:
	CVertexBuffer(CVertexLayout _Layout) noexcept
		: m_Layout(std::move(_Layout))
	{}

	const CVertexLayout& Layout() const noexcept
	{
		return m_Layout;
	}

	VERTEXSIZE Size() noexcept
	{
		return m_Buffer.size() / m_Layout.Size();
	}

	template<class ..._Ty_Params>
	void EmplaceBack(_Ty_Params&&... _Params) noexcept
	{
		m_Buffer.resize(m_Buffer.size() + m_Layout.Size());
		this->Back().SetAttributeByIndex(0u, std::forward<_Ty_Params>(_Params)...);
	}

	CVertex Back() noexcept
	{
		assert(m_Buffer.size() != 0u);
		return CVertex{ m_Buffer.data() + m_Buffer.size() - m_Layout.Size(), m_Layout };
	}

	CVertex Front() noexcept
	{
		assert(m_Buffer.size() != 0u);
		return CVertex{m_Buffer.data(), m_Layout };
	}

	CVertex operator[](size_t _Index) noexcept
	{
		assert(_Index < this->Size());
		return CVertex{ m_Buffer.data() + m_Layout.Size() * _Index, m_Layout };
	}
private:
	std::vector<BYTE> m_Buffer;
	CVertexLayout m_Layout;
};

} // namespace layout

} // namespace engine