#include "VertexLayout.h"

size_t engine::layout::CElement::OffsetAfter() const noexcept
{
	return m_Offset + this->Size();
}

size_t engine::layout::CElement::Offset() const noexcept
{
	return m_Offset;
}

size_t engine::layout::CElement::Size() const noexcept
{
	return this->SizeOf(m_Type);
}

constexpr size_t engine::layout::CElement::SizeOf(ElementType _Type) noexcept
{
	switch (_Type)
	{
	case Position2D:
	case Texture2D:
		return sizeof(Map<Texture2D>::SystemType);
	case Normal:
	case Float3Color:
	case Float4Color:
	case Position3D:
		return sizeof(Map<Position3D>::SystemType);
	case Tangent:
	case Bitangent:
		return sizeof(Map<Tangent>::SystemType);
	case BGRAColor:
		return sizeof(unsigned int);
	}

	assert("Invalid element type" && false);
	return 0u;
}

const char* engine::layout::CElement::Mangle() const noexcept
{
	switch (m_Type)
	{
	case Position2D:
		return Map<Position2D>::Mangled;
	case Position3D:
		return Map<Position3D>::Mangled;
	case Texture2D:
		return Map<Texture2D>::Mangled;
	case Normal:
		return Map<Normal>::Mangled;
	case Float3Color:
		return Map<Float3Color>::Mangled;
	case Float4Color:
		return Map<Float4Color>::Mangled;
	case BGRAColor:
		return Map<BGRAColor>::Mangled;
	case Tangent:
		return Map<Tangent>::Mangled;
	case Bitangent:
		return Map<Bitangent>::Mangled;
	}

	assert("No mangle" && false);
	return "**";
}

engine::layout::ElementType engine::layout::CElement::Type() const noexcept
{
	return m_Type;
}

D3D11_INPUT_ELEMENT_DESC engine::layout::CElement::D3DDescriptor() const noexcept
{
	switch (m_Type)
	{
	case Position2D:
		return GenerateDescriptor<Position2D>(m_Offset);
	case Position3D:
		return GenerateDescriptor<Position3D>(m_Offset);
	case Texture2D:
		return GenerateDescriptor<Texture2D>(m_Offset);
	case Normal:
		return GenerateDescriptor<Normal>(m_Offset);
	case Float3Color:
		return GenerateDescriptor<Float3Color>(m_Offset);
	case Float4Color:
		return GenerateDescriptor<Float4Color>(m_Offset);
	case BGRAColor:
		return GenerateDescriptor<BGRAColor>(m_Offset);
	case Tangent:
		return GenerateDescriptor<Tangent>(m_Offset);
	case Bitangent:
		return GenerateDescriptor<Bitangent>(m_Offset);
	}

	assert("GetD3DDescriptor: Invalid type" && false);
	return { "_INVALID", 0, DXGI_FORMAT_UNKNOWN, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
}

// engine::layout::CElement ^^^^^ // engine::CVertexLayout vvvvv

const engine::layout::CElement& engine::layout::CVertexLayout::QueryByIndex(size_t _Index) const noexcept
{
	return m_Elements[_Index];
}

engine::layout::CVertexLayout& engine::layout::CVertexLayout::Append(ElementType _Type) noexcept
{
	m_Elements.emplace_back(_Type, this->Size());
	return *this;
}

size_t engine::layout::CVertexLayout::Size() const noexcept
{
	return m_Elements.empty() ? 0u : m_Elements.back().OffsetAfter();
}

std::string engine::layout::CVertexLayout::Mangle() const noexcept
{
	std::string man;
	for (const CElement& e : m_Elements)
	{
		man += e.Mangle();
	}
	return man;
}

size_t engine::layout::CVertexLayout::ElementCount() const noexcept
{
	return m_Elements.size();
}

std::vector<D3D11_INPUT_ELEMENT_DESC> engine::layout::CVertexLayout::D3DLayout() const noexcept
{
	std::vector<D3D11_INPUT_ELEMENT_DESC> desc;
	desc.reserve(this->ElementCount());
	for (const auto& e : m_Elements)
	{
		desc.push_back(e.D3DDescriptor());
	}

	return desc;
}

const engine::layout::CVertexLayout& engine::layout::CVertexBuffer::Layout() const noexcept
{
	return m_Layout;
}

const char* engine::layout::CVertexBuffer::Data() const noexcept
{
	return m_Buffer.data();
}

size_t engine::layout::CVertexBuffer::Size() const noexcept
{
	return m_Buffer.size() / m_Layout.Size();
}

size_t engine::layout::CVertexBuffer::SizeBytes() const noexcept
{
	return m_Buffer.size();
}

engine::layout::CVertex engine::layout::CVertexBuffer::Back() noexcept
{
	assert(m_Buffer.size() != 0u);
	return CVertex{ m_Buffer.data() + m_Buffer.size() - m_Layout.Size(), m_Layout };
}

engine::layout::CConstVertex engine::layout::CVertexBuffer::Back() const noexcept
{
	return const_cast<CVertexBuffer*>(this)->Back();
}

engine::layout::CVertex engine::layout::CVertexBuffer::Front() noexcept
{
	assert(m_Buffer.size() != 0u);
	return CVertex{ m_Buffer.data(), m_Layout };
}

engine::layout::CConstVertex engine::layout::CVertexBuffer::Front() const noexcept
{
	return const_cast<CVertexBuffer*>(this)->Front();
}

engine::layout::CVertex engine::layout::CVertexBuffer::operator[](size_t _Index) noexcept
{
		assert(_Index < this->Size());
		return CVertex{ m_Buffer.data() + m_Layout.Size() * _Index, m_Layout };
}

engine::layout::CConstVertex engine::layout::CVertexBuffer::operator[](size_t _Index) const noexcept
{
	return const_cast<CVertexBuffer&>(*this)[_Index];
}
