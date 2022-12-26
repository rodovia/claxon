// |===============================================================================|
// |dynamic_constant.cpp - The implementation of the classes and structs defined in|
// |					   DynamicConstantBuffer.h								   |
// |===============================================================================|

#include "dynamic_constant.h"
#include <engine_tier0/Exceptions.h>
#include <vector>
#include <sstream>

namespace engine
{

namespace buffer
{

struct ExtraData
{
	struct Struct : public CLayoutElement::ExtraDataBase
	{
		std::vector<std::pair<std::string, CLayoutElement>> Elements;
	};

	struct Array : public CLayoutElement::ExtraDataBase
	{
		std::optional<CLayoutElement> Elements;
		size_t Size;
	};
};

} // namespace buffer

#pragma region CLayoutElement

std::string buffer::CLayoutElement::GetSignature() const noexcept
{
	switch (m_Type)
	{
#define K(_E) \
	case _E:  \
		return buffer::AttribMap<_E>::Mangled;
		__hl2_LEAF_ELEMENT_TYPES
#undef K

			case Struct : return this->GetSignatureStruct(); // clang-format: keep
	case Array:
		return this->GetSignatureArray();
	default:
		engine::FastFail("CLayoutElement::GetSignature: invalid type");
		return "?";
	}
}

buffer::CLayoutElement::CLayoutElement(LeafType _Type)
	: m_Type(_Type)
{
	if (_Type == Array)
	{
		m_ExtraData = std::make_unique<ExtraData::Array>();
	}

	if (_Type == Struct)
	{
		m_ExtraData = std::make_unique<ExtraData::Struct>();
	}
}

std::pair<size_t, const buffer::CLayoutElement*>
buffer::CLayoutElement::CalculateIndexingOffset(size_t offset, size_t index) const noexcept
{
	assert(m_Type == Array);
	const ExtraData::Array& data = static_cast<ExtraData::Array&>(*m_ExtraData);
	assert(index < data.Size);
	return {offset + data.Elements->GetSizeInBytes() * index, &*data.Elements};
}

buffer::CLayoutElement& buffer::CLayoutElement::operator[](const std::string& _Key) noexcept
{
	assert(m_Type == Struct);
	for (auto& mem : static_cast<ExtraData::Struct&>(*m_ExtraData).Elements)
	{
		if (mem.first == _Key)
		{
			return mem.second;
		}
	}

	return this->GetEmptyElement();
}

const buffer::CLayoutElement& buffer::CLayoutElement::operator[](const std::string& _Key) const noexcept
{
	return const_cast<CLayoutElement&>(*this)[_Key];
}

buffer::CLayoutElement& buffer::CLayoutElement::GetTypeOfArray() noexcept
{
	assert(m_Type == Array);
	return *static_cast<ExtraData::Array&>(*m_ExtraData).Elements;
}

size_t buffer::CLayoutElement::GetSizeInBytes() const noexcept
{
	size_t begin, end;
	this->GetOffsetLimits(&begin, &end);
	return end - begin;
}

void buffer::CLayoutElement::GetOffsetLimits(size_t* _Out_Begin, size_t* _Out_End) const noexcept
{
	if (_Out_Begin != nullptr)
	{
		(*_Out_Begin) = m_Offset;
	}

	if (_Out_End != nullptr)
	{
		switch (m_Type)
		{
#define K(_E)                                               \
	case _E:                                                \
		(*_Out_End) = m_Offset + AttribMap<_E>::ShaderSize; \
		break;
			__hl2_LEAF_ELEMENT_TYPES
#undef K

				case Struct: // clang-format: keep
			{
				const auto& data = static_cast<ExtraData::Struct&>(*m_ExtraData);
				data.Elements.back().second.GetOffsetLimits(nullptr, _Out_End);
				break;
			}

		case Array:
		{
			const auto& data = static_cast<ExtraData::Array&>(*m_ExtraData);
			(*_Out_End) = data.Elements->GetSizeInBytes() * data.Size;
			break;
		}
		default:
			__assume(0);
		}
	}
}

buffer::CLayoutElement& buffer::CLayoutElement::GetEmptyElement() const noexcept
{
	static buffer::CLayoutElement elem;
	return elem;
}

buffer::CLayoutElement& buffer::CLayoutElement::Add(LeafType _Type, std::string _Name) noexcept
{
	assert(_Type == Struct);
	assert(this->ValidateSymbol(_Name));

	ExtraData::Struct& strct = static_cast<ExtraData::Struct&>(*m_ExtraData);
	for (auto& mem : strct.Elements)
	{
		if (mem.first == _Name)
		{
			engine::FastFail("Cannot add duplicate name to layout elem");
		}
	}
	strct.Elements.emplace_back(std::move(_Name), CLayoutElement{_Type});
	return *this;
}

buffer::CLayoutElement& buffer::CLayoutElement::Set(LeafType _Type, size_t _Index) noexcept
{
	assert(_Type == Array);
	assert(_Index != 0);

	auto& ard = static_cast<ExtraData::Array&>(*m_ExtraData);
	ard.Elements = {_Type};
	ard.Size = _Index;
	return *this;
}

size_t buffer::CLayoutElement::AdvanceIfCrossesBoundary(size_t offset, size_t index) noexcept
{
	if (this->CrossesBoundary(offset, index))
	{
		return this->AdvanceToBoundary(offset);
	}

	return 0u;
}

size_t buffer::CLayoutElement::Finalize(size_t _Offset) noexcept
{
	switch (m_Type)
	{
#define K(_E)                                                                          \
	case _E:                                                                           \
		m_Offset = this->AdvanceIfCrossesBoundary(_Offset, AttribMap<_E>::ShaderSize); \
		return m_Offset + AttribMap<_E>::ShaderSize;
		__hl2_LEAF_ELEMENT_TYPES
#undef K
			case Struct : return this->FinalizeStruct(_Offset);
	case Array:
		return this->FinalizeStruct(_Offset);
	default:
		assert("Bad type in size compile step" && false);
		return 0u;
	}
}

size_t buffer::CLayoutElement::FinalizeStruct(size_t offset) noexcept
{
	ExtraData::Struct& str = static_cast<ExtraData::Struct&>(*m_ExtraData);
	assert(str.Elements.size() != 0u);
	m_Offset = this->AdvanceToBoundary(offset);

	size_t offc = m_Offset;
	for (auto& el : str.Elements)
	{
		offc = el.second.Finalize(offc);
	}
	return offc;
}

size_t buffer::CLayoutElement::FinalizeArray(size_t offset) noexcept
{
	ExtraData::Array& ar = static_cast<ExtraData::Array&>(*m_ExtraData);
	assert(ar.Size != 0u);
	size_t ofe;
	m_Offset = this->AdvanceToBoundary(m_Offset);
	ar.Elements->Finalize(m_Offset);

	this->GetOffsetLimits(nullptr, &ofe);
	return ofe;
}

size_t buffer::CLayoutElement::AdvanceToBoundary(size_t offset) noexcept
{
	return offset + (16u - offset % 16u) % 16u;
}

bool buffer::CLayoutElement::Exists() const noexcept
{
	return m_Type != Empty;
}

bool buffer::CLayoutElement::CrossesBoundary(size_t offset, size_t size) noexcept
{
	const size_t end = offset + size;
	const size_t pageStart = offset / 16u;
	const size_t pageEnd = end / 16u;
	return (pageStart != pageEnd && pageEnd % 16u != 0u) || size > 16u;
}

std::string buffer::CLayoutElement::GetSignatureStruct() const noexcept
{
	assert(m_Type == Struct);
	const ExtraData::Struct& str = static_cast<ExtraData::Struct&>(*m_ExtraData);

	std::stringstream st;
	st << "Struct{";
	for (const auto& elem : str.Elements)
	{
		st << elem.second.GetSignature();
	}
	st << "}";
	return st.str();
}

std::string buffer::CLayoutElement::GetSignatureArray() const noexcept
{
	assert(m_Type == Array);
	const ExtraData::Array& arr = static_cast<ExtraData::Array&>(*m_ExtraData);
	std::stringstream st;
	st << "Array{" << arr.Elements->GetSignature() << "}";
	return st.str();
}

bool buffer::CLayoutElement::ValidateSymbol(const std::string& name) noexcept
{
	// Rules:
	// names must be made up of letters, numbers, or underscores
	// the first character or the only must not be a number
	return !name.empty() && !std::isdigit(name.front()) && std::all_of(name.begin(), name.end(), [](char c)
																	   { return std::isalnum(c) || c == '_'; });
}

#pragma endregion

#pragma region CLayout

buffer::CLayout::CLayout(std::shared_ptr<buffer::CLayoutElement> root)
	: m_Root(root)
{
}

size_t buffer::CLayout::GetSizeBytes() const noexcept
{
	return m_Root->GetSizeInBytes();
}

std::string buffer::CLayout::GetSignature() const noexcept
{
	return m_Root->GetSignature();
}

#pragma endregion

#pragma region CRawLayout

buffer::CRawLayout::CRawLayout() noexcept
	: CLayout(std::shared_ptr<CLayoutElement>(new CLayoutElement(Struct)))
{
}

buffer::CLayoutElement& buffer::CRawLayout::operator[](const std::string& key) noexcept
{
	return (*m_Root)[key];
}

std::shared_ptr<buffer::CLayoutElement> buffer::CRawLayout::DeliverRoot() noexcept
{
	std::shared_ptr<buffer::CLayoutElement> mov = std::move(m_Root);
	mov->Finalize(0);
	(*this) = CRawLayout();
	return std::move(mov);
}

void buffer::CRawLayout::ClearRoot() noexcept
{
	(*this) = CRawLayout();
}

#pragma endregion

#pragma region CLayoutView

buffer::CLayoutView::CLayoutView(std::shared_ptr<CLayoutElement> root) noexcept
	: CLayout(std::move(root))
{
}

buffer::CLayoutView::CLayoutView(CRawLayout root) noexcept
	: CLayout(root.DeliverRoot())
{
}

std::shared_ptr<buffer::CLayoutElement> buffer::CLayoutView::MoveRoot() const
{
	return std::move(m_Root);
}

const buffer::CLayoutElement&
buffer::CLayoutView::operator[](const std::string& key) const noexcept
{
	return (*m_Root)[key];
}

std::shared_ptr<buffer::CLayoutElement> buffer::CLayoutView::ShareRoot() const noexcept
{
	return m_Root;
}

#pragma endregion

#pragma region CPtr and CPtrView

buffer::CPtr::CPtr(CElementRef* ref) noexcept
	: m_Ref(ref)
{
}

buffer::CPtrView::CPtrView(CElementRefView* ref) noexcept
	: m_Ref(ref)
{
}

#pragma endregion

#pragma region CElementRef

buffer::CElementRef::operator buffer::CElementRefView() const noexcept
{
	return {m_Layout, m_Bytes, m_Offset};
}

bool buffer::CElementRef::Exists() const noexcept
{
	return m_Layout->Exists();
}

buffer::CElementRef buffer::CElementRef::operator[](const std::string& key) const noexcept
{
	return {&(*m_Layout)[key], m_Bytes, m_Offset};
}

buffer::CElementRef buffer::CElementRef::operator[](size_t index) const noexcept
{
	const auto idata = m_Layout->CalculateIndexingOffset(m_Offset, index);
	return {idata.second, m_Bytes, idata.first};
}

buffer::CPtr buffer::CElementRef::operator&() const noexcept
{
	return CPtr{const_cast<CElementRef*>(this)};
}

buffer::CElementRef::CElementRef(const CLayoutElement* layout, char* bytes, size_t offset) noexcept
	: m_Layout(layout),
	  m_Bytes(bytes),
	  m_Offset(offset)
{
}

#pragma endregion

#pragma region CElementRefView

bool buffer::CElementRefView::Exists() const noexcept
{
	return m_Layout->Exists();
}

buffer::CElementRefView buffer::CElementRefView::operator[](const std::string& key) const noexcept
{
	return {&(*m_Layout)[key], m_Bytes, m_Offset};
}

buffer::CElementRefView buffer::CElementRefView::operator[](size_t index) const noexcept
{
	const auto idata = m_Layout->CalculateIndexingOffset(m_Offset, index);
	return {idata.second, m_Bytes, idata.first};
}

buffer::CPtrView buffer::CElementRefView::operator&() const noexcept
{
	return CPtrView{ const_cast<buffer::CElementRefView*>(this) };
}

buffer::CElementRefView::CElementRefView(const CLayoutElement* layout, const char* bytes, size_t offset) noexcept
	: m_Layout(layout),
	  m_Bytes(bytes),
	  m_Offset(offset)
{
}

#pragma endregion

#pragma region CBuffer

buffer::CBuffer::CBuffer(CRawLayout&& _Ly) noexcept
	: m_Layout(_Ly.DeliverRoot())
{
}

buffer::CBuffer::CBuffer(const CLayoutView& _Ly) noexcept
	: m_Layout(_Ly.ShareRoot())
{
	size_t ofe;
	m_Layout->GetOffsetLimits(nullptr, &ofe);
	m_Bytes = std::vector<char>(ofe);
}

buffer::CBuffer::CBuffer(CLayoutView&& _Ly) noexcept
	: m_Layout(_Ly.MoveRoot())
{
}

buffer::CBuffer::CBuffer(const CBuffer& _Ly) noexcept
	: m_Layout(_Ly.m_Layout),
	  m_Bytes(_Ly.m_Bytes)
{
}

buffer::CBuffer::CBuffer(CBuffer&& _Ly) noexcept
	: m_Layout(std::move(_Ly.m_Layout)),
	  m_Bytes(std::move(_Ly.m_Bytes))
{
}

buffer::CElementRef buffer::CBuffer::operator[](const std::string& _Key) noexcept 
{
	return {&(*m_Layout)[_Key], m_Bytes.data(), 0u};
}

buffer::CElementRefView buffer::CBuffer::operator[](const std::string& _Key) const noexcept
{
	return const_cast<buffer::CBuffer&>(*this)[_Key];
}

const char* buffer::CBuffer::GetData() const noexcept
{
	return m_Bytes.data();
}

size_t buffer::CBuffer::SizeOf() const noexcept
{
	return m_Bytes.size();
}

const buffer::CLayoutElement& buffer::CBuffer::GetLayoutElement() const noexcept
{
	return *m_Layout;
}

void buffer::CBuffer::CopyFrom(const CBuffer& other) noexcept
{
	assert(&this->GetLayoutElement() == &other.GetLayoutElement());
	std::copy(other.m_Bytes.begin(), other.m_Bytes.end(), m_Bytes.begin());
}

std::shared_ptr<buffer::CLayoutElement> buffer::CBuffer::ShareLayoutRoot() const noexcept
{
	return m_Layout;
}

#pragma endregion

} // namespace engine
